#include <iostream>
#include <string>
#include "iomanip"
#include "multiset.h"
#include "Lexical_Analyzer L51.h"
#include "SPGrammar.h"

using namespace std;

int main() {
	setlocale(LC_ALL, "Rus");
	/*string filename("test1.txt");

	Tokenizer tokenizer;
	auto tokens = tokenizer.tokenize(filename);

	cout << "List of lexems:\n";
	for (const auto& [type, value, line] : tokens) {
		cout << "Lexem: " << left << setw(15) << tokenizer.tokenClassToString(type) << "\tValue: " << left << setw(18) << value << "\tLine: " << line << endl;
	}
	if (!tokenizer.error_lines.empty()) {
		cout << "\nError found on lines: ";
		for (int error_line : tokenizer.error_lines)
			cout << error_line << " ";
		cout << endl;
	}*/

	SP_Analyzer g;

	ifstream file("grammar_3.txt");
	if (!file.is_open()) {
		cerr << "Не удалось открыть файл 'grammar_4.txt'" << endl;
		return 1;
	}

	g.LoadRules(file);
	//g.PrintGrammar();
	if (g.CheckGrammar())
		g.PrintPrecedenceTable();
	else
		cout << "Эта грамматика не ПП :(" << endl;

	string s = "<T>";
	/*cout << "НАЧ(" << s << "): ";
	for (const auto& elem : g.START(s))
		cout << elem << " ";
	s = "<T>";
	cout << endl;
	cout << "FIRST(" << s << "): ";
	for (const auto& elem : g.FIRST(s))
		cout << elem << " ";*/
	/*s = "<T>";
	cout << endl;
	cout << "КОН(" << s << "): ";
	for (const auto& elem : g.END(s))
		cout << elem << " ";*/

	return 0;
}