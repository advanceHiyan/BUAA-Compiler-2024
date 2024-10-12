#include <fstream>
#include <iostream>
#include "LexicalAnalysis.h"
#include <filesystem>
#include "ConstType.h"

int main() {
    std::ifstream input("testfile.txt");
    std::ofstream output("lexer.txt");
    std::ofstream error("error.txt");


    if (!input.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return 1;
    }

    Lexer lexer(input, reserveToConstMap);
    lexer.next();
    ConstType tokenType = lexer.getLexType();
    while (tokenType != ConstType::OVERFILE) {
        switch (tokenType) {
            case ConstType::CONSTTK: output << "CONSTTK"; break;
            case ConstType::INTTK: output << "INTTK"; break;
            case ConstType::MAINTK: output << "MAINTK"; break;
            case ConstType::GETINTTK: output << "GETINTTK"; break;
            case ConstType::PRINTFTK: output << "PRINTFTK"; break;
            case ConstType::RETURNTK: output << "RETURNTK"; break;
            case ConstType::IDENFR: output << "IDENFR"; break;
            case ConstType::INTCON: output << "INTCON"; break;
            case ConstType::STRCON: output << "STRCON"; break;
            case ConstType::CHRCON: output << "CHRCON"; break;
            case ConstType::LBRACK: output << "LBRACK"; break;
            case ConstType::RBRACK: output << "RBRACK"; break;
            case ConstType::ASSIGN: output << "ASSIGN"; break;
            case ConstType::LBRACE: output << "LBRACE"; break;
            case ConstType::RBRACE: output << "RBRACE"; break;
            case ConstType::SEMICN: output << "SEMICN"; break;
            case ConstType::PLUS: output << "PLUS"; break;
            case ConstType::MINU: output << "MINU"; break;
            case ConstType::MULT: output << "MULT"; break;
            case ConstType::DIV: output << "DIV"; break;
            case ConstType::MOD: output << "MOD"; break;
            case ConstType::LSS: output << "LSS"; break;
            case ConstType::GRE: output << "GRE"; break;
            case ConstType::EQL: output << "EQL"; break;
            case ConstType::NEQ: output << "NEQ"; break;
            case ConstType::AND: output << "AND"; break;
            case ConstType::OR: output << "OR"; break;
            case ConstType::LPARENT: output << "LPARENT"; break;
            case ConstType::RPARENT: output << "RPARENT"; break;
            case ConstType::COMMA: output << "COMMA"; break;
            case ConstType::VOIDTK: output << "VOIDTK"; break;
            case ConstType::NOT: output << "NOT"; break;
            case ConstType::BREAKTK: output << "BREAKTK"; break;
            case ConstType::CONTINUETK: output << "CONTINUETK"; break;
            case ConstType::IFTK: output << "IFTK"; break;
            case ConstType::ELSETK: output << "ELSETK"; break;
            case ConstType::CHARTK: output << "CHARTK"; break;
            case ConstType::LEQ: output << "LEQ"; break;
            case ConstType::GEQ: output << "GEQ"; break;
            case ConstType::GETCHARTK: output << "GETCHARTK"; break;
            case ConstType::FORTK: output << "FORTK"; break;

            case ConstType::SINGLEAND: output << "SINGLEAND";  break;
            case ConstType::SINGLEOR: output << "SINGLEOR"; break;
            default: output << "UNKNOWN"; break;
        }
        if (tokenType == SINGLEAND || tokenType == SINGLEOR) {
            error << lexer.getLineNum() << " a";
        }
        std::string tokenValue = lexer.getToken();
        output << " "<< tokenValue << std ::endl;
        lexer.next();
        tokenType = lexer.getLexType();
    }

    input.close();
    output.close();

    return 0;
}