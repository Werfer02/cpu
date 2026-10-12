cmake_minimum_required(VERSION 3.20)

project(cpu LANGUAGES C CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    coututils
    GIT_REPOSITORY https://github.com/Werfer02/coututils.git
    GIT_TAG main
)
FetchContent_MakeAvailable(coututils)
message(STATUS "coututils in: ${coututils_SOURCE_DIR}")

FetchContent_Declare(
    argparser
    GIT_REPOSITORY https://github.com/Werfer02/argparser.git
    GIT_TAG master
)
FetchContent_MakeAvailable(argparser)
message(STATUS "argparser in: ${argparser_SOURCE_DIR}")

add_executable(out main.cpp src/util.cpp
                            src/assembler.cpp
                            src/sim.cpp
                            src/langdefinitions.cpp
                            src/register.cpp
                            src/screen.cpp
                            src/alu.cpp
                            src/app.cpp)
target_include_directories(out PRIVATE "${coututils_SOURCE_DIR}" "${argparser_SOURCE_DIR}" include)
