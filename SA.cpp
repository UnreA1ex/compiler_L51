#include "SA.h"



string TokenToString(tuple<SymbolicTokenClass, TokenValue, int> token) {
	switch (get<0>(token)) {
		case DIM: return "dim";
        case AS: return "as";
        case LET: return "let";
        case WHILE: return "while";
        case DO: return "do";
        case OD: return "od";
        case FOR: return "for";
        case FROM: return "from";
        case BY: return "by";
        case TO: return "to";
        case IF: return "if";
        case ELSE: return "else";
        case FI: return "fi";
        case READ: return "read";
        case WRITE: return "write";
        case LABEL: return "L";
        case JUMP: return "jump";
        case SELECT: return "select";
        case IN: return "in";
        case POW: return "pow";
        case ACS: return "acs";
        case AFM: return "afm";
        case CASE: return "case";
        case OTHERWISE: return "otherwise";
        case NI: return "ni";
        case BAD: return "bad";
        case TYPE: return get<1>(token).str;
        case NUMBER: return "C";
        case MULTISET: return "m";
        case IDENTIFIER: return "V";
        case ADD_OPERATOR: return get<1>(token).str;
        case MULT_OPERATOR: return get<1>(token).str;
        case REL_OPERATOR: return "R";
        case ASSIGN_OPERATOR: return "=";
        case EMPTY_OPERATOR: return ";";
        case MAIN_OPERATOR: return get<1>(token).str;
        case SEPARATOR: return get<1>(token).str;
        case SYMBOL: return get<1>(token).str;
        case DOLLAR: return "$";
        case COMMENT: return "/*комментарий*/";
        case SIMICOLON: return ";";
        case ERROR: return "ERROR";
        default: return "UNKNOWN";
	}
}

//void Syntax_Analyzer::Parse(string filename_P, ifstream& file_G) {
//    size_t len = 0;
//    vector<string> prod; // Накопление для свёртки
//    SP_Analyzer g;
//    g.LoadRules(file_G);
//    auto tokens = tokenize(filename_P);
//    magazine.push_back("$");
//    cout << magazine[0] << TokenToString(tokens[0]) << endl;
//    while (!tokens.empty()) {
//        string cur_token = TokenToString(tokens.front());
//        switch (g.precedence_table[magazine.back()][cur_token]) {
//            case '<':
//            case '=':
//                magazine.push_back(cur_token);
//                tokens.erase(tokens.begin());
//                break;
//            case '>':
//                len = magazine.size();
//                // Возможно выход за пределы вектора, если len = 0
//                while (g.precedence_table[magazine[len - 1]][magazine[len]] == '=') {
//                    prod.insert(prod.begin(), magazine[len]);
//                    magazine.pop_back();
//                    len--;
//                }
//                for (map<string, vector<Production>>::iterator rule = g.productions.begin(); rule != g.productions.end(); rule++)
//                {
//                    if (count((*rule).second.begin(), (*rule).second.end(), prod))
//                    {
//                        magazine.push_back((*rule).first);
//                    }
//                }
//                break;
//            default:
//                cout << "Ошибка: программа некорректна :(" << endl;
//                return;
//        }
//
//        /*cout << "Магазин: ";
//        for (const auto& elem : magazine)
//            cout << elem << " ";
//        cout << "Вход: ";
//        int i = 0;
//        for (const auto& token_ : tokens) {
//            if (i < 5) {
//                cout << TokenToString(token_) << " ";
//                i++;
//            }
//            else
//                break;
//        }
//        break;*/
//
//    }
//}

void replaceAll(string& str, const string& from, const string& to) {
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != string::npos) {
        str.replace(pos, from.length(), to);
        pos += to.length(); // Чтобы избежать бесконечного цикла при замене на подстроку
    }
}

string insertLabels(string str, const map<string, int>& replacements) {
    // Создаем вектор пар (ключ, значение) для сортировки по убыванию длины ключа
    vector<pair<string, int>> sortedReplacements(replacements.begin(), replacements.end());

    // Сортируем по длине ключа в убывающем порядке
    sort(sortedReplacements.begin(), sortedReplacements.end(),
        [](const auto& a, const auto& b) {
            return a.first.length() > b.first.length();
        });

    // Производим замену
    for (const auto& [key, value] : sortedReplacements) {
        replaceAll(str, key, to_string(value));
    }

    return str;
}

// Удаление последовательных дубликатов \n
string removeEmptys(const string& str) {
    string result = str;

    
    auto new_end = unique(result.begin(), result.end(),
        [](char a, char b) {
            return a == '\n' && b == '\n';
        });

    result.erase(new_end, result.end());
    return result;
}

// Функция для тримминга \n в начале и конце + очистка внутри
string trimAndRemoveEmptys(const string& str) {
    if (str.empty()) return "";

    // 1. Тримминг: находим первый и последний символы, не \n
    size_t start = str.find_first_not_of('\n');
    size_t end = str.find_last_not_of('\n');

    // Если строка состоит только из \n, возвращаем пустую
    if (start == string::npos || end == string::npos) {
        return "";
    }

    // Вырезаем подстроку без \n по краям
    string trimmed = str.substr(start, end - start + 1);

    // 2. Удаляем лишние \n внутри
    return removeEmptys(trimmed);
}

string Syntax_Analyzer::ConnectedLabels(const string& program) {
    int line = 1;
    vector<string> program_str;
    istringstream iss(program);
    string p_line;
    string correct = "";
    while (std::getline(iss, p_line)) {
        program_str.push_back(p_line);
    }
    for (size_t i = 0; i < program_str.size(); i++) {
        const auto& elem = program_str[i];
        size_t ind = elem.find("label");
        if (ind != string::npos) {
            table_labels_[elem.substr(ind + 6)] = line;
            //cout << elem.substr(ind + 6) << endl;
            program_str.erase(program_str.begin() + line - 1);
            i--;
        }
        else
            line++;
    }
    /*for (const auto& elem : program_str) {
        cout << elem << endl;
    }*/

    for (auto& elem : program_str) {
        if (elem.find("jmp") != string::npos) {
            elem = "jmp " + to_string(table_labels_[elem.substr(4)]);
            //cout << elem << endl;
        }
        if (elem.find("ji") != string::npos) {
            elem = "ji " + to_string(table_labels_[elem.substr(3)]);
            /*cout << table_labels_[elem.substr(4)] << " ";
            cout << elem << endl;*/
        }
    }

    for (const auto& elem : program_str) {
        correct = correct + elem + "\n";
    }
    correct.pop_back();
    return correct;
}

ofstream out("output.txt");

string Syntax_Analyzer::resolveLabel(const std::string& userLabel) {
    if (labelMap.find(userLabel) == labelMap.end()) {
        labelMap[userLabel] = "L" + to_string(labelCounter++);
    }
    return labelMap[userLabel];
}

