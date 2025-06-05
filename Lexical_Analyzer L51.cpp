#pragma once
#include "Lexical_Analyzer L51.h"

using namespace std;

/*
    Реализовать лексический анализатор языка L51 (5 баллов).
        Построить грамматику языка L51 (5 баллов).
*/

/*
            ... made by Alexey Vilkhov, 25 group ...
*/

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

unordered_map<string, SymbolicTokenClass> keywords = {
    {"dim", DIM}, {"as", AS}, {"let", LET}, {"while", WHILE}, {"do", DO}, {"od", OD}, {"for", FOR}, {"from", FROM}, {"to", TO}, {"by", BY}, {"if", IF},
    {"else", ELSE}, {"fi", FI}, {"read", READ}, {"write", WRITE}, {"jump", JUMP}, {"select", SELECT}, {"in", SymbolicTokenClass::IN}, {"case", CASE}, {"otherwise", OTHERWISE},
    {"ni", NI}, {"bad", BAD}
};

string separators = "(),:;";

unordered_map<string, SymbolicTokenClass> types = {
    {"int", TYPE}, {"multiset", TYPE}
};

unordered_map<string, SymbolicTokenClass> operators = {
    {"=", ASSIGN_OPERATOR}, {"+", ADD_OPERATOR}, {"-", ADD_OPERATOR}, {"*", MULT_OPERATOR},
    {"/", MULT_OPERATOR}, {"%", MULT_OPERATOR}, {">", REL_OPERATOR}, {"<", REL_OPERATOR}, {">=", REL_OPERATOR},
    {"<=", REL_OPERATOR}, {"==", REL_OPERATOR}, {"!=", REL_OPERATOR}, {"&", MULT_OPERATOR}, {"^", MULT_OPERATOR}, {"pow", MAIN_OPERATOR}, {"acs", MAIN_OPERATOR}, {"afm", MAIN_OPERATOR}
};

vector<int> tableConst;
vector<string> tableVariables;
vector<string> tableLables;
list<Multiset> tableMultisets;

Multiset stringToMultiset(string token) {
    Multiset multiset;
    int number = 0;
    for (int i = 1; i < token.size() - 1; i++) {
        if ((token[i] == ' ') || (token[i] == ','))
            i++;
        while ((token[i] != ' ') && (token[i] != ',') && (token[i] != '\n') && (token[i] != '\0') && ((token[i] != '$')))
            number = number * 10 + (token[i++] - '0');
        multiset.insert(number, 1);
        number = 0;
    }
    return multiset;
}

bool is_VariableInTable(string token) {
    string variable = token;
    auto it = find(tableVariables.begin(), tableVariables.end(), variable);
    if (it == tableVariables.end())
        return false;
    return true;
}

bool is_LableInTable(string token) {
    string label = token;
    auto it = find(tableLables.begin(), tableLables.end(), label);
    if (it == tableLables.end())
        return false;
    return true;
}

void Tokenizer::Add_Const_To_Table(string token) {
    int number = stoi(token);
    auto it = find(tableConst.begin(), tableConst.end(), number);
    if (it == tableConst.end()) {
        tableConst.push_back(number);
        it = find(tableConst.begin(), tableConst.end(), number);
    }
    pTable = &(*it);
}

void Tokenizer:: Add_Variable_To_Table(string token) {
    string variable = token;
    auto it = find(tableVariables.begin(), tableVariables.end(), variable);
    if (it == tableVariables.end()) {
        tableVariables.push_back(token);
        it = find(tableVariables.begin(), tableVariables.end(), variable);
    }
    pTable = &(*it);
}

void Tokenizer::Add_Lable_To_Table(string token) {
    string lable = token;
    auto it = find(tableLables.begin(), tableLables.end(), lable);
    if (it == tableLables.end()) {
        tableLables.push_back(token);
        it = find(tableLables.begin(), tableLables.end(), lable);
    }
    pTable = &(*it);
}

