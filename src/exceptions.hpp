// Hierarquia de exceções própria do domínio. Toda exceção lançada pelo
// ai_assistant herda de llm_service_error, permitindo captura genérica pela
// base ou tratamento específico por tipo derivado.

#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <stdexcept>
#include <string>

// Exceção base do domínio — qualquer erro do serviço de LLM deriva daqui.
class llm_service_error : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

// Lançada quando se tenta selecionar um modelo que não está cadastrado
// no catálogo de modelos disponíveis do assistente.
class model_not_found_error : public llm_service_error {
public:
    explicit model_not_found_error(const std::string& model_name)
        : llm_service_error("modelo nao encontrado: " + model_name) {}
};

// Lançada quando o custo total faturado (modelo + ferramentas) ultrapassa
// o limite de orçamento definido.
class budget_exceeded_error : public llm_service_error {
public:
    budget_exceeded_error(double spent, double limit)
        : llm_service_error("orcamento excedido: gasto $" + std::to_string(spent)
                           + " > limite $" + std::to_string(limit)) {}
};

#endif // EXCEPTIONS_HPP
