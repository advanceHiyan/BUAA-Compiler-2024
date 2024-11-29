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
        varNameToInfoMap = new std::unordered_map<std::string, VarInfo>; //没有用，这改变的只是参数指针，而不是外部传地址的指针
        this->stackIndex = stackIndex;
        this->paramCount = paramCount;
        this->haveParams = haveParams;
    }
};

class Interpreter {

public:
    Interpreter(std::vector<PCode*>* pCodeList);
    void interpret();

private:
    std :: vector<PCode*>* pCodeList;
    std:: vector<int> *programStack;
    std::unordered_map<std::string ,FunctInfo> *funNameToPcIndexMap;
    std::unordered_map<std::string, VarInfo> *tempVarNameToInfoMap;
    std::unordered_map<std::string, int> *labelNameToPcIndexMap;
    std::vector<int> *paramStackIndexList;
    std::vector<AR_Info*> *arList;
    int pc = 0;
    int needParams = 0;
    int haveParams = 0;
    int startPcIndex;

    int pop();

    void push(int value);

    void setElement(int index, int value);

    AR_Info *popAR_List();

    VarInfo getVarInfo(std::string varName);
};

#endif //COMPILER_INTERPRETER_H
