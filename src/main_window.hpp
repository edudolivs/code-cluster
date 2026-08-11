// Janela principal da GUI (TP3 - Questao 6). Camada fina: nenhuma regra de
// negocio aqui — cada slot apenas chama metodos ja existentes de
// ai_assistant/persistence_service. A logica de dominio continua testavel
// sem GUI (ver tests/test_tp3.cpp), respeitando o DIP da Questao 4.

#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include <memory>

#include "ai_assistant.hpp"
#include "calculator_tool.hpp"
#include "exceptions.hpp"
#include "persistence_service.hpp"
#include "web_search_tool.hpp"

class main_window : public QWidget {
    Q_OBJECT

public:
    main_window(ai_assistant& assistant, persistence_service& persistence, QWidget* parent = nullptr)
        : QWidget(parent), assistant_(assistant), persistence_(persistence) {
        tool_list_ = new QListWidget(this);
        status_label_ = new QLabel("Pronto.", this);

        auto* add_web_search_button = new QPushButton("Adicionar web_search_tool", this);
        auto* add_calculator_button = new QPushButton("Adicionar calculator_tool", this);
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

        connect(add_web_search_button, &QPushButton::clicked, this, &main_window::add_web_search_tool);
        connect(add_calculator_button, &QPushButton::clicked, this, &main_window::add_calculator_tool);
        connect(remove_button, &QPushButton::clicked, this, &main_window::remove_selected_tool);
        connect(execute_button, &QPushButton::clicked, this, &main_window::execute_selected_tool);
        connect(save_button, &QPushButton::clicked, this, &main_window::save_state);
        connect(load_button, &QPushButton::clicked, this, &main_window::load_state);

        refresh_tool_list();
    }

private slots:
    // Reconstrói a lista exibida a partir do estado atual do assistente,
    // preservando a linha selecionada — sem isso, atualizar a lista após
    // executar apagaria a seleção e o usuário teria de reselecionar a cada
    // clique.
    void refresh_tool_list() {
        const int previous_row = tool_list_->currentRow();
        tool_list_->clear();
        for (const auto& current_tool : assistant_.get_tools()) {
            tool_list_->addItem(QString::fromStdString(current_tool->describe()));
        }
        if (previous_row >= 0 && previous_row < tool_list_->count()) {
            tool_list_->setCurrentRow(previous_row);
        }
    }

    void add_web_search_tool() {
        assistant_.add_tool(std::make_shared<web_search_tool>());
        refresh_tool_list();
        status_label_->setText("web_search_tool adicionada.");
    }

    void add_calculator_tool() {
        assistant_.add_tool(std::make_shared<calculator_tool>());
        refresh_tool_list();
        status_label_->setText("calculator_tool adicionada.");
    }

    void remove_selected_tool() {
        const int row = tool_list_->currentRow();
        if (row < 0) {
            status_label_->setText("Nenhuma ferramenta selecionada.");
            return;
        }
        const auto index = static_cast<std::size_t>(row);
        const std::string tool_name = assistant_.get_tools()[index]->get_name();
        const bool removed = assistant_.remove_tool_at(index);
        refresh_tool_list();
        status_label_->setText(removed
            ? QString::fromStdString("Removida: " + tool_name)
            : "Nao foi possivel remover a ferramenta selecionada.");
    }

    void execute_selected_tool() {
        const int row = tool_list_->currentRow();
        if (row < 0) {
            status_label_->setText("Nenhuma ferramenta selecionada.");
            return;
        }
        auto outcome = assistant_.run_tool_at(static_cast<std::size_t>(row), "entrada da GUI");
        std::visit([this](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, ai_assistant::tool_output>) {
                status_label_->setText(QString::fromStdString(value.content));
            } else {
                status_label_->setText(QString::fromStdString(value));
            }
        }, outcome);
        // Reexibe a lista para que describe() mostre o estado novo da
        // ferramenta (ex.: "buscas realizadas" da web_search_tool).
        refresh_tool_list();
    }

    void save_state() {
        try {
            persistence_.save(assistant_);
            status_label_->setText("Estado salvo com sucesso.");
        } catch (const llm_service_error& error) {
            status_label_->setText(QString::fromStdString(std::string("Erro ao salvar: ") + error.what()));
        }
    }

    void load_state() {
        try {
            assistant_snapshot snapshot = persistence_.load();
            assistant_.set_model(std::make_shared<model>(snapshot.assistant_model));
            assistant_.clear_tools();
            for (const auto& current_tool : snapshot.tools) {
                assistant_.add_tool(current_tool);
            }
            refresh_tool_list();
            status_label_->setText("Estado carregado com sucesso.");
        } catch (const llm_service_error& error) {
            status_label_->setText(QString::fromStdString(std::string("Erro ao carregar: ") + error.what()));
        }
    }

private:
    ai_assistant& assistant_;
    persistence_service& persistence_;
    QListWidget* tool_list_;
    QLabel* status_label_;
};

#endif // MAIN_WINDOW_HPP
