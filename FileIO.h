//
// Created by aba15 on 2024/9/23.
//

#ifndef COMPILER_FILEIO_H
#define COMPILER_FILEIO_H
#include "ConstType.h"
#include "Node.h"
#include "SymbolTable.h"
#include <fstream>

using namespace std;

class FileIO {
    public:
        static string openFile();
        static void closeFile();
        static void printToFile_Lexer(Token token);
    static void printToFile_Grammar(ParsingItem type);
    static void printToFile_Error(int lineNum, string errorMsg);
    static void printToFile_Symbol(std::vector<Symbol*> *printf_list);

    int getSys();
    static std::ifstream input;
    static std::ofstream output;
    static std::ofstream error;
    static int sysAA;
    //非静态成员与函数只不能实现在.h内类外，还可以在.h外或者类内
    //静态成员和函数只能实现在.h外

};

//int FileIO::getSys() {不能 在.h文件里，class括弧外 实现类里的方法或者初始化类里的变量，包括静态变量
//    return 0;
//}
//FileIO::sysAA = 0;

//不能在这里初始化类里的静态变量，因为可能被多次初始化。
inline FileIO fileIO;//可以
const int aSys = 1;//可以这么做
static int stSys = 1;//也可以这么做
//int Sys = 1; 不行

#endif //COMPILER_FILEIO_H
