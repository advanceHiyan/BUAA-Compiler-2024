
#include "Lexer.h"
#include "FileIO.h"
#include "Parser.h"

int main() {
    std::string content = FileIO::openFile();
    Lexer lexer(content, reserveToConstMap);
    Parser parser(lexer);
    parser.parse();

    FileIO::closeFile();
    return 0;
}
