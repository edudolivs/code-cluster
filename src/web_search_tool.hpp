// Ferramenta concreta de busca na web. Sobrescreve todos os métodos puros
// e complementa describe() chamando a versão da base (Tool::describe()).

#ifndef WEB_SEARCH_TOOL_HPP
#define WEB_SEARCH_TOOL_HPP

#include <iostream>
#include <string>

#include "tool.hpp"

class WebSearchTool : public Tool {
private:
    int calls_ = 0;  // quantidade de buscas realizadas

public:
    WebSearchTool()
        : Tool("web_search", "Busca informacoes na internet") {}

    // Destrutor com efeito observável — executa ANTES do destrutor da base
    ~WebSearchTool() override {
        std::cout << "  ~WebSearchTool() destruida (derivada)" << std::endl;
    }

    // Override do método puro — simula a busca e registra a chamada
    std::string execute(const std::string& input) override {
        if (!is_enabled()) {
            return disabled_message();
        }
        ++calls_;
        return "[web_search] Buscando por '" + input + "': 3 resultados encontrados.";
    }

    // Override do método puro — custo por busca
    double cost_per_call() const override { return 0.002; }

    // Override do método não-puro chamando a versão da base primeiro (Q1-B)
    std::string describe() const override {
        return Tool::describe() + " | buscas realizadas: " + std::to_string(calls_);
    }

    // Getter const
    int get_calls() const { return calls_; }
};

#endif // WEB_SEARCH_TOOL_HPP
