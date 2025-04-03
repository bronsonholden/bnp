file(GLOB_RECURSE
    BNP_SRC
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "*.cpp"
    "*.hpp"
    "*.h"
)

add_library(bnplib STATIC ${BNP_SRC})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET bnplib PROPERTY CXX_STANDARD 20)
endif()

# Fetch GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)

FetchContent_MakeAvailable(glm)
target_link_libraries(bnplib PUBLIC glm)
target_include_directories(bnplib PUBLIC ${glm_SOURCE_DIR})

# Fetch SDL
FetchContent_Declare(
    SDL
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.30.11
)

FetchContent_MakeAvailable(SDL)
target_link_libraries(bnplib PUBLIC SDL2::SDL2-static SDL2::SDL2main)

FetchContent_Declare(
    glew
    GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
    GIT_TAG glew-cmake-2.2.0
)

FetchContent_MakeAvailable(glew)
target_link_libraries(bnplib PUBLIC libglew_static)

FetchContent_Declare(
  EnTT
  GIT_REPOSITORY https://github.com/skypjack/entt.git
  GIT_TAG v3.12.2
)

FetchContent_MakeAvailable(EnTT)
target_link_libraries(bnplib PUBLIC EnTT::EnTT)

FetchContent_Declare(
  bitsery
  GIT_REPOSITORY https://github.com/fraillt/bitsery.git
  GIT_TAG v5.2.4
)

FetchContent_MakeAvailable(bitsery)
target_include_directories(bnplib PUBLIC ${bitsery_SOURCE_DIR}/include)
message(STATUS "Bitsery include path: ${bitsery_SOURCE_DIR}/include")
