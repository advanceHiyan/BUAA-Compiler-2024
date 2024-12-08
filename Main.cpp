
// @creator :  BUUAAAA - advanceHiyan
// Copyright (c)  2024 All Rights Reserved.

#include "lexer/Lexer.h"
#include "fileIO/FileIO.h"
#include "parser/Parser.h"
#include "semantic/SemanticVisitor.h"
#include "codeGeneration/ToPcodeVisitor.h"
#include "codeGeneration/Interpreter.h"


int main() {
    std::string content = FileIO::openFile();
    Lexer lexer(content, reserveToConstMap);
    Parser parser(lexer);
    Node* root = parser.parse();
    Visitor *semanticAnalyzer = new SemanticAnalyzer();
    (*semanticAnalyzer).visit(root);
    if (!FileIO::hasError) {
        Visitor *codeGenerator = new ToPcodeVisitor();
        (*codeGenerator).visit(root);
        ToPcodeVisitor * pcodeGenerator = static_cast<ToPcodeVisitor*>(codeGenerator);
        Interpreter *interpreter = new Interpreter(pcodeGenerator->getPcodeList());
        interpreter ->interpret();
    }
    FileIO::closeFile();
    return 0;
}
