// Classe de alto nível que depende apenas da abstração snapshot_repository,
// recebida por injeção no construtor (DIP, TP3-Q4-C). Não sabe se o
// snapshot vai para um arquivo, para memória ou para qualquer outro lugar.

#ifndef PERSISTENCE_SERVICE_HPP
#define PERSISTENCE_SERVICE_HPP

#include "ai_assistant.hpp"
#include "assistant_snapshot.hpp"
#include "snapshot_repository.hpp"

class persistence_service {
private:
    snapshot_repository& repository_;  // abstração injetada — dono é quem chama

public:
    explicit persistence_service(snapshot_repository& repository) : repository_(repository) {}

    // Monta um assistant_snapshot a partir do estado atual do assistente e
    // delega a persistência à abstração injetada.
    void save(const ai_assistant& assistant) {
        auto current_model = assistant.get_model();
        assistant_snapshot snapshot{
            assistant.get_name(),
            current_model != nullptr ? *current_model : model(),
            assistant.get_tools()
        };
        repository_.save(snapshot);
    }

    // Carrega o último snapshot persistido pela abstração injetada.
    assistant_snapshot load() {
        return repository_.load();
    }
};

#endif // PERSISTENCE_SERVICE_HPP
