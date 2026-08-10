// Classe principal que coordena o assistente de IA.
// Gerencia modelo, ferramentas e sessões. Possui destrutor explícito
// que libera as sessões criadas (composição).

#ifndef AI_ASSISTANT_HPP
#define AI_ASSISTANT_HPP

#include <iostream>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

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

public:
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

    // Getter const
    std::string get_name() const { return name_; }

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
