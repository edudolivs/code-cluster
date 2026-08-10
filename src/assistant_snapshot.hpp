// DTO que representa o estado persistível de um ai_assistant: nome, modelo
// e ferramentas. Mantido separado de ai_assistant de propósito — a classe
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

struct assistant_snapshot {
    std::string assistant_name;
    model assistant_model;
    std::vector<std::shared_ptr<tool>> tools;

    // Igualdade estrutural — compara o modelo por valor (model::operator==)
    // e cada ferramenta polimorficamente (tool::equals), sem depender de
    // reserializar para JSON só para comparar.
    bool operator==(const assistant_snapshot& other) const {
        if (assistant_name != other.assistant_name || !(assistant_model == other.assistant_model)) {
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
