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

## Spdlog
add_subdirectory(third-party/spdlog EXCLUDE_FROM_ALL)

find_package(Threads REQUIRED)
find_package(OpenMP REQUIRED)

## Herd - libraries
add_subdirectory(common)

# Targets
add_library(worker_base_runtime STATIC)
target_include_directories(worker_base_runtime PRIVATE include)
target_include_directories(worker_base_runtime INTERFACE include)

target_sources(
        worker_base_runtime
        PRIVATE
        src/base_runtime/data_frame/data_frame.cpp
        src/base_runtime/data_frame/data_frame_input.cpp
        src/base_runtime/data_frame/data_frame_output.cpp

        src/base_runtime/util/config_loader.cpp

        src/base_runtime/executor/runnable_circuit.cpp
        src/base_runtime/executor/executor.cpp
        src/base_runtime/executor/tree_runner.cpp

        src/base_runtime/crypto/crypto.cpp
        src/base_runtime/crypto/crypto_vector.cpp
        src/base_runtime/crypto/crypto_factory.cpp

        src/base_runtime/crypto/binfhe/binfhe.cpp
        src/base_runtime/crypto/binfhe/detail/crypto.cpp
        src/base_runtime/crypto/binfhe/detail/crypto_value.cpp
        src/base_runtime/crypto/binfhe/detail/keyset.cpp

        include/base_runtime/config.hpp
        include//base_runtime/util/config_loader.hpp

        include/base_runtime/crypto/exception.hpp
        include/base_runtime/crypto/crypto.hpp
        include/base_runtime/crypto/i_keyset.hpp
        include/base_runtime/crypto/crypto_value.hpp
        include/base_runtime/crypto/crypto_vector.hpp
        include/base_runtime/crypto/crypto_factory.hpp

        include/base_runtime/crypto/binfhe/binfhe.hpp
        include/base_runtime/crypto/binfhe/detail/crypto.hpp
        include/base_runtime/crypto/binfhe/detail/crypto_value.hpp
        include/base_runtime/crypto/binfhe/detail/keyset.hpp

        include/base_runtime/data_frame/data_frame.hpp
        include/base_runtime/data_frame/data_frame_input.hpp
        include/base_runtime/data_frame/data_frame_output.hpp

        include/base_runtime/executor/executor.hpp
        include/base_runtime/executor/runnable_circuit.hpp
        include/base_runtime/executor/runnable_node.hpp
        include/base_runtime/executor/tree_runner.hpp
        include/base_runtime/executor/exception.hpp
)

target_compile_definitions(
        worker_base_runtime
        PUBLIC
        MATHBACKEND=2
)

target_link_libraries(
        worker_base_runtime
        PRIVATE
        options_target
        warnings_target
)

target_link_libraries(
        worker_base_runtime
        PRIVATE
        pthread

        OPENFHEcore
        OPENFHEbinfhe
        OpenMP::OpenMP_CXX

        spdlog::spdlog_header_only

        herd_common
)

target_include_directories(
        worker_base_runtime
        SYSTEM PRIVATE
        ${OpenFHE_INCLUDE}
        ${OpenFHE_INCLUDE}/third-party/include
        ${OpenFHE_INCLUDE}/core
        ${OpenFHE_INCLUDE}/pke
        ${OpenFHE_INCLUDE}/binfhe
        ${UUID_INCLUDE_DIRS}
)

option(BUILD_GRPC_RUNTIME "Enable GRPC Runtime" TRUE)
if(BUILD_GRPC_RUNTIME)
    add_subdirectory(proto)
    add_subdirectory(mappers)

    add_executable(executor_worker)

    target_include_directories(executor_worker PRIVATE include)

    target_sources(
            executor_worker
            PRIVATE

            src/grpc/main.cpp
            src/grpc/controller/worker_controller.cpp
            src/grpc/mapper/circuit.cpp

            include/grpc/controller/worker_controller.hpp
            include/grpc/mapper/circuit.hpp
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

            worker_base_runtime

            spdlog::spdlog_header_only

            herd_proto
            herd_common
            herd_dto_mappers
    )
endif()