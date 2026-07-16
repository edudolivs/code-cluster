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
#include "message.hpp"
#include "session.hpp"
#include "ai_assistant.hpp"

using std::cout;
using std::endl;

int main() {
    // Instanciando objetos independentes que serão usados nas demonstrações
    User user1("Eduardo", "eduardo@email.com");
    auto gpt = std::make_shared<Model>("GPT-4o", 8192, 0.00003);
    auto search = std::make_shared<WebSearchTool>();
    auto calculator = std::make_shared<CalculatorTool>();

    // ========================================================================
    // PARTE 1: Demonstrações de Métodos com Lógica
    // ========================================================================
    cout << "=== 1. DEMONSTRACOES DE METODOS COM LOGICA ===" << endl;
    
    // Demonstração lógica - Message
    Message demo_msg("user", "Ola, como voce funciona?", "2026-05-28 11:00:00");
    cout << "\n[Message::format()]\n" << demo_msg.format() << endl;

    // Demonstração lógica - Model
    cout << "\n[Model::estimate_cost(5000)] Custo: $" << gpt->estimate_cost(5000) << endl;
    
    // Demonstração lógica - Tool
    cout << "\n[Tool::execute()] " << search->execute("teste") << endl;

    // Configurando um assistente para demonstrar Session e AiAssistant
    AiAssistant main_assistant("Gemini Assistant");
    main_assistant.set_model(gpt);
    main_assistant.add_tool(search);
    main_assistant.add_tool(calculator);

    Session* main_session = main_assistant.create_session(user1);
    main_session->add_message(demo_msg);

    // Demonstração lógica - Session
    cout << "\n[Session::summarize()]\n" << main_session->summarize() << endl;
    
    // Demonstração lógica - AiAssistant
    cout << "\n[AiAssistant::status_report()]\n" << main_assistant.status_report() << endl;
    cout << "\n[AiAssistant::list_tools()]\n" << main_assistant.list_tools() << endl;

    // ========================================================================
    // PARTE 2: Teste de Composição
    // ========================================================================
    cout << "\n=== 2. TESTE DE COMPOSICAO ===" << endl;
    cout << "Criando um escopo isolado para um AiAssistant temporario..." << endl;
    {
        AiAssistant temp_assistant("Assistente Temporario");
        Session* temp_session = temp_assistant.create_session(user1);
        temp_session->add_message(Message("user", "Mensagem descartavel", "Agora"));
        
        cout << ">> Saindo do escopo temporario (Ocorreram as destruicoes em cascata):" << endl;
    } // Destrutor de AiAssistant -> Deleta Session -> Destrói Message
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
    cout << "Criando WebSearchTool armazenado em ponteiro Tool* e deletando:" << endl;
    Tool* raw_tool = new WebSearchTool();
    delete raw_tool;  // cadeia esperada: ~WebSearchTool (derivada) -> ~Tool (base)
    cout << "(derivada destruida ANTES da base -> destrutor virtual correto)" << endl;

    // ========================================================================
    // PARTE 5: Polimorfismo Dinamico (Q2)
    // ========================================================================
    cout << "\n=== 5. POLIMORFISMO DINAMICO ===" << endl;
    {
        // (A) vector de unique_ptr para a base com todas as derivadas
        std::vector<std::unique_ptr<Tool>> tools;
        tools.push_back(std::make_unique<WebSearchTool>());
        tools.push_back(std::make_unique<CalculatorTool>());

        // (B) iteração via ponteiro base — despacho virtual chama a derivada certa
        cout << "Iterando via ponteiro base (despacho virtual):" << endl;
        for (const auto& tool : tools) {
            cout << "  " << tool->describe() << endl;
            cout << "  " << tool->execute("consulta polimorfica") << endl;
        }

        // (D) função livre via ponteiro base (não-proprietário)
        const Tool* most_expensive = most_expensive_tool(tools);
        if (most_expensive != nullptr) {
            cout << "Ferramenta mais cara: " << most_expensive->get_name()
                 << " ($" << most_expensive->cost_per_call()
                 << " por chamada)" << endl;
        }

        // (C) ao sair do escopo, cada unique_ptr destrói sua derivada -> base
        cout << ">> Saindo do escopo do vetor (destruicao em cadeia):" << endl;
    }
    cout << "<< Vetor destruido sem vazamentos." << endl;

    cout << "\n[Encerrando o programa (main_assistant sera destruido agora)]" << endl;
    return 0;
}
