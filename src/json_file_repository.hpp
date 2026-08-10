// Implementação de produção de SnapshotRepository: persiste em arquivo
// JSON no disco (TP3-Q4-D).

#ifndef JSON_FILE_REPOSITORY_HPP
#define JSON_FILE_REPOSITORY_HPP

#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

#include "assistant_snapshot.hpp"
#include "exceptions.hpp"
#include "json_serialization.hpp"
#include "snapshot_repository.hpp"

class JsonFileRepository : public SnapshotRepository {
private:
    std::string path_;  // caminho do arquivo JSON no disco

public:
    explicit JsonFileRepository(const std::string& path) : path_(path) {}

    void save(const AssistantSnapshot& snapshot) override {
        nlohmann::json document = snapshot;
        std::ofstream file(path_);
        if (!file.is_open()) {
            throw LlmServiceError("nao foi possivel abrir para escrita: " + path_);
        }
        file << document.dump(2);
    }

    AssistantSnapshot load() override {
        std::ifstream file(path_);
        if (!file.is_open()) {
            throw LlmServiceError("nao foi possivel abrir para leitura: " + path_);
        }
        nlohmann::json document;
        file >> document;
        return document.get<AssistantSnapshot>();
    }
};

#endif // JSON_FILE_REPOSITORY_HPP
