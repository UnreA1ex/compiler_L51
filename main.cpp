/*
    Описать язык программирования, основанный на обратной польской записи.
    Реализовать лексический анализатор и интерпретатор этого языка.
*/

#include <iostream>
#include <fstream>
#include <stack>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <variant>
#include <locale.h>
#include <map>
#include "multiset.h"

using namespace std;

enum SymbolicTokenClass {
    LETTER,
    DIGIT,
    AR_OP,
    RELATION,
    SPACE,
    LF,
    SEMICOLON,
    DOLLAR,
    COMMA,
    COMMENT,
    PUSH,
    POP,
    JMP,
    JI,
    POW,
    AFM,      // Add First Missing
    ACS,      // Access
    READ,
    WRITE,
    END,
    END_MARKER,
    ERROR
};

enum States {
    A1, A2,
    B1,
    C1,
    D1,
    E1, E2, E3,
    F1, F2, F3,
    G1,
    H1,
    I1, I2,
    J1,
    K1, K2, K3, K4,
    L1, L2, L3, L4,
    STOP
};

enum Relations {
    NOT,
    EQUAL,
    NOT_EQUAL,
    LESS,
    MORE,
    LESS_EQUAL,
    MORE_EQUAL,
    IN
};

map<string, int>Variable_Int;
map<string, Multiset>Variable_Multiset;

vector<int> tableConst;
vector<string> tableVariables;
list<Multiset> tableMultisets;

struct SymbolicToken
{
    SymbolicTokenClass token_class; // Класс лексемы.
    variant<int, string, Multiset*> value;     // Значение лексемы.
    int line;                       // Номер строки
};

string tokenClassToString(SymbolicTokenClass token_class) {
    switch (token_class) {
    case LETTER: return "LETTER";
    case DIGIT: return "DIGIT";
    case AR_OP: return "AR_OP";
    case RELATION: return "RELATION";
    case SPACE: return "SPACE";
    case LF: return "LF";
    case SEMICOLON: return "SEMICOLON";
    case COMMENT: return "COMMENT";
    case IN: return "IN";
    case PUSH: return "PUSH";
    case POP: return "POP";
    case POW: return "POW";
    case ACS: return "ACS";
    case AFM: return "AFM";
    case JMP: return "JMP";
    case JI: return "JI";
    case READ: return "READ";
    case WRITE: return "WRITE";
    case END: return "END";
    case END_MARKER: return "END_MARKER";
    case ERROR: return "ERROR";
    default: return "UNKNOWN";
    }
}

const unordered_set<string> keywords = {
    "end", "ji", "jmp", "pop", "push", "read", "write", "pow", "acs", "afm", "in"
};

bool isKeyword(const string word) {
    return keywords.find(word) != keywords.end();
}

void printSymbolicTokens(const vector<SymbolicToken>& tokens) {
    for (const auto& token : tokens) {
        cout << "Token Class: " << tokenClassToString(token.token_class) << ", Value: ";
        if (holds_alternative<int>(token.value))
            cout << get<int>(token.value);
        else if (holds_alternative<string>(token.value))
            cout << get<string>(token.value);
        else
            cout << *get<Multiset*>(token.value);
        cout << ", Line: " << token.line << endl;
    }
}

SymbolicToken transliterator(int ch)
{
    SymbolicToken s;
    get<int>(s.value) = 0;
    if (isdigit(ch))
    {
        s.token_class = DIGIT;
        get<int>(s.value) = ch - '0';
    }
    else if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z')) {
        s.token_class = LETTER;
        get<int>(s.value) = ch;
    }
    else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || ch == '&' || ch == '^') {
        s.token_class = AR_OP;
        get<int>(s.value) = ch;
    }
    else if (ch == ',') {
        s.token_class = COMMA;
        get<int>(s.value) = ch;
    }
    else if (ch == '$') {
        s.token_class = DOLLAR;
        get<int>(s.value) = ch;
    }
    else if (ch == '!') {
        s.token_class = RELATION;
        get<int>(s.value) = NOT;
    }
    else if (ch == '=') {
        s.token_class = RELATION;
        get<int>(s.value) = EQUAL;
    }
    else if (ch == '<') {
        s.token_class = RELATION;
        get<int>(s.value) = LESS;
    }
    else if (ch == '>') {
        s.token_class = RELATION;
        get<int>(s.value) = MORE;
    }
    else if (ch == ' ' || ch == '\t')
    {
        s.token_class = SPACE;
    }
    else if (ch == '\n') {
        s.token_class = LF;
    }
    else if (ch == ';') {
        s.token_class = SEMICOLON;
        get<int>(s.value) = ';';
    }
    else if (ch == EOF)
    {
        s.token_class = END;
    }
    else
    {
        s.token_class = ERROR;
    }
    return s;
}

struct KeywordDetection {

    enum Transition {
        B1b,
        C1b, C1c, C1d,
        E1a,
        E2a, E2b,
        E3a,
        L1a, L2a, L3a, L4a
    };

    // Начальный вектор
    const vector<vector<int>> initial_vector = {
        {(int)'e', 0},
        {(int)'j', 2},
        {(int)'p', 5},
        {(int)'r', 10},
        {(int)'w', 13},
        {(int)'a', 18},
        {(int)'i', 22}
    };

    // Таблица обнаружения
    const vector<vector<int>> detection_table = {
        {(int)'n', -1, B1b},
        {(int)'d', -1, C1b},
        {(int)'i', 3, E2a},
        {(int)'m', -1, B1b},
        {(int)'p', -1, E2b},
        {(int)'o', 7, B1b},
        {(int)'p', 17, E3a},
        {(int)'u', -1, B1b},
        {(int)'s', -1, B1b},
        {(int)'h', -1, E1a},
        {(int)'e', -1, B1b},
        {(int)'a', -1, B1b},
        {(int)'d', -1, C1c},
        {(int)'r', -1, B1b},
        {(int)'i', -1, B1b},
        {(int)'t', -1, B1b},
        {(int)'e', -1, C1d},
        {(int)'w', -1, L1a},
        {(int)'c', 20, B1b},
        {(int)'s', -1, L2a},
        {(int)'f', -1, B1b},
        {(int)'m', -1, L3a},
        {(int)'n', -1, L4a}
    };
};

