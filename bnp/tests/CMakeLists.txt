include(FetchContent)
set(CATCH_BUILD_SHARED_LIBS OFF)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.2   # Use latest stable tag or known good version
)

FetchContent_MakeAvailable(Catch2)

include(Catch)

file(GLOB TEST_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.test.cpp")

foreach(test_src IN LISTS TEST_FILES)
    get_filename_component(test_name ${test_src} NAME_WE)

    add_executable(test_${test_name} ${test_src})
    target_link_libraries(test_${test_name} PRIVATE Catch2::Catch2WithMain bnplib)
    catch_discover_tests(test_${test_name})
endforeach()
