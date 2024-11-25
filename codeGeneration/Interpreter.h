//
// Created by aba15 on 2024/11/24.
//

#ifndef COMPILER_INTERPRETER_H
#define COMPILER_INTERPRETER_H
#include <iostream>
#include <vector>
#include "Pcode.h"
#include "variant"
//struct FunctInfo {
//    std::string name;
//    int pcIndex;
//};
struct FunctInfo {
    std::string name;
    int pcIndex;
    int paramCount;

    FunctInfo(std::string name, int pcIndex, int paramCount) {
        this->name = name;
        this->pcIndex = pcIndex;
        this->paramCount = paramCount;
    }
};


struct VarInfo {
    std::string name;
    int stackIndex;
    bool isChar;
    bool isArray;
    VarInfo(int stackIndex, std::string name, bool isChar, bool isArray) {
        this->stackIndex = stackIndex;
        this->name = name;
        this->isChar = isChar;
        this->isArray = isArray;
    }
};

struct AR_Info {
    int retPc;
    std::unordered_map<std::string, VarInfo> *varNameToInfoMap;
    int stackIndex;
    int paramCount;
    int haveParams;
    AR_Info(int retPc, std::unordered_map<std::string, VarInfo> *varNameToInfoMap,
            int stackIndex, int paramCount, int haveParams) {
        this->retPc = retPc;
        this->varNameToInfoMap = varNameToInfoMap;
        varNameToInfoMap = new std::unordered_map<std::string, VarInfo>;
        this->stackIndex = stackIndex;
        this->paramCount = paramCount;
        this->haveParams = haveParams;
    }
};

using VariantType = std::variant<int, std::string>;

class Interpreter {

public:
    Interpreter(std::vector<PCode*>* pCodeList);
    void interpret();

private:
    std :: vector<PCode*>* pCodeList;
    std:: vector<VariantType> *programStack;
    std::unordered_map<std::string ,FunctInfo> *funNameToPcIndexMap;
    std::unordered_map<std::string, VarInfo> *tempVarNameToInfoMap;
    std::vector<int> *paramStackIndexList;
    std::vector<AR_Info*> *arList;
    int pc = 0;
    int needParams = 0;
    int haveParams = 0;
    int startPcIndex;

    VariantType pop();

    void push(VariantType value);

    void setElement(int index, VariantType value);

    AR_Info *popAR_List();

    VarInfo getVarInfo(std::string varName);
};
//    while (!myStack.empty()) {
//        VariantType topElement = myStack.top();
//
//        // 使用std::holds_alternative检查元素类型并进行相应处理
//        if (std::holds_alternative<int>(topElement)) {
//            int intValue = std::get<int>(topElement);
//            std::cout << "栈顶元素是整数: " << intValue << std::endl;
//        } else if (std::holds_alternative<std::string>(topElement)) {
//            std::string stringValue = std::get<std::string>(topElement);
//            std::cout << "栈顶元素是字符串: " << stringValue << std::endl;
//        }
//
//        myStack.pop();
//    }

//int* int_ptr = reinterpret_cast<int*>(void_ptr);
#endif //COMPILER_INTERPRETER_H
