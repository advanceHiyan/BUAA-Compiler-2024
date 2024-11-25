//
// Created by aba15 on 2024/11/24.
//

#include <iostream>
#include "Interpreter.h"
#include "../fileIO/FileIO.h"

Interpreter::Interpreter(std::vector<PCode *> *pCodeList) {
    this->pCodeList = pCodeList;
    this->programStack = new std:: vector<VariantType>();
    this->funNameToPcIndexMap = new std::unordered_map<std::string,FunctInfo>();
    this->tempVarNameToInfoMap = new std::unordered_map<std::string, VarInfo>();
    this -> paramStackIndexList = new std::vector<int>();
    this -> arList = new std::vector<AR_Info*>();
    for (int i = 0; i < pCodeList -> size(); i++) {
        if (pCodeList -> at(i)->getType() == CodeType::MAIN) {
            startPcIndex = i;
        } else if (pCodeList -> at(i)->getType() == CodeType::FUNC) {
            std::string* strPtr = reinterpret_cast<std::string*>(pCodeList -> at(i)->getValue1());
            int paramCount = *reinterpret_cast<int*>(pCodeList -> at(i)->getValue2());
            FunctInfo functInfo(*strPtr, i + 1,paramCount);
            funNameToPcIndexMap->insert({*strPtr, functInfo});
        } else if (pCodeList -> at(i)->getType() == CodeType::LABEL) {
            // TODO: handle label
        } else if (pCodeList -> at(i)->getType() == CodeType::OVERFUN &&
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
            case CodeType::PUSHINT: {
                int value = *reinterpret_cast<int*>(pCodeList -> at(pc)->getValue1());
                programStack->push_back(value);
                break;
            }
            case CodeType::PUSHCHAR: {
                std::string* strPtr = reinterpret_cast<std::string*>(pCodeList -> at(pc)->getValue1());
                programStack->push_back(*strPtr);
                break;
            }
            case CodeType::VARINT :{
                std::string* strPtr = reinterpret_cast<std::string*>(pCodeList -> at(pc)->getValue1());
                VarInfo varInfo(programStack->size(), *strPtr, false, false);
                tempVarNameToInfoMap->insert({*strPtr, varInfo});
                break;
            }
            case CodeType::VARCHAR :{
                std::string* strPtr = reinterpret_cast<std::string*>(pCodeList -> at(pc)->getValue1());
                VarInfo varInfo(programStack->size(), *strPtr, true, false);
                tempVarNameToInfoMap->insert({*strPtr, varInfo});
                break;
            }
            case CodeType::VARINTARRAY :{
                std::string* strPtr = reinterpret_cast<std::string*>(pCodeList -> at(pc)->getValue1());
                VarInfo varInfo(programStack->size(), *strPtr, false, true);
                tempVarNameToInfoMap->insert({*strPtr, varInfo});
                break;
            }
            case CodeType::VARCHARARRAY :{
                std::string* strPtr = reinterpret_cast<std::string*>(pCodeList -> at(pc)->getValue1());
                VarInfo varInfo(programStack->size(), *strPtr, true, true);
                tempVarNameToInfoMap->insert({*strPtr, varInfo});
                break;
            }
            case CodeType::POPLVAL :{
                std::variant value = pop();
                int address = std::get<int>(pop());
                setElement(address, value);
                break;
            }
            case CodeType::MAIN :{
                arList->push_back(new AR_Info (pCodeList->size(),tempVarNameToInfoMap,
                                               programStack->size() - 1,0,0));
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
                tempVarNameToInfoMap->insert({*strPtr, param});
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
            case CodeType::APINT :{ //已经调用VALUE指令，数值已经放到最新地址,所以目标地址就是栈顶
                paramStackIndexList->push_back(programStack->size() - 1);
                break;
            }
            case CodeType::APINTARRAY :{//已经调用VALUE，把数组的地址放到栈顶，所以目标地址是栈顶的数值。
                int address = std :: get<int>(programStack->at(programStack->size() - 1));
                paramStackIndexList->push_back(address);
                break;
            }
            case CodeType::CALL :{
                std::string* strPtr = reinterpret_cast<std::string*>(pCodeList -> at(pc)->getValue1());
                FunctInfo functInfo = funNameToPcIndexMap->at(*strPtr);
                AR_Info * arInfo = new AR_Info(pc,tempVarNameToInfoMap,
                                               programStack->size() - 1,functInfo.paramCount,haveParams);
                arList->push_back(arInfo);
                pc = functInfo .pcIndex;
                needParams = functInfo.paramCount;
                haveParams = 0;
                continue;
            }
            case CodeType::OVERFUN :{
                //done
                break;
            }
            case CodeType::GETINT :{
                int value;
                std::cin >> value;
                push(value);
                break;
            }
            case CodeType::GETCHAR :{
                std::string value;
                std::cin >> value;
                push(value);
                break;
            }
            case CodeType::PRINTF :{
                int n = *reinterpret_cast<int*>(pCodeList -> at(pc)->getValue2());
                std::string* strPtr = reinterpret_cast<std::string*>(pCodeList -> at(pc)->getValue1());
                std::vector<VariantType> values;
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
                        if(std::holds_alternative<int>(values.at(j))) {
                            int value = std::get<int>(values.at(j));
                            printfStr += std::to_string(value);
                        } else {
                            std::string tempValue = std::get<std::string>(values.at(j));
                            char value = tempValue.at(0);
                            int intValue = value;
                            printfStr += to_string(intValue);
                        }
                        j--;
                        i++;
                    } else if (strPtr->at(i) == '%' && i < strPtr->size() - 1 && strPtr->at(i + 1) == 'c') {
                        if(std::holds_alternative<int>(values.at(j))) {
                            int value = std::get<int>(values.at(j));
                            char charValue = value;
                            printfStr += charValue; //string +| char正常加，char本身是int，to_string会把int转成string再加
                        } else {
                            std::string value = std::get<std::string>(values.at(j));
                            printfStr += value;
                        }
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
            case CodeType::VALUE :{
                string* strPtr = reinterpret_cast<string*>(pCodeList -> at(pc)->getValue1());
                VarInfo varInfo = getVarInfo(*strPtr);
                if (varInfo.isArray) {
                    int v2 = *reinterpret_cast<int*>(pCodeList -> at(pc)->getValue2());
                    if (v2 == 0) {//数组，但是没有下标，把地址放到栈顶
                        push(varInfo.stackIndex);
                    } else {//数组，并且有下标，把它的值放到栈顶
                        int skew = std::get<int>(pop());
                        push(programStack->at(varInfo.stackIndex + skew));
                    }
                } else {
                    push(programStack->at(varInfo.stackIndex));
                }
                break;
            }
            case CodeType::ADDRESS :{
                string* strPtr = reinterpret_cast<string*>(pCodeList -> at(pc)->getValue1());
                VarInfo varInfo = getVarInfo(*strPtr);
                if (varInfo.isArray) {
                    int skew = std::get<int>(pop());
                    push(varInfo.stackIndex + skew);
                } else {
                    push(varInfo.stackIndex);
                }
                break;
            }
            case CodeType::PLACESPACE :{
                string* strPtr = reinterpret_cast<string*>(pCodeList -> at(pc)->getValue1());
                int v2 = *reinterpret_cast<int*>(pCodeList -> at(pc)->getValue2());
                VarInfo varInfo = getVarInfo(*strPtr);
                VariantType placeValue = 0;
                if (varInfo.isChar) {
                    placeValue = "\0";
                }
                if (varInfo.isArray) {
                    std::vector<VariantType> valueEDs;
                    for (int i = 0; i < v2; i++) {
                        valueEDs.push_back(pop());
                    }
                    int needSize = std::get<int>(pop());
                    for(int i = valueEDs.size() - 1; i >= 0; i--) {
                        push(valueEDs.at(i));
                    }
                    for (int i = 0; i < needSize - valueEDs.size(); i++) {
                        push(placeValue);
                    }
                } else {
                    if (v2 == 0) {
                        push(placeValue);
                    }
                }
                break;
            }
            case CodeType::EXIT :{
                return;
            }
            case CodeType::LABEL :{
                //TODO: handle label
                break;
            }
            case CodeType::ADD :{
                VariantType topElement1 = pop();
                VariantType topElement2 = pop();
                if (std::holds_alternative<int>(topElement1)) {
                    int value1 = std::get<int>(topElement1);
                    if (std::holds_alternative<int>(topElement2)) {
                        int value2 = std::get<int>(topElement2);
                        push(value2 + value1);
                    } else {
                        std::string tempValue2 = std::get<std::string>(topElement2);
                        char value2 = tempValue2.at(0);
                        push(value2 + value1);
                    }
                } else {
                    std::string tempValue1 = std::get<std::string>(topElement1);
                    char value1 = tempValue1.at(0);
                    if (std::holds_alternative<int>(topElement2)) {
                        int value2 = std::get<int>(topElement2);
                        push(value2 + value1);
                    } else {
                        std::string tempValue2 = std::get<std::string>(topElement2);
                        char value2 = tempValue2.at(0);
                        push(value2 + value1);
                    }
                }
                break;
            }
            case CodeType::SUB :{
                VariantType topElement1 = pop();
                VariantType topElement2 = pop();
                if (std::holds_alternative<int>(topElement1)) {
                    int value1 = std::get<int>(topElement1);
                    if (std::holds_alternative<int>(topElement2)) {
                        int value2 = std::get<int>(topElement2);
                        push(value2 - value1);
                    } else {
                        std::string tempValue2 = std::get<std::string>(topElement2);
                        char value2 = tempValue2.at(0);
                        push(value2 - value1);
                    }
                } else {
                    std::string tempValue1 = std::get<std::string>(topElement1);
                    char value1 = tempValue1.at(0);
                    if (std::holds_alternative<int>(topElement2)) {
                        int value2 = std::get<int>(topElement2);
                        push(value2 - value1);
                    } else {
                        std::string tempValue2 = std::get<std::string>(topElement2);
                        char value2 = tempValue2.at(0);
                        push(value2 - value1);
                    }
                }
                break;
            }
            case CodeType::MULT :{
                VariantType topElement1 = pop();
                VariantType topElement2 = pop();
                if (std::holds_alternative<int>(topElement1)) {
                    int value1 = std::get<int>(topElement1);
                    if (std::holds_alternative<int>(topElement2)) {
                        int value2 = std::get<int>(topElement2);
                        push(value2 * value1);
                    } else {
                        std::string tempValue2 = std::get<std::string>(topElement2);
                        char value2 = tempValue2.at(0);
                        push(value2 * value1);
                    }
                } else {
                    std::string tempValue1 = std::get<std::string>(topElement1);
                    char value1 = tempValue1.at(0);
                    if (std::holds_alternative<int>(topElement2)) {
                        int value2 = std::get<int>(topElement2);
                        push(value2 * value1);
                    } else {
                        std::string tempValue2 = std::get<std::string>(topElement2);
                        char value2 = tempValue2.at(0);
                        push(value2 * value1);
                    }
                }
                break;
            }
            case CodeType::DIV :{
                VariantType topElement1 = pop();
                VariantType topElement2 = pop();
                if (std::holds_alternative<int>(topElement1)) {
                    int value1 = std::get<int>(topElement1);
                    if (std::holds_alternative<int>(topElement2)) {
                        int value2 = std::get<int>(topElement2);
                        push(value2 / value1);
                    } else {
                        std::string tempValue2 = std::get<std::string>(topElement2);
                        char value2 = tempValue2.at(0);
                        push(value2 / value1);
                    }
                } else {
                    std::string tempValue1 = std::get<std::string>(topElement1);
                    char value1 = tempValue1.at(0);
                    if (std::holds_alternative<int>(topElement2)) {
                        int value2 = std::get<int>(topElement2);
                        push(value2 / value1);
                    } else {
                        std::string tempValue2 = std::get<std::string>(topElement2);
                        char value2 = tempValue2.at(0);
                        push(value2 / value1);
                    }
                }
                break;
            }
            case CodeType::MOD :{
                VariantType topElement1 = pop();
                VariantType topElement2 = pop();
                if (std::holds_alternative<int>(topElement1)) {
                    int value1 = std::get<int>(topElement1);
                    if (std::holds_alternative<int>(topElement2)) {
                        int value2 = std::get<int>(topElement2);
                        push(value2 % value1);
                    } else {
                        std::string tempValue2 = std::get<std::string>(topElement2);
                        char value2 = tempValue2.at(0);
                        push(value2 % value1);
                    }
                } else {
                    std::string tempValue1 = std::get<std::string>(topElement1);
                    char value1 = tempValue1.at(0);
                    if (std::holds_alternative<int>(topElement2)) {
                        int value2 = std::get<int>(topElement2);
                        push(value2 % value1);
                    } else {
                        std::string tempValue2 = std::get<std::string>(topElement2);
                        char value2 = tempValue2.at(0);
                        push(value2 % value1);
                    }
                }
                break;
            }
            case CodeType::NOT :{
                //TODO: handle not
                break;
            }
            case CodeType::MINU :{
                VariantType topElement1 = pop();
                if (std::holds_alternative<int>(topElement1)) {
                    int value1 = std::get<int>(topElement1);
                    push(-value1);
                } else {
                    std::string tempValue1 = std::get<std::string>(topElement1);
                    char value1 = tempValue1.at(0);
                    push(-value1);
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


VariantType Interpreter::pop() {
    if (this -> pCodeList->empty()) {
        std::cerr << "error program stack is empty" << std::endl;
    }
    VariantType value = this -> programStack->back();
    this -> programStack->pop_back();
    return value;
}

void Interpreter::push(VariantType value) {
    this -> programStack->push_back(value);
}

void Interpreter::setElement(int index, VariantType value) {
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


