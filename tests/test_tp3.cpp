// Testes automatizados do TP3 com Catch2 (Questao 5, extra).
// Um TEST_CASE cobrindo cada topico: template/concept (Q1), tratamento de
// erros (Q2), serializacao round-trip (Q4) e DIP com MemoryRepository (Q4).

#include <catch2/catch_test_macros.hpp>

#include <nlohmann/json.hpp>

#include "ai_assistant.hpp"
#include "assistant_snapshot.hpp"
#include "calculator_tool.hpp"
#include "catalog.hpp"
#include "exceptions.hpp"
#include "json_serialization.hpp"
#include "memory_repository.hpp"
#include "model.hpp"
#include "persistence_service.hpp"
#include "web_search_tool.hpp"

TEST_CASE("Q1 (TP3): Catalog<T> busca por nome com optional", "[tp3][q1]") {
    Catalog<Model> catalog;
    catalog.add(Model("GPT-4o", 8192, 0.00003));
    catalog.add(Model("Claude-Opus", 200000, 0.000015));

    REQUIRE(catalog.size() == 2);

    auto found = catalog.find_by_name("Claude-Opus");
    REQUIRE(found.has_value());
    REQUIRE(found->get_max_tokens() == 200000);

    auto missing = catalog.find_by_name("Modelo-Fantasma");
    REQUIRE_FALSE(missing.has_value());
}

TEST_CASE("Q2 (TP3): excecao pela base e optional em find_available_model", "[tp3][q2]") {
    AiAssistant assistant("Assistente de Teste");
    assistant.register_available_model(Model("GPT-4o", 8192, 0.00003));

    SECTION("select_model lanca ModelNotFoundError, capturavel pela base") {
        REQUIRE_THROWS_AS(assistant.select_model("Modelo-Fantasma"), LlmServiceError);
    }

    SECTION("find_available_model retorna optional nos dois casos") {
        REQUIRE(assistant.find_available_model("GPT-4o").has_value());
        REQUIRE_FALSE(assistant.find_available_model("Modelo-Fantasma").has_value());
    }
}

TEST_CASE("Q4 (TP3): serializacao round-trip via nlohmann::json", "[tp3][q4]") {
    AssistantSnapshot original{
        "Assistente de Teste",
        Model("GPT-4o", 8192, 0.00003),
        {std::make_shared<WebSearchTool>(), std::make_shared<CalculatorTool>()}
    };

    nlohmann::json document = original;
    AssistantSnapshot restored = document.get<AssistantSnapshot>();

    REQUIRE(restored == original);
    REQUIRE(document.at("version").get<int>() == 1);
}

TEST_CASE("Q4 (TP3): DIP - MemoryRepository exercita a logica sem tocar disco", "[tp3][q4][dip]") {
    AiAssistant assistant("Assistente de Teste");
    assistant.set_model(std::make_shared<Model>("GPT-4o", 8192, 0.00003));
    assistant.add_tool(std::make_shared<CalculatorTool>());

    MemoryRepository memory_repository;
    PersistenceService persistence(memory_repository);

    persistence.save(assistant);
    AssistantSnapshot restored = persistence.load();

    REQUIRE(restored.assistant_name == "Assistente de Teste");
    REQUIRE(restored.tools.size() == 1);
}
