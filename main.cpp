
#include "Lexer.h"
#include "FileIO.h"
#include "Parser.h"

int main() {
    std::string content = fileIO.openFile();
    Lexer lexer(content, reserveToConstMap);
    Parser parser(lexer);
    parser.parse();

//    lexer.next();
//    ConstType tokenType = lexer.getLexType();
//    while (tokenType != ConstType::OVERFILE) {
//        Token token(tokenType, lexer.getToken(), lexer.getLineNum());
//        fileIO.printToFile_Lexer(token);
//        lexer.next();
//        tokenType = lexer.getLexType();
//    }

    fileIO.closeFile();
    return 0;
}
