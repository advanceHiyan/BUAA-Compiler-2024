
#include "Lexer.h"
#include "FileIO.h"
#include "Parser.h"
#include "Visitor.h"


int main() {
    std::string content = FileIO::openFile();
    Lexer lexer(content, reserveToConstMap);
    Parser parser(lexer);
    Node* root = parser.parse();
    SemanticAnalyzer semanticAnalyzer;
    semanticAnalyzer.visit(root);


    FileIO::closeFile();
    return 0;
}
