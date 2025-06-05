#pragma once
#include <fstream>
#include "Lexical_Analyzer L51.h"
#include "SPGrammar.h"

struct ParseTreeNode {
	string type; // Ќетерминал или терминал
	tuple<SymbolicTokenClass, TokenValue, int> token; // “олько дл€ терминалов
	vector<ParseTreeNode*> children;
	

	ParseTreeNode(const std::string& t) : type(t) {}
	ParseTreeNode(const std::string& t, const tuple<SymbolicTokenClass, TokenValue, int>& tok) : type(t), token(tok) {}
};

string TokenToString(tuple<SymbolicTokenClass, TokenValue, int> token);

class Syntax_Analyzer : protected Tokenizer {
	vector<string> magazine;
	vector<map<string, vector<string>>> history;
	stack<ParseTreeNode*> parse_stack;
	unordered_map<string, string> labelMap;
	int labelCounter = 0;

	string resolveLabel(const std::string& userLabel);

public:

	void Parse(string filename_P, ifstream& file_G);
	void ShowHistory();
	void PrintTree(ParseTreeNode*, int);
	void CollectIdentifiers(ParseTreeNode* node, vector<string>& vars);
	void GenerateCodeAsData(ParseTreeNode* node);
	void GenerateCode(ParseTreeNode* node);
	void ReportUnusedLabels();
	int label_counter = 0; // дл€ генерации уникальных меток

	string newLabel() {
		return "L" + to_string(label_counter++);
	}
};