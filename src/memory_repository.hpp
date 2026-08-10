// Implementação de teste de SnapshotRepository: guarda o snapshot em
// memória, sem tocar disco ou rede — permite testar a lógica de persistência
// sem efeitos colaterais (TP3-Q4-D).

#ifndef MEMORY_REPOSITORY_HPP
#define MEMORY_REPOSITORY_HPP

#include <optional>

#include "assistant_snapshot.hpp"
#include "exceptions.hpp"
#include "snapshot_repository.hpp"

class MemoryRepository : public SnapshotRepository {
private:
    std::optional<AssistantSnapshot> stored_;  // ultimo snapshot salvo, se houver

public:
    void save(const AssistantSnapshot& snapshot) override {
        stored_ = snapshot;
    }

    AssistantSnapshot load() override {
        if (!stored_.has_value()) {
            throw LlmServiceError("nenhum snapshot salvo em memoria");
        }
        return *stored_;
    }
};

#endif // MEMORY_REPOSITORY_HPP
