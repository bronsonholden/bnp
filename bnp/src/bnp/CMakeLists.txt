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

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.89.9
)

FetchContent_MakeAvailable(imgui)

# Core ImGui library
add_library(imgui_lib
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
 "game/prefabs/ui.h")

target_include_directories(imgui_lib PUBLIC
    ${imgui_SOURCE_DIR}
)

add_library(imgui_backend
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
 "game/prefabs/ui.h")

target_include_directories(imgui_backend PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui_backend PUBLIC SDL2::SDL2-static)

target_include_directories(bnplib PUBLIC ${bitsery_SOURCE_DIR})
target_link_libraries(bnplib PUBLIC imgui_lib imgui_backend)

# stb_image
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)

FetchContent_MakeAvailable(stb)

target_include_directories(bnplib PUBLIC ${stb_SOURCE_DIR})

# nlohmann/json
include(FetchContent)

FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2 # or latest stable release
)

FetchContent_MakeAvailable(nlohmann_json)
target_link_libraries(bnplib PUBLIC nlohmann_json)

FetchContent_Declare(
  box2d
  GIT_REPOSITORY https://github.com/erincatto/box2d.git
  GIT_TAG v2.4.1
)

FetchContent_MakeAvailable(box2d)

target_link_libraries(bnplib PRIVATE box2d)
target_include_directories(bnplib PUBLIC ${box2d_SOURCE_DIR}/include)

target_link_libraries(bnplib PUBLIC lua-static)
