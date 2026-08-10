// Ferramenta concreta de calculadora. Classe final: folha completa da
// hierarquia, não deve ser especializada (justificativa no README).

#ifndef CALCULATOR_TOOL_HPP
#define CALCULATOR_TOOL_HPP

#include <iostream>
#include <string>

#include "tool.hpp"

class CalculatorTool final : public Tool {
public:
    CalculatorTool()
        : Tool("calculator", "Realiza calculos matematicos") {}

    // Destrutor com efeito observável — executa ANTES do destrutor da base
    ~CalculatorTool() override {
        std::cout << "  ~CalculatorTool() destruida (derivada)" << std::endl;
    }

    // Override do método puro — simula a avaliação da expressão
    std::string execute(const std::string& input) override {
        if (!is_enabled()) {
            return disabled_message();
        }
        return "[calculator] Expressao '" + input + "' avaliada com sucesso.";
    }

    // Override do método puro — custo por calculo
    double cost_per_call() const override { return 0.0005; }

    // Override de equals() — CalculatorTool nao tem estado proprio, entao
    // apenas confirma o tipo e delega para a comparacao da base (TP3-Q4)
    bool equals(const Tool& other) const override {
        return dynamic_cast<const CalculatorTool*>(&other) != nullptr && Tool::equals(other);
    }
};

#endif // CALCULATOR_TOOL_HPP
