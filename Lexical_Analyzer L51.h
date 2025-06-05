#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <variant>
#include <tuple>
#include <locale.h>
#include <map>
#include "multiset.h"

using namespace std;

/*
    Реализовать лексический анализатор языка L51 (5 баллов).
        Построить грамматику языка L51 (5 баллов).
*/

/*
            ... made by Alexey Vilkhov, 25 group ...
*/

struct TokenValue {
    string str = "";
    int* pInt = nullptr;
    string* pString = nullptr;
    Multiset* pMultiset = nullptr;
};

enum SymbolicTokenClass {
    DIM,
    AS,
    LET,
    WHILE,
    DO,
    OD,
    FOR,
    FROM,
    BY,
    TO,
    IF,
    ELSE,
    FI,
    READ,
    WRITE,
    LABEL,
    JUMP,
    SELECT,
    IN,
    POW,
    ACS,
    AFM,
    CASE,
    OTHERWISE,
    NI,
    BAD,
    TYPE,
    NUMBER,
    MULTISET,
    IDENTIFIER,
    ADD_OPERATOR,
    MULT_OPERATOR,
    REL_OPERATOR,
    MAIN_OPERATOR,
    ASSIGN_OPERATOR,
    EMPTY_OPERATOR,
    SEPARATOR,
    SIMICOLON,
    SYMBOL,
    DOLLAR,
    COMMENT,
    ERROR
};

//enum Keywords {
//    DIM,
//    AS,
//    LET,
//    WHILE,
//    DO,
//    OD,
//    FOR,
//    FROM,
//    TO,
//    IF,
//    ELSE,
//    FI,
//    READ,
//    WRITE,
//    JUMP,
//    SELECT,
//    IN,
//    CASE,
//    OTHERWISE,
//    NI,
//    BAD
//};



Multiset stringToMultiset(string token);

bool is_VariableInTable(string token);

bool is_LableInTable(string token);

class Tokenizer {
    string input;
    bool comment_flag = false;
    bool jump_flag = false;
    int line = 1;
    size_t pos = 0;
    variant<int*, string*, Multiset*> pTable;
    vector<tuple<SymbolicTokenClass, TokenValue, int>> vector_tokens;
    bool error_flag = false;

    void Add_Const_To_Table(string token);

    void Add_Variable_To_Table(string token);

    void Add_Lable_To_Table(string token);

    void Add_Multiset_To_Table(string token);

    char peek();

    char get();

    void Skip_Spaces();

    void Read_Comment();

    void Read_Identifier();

    void Read_Number();

    void Read_Multiset();

    void Read_Operator();

    void Read_EmptyOperator();

    void Read_Separator();

    bool isErrors();

public:

    vector<int> error_lines;

    vector<tuple<SymbolicTokenClass, TokenValue, int>> Get_Tokens();

    string tokenClassToString(SymbolicTokenClass token_class);

    vector<tuple<SymbolicTokenClass, TokenValue, int>> tokenize(string filename);
};