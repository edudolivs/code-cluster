// Template genérico de catálogo nomeado, reutilizável para qualquer tipo
// do domínio que exponha get_name(). Restringido pelo concept 'nameable'
// (C++20), que garante erros de compilação claros ao ser violado.

#ifndef CATALOG_HPP
#define CATALOG_HPP

#include <concepts>
#include <optional>
#include <string>
#include <vector>

// Concept que exige um método get_name() convertível para std::string.
template <typename T>
concept nameable = requires(const T& value) {
    { value.get_name() } -> std::convertible_to<std::string>;
};

// Catálogo genérico: guarda itens nomeáveis e permite busca por nome.
// Não é um simples apelido para std::vector — adiciona a semântica de
// "registro pesquisável por nome" sobre qualquer tipo do domínio.
template <nameable T>
class Catalog {
private:
    std::vector<T> items_;  // itens armazenados, na ordem de inserção

public:
    // Adiciona um item ao catálogo
    void add(T item) {
        items_.push_back(std::move(item));
    }

    // Busca um item pelo nome; retorna nullopt se não encontrado
    std::optional<T> find_by_name(const std::string& name) const {
        for (const auto& item : items_) {
            if (item.get_name() == name) {
                return item;
            }
        }
        return std::nullopt;
    }

    // Getters const
    const std::vector<T>& all() const { return items_; }
    std::size_t size() const { return items_.size(); }
};

#endif // CATALOG_HPP
