// Implementação de produção de snapshot_repository: persiste em arquivo
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

class json_file_repository : public snapshot_repository {
private:
    std::string path_;  // caminho do arquivo JSON no disco

public:
    explicit json_file_repository(const std::string& path) : path_(path) {}

    void save(const assistant_snapshot& snapshot) override {
        nlohmann::json document = snapshot;
        std::ofstream file(path_);
        if (!file.is_open()) {
            throw llm_service_error("nao foi possivel abrir para escrita: " + path_);
        }
        file << document.dump(2);
    }

    assistant_snapshot load() override {
        std::ifstream file(path_);
        if (!file.is_open()) {
            throw llm_service_error("nao foi possivel abrir para leitura: " + path_);
        }
        nlohmann::json document;
        file >> document;
        return document.get<assistant_snapshot>();
    }
};

#endif // JSON_FILE_REPOSITORY_HPP
