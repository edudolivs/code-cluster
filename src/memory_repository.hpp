// Implementação de teste de snapshot_repository: guarda o snapshot em
// memória, sem tocar disco ou rede — permite testar a lógica de persistência
// sem efeitos colaterais (TP3-Q4-D).

#ifndef MEMORY_REPOSITORY_HPP
#define MEMORY_REPOSITORY_HPP

#include <optional>

#include "assistant_snapshot.hpp"
#include "exceptions.hpp"
#include "snapshot_repository.hpp"

class memory_repository : public snapshot_repository {
private:
    std::optional<assistant_snapshot> stored_;  // ultimo snapshot salvo, se houver

public:
    void save(const assistant_snapshot& snapshot) override {
        stored_ = snapshot;
    }

    assistant_snapshot load() override {
        if (!stored_.has_value()) {
            throw llm_service_error("nenhum snapshot salvo em memoria");
        }
        return *stored_;
    }
};

#endif // MEMORY_REPOSITORY_HPP
