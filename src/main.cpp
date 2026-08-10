// Ponto de entrada do sistema de serviço de LLM.
// Demonstra composição, agregação, herança e polimorfismo entre as classes do domínio.

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "user.hpp"
#include "model.hpp"
#include "tool.hpp"
#include "web_search_tool.hpp"
#include "calculator_tool.hpp"
#include "tool_utils.hpp"
#include "billable.hpp"
#include "message.hpp"
#include "session.hpp"
#include "ai_assistant.hpp"
#include "assistant_snapshot.hpp"
#include "catalog.hpp"
#include "concurrent_billing.hpp"
#include "exceptions.hpp"
#include "json_file_repository.hpp"
#include "memory_repository.hpp"
#include "persistence_service.hpp"

using std::cout;
using std::endl;

int main() {
    // Instanciando objetos independentes que serão usados nas demonstrações
    user user1("Eduardo", "eduardo@email.com");
    auto gpt = std::make_shared<model>("GPT-4o", 8192, 0.00003);
    auto search = std::make_shared<web_search_tool>();
    auto calculator = std::make_shared<calculator_tool>();

    // ========================================================================
    // PARTE 1: Demonstrações de Métodos com Lógica
    // ========================================================================
    cout << "=== 1. DEMONSTRACOES DE METODOS COM LOGICA ===" << endl;
    
    // Demonstração lógica - message
    message demo_msg("user", "Ola, como voce funciona?", "2026-05-28 11:00:00");
    cout << "\n[message::format()]\n" << demo_msg.format() << endl;

    // Demonstração lógica - model
    cout << "\n[model::estimate_cost(5000)] Custo: $" << gpt->estimate_cost(5000) << endl;
    
    // Demonstração lógica - tool
    cout << "\n[tool::execute()] " << search->execute("teste") << endl;

    // Configurando um assistente para demonstrar session e ai_assistant
    ai_assistant main_assistant("Gemini Assistant");
    main_assistant.set_model(gpt);
    main_assistant.add_tool(search);
    main_assistant.add_tool(calculator);

    session& main_session = main_assistant.create_session(user1);
    main_session.add_message(demo_msg);

    // Demonstração lógica - session
    cout << "\n[session::summarize()]\n" << main_session.summarize() << endl;
    
    // Demonstração lógica - ai_assistant
    cout << "\n[ai_assistant::status_report()]\n" << main_assistant.status_report() << endl;
    cout << "\n[ai_assistant::list_tools()]\n" << main_assistant.list_tools() << endl;

    // ========================================================================
    // PARTE 2: Teste de Composição
    // ========================================================================
    cout << "\n=== 2. TESTE DE COMPOSICAO ===" << endl;
    cout << "Criando um escopo isolado para um ai_assistant temporario..." << endl;
    {
        ai_assistant temp_assistant("Assistente Temporario");
        session& temp_session = temp_assistant.create_session(user1);
        temp_session.add_message(message("user", "Mensagem descartavel", "Agora"));
        
        cout << ">> Saindo do escopo temporario (Ocorreram as destruicoes em cascata):" << endl;
    } // Destrutor de ai_assistant -> Deleta session -> Destrói message
    cout << "<< Fim do escopo temporario." << endl;

    // ========================================================================
    // PARTE 3: Teste de Agregação
    // ========================================================================
    cout << "\n=== 3. TESTE DE AGREGACAO ===" << endl;
    cout << "Verificando que objetos agregados sobrevivem:" << endl;
    cout << "- Usuario: " << user1.display_info() << " (Sobreviveu)" << endl;
    cout << "- Modelo: " << gpt->get_name() << " (Sobreviveu)" << endl;
    cout << "- Ferramenta: " << search->get_name() << " (Sobreviveu)" << endl;

    // ========================================================================
    // PARTE 4: Teste de Destrutor Virtual (Q1)
    // ========================================================================
    cout << "\n=== 4. TESTE DE DESTRUTOR VIRTUAL ===" << endl;
    cout << "Criando web_search_tool armazenado em ponteiro tool* e deletando:" << endl;
    tool* raw_tool = new web_search_tool();
    delete raw_tool;  // cadeia esperada: ~web_search_tool (derivada) -> ~tool (base)
    cout << "(derivada destruida ANTES da base -> destrutor virtual correto)" << endl;

    // ========================================================================
    // PARTE 5: Polimorfismo Dinamico (Q2)
    // ========================================================================
    cout << "\n=== 5. POLIMORFISMO DINAMICO ===" << endl;
    {
        // (A) vector de unique_ptr para a base com todas as derivadas
        std::vector<std::unique_ptr<tool>> tools;
        tools.push_back(std::make_unique<web_search_tool>());
        tools.push_back(std::make_unique<calculator_tool>());

        // (B) iteração via ponteiro base — despacho virtual chama a derivada certa
        cout << "Iterando via ponteiro base (despacho virtual):" << endl;
        for (const auto& current_tool : tools) {
            cout << "  " << current_tool->describe() << endl;
            cout << "  " << current_tool->execute("consulta polimorfica") << endl;
        }

        // (D) função livre via ponteiro base (não-proprietário)
        const tool* most_expensive = most_expensive_tool(tools);
        if (most_expensive != nullptr) {
            cout << "Ferramenta mais cara: " << most_expensive->get_name()
                 << " ($" << most_expensive->cost_per_call()
                 << " por chamada)" << endl;
        }

        // (C) ao sair do escopo, cada unique_ptr destrói sua derivada -> base
        cout << ">> Saindo do escopo do vetor (destruicao em cadeia):" << endl;
    }
    cout << "<< Vetor destruido sem vazamentos." << endl;

    // ========================================================================
    // PARTE 6: Interface Pura billable (Q3)
    // ========================================================================
    cout << "\n=== 6. INTERFACE PURA (billable) ===" << endl;
    gpt->register_usage(5000);
    cout << "Fatura do modelo " << gpt->get_name()
         << " (5000 tokens usados):" << endl;
    print_bill(*gpt);  // model passado como const billable&
    cout << "Fatura da ferramenta " << search->get_name()
         << " (" << search->get_calls() << " busca(s) realizada(s)):" << endl;
    print_bill(*search);  // web_search_tool passado como const billable&

    // ========================================================================
    // PARTE 7: Programacao Generica - Templates, CRTP, Concepts, Ranges (TP3-Q1)
    // ========================================================================
    cout << "\n=== 7. PROGRAMACAO GENERICA (TP3 - Questao 1) ===" << endl;

    // (A) catalog<T> instanciado com dois tipos diferentes, restringido
    // pelo concept 'nameable' (T precisa expor get_name())
    catalog<model> model_catalog;
    model_catalog.add(*gpt);
    model_catalog.add(model("Claude-Opus", 200000, 0.000015));
    cout << "\n[catalog<model>] " << model_catalog.size() << " modelo(s) cadastrado(s)" << endl;
    auto found_model = model_catalog.find_by_name("Claude-Opus");
    cout << "Busca 'Claude-Opus': " << (found_model.has_value() ? "encontrado" : "nao encontrado") << endl;

    catalog<user> user_catalog;
    user_catalog.add(user1);
    user_catalog.add(user("Ana", "ana@email.com"));
    cout << "[catalog<user>] " << user_catalog.size() << " usuario(s) cadastrado(s)" << endl;
    auto missing_user = user_catalog.find_by_name("Inexistente");
    cout << "Busca 'Inexistente': " << (missing_user.has_value() ? "encontrado" : "nao encontrado") << endl;

    // (B) CRTP - contagem de instancias sem vtable (message/session)
    cout << "\n[CRTP instance_counted] Messages vivas: " << message::alive()
         << " | Sessions vivas: " << session::alive() << endl;
    cout << "[CRTP textual_clonable] clone textual da sessao: "
         << main_session.clone_as_text() << endl;

    // (E) Pipeline de ranges com dois adaptadores encadeados (filter + transform)
    auto cheap_tools = main_assistant.affordable_tool_names(0.001);
    cout << "\n[ranges: filter | transform] Ferramentas ativas ate $0.001/chamada:" << endl;
    for (const auto& name : cheap_tools) {
        cout << "  - " << name << endl;
    }

    // ========================================================================
    // PARTE 8: Tratamento de Erros - excecoes, optional, variant (TP3-Q2)
    // ========================================================================
    cout << "\n=== 8. TRATAMENTO DE ERROS (TP3 - Questao 2) ===" << endl;

    main_assistant.register_available_model(*gpt);

    // (D) try/catch capturando pela classe BASE (llm_service_error)
    cout << "\n[try/catch pela base] selecionando modelo inexistente:" << endl;
    try {
        main_assistant.select_model("Modelo-Fantasma");
    } catch (const llm_service_error& e) {
        cout << "  Capturado (base llm_service_error): " << e.what() << endl;
    }

    cout << "[try/catch pela base] selecionando modelo existente:" << endl;
    try {
        main_assistant.select_model("GPT-4o");
        cout << "  Modelo selecionado com sucesso." << endl;
    } catch (const llm_service_error& e) {
        cout << "  Capturado (base llm_service_error): " << e.what() << endl;
    }

    // (A)(D) segunda excecao especifica do dominio (budget_exceeded_error),
    // tambem capturada pela BASE. Gasto faturado ate aqui: $0.15 do modelo
    // (5000 tokens) + $0.002 da web_search (1 busca) = $0.152
    cout << "[try/catch pela base] verificando orcamento:" << endl;
    try {
        main_assistant.enforce_budget(1.00);  // dentro do limite -> nao lanca
        cout << "  Orcamento de $1.00 respeitado." << endl;
        main_assistant.enforce_budget(0.10);  // estoura -> budget_exceeded_error
    } catch (const llm_service_error& e) {
        cout << "  Capturado (base llm_service_error): " << e.what() << endl;
    }

    // (D) optional nos dois casos: achou / nao achou
    auto model_found = main_assistant.find_available_model("GPT-4o");
    cout << "\n[optional] find_available_model(\"GPT-4o\"): "
         << (model_found.has_value() ? "achou (" + model_found->get_name() + ")" : "nao achou") << endl;
    auto model_missing = main_assistant.find_available_model("Modelo-Fantasma");
    cout << "[optional] find_available_model(\"Modelo-Fantasma\"): "
         << (model_missing.has_value() ? "achou" : "nao achou") << endl;

    // (D) variant: sucesso (tool_output) ou erro (string)
    cout << "\n[variant] run_tool(\"calculator\", ...):" << endl;
    auto outcome_ok = main_assistant.run_tool("calculator", "2 + 2");
    std::visit([](const auto& value) {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, ai_assistant::tool_output>) {
            cout << "  Sucesso: " << value.content << " (custo $" << value.cost << ")" << endl;
        } else {
            cout << "  Erro: " << value << endl;
        }
    }, outcome_ok);

    cout << "[variant] run_tool(\"ferramenta_inexistente\", ...):" << endl;
    auto outcome_err = main_assistant.run_tool("ferramenta_inexistente", "input");
    std::visit([](const auto& value) {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, ai_assistant::tool_output>) {
            cout << "  Sucesso: " << value.content << " (custo $" << value.cost << ")" << endl;
        } else {
            cout << "  Erro: " << value << endl;
        }
    }, outcome_err);

    // ========================================================================
    // PARTE 9: STL e Concorrencia (TP3-Q3)
    // ========================================================================
    cout << "\n=== 9. STL E CONCORRENCIA (TP3 - Questao 3) ===" << endl;

    std::vector<std::shared_ptr<tool>> demo_tools{search, calculator};

    // (A) containers apropriados: std::map (indice por nome, ordenado)
    auto tools_by_name = index_tools_by_name(demo_tools);
    cout << "\n[std::map<string, shared_ptr<tool>>] indice por nome:" << endl;
    for (const auto& [tool_name, current_tool] : tools_by_name) {
        cout << "  " << tool_name << " -> $" << current_tool->cost_per_call() << "/chamada" << endl;
    }

    // (A) segundo container: std::unordered_set (papeis distintos da sessao)
    auto roles = main_session.distinct_roles();
    cout << "[std::unordered_set<string>] papeis distintos na sessao: " << roles.size() << endl;

    // (B) algoritmos + lambda com captura
    sort_tools_by_cost(demo_tools);
    cout << "\n[std::sort] ferramentas ordenadas por custo:" << endl;
    for (const auto& current_tool : demo_tools) {
        cout << "  " << current_tool->get_name() << ": $" << current_tool->cost_per_call() << endl;
    }
    double cost_threshold = 0.001;
    auto above_threshold = count_tools_above_cost(demo_tools, cost_threshold);
    cout << "[std::count_if, lambda com captura de threshold=$" << cost_threshold << "] "
         << above_threshold << " ferramenta(s) acima do limite" << endl;
    cout << "[std::accumulate] custo total por chamada: $" << total_cost_per_call(demo_tools) << endl;
    const tool* found_by_name = find_tool_by_name(demo_tools, "calculator");
    cout << "[std::find_if] busca por 'calculator': "
         << (found_by_name != nullptr ? "encontrada" : "nao encontrada") << endl;

    // (C)(D) paralelizacao de operacao independente com std::async, estado
    // compartilhado (audit_trail) protegido por std::mutex
    std::vector<int> token_batches{1000, 2500, 500, 4000};
    std::vector<cost_estimate> audit_trail;
    auto parallel_estimates = estimate_batches_parallel(*gpt, token_batches, audit_trail);
    double parallel_total = std::accumulate(parallel_estimates.begin(), parallel_estimates.end(), 0.0,
        [](double acc, const cost_estimate& estimate) { return acc + estimate.cost; });
    cout << "\n[std::async + std::mutex] " << parallel_estimates.size()
         << " lote(s) estimados em paralelo; trilha de auditoria com "
         << audit_trail.size() << " entrada(s); custo total $" << parallel_total << endl;

    // ========================================================================
    // PARTE 10: Serializacao JSON e DIP (TP3-Q4)
    // ========================================================================
    cout << "\n=== 10. SERIALIZACAO JSON E SOLID (TP3 - Questao 4) ===" << endl;

    json_file_repository json_repository("assistant_snapshot.json");
    persistence_service persistence(json_repository);

    persistence.save(main_assistant);
    cout << "\n[persistence_service::save] estado salvo em assistant_snapshot.json" << endl;

    assistant_snapshot restored = persistence.load();
    cout << "[persistence_service::load] assistente restaurado: " << restored.assistant_name
         << " | modelo: " << restored.assistant_model.get_name()
         << " | ferramentas: " << restored.tools.size() << endl;

    assistant_snapshot original{
        main_assistant.get_name(),
        *main_assistant.get_model(),
        main_assistant.get_tools()
    };
    cout << "[round-trip] snapshot original == snapshot restaurado: "
         << (original == restored ? "sim" : "nao") << endl;

    // (D) DIP: mesma logica (persistence_service) com a implementacao de
    // teste (memory_repository), sem tocar disco
    memory_repository in_memory_repository;
    persistence_service memory_persistence(in_memory_repository);
    memory_persistence.save(main_assistant);
    assistant_snapshot restored_from_memory = memory_persistence.load();
    cout << "[DIP: memory_repository, sem I/O] assistente restaurado: "
         << restored_from_memory.assistant_name
         << " | igual ao original: " << (original == restored_from_memory ? "sim" : "nao") << endl;

    cout << "\n[Encerrando o programa (main_assistant sera destruido agora)]" << endl;
    return 0;
}