void Syntax_Analyzer::Parse(string filename_P, ifstream& file_G) {
    if (!out) {
        cout << "Ошибка открытия файла :(" << endl;
        return;
    }
    tuple<SymbolicTokenClass, TokenValue, int> tok{};
    SP_Analyzer g;
    g.LoadRules(file_G);
    //g.PrintGrammar();
    if (g.CheckGrammar()) {
        //cout << "Эта грамматика является ПП!" << endl;
        //cout << g.precedence_table["L"][":"] << endl;
    }
    else {
        cout << "Эта грамматика не ПП :(" << endl;
        return;
    }
    vector<withAtr> list_variables;
    vector<vartype> table_variables;
    vector<string> ifInited_variables;
    vector<string> table_labels;
    vector<string> table_jumplabels;
    auto tokens = tokenize(filename_P);
    if (isErrors())
		return;
    TokenValue tokenvalue;
    tokenvalue.pInt = nullptr;
    tokenvalue.pMultiset = nullptr;
    tokenvalue.pString = nullptr;
    tokenvalue.str = "$";
    tokens.push_back(make_tuple(DOLLAR, tokenvalue, -1));
    withAtr temp;
    temp.symbol = "$";
    magazine.push_back(temp);
    if (tokens.empty()) {
        cout << "Ошибка: нет токенов для анализа" << endl;
        return;
    }

    while (!tokens.empty()) {
        string cur_token = TokenToString(tokens.front());
        if (magazine.back().symbol == g.start_symbol)
            break;
        // Проверка наличия элементов в таблице предшествования
        if (g.precedence_table.count(magazine.back().symbol) == 0 ||
            g.precedence_table[magazine.back().symbol].count(cur_token) == 0) {
            cout << "Ошибка: отсутствует отношение предшествования между "
                << magazine.back().symbol << " и " << cur_token << endl;
            return;
        }

        char relation = g.precedence_table[magazine.back().symbol][cur_token];

        switch (relation) {
        case '<':
        case '=':
            temp.symbol = cur_token;
            if (cur_token == "V") {
                temp.atrs.push_back(std::get<1>(tokens.front()).str);
                //cout << std::get<1>(tokens.front()).str << endl;
            }
            if (cur_token == "R") {
                if (std::get<1>(tokens.front()).str == "==")
                    temp.atrs.push_back("=");
                else
                    temp.atrs.push_back(std::get<1>(tokens.front()).str);
                //cout << std::get<1>(tokens.front()).str << endl;
            }
            if (cur_token == "C") {
                temp.atrs.push_back(std::get<1>(tokens.front()).str);
                //cout << std::get<1>(tokens.front()).str << endl;
            }
            if (cur_token == "L") {
                temp.atrs.push_back(std::get<1>(tokens.front()).str);
                //cout << std::get<1>(tokens.front()).str << endl;
            }
            magazine.push_back(temp);
            temp.atrs.clear();
            tok = tokens[0]; // сохраняем токен
            parse_stack.push(new ParseTreeNode(cur_token, tok)); // терминальный узел
            tokens.erase(tokens.begin());
            break;

        case '>': {
            vector<withAtr> prod;
            size_t len = magazine.size();

            // Безопасный проход по магазину
            while (len >= 2) {
                withAtr prev;
                prev.symbol = magazine[len - 2].symbol;
                prev.atrs = magazine[len - 2].atrs;
                withAtr current;
                current.symbol = magazine[len - 1].symbol;
                current.atrs = magazine[len - 1].atrs;
                /*cout << endl << "--------" << endl;
                cout << current.symbol << endl;
                for (const auto& elem : current.atrs)
                    cout << elem << " ";
                cout << endl;
                cout << "--------" << endl;
                cout << endl << endl;*/

                if (g.precedence_table.count(prev.symbol) &&
                    g.precedence_table[prev.symbol].count(current.symbol) &&
                    g.precedence_table[prev.symbol][current.symbol] == '=') {
                    prod.insert(prod.begin(), current);
                    magazine.pop_back();
                    len--;
                }
                else {
                    prod.insert(prod.begin(), current);
                    magazine.pop_back();
                    break;
                }
            }

            // Поиск подходящего правила
            int i = 0;
            bool found = false;
            for (const auto& [lhs, rules] : g.productions) {
                for (const auto& production : rules) {
                    bool areEqual = (production.symbols.size() == prod.size()) && std::equal(
                        production.symbols.begin(), production.symbols.end(),
                        prod.begin(),
                        [](const string& s, const withAtr& wa) {
                            return s == wa.symbol;
                        }
                    );
                    if (areEqual) {
                        /*vector<ParseTreeNode*> children;
                        for (int i = 0; i < prod.size(); ++i) {
                            if (!parse_stack.empty()) {
                                children.insert(children.begin(), parse_stack.top());
                                parse_stack.pop();
                            }
                        }
                        auto parent = new ParseTreeNode(rule.first);
                        parent->children = move(children);
                        parse_stack.push(parent);
                        temp.symbol = rule.first;
                        history.emplace_back(map<string, vector<withAtr>>{{rule.first, prod}});*/
                        withAtr cur_1;
                        withAtr cur_2;
                        string s = "";

                        string r;
                        string list_labels;
                        string list_consts;
                        string atr4;
                        string m1;
                        string m2;
                        string m;
                        stringstream ss_labels;
                        stringstream ss_consts;
                        vector<string> case_labels;
                        vector<string> case_consts;
                        string str;
                        string r_;
                        vartype cur_var;
                        vartype cur_var2;
                        bool flag_1;
                        bool flag_2;
                        bool isThere;

                        switch (i) {
                        // <E'> -> + <T> <E'>
                        case 0:
                            if (prod[1].atrs[1] != prod[2].atrs[1]) {
                                cout << "Ошибка: Несоответствие типов в выражении :(" << endl;
                                return;
                            }
                            cur_1.symbol = lhs;
                            s = prod[1].atrs[0] + "\n+\n" + prod[2].atrs[0];
                            cur_1.atrs.push_back(s);
                            cur_1.atrs.push_back(prod[1].atrs[1]);
                            lineCounter += 1;
                            break;
                        // <E'> -> - <T> <E'>
                        case 1:
                            if (prod[1].atrs[1] != prod[2].atrs[1]) {
                                cout << "Ошибка: Несоответствие типов в выражении :(" << endl;
                                return;
                            }
                            cur_1.symbol = lhs;
                            s = prod[1].atrs[0] + "\n-\n" + prod[2].atrs[0];
                            cur_1.atrs.push_back(s);
                            cur_1.atrs.push_back(prod[1].atrs[1]);
                            lineCounter += 1;
                            break;
                        // <E'> -> ^ <T> <E'>
                        case 2:
                            if (prod[1].atrs[1] != prod[2].atrs[1]) {
                                cout << "Ошибка: Несоответствие типов в выражении :(" << endl;
                                return;
                            }
                            cur_1.symbol = lhs;
                            s = prod[1].atrs[0] + "\n^\n" + prod[2].atrs[0];
                            cur_1.atrs.push_back(s);
                            cur_1.atrs.push_back(prod[1].atrs[1]);
                            lineCounter += 1;
                            break;
                        // <E'> -> + <T>
                        case 3:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[1].atrs[0] + "\n" + "+");
                            cur_1.atrs.push_back(prod[1].atrs[1]);
                            lineCounter += 1;
                            break;
                        // <E'> -> - <T>
                        case 4:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[1].atrs[0] + "\n" + "-");
                            cur_1.atrs.push_back(prod[1].atrs[1]);
                            lineCounter += 1;
                            break;
                        // <E'> -> ^ <T>
                        case 5:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[1].atrs[0] + "\n" + "^");
                            cur_1.atrs.push_back(prod[1].atrs[1]);
                            lineCounter += 1;
                            break;
                        // <E> -> <T> <E'>
                        case 6:
                            if (prod[0].atrs[1] != prod[1].atrs[1]) {
                                cout << "Ошибка: Несоответствие типов в выражении :(" << endl;
                                return;
                            }
                            cur_1.symbol = lhs;
                            s = prod[0].atrs[0] + "\n" + prod[1].atrs[0];
                            cur_1.atrs.push_back(s);
                            cur_1.atrs.push_back(prod[0].atrs[1]);
                            break;
                        // <E> -> <T>
                        case 7:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[0].atrs[0]);
                            cur_1.atrs.push_back(prod[0].atrs[1]);
                            break;
                        // <E_bracket> -> <E> )
                        case 8:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[0].atrs[0]);
                            cur_1.atrs.push_back(prod[0].atrs[1]);
                            break;
                        // <F> -> C
                        case 9:
                            cur_1.symbol = lhs;
                            //string s = "push " + *(std::get<1>(tokens.front()).pInt);
                            s = "push " + std::get<1>(tok).str;
                            cur_1.atrs.push_back(s);
                            cur_1.atrs.push_back("int");
                            lineCounter += 1;
                            break;
                        // <F> -> V
                        case 10:
                            cur_1.symbol = lhs;
                            //string s = "push " + *(std::get<1>(tokens.front()).pInt);
                            s = "push " + std::get<1>(tok).str;
                            // АШИБКА !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                            cur_1.atrs.push_back(s);
                            flag_1 = false;
                            flag_2 = false;
                            for (const auto& [var, type] : table_variables) {
                                if (var == prod[0].atrs[0]) {
                                    cur_var.var = var;
                                    cur_var.type = type;
                                    flag_2 = true;
                                    /*if (type == "int") {
                                        flag_1 = true;
                                        break;
                                    }*/
                                }
                            }
                            if (prod[0].atrs[0][0] != '$')
                                if (prod[0].atrs[0][0] < 48 || prod[0].atrs[0][0] > 57) {
                                    if (!flag_2) {
                                        cout << "Ошибка: Переменная " << prod[0].atrs[0] << " не объявлена" << endl;
                                        return;
                                    }
                                    if (!count(ifInited_variables.begin(), ifInited_variables.end(), prod[0].atrs[0])) {
                                        cout << "Ошибка: Переменная " << prod[0].atrs[0] << " не инициализирована" << endl;
                                        return;
                                    }
                                }
                            cur_1.atrs.push_back(cur_var.type);
                            lineCounter += 1;
                            break;
                        // <F> -> ( <E> )
                        case 11:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[1].atrs[0]);
                            cur_1.atrs.push_back(prod[1].atrs[1]);
                            break;
                        // <F> -> pow ( <E> )
                        case 12:
                            if (prod[2].atrs[1] != "multiset") {
                                cout << "Ошибка: Неверный тип аргумента при вызове функции" << endl;
                                return;
                            }
                            cur_1.symbol = lhs;
                            s = prod[2].atrs[0] + "\npow";
                            cur_1.atrs.push_back(s);
                            cur_1.atrs.push_back("int");
                            lineCounter += 1;
                            break;
                        // <F> -> afm ( <E> )
                        case 13:
                            if (prod[2].atrs[1] != "multiset") {
                                cout << "Ошибка: Неверный тип аргумента при вызове функции" << endl;
                                return;
                            }
                            cur_1.symbol = lhs;
                            s = prod[2].atrs[0] + "\nafm";
                            cur_1.atrs.push_back(s);
                            cur_1.atrs.push_back("multiset");
                            lineCounter += 1;
                            break;
                        // <F> -> m
                        case 14:
                            cur_1.symbol = lhs;
                            //string s = "push " + *(std::get<1>(tokens.front()).pInt);
                            s = "push " + std::get<1>(tok).str;
                            cur_1.atrs.push_back(s);
                            cur_1.atrs.push_back("multiset");
                            lineCounter += 1;
                            break;
                        // <F> -> acs ( <E> <comma> <E_bracket>
                        case 15:
                            if ((prod[2].atrs[1] != "multiset") || (prod[4].atrs[1] != "int")) {
                                cout << "Ошибка: Неверные типы аргументов при вызове функции" << endl;
                                return;
                            }
                            cur_1.symbol = lhs;
                            s = prod[2].atrs[0] + "\n" + prod[4].atrs[0] + "\nacs";
                            cur_1.atrs.push_back(s);
                            cur_1.atrs.push_back("int");
                            lineCounter += 1;
                            break;
                        // <T'> -> * <F> <T'>
                        case 16:
                            if (prod[1].atrs[1] != prod[2].atrs[1]) {
                                cout << "Ошибка: Несоответствие типов в выражении :(" << endl;
                                return;
                            }
                            cur_1.symbol = lhs;
                            s = prod[1].atrs[0] + "\n*\n" + prod[2].atrs[0];
                            cur_1.atrs.push_back(s);
                            cur_1.atrs.push_back(prod[1].atrs[1]);
                            lineCounter += 1;
                            break;
                        // <T'> -> / <F> <T'>
                        case 17:
                            if (prod[1].atrs[1] != prod[2].atrs[1]) {
                                cout << "Ошибка: Несоответствие типов в выражении :(" << endl;
                                return;
                            }
                            cur_1.symbol = lhs;
                            s = prod[1].atrs[0] + "\n/\n" + prod[2].atrs[0];
                            cur_1.atrs.push_back(s);
                            cur_1.atrs.push_back(prod[1].atrs[1]);
                            lineCounter += 1;
                            break;
                        // <T'> -> % <F> <T'>
                        case 18:
                            if (prod[1].atrs[1] != prod[2].atrs[1]) {
                                cout << "Ошибка: Несоответствие типов в выражении :(" << endl;
                                return;
                            }
                            cur_1.symbol = lhs;
                            s = prod[1].atrs[0] + "\n%\n" + prod[2].atrs[0];
                            cur_1.atrs.push_back(s);
                            cur_1.atrs.push_back(prod[1].atrs[1]);
                            lineCounter += 1;
                            break;
                        // <T'> -> & <F> <T'>
                        case 19:
                            if (prod[1].atrs[1] != prod[2].atrs[1]) {
                                cout << "Ошибка: Несоответствие типов в выражении :(" << endl;
                                return;
                            }
                            cur_1.symbol = lhs;
                            s = prod[1].atrs[0] + "\n&\n" + prod[2].atrs[0];
                            cur_1.atrs.push_back(s);
                            cur_1.atrs.push_back(prod[1].atrs[1]);
                            lineCounter += 1;
                            break;
                        // <T'> -> * <F>
                        case 20:
                            cur_1.symbol = lhs;
                            s = prod[1].atrs[0] + "\n" + "*";
                            cur_1.atrs.push_back(s);
                            cur_1.atrs.push_back(prod[1].atrs[1]);
                            lineCounter += 1;
                            break;
                        // <T'> -> / <F>
                        case 21:
                            cur_1.symbol = lhs;
                            s = prod[1].atrs[0] + "\n" + "/";
                            cur_1.atrs.push_back(s);
                            cur_1.atrs.push_back(prod[1].atrs[1]);
                            lineCounter += 1;
                            break;
                        // <T'> -> % <F>
                        case 22:
                            cur_1.symbol = lhs;
                            s = prod[1].atrs[0] + "\n" + "%";
                            cur_1.atrs.push_back(s);
                            cur_1.atrs.push_back(prod[1].atrs[1]);
                            lineCounter += 1;
                            break;
                        // <T'> -> & <F>
                        case 23:
                            cur_1.symbol = lhs;
                            s = prod[1].atrs[0] + "\n" + "&";
                            cur_1.atrs.push_back(s);
                            cur_1.atrs.push_back(prod[1].atrs[1]);
                            lineCounter += 1;
                            break;
                        // <T> -> <F> <T'>
                        case 24:
                            if (prod[0].atrs[1] != prod[1].atrs[1]) {
                                cout << "Ошибка: Несоответствие типов в выражении :(" << endl;
                                return;
                            }
                            cur_1.symbol = lhs;
                            s = prod[0].atrs[0] + "\n" + prod[1].atrs[0];
                            cur_1.atrs.push_back(s);
                            cur_1.atrs.push_back(prod[0].atrs[1]);
                            break;
                        // <T> -> <F>
                        case 25:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[0].atrs[0]);
                            cur_1.atrs.push_back(prod[0].atrs[1]);
                            break;
                        // <announ> -> <vl> as <type> ;
                        case 26:
                            for (auto& elem : list_variables) {
                                elem.atrs.push_back(prod[2].atrs[0]);
                            }
                            cur_var.type = prod[2].atrs[0];
                            for (auto& elem : list_variables) {
                                flag_2 = false;
                                for (const auto& [var, type] : table_variables) {
                                    if (var == elem.atrs[0]) {
                                        flag_2 = true;
                                        break;
                                    }
                                }
                                if (flag_2) {
                                    cout << "Ошибка: Переменная " << elem.atrs[0] << " уже объявлена" << endl;
                                    return;
                                }
                                cur_var.var = elem.atrs[0];
                                table_variables.push_back(cur_var);
                            }
                            list_variables.clear();
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back("");
                            break;
                        // <bran'> -> <case> <bran'>
                        case 27:
                            cur_1.symbol = lhs;
                            s = prod[0].atrs[0] + "\njmp " + prod[1].atrs[3] + "\n" + prod[1].atrs[0];
                            cur_1.atrs.push_back(s);
                            list_labels = prod[0].atrs[1] + " " + prod[1].atrs[1];
                            cur_1.atrs.push_back(list_labels);
                            list_consts = prod[0].atrs[2] + " " + prod[1].atrs[2];
                            cur_1.atrs.push_back(list_consts);
                            cur_1.atrs.push_back(prod[1].atrs[3]);
                            lineCounter += 1;
                            break;
                        // <bran'> -> <otherwise>
                        case 28:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[0].atrs[0]);
                            cur_1.atrs.push_back(prod[0].atrs[1]);
                            cur_1.atrs.push_back(prod[0].atrs[2]);
                            cur_1.atrs.push_back(prod[0].atrs[3]);
                            break;
                        // <bran'> -> <case>
                        case 29:
                            cur_1.symbol = lhs;
                            r = newLabel();
                            cur_1.atrs.push_back(prod[0].atrs[0] + "\nlabel " + r + "\n");
                            cur_1.atrs.push_back(prod[0].atrs[1]);
                            cur_1.atrs.push_back(prod[0].atrs[2]);
                            cur_1.atrs.push_back(r);
                            lineCounter += 1;
                            break;
                        // <bran> -> <bran'>
                        case 30:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[0].atrs[0]);
                            cur_1.atrs.push_back(prod[0].atrs[1]);
                            cur_1.atrs.push_back(prod[0].atrs[2]);
                            cur_1.atrs.push_back(prod[0].atrs[3]);
                            break;
                        // <case> -> case C <colon> <prog>
                        case 31:
                            cur_1.symbol = lhs;
                            r = newLabel();
                            s = "label " + r + "\n" + prod[3].atrs[0];
                            cur_1.atrs.push_back(s);
                            list_labels = r;
                            cur_1.atrs.push_back(list_labels);
                            list_consts = prod[1].atrs[0];
                            cur_1.atrs.push_back(list_consts);
                            lineCounter += 1;
                            break;
                        // <colon> -> :
                        case 32:
                            cur_1.symbol = lhs;
                            break;
                        // <comma> -> ,
                        case 33:
                            cur_1.symbol = lhs;
                            break;
                        // <else> -> else <prog>
                        case 34:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[1].atrs[0]);
                            break;
                        // <for_1> -> <for_3> do <prog>
                        case 35:
                            cur_1.symbol = lhs;
                            m1 = newLabel();
                            m2 = newLabel();
                            s = prod[0].atrs[1] + "\n" +
                                "pop " + prod[0].atrs[0] + "\n" +
                                "label " + m1 + "\n" +
                                "push " + prod[0].atrs[0] + "\n" +
                                prod[0].atrs[2] + "\n" +
                                ">\n" +
                                "ji " + m2 + "\n" +
                                prod[2].atrs[0] + "\n" +
                                "push " + prod[0].atrs[0] + "\n" +
                                prod[0].atrs[3] + "\n" +
                                "+\n" +
                                "pop " + prod[0].atrs[0] + "\n" +
                                "jmp " + m1 + "\n" +
                                "label " + m2;
                            cur_1.atrs.push_back(s);
                            lineCounter += 10;
                            break;
                        // <for_2> -> <for_4> do <prog>
                        case 36:
                            cur_1.symbol = lhs;
                            m1 = newLabel();
                            m2 = newLabel();
                            s = prod[0].atrs[1] + "\n" +
                                "pop " + prod[0].atrs[0] + "\n" +
                                "label " + m1 + "\n" +
                                "push " + prod[0].atrs[0] + "\n" +
                                prod[0].atrs[2] + "\n" +
                                ">\n" +
                                "ji " + m2 + "\n" +
                                prod[2].atrs[0] + "\n" +
                                "push " + prod[0].atrs[0] + "\n" +
                                "push 1" + "\n" +
                                "+\n" +
                                "pop " + prod[0].atrs[0] + "\n" +
                                "jmp " + m1 + "\n" +
                                "label " + m2;
                            cur_1.atrs.push_back(s);
                            lineCounter += 11;
                            break;
                        // <for_3> -> for V from <E> to <E> <step>
                        case 37:
                            cur_1.symbol = lhs;
                            flag_1 = false;
                            flag_2 = false;
                            for (const auto& [var, type] : table_variables) {
                                if (var == prod[1].atrs[0]) {
                                    cur_var.var = var;
                                    cur_var.type = type;
                                    flag_2 = true;
                                    break;
                                    /*if (type == "int") {
                                        flag_1 = true;
                                        break;
                                    }*/
                                }
                            }
                            if (!flag_2) {
                                cout << "Ошибка: Переменная " << prod[1].atrs[0] << " не объявлена" << endl;
                                return;
                            }
                            if ((cur_var.type != "int") || (prod[3].atrs[1] != "int") || (prod[5].atrs[1] != "int")) {
                                cout << "Ошибка: Невозможна итерация по multiset" << endl;
                                return;
                            }
                            cur_1.atrs.push_back(prod[1].atrs[0]);
                            cur_1.atrs.push_back(prod[3].atrs[0]);
                            cur_1.atrs.push_back(prod[5].atrs[0]);
                            cur_1.atrs.push_back(prod[6].atrs[0]);
                            break;
                        // <for_4> -> for V from <E> to <E>
                        case 38:
                            cur_1.symbol = lhs;
                            flag_1 = false;
                            flag_2 = false;
                            for (const auto& [var, type] : table_variables) {
                                if (var == prod[1].atrs[0]) {
                                    cur_var.var = var;
                                    cur_var.type = type;
                                    flag_2 = true;
                                    break;
                                    /*if (type == "int") {
                                        flag_1 = true;
                                        break;
                                    }*/
                                }
                            }
                            if (!flag_2) {
                                cout << "Ошибка: Переменная " << prod[1].atrs[0] << " не объявлена" << endl;
                                return;
                            }
                            if ((cur_var.type != "int") || (prod[3].atrs[1] != "int") || (prod[5].atrs[1] != "int")) {
                                cout << "Ошибка: Невозможна итерация по multiset" << endl;
                                return;
                            }
                            cur_1.atrs.push_back(prod[1].atrs[0]);
                            cur_1.atrs.push_back(prod[3].atrs[0]);
                            cur_1.atrs.push_back(prod[5].atrs[0]);
                            break;
                        // <if_1> -> if <test_if> <prog>
                        case 39:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[1].atrs[0]);
                            cur_1.atrs.push_back(prod[2].atrs[0]);
                            break;
                        // <label> -> L
                        case 40:
                            cur_1.symbol = lhs;
                            r = prod[0].atrs[0];
                            table_jumplabels.push_back(prod[0].atrs[0]);
                            s = "jmp " + prod[0].atrs[0];
                            cur_1.atrs.push_back(s);
                            lineCounter += 1;
                            break;
                        // <let> -> let V = <E>
                        case 41:
                            cur_1.symbol = lhs;
                            flag_1 = false;
                            flag_2 = false;
                            for (const auto& [var, type] : table_variables) {
                                if (var == prod[1].atrs[0]) {
                                    cur_var.var = var;
                                    cur_var.type = type;
                                    flag_2 = true;
                                    break;
                                    /*if (type == "int") {
                                        flag_1 = true;
                                        break;
                                    }*/
                                }
                            }
                            if (!flag_2) {
                                cout << "Ошибка: Переменная " << prod[1].atrs[0] << " не объявлена" << endl;
                                return;
                            }
                            if (!flag_1)
                                if (cur_var.type == "int" && prod[3].atrs[1] == "int") {
                                    s = prod[3].atrs[0] + "\n" +
                                        "pop " + prod[1].atrs[0];
                                    cur_1.atrs.push_back(s);
                                } else {
                                    flag_1 = false;
                                    if ((cur_var.type == "multiset") && (prod[3].atrs[1] == "multiset")) {
                                        s = prod[3].atrs[0] + "\n" +
                                            "pop " + prod[1].atrs[0];
                                        cur_1.atrs.push_back(s);
                                    } else {
                                        cout << "Ошибка: Несоответствие типов в присваивании:" << endl;
                                        cout << "Ошибка: Значение типа " << prod[3].atrs[1] << " нельзя присвоить переменной " << cur_var.var << " типа " << cur_var.type << endl;
                                        return;
                                    }
                                }
                            else {
                                if (cur_var.type == "int" && cur_var2.type == "int") {
                                    s = prod[3].atrs[0] + "\n" +
                                        "pop " + prod[1].atrs[0];
                                    cur_1.atrs.push_back(s);
                                }
                                else {
                                    flag_1 = false;
                                    for (const auto& [var, type] : table_variables) {
                                        if ((var == prod[1].atrs[0]) && type == "multiset") {
                                            cur_var.var = var;
                                            cur_var.type = type;
                                            flag_1 = true;
                                        }
                                    }
                                    if ((cur_var.type == "multiset") && (cur_var2.type == "multiset")) {
                                        if (count(ifInited_variables.begin(), ifInited_variables.end(), cur_var2.var)) {
                                            s = prod[3].atrs[0] + "\n" +
                                                "pop " + prod[1].atrs[0];
                                            cur_1.atrs.push_back(s);
                                        }
                                        else {
                                            cout << "Ошибка: Переменная " << cur_var2.var << " не инициализирована" << endl;
                                            return;
                                        }
                                    }
                                    else {
                                        cout << "Ошибка: Несоответствие типов в присваивании:" << endl;
                                        cout << "Ошибка: Значение типа " << cur_var2.type << " нельзя присвоить переменной " << cur_var.var << " типа " << cur_var.type << endl;
                                        return;
                                    }

                                    // Вариант "сначала проверка инициализации, затем на совместимость типов
                                    /*if (count(ifInited_variables.begin(), ifInited_variables.end(), cur_var2.var)) {
                                        if ((cur_var.type == "multiset") && (cur_var2.type == "multiset")) {
                                            s = prod[3].atrs[0] + "\n" +
                                                "pop " + prod[1].atrs[0];
                                            cur_1.atrs.push_back(s);
                                        }
                                        else {
                                            cout << "Несоответствие типов в присваивании:" << endl;
                                            cout << "Значение типа " << cur_var2.type << " нельзя присвоить переменной " << cur_var.var << " типа " << cur_var.type << endl;
                                            return;
                                        }
                                    }
                                    else {
                                        cout << "Переменная " << cur_var2.var << " не инициализирована" << endl;
                                        return;
                                    }*/

                                }
                            }
                            if (!count(ifInited_variables.begin(), ifInited_variables.end(), cur_var.var))
                                ifInited_variables.push_back(cur_var.var);
                            lineCounter += 1;
                            break;
                        // <op> -> /*комментарий*/
                        case 42:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back("");
                            break;
                        // <op> -> ;
                        case 43:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back("");
                            break;
                            // <op> -> L :
                        case 44:
                            cur_1.symbol = lhs;
                            r = prod[0].atrs[0];
                            flag_2 = false;
                            for (const auto& [var, type] : table_variables) {
                                if (var == r) {
                                    flag_2 = true;
                                    break;
                                }
                            }
                            if (flag_2) {
                                cout << "Ошибка: Имя метки " << r << " не должно совпадать с именем переменной" << endl;
                                return;
                            }
                            table_labels.push_back(prod[0].atrs[0]);
                            s = "label " + r;
                            cur_1.atrs.push_back(s);
                            lineCounter += 1;
                            break;
                        case 45:
                            cur_1.symbol = lhs;
                            s = "end";
                            cur_1.atrs.push_back(s);
                            lineCounter += 1;
                            break;
                        // <op> -> <announ>
                        case 46:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[0].atrs[0]);
                            break;
                        // <op> -> <let> ;
                        case 47:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[0].atrs[0]);
                            break;
                        // <op> -> <while> od ;
                        case 48:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[0].atrs[0]);
                            break;
                        // <op> -> <for_1> od ;
                        case 49:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[0].atrs[0]);
                            break;
                        // <op> -> <for_2> od ;
                        case 50:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[0].atrs[0]);
                            break;
                        // <op> -> <if_1> fi ;
                        case 51:
                            cur_1.symbol = lhs;
                            m = newLabel();
                            s = prod[0].atrs[0] + "\n" +
                                "ji " + m + "\n" +
                                prod[0].atrs[1] + "\n" +
                                "label " + m;
                            cur_1.atrs.push_back(s);
                            lineCounter += 2;
                            break;
                        // <op> -> <if_1> <else> fi ;
                        case 52:
                            cur_1.symbol = lhs;
                            m1 = newLabel();
                            m2 = newLabel();
                            s = prod[0].atrs[0] + "\n" +
                                "ji " + m1 + "\n" +
                                prod[0].atrs[1] + "\n" +
                                "jmp " + m2 + "\n" +
                                "label " + m1 + "\n" +
                                prod[1].atrs[0] + "\n" +
                                "label " + m2;
                            cur_1.atrs.push_back(s);
                            lineCounter += 4;
                            break;
                        // <op> -> <read> ;
                        case 53:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[0].atrs[0]);
                            break;
                        // <op> -> <write> ;
                        case 54:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[0].atrs[0]);
                            break;
                        // <op> -> jump <label> ;
                        case 55:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[1].atrs[0]);
                            break;
                        // <op> -> <select> <bran> ni ;
                        case 56:
                            cur_1.symbol = lhs;
                            r = prod[0].atrs[0].substr(5);
                            ss_labels.str(prod[1].atrs[1]);
                            ss_consts.str(prod[1].atrs[2]);
                            while (ss_labels >> str) {
                                case_labels.push_back(str);
                            }
                            while (ss_consts >> str) {
                                case_consts.push_back(str);
                            }
                            if (case_labels.size() > case_consts.size())
                                r_ = case_labels[case_labels.size() - 1];
                            else
                                r_ = prod[1].atrs[3];
                            s = "";
                            for (int i = 0; i < case_consts.size(); i++) {
                                s = s + "push " + case_consts[i] + "\n" +
                                    "push " + r + "\n" +
                                    "=" + "\n" +
                                    "ji " + case_labels[i] + "\n";
                            }
                            s = s + "jmp " + r_ + "\n" + prod[1].atrs[0];
                            cur_1.atrs.push_back(s);
                            lineCounter = lineCounter + 4 * case_consts.size();
                            break;
                        //// <op> -> afm ( <E> )
                        //case 57:
                        //    if (prod[2].atrs[1] != "multiset") {
                        //        cout << "Ошибка: Неверный тип аргумента при вызове функции" << endl;
                        //        return;
                        //    }
                        //    cur_1.symbol = lhs;
                        //    s = prod[2].atrs[0] + "\n" +
                        //        "afm";
                        //    cur_1.atrs.push_back(s);
                        //    break;
                        // <op_in> -> <read> ,
                        case 57:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[0].atrs[0]);
                            break;
                        // <op_out> -> <E> <comma> <op_out>
                        case 58:
                            cur_1.symbol = lhs;
                            s = prod[0].atrs[0] + "\n" +
                                "write\n" +
                                prod[2].atrs[0];
                            cur_1.atrs.push_back(s);
                            lineCounter += 1;
                            break;
                        // <op_out> -> <E>
                        case 59:
                            cur_1.symbol = lhs;
                            s = prod[0].atrs[0] + "\n" +
                                "write";
                            cur_1.atrs.push_back(s);
                            lineCounter += 1;
                            break;
                        // <otherwise> -> otherwise <prog>
                        case 60:
                            cur_1.symbol = lhs;
                            r = newLabel();
                            atr4 = newLabel();
                            s = "label " + r + "\n" +
                                prod[1].atrs[0] + "\n" + 
                                "label " + atr4;
                            cur_1.atrs.push_back(s);
                            cur_1.atrs.push_back(r);
                            cur_1.atrs.push_back("");
                            cur_1.atrs.push_back(atr4);
                            lineCounter += 2;
                            break;
                        // <prog> -> <op> <prog>
                        case 61:
                            cur_1.symbol = lhs;
                            s = prod[0].atrs[0] + "\n" +
                                prod[1].atrs[0];
                            cur_1.atrs.push_back(s);
                            break;
                        // <prog> -> <op>
                        case 62:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[0].atrs[0]);
                            break;
                        // <read> -> read V
                        case 63:
                            cur_1.symbol = lhs;
                            flag_2 = false;
                            for (const auto& [var, type] : table_variables) {
                                if (var == prod[1].atrs[0]) {
                                    flag_2 = true;
                                    break;
                                }
                            }
                            if (!flag_2) {
                                cout << "Ошибка: Переменная " << prod[1].atrs[0] << " не объявлена" << endl;
                                return;
                            }
                            s = "read\npop " + prod[1].atrs[0];
                            ifInited_variables.push_back(prod[1].atrs[0]);
                            cur_1.atrs.push_back(s);
                            lineCounter += 2;
                            break;
                        // <read> -> <op_in> V
                        case 64:
                            cur_1.symbol = lhs;
                            s = prod[0].atrs[0] + "\nread\npop " + prod[1].atrs[0];
                            ifInited_variables.push_back(prod[1].atrs[0]);
                            cur_1.atrs.push_back(s);
                            lineCounter += 2;
                            break;
                        // <select> -> select <E> in
                        case 65:
                            if (prod[1].atrs[1] != "int") {
                                cout << "Ошибка: Требуется целое число" << endl;
                                return;
                            }
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[1].atrs[0]);
                            break;
                        // <step> -> by <E>
                        case 66:
                            if (prod[1].atrs[1] != "int") {
                                cout << "Ошибка: Невозможна итерация по multiset" << endl;
                                return;
                            }
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[1].atrs[0]);
                            break;
                        // <test_if> -> ( <E> R <E> )
                        case 67:
                            if (prod[1].atrs[1] != prod[3].atrs[1]) {
                                cout << "Ошибка: Несоответствие типов при сравнении в условном операторе" << endl;
                                return;
                            }
                            cur_1.symbol = lhs;
                            s = prod[1].atrs[0] + "\n" +
                                prod[3].atrs[0] + "\n" +
                                OTR(prod[2].atrs[0]);
                            cur_1.atrs.push_back(s);
                            lineCounter += 1;
                            break;
                        // <test_w> -> <E> R <E>
                        case 68:
                            if (prod[0].atrs[1] != prod[2].atrs[1]) {
                                cout << "Ошибка: Несоответствие типов при сравнении в условном операторе" << endl;
                                return;
                            }
                            cur_1.symbol = lhs;
                            s = prod[0].atrs[0] + "\n" +
                                prod[2].atrs[0] + "\n" +
                                OTR(prod[1].atrs[0]);
                            cur_1.atrs.push_back(s);
                            lineCounter += 1;
                            break;
                        // <type> -> int
                        case 69:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[0].symbol);
                            break;
                        // <type> -> multiset
                        case 70:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[0].symbol);
                            break;
                        // <vl'> -> <vl> ,
                        case 71:
                            cur_1.symbol = lhs;
                            break;
                        // <vl> -> dim V
                        case 72:
                            cur_1.symbol = lhs;
                            list_variables.push_back(prod[1]);
                            break;
                        // <vl> -> <vl'> V
                        case 73:
                            cur_1.symbol = lhs;
                            list_variables.push_back(prod[1]);
                            break;
                        // <while> -> while <test_w> do <prog>
                        case 74:
                            cur_1.symbol = lhs;
                            m1 = newLabel();
                            m2 = newLabel();
                            s = "label " + m1 + "\n" +
                                prod[1].atrs[0] + "\n" +
                                "ji " + m2 + "\n" +
                                prod[3].atrs[0] + "\n" +
                                "jmp " + m1 + "\n" +
                                "label " + m2;
                            cur_1.atrs.push_back(s);
                            lineCounter += 4;
                            break;
                        // <write> -> write <op_out>
                        case 75:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[1].atrs[0]);
                            break;
                        // S -> <prog>
                        case 76:
                            cur_1.symbol = lhs;
                            cur_1.atrs.push_back(prod[0].atrs[0]);
                            break;
                        }

                        magazine.push_back(cur_1);
                        found = true;
                        break;
                    }
                    i++;
                    temp.atrs.clear();
                }
                if (found) break;
            }

            if (!found) {
                cout << "Ошибка: не найдено правило для свёртки: ";
                for (const auto& s : prod) cout << s.symbol << " ";
                cout << endl;
                return;
            }
            break;
        }

        default:
            cout << "Ошибка: программа некорректна :(" << endl;
            return;
        }

        /*cout << "Магазин: ";
        for (const auto& elem : magazine)
            cout << elem.symbol << " ";
        cout << "\nВход: ";
        int i = 0;
        for (const auto& token_ : tokens) {
            if (i < 5) {
                cout << TokenToString(token_) << " ";
                i++;
            }
            else
                break;
        }
        cout << endl;*/
    }
    bool flag_l = false;
    for (const auto& elem : table_jumplabels) {
        if (!count(table_labels.begin(), table_labels.end(), elem)) {
            cout << "Ошибка: Переход на несуществующую метку " << elem << endl;
            flag_l = true;
        }
    }

    if (flag_l)
        return;

    

    // Финальная проверка успешного разбора
    if (magazine.size() == 2 && magazine.back().symbol == g.start_symbol) {
        magazine.back().atrs[0] = trimAndRemoveEmptys(magazine.back().atrs[0]);
        magazine.back().atrs[0] = ConnectedLabels(magazine.back().atrs[0]);
        cout << "Разбор завершён успешно!" << endl;
        ShowProgram();
        //cout << endl << lineCounter << endl;
        /*if (!parse_stack.empty()) {
            ParseTreeNode* root = parse_stack.top();
            PrintTree(root, 0);
            GenerateCode(root);
            ReportUnusedLabels();
        }*/
    }
    else {
        cout << "Ошибка: не удалось завершить разбор" << endl;
    }
    cout << endl;
    out.close();
}

