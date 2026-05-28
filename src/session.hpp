// Classe que representa uma sessão de conversa.
// Contém composição de mensagens e agregação com usuário.
// Possui destrutor explícito com efeito observável.

#ifndef SESSION_HPP
#define SESSION_HPP

#include <iostream>
#include <string>
#include <vector>

#include "message.hpp"
#include "user.hpp"

class Session {
private:
    int id_;                        // identificador da sessão
    std::vector<Message> messages_; // mensagens da sessão (composição)
    User* user_;                    // usuário dono da sessão (agregação)

public:
    // Construtor com lista de inicialização
    Session(int id, User* user)
        : id_(id), messages_(), user_(user) {
        std::cout << "  Session(" << id_ << ") criada" << std::endl;
    }

    // Destrutor explícito com efeito observável — imprime mensagem ao encerrar sessão
    ~Session() {
        std::cout << "  ~Session(" << id_ << ") destruida ("
                  << messages_.size() << " mensagem(ns) descartada(s))" << std::endl;
    }

    // Getters const
    int get_id() const { return id_; }
    User* get_user() const { return user_; }
    size_t get_message_count() const { return messages_.size(); }

    // Adiciona uma mensagem à sessão
    void add_message(const Message& message) {
        messages_.push_back(message);
        std::cout << "    Message(\"" << message.get_role() << "\") adicionada" << std::endl;
    }

    // Gera um resumo da sessão contendo id, usuário e contagem de mensagens
    std::string summarize() const {
        std::string summary = "Sessao #" + std::to_string(id_);
        if (user_ != nullptr) {
            summary += " | Usuario: " + user_->get_name();
        }
        summary += " | Mensagens: " + std::to_string(messages_.size());
        return summary;
    }
};

#endif // SESSION_HPP
