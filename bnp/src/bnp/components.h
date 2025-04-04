#define ALL_COMPONENTS           \
       bnp::Transform,            \
       bnp::Instances,           \
       bnp::Renderable,          \
       bnp::Mesh,                \
       bnp::Material             \

#define REGISTERED_COMPONENTS     \
       X(bnp::Transform)          \
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