void Tokenizer::Add_Multiset_To_Table(string token) {
    Multiset multiset = stringToMultiset(token);
    auto it = find(tableMultisets.begin(), tableMultisets.end(), multiset);
    if (it == tableMultisets.end()) {
        tableMultisets.push_back(move(multiset));
        it = prev(tableMultisets.end());
    }
    pTable = &(*it);
}

char Tokenizer::peek() {
    return pos < input.size() ? input[pos] : '\0';
}

char Tokenizer::get() {
    return pos < input.size() ? input[pos++] : '\0';
}

void Tokenizer::Skip_Spaces() {
    while (isspace(peek())) {
        if (peek() == '\n') line++;
        pos++;
    }
}

void Tokenizer::Read_Comment() {
    if (!comment_flag) {
        pos++; pos++;
        comment_flag = true;
        TokenValue tokenvalue;
        tokenvalue.pInt = nullptr;
        tokenvalue.pMultiset = nullptr;
        tokenvalue.pString = nullptr;
        tokenvalue.str = "";
        vector_tokens.emplace_back(COMMENT, tokenvalue, line);
    }
    while (pos < input.size()) {
        if (peek() == '*' && input[pos + 1] == '/') {
            pos++; pos++;
            comment_flag = false;
            return;
        }
        if (get() == '\n') {
            line++; pos = 0;
        }
    }
}

void Tokenizer::Read_Identifier() {
    string token;
    while (isalnum(peek()) || peek() == '_') token += get();
    if (peek() == ':') {
        if (keywords.count(token)) {
            error_lines.push_back(line);
            TokenValue tokenvalue;
            tokenvalue.pInt = nullptr;
            tokenvalue.pMultiset = nullptr;
            tokenvalue.pString = nullptr;
            tokenvalue.str = token;
            vector_tokens.emplace_back(ERROR, tokenvalue, line);
            cout << "Error: Label name \"" << token << "\" can't be keyword. Line " << line << endl;
        }
        else if (!is_VariableInTable(token)) {
            Add_Lable_To_Table(token);
            TokenValue tokenvalue;
            tokenvalue.pInt = nullptr;
            tokenvalue.pMultiset = nullptr;
            tokenvalue.pString = std::get<string*>(pTable);
            tokenvalue.str = token;
            vector_tokens.emplace_back(LABEL, tokenvalue, line);
        }
        else {
            error_lines.push_back(line);
            TokenValue tokenvalue;
            tokenvalue.pInt = nullptr;
            tokenvalue.pMultiset = nullptr;
            tokenvalue.pString = nullptr;
            tokenvalue.str = token;
            vector_tokens.emplace_back(ERROR, tokenvalue, line);
            cout << "Error: The label and variable must have different names. Line " << line << endl;
        }
        Read_Separator();
    }
    else if (types.count(token)) {
        TokenValue tokenvalue;
        tokenvalue.pInt = nullptr;
        tokenvalue.pMultiset = nullptr;
        tokenvalue.pString = nullptr;
        tokenvalue.str = token;
        vector_tokens.emplace_back(TYPE, tokenvalue, line);
    }
    else if (keywords.count(token)) {
        if (token == "jump") {
            TokenValue tokenvalue;
            tokenvalue.pInt = nullptr;
            tokenvalue.pMultiset = nullptr;
            tokenvalue.pString = nullptr;
            tokenvalue.str = token;
            vector_tokens.emplace_back(keywords[token], tokenvalue, line);
            jump_flag = true;
        }
        else if (jump_flag) {
            error_lines.push_back(line);
            TokenValue tokenvalue;
            tokenvalue.pInt = nullptr;
            tokenvalue.pMultiset = nullptr;
            tokenvalue.pString = nullptr;
            tokenvalue.str = token;
            vector_tokens.emplace_back(ERROR, tokenvalue, line);
            cout << "Error: Label name \"" << token << "\" can't be keyword. Line " << line << endl;
            jump_flag = false;
        }
        else {
            TokenValue tokenvalue;
            tokenvalue.pInt = nullptr;
            tokenvalue.pMultiset = nullptr;
            tokenvalue.pString = nullptr;
            tokenvalue.str = token;
            vector_tokens.emplace_back(keywords[token], tokenvalue, line);
        }
    }
    else if (operators.count(token)) {
        TokenValue tokenvalue;
        tokenvalue.pInt = nullptr;
        tokenvalue.pMultiset = nullptr;
        tokenvalue.pString = nullptr;
        tokenvalue.str = token;
        vector_tokens.emplace_back(operators[token], tokenvalue, line);
    }
    else {
        if (jump_flag) {
            Add_Lable_To_Table(token);
            TokenValue tokenvalue;
            tokenvalue.pInt = nullptr;
            tokenvalue.pMultiset = nullptr;
            tokenvalue.pString = std::get<string*>(pTable);
            tokenvalue.str = token;
            vector_tokens.emplace_back(LABEL, tokenvalue, line);
            jump_flag = false;
        }
        else if (!is_LableInTable(token)) {
            Add_Variable_To_Table(token);
            TokenValue tokenvalue;
            tokenvalue.pInt = nullptr;
            tokenvalue.pMultiset = nullptr;
            tokenvalue.pString = std::get<string*>(pTable);
            tokenvalue.str = token;
            vector_tokens.emplace_back(IDENTIFIER, tokenvalue, line);
        }
        else {
            error_lines.push_back(line);
            TokenValue tokenvalue;
            tokenvalue.pInt = nullptr;
            tokenvalue.pMultiset = nullptr;
            tokenvalue.pString = nullptr;
            tokenvalue.str = token;
            vector_tokens.emplace_back(ERROR, tokenvalue, line);
            cout << "Error: The variable and label must have different names. Line " << line << endl;
        }
    }
}

