//
// Created by aba15 on 2024/10/15.
//

#ifndef COMPILER_SYMBOLTABLE_H
#define COMPILER_SYMBOLTABLE_H

#include <string>
#include <vector>
#include "ConstType.h"

struct Symbol {
    std::string name;
    SymbolType type;
    int blockNum;

    Symbol(std::string name, SymbolType type,int blockNum) {
        this->name = name;
        this->type = type;
        this->blockNum = blockNum;
    }
};

class SymbolTable {
    public:
        std::string name;
        TableType tableType;
        SymbolTable *lowerFloorTable;//指向上一层的符号表，一直有用
        int BlockNum;

        SymbolTable(std::string name,SymbolTable *lowerFloorTable, int BlockNum, TableType tableType) {
            this->name = name;
            this->lowerFloorTable = lowerFloorTable;
            this->BlockNum = BlockNum;
            this->tableType = tableType;
        };

        virtual void addSymbol(std::string name, Symbol *symbol) {
            symbols[name] = symbol;
        };

        bool findSymbolInThis(std::string name) {//在当前符号表中查找，不查Fun
            if (symbols.find(name)!= symbols.end()) {
                return true;
            }
            return false;
        }

        bool isInFor() {
            if (tableType == TableType::ForBlock) {
                return true;
            } else {
                if(lowerFloorTable== nullptr) {
                    return false;
                }
                return lowerFloorTable->isInFor();
            }
        }

        int isReturnFun() {
            if (tableType == TableType::IntFunc || tableType == TableType::CharFunc) {
                return 1;
            } else if(tableType == TableType::VoidFunc) {
                return 2;
            } else {
                if(lowerFloorTable== nullptr) {
                    return -1;
                }
                return lowerFloorTable->isReturnFun();
            }
        }

        int findSymbolInUpperFloor(std::string name) {
            if (symbols.find(name)!= symbols.end()) {
                if(symbols[name] ->type == SymbolType::ConstCharArray || symbols[name] ->type == SymbolType::ConstIntArray
                || symbols[name] ->type == SymbolType::ConstChar || symbols[name] ->type == SymbolType::ConstInt) {
                    return 2;//常量
                } else if(symbols[name] ->type == SymbolType::Int || symbols[name] ->type == SymbolType::Char ||
                symbols[name] ->type == SymbolType::IntArray || symbols[name] ->type == SymbolType::CharArray) {
                    return 1;//变量
                }
                return 0;//函数
            }
            if(lowerFloorTable== nullptr) {
                return -1;
            }
            return lowerFloorTable->findSymbolInUpperFloor(name);
        }

        int getSymbolFunType(std ::string name) {
            std::cout<<"jhghg" << std::endl;
            int ret = -1;
            if (symbols.find(name)!= symbols.end()) {
                if(symbols[name] ->type == SymbolType::Int || symbols[name] ->type == SymbolType::Char
                || symbols[name] ->type == SymbolType::ConstInt || symbols[name] ->type == SymbolType::ConstChar
                || symbols[name] ->type == SymbolType::IntFunc || symbols[name] ->type == SymbolType::CharFunc) {
                    return 0;
                }
                if(symbols[name] ->type == SymbolType::IntArray || symbols[name] ->type == SymbolType::ConstIntArray) {
                    return 1;
                }
                if(symbols[name] ->type == SymbolType::CharArray || symbols[name] ->type == SymbolType::ConstCharArray) {
                    return 2;
                }
                return -1;
            }
            if(lowerFloorTable== nullptr) {
                return -1;
            }
            return lowerFloorTable->getSymbolFunType(name);
        }

protected:
        std::unordered_map<std::string, Symbol*> symbols;
};

class FunSymbolTable : public SymbolTable {
public:
    FunSymbolTable(std::string name, SymbolTable *lowerFloorTable, int BlockNum, TableType tableType) :
    SymbolTable(name, lowerFloorTable, BlockNum, tableType) {}//不写，默认为（）无参数
    std::vector<SymbolType> paramTypes;
};

class OverallSymbolTable : public SymbolTable {
public:
    OverallSymbolTable(std::string name, SymbolTable *lowerFloorTable, int BlockNum, TableType tableType) :
    SymbolTable(name, lowerFloorTable, BlockNum, tableType) {}//不写，默认为（）无参数

    std::unordered_map<std::string, FunSymbolTable*> funTables;
    //override函数，那么父类必须是虚函数。当然也可以不用virtual和override，但是那样重写，
    // 如果指针是父类指针，对象是子类对象，会导致调用父类的函数，而不是子类的函数。
    bool addFunTable(std::string name,Symbol *symbol ,FunSymbolTable *funTable) {
        if (funTables.find(name) == funTables.end() && !findSymbolInThis(name)) {
            funTables[name] = funTable;
            symbols[name] = symbol;
            return true;
        }
        return false;
    }

    std::vector<SymbolType>* findFunType(std::string name) {
        if (funTables.find(name) != funTables.end()) {
            return &funTables[name]->paramTypes;
        }
        std :: cout << "error: function " << name << " not found" << std:: endl;
        return nullptr;
    }
};

//        std::vector<SymbolTable*> upperFloorTables;//只有语义分析有用，后面没用
//        SymbolTable *prevTable;//指向返回的符号表而不是上一层的符号表,语义分析用不到
//        void setPrevTable(SymbolTable *prevTable) { this->prevTable = prevTable; }
//        void addUpperFloorTable(SymbolTable *upperFloorTable) { upperFloorTables.push_back(upperFloorTable); }


#endif //COMPILER_SYMBOLTABLE_H
