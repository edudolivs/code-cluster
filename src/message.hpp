// Classe que representa uma mensagem trocada em uma sessão.
// Armazena papel (user/assistant), conteúdo e timestamp formatado.

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>

#include "crtp_mixins.hpp"

// message usa CRTP (instance_counted/textual_clonable) para ganhar contagem
// de instâncias e clonagem textual sem custo de vtable — família sem
// polimorfismo dinâmico, ao contrário de tool.
class message : public instance_counted<message>, public textual_clonable<message> {
private:
    std::string role_;       // papel do remetente (ex: "user", "assistant")
    std::string content_;    // conteúdo da mensagem
    std::string timestamp_;  // momento em que a mensagem foi criada

public:
    // Construtor com lista de inicialização
    message(const std::string& role, const std::string& content,
            const std::string& timestamp)
        : role_(role), content_(content), timestamp_(timestamp) {}

    // Getters const
    std::string get_role() const { return role_; }
    std::string get_content() const { return content_; }
    std::string get_timestamp() const { return timestamp_; }

    // Formata a mensagem para exibição no terminal
    std::string format() const {
        return "[" + timestamp_ + "] " + role_ + ": " + content_;
    }

    // Requisito do mixin textual_clonable — reaproveita a formatação já existente
    std::string to_text() const { return format(); }
};

#endif // MESSAGE_HPP