void Syntax_Analyzer::ShowHistory() {
    cout << "История правил сворачивания: " << endl;
    for (const auto& elem : history) {
        for (const auto& [lhs, rhs] : elem) {
            cout << lhs << " -> ";
            for (const auto& elem_ : rhs) {
                cout << elem_.symbol << " ";
            }
        }
        cout << endl;
    }
}


//void Syntax_Analyzer::PrintTree(ParseTreeNode* node, int depth = 0) {
//    if (!node) return;
//    cout << string(depth * 2, ' ') << node->type;
//    if (!(std::get<1>(node->token).str).empty())
//        std::cout << " [" << std::get<1>(node->token).str << "]";
//
//    cout << endl;
//    for (auto* child : node->children) {
//        PrintTree(child, depth + 1);
//    }
//}
//
//void Syntax_Analyzer::CollectIdentifiers(ParseTreeNode* node, vector<string>& vars) {
//    if (!node) return;
//
//    if (node->type == "V") {
//        vars.push_back(std::get<1>(node->token).str);
//    }
//
//    for (auto* child : node->children) {
//        CollectIdentifiers(child, vars);
//    }
//}
//
//void Syntax_Analyzer::GenerateCodeAsData(ParseTreeNode* node) {
//    if (!node) return;
//
//    if (node->type == "<E>" || node->type == "<T>") {
//        GenerateCodeAsData(node->children[0]);
//        if (node->children.size() > 1)
//            GenerateCodeAsData(node->children[1]);
//    }
//    else if (node->type == "<E'>" || node->type == "<T'>") {
//        // + <T> <E'> или * <F> <T'>
//        string op = node->children[0]->type;
//        GenerateCodeAsData(node->children[1]);
//
//        out << "push " << op << endl;
//
//        if (node->children.size() == 3)
//            GenerateCodeAsData(node->children[2]);
//    }
//    else if (node->type == "<F>") {
//        ParseTreeNode* leaf = node->children[0];
//        if (leaf->type == "V") {
//            out << "push " << std::get<1>(leaf->token).str << endl;
//        }
//        else if (leaf->type == "m") {
//            out << "push " << std::get<1>(leaf->token).str << endl;
//        }
//        else if (leaf->type == "C" || leaf->type == "c") {
//            out << "push " << std::get<1>(leaf->token).str << endl;
//        }
//    }
//}
//
//void Syntax_Analyzer::GenerateCode(ParseTreeNode* node) {
//
//    if (!node) return;
//
//    if (node->type == "<prog>") {
//        for (auto child : node->children)
//            GenerateCode(child);
//    }
//    else if (node->type == "L") {
//        string label = newLabel();
//        out << label << ":" << endl;
//    }
//    else if (node->type == "<op>" && node->children.size() >= 2 &&
//        node->children[0]->type == "jump" &&
//        node->children[1]->type == "<label>") {
//        
//        string userLabel = std::get<1>(node->children[1]->children[0]->token).str;
//        string internalLabel = resolveLabel(userLabel);
//        out << "jmp " << internalLabel << endl;
//    }
//    else if (node->type == "<op>" && node->children.size() >= 2 &&
//        node->children[0]->type == "L" &&
//        node->children[1]->type == ":") {
//
//        string userLabel = std::get<1>(node->children[0]->token).str;
//        string internalLabel = resolveLabel(userLabel);
//        out << internalLabel << ":" << endl;
//    }
//    else if (node->type == "<while>") {
//        // while <test> do <prog> od;
//
//        string startLabel = newLabel();
//        string endLabel = newLabel();
//
//        out << startLabel << ":" << endl;
//        // <test_w> всегда 3 ребёнка: E, R, E
//        ParseTreeNode* test = node->children[1]; // <test_w>
//        GenerateCode(test->children[0]); 
//        GenerateCode(test->children[2]);
//
//        // Сравнение
//        std::string rel_op = std::get<1>(test->children[1]->token).str;
//
//        if (rel_op == "<") out << ">=" << endl;
//        else if (rel_op == "<=") out << ">" << endl;
//        else if (rel_op == ">") out << "<=" << endl;
//        else if (rel_op == ">=") out << "<" << endl;
//        else if (rel_op == "==") out << "!=" << endl;
//        else if (rel_op == "!=") out << "==" << endl;
//        else out << rel_op << endl;
//
//        out << "ji " << endLabel << endl;
//
//        // Тело цикла
//        GenerateCode(node->children[3]); // <prog>
//
//        out << "jmp " << startLabel << endl;
//        out << endLabel << ":" << endl;
//    }
//    else if (node->type == "<for_1>") {
//        auto for3 = node->children[0];
//        auto body = node->children[1];
//
//        string varName = std::get<1>(for3->children[1]->token).str;
//
//        ParseTreeNode* fromExpr = for3->children[3];
//        ParseTreeNode* toExpr = for3->children[5];
//        ParseTreeNode* stepExpr = nullptr;
//        if (for3->children.size() > 6)
//            stepExpr = for3->children[6]->children[1];
//
//        // Инициализация счётчика
//        out << "push " << varName << endl;
//        GenerateCode(fromExpr);
//        out << "pop " << varName << endl;
//
//        string startLabel = newLabel();
//        string endLabel = newLabel();
//
//        out << startLabel << ":" << endl;
//
//        out << "push " << varName << endl;
//        GenerateCode(toExpr);
//        out << ">" << endl;
//        out << "ji " << endLabel << endl;
//
//        // тело цикла
//        GenerateCode(body);
//
//        out << "push " << varName << endl;
//        if (stepExpr) GenerateCode(stepExpr);
//        else out << "push 1" << endl;
//        out << "+" << endl;
//        out << "pop " << varName << endl;
//
//        out << "jmp " << startLabel << endl;
//        out << endLabel << ":" << endl;
//    }
//    else if (node->type == "<if_1>") {
//        // if <test> <prog>
//
//        string elseLabel = newLabel();
//        string endLabel = newLabel();
//
//        GenerateCode(node->children[0]); // <test>
//        out << "ji " << elseLabel << endl;
//
//        GenerateCode(node->children[1]); // <prog> (then branch)
//        out << "jmp " << endLabel << endl;
//
//        out << elseLabel << ":" << endl;
//        if (node->children.size() > 2) // если есть else
//            GenerateCode(node->children[2]); // <else> (else branch)
//
//        out << endLabel << ":" << endl;
//    }
//    else if (node->type == "<test_if>") {
//        // ( <E> R <E> )
//        GenerateCode(node->children[1]); // левая часть
//        GenerateCode(node->children[3]); // правая часть
//
//        string rel_op = std::get<1>(node->children[2]->token).str;
//        if (rel_op == "<") out << ">=" << endl;
//        else if (rel_op == "<=") out << ">" << endl;
//        else if (rel_op == ">") out << "<=" << endl;
//        else if (rel_op == ">=") out << "<" << endl;
//        else if (rel_op == "==") out << "!=" << endl;
//        else if (rel_op == "!=") out << "==" << endl;
//        else out << rel_op << endl;
//    }
//    else if (node->type == "<let>") {
//        // let V = <E>
//        string varName = std::get<1>(node->children[1]->token).str;
//        GenerateCodeAsData(node->children[3]);
//        
//        out << "pop " << varName << endl;
//    }
//    else if (node->type == "<write>") {
//        // write <op_out>
//        GenerateCode(node->children[0]);
//        out << "write " << endl;
//    }
//    else if (node->type == "<read>") {
//        // read V [, ...]
//        vector<string> vars;
//        CollectIdentifiers(node, vars);
//        for (const auto& varName : vars) {
//            out << "read " << varName << endl;
//        }
//    }
//    else if (node->type == "<E>") {
//        // <E> -> <T> | <T> <E'>
//        GenerateCode(node->children[0]); // <T>
//        if (node->children.size() > 1)
//            GenerateCode(node->children[1]); // <E'>
//    }
//    else if (node->type == "<E'>") {
//        // <E'> -> + <T> <E'> | - <T> <E'> | ...
//        const string& op = node->children[0]->type;
//        GenerateCode(node->children[1]); // <T>
//        out << op << endl;
//        if (node->children.size() == 3)
//            GenerateCode(node->children[2]); // <E'>
//    }
//    else if (node->type == "<T>") {
//        GenerateCode(node->children[0]); // <F>
//        if (node->children.size() > 1)
//            GenerateCode(node->children[1]); // <T'>
//    }
//    else if (node->type == "<T'>") {
//        const std::string& op = node->children[0]->type;
//        GenerateCode(node->children[1]); // <F>
//        out << op << endl;
//
//        if (node->children.size() == 3)
//            GenerateCode(node->children[2]); // <T'>
//    }
//    else if (node->type == "<F>") {
//        ParseTreeNode* leaf = node->children[0];
//        if (leaf->type == "V") {
//            out << "push " << std::get<1>(leaf->token).str << endl;
//        }
//        else if (leaf->type == "C") {
//            out << "push " << std::get<1>(leaf->token).str << endl;
//        }
//        else if (leaf->type == "m") {
//            out << "push " << std::get<1>(leaf->token).str << endl;
//        }
//        else if (leaf->type == "(") {
//            GenerateCode(node->children[1]); // <E>
//        }
//    }
//    else if (node->type == "pow") {
//        // pow(<E>)
//        GenerateCode(node->children[2]); // <E>
//        out << "pow" << endl;
//    }
//    else if (node->type == "m") {
//        // m (множество или константа множества)
//        out << "push " << std::get<1>(node->token).str << endl;
//    }
//    else if (node->type == "afm") {
//        // afm(<E>)
//        GenerateCode(node->children[1]); // <E>
//        out << "afm" << endl;
//    }
//    else if (node->type == "acs") {
//        // acs(<E>, <E>)
//        GenerateCode(node->children[1]); // первый <E>
//        GenerateCode(node->children[3]); // второй <E> — после запятой
//        out << "acs" << endl;
//    }
//    else { // Для терминалов и остальных — рекурсивно обходить детей
//        for (auto child : node->children)
//            GenerateCode(child);
//    }
//}
//
//void Syntax_Analyzer::ReportUnusedLabels() {
//    unordered_set<string> used;
//
//    // Собираем все использованные внутренние метки
//    ifstream in("output.txt");
//    string line;
//    while (getline(in, line)) {
//        if (line.find("jmp ") != string::npos || line.find("ji ") != string::npos) {
//            istringstream iss(line);
//            string word, label;
//            iss >> word >> label;
//            used.insert(label);
//        }
//    }
//    in.close();
//
//    for (const auto& [user, internal] : labelMap) {
//        if (used.find(internal) == used.end()) {
//            cerr << "Предупреждение: Метка '" << user << "' объявлена, но нигде не используется\n";
//        }
//    }
//
//    for (const string& label : used) {
//        bool declared = false;
//        for (const auto& [user, internal] : labelMap) {
//            if (internal == label) {
//                declared = true;
//                break;
//            }
//        }
//        if (!declared) {
//            cerr << "Ошибка: Переход на несуществующую метку '" << label << "'\n";
//        }
//    }
//}

void Syntax_Analyzer::ShowProgram() {
    cout << "Программа на стековом языке:\n" << endl;
    cout << magazine.back().atrs[0] + "\nend";
    lineCounter++;
}

string Syntax_Analyzer::OTR(string R) {
    if (R == "=")
        return "!=";
    if (R == "!=")
        return "=";
    if (R == "<")
        return ">=";
    if (R == "<=")
        return ">";
    if (R == ">")
        return "<=";
    if (R == ">=")
        return "<";
    else "Оператор не распознан";
}
