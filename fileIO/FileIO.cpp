//
// Created by aba15 on 2024/9/23.
//
#include "FileIO.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include "ConstType.h"
std::ifstream FileIO::input = std::ifstream ("testfile.txt");
std::ofstream FileIO::output = std::ofstream ("parser.txt");
std::ofstream FileIO::error = std::ofstream ("error.txt");
std::ofstream FileIO::symbol = std::ofstream ("symbol.txt");
std::ofstream FileIO::result = std::ofstream ("pcoderesult.txt");
std::unordered_map<int, std::string>* FileIO::errorMap = new std::unordered_map<int, std::string>;
bool FileIO::hasError = false;
//额外的初始化


std::string FileIO:: openFile() {
    if (!input.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << input.rdbuf(); // 读取文件内容到 stringstream
    std::string content = buffer.str();
    return content;
}

void FileIO:: closeFile() {
    FileIO::printToFile_Error(-1,"printError");
    input.close();
    output.close();
    error.close();
}

void FileIO:: printToFile_Result(std::string result) {
    FileIO::result << result.substr(1,result.size()-2);
}

bool greaterThan(Symbol* a, Symbol* b) {//交换参数不能同时为真
    if(a->blockNum < b->blockNum) {
        return true;
    }
    if(b->blockNum < a->blockNum) {
        return false;
    }
    return false;
}

void FileIO::printToFile_Symbol(std::vector<Symbol*> *printf_list) {
    std::stable_sort((*printf_list).begin(), (*printf_list).end(), greaterThan);//换个方法
    for (auto symbol : (*printf_list)) {
    std::string typeStr;
    switch (symbol->type) {
        case SymbolType::ConstChar: typeStr = "ConstChar"; break;
        case SymbolType::ConstInt: typeStr = "ConstInt"; break;
        case SymbolType::ConstCharArray: typeStr = "ConstCharArray"; break;
        case SymbolType::ConstIntArray: typeStr = "ConstIntArray"; break;
        case SymbolType::Char: typeStr = "Char"; break;
        case SymbolType::Int: typeStr = "Int"; break;
        case SymbolType::CharArray: typeStr = "CharArray"; break;
        case SymbolType::IntArray: typeStr = "IntArray"; break;
        case SymbolType::VoidFunc: typeStr = "VoidFunc"; break;
        case SymbolType::CharFunc: typeStr = "CharFunc"; break;
        case SymbolType::IntFunc: typeStr = "IntFunc"; break;
    }
    FileIO::symbol<< symbol->blockNum << " " << symbol->name << " " << typeStr << std::endl;
    }
}

void FileIO::printToFile_Grammar(ParsingItem type) {
    std::string typeStr;
    switch(type) {
        case ParsingItem::CompUnit: typeStr = "CompUnit"; break;
        case ParsingItem::MainFuncDef: typeStr = "MainFuncDef"; break;
        case ParsingItem::ConstDecl: typeStr = "ConstDecl"; break;
        case ParsingItem::ConstDef: typeStr = "ConstDef"; break;
        case ParsingItem::ConstInitVal: typeStr = "ConstInitVal"; break;
        case ParsingItem::VarDecl: typeStr = "VarDecl"; break;
        case ParsingItem::VarDef: typeStr = "VarDef"; break;
        case ParsingItem::InitVal: typeStr = "InitVal"; break;

        case ParsingItem::FuncDef: typeStr = "FuncDef"; break;
        case ParsingItem::FuncType: typeStr = "FuncType"; break;
        case ParsingItem::FuncFParams: typeStr = "FuncFParams"; break;
        case ParsingItem::FuncFParam: typeStr = "FuncFParam"; break;
        case ParsingItem::Block: typeStr = "Block"; break;

        case ParsingItem::Stmt: typeStr = "Stmt"; break;
        case ParsingItem::ForStmt: typeStr = "ForStmt"; break;
        case ParsingItem::Character: typeStr = "Character"; break;
        case ParsingItem::Number: typeStr = "Number"; break;

        case ParsingItem::Exp: typeStr = "Exp"; break;
        case ParsingItem::Cond: typeStr = "Cond"; break;
        case ParsingItem::LVal: typeStr = "LVal"; break;
        case ParsingItem::PrimaryExp: typeStr = "PrimaryExp"; break;

        case ParsingItem::UnaryExp: typeStr = "UnaryExp"; break;
        case ParsingItem::UnaryOp: typeStr = "UnaryOp"; break;
        case ParsingItem::FuncRParams: typeStr = "FuncRParams"; break;
        case ParsingItem::MulExp: typeStr = "MulExp"; break;
        case ParsingItem::AddExp: typeStr = "AddExp"; break;
        case ParsingItem::RelExp: typeStr = "RelExp"; break;
        case ParsingItem::EqExp: typeStr = "EqExp"; break;
        case ParsingItem::LAndExp: typeStr = "LAndExp"; break;
        case ParsingItem::LOrExp: typeStr = "LOrExp"; break;
        case ParsingItem::ConstExp: typeStr = "ConstExp"; break;

        case ParsingItem::BlockItem: typeStr = "BlockItem"; break;
        case ParsingItem::BType: typeStr = "BType"; break;
        case ParsingItem::Decl: typeStr = "Decl"; break;

        case ParsingItem::OverToken: typeStr = "OverToken"; break;
    }
    output << "<" << typeStr << ">" << std::endl;
}

void FileIO:: printToFile_Error(int lineNum, std::string errorMsg) {
    hasError = true;
    if(errorMsg == "printError") {
        int num = (*FileIO::errorMap).size();
        int i = 0;
        int outputNum = 0;
        while(outputNum < num) {
            if((*FileIO::errorMap)[i] != "") {
                error<< i << " " << (*FileIO::errorMap)[i] << std::endl;
                outputNum++;
            }
            i++;
        }
    } else {
        (*FileIO::errorMap)[lineNum] = errorMsg;
    }
//    error << lineNum << " " << errorMsg << std::endl;
}

void FileIO ::printToFile_Lexer(Token token) {
    int lineNum = token.lineNumber;
    std::string tokenValue = token.tokenValue;
    ConstType tokenType = token.tokenType;
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
    if (tokenType == ConstType::SINGLEAND || tokenType == ConstType::SINGLEOR) {
        printToFile_Error(lineNum,"a");
    }
    output << " "<< tokenValue << std ::endl;
}



