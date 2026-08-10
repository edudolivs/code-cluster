// Janela principal da GUI (TP3 - Questao 6). Camada fina: nenhuma regra de
// negocio aqui — cada slot apenas chama metodos ja existentes de
// AiAssistant/PersistenceService. A logica de dominio continua testavel
// sem GUI (ver tests/test_tp3.cpp), respeitando o DIP da Questao 4.

#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include <memory>

#include "ai_assistant.hpp"
#include "calculator_tool.hpp"
#include "exceptions.hpp"
#include "persistence_service.hpp"
#include "web_search_tool.hpp"

class MainWindow : public QWidget {
    Q_OBJECT

public:
    MainWindow(AiAssistant& assistant, PersistenceService& persistence, QWidget* parent = nullptr)
        : QWidget(parent), assistant_(assistant), persistence_(persistence) {
        tool_list_ = new QListWidget(this);
        status_label_ = new QLabel("Pronto.", this);

        auto* add_web_search_button = new QPushButton("Adicionar WebSearchTool", this);
        auto* add_calculator_button = new QPushButton("Adicionar CalculatorTool", this);
        auto* remove_button = new QPushButton("Remover selecionada", this);
        auto* execute_button = new QPushButton("Executar selecionada", this);
        auto* save_button = new QPushButton("Salvar", this);
        auto* load_button = new QPushButton("Carregar", this);

        auto* button_row = new QHBoxLayout();
        button_row->addWidget(add_web_search_button);
        button_row->addWidget(add_calculator_button);
        button_row->addWidget(remove_button);
        button_row->addWidget(execute_button);
        button_row->addWidget(save_button);
        button_row->addWidget(load_button);

        auto* layout = new QVBoxLayout(this);
        layout->addWidget(tool_list_);
        layout->addLayout(button_row);
        layout->addWidget(status_label_);

        connect(add_web_search_button, &QPushButton::clicked, this, &MainWindow::add_web_search_tool);
        connect(add_calculator_button, &QPushButton::clicked, this, &MainWindow::add_calculator_tool);
        connect(remove_button, &QPushButton::clicked, this, &MainWindow::remove_selected_tool);
        connect(execute_button, &QPushButton::clicked, this, &MainWindow::execute_selected_tool);
        connect(save_button, &QPushButton::clicked, this, &MainWindow::save_state);
        connect(load_button, &QPushButton::clicked, this, &MainWindow::load_state);

        refresh_tool_list();
    }

private slots:
    // Reconstrói a lista exibida a partir do estado atual do assistente
    void refresh_tool_list() {
        tool_list_->clear();
        for (const auto& tool : assistant_.get_tools()) {
            tool_list_->addItem(QString::fromStdString(tool->describe()));
        }
    }

    void add_web_search_tool() {
        assistant_.add_tool(std::make_shared<WebSearchTool>());
        refresh_tool_list();
        status_label_->setText("WebSearchTool adicionada.");
    }

    void add_calculator_tool() {
        assistant_.add_tool(std::make_shared<CalculatorTool>());
        refresh_tool_list();
        status_label_->setText("CalculatorTool adicionada.");
    }

    void remove_selected_tool() {
        const QListWidgetItem* selected = tool_list_->currentItem();
        if (selected == nullptr) {
            status_label_->setText("Nenhuma ferramenta selecionada.");
            return;
        }
        const std::string tool_name = selected_tool_name();
        const bool removed = assistant_.remove_tool(tool_name);
        refresh_tool_list();
        status_label_->setText(removed
            ? QString::fromStdString("Removida: " + tool_name)
            : "Nao foi possivel remover a ferramenta selecionada.");
    }

    void execute_selected_tool() {
        if (tool_list_->currentItem() == nullptr) {
            status_label_->setText("Nenhuma ferramenta selecionada.");
            return;
        }
        const std::string tool_name = selected_tool_name();
        auto outcome = assistant_.run_tool(tool_name, "entrada da GUI");
        std::visit([this](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, AiAssistant::ToolOutput>) {
                status_label_->setText(QString::fromStdString(value.content));
            } else {
                status_label_->setText(QString::fromStdString(value));
            }
        }, outcome);
    }

    void save_state() {
        try {
            persistence_.save(assistant_);
            status_label_->setText("Estado salvo com sucesso.");
        } catch (const LlmServiceError& error) {
            status_label_->setText(QString::fromStdString(std::string("Erro ao salvar: ") + error.what()));
        }
    }

    void load_state() {
        try {
            AssistantSnapshot snapshot = persistence_.load();
            assistant_.set_model(std::make_shared<Model>(snapshot.model));
            for (const auto& tool_name : tool_names_currently_loaded()) {
                assistant_.remove_tool(tool_name);
            }
            for (const auto& tool : snapshot.tools) {
                assistant_.add_tool(tool);
            }
            refresh_tool_list();
            status_label_->setText("Estado carregado com sucesso.");
        } catch (const LlmServiceError& error) {
            status_label_->setText(QString::fromStdString(std::string("Erro ao carregar: ") + error.what()));
        }
    }

private:
    AiAssistant& assistant_;
    PersistenceService& persistence_;
    QListWidget* tool_list_;
    QLabel* status_label_;

    // Extrai o nome da ferramenta a partir do texto de describe() exibido
    // no item selecionado (formato "[nome] ...").
    std::string selected_tool_name() const {
        const std::string text = tool_list_->currentItem()->text().toStdString();
        const auto start = text.find('[');
        const auto end = text.find(']');
        if (start == std::string::npos || end == std::string::npos) {
            return "";
        }
        return text.substr(start + 1, end - start - 1);
    }

    std::vector<std::string> tool_names_currently_loaded() const {
        std::vector<std::string> names;
        for (const auto& tool : assistant_.get_tools()) {
            names.push_back(tool->get_name());
        }
        return names;
    }
};

#endif // MAIN_WINDOW_HPP
