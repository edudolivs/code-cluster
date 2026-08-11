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

class ai_assistant {
private:
    std::string name_;              // nome do assistente
    std::shared_ptr<model> model_;                  // modelo de linguagem utilizado (agregação via shared_ptr)
    std::vector<std::shared_ptr<tool>> tools_;      // ferramentas disponíveis (agregação via shared_ptr)
    std::vector<std::unique_ptr<session>> sessions_; // sessões gerenciadas (composição via unique_ptr)
    catalog<model> available_models_;               // modelos cadastrados, pesquisáveis por nome

public:
    // Resultado da execução de uma ferramenta: dados de sucesso ou mensagem de erro
    struct tool_output {
        std::string tool_name;
        std::string content;
        double cost;
    };
    using execution_outcome = std::variant<tool_output, std::string>;

    // Construtor com lista de inicialização
    ai_assistant(const std::string& name)
        : name_(name), model_(nullptr), tools_(), sessions_() {
        std::cout << "ai_assistant(\"" << name_ << "\") criado" << std::endl;
    }

    // Destrutor explícito — unique_ptr libera as sessões automaticamente (composição)
    ~ai_assistant() {
        sessions_.clear(); // unique_ptr destrói cada session aqui
        std::cout << "~ai_assistant(\"" << name_ << "\") destruido" << std::endl;
    }

    // Getters const
    std::string get_name() const { return name_; }
    std::shared_ptr<model> get_model() const { return model_; }
    const std::vector<std::shared_ptr<tool>>& get_tools() const { return tools_; }

    // Define o modelo de linguagem a ser utilizado
    void set_model(std::shared_ptr<model> current_model) {
        model_ = current_model;
    }

    // Adiciona uma ferramenta ao assistente
    void add_tool(std::shared_ptr<tool> current_tool) {
        tools_.push_back(current_tool);
    }

    // Remove a ferramenta na posicao 'index'. Retorna false se o indice nao
    // existir (TP3-Q6-A: operacao exposta pela GUI).
    bool remove_tool_at(std::size_t index) {
        if (index >= tools_.size()) {
            return false;
        }
        tools_.erase(tools_.begin() + static_cast<std::ptrdiff_t>(index));
        return true;
    }

    // Descarta todas as ferramentas — usado ao recarregar o estado do disco.
    void clear_tools() {
        tools_.clear();
    }

    // Cria uma nova sessão para o usuário e a armazena internamente.
    // Retorna referência observadora — o assistente permanece dono (unique_ptr).
    session& create_session(user& current_user) {
        int new_id = static_cast<int>(sessions_.size()) + 1;
        auto new_session = std::make_unique<session>(new_id, current_user);
        session& ref = *new_session;
        sessions_.push_back(std::move(new_session));
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
            | std::views::filter([max_cost](const std::shared_ptr<tool>& current_tool) {
                  return current_tool->is_enabled() && current_tool->cost_per_call() <= max_cost;
              })
            | std::views::transform([](const std::shared_ptr<tool>& current_tool) {
                  return current_tool->get_name();
              });
        return std::vector<std::string>(names.begin(), names.end());
    }

    // Cadastra um modelo no catálogo de modelos disponíveis para seleção
    void register_available_model(const model& current_model) {
        available_models_.add(current_model);
    }

    // Busca um modelo disponível pelo nome — retorna nullopt se não achar,
    // em vez de lançar exceção ou devolver ponteiro nulo (TP3-Q2-B).
    std::optional<model> find_available_model(const std::string& name) const {
        return available_models_.find_by_name(name);
    }

    // Seleciona o modelo ativo do assistente pelo nome. Lança
    // model_not_found_error (derivada de llm_service_error) se o modelo não
    // estiver cadastrado no catálogo (TP3-Q2-A).
    void select_model(const std::string& name) {
        auto found = find_available_model(name);
        if (!found.has_value()) {
            throw model_not_found_error(name);
        }
        model_ = std::make_shared<model>(*found);
    }

    // Verifica se o custo total já faturado (modelo + ferramentas
    // faturáveis) ultrapassa o limite informado. Lança budget_exceeded_error
    // (derivada de llm_service_error) em caso de estouro (TP3-Q2-A).
    void enforce_budget(double limit) const {
        double spent = 0.0;
        if (model_ != nullptr) {
            spent += model_->billed_cost();
        }
        for (const auto& current_tool : tools_) {
            const auto* billable_ref = dynamic_cast<const billable*>(current_tool.get());
            if (billable_ref != nullptr) {
                spent += billable_ref->billed_cost();
            }
        }
        if (spent > limit) {
            throw budget_exceeded_error(spent, limit);
        }
    }

    // Executa a PRIMEIRA ferramenta de nome 'name' com o input informado.
    // Retorna um variant: tool_output em caso de sucesso, ou uma mensagem de
    // erro em std::string caso a ferramenta não exista (TP3-Q2-C).
    // Não é const porque tool::execute() muta o estado da ferramenta.
    execution_outcome run_tool(const std::string& name, const std::string& input) {
        for (const auto& current_tool : tools_) {
            if (current_tool->get_name() == name) {
                return tool_output{current_tool->get_name(), current_tool->execute(input),
                                  current_tool->cost_per_call()};
            }
        }
        return std::string("ferramenta nao encontrada: " + name);
    }

    // Executa a ferramenta na posicao 'index'. A GUI endereça por posição,
    // não por nome: o nome não é identidade, já que várias ferramentas do
    // mesmo tipo o compartilham (duas web_search_tool se chamam "web_search")
    // e a busca por nome sempre atingiria a primeira delas (TP3-Q6).
    execution_outcome run_tool_at(std::size_t index, const std::string& input) {
        if (index >= tools_.size()) {
            return std::string("indice de ferramenta invalido: " + std::to_string(index));
        }
        const auto& current_tool = tools_[index];
        return tool_output{current_tool->get_name(), current_tool->execute(input),
                          current_tool->cost_per_call()};
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
