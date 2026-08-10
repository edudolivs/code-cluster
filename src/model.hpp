// Classe que representa um modelo de linguagem (LLM).
// Implementa a interface Billable: armazena nome, limite de tokens,
// custo por token e tokens usados. Fornece metodos para estimar custo
// e registrar uso de tokens.

#ifndef MODEL_HPP
#define MODEL_HPP

#include <string>

#include "billable.hpp"

class Model : public Billable {
private:
    std::string name_;       // nome do modelo
    int max_tokens_;         // limite máximo de tokens
    double cost_per_token_;  // custo por token em dólares
    int tokens_used_ = 0;    // total de tokens processados

public:
    // Construtor padrão — necessário para o padrão to_json/from_json do
    // nlohmann::json, que constrói um valor vazio antes de populá-lo
    // (TP3-Q4).
    Model() : name_(""), max_tokens_(0), cost_per_token_(0.0) {}

    // Construtor com lista de inicialização
    Model(const std::string& name, int max_tokens, double cost_per_token)
        : name_(name), max_tokens_(max_tokens), cost_per_token_(cost_per_token) {}

    // Getters const
    std::string get_name() const { return name_; }
    int get_max_tokens() const { return max_tokens_; }
    double get_cost_per_token() const { return cost_per_token_; }
    int get_tokens_used() const { return tokens_used_; }

    // Calcula o custo estimado para uma quantidade de tokens,
    // limitando ao máximo permitido pelo modelo
    double estimate_cost(int tokens) const {
        int effective_tokens = tokens;
        if (effective_tokens > max_tokens_) {
            effective_tokens = max_tokens_;
        }
        if (effective_tokens < 0) {
            effective_tokens = 0;
        }
        return effective_tokens * cost_per_token_;
    }

    // Registra tokens consumidos pelo modelo (ignora valores não-positivos)
    void register_usage(int tokens) {
        if (tokens > 0) {
            tokens_used_ += tokens;
        }
    }

    // Override da interface Billable — custo total dos tokens consumidos
    double billed_cost() const override {
        return tokens_used_ * cost_per_token_;
    }

    // Igualdade estrutural — usada para validar round-trip de serializacao
    // (TP3-Q4) sem precisar reserializar para comparar.
    bool operator==(const Model& other) const {
        return name_ == other.name_
            && max_tokens_ == other.max_tokens_
            && cost_per_token_ == other.cost_per_token_
            && tokens_used_ == other.tokens_used_;
    }
};

#endif // MODEL_HPP