// Таблица обнаружения ключевых слов
const KeywordDetection detect;

class Tokenizer {
    vector<SymbolicToken> vector;
    int ch;                                  // Входной символ.
    SymbolicToken s;                         // Символьная лексема.
    SymbolicTokenClass token;                // Класс лексемы
    int value;                               // Значение лексем
    variant<int*, string*, Multiset*> pTable;        // Указатель на таблицу имён лексем PUSH и POP
    Relations relation;                      // Первый символ отношения
    string variable;                         // Имя переменной
    int detection;                           // Номер позиции в таблице обнаружения для поиска ключевых слов
    int line = 1;                            // Номер текущей строки в программе
    bool flag4const;                         // 1 - константа после ключевого слова push, 0 - номер строки после слова jmp или ji
    int state = A1;                          // Текущее состояние автомата.
    int sign;                                // Регистр знака.
    int number;                              // Регистр числа.
    Multiset multiset;

public:

    std::vector<SymbolicToken> Get_Tokens() {
        return vector;
    }

    void Add_To_Multiset() {
        multiset.insert(number, 1);
    }

    void Create_Token() {
        SymbolicToken token_;
        token_.token_class = token;
        if (token_.token_class == PUSH || token_.token_class == POP) {
            if (holds_alternative<int*>(pTable))
                token_.value = *get<int*>(pTable);
            else if (holds_alternative<string*>(pTable))
                token_.value = *get<string*>(pTable);
            else
                token_.value = get<Multiset*>(pTable);
        }
        else {
            if (token_.token_class == AR_OP)
                token_.value = value;
            else if (token_.token_class == JI)
                token_.value = number;
            else if (token_.token_class == JMP)
                token_.value = number;
            else if (token_.token_class == RELATION)
                token_.value = relation;
        }
        token_.line = line;
        vector.push_back(token_);
    }

    void Error_Handling() {
        token = ERROR;
        Create_Token();
        cout << "Обнаружена ошибка в строке " << line << endl;
        state = J1;
    }

    void Add_Const_To_Table() {
        int number_ = number;
        auto it = find(tableConst.begin(), tableConst.end(), number_);
        if (it == tableConst.end()) {
            tableConst.push_back(number_);
            it = find(tableConst.begin(), tableConst.end(), number_);
        }
        pTable = &(*it);
    }

    void Add_Variable_To_Table() {
        if (isKeyword(variable))
            Error_Handling();
        string variable_ = variable;
        auto it = find(tableVariables.begin(), tableVariables.end(), variable_);
        if (it == tableVariables.end()) {
            tableVariables.push_back(variable);
            it = find(tableVariables.begin(), tableVariables.end(), variable_);
        }
        pTable = &(*it);
    }

    void Add_Multiset_To_Table() {
        auto it = find(tableMultisets.begin(), tableMultisets.end(), multiset);
        if (it == tableMultisets.end()) {
            tableMultisets.push_back(multiset);
            it = prev(tableMultisets.end());
        }
        pTable = &(*it);
    }

    void A1a() {
        state = A1;
        Create_Token();
        line++;
    }

    void A1b() {
        state = A1;
        line++;
    }

    void A2a() {
        state = A2;
        line++;
    }

    void A2b() {
        state = A2;
        Create_Token();
        line++;
    }

    void A2c() {
        state = A2;
        if (flag4const)
            Add_Const_To_Table();
        Create_Token();
        line++;
    }

    void A2d() {
        state = A2;
        Add_Variable_To_Table();
        Create_Token();
        line++;
    }

    void A2e() {
        state = A2;
        if (relation == NOT) {
            Error_Handling();
        }
        Create_Token();
        line++;
    }

    void A2f() {
        state = A2;
        token = ERROR;
        Create_Token();
        line++;
    }

    void A2g() {
        state = A2;
        Add_Multiset_To_Table();
        multiset.clear();
        Create_Token();
        line++;
    }

    void A2h() {
        state = A2;
        token = POW;
        Create_Token();
        line++;
    }

    void A2i() {
        state = A2;
        token = ACS;
        Create_Token();
        line++;
    }

    void A2j() {
        state = A2;
        token = AFM;
        Create_Token();
        line++;
    }

    void A2k() {
        state = A2;
        Create_Token();
        line++;
    }

    void B1a() {
        state = B1;
        for (int i = 0; i < 7; i++) {
            if (detect.initial_vector[i][0] == get<int>(s.value)) {
                detection = detect.initial_vector[i][1];
                return;
            }
        }
        Error_Handling();
    }

    void B1b() {
        state = B1;
        detection++;
    }

    void C1a() {
        state = C1;
        token = AR_OP;
        value = get<int>(s.value);
        Create_Token();
    }

    void C1b() {
        state = C1;
        token = END;
        Create_Token();
    }

    void C1c() {
        state = C1;
        token = READ;
        Create_Token();
    }

    void C1d() {
        state = C1;
        token = WRITE;
        Create_Token();
    }

    void C1e() {
        state = C1;
        int number_ = number;
        auto it = find(tableConst.begin(), tableConst.end(), number_);
        if (flag4const)
            Add_Const_To_Table();
        Create_Token();
    }

    void C1f() {
        state = C1;
        Add_Variable_To_Table();
        Create_Token();
    }

    void C1g() {
        state = C1;
        if (relation == NOT) {
            Error_Handling();
        }
        Create_Token();
    }

    void C1h() {
        state = C1;
        if (relation == LESS and Relations(get<int>(s.value)) == EQUAL)
            relation = LESS_EQUAL;
        else if (relation == MORE and Relations(get<int>(s.value)) == EQUAL)
            relation = MORE_EQUAL;
        else if (relation == NOT and Relations(get<int>(s.value)) == EQUAL)
            relation = NOT_EQUAL;
        else Error_Handling();
        Create_Token();
    }

    void D1a() {
        state = D1;
        token = RELATION;
        relation = (Relations)get<int>(s.value);
    }

