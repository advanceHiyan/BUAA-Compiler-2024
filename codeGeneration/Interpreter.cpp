//
// Created by aba15 on 2024/11/24.
//

#include <iostream>
#include "Interpreter.h"
#include "../fileIO/FileIO.h"
int getint();
int getchar();

Interpreter::Interpreter(std::vector<PCode *> *pCodeList) {
    this ->pCodeList = pCodeList;
    this ->programStack = new std:: vector<int>();
    this ->funNameToPcIndexMap = new std::unordered_map<std::string,FunctInfo>();
    this ->tempVarNameToInfoMap = new std::unordered_map<std::string, VarInfo>();
    this ->paramStackIndexList = new std::vector<int>();
    this ->arList = new std::vector<AR_Info*>();
    this ->labelNameToPcIndexMap = new std::unordered_map<std::string, int>();
    for (int i = 0; i < pCodeList -> size(); i++) {
        if (pCodeList -> at(i)->getType() == CodeType::MAIN) {
            startPcIndex = i;
        } else if (pCodeList -> at(i)->getType() == CodeType::FUNC) {
            std::string* strPtr = reinterpret_cast<std::string*>(pCodeList -> at(i)->getValue1());
            int paramCount = *reinterpret_cast<int*>(pCodeList -> at(i)->getValue2());
            FunctInfo functInfo(*strPtr, i + 1,paramCount);
            funNameToPcIndexMap->insert({*strPtr, functInfo});
        } else if (pCodeList -> at(i)->getType() == CodeType::LABEL) {
            labelNameToPcIndexMap->insert({*reinterpret_cast<std::string*>(pCodeList -> at(i)->getValue1()), i});
        } else if (pCodeList -> at(i)->getType() == CodeType::EXF &&
        pCodeList -> at(i - 1) -> getType() != CodeType::RET) {
            pCodeList->at(i) = new PCode(CodeType::RET, new int (2));
        }
    }
}

