// Ponto de entrada da GUI Qt (TP3 - Questao 6). Monta o AiAssistant e o
// PersistenceService reaproveitando exatamente a mesma logica de dominio
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

    AiAssistant assistant("Gemini Assistant (GUI)");
    assistant.set_model(std::make_shared<Model>("GPT-4o", 8192, 0.00003));
    assistant.add_tool(std::make_shared<WebSearchTool>());
    assistant.add_tool(std::make_shared<CalculatorTool>());

    JsonFileRepository repository("assistant_snapshot.json");
    PersistenceService persistence(repository);

    MainWindow window(assistant, persistence);
    window.setWindowTitle("Code Cluster — Assistente de IA");
    window.resize(480, 360);
    window.show();

    return application.exec();
}
