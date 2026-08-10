// Classe principal que coordena o assistente de IA.
// Gerencia modelo, ferramentas e sessões. Possui destrutor explícito
// que libera as sessões criadas (composição).

#ifndef AI_ASSISTANT_HPP
#define AI_ASSISTANT_HPP

#include <iostream>
#include <memory>
#include <optional>
#include <ranges>
#include <string>
#include <variant>
#include <vector>

#include "billable.hpp"
#include "catalog.hpp"
#include "exceptions.hpp"
#include "model.hpp"
#include "session.hpp"
#include "tool.hpp"
#include "user.hpp"

class AiAssistant {
private:
    std::string name_;              // nome do assistente
    std::shared_ptr<Model> model_;                  // modelo de linguagem utilizado (agregação via shared_ptr)
    std::vector<std::shared_ptr<Tool>> tools_;      // ferramentas disponíveis (agregação via shared_ptr)
    std::vector<std::unique_ptr<Session>> sessions_; // sessões gerenciadas (composição via unique_ptr)
    Catalog<Model> available_models_;               // modelos cadastrados, pesquisáveis por nome

public:
    // Resultado da execução de uma ferramenta: dados de sucesso ou mensagem de erro
    struct ToolOutput {
        std::string tool_name;
        std::string content;
        double cost;
    };
    using ExecutionOutcome = std::variant<ToolOutput, std::string>;

    // Construtor com lista de inicialização
    AiAssistant(const std::string& name)
        : name_(name), model_(nullptr), tools_(), sessions_() {
        std::cout << "AiAssistant(\"" << name_ << "\") criado" << std::endl;
    }

    // Destrutor explícito — unique_ptr libera as sessões automaticamente (composição)
    ~AiAssistant() {
        sessions_.clear(); // unique_ptr destrói cada Session aqui
        std::cout << "~AiAssistant(\"" << name_ << "\") destruido" << std::endl;
    }

    // Getters const
    std::string get_name() const { return name_; }
    std::shared_ptr<Model> get_model() const { return model_; }
    const std::vector<std::shared_ptr<Tool>>& get_tools() const { return tools_; }

    // Define o modelo de linguagem a ser utilizado
    void set_model(std::shared_ptr<Model> model) {
        model_ = model;
    }

    // Adiciona uma ferramenta ao assistente
    void add_tool(std::shared_ptr<Tool> tool) {
        tools_.push_back(tool);
    }

    // Cria uma nova sessão para o usuário e a armazena internamente.
    // Retorna referência observadora — o assistente permanece dono (unique_ptr).
    Session& create_session(User& user) {
        int new_id = static_cast<int>(sessions_.size()) + 1;
        auto session = std::make_unique<Session>(new_id, user);
        Session& ref = *session;
        sessions_.push_back(std::move(session));
        return ref;
    }

    // Lista todas as ferramentas registradas e seus estados
    std::string list_tools() const {
        if (tools_.empty()) {
            return "Nenhuma ferramenta registrada.";
        }
        std::string result = "Ferramentas disponiveis:\n";
        for (size_t i = 0; i < tools_.size(); ++i) {
            result += "  " + std::to_string(i + 1) + ". " + tools_[i]->get_name();
            result += (tools_[i]->is_enabled() ? " [ativa]" : " [inativa]");
            result += " - " + tools_[i]->get_description();
            if (i < tools_.size() - 1) {
                result += "\n";
            }
        }
        return result;
    }

    // Retorna os nomes das ferramentas ativas com custo por chamada até
    // 'max_cost', usando um pipeline de ranges (C++20) com dois adaptadores
    // encadeados: filter (ativas e dentro do orçamento) + transform (nome).
    // Ver README ("Programação Genérica") para o contraste com list_tools().
    std::vector<std::string> affordable_tool_names(double max_cost) const {
        auto names = tools_
            | std::views::filter([max_cost](const std::shared_ptr<Tool>& tool) {
                  return tool->is_enabled() && tool->cost_per_call() <= max_cost;
              })
            | std::views::transform([](const std::shared_ptr<Tool>& tool) {
                  return tool->get_name();
              });
        return std::vector<std::string>(names.begin(), names.end());
    }

    // Cadastra um modelo no catálogo de modelos disponíveis para seleção
    void register_available_model(const Model& model) {
        available_models_.add(model);
    }

    // Busca um modelo disponível pelo nome — retorna nullopt se não achar,
    // em vez de lançar exceção ou devolver ponteiro nulo (TP3-Q2-B).
    std::optional<Model> find_available_model(const std::string& name) const {
        return available_models_.find_by_name(name);
    }

    // Seleciona o modelo ativo do assistente pelo nome. Lança
    // ModelNotFoundError (derivada de LlmServiceError) se o modelo não
    // estiver cadastrado no catálogo (TP3-Q2-A).
    void select_model(const std::string& name) {
        auto found = find_available_model(name);
        if (!found.has_value()) {
            throw ModelNotFoundError(name);
        }
        model_ = std::make_shared<Model>(*found);
    }

    // Verifica se o custo total já faturado (modelo + ferramentas
    // faturáveis) ultrapassa o limite informado. Lança BudgetExceededError
    // (derivada de LlmServiceError) em caso de estouro (TP3-Q2-A).
    void enforce_budget(double limit) const {
        double spent = 0.0;
        if (model_ != nullptr) {
            spent += model_->billed_cost();
        }
        for (const auto& tool : tools_) {
            const auto* billable = dynamic_cast<const Billable*>(tool.get());
            if (billable != nullptr) {
                spent += billable->billed_cost();
            }
        }
        if (spent > limit) {
            throw BudgetExceededError(spent, limit);
        }
    }

    // Executa a ferramenta de nome 'name' com o input informado. Retorna um
    // variant: ToolOutput em caso de sucesso, ou uma mensagem de erro em
    // std::string caso a ferramenta não exista (TP3-Q2-C).
    ExecutionOutcome run_tool(const std::string& name, const std::string& input) const {
        for (const auto& tool : tools_) {
            if (tool->get_name() == name) {
                return ToolOutput{tool->get_name(), tool->execute(input), tool->cost_per_call()};
            }
        }
        return std::string("ferramenta nao encontrada: " + name);
    }

    // Gera um relatório completo do estado do assistente
    std::string status_report() const {
        std::string report = "=== Relatorio do Assistente ===\n";
        report += "Nome: " + name_ + "\n";

        if (model_ != nullptr) {
            report += "Modelo: " + model_->get_name()
                    + " (max " + std::to_string(model_->get_max_tokens()) + " tokens)\n";
        } else {
            report += "Modelo: nenhum configurado\n";
        }

        report += "Ferramentas: " + std::to_string(tools_.size()) + "\n";
        report += "Sessoes ativas: " + std::to_string(sessions_.size());
        return report;
    }
};

#endif // AI_ASSISTANT_HPP
