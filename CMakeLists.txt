cmake_minimum_required(VERSION 3.28)
project(Compiler)

set(CMAKE_CXX_STANDARD 17)

add_executable(Compiler Main.cpp
        lexer/Lexer.cpp
        fileIO/FileIO.h
        fileIO/FileIO.cpp
        parser/Node.h
        parser/Parser.cpp
        parser/Parser.h
        semantic/SymbolTable.h
        semantic/SemanticVisitor.cpp
        semantic/SemanticVisitor.h
        codeGeneration/PcodeType.h
        codeGeneration/Pcode.h
        codeGeneration/ToPcodeVisitor.cpp
        codeGeneration/ToPcodeVisitor.h
        semantic/Visitor.h
        codeGeneration/Interpreter.cpp
        codeGeneration/Interpreter.h
)
