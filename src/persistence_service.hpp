// Classe de alto nível que depende apenas da abstração SnapshotRepository,
// recebida por injeção no construtor (DIP, TP3-Q4-C). Não sabe se o
// snapshot vai para um arquivo, para memória ou para qualquer outro lugar.

#ifndef PERSISTENCE_SERVICE_HPP
#define PERSISTENCE_SERVICE_HPP

#include "ai_assistant.hpp"
#include "assistant_snapshot.hpp"
#include "snapshot_repository.hpp"

class PersistenceService {
private:
    SnapshotRepository& repository_;  // abstração injetada — dono é quem chama

public:
    explicit PersistenceService(SnapshotRepository& repository) : repository_(repository) {}

    // Monta um AssistantSnapshot a partir do estado atual do assistente e
    // delega a persistência à abstração injetada.
    void save(const AiAssistant& assistant) {
        auto model = assistant.get_model();
        AssistantSnapshot snapshot{
            assistant.get_name(),
            model != nullptr ? *model : Model(),
            assistant.get_tools()
        };
        repository_.save(snapshot);
    }

    // Carrega o último snapshot persistido pela abstração injetada.
    AssistantSnapshot load() {
        return repository_.load();
    }
};

#endif // PERSISTENCE_SERVICE_HPP