void Interpreter::interpret() {
    cout << "\n\nInterpreting...\n" ;
    while (pc < pCodeList -> size()) {
        cout << "pc: " << pc << " code: " << pCodeList -> at(pc)->getCodeStr() <<endl;
        switch(pCodeList -> at(pc)->getType()) {
            case CodeType::LDI: {
                int value = *reinterpret_cast<int*>(pCodeList -> at(pc)->getValue1());
                programStack->push_back(value);
                break;
            }
            case CodeType::LDC: {
                std::string* strPtr = reinterpret_cast<std::string*>(pCodeList -> at(pc)->getValue1());
                char value;
                if (strPtr->length() == 1)
                    value = strPtr->at(0);
                else
                    value = '\0';
                int intVa = value;
                programStack->push_back(intVa);
                break;
            }
            case CodeType::VARINT :{
                std::string* strPtr = reinterpret_cast<std::string*>(pCodeList -> at(pc)->getValue1());
                VarInfo varInfo(programStack->size(), *strPtr, false, false);
                auto it = tempVarNameToInfoMap->insert({*strPtr, varInfo});
                if (!it.second) {
                    tempVarNameToInfoMap ->erase(*strPtr);
                    tempVarNameToInfoMap ->insert({*strPtr, varInfo});
                }
                break;
            }
            case CodeType::VARCHAR :{
                std::string* strPtr = reinterpret_cast<std::string*>(pCodeList -> at(pc)->getValue1());
                VarInfo varInfo(programStack->size(), *strPtr, true, false);
                auto it = tempVarNameToInfoMap->insert({*strPtr, varInfo});
                if (!it.second) {
                    tempVarNameToInfoMap ->erase(*strPtr);
                    tempVarNameToInfoMap ->insert({*strPtr, varInfo});
                }
                break;
            }
            case CodeType::VARINTARRAY :{
                std::string* strPtr = reinterpret_cast<std::string*>(pCodeList -> at(pc)->getValue1());
                VarInfo varInfo(programStack->size(), *strPtr, false, true);
                auto i = tempVarNameToInfoMap->insert({*strPtr, varInfo});
                if (!i.second) {
                    tempVarNameToInfoMap ->erase(*strPtr);
                    tempVarNameToInfoMap ->insert({*strPtr, varInfo});
                }
                break;
            }
            case CodeType::VARCHARARRAY :{
                std::string* strPtr = reinterpret_cast<std::string*>(pCodeList -> at(pc)->getValue1());
                VarInfo varInfo(programStack->size(), *strPtr, true, true);
                auto i = tempVarNameToInfoMap->insert({*strPtr, varInfo});
                if (!i.second) {
                    tempVarNameToInfoMap ->erase(*strPtr);
                    tempVarNameToInfoMap ->insert({*strPtr, varInfo});
                }
                break;
            }
            case CodeType::PAL :{
                int value = pop();
                int address = (pop());
                setElement(address, value);
                break;
            }
            case CodeType::MAIN :{
                arList->push_back(new AR_Info (pCodeList->size(),tempVarNameToInfoMap,
                                               programStack->size() - 1,0,0));
                tempVarNameToInfoMap = new std::unordered_map<std::string, VarInfo>();
                break;
            }
            case CodeType::FUNC :{
                pc = startPcIndex;
                continue;
            }
            case CodeType::RET :{
                int v1 = *reinterpret_cast<int*>(pCodeList -> at(pc)->getValue1());
                AR_Info* arInfo = popAR_List();
                needParams = arInfo ->paramCount;
                haveParams = arInfo ->haveParams;
                tempVarNameToInfoMap = arInfo -> varNameToInfoMap;
                pc = arInfo -> retPc;
                if (v1 == 1) {//char or int
                    programStack ->erase(programStack -> begin() + arInfo ->stackIndex + 1 - arInfo ->paramCount,
                                         programStack -> end() - 1);
                } else {//return;
                    programStack ->erase(programStack -> begin() + arInfo ->stackIndex + 1 - arInfo ->paramCount,
                                         programStack -> end());
                }
                break;
            }
            case CodeType::PARINT :{
                std::string* strPtr = reinterpret_cast<std::string*>(pCodeList -> at(pc)->getValue1());
                VarInfo param(paramStackIndexList ->at(paramStackIndexList->size() - needParams + haveParams),
                              *strPtr, false, false);
                tempVarNameToInfoMap->insert({*strPtr, param}); //不会覆盖；
                haveParams++;
                if (haveParams == needParams) {
                    paramStackIndexList->erase(paramStackIndexList->end() - haveParams, paramStackIndexList->end());
                }
                break;
            }
            case CodeType::PARCHAR :{
                std::string* strPtr = reinterpret_cast<std::string*>(pCodeList -> at(pc)->getValue1());
                VarInfo param(paramStackIndexList ->at(paramStackIndexList->size() - needParams + haveParams),
                              *strPtr, true, false);
                tempVarNameToInfoMap->insert({*strPtr, param});
                haveParams++;
                if (haveParams == needParams) {
                    paramStackIndexList->erase(paramStackIndexList->end() - haveParams, paramStackIndexList->end());
                }
                break;
            }
            case CodeType::PARINTARRAY :{
                std::string* strPtr = reinterpret_cast<std::string*>(pCodeList -> at(pc)->getValue1());
                VarInfo param(paramStackIndexList ->at(paramStackIndexList->size() - needParams + haveParams),
                              *strPtr, false, true);
                tempVarNameToInfoMap->insert({*strPtr, param});
                haveParams++;
                if (haveParams == needParams) {
                    paramStackIndexList->erase(paramStackIndexList->end() - haveParams, paramStackIndexList->end());
                }
                break;
            }
            case CodeType::PARCHARARRAY :{
                std::string* strPtr = reinterpret_cast<std::string*>(pCodeList -> at(pc)->getValue1());
                VarInfo param(paramStackIndexList ->at(paramStackIndexList->size() - needParams + haveParams),
                              *strPtr, true, true);
                tempVarNameToInfoMap->insert({*strPtr, param});
                haveParams++;
                if (haveParams == needParams) {
                    paramStackIndexList->erase(paramStackIndexList->end() - haveParams, paramStackIndexList->end());
                }
                break;
            }
            case CodeType::APR :{ //已经调用LOD指令，数值已经放到最新地址,所以目标地址就是栈顶
                paramStackIndexList->push_back(programStack->size() - 1);
                break;
            }
            case CodeType::APA :{//已经调用LDA，把数组的地址放到栈顶，所以目标地址是栈顶的数值。
                int address = (programStack->at(programStack->size() - 1));
                paramStackIndexList->push_back(address);
                break;
            }
            case CodeType::CAL :{
                std::string* strPtr = reinterpret_cast<std::string*>(pCodeList -> at(pc)->getValue1());
                FunctInfo functInfo = funNameToPcIndexMap->at(*strPtr);
                AR_Info * arInfo = new AR_Info(pc,tempVarNameToInfoMap,
                                               programStack->size() - 1,functInfo.paramCount,haveParams);
                tempVarNameToInfoMap = new std::unordered_map<std::string, VarInfo>();
                arList->push_back(arInfo);
                pc = functInfo .pcIndex;
                needParams = functInfo.paramCount;
                haveParams = 0;
                continue;
            }
            case CodeType::EXF :{
                //done
                break;
            }
            case CodeType::GETINT :{
                int value = getint();
                push(value);
                break;
            }
            case CodeType::GETCHAR :{
                int value = getchar();
                push(value);
                break;
            }
            case CodeType::PRINTF :{
                int n = *reinterpret_cast<int*>(pCodeList -> at(pc)->getValue2());
                std::string* strPtr = reinterpret_cast<std::string*>(pCodeList -> at(pc)->getValue1());
                std::vector<int> values;
                for (int i = 0; i < n; i++) {
                    values.push_back(pop());
                }
                std::string printfStr;
                int j = n - 1;
                //char本身是int，int + string会把init对应ASCII码转为字符拼接。
                //所以char + string正常加，char本身是int，
                // to_string(char) + string 会把int转成string再加上，反而不对
                for (int i = 0; i < strPtr->size(); i++) {
                    if (strPtr->at(i) == '%' && i < strPtr->size() - 1 && strPtr->at(i + 1) == 'd') {
                            int value = (values.at(j));
                            printfStr += std::to_string(value);
                        j--;
                        i++;
                    } else if (strPtr->at(i) == '%' && i < strPtr->size() - 1 && strPtr->at(i + 1) == 'c') {
                            int value = (values.at(j));
                            char charValue = value;
                            printfStr += charValue; //string +| char正常加，char本身是int，to_string会把int转成string再加
                        j--;
                        i++;
                    } else if(strPtr->at(i) == '\\' && i < strPtr->size() - 1) {
                        char c = strPtr->at(i + 1);
                        if (c == 'n') {
                            printfStr += "\n";
                        } else if (c == 't') {
                            printfStr += "\t";
                        } else if (c == 'a') {
                            printfStr += "\a";
                        } else if(c == 'b') {
                            printfStr += "\b";
                        } else if(c == 'f') {
                            printfStr += "\f";
                        } else if(c == '0') {
                            printfStr += "\0";
                        } else if(c == 'v') {
                            printfStr += "\v";
                        } else {
                            printfStr += strPtr->substr(i, 1); //!!!!
                        }
                        i++;
                    }
                    else {
                        printfStr += strPtr->at(i);
                    }
                }
                FileIO::printToFile_Result(printfStr);
                break;
            }
            case CodeType::LOD :{//注意int x = a[i];这种情况a会调用LOD
                string* strPtr = reinterpret_cast<string*>(pCodeList -> at(pc)->getValue1());
                VarInfo varInfo = getVarInfo(*strPtr);
                int value;
                if (varInfo.isArray) {
                    int skew = (pop());
                     value = programStack->at(varInfo.stackIndex + skew);
                } else {
                    value = programStack->at(varInfo.stackIndex);
                }
                if (varInfo.isChar) {
                    int intValue = (value);
                    char charValue = (char)intValue;
                    push(charValue);
                } else {
                    push(value);
                }
                break;
            }
            case CodeType::LDA :{
                string* strPtr = reinterpret_cast<string*>(pCodeList -> at(pc)->getValue1());
                VarInfo varInfo = getVarInfo(*strPtr);
                if (varInfo.isArray) {
                    int v2 = *reinterpret_cast<int*>(pCodeList -> at(pc)->getValue2());
                    if (v2 >= 0) {
                        push(varInfo.stackIndex + v2);
                    } else {
                        int skew;
                        int value = pop();
                        skew = value;
                        push(varInfo.stackIndex + skew);
                    }
                } else {
                    push(varInfo.stackIndex);
                }
                break;
            }
            case CodeType::EMPTY :{
                string* strPtr = reinterpret_cast<string*>(pCodeList -> at(pc)->getValue1());
                int v2 = *reinterpret_cast<int*>(pCodeList -> at(pc)->getValue2());
                VarInfo varInfo = getVarInfo(*strPtr);
                if (varInfo.isArray) {
                    std::vector<int> valueEDs;
                    for (int i = 0; i < v2; i++) {
                        valueEDs.push_back(pop());
                    }
                    int needSize = (pop());
                    for(int i = valueEDs.size() - 1; i >= 0; i--) {
                        push(valueEDs.at(i));
                    }
                    for (int i = 0; i < needSize - valueEDs.size(); i++) {
                        push(0);
                    }
                } else {
                    if (v2 == 0) {
                        push(0);
                    }
                }
                break;
            }
            case CodeType::EXIT :{
                return;
            }
            case CodeType::LABEL :{
                //done
                break;
            }
            case CodeType::JMP :{
                std::string* strPtr = reinterpret_cast<std::string*>(pCodeList -> at(pc)->getValue1());
                pc = labelNameToPcIndexMap->at(*strPtr);
                break;
            }
            case CodeType::JPF: {
                std::string* strPtr = reinterpret_cast<std::string*>(pCodeList -> at(pc)->getValue1());
                int value = pop();
                if (value != 0) {
                    pc = labelNameToPcIndexMap->at(*strPtr);
                    push(value);
                } else {
                    push(value);
                }
                break;
            }
            case CodeType::JPC :{ //是0就跳转
                int value = pop();
                if (value == 0) {
                    std::string* strPtr = reinterpret_cast<std::string*>(pCodeList -> at(pc)->getValue1());
                    pc = labelNameToPcIndexMap->at(*strPtr);
                    push(value);
                } else {
                    push(value);
                }
                break;
            }
            case CodeType::ADD :{
                int value1 = pop();
                int value2 = pop();
                push(value2 + value1);
                break;
            }
            case CodeType::SUB :{
                int value1 = pop();
                int value2 = pop();
                push(value2 - value1);
                break;
            }
            case CodeType::MUL :{
                int value1 = pop();
                int value2 = pop();
                push(value2 * value1);
                break;
            }
            case CodeType::DIV :{
                int value1 = pop();
                int value2 = pop();
                if (value1 == 0) {
                    cout << "Error: Division by zero" << endl;
                    exit(1);
                }
                push(value2 / value1);
                break;
            }
            case CodeType::MOD :{
                int value1 = pop();
                int value2 = pop();
                if (value1 == 0) {
                    cout << "Error: Division by zero" << endl;
                    exit(1);
                }
                push(value2 % value1);
                break;
            }
            case CodeType::NOT :{
                int value = pop();
                if (value == 0) {
                    push(1);
                } else {
                    push(0);
                }
                break;
            }
            case CodeType::MUS :{
                int value1 = pop();
                push(-value1);
                break;
            }
            case CodeType::OR :{
                int value1 = pop();
                int value2 = pop();
                if (value1 != 0 || value2 != 0) {
                    push(1);
                } else {
                    push(0);
                }
                break;
            }
            case CodeType::AND :{
                int value1 = pop();
                int value2 = pop();
                if (value1 != 0 && value2 != 0) {
                    push(1);
                } else {
                    push(0);
                }
                break;
            }
            case CodeType::EQL :{ //==
                int value1 = pop();
                int value2 = pop();
                if (value1 == value2) {
                    push(1);
                } else {
                    push(0);
                }
                break;
            }
            case CodeType::NEQ :{// !=
                int value1 = pop();
                int value2 = pop();
                if (value1 != value2) {
                    push(1);
                } else {
                    push(0);
                }
                break;
            }
            case CodeType::LSS :{ // <
                int value1 = pop();
                int value2 = pop();
                if (value2 < value1) {
                    push(1);
                } else {
                    push(0);
                }
                break;
            }
            case CodeType::LER :{// <=
                int value1 = pop();
                int value2 = pop();
                if (value2 <= value1) {
                    push(1);
                } else {
                    push(0);
                }
                break;
            }
            case CodeType::GRT :{// >
                int value1 = pop();
                int value2 = pop();
                if (value2 > value1) {
                    push(1);
                } else {
                    push(0);
                }
                break;
            }
            case CodeType::GEQ :{// >=
                int value1 = pop();
                int value2 = pop();
                if (value2 >= value1) {
                    push(1);
                } else {
                    push(0);
                }
                break;
            }
            default: {
                std::cout << "please DO this code: " << codeTypeToStringMap.at(pCodeList -> at(pc)->getType()) << std::endl;
                break;
            }
        }
        pc++;
    }
}

