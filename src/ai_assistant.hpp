// Classe principal que coordena o assistente de IA.
// Gerencia modelo, ferramentas e sessões. Possui destrutor explícito
// que libera as sessões criadas (composição).

#ifndef AI_ASSISTANT_HPP
#define AI_ASSISTANT_HPP

#include <iostream>
#include <string>
#include <vector>

#include "model.hpp"
#include "session.hpp"
#include "tool.hpp"
#include "user.hpp"

class AiAssistant {
private:
    std::string name_;              // nome do assistente
    Model* model_;                  // modelo de linguagem utilizado (agregação)
    std::vector<Tool*> tools_;      // ferramentas disponíveis (agregação)
    std::vector<Session*> sessions_; // sessões gerenciadas (composição — o assistente é dono)

public:
    // Construtor com lista de inicialização
    AiAssistant(const std::string& name)
        : name_(name), model_(nullptr), tools_(), sessions_() {}

    // Destrutor explícito — libera as sessões criadas (composição)
    ~AiAssistant() {
        std::cout << "[~AiAssistant] Encerrando assistente '" << name_
                  << "'. Liberando " << sessions_.size()
                  << " sessao(oes)..." << std::endl;
        for (Session* session : sessions_) {
            delete session;
        }
        sessions_.clear();
    }

    // Getter const
    std::string get_name() const { return name_; }

    // Define o modelo de linguagem a ser utilizado
    void set_model(Model* model) {
        model_ = model;
    }

    // Adiciona uma ferramenta ao assistente
    void add_tool(Tool* tool) {
        tools_.push_back(tool);
    }

    // Cria uma nova sessão para o usuário e a armazena internamente
    Session* create_session(User* user) {
        int new_id = static_cast<int>(sessions_.size()) + 1;
        Session* session = new Session(new_id, user);
        sessions_.push_back(session);
        return session;
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
