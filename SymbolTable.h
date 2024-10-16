//
// Created by aba15 on 2024/10/15.
//

#ifndef COMPILER_SYMBOLTABLE_H
#define COMPILER_SYMBOLTABLE_H

#include <string>
#include <vector>
#include "ConstType.h"

struct Symbol {
    int id;
    std::string name;
    SymbolType type;
    int value;
    int blockNum;

    Symbol(std::string name, SymbolType type,int blockNum) {
        this->name = name;
        this->type = type;
        this->blockNum = blockNum;
    }
};

class SymbolTable {
    public:
        int id;
        std::string name;
        TableType tableType;
        SymbolTable *lowerFloorTable;//指向上一层的符号表，一直有用
//        std::vector<SymbolTable*> upperFloorTables;//只有语义分析有用，后面没用
//        SymbolTable *prevTable;//指向返回的符号表而不是上一层的符号表,语义分析用不到
        int BlockNum;
        SymbolTable(std::string name,SymbolTable *lowerFloorTable, int BlockNum, TableType tableType) {
            this->name = name;
            this->lowerFloorTable = lowerFloorTable;
            this->BlockNum = BlockNum;
            this->tableType = tableType;
        };
        void addSymbol(std::string name, Symbol *symbol) {
            symbols[name] = symbol;
        };
//        void setPrevTable(SymbolTable *prevTable) { this->prevTable = prevTable; }
//        void addUpperFloorTable(SymbolTable *upperFloorTable) { upperFloorTables.push_back(upperFloorTable); }

    private:
        std::unordered_map<std::string, Symbol*> symbols;

};




#endif //COMPILER_SYMBOLTABLE_H