VarInfo Interpreter::getVarInfo(std::string varName) {
    if (tempVarNameToInfoMap->find(varName) != tempVarNameToInfoMap->end()) {
        return tempVarNameToInfoMap->at(varName);
    } else {
        std::unordered_map<std::string, VarInfo>  *overAllVarInfoMap = arList->at(0)->varNameToInfoMap;
        if (overAllVarInfoMap->find(varName) != overAllVarInfoMap->end()) {
            return overAllVarInfoMap->at(varName);
        } else {
            std::cerr << "error varName not found" << std::endl;
            return VarInfo(-1, "", false, false);
        }
    }
}


int Interpreter::pop() {
    if (this -> pCodeList->empty()) {
        std::cerr << "error program stack is empty" << std::endl;
    }
    int value = this -> programStack->back();
    this -> programStack->pop_back();
    return value;
}

void Interpreter::push(int value) {
    this -> programStack->push_back(value);
}

void Interpreter::setElement(int index, int value) {
    if (this -> pCodeList->empty()) {
        std::cerr << "error program stack is empty" << std::endl;
    }
    if (index < 0 || index >= this -> programStack->size()) {
        std::cerr << "error index out of range" << std::endl;
    }
    this -> programStack->at(index) = value;
}

AR_Info* Interpreter::popAR_List() {
    if (this -> arList->empty()) {
        std::cerr << "error AR_List is empty" << std::endl;
    }
    AR_Info* arInfo = this -> arList->back();
    this -> arList->pop_back();
    return arInfo;
}

int getchar(){
    char c;
    scanf("%c",&c);
    return (int)c;
}

int getint(){
    int t;
    scanf("%d",&t);
    while(getchar()!='\n');
    return t;
}


