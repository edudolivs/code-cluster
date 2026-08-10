// Mixins CRTP (Curiously Recurring Template Pattern): adicionam
// comportamento estático a uma família de classes sem custo de vtable
// (diferente da hierarquia polimórfica de Tool, que usa despacho virtual).

#ifndef CRTP_MIXINS_HPP
#define CRTP_MIXINS_HPP

#include <string>

// Mixin que conta quantas instâncias de Derived estão vivas no momento.
// O contador é um membro estático por instanciação do template, ou seja,
// cada classe derivada tem sua própria contagem independente.
template <typename Derived>
class InstanceCounted {
protected:
    InstanceCounted() { ++count_; }
    InstanceCounted(const InstanceCounted&) { ++count_; }
    InstanceCounted(InstanceCounted&&) noexcept { ++count_; }
    ~InstanceCounted() { --count_; }

    // Atribuição não cria nem destrói instâncias, então não altera a contagem
    InstanceCounted& operator=(const InstanceCounted&) = default;
    InstanceCounted& operator=(InstanceCounted&&) noexcept = default;

public:
    static int alive() { return count_; }

private:
    static inline int count_ = 0;
};

// Mixin que exige da derivada um método to_text() e expõe um "clone
// textual" padronizado sobre ele, resolvido em tempo de compilação
// (static_cast para Derived, sem vtable).
template <typename Derived>
class TextualClonable {
public:
    std::string clone_as_text() const {
        return static_cast<const Derived&>(*this).to_text();
    }
};

#endif // CRTP_MIXINS_HPP
