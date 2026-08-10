// Classe que representa um usuário do sistema de LLM.
// Armazena nome e e-mail, com método para exibir informações formatadas.

#ifndef USER_HPP
#define USER_HPP

#include <string>

class user {
private:
    std::string name_;    // nome do usuário
    std::string email_;   // e-mail do usuário

public:
    // Construtor com lista de inicialização
    user(const std::string& name, const std::string& email)
        : name_(name), email_(email) {}

    // Getters const
    std::string get_name() const { return name_; }
    std::string get_email() const { return email_; }

    // Retorna informações formatadas do usuário
    std::string display_info() const {
        return "Usuario: " + name_ + " <" + email_ + ">";
    }
};

#endif // USER_HPP
