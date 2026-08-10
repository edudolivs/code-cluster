// Ferramenta concreta de busca na web. Heranca multipla: implementa tanto
// a interface tool (ferramenta) quanto billable (faturacao).
// Sobrescreve todos os metodos puros e complementa describe() chamando a
// versao da base (tool::describe()).

#ifndef WEB_SEARCH_TOOL_HPP
#define WEB_SEARCH_TOOL_HPP

#include <iostream>
#include <string>

#include "tool.hpp"
#include "billable.hpp"

class web_search_tool : public tool, public billable {
private:
    int calls_ = 0;  // quantidade de buscas realizadas

public:
    web_search_tool()
        : tool("web_search", "Busca informacoes na internet") {}

    // Reconstroi a ferramenta com um numero de chamadas ja registrado —
    // usado por from_json (TP3-Q4) para restaurar o estado fielmente.
    explicit web_search_tool(int calls)
        : tool("web_search", "Busca informacoes na internet"), calls_(calls) {}

    // Destrutor com efeito observável — executa ANTES do destrutor da base
    ~web_search_tool() override {
        std::cout << "  ~web_search_tool() destruida (derivada)" << std::endl;
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

    // Override da interface billable — custo total das buscas realizadas
    double billed_cost() const override {
        return calls_ * cost_per_call();
    }

    // Override do método não-puro chamando a versão da base primeiro (Q1-B)
    std::string describe() const override {
        return tool::describe() + " | buscas realizadas: " + std::to_string(calls_);
    }

    // Getter const
    int get_calls() const { return calls_; }

    // Override de equals() incluindo o campo proprio calls_ (TP3-Q4)
    bool equals(const tool& other) const override {
        const auto* typed_other = dynamic_cast<const web_search_tool*>(&other);
        return typed_other != nullptr
            && tool::equals(other)
            && calls_ == typed_other->calls_;
    }
};

#endif // WEB_SEARCH_TOOL_HPP
