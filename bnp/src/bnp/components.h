#define ALL_COMPONENTS           \
       bnp::Position,            \
       bnp::Scale,               \
       bnp::Rotation,            \
       bnp::Instances,           \
       bnp::Renderable,          \
       bnp::Mesh,                \
       bnp::Material             \

#define REGISTERED_COMPONENTS     \
       X(bnp::Position)           \
       X(bnp::Scale)              \
       X(bnp::Rotation)           \
       X(bnp::Instances)          \
       X(bnp::Renderable)

#if 0

X(bnp::Camera)             \
X(bnp::Renderable)         \
X(bnp::Mesh)               \
X(bnp::Material)           \

#endif

#include <bnp/components/hierarchy.h>
#include <bnp/components/transform.h>
#include <bnp/components/graphics.h>
