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

        bool findSymbolInThis(std::string name) {
            if (symbols.find(name)!= symbols.end()) {
                return true;
            }
            return false;
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
    bool addFunTable(std::string name, FunSymbolTable *funTable) {
        if (funTables.find(name) == funTables.end()) {
            funTables[name] = funTable;
            return true;
        }
        return false;
    }
};

//        std::vector<SymbolTable*> upperFloorTables;//只有语义分析有用，后面没用
//        SymbolTable *prevTable;//指向返回的符号表而不是上一层的符号表,语义分析用不到
//        void setPrevTable(SymbolTable *prevTable) { this->prevTable = prevTable; }
//        void addUpperFloorTable(SymbolTable *upperFloorTable) { upperFloorTables.push_back(upperFloorTable); }


#endif //COMPILER_SYMBOLTABLE_H
