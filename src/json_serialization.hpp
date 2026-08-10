// Serialização/desserialização JSON não-intrusiva (to_json/from_json fora
// das classes) via nlohmann::json. Inclui campo "version" no formato e
// "type" para recriar o tipo concreto da hierarquia polimórfica de Tool
// (TP3-Q4).

#ifndef JSON_SERIALIZATION_HPP
#define JSON_SERIALIZATION_HPP

#include <memory>
#include <nlohmann/json.hpp>
#include <string>

#include "assistant_snapshot.hpp"
#include "calculator_tool.hpp"
#include "exceptions.hpp"
#include "model.hpp"
#include "tool.hpp"
#include "web_search_tool.hpp"

using nlohmann::json;

// ---- Model: to_json/from_json não-intrusivos ----

inline void to_json(json& j, const Model& model) {
    j = json{
        {"name", model.get_name()},
        {"max_tokens", model.get_max_tokens()},
        {"cost_per_token", model.get_cost_per_token()},
        {"tokens_used", model.get_tokens_used()}
    };
}

inline void from_json(const json& j, Model& model) {
    model = Model(j.at("name").get<std::string>(),
                   j.at("max_tokens").get<int>(),
                   j.at("cost_per_token").get<double>());
    model.register_usage(j.at("tokens_used").get<int>());
}

// ---- Tool: dispatch polimórfico via campo "type" ----
// Tool é abstrata (não instanciável) e faz parte da hierarquia do TP2, então
// a desserialização não pode usar o padrão genérico to_json/from_json — é
// necessária uma fábrica que decide o tipo concreto a partir de "type".

inline std::string tool_type_name(const Tool& tool) {
    if (dynamic_cast<const WebSearchTool*>(&tool) != nullptr) {
        return "web_search_tool";
    }
    if (dynamic_cast<const CalculatorTool*>(&tool) != nullptr) {
        return "calculator_tool";
    }
    throw LlmServiceError("tipo de ferramenta desconhecido para serializacao");
}

inline void tool_to_json(json& j, const Tool& tool) {
    j = json{
        {"type", tool_type_name(tool)},
        {"name", tool.get_name()},
        {"description", tool.get_description()},
        {"enabled", tool.is_enabled()}
    };
    if (const auto* web_search = dynamic_cast<const WebSearchTool*>(&tool)) {
        j["calls"] = web_search->get_calls();
    }
}

inline std::shared_ptr<Tool> tool_from_json(const json& j) {
    const std::string type = j.at("type").get<std::string>();
    std::shared_ptr<Tool> tool;
    if (type == "web_search_tool") {
        tool = std::make_shared<WebSearchTool>(j.at("calls").get<int>());
    } else if (type == "calculator_tool") {
        tool = std::make_shared<CalculatorTool>();
    } else {
        throw LlmServiceError("tipo de ferramenta desconhecido na desserializacao: " + type);
    }
    if (!j.at("enabled").get<bool>()) {
        tool->toggle();
    }
    return tool;
}

// ---- AssistantSnapshot: to_json/from_json não-intrusivos, com "version" ----

inline void to_json(json& j, const AssistantSnapshot& snapshot) {
    json tools_json = json::array();
    for (const auto& tool : snapshot.tools) {
        json tool_json;
        tool_to_json(tool_json, *tool);
        tools_json.push_back(tool_json);
    }
    j = json{
        {"version", 1},
        {"assistant_name", snapshot.assistant_name},
        {"model", snapshot.model},
        {"tools", tools_json}
    };
}

inline void from_json(const json& j, AssistantSnapshot& snapshot) {
    const int version = j.at("version").get<int>();
    if (version != 1) {
        throw LlmServiceError("versao de snapshot nao suportada: " + std::to_string(version));
    }
    snapshot.assistant_name = j.at("assistant_name").get<std::string>();
    snapshot.model = j.at("model").get<Model>();
    snapshot.tools.clear();
    for (const auto& tool_json : j.at("tools")) {
        snapshot.tools.push_back(tool_from_json(tool_json));
    }
}

#endif // JSON_SERIALIZATION_HPP
