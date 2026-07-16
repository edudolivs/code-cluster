// Interface pura: capacidade de gerar custo faturavel.
// Sem atributos de dado — apenas contrato. Qualquer classe do dominio
// pode implementá-la, independentemente da hierarquia principal.

#ifndef BILLABLE_HPP
#define BILLABLE_HPP

#include <iostream>

class Billable {
public:
    // Custo total faturado em dolares
    virtual double billed_cost() const = 0;

    // Destrutor virtual obrigatorio em interfaces
    virtual ~Billable() = default;
};

// Funcao livre que so conhece a interface — recebe qualquer implementacao
// por referencia, sem saber o tipo concreto (despacho via vtable)
inline void print_bill(const Billable& billable) {
    std::cout << "  Fatura: $" << billable.billed_cost() << std::endl;
}

#endif // BILLABLE_HPP
