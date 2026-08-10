// Classe que representa uma sessão de conversa.
// Contém composição de mensagens e agregação com usuário.
// Possui destrutor explícito com efeito observável.

#ifndef SESSION_HPP
#define SESSION_HPP

#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "crtp_mixins.hpp"
#include "message.hpp"
#include "user.hpp"

// session usa CRTP (instance_counted/textual_clonable) — mesma técnica de
// message, sem custo de vtable, adequada porque session não participa de
// uma hierarquia polimórfica.
class session : public instance_counted<session>, public textual_clonable<session> {
private:
    int id_;                        // identificador da sessão
    std::vector<message> messages_; // mensagens da sessão (composição)
    user& user_;                    // usuário dono da sessão (agregação via referência)

public:
    // Construtor com lista de inicialização
    session(int id, user& current_user)
        : id_(id), messages_(), user_(current_user) {
        std::cout << "  session(" << id_ << ") criada" << std::endl;
    }

    // Destrutor explícito com efeito observável — imprime mensagem ao encerrar sessão
    ~session() {
        std::cout << "  ~session(" << id_ << ") destruida ("
                  << messages_.size() << " mensagem(ns) descartada(s))" << std::endl;
    }

    // Getters const
    int get_id() const { return id_; }
    const user& get_user() const { return user_; }
    size_t get_message_count() const { return messages_.size(); }

    // Adiciona uma mensagem à sessão
    void add_message(const message& new_message) {
        messages_.push_back(new_message);
        std::cout << "    message(\"" << new_message.get_role() << "\") adicionada" << std::endl;
    }

    // Gera um resumo da sessão contendo id, usuário e contagem de mensagens
    std::string summarize() const {
        std::string summary = "Sessao #" + std::to_string(id_);
        summary += " | Usuario: " + user_.get_name();
        summary += " | Mensagens: " + std::to_string(messages_.size());
        return summary;
    }

    // Requisito do mixin textual_clonable — reaproveita o resumo já existente
    std::string to_text() const { return summarize(); }

    // Retorna os papéis (roles) distintos presentes nas mensagens da sessão.
    // std::unordered_set é apropriado aqui pelo acesso/insercao O(1) e por
    // a ordem dos papéis ser irrelevante — só a unicidade importa
    // (TP3-Q3-A, container 2, diferente do std::map já usado em tool_utils).
    std::unordered_set<std::string> distinct_roles() const {
        std::unordered_set<std::string> roles;
        for (const auto& current_message : messages_) {
            roles.insert(current_message.get_role());
        }
        return roles;
    }
};

#endif // SESSION_HPP