    void E1a() {
        state = E1;
        token = PUSH;
        flag4const = true;
    }

    void E2a() {
        state = E2;
        token = JI;
        flag4const = false;
    }

    void E2b() {
        state = E2;
        token = JMP;
        flag4const = false;
    }

    void E3a() {
        state = E3;
        token = POP;
    }

    void G1a() {
        state = G1;
        number = get<int>(s.value);
    }

    void G1b() {
        state = G1;
        number = 10 * number + get<int>(s.value);
    }

    void H1a() {
        state = H1;
        variable = get<int>(s.value);
    }

    void H1b() {
        state = H1;
        variable.push_back(get<int>(s.value));
    }

    void I1a() {
        state = I1;
        token = COMMENT;
    }

    void I2a() {
        state = I2;
        token = COMMENT;
    }

    void I2b() {
        state = I2;
        if (flag4const)
            Add_Const_To_Table();
        Create_Token();
        token = COMMENT;
    }

    void I2c() {
        state = I2;
        Add_Variable_To_Table();
        Create_Token();
        token = COMMENT;
    }

    void I2d() {
        state = I2;
        if (relation == NOT)
            Error_Handling();
        Create_Token();
        token = COMMENT;
    }

    void I2e() {
        state = I2;
        Create_Token();
        token = COMMENT;
    }

    void I2f() {
        state = I2;
        Create_Token();
        token = POW;
    }

    void I2g() {
        state = I2;
        Create_Token();
        token = ACS;
    }

    void I2h() {
        state = I2;
        Create_Token();
        token = AFM;
    }

    void I2i() {
        state = I2;
        Create_Token();
    }

    void K2a() {
        state = K2;
        number = get<int>(s.value);
        Add_To_Multiset();
    }

    void L1a() {
        state = L1;
        token = POW;
    }

    void L2a() {
        state = L2;
        token = ACS;
    }

    void L3a() {
        state = L3;
        token = AFM;
    }

    void L4a() {
        state = L4;
        token = RELATION;
        relation = IN;
    }

    void M1() {
        if (detect.detection_table[detection][0] == get<int>(s.value)) {
            switch (detect.detection_table[detection][2]) {
            case KeywordDetection::B1b:
                B1b();
                break;
            case KeywordDetection::C1b:
                C1b();
                break;
            case KeywordDetection::C1c:
                C1c();
                break;
            case KeywordDetection::C1d:
                C1d();
                break;
            case KeywordDetection::E1a:
                E1a();
                break;
            case KeywordDetection::E2a:
                E2a();
                break;
            case KeywordDetection::E2b:
                E2b();
                break;
            case KeywordDetection::E3a:
                E3a();
                break;
            case KeywordDetection::L1a:
                L1a();
                break;
            case KeywordDetection::L2a:
                L2a();
                break;
            case KeywordDetection::L3a:
                L3a();
                break;
            case KeywordDetection::L4a:
                L4a();
                break;
            default:
                state = STOP;
                cout << "Беда в таблице обнаружения" << endl;
                Error_Handling();
                break;
            }
        }
        else {
            detection = detect.detection_table[detection][1];
            if (detection == -1)
                Error_Handling();
            else
                M1();
        }
    }

    void EXIT1() {
        state = STOP;
        token = END_MARKER;
        Create_Token();
    }

    void EXIT2() {
        state = STOP;
        if (relation == NOT)
            token = ERROR;
        Create_Token();
        token = END_MARKER;
        Create_Token();
    }

    void EXIT3() {
        state = STOP;
        if (flag4const)
            Add_Const_To_Table();
        Create_Token();
        token = END_MARKER;
        Create_Token();
    }

    void EXIT4() {
        state = STOP;
        Add_Variable_To_Table();
        Create_Token();
        token = END_MARKER;
        Create_Token();
    }

    void EXIT5() {
        state = STOP;
        Create_Token();
        token = END_MARKER;
        Create_Token();
    }

    void EXIT6() {
        state = STOP;
        Add_Multiset_To_Table();
        multiset.clear();
        Create_Token();
        token = END_MARKER;
        Create_Token();
    }

