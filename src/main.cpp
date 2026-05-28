// Ponto de entrada do sistema de serviço de LLM.
// Demonstra composição e agregação entre as classes do domínio.

#include <iostream>
#include <string>

#include "user.hpp"
#include "model.hpp"
#include "tool.hpp"
#include "message.hpp"
#include "session.hpp"
#include "ai_assistant.hpp"

int main() {
    // Objetos independentes — existem fora do assistente (agregação)
    User user1("Eduardo", "eduardo@email.com");
    Model gpt("GPT-4o", 8192, 0.00003);
    Tool search("web_search", "Busca informacoes na internet");
    Tool calculator("calculator", "Realiza calculos matematicos");

    // --- Composição: AiAssistant cria e destrói Sessions ---
    std::cout << "--- COMPOSICAO ---" << std::endl;
    {
        AiAssistant assistant("Gemini Assistant");
        assistant.set_model(&gpt);
        assistant.add_tool(&search);
        assistant.add_tool(&calculator);

        Session* s1 = assistant.create_session(&user1);
        s1->add_message(Message("user", "Ola, como voce funciona?", "2026-05-28 11:00:00"));
        s1->add_message(Message("assistant", "Sou um assistente de IA!", "2026-05-28 11:00:01"));

        std::cout << assistant.status_report() << std::endl;
    } // ~AiAssistant → deleta Sessions → ~Session imprime destruição

    // --- Agregação: objetos sobrevivem à destruição do assistente ---
    std::cout << "\n--- AGREGACAO ---" << std::endl;
    std::cout << user1.display_info() << " (sobreviveu)" << std::endl;
    std::cout << "Modelo: " << gpt.get_name() << " (sobreviveu)" << std::endl;
    std::cout << search.execute("teste") << " (sobreviveu)" << std::endl;

    return 0;
}
