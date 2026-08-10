// Abstração de infraestrutura para persistência de assistant_snapshot (DIP,
// TP3-Q4-C). Classes de alto nível dependem desta interface, nunca de uma
// implementação concreta.

#ifndef SNAPSHOT_REPOSITORY_HPP
#define SNAPSHOT_REPOSITORY_HPP

#include "assistant_snapshot.hpp"

class snapshot_repository {
public:
    virtual void save(const assistant_snapshot& snapshot) = 0;
    virtual assistant_snapshot load() = 0;
    virtual ~snapshot_repository() = default;
};

#endif // SNAPSHOT_REPOSITORY_HPP
