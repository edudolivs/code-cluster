// Funções livres que operam sobre a hierarquia de Tool via ponteiro base.

#ifndef TOOL_UTILS_HPP
#define TOOL_UTILS_HPP

#include <memory>
#include <vector>

#include "tool.hpp"

// Retorna um ponteiro NÃO-proprietário para a ferramenta de maior custo
// por chamada, ou nullptr se o vetor estiver vazio. O vetor continua
// dono dos objetos (unique_ptr); o chamador apenas observa.
inline const Tool* most_expensive_tool(
        const std::vector<std::unique_ptr<Tool>>& tools) {
    const Tool* most_expensive = nullptr;
    for (const auto& tool : tools) {
        if (most_expensive == nullptr
                || tool->cost_per_call() > most_expensive->cost_per_call()) {
            most_expensive = tool.get();
        }
    }
    return most_expensive;
}

#endif // TOOL_UTILS_HPP
