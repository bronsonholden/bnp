// node_ref is excluded because we'll emplace it
// upon instantiation

#define REGISTERED_COMPONENTS     \
        X(bnp::Position)          \
        X(bnp::Scale)             \
        X(bnp::Rotation)          \

#include <bnp/components/hierarchy.h>
#include <bnp/components/transform.h>
