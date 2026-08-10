// Funções livres que operam sobre a hierarquia de Tool via ponteiro base.

#ifndef TOOL_UTILS_HPP
#define TOOL_UTILS_HPP

#include <algorithm>
#include <map>
#include <memory>
#include <numeric>
#include <string>
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

// Retorna um ponteiro NÃO-proprietário para a primeira ferramenta com o
// nome informado, ou nullptr se não existir. Usa std::find_if (TP3-Q3-B).
inline const Tool* find_tool_by_name(
        const std::vector<std::shared_ptr<Tool>>& tools, const std::string& name) {
    auto it = std::find_if(tools.begin(), tools.end(),
        [&name](const std::shared_ptr<Tool>& tool) { return tool->get_name() == name; });
    return it == tools.end() ? nullptr : it->get();
}

// Ordena o vetor de ferramentas por custo por chamada, do mais barato ao
// mais caro, usando std::sort com um comparador lambda (TP3-Q3-B).
inline void sort_tools_by_cost(std::vector<std::shared_ptr<Tool>>& tools) {
    std::sort(tools.begin(), tools.end(),
        [](const std::shared_ptr<Tool>& a, const std::shared_ptr<Tool>& b) {
            return a->cost_per_call() < b->cost_per_call();
        });
}

// Conta quantas ferramentas têm custo por chamada acima de 'threshold'.
// Usa std::count_if com uma lambda que CAPTURA 'threshold' (TP3-Q3-B).
inline std::size_t count_tools_above_cost(
        const std::vector<std::shared_ptr<Tool>>& tools, double threshold) {
    return static_cast<std::size_t>(std::count_if(tools.begin(), tools.end(),
        [threshold](const std::shared_ptr<Tool>& tool) { return tool->cost_per_call() > threshold; }));
}

// Soma o custo por chamada de todas as ferramentas usando std::accumulate
// (TP3-Q3-B).
inline double total_cost_per_call(const std::vector<std::shared_ptr<Tool>>& tools) {
    return std::accumulate(tools.begin(), tools.end(), 0.0,
        [](double acc, const std::shared_ptr<Tool>& tool) { return acc + tool->cost_per_call(); });
}

// Indexa as ferramentas por nome em um std::map, apropriado aqui porque
// os nomes são únicos e a ordenação alfabética é útil para exibição
// determinística (TP3-Q3-A, container 1).
inline std::map<std::string, std::shared_ptr<Tool>> index_tools_by_name(
        const std::vector<std::shared_ptr<Tool>>& tools) {
    std::map<std::string, std::shared_ptr<Tool>> index;
    for (const auto& tool : tools) {
        index[tool->get_name()] = tool;
    }
    return index;
}

#endif // TOOL_UTILS_HPP
