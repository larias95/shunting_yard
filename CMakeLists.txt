cmake_minimum_required(VERSION 3.5.0)
project(ShuntingYard VERSION 0.1.0 LANGUAGES C CXX)

set(SOURCE src/app/main.cpp)
set(INCLUDE src/core/)

add_executable(ShuntingYard ${SOURCE})
target_include_directories(ShuntingYard PRIVATE ${INCLUDE})
