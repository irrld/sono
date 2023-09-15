cmake_minimum_required(VERSION 3.25)
project(SonoLang)

set(CMAKE_CXX_STANDARD 23)

include_directories(include)

add_executable(SonoLang
        src/main.cc
        src/lexer.cc
        src/error_handler.cc
        src/parser.cc
        src/astree.cc
        src/grammar.cc
        include/lexer.h
        include/error_handler.h
        include/parser.h
        include/astree.h
        include/grammar.h
        include/json.h
)
