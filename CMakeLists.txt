cmake_minimum_required(VERSION 3.10)

#Project
project(codeGSimulator VERSION 0.1)

#Policy
cmake_policy(SET CMP0076 NEW) #target_sources

#Enabling CTest
#enable_testing()

include(FetchContent)
FetchContent_Declare(
        cli11
        GIT_REPOSITORY https://github.com/CLIUtils/CLI11
        GIT_TAG        v2.3.1
)

FetchContent_MakeAvailable(cli11)
include_directories(${CLI11_SOURCE_DIR}/include)

#Variables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release)

#Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

#Copy example folder
file(COPY "example/" DESTINATION "example/")

#Set cache entry
set(TARGET_ARCH "" CACHE STRING "Set architecture type (32 or 64 or nothing (auto))")

#Not defined = default to empty
if (NOT DEFINED TARGET_ARCH)
    set(TARGET_ARCH "")
endif()

#Set compiler 32/64/(auto) flag
set(TARGET_ARCH_FLAG "")

if (NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    if (TARGET_ARCH MATCHES "32")
        set(TARGET_ARCH_FLAG "-m32")
    elseif (TARGET_ARCH MATCHES "64")
        set(TARGET_ARCH_FLAG "-m64")
    else()
        set(TARGET_ARCH_FLAG "")
    endif()
endif()

#Check for architecture
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH 64)
else()
    set(ARCH 32)
endif()

#Configure header file
configure_file(CMakeConfig.hpp.in CMakeConfig.hpp)

#C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#Set compiler options
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # using Clang
    add_compile_options(-Wall -Wextra -pedantic ${TARGET_ARCH_FLAG})
    set(debug_flags -g)
    set(release_flags -O3 -static)
    add_compile_options("$<$<CONFIG:DEBUG>:${debug_flags}>")
    add_compile_options("$<$<CONFIG:RELEASE>:${release_flags}>")
    add_link_options(${TARGET_ARCH_FLAG})
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    # using Visual Studio C++
    add_compile_options(/EHsc)
    set(debug_flags /DEBUG)
    set(release_flags /O2)
    add_compile_options("$<$<CONFIG:DEBUG>:${debug_flags}>")
    add_compile_options("$<$<CONFIG:RELEASE>:${release_flags}>")
else()
    # using GCC
    add_compile_options(-Wall -Wextra -pedantic ${TARGET_ARCH_FLAG})
    set(debug_flags -g)
    set(release_flags -O3 -s -static)
    add_compile_options($<$<CONFIG:DEBUG>:${debug_flags}>)
    add_compile_options("$<$<CONFIG:RELEASE>:${release_flags}>")
    add_link_options(${TARGET_ARCH_FLAG})
endif()

#Executable
add_executable(${PROJECT_NAME})

#Includes path
target_include_directories(${PROJECT_NAME} PUBLIC "include/")
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}")

#Source files
target_sources(${PROJECT_NAME} PUBLIC "src/main.cpp")
target_sources(${PROJECT_NAME} PUBLIC "src/C_console.cpp")
target_sources(${PROJECT_NAME} PUBLIC "src/C_string.cpp")

target_sources(${PROJECT_NAME} PUBLIC "src/memoryModule/C_MM1.cpp")
target_sources(${PROJECT_NAME} PUBLIC "src/memoryModule/memoryModules.cpp")

target_sources(${PROJECT_NAME} PUBLIC "src/peripheral/C_uart.cpp")

target_sources(${PROJECT_NAME} PUBLIC "src/motherboard/motherboards.cpp")
target_sources(${PROJECT_NAME} PUBLIC "src/motherboard/C_GCM_5_1.cpp")

target_sources(${PROJECT_NAME} PUBLIC "src/processor/C_GP8B_5_1.cpp")
target_sources(${PROJECT_NAME} PUBLIC "src/processor/C_ALUminium_1_1.cpp")

#Header files
target_sources(${PROJECT_NAME} PUBLIC "include/C_console.hpp")
target_sources(${PROJECT_NAME} PUBLIC "include/C_string.hpp")
target_sources(${PROJECT_NAME} PUBLIC "include/C_bus.hpp")
target_sources(${PROJECT_NAME} PUBLIC "include/C_signal.hpp")
target_sources(${PROJECT_NAME} PUBLIC "include/C_codeg.hpp")

target_sources(${PROJECT_NAME} PUBLIC "include/memoryModule/memoryModules.hpp")
target_sources(${PROJECT_NAME} PUBLIC "include/memoryModule/C_MM1.hpp")

target_sources(${PROJECT_NAME} PUBLIC "include/peripheral/C_peripheral.hpp")
target_sources(${PROJECT_NAME} PUBLIC "include/peripheral/C_uart.hpp")

target_sources(${PROJECT_NAME} PUBLIC "include/motherboard/motherboards.hpp")
target_sources(${PROJECT_NAME} PUBLIC "include/motherboard/C_GCM_5_1.hpp")

target_sources(${PROJECT_NAME} PUBLIC "include/processor/C_processor.hpp")
target_sources(${PROJECT_NAME} PUBLIC "include/processor/C_GP8B_5_1.hpp")
target_sources(${PROJECT_NAME} PUBLIC "include/processor/C_alu.hpp")
target_sources(${PROJECT_NAME} PUBLIC "include/processor/C_ALUminium_1_1.hpp")

#Add test
#add_test(NAME "CompilingTestFile" COMMAND ${PROJECT_NAME} "--in=example/test")
#add_test(NAME "CompilingModelFile" COMMAND ${PROJECT_NAME} "--in=example/model")
#add_test(NAME "CompilingUartTestFile" COMMAND ${PROJECT_NAME} "--in=example/uart_test")
