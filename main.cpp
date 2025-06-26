#include <iostream>
#include <string>
#include <iomanip>
#include "SPGrammar.h"
#include "SA.h"

int main() {
	setlocale(LC_ALL, "Rus");
	string filename_P("test12.txt");

	Tokenizer tokenizer;
	auto tokens = tokenizer.tokenize(filename_P);
	
	/*
	cout << "List of lexems:\n";
	for (const auto& [type, value, line] : tokens) {
		cout << "Lexem: " << left << setw(15) << tokenizer.tokenClassToString(type) << "\tValue: " << left << setw(18) << value.str << "\tLine: " << line << endl;
	}
	if (!tokenizer.error_lines.empty()) {
		cout << "\nError found on lines: ";
		for (int error_line : tokenizer.error_lines)
			cout << error_line << " ";
		cout << endl;
	}
	cout << "---------------------------------------------------------------" << endl;
	*/

	//SP_Analyzer g;
	ifstream file_G("grammar.txt");
	if (!file_G.is_open()) {
		cerr << "Не удалось открыть файл 'grammar.txt'" << endl;
		return 1;
	}
	//g.LoadRules(file_G);

	Syntax_Analyzer SA;
	SA.Parse(filename_P, file_G);
	//SA.ShowHistory();
	//g.PrintGrammar();
	/*g.CheckGrammar();
	g.PrintPrecedenceTable();*/
	/*if (g.CheckGrammar()) {
		cout << "Эта грамматика является ПП!" << endl;
		g.PrintPrecedenceTable();
	}
	else
		cout << "Эта грамматика не ПП :(" << endl;*/



		/*string s = "<E>";
		set<string> visited;
		cout << "НАЧ(" << s << "): ";
		for (const auto& elem : g.START(s, visited))
			cout << elem << " ";
		s = "<E>";
		cout << endl;
		cout << "FIRST(" << s << "): ";
		visited.clear();
		for (const auto& elem : g.FIRST(s, visited))
			cout << elem << " ";*/
			/*set<string> v;
			s = "<T>";
			cout << endl;
			cout << "КОН(" << s << "): ";
			for (const auto& elem : g.END(s, v))
				cout << elem << " ";*/

	return 0;
}