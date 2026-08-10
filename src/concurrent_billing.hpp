// Estimativa de custo de múltiplos lotes de tokens em paralelo. Cada lote
// é independente dos demais (Model::estimate_cost é const e não compartilha
// estado entre chamadas), então o processamento pode ser paralelizado com
// segurança; o único estado compartilhado (a trilha de auditoria) é
// protegido por mutex.

#ifndef CONCURRENT_BILLING_HPP
#define CONCURRENT_BILLING_HPP

#include <future>
#include <mutex>
#include <vector>

#include "model.hpp"

// Estimativa de custo de um lote específico de tokens.
struct CostEstimate {
    int tokens;
    double cost;
};

// Estima o custo de cada lote de 'token_batches' em paralelo (um
// std::async por lote) e registra cada estimativa concluída em
// 'audit_trail', protegido por mutex contra corrida entre threads.
inline std::vector<CostEstimate> estimate_batches_parallel(
        const Model& model,
        const std::vector<int>& token_batches,
        std::vector<CostEstimate>& audit_trail) {
    std::mutex audit_mutex;
    std::vector<std::future<CostEstimate>> futures;

    for (int tokens : token_batches) {
        futures.push_back(std::async(std::launch::async, [&model, &audit_trail, &audit_mutex, tokens] {
            CostEstimate estimate{tokens, model.estimate_cost(tokens)};
            {
                std::lock_guard<std::mutex> lock(audit_mutex);
                audit_trail.push_back(estimate);
            }
            return estimate;
        }));
    }

    std::vector<CostEstimate> results;
    results.reserve(futures.size());
    for (auto& future : futures) {
        results.push_back(future.get());
    }
    return results;
}

#endif // CONCURRENT_BILLING_HPP
