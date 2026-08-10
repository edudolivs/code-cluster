// Ponto de entrada da GUI Qt (TP3 - Questao 6). Monta o ai_assistant e o
// persistence_service reaproveitando exatamente a mesma logica de dominio
// usada em main.cpp e nos testes — a janela e so uma camada fina por cima.

#include <QApplication>

#include <memory>

#include "ai_assistant.hpp"
#include "calculator_tool.hpp"
#include "json_file_repository.hpp"
#include "main_window.hpp"
#include "model.hpp"
#include "persistence_service.hpp"
#include "web_search_tool.hpp"

int main(int argc, char** argv) {
    QApplication application(argc, argv);

    ai_assistant assistant("Gemini Assistant (GUI)");
    assistant.set_model(std::make_shared<model>("GPT-4o", 8192, 0.00003));
    assistant.add_tool(std::make_shared<web_search_tool>());
    assistant.add_tool(std::make_shared<calculator_tool>());

    json_file_repository repository("assistant_snapshot.json");
    persistence_service persistence(repository);

    main_window window(assistant, persistence);
    window.setWindowTitle("Code Cluster — Assistente de IA");
    window.resize(480, 360);
    window.show();

    return application.exec();
}
