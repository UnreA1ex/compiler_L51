#pragma once
#include <set>
#include <sstream>
#include <iomanip>
#include "Lexical_Analyzer L51.h"
#include "SA.h"

using namespace std;

//enum Nonterms {
//	S,
//	announ,
//	ann,
//	vl,
//	type,
//	prog,
//	op,
//	test,
//	step,
//	els,
//	op_in,
//	op_out,
//	mult_bran,
//	o_wise,
//	E,
//	T,
//	F
//};

//unordered_map<string, Nonterms> nonterms = {
//	{"S", S}, {"<announ>", announ}, {"<ann>", ann}, {"<vl>", vl}, {"<type>", type}, {"<prog>", prog},
//	{"<op>", op}, {"<test>", test}, {"<step>", step}, {"<els>", els}, {"<op_in>", op_in}, {"<op_out>", op_out},
//	{"<mult_bran>", mult_bran}, {"<o_wise>", o_wise}, {"<E>", E}, {"<T>", T}, {"<F>", F}
//};

struct Production {
	vector<string> symbols; // Правые части
};

class SP_Analyzer {
	friend class Syntax_Analyzer;
	set<string> nonterminals;
	set<string> terminals;
	string start_symbol;
	map<string, vector<Production>> productions;  // Каждому нетерминалу свои правые части из структуры "Production"
	unordered_map<string, unordered_map<string, char>> precedence_table;
	ofstream table;
	bool flag_SP;

public:
	SP_Analyzer() : flag_SP(true) {}
	bool isNonterminal(string);
	void AddProduction(const string&, const vector<string>&);
	void Transfer();
	void PrintGrammar() const;
	void LoadRules(ifstream& file);
	const set<string> FIRST(string nonterminal, set<string>& visited);
	const set<string> START(string nonterminal, set<string>& visited);
	const set<string> END(string nonterminal, set<string>& visited);
	//const set<string> END_Terminals(string nonterminal, set<string>& visited);
	bool CheckGrammar();
	void ifUseless();
	void ifLRecursion();
	void PrintPrecedenceTable();
};