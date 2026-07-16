// Ferramenta concreta de busca na web. Heranca multipla: implementa tanto
// a interface Tool (ferramenta) quanto Billable (faturacao).
// Sobrescreve todos os metodos puros e complementa describe() chamando a
// versao da base (Tool::describe()).

#ifndef WEB_SEARCH_TOOL_HPP
#define WEB_SEARCH_TOOL_HPP

#include <iostream>
#include <string>

#include "tool.hpp"
#include "billable.hpp"

class WebSearchTool : public Tool, public Billable {
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

    // Override da interface Billable — custo total das buscas realizadas
    double billed_cost() const override {
        return calls_ * cost_per_call();
    }

    // Override do método não-puro chamando a versão da base primeiro (Q1-B)
    std::string describe() const override {
        return Tool::describe() + " | buscas realizadas: " + std::to_string(calls_);
    }

    // Getter const
    int get_calls() const { return calls_; }
};

#endif // WEB_SEARCH_TOOL_HPP
