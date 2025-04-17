file(GLOB LUA_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

# Exclude the interpreter and compiler if not needed
list(REMOVE_ITEM LUA_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lua.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/luac.c"
)

add_library(lua-static STATIC ${LUA_SOURCES})
target_include_directories(lua-static PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
set_target_properties(lua-static PROPERTIES OUTPUT_NAME lua)
