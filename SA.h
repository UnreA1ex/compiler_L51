#pragma once
#include <fstream>
#include "Lexical_Analyzer L51.h"
#include "SPGrammar.h"
#include <algorithm>

struct ParseTreeNode {
	string type; // ���������� ��� ��������
	tuple<SymbolicTokenClass, TokenValue, int> token; // ������ ��� ����������
	vector<ParseTreeNode*> children;
	

	ParseTreeNode(const std::string& t) : type(t) {}
	ParseTreeNode(const std::string& t, const tuple<SymbolicTokenClass, TokenValue, int>& tok) : type(t), token(tok) {}
};

struct label {
	string name;
	int line;
};

struct vartype {
	string var;
	string type;
};

struct withAtr {
	string symbol;
	vector<string> atrs;
};



string TokenToString(tuple<SymbolicTokenClass, TokenValue, int> token);

class Syntax_Analyzer : protected Tokenizer {
	vector<withAtr> magazine;
	vector<map<string, vector<withAtr>>> history;
	stack<ParseTreeNode*> parse_stack;
	unordered_map<string, string> labelMap;
	map<string, int> table_labels_;
	int labelCounter = 0;
	int lineCounter = 0;

	string resolveLabel(const std::string& userLabel);

public:

	string ConnectedLabels(const string& program);
	void Parse(string filename_P, ifstream& file_G);
	void ShowHistory();
	/*void PrintTree(ParseTreeNode*, int);
	void CollectIdentifiers(ParseTreeNode* node, vector<string>& vars);
	void GenerateCodeAsData(ParseTreeNode* node);
	void GenerateCode(ParseTreeNode* node);
	void ReportUnusedLabels();*/
	void ShowProgram();
	int label_counter = 0; // ��� ��������� ���������� �����

	string newLabel() {
		return to_string(label_counter++);
	}

	string OTR(string R);
};