    void parse(const char* filename)
    {
        ifstream in(filename);
        if (!in)
        {
            cout << "Не удалось открыть файл " << filename << endl;
            return;
        }
        bool stop = false;  // Флаг остановки. Цикл завершает работу, когда flag = true.
        while (!stop)
        {
            ch = in.get();
            s = transliterator(ch);
            switch (state)
            {
            case A1:
                switch (s.token_class)
                {
                case LETTER:
                    B1a();
                    break;
                case AR_OP:
                    C1a();
                    break;
                case RELATION:
                    D1a();
                    break;
                case SPACE:
                    state = A1;
                    break;
                case LF:
                    A1b();
                    break;
                case SEMICOLON:
                    I1a();
                    break;
                case END:
                    EXIT1();
                    break;
                default:
                    state = STOP;
                    Error_Handling();
                    break;
                }
                break;
            case A2:
                switch (s.token_class)
                {
                case LETTER:
                    B1a();
                    break;
                case AR_OP:
                    C1a();
                    break;
                case RELATION:
                    D1a();
                    break;
                case SPACE:
                    state = A2;
                    break;
                case LF:
                    A2a();
                    break;
                case SEMICOLON:
                    I2a();
                    break;
                case END:
                    EXIT1();
                    break;
                default:
                    state = STOP;
                    Error_Handling();
                    break;
                }
                break;
            case B1:
                switch (s.token_class)
                {
                case LETTER:
                    M1();
                    break;
                case LF:
                    A2f();
                    break;
                default:
                    state = STOP;
                    Error_Handling();
                    break;
                }
                break;
            case C1:
                switch (s.token_class)
                {
                case SPACE:
                    state = C1;
                    break;
                case LF:
                    A2a();
                    break;
                case SEMICOLON:
                    I2a();
                    break;
                case END:
                    EXIT1();
                    break;
                default:
                    state = STOP;
                    Error_Handling();
                    break;
                }
                break;
            case D1:
                switch (s.token_class)
                {
                case RELATION:
                    C1h();
                    break;
                case SPACE:
                    C1g();
                    break;
                case LF:
                    A2e();
                    break;
                case SEMICOLON:
                    I2d();
                    break;
                case END:
                    EXIT2();
                    break;
                default:
                    state = STOP;
                    Error_Handling();
                    break;
                }
                break;
            case E1:
                switch (s.token_class) {
                case SPACE:
                    state = F1;
                    break;
                case LF:
                    A2f();
                    break;
                default:
                    state = STOP;
                    Error_Handling();
                    break;
                }
                break;
            case E2:
                switch (s.token_class) {
                case SPACE:
                    state = F2;
                    break;
                case LF:
                    A2f();
                    break;
                default:
                    state = STOP;
                    Error_Handling();
                    break;
                }
                break;
            case E3:
                switch (s.token_class) {
                case SPACE:
                    state = F3;
                    break;
                case LF:
                    A2f();
                    break;
                default:
                    state = STOP;
                    Error_Handling();
                    break;
                }
                break;
            case F1:
                switch (s.token_class) {
                case LETTER:
                    H1a();
                    break;
                case DIGIT:
                    G1a();
                    break;
                case DOLLAR:
                    state = K1;
                    break;
                case SPACE:
                    state = F1;
                    break;
                case LF:
                    A2f();
                    break;
                default:
                    state = STOP;
                    Error_Handling();
                    break;
                }
                break;
            case F2:
                switch (s.token_class) {
                case DIGIT:
                    G1a();
                    break;
                case SPACE:
                    state = F2;
                    break;
                case LF:
                    A2f();
                    break;
                default:
                    state = STOP;
                    Error_Handling();
                    break;
                }
                break;
            case F3:
                switch (s.token_class) {
                case LETTER:
                    H1a();
                    break;
                case SPACE:
                    state = F3;
                    break;
                case LF:
                    A2f();
                    break;
                default:
                    state = STOP;
                    Error_Handling();
                    break;
                }
                break;
            case G1:
                switch (s.token_class) {
                case DIGIT:
                    G1b();
                    break;
                case SPACE:
                    C1e();
                    break;
                case LF:
                    A2c();
                    break;
                case SEMICOLON:
                    I2b();
                    break;
                case END:
                    EXIT3();
                    break;
                default:
                    state = STOP;
                    Error_Handling();
                    break;
                }
                break;
            case H1:
                switch (s.token_class) {
                case LETTER:
                    H1b();
                    break;
                case DIGIT:
                    H1b();
                    break;
                case SPACE:
                    C1f();
                    break;
                case LF:
                    A2d();
                    break;
                case SEMICOLON:
                    I2c();
                    break;
                case END:
                    EXIT4();
                    break;
                default:
                    state = STOP;
                    Error_Handling();
                    break;
                }
                break;
            case I1:
                switch (s.token_class)
                {
                case LETTER:
                    break;
                case DIGIT:
                    break;
                case AR_OP:
                    break;
                case RELATION:
                    break;
                case SPACE:
                    break;
                case LF:
                    A1a();
                    break;
                case COMMA:
                    break;
                case DOLLAR:
                    break;
                case SEMICOLON:
                    break;
                case ERROR:
                    break;
                case END:
                    EXIT1();
                    break;
                default:
                    state = STOP;
                    Error_Handling();
                    break;
                }
                break;
            case I2:
                switch (s.token_class)
                {
                case LETTER:
                    break;
                case DIGIT:
                    break;
                case AR_OP:
                    break;
                case RELATION:
                    break;
                case SPACE:
                    break;
                case LF:
                    A2b();
                    break;
                case COMMA:
                    break;
                case DOLLAR:
                    break;
                case SEMICOLON:
                    break;
                case ERROR:
                    break;
                case END:
                    EXIT5();
                    break;
                default:
                    state = STOP;
                    Error_Handling();
                    break;
                }
                break;
            case J1:
                switch (s.token_class)
                {
                case LETTER:
                    break;
                case DIGIT:
                    break;
                case AR_OP:
                    break;
                case RELATION:
                    break;
                case SPACE:
                    break;
                case LF:
                    A2a();
                    break;
                case COMMA:
                    break;
                case DOLLAR:
                    break;
                case SEMICOLON:
                    break;
                case ERROR:
                    break;
                case END:
                    EXIT1();
                    break;
                default:
                    state = STOP;
                    Error_Handling();
                    break;
                }
                break;
            case K1:
                switch (s.token_class)
                {
                case DIGIT:
                    K2a();
                    break;
                case SPACE:
                    break;
                case DOLLAR:
                    state = K4;
                    break;
                case SEMICOLON:

                default:
                    state = STOP;
                    Error_Handling();
                    break;
                }
                break;
            case K2:
                switch (s.token_class) {
                case DOLLAR:
                    state = K4;
                    break;
                case COMMA:
                    state = K3;
                    break;
                default:
                    state = STOP;
                    Error_Handling();
                    break;
                }
                break;
            case K3:
                switch (s.token_class) {
                case SPACE:
                    break;
                case DIGIT:
                    K2a();
                    break;
                default:
                    state = STOP;
                    Error_Handling();
                    break;
                }
                break;
            case K4:
                switch (s.token_class) {
                case LF:
                    A2g();
                    break;
                case SEMICOLON:
                    I2e();
                    break;
                case SPACE:
                    break;
                case END:
                    EXIT6();
                    break;
                default:
                    state = STOP;
                    Error_Handling();
                    break;
                }
                break;
            case L1:
                switch (s.token_class) {
                case LF:
                    A2h();
                    break;
                case SPACE:
                    break;
                case SEMICOLON:
                    I2f();
                    break;
                case END:
                    Create_Token();
                    state = STOP;
                    break;
                default:
                    state = STOP;
                    Error_Handling();
                    break;
                }
                break;
            case L2:
                switch (s.token_class) {
                case LF:
                    A2i();
                    break;
                case SPACE:
                    break;
                case SEMICOLON:
                    I2g();
                    break;
                case END:
                    Create_Token();
                    state = STOP;
                    break;
                default:
                    state = STOP;
                    Error_Handling();
                    break;
                }
                break;
            case L3:
                switch (s.token_class) {
                case LF:
                    A2j();
                    break;
                case SPACE:
                    break;
                case SEMICOLON:
                    I2h();
                    break;
                case END:
                    Create_Token();
                    state = STOP;
                    break;
                default:
                    state = STOP;
                    Error_Handling();
                    break;
                }
                break;
            case L4:
                switch (s.token_class) {
                case LF:
                    A2k();
                    break;
                case SPACE:
                    break;
                case SEMICOLON:
                    I2i();
                    break;
                case END:
                    Create_Token();
                    state = STOP;
                    break;
                default:
                    state = STOP;
                    Error_Handling();
                    break;
                }
                break;
            case STOP:
                stop = true;
                break;
            default:
                state = STOP;
                Error_Handling();
                break;
            }
        }
        in.close();
        //printSymbolicTokens(vector);
    }
};

