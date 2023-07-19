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

add_executable(dataframe_decryptor)
target_compile_features(dataframe_decryptor PRIVATE cxx_std_20)

target_sources(
        dataframe_decryptor
        PRIVATE
        dataframe_decryptor.cpp
)

target_compile_definitions(
        dataframe_decryptor
        PUBLIC
        MATHBACKEND=2
)

target_link_libraries(
        dataframe_decryptor
        PRIVATE
        warnings_target
)

target_link_libraries(
        dataframe_decryptor
        PRIVATE
        pthread

        OPENFHEcore
        OPENFHEbinfhe
        OpenMP::OpenMP_CXX
)

target_include_directories(
        dataframe_decryptor
        SYSTEM PRIVATE
        ${OpenFHE_INCLUDE}
        ${OpenFHE_INCLUDE}/third-party/include
        ${OpenFHE_INCLUDE}/core
        ${OpenFHE_INCLUDE}/pke
        ${OpenFHE_INCLUDE}/binfhe
)

add_executable(data_generator)

target_sources(
        data_generator
        PRIVATE
        data_generator.cpp
)

target_compile_definitions(
        data_generator
        PUBLIC
        MATHBACKEND=2
)

target_link_libraries(
        dataframe_decryptor
        PRIVATE
        warnings_target
)

target_link_libraries(
        data_generator
        PRIVATE
        pthread

        OPENFHEcore
        OPENFHEbinfhe
        OpenMP::OpenMP_CXX
)

target_include_directories(
        data_generator
        SYSTEM PRIVATE
        ${OpenFHE_INCLUDE}
        ${OpenFHE_INCLUDE}/third-party/include
        ${OpenFHE_INCLUDE}/core
        ${OpenFHE_INCLUDE}/pke
        ${OpenFHE_INCLUDE}/binfhe
)