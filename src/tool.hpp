// Classe que representa uma ferramenta disponível para o assistente de IA.
// Pode ser habilitada/desabilitada e simula a execução com uma entrada.

#ifndef TOOL_HPP
#define TOOL_HPP

#include <string>

class Tool {
private:
    std::string name_;         // nome da ferramenta
    std::string description_;  // descrição da ferramenta
    bool enabled_;             // indica se a ferramenta está ativa

public:
    // Construtor com lista de inicialização
    Tool(const std::string& name, const std::string& description, bool enabled = true)
        : name_(name), description_(description), enabled_(enabled) {}

    // Getters const
    std::string get_name() const { return name_; }
    std::string get_description() const { return description_; }
    bool is_enabled() const { return enabled_; }

    // Alterna o estado habilitado/desabilitado da ferramenta
    void toggle() {
        enabled_ = !enabled_;
    }

    // Simula a execução da ferramenta com uma entrada.
    // Retorna mensagem de erro se desabilitada, senão simula resultado.
    std::string execute(const std::string& input) const {
        if (!enabled_) {
            return "[ERRO] Ferramenta '" + name_ + "' esta desabilitada.";
        }
        return "[" + name_ + "] Resultado para '" + input + "': operacao concluida.";
    }
};

#endif // TOOL_HPP
