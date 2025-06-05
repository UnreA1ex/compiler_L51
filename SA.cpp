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
    if (g.CheckGrammar()) {
        //cout << "Эта грамматика является ПП!" << endl;
        //cout << g.precedence_table["L"][":"] << endl;
    }
    else {
        cout << "Эта грамматика не ПП :(" << endl;
        return;
    }
    auto tokens = tokenize(filename_P);
    TokenValue tokenvalue;
    tokenvalue.pInt = nullptr;
    tokenvalue.pMultiset = nullptr;
    tokenvalue.pString = nullptr;
    tokenvalue.str = "$";
    tokens.push_back(make_tuple(DOLLAR, tokenvalue, -1));

    magazine.push_back("$");
    if (tokens.empty()) {
        cout << "Ошибка: нет токенов для анализа" << endl;
        return;
    }

    cout << magazine[0] << " " << TokenToString(tokens[0]) << endl;

    while (!tokens.empty()) {
        string cur_token = TokenToString(tokens.front());
        if (magazine.back() == g.start_symbol)
            break;
        // Проверка наличия элементов в таблице предшествования
        if (g.precedence_table.count(magazine.back()) == 0 ||
            g.precedence_table[magazine.back()].count(cur_token) == 0) {
            cout << "Ошибка: отсутствует отношение предшествования между "
                << magazine.back() << " и " << cur_token << endl;
            return;
        }

        char relation = g.precedence_table[magazine.back()][cur_token];

        switch (relation) {
        case '<':
        case '=':
            magazine.push_back(cur_token);
            tok = tokens[0]; // сохраняем токен
            parse_stack.push(new ParseTreeNode(cur_token, tok)); // терминальный узел
            tokens.erase(tokens.begin());
            break;

        case '>': {
            vector<string> prod;
            size_t len = magazine.size();

            // Безопасный проход по магазину
            while (len >= 2) {
                string prev = magazine[len - 2];
                string current = magazine[len - 1];

                if (g.precedence_table.count(prev) &&
                    g.precedence_table[prev].count(current) &&
                    g.precedence_table[prev][current] == '=') {
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
            bool found = false;
            for (const auto& rule : g.productions) {
                for (const auto& production : rule.second) {
                    if (production.symbols == prod) {
                        vector<ParseTreeNode*> children;
                        for (int i = 0; i < prod.size(); ++i) {
                            if (!parse_stack.empty()) {
                                children.insert(children.begin(), parse_stack.top());
                                parse_stack.pop();
                            }
                        }
                        auto parent = new ParseTreeNode(rule.first);
                        parent->children = move(children);
                        parse_stack.push(parent);
                        magazine.push_back(rule.first);
                        history.emplace_back(map<string, vector<string>>{{rule.first, prod}});
                        found = true;
                        break;
                    }
                }
                if (found) break;
            }

            if (!found) {
                cout << "Ошибка: не найдено правило для свёртки: ";
                for (const auto& s : prod) cout << s << " ";
                cout << endl;
                return;
            }
            break;
        }

        default:
            cout << "Ошибка: программа некорректна :(" << endl;
            return;
        }

        cout << "Магазин: ";
        for (const auto& elem : magazine)
            cout << elem << " ";
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
        cout << endl;
    }

    // Финальная проверка успешного разбора
    if (magazine.size() == 2 && magazine.back() == g.start_symbol) {
        cout << "Разбор завершён успешно!" << endl;
        if (!parse_stack.empty()) {
            ParseTreeNode* root = parse_stack.top();
            PrintTree(root, 0);
            GenerateCode(root);
            ReportUnusedLabels();
        }
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
                cout << elem_ << " ";
            }
        }
        cout << endl;
    }
}

void Syntax_Analyzer::PrintTree(ParseTreeNode* node, int depth = 0) {
    if (!node) return;
    cout << string(depth * 2, ' ') << node->type;
    if (!(std::get<1>(node->token).str).empty())
        std::cout << " [" << std::get<1>(node->token).str << "]";

    cout << endl;
    for (auto* child : node->children) {
        PrintTree(child, depth + 1);
    }
}

void Syntax_Analyzer::CollectIdentifiers(ParseTreeNode* node, vector<string>& vars) {
    if (!node) return;

    if (node->type == "V") {
        vars.push_back(std::get<1>(node->token).str);
    }

    for (auto* child : node->children) {
        CollectIdentifiers(child, vars);
    }
}

