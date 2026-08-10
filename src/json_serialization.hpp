// Serialização/desserialização JSON não-intrusiva (to_json/from_json fora
// das classes) via nlohmann::json. Inclui campo "version" no formato e
// "type" para recriar o tipo concreto da hierarquia polimórfica de tool
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

// ---- model: to_json/from_json não-intrusivos ----

inline void to_json(json& j, const model& current_model) {
    j = json{
        {"name", current_model.get_name()},
        {"max_tokens", current_model.get_max_tokens()},
        {"cost_per_token", current_model.get_cost_per_token()},
        {"tokens_used", current_model.get_tokens_used()}
    };
}

inline void from_json(const json& j, model& target_model) {
    target_model = model(j.at("name").get<std::string>(),
                          j.at("max_tokens").get<int>(),
                          j.at("cost_per_token").get<double>());
    target_model.register_usage(j.at("tokens_used").get<int>());
}

// ---- tool: dispatch polimórfico via campo "type" ----
// tool é abstrata (não instanciável) e faz parte da hierarquia do TP2, então
// a desserialização não pode usar o padrão genérico to_json/from_json — é
// necessária uma fábrica que decide o tipo concreto a partir de "type".

inline std::string tool_type_name(const tool& current_tool) {
    if (dynamic_cast<const web_search_tool*>(&current_tool) != nullptr) {
        return "web_search_tool";
    }
    if (dynamic_cast<const calculator_tool*>(&current_tool) != nullptr) {
        return "calculator_tool";
    }
    throw llm_service_error("tipo de ferramenta desconhecido para serializacao");
}

inline void tool_to_json(json& j, const tool& current_tool) {
    j = json{
        {"type", tool_type_name(current_tool)},
        {"name", current_tool.get_name()},
        {"description", current_tool.get_description()},
        {"enabled", current_tool.is_enabled()}
    };
    if (const auto* web_search = dynamic_cast<const web_search_tool*>(&current_tool)) {
        j["calls"] = web_search->get_calls();
    }
}

inline std::shared_ptr<tool> tool_from_json(const json& j) {
    const std::string type = j.at("type").get<std::string>();
    std::shared_ptr<tool> restored_tool;
    if (type == "web_search_tool") {
        restored_tool = std::make_shared<web_search_tool>(j.at("calls").get<int>());
    } else if (type == "calculator_tool") {
        restored_tool = std::make_shared<calculator_tool>();
    } else {
        throw llm_service_error("tipo de ferramenta desconhecido na desserializacao: " + type);
    }
    if (!j.at("enabled").get<bool>()) {
        restored_tool->toggle();
    }
    return restored_tool;
}

// ---- assistant_snapshot: to_json/from_json não-intrusivos, com "version" ----

inline void to_json(json& j, const assistant_snapshot& snapshot) {
    json tools_json = json::array();
    for (const auto& current_tool : snapshot.tools) {
        json tool_json;
        tool_to_json(tool_json, *current_tool);
        tools_json.push_back(tool_json);
    }
    j = json{
        {"version", 1},
        {"assistant_name", snapshot.assistant_name},
        {"model", snapshot.assistant_model},
        {"tools", tools_json}
    };
}

inline void from_json(const json& j, assistant_snapshot& snapshot) {
    const int version = j.at("version").get<int>();
    if (version != 1) {
        throw llm_service_error("versao de snapshot nao suportada: " + std::to_string(version));
    }
    snapshot.assistant_name = j.at("assistant_name").get<std::string>();
    snapshot.assistant_model = j.at("model").get<model>();
    snapshot.tools.clear();
    for (const auto& tool_json : j.at("tools")) {
        snapshot.tools.push_back(tool_from_json(tool_json));
    }
}

#endif // JSON_SERIALIZATION_HPP
