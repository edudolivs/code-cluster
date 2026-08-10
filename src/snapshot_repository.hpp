// Abstração de infraestrutura para persistência de AssistantSnapshot (DIP,
// TP3-Q4-C). Classes de alto nível dependem desta interface, nunca de uma
// implementação concreta.

#ifndef SNAPSHOT_REPOSITORY_HPP
#define SNAPSHOT_REPOSITORY_HPP

#include "assistant_snapshot.hpp"

class SnapshotRepository {
public:
    virtual void save(const AssistantSnapshot& snapshot) = 0;
    virtual AssistantSnapshot load() = 0;
    virtual ~SnapshotRepository() = default;
};

#endif // SNAPSHOT_REPOSITORY_HPP