void Tokenizer::Read_Number() {
    string token;
    while (isdigit(peek())) token += get();
    if (isalnum(peek()) || peek() == '_') {
        while (isalnum(peek()) || peek() == '_') token += get();
        if (peek() == ':') {
            get();
            error_lines.push_back(line);
            TokenValue tokenvalue;
            tokenvalue.pInt = nullptr;
            tokenvalue.pMultiset = nullptr;
            tokenvalue.pString = nullptr;
            tokenvalue.str = token;
            vector_tokens.emplace_back(ERROR, tokenvalue, line);
            cout << "Error: Label name \"" << token << "\" can't start with a number. Line " << line << endl;
        }
        else if (jump_flag) {
            error_lines.push_back(line);
            TokenValue tokenvalue;
            tokenvalue.pInt = nullptr;
            tokenvalue.pMultiset = nullptr;
            tokenvalue.pString = nullptr;
            tokenvalue.str = token;
            vector_tokens.emplace_back(ERROR, tokenvalue, line);
            cout << "Error: Label name \"" << token << "\" can't start with a number. Line " << line << endl;
            jump_flag = false;
        }
        else {
            error_lines.push_back(line);
            TokenValue tokenvalue;
            tokenvalue.pInt = nullptr;
            tokenvalue.pMultiset = nullptr;
            tokenvalue.pString = nullptr;
            tokenvalue.str = token;
            vector_tokens.emplace_back(ERROR, tokenvalue, line);
            cout << "Error: Variable name \"" << token << "\" can't start with a number. Line " << line << endl;
        }
    }
    else {
        Add_Const_To_Table(token);
        TokenValue tokenvalue;
        tokenvalue.pInt = std::get<int*>(pTable);
        tokenvalue.pMultiset = nullptr;
        tokenvalue.pString = nullptr;
        tokenvalue.str = token;
        vector_tokens.emplace_back(NUMBER, tokenvalue, line);
    }
}

