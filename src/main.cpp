// Ponto de entrada do sistema de serviço de LLM.
// Instancia todas as classes e demonstra os métodos com lógica real.

#include <iostream>
#include <string>

#include "user.hpp"
#include "model.hpp"
#include "tool.hpp"
#include "message.hpp"
#include "session.hpp"
#include "ai_assistant.hpp"

int main() {
    std::cout << "============================================" << std::endl;
    std::cout << "   Sistema de Servico de LLM" << std::endl;
    std::cout << "============================================\n" << std::endl;

    // --- Criação de objetos ---

    // Cria usuários
    User user1("Eduardo", "eduardo@email.com");
    User user2("Maria", "maria@email.com");

    std::cout << ">> Usuarios criados:" << std::endl;
    std::cout << "   " << user1.display_info() << std::endl;
    std::cout << "   " << user2.display_info() << std::endl;
    std::cout << std::endl;

    // Cria modelo de linguagem
    Model gpt("GPT-4o", 8192, 0.00003);

    std::cout << ">> Modelo configurado:" << std::endl;
    std::cout << "   Nome: " << gpt.get_name() << std::endl;
    std::cout << "   Max tokens: " << gpt.get_max_tokens() << std::endl;
    std::cout << "   Custo estimado (5000 tokens): $"
              << gpt.estimate_cost(5000) << std::endl;
    std::cout << "   Custo estimado (10000 tokens, limitado a 8192): $"
              << gpt.estimate_cost(10000) << std::endl;
    std::cout << std::endl;

    // Cria ferramentas
    Tool search("web_search", "Busca informacoes na internet");
    Tool calculator("calculator", "Realiza calculos matematicos");
    Tool code_runner("code_runner", "Executa trechos de codigo", false);

    std::cout << ">> Ferramentas criadas:" << std::endl;
    std::cout << "   " << search.execute("clima hoje") << std::endl;
    std::cout << "   " << calculator.execute("2 + 2") << std::endl;
    std::cout << "   " << code_runner.execute("print('hello')") << std::endl;

    // Demonstra toggle da ferramenta desabilitada
    code_runner.toggle();
    std::cout << "   (apos toggle) " << code_runner.execute("print('hello')") << std::endl;
    std::cout << std::endl;

    // Cria mensagens
    Message msg1("user", "Ola, como voce funciona?", "2026-05-28 11:00:00");
    Message msg2("assistant", "Sou um assistente baseado em LLM!", "2026-05-28 11:00:01");
    Message msg3("user", "Qual a previsao do tempo?", "2026-05-28 11:01:00");

    std::cout << ">> Mensagens formatadas:" << std::endl;
    std::cout << "   " << msg1.format() << std::endl;
    std::cout << "   " << msg2.format() << std::endl;
    std::cout << "   " << msg3.format() << std::endl;
    std::cout << std::endl;

    // --- Monta o assistente de IA ---
    // O bloco abaixo usa escopo para demonstrar o destrutor do AiAssistant
    {
        AiAssistant assistant("Gemini Assistant");
        assistant.set_model(&gpt);
        assistant.add_tool(&search);
        assistant.add_tool(&calculator);
        assistant.add_tool(&code_runner);

        std::cout << ">> Lista de ferramentas do assistente:" << std::endl;
        std::cout << assistant.list_tools() << std::endl;
        std::cout << std::endl;

        // Cria sessões e adiciona mensagens
        Session* session1 = assistant.create_session(&user1);
        session1->add_message(msg1);
        session1->add_message(msg2);
        session1->add_message(msg3);

        Session* session2 = assistant.create_session(&user2);
        session2->add_message(Message("user", "Oi!", "2026-05-28 12:00:00"));

        std::cout << ">> Resumo das sessoes:" << std::endl;
        std::cout << "   " << session1->summarize() << std::endl;
        std::cout << "   " << session2->summarize() << std::endl;
        std::cout << std::endl;

        // Relatório final do assistente
        std::cout << assistant.status_report() << std::endl;
        std::cout << std::endl;

        std::cout << ">> Encerrando assistente (destrutor sera chamado)...\n" << std::endl;
    } // Ao sair do escopo, ~AiAssistant é chamado, que chama ~Session para cada sessão

    std::cout << "============================================" << std::endl;
    std::cout << "   Programa finalizado com sucesso." << std::endl;
    std::cout << "============================================" << std::endl;

    return 0;
}
