include(FetchContent)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# bnplib
add_subdirectory(src)
add_subdirectory(tests)

# TODO
# ImGui
#

# Assimp
#
# FetchContent_Declare(
#     assimp
#     GIT_REPOSITORY https://github.com/assimp/assimp.git
#     GIT_TAG master  # Or specify a commit/tag/branch
# )
#
# # Make sure Assimp is downloaded and built before your project
# FetchContent_MakeAvailable(assimp)
#
# # Link Assimp to your project
# target_link_libraries(bnplib PUBLIC assimp)

add_subdirectory(vendor/lua-5.4.6)
add_subdirectory(app)