void Tokenizer::Read_Multiset() {
    bool comma = false;
    bool space = true;
    string token;
    token += get();
    while (peek() != '$') {
        if (peek() == '\0') {
            error_lines.push_back(line);
            TokenValue tokenvalue;
            tokenvalue.pInt = nullptr;
            tokenvalue.pMultiset = nullptr;
            tokenvalue.pString = nullptr;
            tokenvalue.str = token;
            vector_tokens.emplace_back(ERROR, tokenvalue, line);
            cout << "Error: Not closed multiset in line " << line << ". Expected '$'" << endl;
            return;
        }
        else if (isdigit(peek())) {
            if (input[pos - 1] == ',')
                token += ' ';
            token += get();
            comma = false;
            space = false;
        }
        else if (peek() == ',') {
            if (!comma) {
                token += get();
                comma = true;
            }
            else {
                cout << "Error: Uncorrect entered multiset in line " << line << ". Maybe you missed a number";
                error_lines.push_back(line);
                TokenValue tokenvalue;
                tokenvalue.pInt = nullptr;
                tokenvalue.pMultiset = nullptr;
                tokenvalue.pString = nullptr;
                tokenvalue.str = token;
                vector_tokens.emplace_back(ERROR, tokenvalue, line);
                pos++;
            }
        }
        else if (peek() == ' ') {
            if (!space) {
                token += get();
                space = true;
            }
            else
                get();
        }
        else {
            cout << "Error: Unexpected symbol '" << get() << "' in multiset on line " << line << ". Maybe you missclicked?" << endl;
        }

    }
    if (space)
        token.pop_back();
    token += get();
    Add_Multiset_To_Table(token);
    TokenValue tokenvalue;
    tokenvalue.pInt = nullptr;
    tokenvalue.pMultiset = std::get<Multiset*>(pTable);
    tokenvalue.pString = nullptr;
    tokenvalue.str = token;
    vector_tokens.emplace_back(MULTISET, tokenvalue, line);
    
}

void Tokenizer::Read_Operator() {
    string token;
    token += get();
    if (operators.count(token + peek())) {
        token += get();
        TokenValue tokenvalue;
        tokenvalue.pInt = nullptr;
        tokenvalue.pMultiset = nullptr;
        tokenvalue.pString = nullptr;
        tokenvalue.str = token;
        vector_tokens.emplace_back(operators[token], tokenvalue, line);
    }
    else if (operators.count(token) && operators.count(string(1, peek()))) {
        while (operators.count(string(1, peek()))) {
            token += get();
        }
        error_lines.push_back(line);
        TokenValue tokenvalue;
        tokenvalue.pInt = nullptr;
        tokenvalue.pMultiset = nullptr;
        tokenvalue.pString = nullptr;
        tokenvalue.str = token;
        vector_tokens.emplace_back(ERROR, tokenvalue, line);
        cout << "Error: Unacceptable operator \"" << token << "\" in line " << line << endl;
    }
    else {
        vector_tokens.emplace_back(operators[token], token, line);
    }
}

void Tokenizer::Read_EmptyOperator() {
    TokenValue tokenvalue;
    tokenvalue.pInt = nullptr;
    tokenvalue.pMultiset = nullptr;
    tokenvalue.pString = nullptr;
    tokenvalue.str = string(1, get());
    vector_tokens.emplace_back(EMPTY_OPERATOR, tokenvalue, line);
}

void Tokenizer::Read_Separator() {
    TokenValue tokenvalue;
    tokenvalue.pInt = nullptr;
    tokenvalue.pMultiset = nullptr;
    tokenvalue.pString = nullptr;
    tokenvalue.str = string(1, get());
    vector_tokens.emplace_back(SEPARATOR, tokenvalue, line);
}

vector<tuple<SymbolicTokenClass, TokenValue, int>> Tokenizer::Get_Tokens() {
    return vector_tokens;
}

