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
        FileIO();
        string openFile();
        void closeFile();
        void printToFile_Lexer(Token token);
    void printToFile_Grammar(ParsingItem type);
    void printToFile_Error(int lineNum, string errorMsg);

    private:
    std::ifstream input;
    std::ofstream output;
    std::ofstream error;
    std::string fileName;

};

inline FileIO fileIO;
inline int sys =11;

#endif //COMPILER_FILEIO_H