void Syntax_Analyzer::GenerateCodeAsData(ParseTreeNode* node) {
    if (!node) return;

    if (node->type == "<E>" || node->type == "<T>") {
        GenerateCodeAsData(node->children[0]);
        if (node->children.size() > 1)
            GenerateCodeAsData(node->children[1]);
    }
    else if (node->type == "<E'>" || node->type == "<T'>") {
        // + <T> <E'> или * <F> <T'>
        string op = node->children[0]->type;
        GenerateCodeAsData(node->children[1]);

        out << "push " << op << endl;

        if (node->children.size() == 3)
            GenerateCodeAsData(node->children[2]);
    }
    else if (node->type == "<F>") {
        ParseTreeNode* leaf = node->children[0];
        if (leaf->type == "V") {
            out << "push " << std::get<1>(leaf->token).str << endl;
        }
        else if (leaf->type == "m") {
            out << "push " << std::get<1>(leaf->token).str << endl;
        }
        else if (leaf->type == "C" || leaf->type == "c") {
            out << "push " << std::get<1>(leaf->token).str << endl;
        }
    }
}

void Syntax_Analyzer::GenerateCode(ParseTreeNode* node) {

    if (!node) return;

    if (node->type == "<prog>") {
        for (auto child : node->children)
            GenerateCode(child);
    }
    else if (node->type == "L") {
        string label = newLabel();
        out << label << ":" << endl;
    }
    else if (node->type == "<op>" && node->children.size() >= 2 &&
        node->children[0]->type == "jump" &&
        node->children[1]->type == "<label>") {
        
        string userLabel = std::get<1>(node->children[1]->children[0]->token).str;
        string internalLabel = resolveLabel(userLabel);
        out << "jmp " << internalLabel << endl;
    }
    else if (node->type == "<op>" && node->children.size() >= 2 &&
        node->children[0]->type == "L" &&
        node->children[1]->type == ":") {

        string userLabel = std::get<1>(node->children[0]->token).str;
        string internalLabel = resolveLabel(userLabel);
        out << internalLabel << ":" << endl;
    }
    else if (node->type == "<while>") {
        // while <test> do <prog> od;

        string startLabel = newLabel();
        string endLabel = newLabel();

        out << startLabel << ":" << endl;
        // <test_w> всегда 3 ребёнка: E, R, E
        ParseTreeNode* test = node->children[1]; // <test_w>
        GenerateCode(test->children[0]); 
        GenerateCode(test->children[2]);

        // Сравнение
        std::string rel_op = std::get<1>(test->children[1]->token).str;

        if (rel_op == "<") out << ">=" << endl;
        else if (rel_op == "<=") out << ">" << endl;
        else if (rel_op == ">") out << "<=" << endl;
        else if (rel_op == ">=") out << "<" << endl;
        else if (rel_op == "==") out << "!=" << endl;
        else if (rel_op == "!=") out << "==" << endl;
        else out << rel_op << endl;

        out << "ji " << endLabel << endl;

        // Тело цикла
        GenerateCode(node->children[3]); // <prog>

        out << "jmp " << startLabel << endl;
        out << endLabel << ":" << endl;
    }
    else if (node->type == "<for_1>") {
        auto for3 = node->children[0];
        auto body = node->children[1];

        string varName = std::get<1>(for3->children[1]->token).str;

        ParseTreeNode* fromExpr = for3->children[3];
        ParseTreeNode* toExpr = for3->children[5];
        ParseTreeNode* stepExpr = nullptr;
        if (for3->children.size() > 6)
            stepExpr = for3->children[6]->children[1];

        // Инициализация счётчика
        out << "push " << varName << endl;
        GenerateCode(fromExpr);
        out << "pop " << varName << endl;

        string startLabel = newLabel();
        string endLabel = newLabel();

        out << startLabel << ":" << endl;

        out << "push " << varName << endl;
        GenerateCode(toExpr);
        out << ">" << endl;
        out << "ji " << endLabel << endl;

        // тело цикла
        GenerateCode(body);

        out << "push " << varName << endl;
        if (stepExpr) GenerateCode(stepExpr);
        else out << "push 1" << endl;
        out << "+" << endl;
        out << "pop " << varName << endl;

        out << "jmp " << startLabel << endl;
        out << endLabel << ":" << endl;
    }
    else if (node->type == "<if_1>") {
        // if <test> <prog>

        string elseLabel = newLabel();
        string endLabel = newLabel();

        GenerateCode(node->children[0]); // <test>
        out << "ji " << elseLabel << endl;

        GenerateCode(node->children[1]); // <prog> (then branch)
        out << "jmp " << endLabel << endl;

        out << elseLabel << ":" << endl;
        if (node->children.size() > 2) // если есть else
            GenerateCode(node->children[2]); // <else> (else branch)

        out << endLabel << ":" << endl;
    }
    else if (node->type == "<test_if>") {
        // ( <E> R <E> )
        GenerateCode(node->children[1]); // левая часть
        GenerateCode(node->children[3]); // правая часть

        string rel_op = std::get<1>(node->children[2]->token).str;
        if (rel_op == "<") out << ">=" << endl;
        else if (rel_op == "<=") out << ">" << endl;
        else if (rel_op == ">") out << "<=" << endl;
        else if (rel_op == ">=") out << "<" << endl;
        else if (rel_op == "==") out << "!=" << endl;
        else if (rel_op == "!=") out << "==" << endl;
        else out << rel_op << endl;
    }
    else if (node->type == "<let>") {
        // let V = <E>
        string varName = std::get<1>(node->children[1]->token).str;
        GenerateCodeAsData(node->children[3]);
        
        out << "pop " << varName << endl;
    }
    else if (node->type == "<write>") {
        // write <op_out>
        GenerateCode(node->children[0]);
        out << "write " << endl;
    }
    else if (node->type == "<read>") {
        // read V [, ...]
        vector<string> vars;
        CollectIdentifiers(node, vars);
        for (const auto& varName : vars) {
            out << "read " << varName << endl;
        }
    }
    else if (node->type == "<E>") {
        // <E> -> <T> | <T> <E'>
        GenerateCode(node->children[0]); // <T>
        if (node->children.size() > 1)
            GenerateCode(node->children[1]); // <E'>
    }
    else if (node->type == "<E'>") {
        // <E'> -> + <T> <E'> | - <T> <E'> | ...
        const string& op = node->children[0]->type;
        GenerateCode(node->children[1]); // <T>
        out << op << endl;
        if (node->children.size() == 3)
            GenerateCode(node->children[2]); // <E'>
    }
    else if (node->type == "<T>") {
        GenerateCode(node->children[0]); // <F>
        if (node->children.size() > 1)
            GenerateCode(node->children[1]); // <T'>
    }
    else if (node->type == "<T'>") {
        const std::string& op = node->children[0]->type;
        GenerateCode(node->children[1]); // <F>
        out << op << endl;

        if (node->children.size() == 3)
            GenerateCode(node->children[2]); // <T'>
    }
    else if (node->type == "<F>") {
        ParseTreeNode* leaf = node->children[0];
        if (leaf->type == "V") {
            out << "push " << std::get<1>(leaf->token).str << endl;
        }
        else if (leaf->type == "C") {
            out << "push " << std::get<1>(leaf->token).str << endl;
        }
        else if (leaf->type == "m") {
            out << "push " << std::get<1>(leaf->token).str << endl;
        }
        else if (leaf->type == "(") {
            GenerateCode(node->children[1]); // <E>
        }
    }
    else if (node->type == "pow") {
        // pow(<E>)
        GenerateCode(node->children[2]); // <E>
        out << "pow" << endl;
    }
    else if (node->type == "m") {
        // m (множество или константа множества)
        out << "push " << std::get<1>(node->token).str << endl;
    }
    else if (node->type == "afm") {
        // afm(<E>)
        GenerateCode(node->children[1]); // <E>
        out << "afm" << endl;
    }
    else if (node->type == "acs") {
        // acs(<E>, <E>)
        GenerateCode(node->children[1]); // первый <E>
        GenerateCode(node->children[3]); // второй <E> — после запятой
        out << "acs" << endl;
    }
    else { // Для терминалов и остальных — рекурсивно обходить детей
        for (auto child : node->children)
            GenerateCode(child);
    }
}

void Syntax_Analyzer::ReportUnusedLabels() {
    unordered_set<string> used;

    // Собираем все использованные внутренние метки
    ifstream in("output.txt");
    string line;
    while (getline(in, line)) {
        if (line.find("jmp ") != string::npos || line.find("ji ") != string::npos) {
            istringstream iss(line);
            string word, label;
            iss >> word >> label;
            used.insert(label);
        }
    }
    in.close();

    for (const auto& [user, internal] : labelMap) {
        if (used.find(internal) == used.end()) {
            cerr << "Предупреждение: Метка '" << user << "' объявлена, но нигде не используется\n";
        }
    }

    for (const string& label : used) {
        bool declared = false;
        for (const auto& [user, internal] : labelMap) {
            if (internal == label) {
                declared = true;
                break;
            }
        }
        if (!declared) {
            cerr << "Ошибка: Переход на несуществующую метку '" << label << "'\n";
        }
    }
}