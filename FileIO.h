//
// Created by aba15 on 2024/9/23.
//

#ifndef COMPILER_FILEIO_H
#define COMPILER_FILEIO_H
#include "ConstType.h"
#include "Node.h"
#include <fstream>

using namespace std;

class FileIO {
    public:
        static string openFile();
        static void closeFile();
        static void printToFile_Lexer(Token token);
    static void printToFile_Grammar(ParsingItem type);
    static void printToFile_Error(int lineNum, string errorMsg);

    private:
    static std::ifstream input;
    static std::ofstream output;
    static std::ofstream error;

};

//inline FileIO fileIO;

#endif //COMPILER_FILEIO_H
