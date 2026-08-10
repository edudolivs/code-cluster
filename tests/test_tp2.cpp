// Testes automatizados do TP2 com Catch2.
// Um TEST_CASE por questão: destruição em cadeia (Q1), polimorfismo
// dinâmico (Q2) e interface pura (Q3).

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "billable.hpp"
#include "calculator_tool.hpp"
#include "model.hpp"
#include "tool.hpp"
#include "tool_utils.hpp"
#include "web_search_tool.hpp"

namespace {

// Captura o que os destrutores imprimem em std::cout ao destruir a
// ferramenta via ponteiro para a base
std::string capture_destruction_log(std::unique_ptr<tool> owned_tool) {
    std::ostringstream captured;
    std::streambuf* original = std::cout.rdbuf(captured.rdbuf());
    owned_tool.reset();  // dispara ~Derivada -> ~tool
    std::cout.rdbuf(original);
    return captured.str();
}

}  // namespace

TEST_CASE("Q1: cadeia de destruicao derivada -> base", "[q1]") {
    const std::string log =
        capture_destruction_log(std::make_unique<web_search_tool>());

    const auto derived_pos = log.find("~web_search_tool");
    const auto base_pos = log.find("~tool");

    REQUIRE(derived_pos != std::string::npos);  // destrutor da derivada executou
    REQUIRE(base_pos != std::string::npos);     // destrutor da base executou
    REQUIRE(derived_pos < base_pos);            // derivada ANTES da base
}

TEST_CASE("Q2: polimorfismo dinamico via unique_ptr<tool>", "[q2]") {
    std::vector<std::unique_ptr<tool>> tools;
    tools.push_back(std::make_unique<web_search_tool>());
    tools.push_back(std::make_unique<calculator_tool>());

    SECTION("cada derivada responde com sua propria implementacao") {
        REQUIRE(tools[0]->cost_per_call() == Catch::Approx(0.002));
        REQUIRE(tools[1]->cost_per_call() == Catch::Approx(0.0005));
        REQUIRE(tools[0]->execute("tp2").find("[web_search]") == 0);
        REQUIRE(tools[1]->execute("2+2").find("[calculator]") == 0);
    }

    SECTION("funcao livre encontra a ferramenta mais cara via ponteiro base") {
        const tool* most_expensive = most_expensive_tool(tools);
        REQUIRE(most_expensive != nullptr);
        REQUIRE(most_expensive->get_name() == "web_search");

        const std::vector<std::unique_ptr<tool>> empty;
        REQUIRE(most_expensive_tool(empty) == nullptr);
    }
}

TEST_CASE("Q3: interface pura billable por referencia", "[q3]") {
    SECTION("web_search_tool implementa billable (heranca multipla)") {
        web_search_tool search;
        search.execute("primeira busca");
        search.execute("segunda busca");

        const billable& billable_ref = search;  // referência à interface
        REQUIRE(billable_ref.billed_cost() == Catch::Approx(2 * 0.002));
    }

    SECTION("model implementa billable fora da hierarquia de tool") {
        model gpt_model("GPT-4o", 8192, 0.00003);
        gpt_model.register_usage(5000);

        const billable& billable_ref = gpt_model;  // referência à interface
        REQUIRE(billable_ref.billed_cost() == Catch::Approx(5000 * 0.00003));
    }
}
