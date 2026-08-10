// DTO que representa o estado persistível de um AiAssistant: nome, modelo
// e ferramentas. Mantido separado de AiAssistant de propósito — a classe
// de domínio não precisa saber nada sobre serialização ou persistência
// (SRP: ver seção "SOLID" do README).

#ifndef ASSISTANT_SNAPSHOT_HPP
#define ASSISTANT_SNAPSHOT_HPP

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "model.hpp"
#include "tool.hpp"

struct AssistantSnapshot {
    std::string assistant_name;
    Model model;
    std::vector<std::shared_ptr<Tool>> tools;

    // Igualdade estrutural — compara o modelo por valor (Model::operator==)
    // e cada ferramenta polimorficamente (Tool::equals), sem depender de
    // reserializar para JSON só para comparar.
    bool operator==(const AssistantSnapshot& other) const {
        if (assistant_name != other.assistant_name || !(model == other.model)) {
            return false;
        }
        if (tools.size() != other.tools.size()) {
            return false;
        }
        for (std::size_t i = 0; i < tools.size(); ++i) {
            if (!tools[i]->equals(*other.tools[i])) {
                return false;
            }
        }
        return true;
    }
};

#endif // ASSISTANT_SNAPSHOT_HPP
