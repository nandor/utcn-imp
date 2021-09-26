cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(imp)

add_compile_options(
    -std=c++17
    -Wall
    -Wextra
    -Wno-unused-parameter
    -pedantic
)

add_executable(imp
    ast.cpp
    codegen.cpp
    interp.cpp
    lexer.cpp
    main.cpp
    parser.cpp
    program.cpp
    runtime.cpp
    verifier.cpp
)
