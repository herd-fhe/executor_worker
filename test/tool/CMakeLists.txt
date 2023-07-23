cmake_minimum_required(VERSION 3.21)
project(
        test_tools
        VERSION 0.1
        DESCRIPTION "test tools"
        LANGUAGES CXX
)

# Warnings config
add_library(warnings_target INTERFACE)
if(MSVC)
    target_compile_options(
            warnings_target
            INTERFACE
            /W4
            /WX "/permissive-"
    )
else()
    target_compile_options(
            warnings_target
            INTERFACE
            -Wall
            -Wextra
            -Wshadow
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Wcast-align
            -Wdouble-promotion
            -Wunused
            -Woverloaded-virtual
            -Wpedantic
            -Wconversion
            -Wsign-conversion
            -Wnull-dereference
            -Werror
    )
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        target_compile_options(
                warnings_target
                INTERFACE
                -Waddress-of-packed-member
                -Wmisleading-indentation
                -Wduplicated-cond
                -Wduplicated-branches
                -Wlogical-op
                -Wuseless-cast
        )
    endif()
endif()

## OpenFHE
find_package(OpenFHE REQUIRED)

find_package(Threads REQUIRED)
find_package(OpenMP REQUIRED)

add_executable(crypto_tool)
target_compile_features(crypto_tool PRIVATE cxx_std_20)

target_include_directories(crypto_tool PRIVATE include)
target_sources(
        crypto_tool
        PRIVATE
        src/main.cpp
        src/cli/parameter.cpp
        src/cli/subcommand.cpp
        src/cli/command_line.cpp
        src/cli/parameter_proxy.cpp

        src/commands/key_command.cpp
        src/commands/encryption_command.cpp

        src/encryption/key.cpp
        src/encryption/crypto.cpp

        include/cli/command_line.hpp
        include/cli/subcommand.hpp
        include/cli/parameter.hpp
        include/cli/parameter_proxy.hpp

        include/commands/key_command.hpp
        include/commands/encryption_command.hpp

        include/encryption/key.hpp
        include/encryption/crypto.hpp
)

target_compile_definitions(
        crypto_tool
        PUBLIC
        MATHBACKEND=2
)

target_link_libraries(
        crypto_tool
        PRIVATE
        warnings_target
)

target_link_libraries(
        crypto_tool
        PRIVATE
        pthread

        OPENFHEcore
        OPENFHEbinfhe
        OpenMP::OpenMP_CXX
)

target_include_directories(
        crypto_tool
        SYSTEM PRIVATE
        ${OpenFHE_INCLUDE}
        ${OpenFHE_INCLUDE}/third-party/include
        ${OpenFHE_INCLUDE}/core
        ${OpenFHE_INCLUDE}/pke
        ${OpenFHE_INCLUDE}/binfhe
)
