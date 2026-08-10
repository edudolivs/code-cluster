// Hierarquia de exceções própria do domínio. Toda exceção lançada pelo
// AiAssistant herda de LlmServiceError, permitindo captura genérica pela
// base ou tratamento específico por tipo derivado.

#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <stdexcept>
#include <string>

// Exceção base do domínio — qualquer erro do serviço de LLM deriva daqui.
class LlmServiceError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

// Lançada quando se tenta selecionar um modelo que não está cadastrado
// no catálogo de modelos disponíveis do assistente.
class ModelNotFoundError : public LlmServiceError {
public:
    explicit ModelNotFoundError(const std::string& model_name)
        : LlmServiceError("modelo nao encontrado: " + model_name) {}
};

// Lançada quando o custo total faturado (modelo + ferramentas) ultrapassa
// o limite de orçamento definido.
class BudgetExceededError : public LlmServiceError {
public:
    BudgetExceededError(double spent, double limit)
        : LlmServiceError("orcamento excedido: gasto $" + std::to_string(spent)
                           + " > limite $" + std::to_string(limit)) {}
};

#endif // EXCEPTIONS_HPP
