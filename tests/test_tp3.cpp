// Testes automatizados do TP3 com Catch2 (Questao 5, extra).
// Um TEST_CASE cobrindo cada topico: template/concept (Q1), tratamento de
// erros (Q2), serializacao round-trip (Q4) e DIP com memory_repository (Q4).

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

TEST_CASE("Q1 (TP3): catalog<T> busca por nome com optional", "[tp3][q1]") {
    catalog<model> model_catalog;
    model_catalog.add(model("GPT-4o", 8192, 0.00003));
    model_catalog.add(model("Claude-Opus", 200000, 0.000015));

    REQUIRE(model_catalog.size() == 2);

    auto found = model_catalog.find_by_name("Claude-Opus");
    REQUIRE(found.has_value());
    REQUIRE(found->get_max_tokens() == 200000);

    auto missing = model_catalog.find_by_name("Modelo-Fantasma");
    REQUIRE_FALSE(missing.has_value());
}

TEST_CASE("Q2 (TP3): excecao pela base e optional em find_available_model", "[tp3][q2]") {
    ai_assistant assistant("Assistente de Teste");
    assistant.register_available_model(model("GPT-4o", 8192, 0.00003));

    SECTION("select_model lanca model_not_found_error, capturavel pela base") {
        REQUIRE_THROWS_AS(assistant.select_model("Modelo-Fantasma"), llm_service_error);
    }

    SECTION("find_available_model retorna optional nos dois casos") {
        REQUIRE(assistant.find_available_model("GPT-4o").has_value());
        REQUIRE_FALSE(assistant.find_available_model("Modelo-Fantasma").has_value());
    }

    SECTION("enforce_budget lanca budget_exceeded_error, capturavel pela base") {
        assistant.set_model(std::make_shared<model>("GPT-4o", 8192, 0.00003));
        assistant.get_model()->register_usage(5000);  // $0.15 faturados
        REQUIRE_NOTHROW(assistant.enforce_budget(1.00));
        REQUIRE_THROWS_AS(assistant.enforce_budget(0.10), llm_service_error);
    }
}

TEST_CASE("Q4 (TP3): serializacao round-trip via nlohmann::json", "[tp3][q4]") {
    assistant_snapshot original{
        "Assistente de Teste",
        model("GPT-4o", 8192, 0.00003),
        {std::make_shared<web_search_tool>(), std::make_shared<calculator_tool>()}
    };

    nlohmann::json document = original;
    assistant_snapshot restored = document.get<assistant_snapshot>();

    REQUIRE(restored == original);
    REQUIRE(document.at("version").get<int>() == 1);
}

TEST_CASE("Q4 (TP3): DIP - memory_repository exercita a logica sem tocar disco", "[tp3][q4][dip]") {
    ai_assistant assistant("Assistente de Teste");
    assistant.set_model(std::make_shared<model>("GPT-4o", 8192, 0.00003));
    assistant.add_tool(std::make_shared<calculator_tool>());

    memory_repository in_memory_repository;
    persistence_service persistence(in_memory_repository);

    persistence.save(assistant);
    assistant_snapshot restored = persistence.load();

    REQUIRE(restored.assistant_name == "Assistente de Teste");
    REQUIRE(restored.tools.size() == 1);
}