string Tokenizer::tokenClassToString(SymbolicTokenClass token_class) {
    switch (token_class) {
    case DIM: return "DIM";
    case AS: return "AS";
    case LET: return "LET";
    case WHILE: return "WHILE";
    case DO: return "DO";
    case OD: return "OD";
    case FOR: return "FOR";
    case FROM: return "FROM";
    case BY: return "BY";
    case TO: return "TO";
    case IF: return "IF";
    case ELSE: return "ELSE";
    case FI: return "FI";
    case READ: return "READ";
    case WRITE: return "WRITE";
    case LABEL: return "LABEL";
    case JUMP: return "JUMP";
    case SELECT: return "SELECT";
    case IN: return "IN";
    case POW: return "POW";
    case ACS: return "ACS";
    case AFM: return "AFM";
    case CASE: return "CASE";
    case OTHERWISE: return "OTHERWISE";
    case NI: return "NI";
    case BAD: return "BAD";
    case TYPE: return "TYPE";
    case NUMBER: return "NUMBER";
    case MULTISET: return "MULTISET";
    case IDENTIFIER: return "IDENTIFIER";
    case ADD_OPERATOR: return "ADD_OPERATOR";
    case MULT_OPERATOR: return "MULT_OPERATOR";
    case REL_OPERATOR: return "REL_OPERATOR";
    case ASSIGN_OPERATOR: return "ASSIGN_OPERATOR";
    case EMPTY_OPERATOR: return "EMPTY_OPERATOR";
    case MAIN_OPERATOR: return "MAIN_OPERATOR";
    case SEPARATOR: return "SEPARATOR";
    case SYMBOL: return "SYMBOL";
    case COMMENT: return "COMMENT";
    case SIMICOLON: return "SIMICOLON";
    case ERROR: return "ERROR";
    default: return "UNKNOWN";
    }
}

vector<tuple<SymbolicTokenClass, TokenValue, int>> Tokenizer::tokenize(string filename) {
    ifstream infile(filename);
    if (infile.is_open()) {
        while (getline(infile, input)) {
            while (pos < input.size()) {
                if (comment_flag)
                    Read_Comment();
                Skip_Spaces();
                if (pos >= input.size()) break;
                char ch = peek();
                if (ch == ';') {
                    if (pos) {
                        if (isspace(input[pos - 1]) || input[pos - 1] == ';')
                            Read_EmptyOperator();
                        else {
                            TokenValue tokenvalue;
                            tokenvalue.pInt = nullptr;
                            tokenvalue.pMultiset = nullptr;
                            tokenvalue.pString = nullptr;
                            tokenvalue.str = string(1, get());
                            vector_tokens.emplace_back(SIMICOLON, tokenvalue, line);
                        }
                    }
                    else Read_EmptyOperator();
                }
                else if (isalpha(ch) || ch == '_') {
                    Read_Identifier();
                }
                else if (ch == '$') {
                    Read_Multiset();
                }
                else if (isdigit(ch)) {
                    Read_Number();
                }
                else if (ch == '/' && input[pos + 1] == '*') {
                    Read_Comment();
                }
                else if (operators.count(string(1, ch))) {
                    Read_Operator();
                }
                else if (separators.find(ch) != string::npos) {
                    Read_Separator();
                }
                else {
                    error_lines.push_back(line);
                    TokenValue tokenvalue;
                    tokenvalue.pInt = nullptr;
                    tokenvalue.pMultiset = nullptr;
                    tokenvalue.pString = nullptr;
                    tokenvalue.str = string(1, get());
                    vector_tokens.emplace_back(ERROR, tokenvalue, line);
                    cout << "Error: Unacceptable symbol '" << ch << "' on line " << line << endl;
                    get();
                }
            }
            pos = 0;
            line++;
        }
        if (comment_flag)
            cout << "Error: Unclosed comment at the end of the file" << endl;
        return vector_tokens;
    }
}

bool Tokenizer::isErrors() {
    return !error_lines.empty();
}