class Interpreter {
    vector<SymbolicToken> vector;
    stack<variant<int, Multiset>> stack;
    int help_int;
    Multiset help_multiset;
    variant<int, Multiset> for_read;
    int help_line = 0;
    bool flag = false;
public:
    Interpreter(std::vector<SymbolicToken> other_) : vector(other_) {}
    void Execution() {
        auto it = vector.begin();
        auto it1 = vector.begin();
        for (; it != vector.end(); flag == false ? it++ : it) {
            flag = false;
            switch (it->token_class) {
            case PUSH:
                if (holds_alternative<int>(it->value)) {
                    stack.push(get<int>(it->value));
                }
                else {
                    if (holds_alternative<string>(it->value)) {
                        if (Variable_Int.find(get<string>(it->value)) != Variable_Int.end())
                            stack.push(Variable_Int[get<string>(it->value)]);
                        else if (Variable_Multiset.find(get<string>(it->value)) != Variable_Multiset.end())
                            stack.push(Variable_Multiset[get<string>(it->value)]);
                        else {
                            cerr << "Переменная не инициализированна!" << endl;
                            return;
                        }
                    }
                    else {
                        stack.push(*get<Multiset*>(it->value));
                    }
                }
                break;
            case POP:
                if (holds_alternative<string>(it->value)) {
                    if (!stack.empty())
                        if (holds_alternative<int>(stack.top())) {
                            Variable_Int[get<string>(it->value)] = get<int>(stack.top());
                            stack.pop();
                        }
                        else {
                            if (holds_alternative<Multiset>(stack.top())) {
                                Variable_Multiset[get<string>(it->value)] = get<Multiset>(stack.top());
                                stack.pop();
                            }
                            else {
                                cerr << "Неверный тип данных!" << endl;
                                return;
                            }
                        }
                    else {
                        cerr << "Стек пуст!" << endl;
                        return;
                    }
                }
                else {
                    cerr << "Переменная не может быть числом!" << endl;
                    return;
                }
                break;
            case POW:
                help_int = get<Multiset>(stack.top()).Power();
                stack.pop();
                stack.push(help_int);
                break;
            case ACS:
                help_int = get<int>(stack.top());
                stack.pop();
                help_int = get<Multiset>(stack.top())[help_int];
                stack.pop();
                stack.push(help_int);
                break;
            case AFM:
                get<Multiset>(stack.top()).Add_First_Missing();
                break;
            case AR_OP:
                switch (get<int>(it->value)) {
                case '+':
                    if (holds_alternative<int>(stack.top())) {
                        help_int = get<int>(stack.top());
                        stack.pop();
                        if (holds_alternative<int>(stack.top())) {
                            help_int = get<int>(stack.top()) + help_int;
                            stack.pop();
                            stack.push(help_int);
                        }
                        else {
                            if (holds_alternative<Multiset>(stack.top())) {
                                Multiset temp_multiset(help_int, 1);
                                help_multiset = get<Multiset>(stack.top()).Union(temp_multiset);
                                stack.pop();
                                stack.push(help_multiset);
                            }
                            else {
                                cerr << "Неверный тип данных!" << endl;
                                return;
                            }
                        }
                    }
                    else {
                        if (holds_alternative<Multiset>(stack.top())) {
                            help_multiset = get<Multiset>(stack.top());
                            stack.pop();
                            if (holds_alternative<int>(stack.top())) {
                                Multiset temp_multiset(get<int>(stack.top()), 1);
                                help_multiset = temp_multiset.Union(help_multiset);
                                stack.pop();
                                stack.push(help_multiset);
                            }
                            else {
                                if (holds_alternative<Multiset>(stack.top())) {
                                    help_multiset = get<Multiset>(stack.top()).Union(help_multiset);
                                    stack.pop();
                                    stack.push(help_multiset);
                                }
                            }
                        }
                        else {
                            cerr << "Неверный тип данных!" << endl;
                            return;
                        }
                    }
                    break;
                case '-':
                    if (holds_alternative<int>(stack.top())) {
                        help_int = get<int>(stack.top());
                        stack.pop();
                        if (holds_alternative<int>(stack.top())) {
                            help_int >= get<int>(stack.top()) ? help_int = 0 : help_int = get<int>(stack.top()) - help_int;
                            stack.pop();
                            stack.push(help_int);
                        }
                        else {
                            if (holds_alternative<Multiset>(stack.top())) {
                                Multiset temp_multiset(help_int, 1);
                                help_multiset = get<Multiset>(stack.top()).Difference(temp_multiset);
                                stack.pop();
                                stack.push(help_multiset);
                            }
                            else {
                                cerr << "Неверный тип данных!" << endl;
                                return;
                            }
                        }
                    }
                    else {
                        if (holds_alternative<Multiset>(stack.top())) {
                            help_multiset = get<Multiset>(stack.top());
                            stack.pop();
                            if (holds_alternative<int>(stack.top())) {
                                Multiset temp_multiset(get<int>(stack.top()), 1);
                                help_multiset = temp_multiset.Difference(help_multiset);
                                stack.pop();
                                stack.push(help_multiset);
                            }
                            else {
                                if (holds_alternative<Multiset>(stack.top())) {
                                    help_multiset = get<Multiset>(stack.top()).Difference(help_multiset);
                                    stack.pop();
                                    stack.push(help_multiset);
                                }
                            }
                        }
                        else {
                            cerr << "Неверный тип данных!" << endl;
                            return;
                        }
                    }
                    break;
                case '*':
                    if (holds_alternative<int>(stack.top())) {
                        help_int = get<int>(stack.top());
                        stack.pop();
                        if (holds_alternative<int>(stack.top())) {
                            help_int = get<int>(stack.top()) * help_int;
                            stack.pop();
                            stack.push(help_int);
                        }
                        else {
                            if (holds_alternative<Multiset>(stack.top())) {
                                cerr << "Я не могу умножить мультимножество на число :(" << endl;
                                return;
                            }
                            else {
                                cerr << "Неверный тип данных!" << endl;;
                                return;
                            }
                        }
                    }
                    else {
                        if (holds_alternative<Multiset>(stack.top())) {
                            stack.pop();
                            if (holds_alternative<int>(stack.top())) {
                                stack.pop();
                                cerr << "Я не могу умножить число на мультимножество :(" << endl;
                            }
                            else {
                                if (holds_alternative<Multiset>(stack.top())) {
                                    stack.pop();
                                    cerr << "Я не умею перемножать мультимножества :(" << endl;
                                }
                            }
                        }
                        else {
                            cerr << "Неверный тип данных!" << endl;
                            return;
                        }
                    }
                    break;
                case '/':
                    if (holds_alternative<int>(stack.top())) {
                        help_int = get<int>(stack.top());
                        stack.pop();
                        if (holds_alternative<int>(stack.top())) {
                            if (!help_int) {
                                stack.pop();
                                cerr << "ААА, ДЕЛЕНИЕ НА НОЛЬ!!!" << endl;
                            }
                            help_int = get<int>(stack.top()) / help_int;
                            stack.pop();
                            stack.push(help_int);
                        }
                        else {
                            if (holds_alternative<Multiset>(stack.top())) {
                                cerr << "Я не могу делить мультимножество на число :(" << endl;
                                return;
                            }
                            else {
                                cerr << "Неверный тип данных!" << endl;;
                                return;
                            }
                        }
                    }
                    else {
                        if (holds_alternative<Multiset>(stack.top())) {
                            stack.pop();
                            if (holds_alternative<int>(stack.top())) {
                                stack.pop();
                                cerr << "Я не могу поделить число на мультимножество :(" << endl;
                            }
                            else {
                                if (holds_alternative<Multiset>(stack.top())) {
                                    stack.pop();
                                    cerr << "Я не умею делить мультимножества :(" << endl;
                                }
                            }
                        }
                        else {
                            cerr << "Неверный тип данных!" << endl;
                            return;
                        }
                    }
                    break;
                case '%':
                    if (holds_alternative<int>(stack.top())) {
                        help_int = get<int>(stack.top());
                        stack.pop();
                        if (holds_alternative<int>(stack.top())) {
                            if (!help_int) {
                                stack.pop();
                                cerr << "ААА, ДЕЛЕНИЕ НА НОЛЬ!!!" << endl;
                            }
                            help_int = get<int>(stack.top()) % help_int;
                            stack.pop();
                            stack.push(help_int);
                        }
                        else {
                            if (holds_alternative<Multiset>(stack.top())) {
                                cerr << "Я не могу делить мультимножество на число :(" << endl;
                                return;
                            }
                            else {
                                cerr << "Неверный тип данных!" << endl;;
                                return;
                            }
                        }
                    }
                    else {
                        if (holds_alternative<Multiset>(stack.top())) {
                            stack.pop();
                            if (holds_alternative<int>(stack.top())) {
                                stack.pop();
                                cerr << "Я не могу поделить число на мультимножество :(" << endl;
                            }
                            else {
                                if (holds_alternative<Multiset>(stack.top())) {
                                    stack.pop();
                                    cerr << "Я не умею делить мультимножества :(" << endl;
                                }
                            }
                        }
                        else {
                            cerr << "Неверный тип данных!" << endl;
                            return;
                        }
                    }
                    break;
                case '&':
                    if (holds_alternative<int>(stack.top())) {
                        Multiset temp_multiset(get<int>(stack.top()), 1);
                        stack.pop();
                        if (holds_alternative<int>(stack.top())) {
                            Multiset temp_multiset2(get<int>(stack.top()), 1);
                            temp_multiset2 = temp_multiset2.Intersection(temp_multiset);
                            stack.pop();
                            stack.push(temp_multiset2);
                        }
                        else {
                            if (holds_alternative<Multiset>(stack.top())) {
                                help_multiset = get<Multiset>(stack.top()).Intersection(temp_multiset);
                                stack.pop();
                                stack.push(help_multiset);
                            }
                            else {
                                cerr << "Неверный тип данных!" << endl;
                                return;
                            }
                        }
                    }
                    else {
                        if (holds_alternative<Multiset>(stack.top())) {
                            help_multiset = get<Multiset>(stack.top());
                            stack.pop();
                            if (holds_alternative<int>(stack.top())) {
                                Multiset temp_multiset(get<int>(stack.top()), 1);
                                help_multiset = temp_multiset.Intersection(help_multiset);
                                stack.pop();
                                stack.push(help_multiset);
                            }
                            else {
                                if (holds_alternative<Multiset>(stack.top())) {
                                    help_multiset = get<Multiset>(stack.top()).Intersection(help_multiset);
                                    stack.pop();
                                    stack.push(help_multiset);
                                }
                            }
                        }
                        else {
                            cerr << "Неверный тип данных!" << endl;
                            return;
                        }
                    }
                    break;
                case '^':
                    if (holds_alternative<int>(stack.top())) {
                        Multiset temp_multiset(get<int>(stack.top()), 1);
                        stack.pop();
                        if (holds_alternative<int>(stack.top())) {
                            Multiset temp_multiset2(get<int>(stack.top()), 1);
                            temp_multiset2 = temp_multiset2.Symmetric_Difference(temp_multiset);
                            stack.pop();
                            stack.push(temp_multiset2);
                        }
                        else {
                            if (holds_alternative<Multiset>(stack.top())) {
                                help_multiset = get<Multiset>(stack.top()).Symmetric_Difference(temp_multiset);
                                stack.pop();
                                stack.push(help_multiset);
                            }
                            else {
                                cerr << "Неверный тип данных!" << endl;
                                return;
                            }
                        }
                    }
                    else {
                        if (holds_alternative<Multiset>(stack.top())) {
                            help_multiset = get<Multiset>(stack.top());
                            stack.pop();
                            if (holds_alternative<int>(stack.top())) {
                                Multiset temp_multiset(get<int>(stack.top()), 1);
                                help_multiset = temp_multiset.Symmetric_Difference(help_multiset);
                                stack.pop();
                                stack.push(help_multiset);
                            }
                            else {
                                if (holds_alternative<Multiset>(stack.top())) {
                                    help_multiset = get<Multiset>(stack.top()).Symmetric_Difference(help_multiset);
                                    stack.pop();
                                    stack.push(help_multiset);
                                }
                            }
                        }
                        else {
                            cerr << "Неверный тип данных!" << endl;
                            return;
                        }
                    }
                    break;
                }
                break;
            case RELATION:
                switch (get<int>(it->value)) {
                case EQUAL:
                    if (holds_alternative<int>(stack.top())) {
                        help_int = get<int>(stack.top());
                        stack.pop();
                        if (holds_alternative<int>(stack.top())) {
                            help_int = get<int>(stack.top()) == help_int;
                            stack.pop();
                            stack.push(help_int);
                        }
                        else {
                            if (holds_alternative<Multiset>(stack.top())) {
                                Multiset temp_multiset(help_int, 1);
                                help_int = get<Multiset>(stack.top()) == temp_multiset;
                                stack.pop();
                                stack.push(help_int);
                            }
                            else {
                                cerr << "Неверный тип данных!" << endl;
                                return;
                            }
                        }
                    }
                    else {
                        if (holds_alternative<Multiset>(stack.top())) {
                            help_multiset = get<Multiset>(stack.top());
                            stack.pop();
                            if (holds_alternative<int>(stack.top())) {
                                Multiset temp_multiset(get<int>(stack.top()), 1);
                                help_int = temp_multiset == help_multiset;
                                stack.pop();
                                stack.push(help_int);
                            }
                            else {
                                if (holds_alternative<Multiset>(stack.top())) {
                                    help_int = get<Multiset>(stack.top()) == help_multiset;
                                    stack.pop();
                                    stack.push(help_int);
                                }
                            }
                        }
                        else {
                            cerr << "Неверный тип данных!" << endl;
                            return;
                        }
                    }
                    break;
                case NOT_EQUAL:
                    if (holds_alternative<int>(stack.top())) {
                        help_int = get<int>(stack.top());
                        stack.pop();
                        if (holds_alternative<int>(stack.top())) {
                            help_int = get<int>(stack.top()) != help_int;
                            stack.pop();
                            stack.push(help_int);
                        }
                        else {
                            if (holds_alternative<Multiset>(stack.top())) {
                                Multiset temp_multiset(help_int, 1);
                                help_int = get<Multiset>(stack.top()) != temp_multiset;
                                stack.pop();
                                stack.push(help_int);
                            }
                            else {
                                cerr << "Неверный тип данных!" << endl;
                                return;
                            }
                        }
                    }
                    else {
                        if (holds_alternative<Multiset>(stack.top())) {
                            help_multiset = get<Multiset>(stack.top());
                            stack.pop();
                            if (holds_alternative<int>(stack.top())) {
                                Multiset temp_multiset(get<int>(stack.top()), 1);
                                help_int = temp_multiset != help_multiset;
                                stack.pop();
                                stack.push(help_int);
                            }
                            else {
                                if (holds_alternative<Multiset>(stack.top())) {
                                    help_int = get<Multiset>(stack.top()) != help_multiset;
                                    stack.pop();
                                    stack.push(help_int);
                                }
                            }
                        }
                        else {
                            cerr << "Неверный тип данных!" << endl;
                            return;
                        }
                    }
                    break;
                case LESS:
                    if (holds_alternative<int>(stack.top())) {
                        help_int = get<int>(stack.top());
                        stack.pop();
                        if (holds_alternative<int>(stack.top())) {
                            help_int = get<int>(stack.top()) < help_int;
                            stack.pop();
                            stack.push(help_int);
                        }
                        else {
                            if (holds_alternative<Multiset>(stack.top())) {
                                cerr << "Я не могу сравнивать мультимножество и число :(" << endl;
                                return;
                            }
                            else {
                                cerr << "Неверный тип данных!" << endl;;
                                return;
                            }
                        }
                    }
                    else {
                        if (holds_alternative<Multiset>(stack.top())) {
                            stack.pop();
                            if (holds_alternative<int>(stack.top())) {
                                stack.pop();
                                cerr << "Я не могу сравнивать число и мультимножество :(" << endl;
                            }
                            else {
                                if (holds_alternative<Multiset>(stack.top())) {
                                    stack.pop();
                                    cerr << "Я не умею сравнивать мультимножества :(" << endl;
                                }
                            }
                        }
                        else {
                            cerr << "Неверный тип данных!" << endl;
                            return;
                        }
                    }
                    break;
                case MORE:
                    if (holds_alternative<int>(stack.top())) {
                        help_int = get<int>(stack.top());
                        stack.pop();
                        if (holds_alternative<int>(stack.top())) {
                            help_int = get<int>(stack.top()) > help_int;
                            stack.pop();
                            stack.push(help_int);
                        }
                        else {
                            if (holds_alternative<Multiset>(stack.top())) {
                                cerr << "Я не могу сравнивать мультимножество и число :(" << endl;
                                return;
                            }
                            else {
                                cerr << "Неверный тип данных!" << endl;;
                                return;
                            }
                        }
                    }
                    else {
                        if (holds_alternative<Multiset>(stack.top())) {
                            stack.pop();
                            if (holds_alternative<int>(stack.top())) {
                                stack.pop();
                                cerr << "Я не могу сравнивать число и мультимножество :(" << endl;
                            }
                            else {
                                if (holds_alternative<Multiset>(stack.top())) {
                                    stack.pop();
                                    cerr << "Я не умею сравнивать мультимножества :(" << endl;
                                }
                            }
                        }
                        else {
                            cerr << "Неверный тип данных!" << endl;
                            return;
                        }
                    }
                    break;
                case LESS_EQUAL:
                    if (holds_alternative<int>(stack.top())) {
                        help_int = get<int>(stack.top());
                        stack.pop();
                        if (holds_alternative<int>(stack.top())) {
                            help_int = get<int>(stack.top()) <= help_int;
                            stack.pop();
                            stack.push(help_int);
                        }
                        else {
                            if (holds_alternative<Multiset>(stack.top())) {
                                cerr << "Я не могу сравнивать мультимножество и число :(" << endl;
                                return;
                            }
                            else {
                                cerr << "Неверный тип данных!" << endl;;
                                return;
                            }
                        }
                    }
                    else {
                        if (holds_alternative<Multiset>(stack.top())) {
                            stack.pop();
                            if (holds_alternative<int>(stack.top())) {
                                stack.pop();
                                cerr << "Я не могу сравнивать число и мультимножество :(" << endl;
                            }
                            else {
                                if (holds_alternative<Multiset>(stack.top())) {
                                    stack.pop();
                                    cerr << "Я не умею сравнивать мультимножества :(" << endl;
                                }
                            }
                        }
                        else {
                            cerr << "Неверный тип данных!" << endl;
                            return;
                        }
                    }
                    break;
                case MORE_EQUAL:
                    if (holds_alternative<int>(stack.top())) {
                        help_int = get<int>(stack.top());
                        stack.pop();
                        if (holds_alternative<int>(stack.top())) {
                            help_int = get<int>(stack.top()) >= help_int;
                            stack.pop();
                            stack.push(help_int);
                        }
                        else {
                            if (holds_alternative<Multiset>(stack.top())) {
                                cerr << "Я не могу сравнивать мультимножество и число :(" << endl;
                                return;
                            }
                            else {
                                cerr << "Неверный тип данных!" << endl;;
                                return;
                            }
                        }
                    }
                    else {
                        if (holds_alternative<Multiset>(stack.top())) {
                            stack.pop();
                            if (holds_alternative<int>(stack.top())) {
                                stack.pop();
                                cerr << "Я не могу сравнивать число и мультимножество :(" << endl;
                            }
                            else {
                                if (holds_alternative<Multiset>(stack.top())) {
                                    stack.pop();
                                    cerr << "Я не умею сравнивать мультимножества :(" << endl;
                                }
                            }
                        }
                        else {
                            cerr << "Неверный тип данных!" << endl;
                            return;
                        }
                    }
                    break;
                case IN:
                    if (holds_alternative<int>(stack.top())) {
                        Multiset temp_multiset(get<int>(stack.top()), 1);
                        stack.pop();
                        if (holds_alternative<int>(stack.top())) {
                            Multiset temp_multiset2(get<int>(stack.top()), 1);
                            help_int = temp_multiset2.in(temp_multiset);
                            stack.pop();
                            stack.push(help_int);
                        }
                        else {
                            if (holds_alternative<Multiset>(stack.top())) {
                                help_int = get<Multiset>(stack.top()).in(temp_multiset);
                                stack.pop();
                                stack.push(help_int);
                            }
                            else {
                                cerr << "Неверный тип данных!" << endl;
                                return;
                            }
                        }
                    }
                    else {
                        if (holds_alternative<Multiset>(stack.top())) {
                            help_multiset = get<Multiset>(stack.top());
                            stack.pop();
                            if (holds_alternative<int>(stack.top())) {
                                Multiset temp_multiset(get<int>(stack.top()), 1);
                                help_int = temp_multiset.in(help_multiset);
                                stack.pop();
                                stack.push(help_int);
                            }
                            else {
                                if (holds_alternative<Multiset>(stack.top())) {
                                    help_int = get<Multiset>(stack.top()).in(help_multiset);
                                    stack.pop();
                                    stack.push(help_int);
                                }
                            }
                        }
                        else {
                            cerr << "Неверный тип данных!" << endl;
                            return;
                        }
                    }
                    break;
                }
                break;
            case WRITE:
                if (holds_alternative<int>(stack.top()))
                    cout << get<int>(stack.top()) << endl;
                else if (holds_alternative<Multiset>(stack.top()))
                    cout << get<Multiset>(stack.top()) << endl;
                stack.pop();
                break;
            case READ:
                if (cin.peek() == '$') {
                    cin >> help_multiset;
                    stack.push(help_multiset);
                }
                else if (isdigit(cin.peek())) {
                    cin >> help_int;
                    stack.push(help_int);
                }
                else {
                    cerr << "Неверный тип данных!" << endl;
                    return;
                }
                break;
            case COMMENT:
                break;
            case JMP:
                for (it1 = vector.begin(); it1->line < get<int>(it->value); it1++);
                it = it1;
                flag = true;
                break;
            case JI:
                if (holds_alternative<int>(stack.top()))
                    if (!get<int>(stack.top()))
                        break;
                    else {
                        if (holds_alternative<Multiset>(stack.top()))
                            if (get<Multiset>(stack.top()).isEmpty())
                                break;
                    }

                for (it1 = vector.begin(); it1->line < get<int>(it->value); it1++);
                it = it1;
                flag = true;
                break;
            case END:
                return;
            case END_MARKER:
                return;
            }
        }
        /*while (!stack.empty()) {
            if (holds_alternative<int>(stack.top()))
                cout << "Элемент: " << get<int>(stack.top()) << endl;
            else if (holds_alternative<Multiset>(stack.top()))
                cout << "Элемент: " << get<Multiset>(stack.top()) << endl;
            stack.pop();
        }*/
    }
};
//
int main() {
    setlocale(LC_ALL, "Rus");
    Tokenizer tokenizer;
    vector<SymbolicToken> tokens;
    tokenizer.parse("test3.txt");
    tokens = tokenizer.Get_Tokens();
    Interpreter interpreter(tokens);
    interpreter.Execution();
}