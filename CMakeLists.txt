cmake_minimum_required(VERSION 3.21)
project(
        executor_worker
        VERSION 0.1
        DESCRIPTION "herd executor worker"
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


# Project options config

add_library(options_target INTERFACE)
target_compile_features(options_target INTERFACE cxx_std_20)

if(!MSVC)
    option(ENABLE_COVERAGE "Enable coverage reporting (gss/clang)" FALSE)

    if(ENABLE_COVERAGE)
        target_compile_options(options_target INTERFACE --coverage -O0 -g)
        target_link_libraries(options_target INTERFACE --coverage)
    endif()
endif()

# Dependencies
find_package(PkgConfig REQUIRED)

## OpenFHE
find_package(OpenFHE REQUIRED)

## SQLite
find_package(SQLite3)

## Sodium
pkg_check_modules(SODIUM REQUIRED libsodium)

## Spdlog
add_subdirectory(third-party/spdlog EXCLUDE_FROM_ALL)

find_package(Threads REQUIRED)
find_package(OpenMP REQUIRED)

## Herd - libraries
add_subdirectory(proto)
add_subdirectory(common)
add_subdirectory(mappers)

# Targets

## Executor
add_executable(executor_worker)

target_include_directories(executor_worker PRIVATE include)

target_sources(
        executor_worker
        PRIVATE

        src/main.cpp

        src/controller/worker_controller.cpp

        src/util/config_loader.cpp

        src/data_frame/data_frame.cpp
        src/data_frame/data_frame_input.cpp
        src/data_frame/data_frame_output.cpp

        src/executor/runnable_circuit.cpp
        src/executor/executor.cpp
        src/executor/tree_runner.cpp

        src/mapper/circuit.cpp

        src/crypto/crypto.cpp
        src/crypto/crypto_vector.cpp
        src/crypto/crypto_factory.cpp

        src/crypto/binfhe/binfhe.cpp
        src/crypto/binfhe/detail/crypto.cpp
        src/crypto/binfhe/detail/crypto_value.cpp
        src/crypto/binfhe/detail/keyset.cpp

        include/controller/worker_controller.hpp

        include/config.hpp
        include/util/config_loader.hpp

        include/crypto/exception.hpp
        include/crypto/crypto.hpp
        include/crypto/i_keyset.hpp
        include/crypto/crypto_value.hpp
        include/crypto/crypto_vector.hpp
        include/crypto/crypto_factory.hpp

        include/crypto/binfhe/binfhe.hpp
        include/crypto/binfhe/detail/crypto.hpp
        include/crypto/binfhe/detail/crypto_value.hpp
        include/crypto/binfhe/detail/keyset.hpp

        include/data_frame/data_frame.hpp
        include/data_frame/data_frame_input.hpp
        include/data_frame/data_frame_output.hpp

        include/executor/executor.hpp
        include/executor/runnable_circuit.hpp
        include/executor/runnable_node.hpp
        include/executor/tree_runner.hpp
        include/executor/exception.hpp

        include/mapper/circuit.hpp
)

target_compile_definitions(
        executor_worker
        PUBLIC
        MATHBACKEND=2
)

target_link_libraries(
        executor_worker
        PRIVATE
        options_target
        warnings_target
)

target_link_libraries(
        executor_worker
        PRIVATE
        pthread

        OPENFHEcore
        OPENFHEbinfhe
        OpenMP::OpenMP_CXX

        ${SODIUM_LIBRARIES}

        spdlog::spdlog_header_only

        herd_proto
        herd_common
        herd_dto_mappers
)

target_include_directories(
        executor_worker
        SYSTEM PRIVATE
        ${OpenFHE_INCLUDE}
        ${OpenFHE_INCLUDE}/third-party/include
        ${OpenFHE_INCLUDE}/core
        ${OpenFHE_INCLUDE}/pke
        ${OpenFHE_INCLUDE}/binfhe
        ${SODIUM_INCLUDE_DIRS}
        ${UUID_INCLUDE_DIRS}
)

# Test

#option(ENABLE_TESTS "Enable tests" FALSE)
#
#if(ENABLE_TESTS)
#    add_subdirectory(test)
#endif()