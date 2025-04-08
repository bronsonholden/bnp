#define ALL_COMPONENTS           \
       bnp::Transform,           \
       bnp::Instances,           \
       bnp::Material,            \
       bnp::Mesh,                \
       bnp::Renderable

#define REGISTERED_COMPONENTS     \
       X(bnp::Transform)          \
       X(bnp::Instances)          \
       X(bnp::Material)           \
       X(bnp::Mesh)               \
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
