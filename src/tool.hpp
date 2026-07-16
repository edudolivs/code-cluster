// Classe base abstrata que representa uma ferramenta do assistente de IA.
// Define o contrato de execução (puro), custo por chamada (puro) e uma
// descrição padrão (virtual não-puro). Destrutor virtual com efeito observável.

#ifndef TOOL_HPP
#define TOOL_HPP

#include <iostream>
#include <string>

class Tool {
private:
    std::string name_;         // nome da ferramenta
    std::string description_;  // descrição da ferramenta
    bool enabled_;             // indica se a ferramenta está ativa

protected:
    // Mensagem padrão de erro usada pelas derivadas quando desabilitadas
    std::string disabled_message() const {
        return "[ERRO] Ferramenta '" + name_ + "' esta desabilitada.";
    }

public:
    // Construtor com lista de inicialização
    Tool(const std::string& name, const std::string& description, bool enabled = true)
        : name_(name), description_(description), enabled_(enabled) {}

    // Destrutor virtual obrigatório — imprime para demonstrar a cadeia de destruição
    virtual ~Tool() {
        std::cout << "  ~Tool(\"" << name_ << "\") destruida (base)" << std::endl;
    }

    // Método virtual puro — cada ferramenta executa de forma própria
    virtual std::string execute(const std::string& input) = 0;

    // Método virtual puro — custo em dólares de uma chamada da ferramenta
    virtual double cost_per_call() const = 0;

    // Método virtual não-puro — descrição padrão que derivadas podem complementar
    virtual std::string describe() const {
        return "[" + name_ + "] " + description_
             + (enabled_ ? " (ativa)" : " (inativa)");
    }

    // Getters const
    std::string get_name() const { return name_; }
    std::string get_description() const { return description_; }
    bool is_enabled() const { return enabled_; }

    // Alterna o estado habilitado/desabilitado da ferramenta
    void toggle() { enabled_ = !enabled_; }
};

#endif // TOOL_HPP
