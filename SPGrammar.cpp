#include "SPGrammar.h"

bool isInSet(const string& element, const set<string>& my_set) {
	return my_set.count(element);
}

bool SP_Analyzer::isNonterminal(string s) {
	return isInSet(s, nonterminals);
}

void SP_Analyzer::AddProduction(const string& lhs, const vector<string>& rhs) {
	nonterminals.insert(lhs);
	productions[lhs].push_back({ rhs }); // {rhs} - uniform initialization ��� �������� � ����� �������
	for (const string& sym : rhs) {
		if (sym != "eps")
			terminals.insert(sym);
		else {
			cout << "������� eps-�������" << endl;
			flag_SP = false;
		}
	}
}

void SP_Analyzer::Transfer() {
	for (const auto& nont : nonterminals) {
		terminals.erase(nont);
	}
}

void SP_Analyzer::PrintGrammar() const {
	cout << "��������� ����������: " << start_symbol << "\n\n�������:\n";
	for (const auto& [lhs, rules] : productions) {
		for (const auto& prod : rules) {
			cout << lhs << " -> ";
			for (const auto& syms : prod.symbols) {
				cout << syms << ' ';
			}
			cout << '\n';
		}
	}
	cout << "\n�����������: ";
	for (const auto& nt : nonterminals) cout << nt << " ";
	cout << "\n���������: ";
	for (const auto& t : terminals) cout << t << " ";
	cout << endl;
}

//void SP_Analyzer::LoadRules(ifstream& file) {
//	string line;
//	while (getline(file, line)) {
//		if (line.empty()) continue;
//
//		size_t pos = line.find("->");  // ����� ������� ���������
//		/*for (int i = 0; i < line.size(); i++) {
//		  if (i < pos) {
//			if (i == 0) {
//			  if (line[i] == 'S') {
//				lhs = line[i];
//			  }
//			  else {
//				continue;
//			  }
//			}
//			while (line[i] != '>')
//			  lhs += line[i];
//		  }
//		  else {
//			i +=
//		  }
//		}*/
//		string lhs = line.substr(0, pos - 1);
//		string rhs = line.substr(pos + 2);
//
//		stringstream ss(rhs);
//		string token;
//		vector<string> current;
//
//		while (ss >> token) {
//			if (token == "/*") {
//				string s1;
//				string s2;
//				ss >> s1 >> s2;
//				token += s1 + s2;
//			}
//			if (token == "|") {
//				AddProduction(lhs, current);
//				current.clear();
//			}
//			else {
//				size_t position = token.find('>');
//				if (position != string::npos) {
//					bool flag = false;
//					string cur = "";
//					for (int i = 0; i < token.size(); i++) {
//						if (token[i] == '<') {
//							flag = true;
//							if (!cur.empty()) {
//								current.push_back(cur);
//								cur = "";
//							}
//						}
//						cur += token[i];
//						if (token[i] == '>') {
//							flag = false;
//							current.push_back(cur);
//							cur = "";
//						}
//					}
//					if (!cur.empty()) {
//						current.push_back(cur);
//					}
//				}
//				else
//					current.push_back(token);
//			}
//		}
//
//		if (!current.empty()) AddProduction(lhs, current);
//
//		if (start_symbol.empty())
//			start_symbol = lhs;
//	}
//
//	ifUseless();
//	Transfer();
//	ifLRecursion();
//
//}


void SP_Analyzer::LoadRules(ifstream& file) {
	string line;
	while (getline(file, line)) {
		if (line.empty()) continue;

		size_t pos = line.find("->");  // ����� ������� ���������
		/*for (int i = 0; i < line.size(); i++) {
			if (i < pos) {
				if (i == 0) {
					if (line[i] == 'S') {
						lhs = line[i];
					}
					else {
						continue;
					}
				}
				while (line[i] != '>')
					lhs += line[i];
			}
			else {
				i +=
			}
		}*/
		string lhs = line.substr(0, pos - 1);
		string rhs = line.substr(pos + 2);

		stringstream ss(rhs);
		string token;
		vector<string> current;

		while (ss >> token) {
			size_t pos_1 = token.find(',');
			if (token == "(") {
				current.push_back(token);
				continue;
			}
			if (token == ")") {
				current.push_back(token);
				continue;
			}
			if (token == "L:") {
				current.push_back("L");
				current.push_back(":");
				continue;
			}
			if (token == ",") {
				current.push_back(token);
				continue;
			}
			if (pos_1 != string::npos) {
				current.push_back(token.substr(0, pos_1));
				current.push_back(token.substr(pos_1));
				continue;
			}
			if (token == "/*") {
				string s1;
				string s2;
				ss >> s1 >> s2;
				token += s1 + s2;
			}
			if (token == "|") {
				AddProduction(lhs, current);
				current.clear();
			}
			else {
				size_t position = token.find('(');
				size_t position_1 = token.find('>');
				if (position_1 != string::npos && position != string::npos) {
					bool flag = false;
					string cur = "";
					for (int i = 0; i < token.size(); i++) {
						if (token[i] == '(') {
							if (!cur.empty()) {
								current.push_back(cur);
								cur = "";
							}
							current.push_back(string(1, '('));
							i++;
						}
						if (token[i] == '<') {
							flag = true;
							if (!cur.empty()) {
								current.push_back(cur);
								cur = "";
							}
						}
						cur += token[i];
						if (token[i] == '>') {
							flag = false;
							current.push_back(cur);
							cur = "";
						}
					}
					if (!cur.empty()) {
						current.push_back(cur);
					}
				}
				else {
					if (position != string::npos) {
						string cur = "";
						for (int i = 0; i < token.size(); i++) {
							if (token[i] == '(') {
								current.push_back(string(1, '('));
								i++;
							}
							cur += token[i];
						}
						if (cur.size() > 0) {
							current.push_back(cur);
						}
					}
					else if (position_1 != string::npos) {
						bool flag = false;
						string cur = "";
						for (int i = 0; i < token.size(); i++) {
							if (token[i] == '<') {
								flag = true;
								if (!cur.empty()) {
									current.push_back(cur);
									cur = "";
								}
							}
							cur += token[i];
							if (token[i] == '>') {
								flag = false;
								current.push_back(cur);
								cur = "";
							}
						}
						if (!cur.empty()) {
							current.push_back(cur);
						}
					}
					else
						current.push_back(token);
				}
			}
		}

		if (!current.empty()) AddProduction(lhs, current);

		if (start_symbol.empty())
			start_symbol = lhs;
	}

	ifUseless();
	Transfer();
	ifLRecursion();
}

const set<string> SP_Analyzer::FIRST(string nonterminal, set<string>& visited) {
	set<string> set_first;

	if (visited.count(nonterminal))
		return {};

	visited.insert(nonterminal);

	if (!isNonterminal(nonterminal))
		return { nonterminal };
	for (const auto& [lhs, rules] : productions) {
		if (lhs == nonterminal) {
			for (const auto& rule : rules) {
				if (!isNonterminal(rule.symbols[0])) {
					set_first.insert(rule.symbols[0]);
				}
				else {
					set<string> first = FIRST(rule.symbols[0], visited);
					for (const auto& sym : first) {
						set_first.insert(sym);
					}
				}
			}
		}
	}
	return set_first;
}

/*
const set<string> SP_Analyzer::FIRST(string nonterminal) {
	set<string> set_first;
	if (!isNonterminal(nonterminal))
		return { nonterminal };
	for (const auto& [lhs, rules] : productions) {
		if (lhs == nonterminal) {
			for (const auto& rule : rules) {
				if (rule.symbols[0] != nonterminal)
					if (!isNonterminal(rule.symbols[0])) {
						set_first.insert(rule.symbols[0]);
					}
					else {
						for (const auto& sym : FIRST(rule.symbols[0])) {
							set_first.insert(sym);
						}
					}
			}
		}
	}
	return set_first;
}
*/
/*
const set<string> SP_Analyzer::START(string nonterminal) {
	set<string> set_start;
	if (!isNonterminal(nonterminal))
		return {};
	for (const auto& [lhs, rules] : productions) {
		if (lhs == nonterminal) {
			for (const auto& rule : rules) {
				set_start.insert(rule.symbols[0]);
				if (rule.symbols[0] != nonterminal)
					if (isNonterminal(rule.symbols[0]))
						for (const auto& sym : START(rule.symbols[0])) {
							set_start.insert(sym);
						}
			}
		}
	}
	return set_start;
}
*/

const set<string> SP_Analyzer::START(string nonterminal, set<string>& visited) {
	set<string> set_start;

	if (visited.count(nonterminal))
		return {};

	visited.insert(nonterminal);

	if (!isNonterminal(nonterminal))
		return {};
	for (const auto& [lhs, rules] : productions) {
		if (lhs == nonterminal) {
			for (const auto& rule : rules) {
				set_start.insert(rule.symbols[0]);
				if (isNonterminal(rule.symbols[0])) {
					set<string> start = START(rule.symbols[0], visited);
					for (const auto& sym : start) {
						set_start.insert(sym);
					}
				}
			}
		}
	}
	return set_start;
}



const set<string> SP_Analyzer::END(string nonterminal, set<string>& visited) {
	set<string> set_end;

	// ���� ������ nonterminal ��� �����������
	if (visited.count(nonterminal))
		return {};

	visited.insert(nonterminal);

	if (!isNonterminal(nonterminal))
		return {};

	for (const auto& [lhs, rules] : productions) {
		if (lhs == nonterminal) {
			for (const auto& rule : rules) {
				string s = *rule.symbols.rbegin();
				set_end.insert(s);
				if (isNonterminal(s)) {
					set<string> end = END(s, visited);
					for (const auto& sym : end) {
						set_end.insert(sym);
					}
				}
			}
		}
	}
	return set_end;
}

//const set<string> SP_Analyzer::END_Terminals(string nonterminal, set<string>& visited) {
//	set<string> set_endTerminals;
//	// ���� ������ nonterminal ��� �����������
//	if (visited.count(nonterminal))
//		return {};
//
//	visited.insert(nonterminal);
//
//	if (!isNonterminal(nonterminal))
//		return {};
//
//	for (const auto& [lhs, rules] : productions) {
//		if (lhs == nonterminal) {
//			for (const auto& rule : rules) {
//				string s = *rule.symbols.rbegin();
//				if (isNonterminal(s))
//					for (const auto& sym : END_Terminals(s, visited)) {
//						set_endTerminals.insert(sym);
//					}
//				else
//					set_endTerminals.insert(s);
//			}
//		}
//	}
//	return set_endTerminals;
//}



//bool SP_Analyzer::CheckGrammar() {
//	
//	if (!flag_SP)
//		return false;
//
//	for (const auto& [lhs, rules] : productions) {
//		for (const auto& prod : rules) {
//			auto it_row = precedence_table.find("$");
//			if (it_row != precedence_table.end()) {
//				set<string> visited;
//				set<string> start = START(start_symbol, visited);
//				for (const auto& syms : start) {
//					auto it_col = it_row->second.find(syms);
//					if (it_col == it_row->second.end()) {
//						precedence_table["$"][syms] = '<';
//					}
//					else {
//						if (it_col->second != '<') {
//							// ������ ��� ���� ��������� � ��� �� '<' � ����� ���������� ��������
//							cout << "��������: ��� ���� " << it_col->second << ", � ����� �������� <" << endl;
//							return false;
//						}
//					}
//				}
//			}
//			else {
//				set<string> visited;
//				set<string> first = FIRST(start_symbol, visited);
//				for (const auto& syms : first) {
//					precedence_table["$"][syms] = '<';
//				}
//			}
//			
//			for (int i = 0; i < prod.symbols.size() - 1; i++) {
//				// ��������, ���������� �� ���� prod.symbols[i] � �������
//				it_row = precedence_table.find(prod.symbols[i]);
//				if (it_row != precedence_table.end()) {
//					// ������ ������� � ��������� �������
//					auto it_col = it_row->second.find(prod.symbols[i + 1]);
//					if (it_col == it_row->second.end()) {
//						// ������ ��� �� ��������� � ����� ���������
//						precedence_table[prod.symbols[i]][prod.symbols[i + 1]] = '=';
//						if (isNonterminal(prod.symbols[i + 1])) {
//							//cout << "START ������ �����" << endl;
//							set<string> visited;
//							set<string> start = START(prod.symbols[i + 1], visited);
//							for (const auto& syms : start) {
//								// ������ ��� ��������� <�
//								it_col = it_row->second.find(syms);
//								if (it_col == it_row->second.end()) {
//									// ������ ��� �� ��������� � ����� ���������
//									precedence_table[prod.symbols[i]][syms] = '<';
//								}
//								else {
//									if (it_col->second != '<') {
//										// ������ ��� ���� ��������� � ��� �� '<' � ����� ���������� ��������
//										cout << "��������: ��� ���� " << it_col->second << ", � ����� �������� <" << endl;
//										cout << "�������� ����� " << prod.symbols[i] << " � " << syms << endl;
//										return false;
//									}
//								}
//							}
//						}
//						if (isNonterminal(prod.symbols[i])) {
//							//cout << "END ������ �����" << endl;
//							set<string> visited;
//							set<string> end = END(prod.symbols[i], visited);
//							for (const auto& syms : end) {
//								//cout << "FIRST ������ �����" << endl;
//								set<string> visited;
//								set<string> first = FIRST(prod.symbols[i + 1], visited);
//								for (const auto& syms_next : first) {
//									// ������ ��� ��������� �>
//									// ��������, ���������� �� ���� syms = END(prod.symbols[i]) � �������
//									it_row = precedence_table.find(syms);
//									if (it_row != precedence_table.end()) {
//										// ������ ������� � ��������� �������
//										it_col = it_row->second.find(syms_next);
//										if (it_col == it_row->second.end()) {
//											// ������ ��� �� ��������� � ����� ���������
//											precedence_table[syms][syms_next] = '>';
//										}
//										else {
//											if (it_col->second != '>') {
//												// ������ ��� ���� ��������� � ��� �� '>' � ����� ���������� ��������
//												cout << "��������: ��� ���� " << it_col->second << ", � ����� �������� >" << endl;
//												cout << "�������� ����� " << syms << " � " << syms_next << endl;
//												return false;
//											}
//										}
//									}
//									else {
//										// ������ �� ������� � ����� ��������� �������
//										precedence_table[syms][syms_next] = '>';
//									}
//								}
//								
//							}
//						}
//					}
//					else {
//						if (it_col->second != '=') {
//							// ������ ��� ���� ��������� � ��� �� '=' � ����� ���������� ��������
//							cout << "��������: ��� ���� " << it_col->second << ", � ����� �������� =" << endl;
//							cout << "�������� ����� " << prod.symbols[i] << " � " << prod.symbols[i + 1] << endl;
//							return false;
//						}
//					}
//				}
//				else {
//					// ������ �� ������� � ����� ��������� �������
//					precedence_table[prod.symbols[i]][prod.symbols[i + 1]] = '=';
//					it_row = precedence_table.find(prod.symbols[i]);
//					if (isNonterminal(prod.symbols[i + 1])) {
//						//cout << "START ������ �����" << endl;
//						set<string> visited;
//						set<string> start = START(prod.symbols[i + 1], visited);
//						for (const auto& syms : start) {
//							// ������ ��� ��������� <�
//							auto it_col = it_row->second.find(syms);
//							if (it_col == it_row->second.end()) {
//								// ������ ��� �� ��������� � ����� ���������
//								precedence_table[prod.symbols[i]][syms] = '<';
//							}
//							else {
//								if (it_col->second != '<') {
//									// ������ ��� ���� ��������� � ��� �� '<' � ����� ���������� ��������
//									cout << "��������: ��� ���� " << it_col->second << ", � ����� �������� <" << endl;
//									cout << "�������� ����� " << prod.symbols[i] << " � " << syms << endl;
//									return false;
//								}
//							}
//						}
//					}
//					if (isNonterminal(prod.symbols[i])) {
//						//cout << "END ������ �����" << endl;
//						set<string> visited;
//						set<string> end = END(prod.symbols[i], visited);
//						for (const auto& syms : end) {
//							//cout << "FIRST ������ �����" << endl;
//							set<string> visited;
//							set<string> first = FIRST(prod.symbols[i + 1], visited);
//							for (const auto& syms_next : first) {
//								// ������ ��� ��������� �>
//								// ��������, ���������� �� ���� syms = END(prod.symbols[i]) � �������
//								it_row = precedence_table.find(syms);
//								if (it_row != precedence_table.end()) {
//									// ������ ������� � ��������� �������
//									auto it_col = it_row->second.find(syms_next);
//									if (it_col == it_row->second.end()) {
//										// ������ ��� �� ��������� � ����� ���������
//										precedence_table[syms][syms_next] = '>';
//									}
//									else {
//										if (it_col->second != '>') {
//											// ������ ��� ���� ��������� � ��� �� '>' � ����� ���������� ��������
//											cout << "��������: ��� ���� " << it_col->second << ", � ����� �������� >" << endl;
//											cout << "�������� ����� " << syms << " � " << syms_next << endl;
//											cout << "���������� �������� �� ����� ������������ END: ";
//											for (const auto& elem : visited)
//												cout << elem << " ";
//											cout << endl;
//											cout << "END(" << prod.symbols[i] << "): ";
//											for (const auto& elem : end)
//												cout << elem << " ";
//											cout << endl;
//											cout << "��������� ���� �� " << prod.symbols[i] << " � " << prod.symbols[i + 1];
//											cout << endl;
//											return false;
//										}
//									}
//								}
//								else {
//									// ������ �� ������� � ����� ��������� �������
//									precedence_table[syms][syms_next] = '>';
//								}
//							}
//
//						}
//					}
//				}
//			}
//			set<string> visited;
//			//cout << "END ������ �����" << endl;
//			set<string> end = END(start_symbol, visited);
//			for (const auto& syms : end) {
//				it_row = precedence_table.find(syms);
//				if (it_row != precedence_table.end()) {
//					auto it_col = it_row->second.find("$");
//					if (it_col == it_row->second.end()) {
//						precedence_table[syms]["$"] = '>';
//					}
//					else {
//						if (it_col->second != '>') {
//							// ������ ��� ���� ��������� � ��� �� '>' � ����� ���������� ��������
//							cout << "��������: ��� ���� " << it_col->second << ", � ����� �������� >" << endl;
//							return false;
//						}
//					}
//				}
//				else {
//					// ������ �� ������� � ����� ��������� �������
//					precedence_table[syms]["$"] = '>';
//				}
//			}	
//		}
//	}
//	return true;
//}

bool SP_Analyzer::CheckGrammar() {

	if (!flag_SP)
		return false;

	for (const auto& [lhs, rules] : productions) {
		for (const auto& prod : rules) {
			auto it_row = precedence_table.find("$");
			if (it_row != precedence_table.end()) {
				set<string> visited;
				set<string> start = START(start_symbol, visited);
				for (const auto& syms : start) {
					auto it_col = it_row->second.find(syms);
					if (it_col == it_row->second.end()) {
						precedence_table["$"][syms] = '<';
					}
					else {
						if (it_col->second != '<') {
							// ������ ��� ���� ��������� � ��� �� '<' � ����� ���������� ��������
							cout << "��������: ��� ���� " << it_col->second << ", � ����� �������� <" << endl;
							return false;
						}
					}
				}
			}
			else {
				set<string> visited;
				set<string> first = FIRST(start_symbol, visited);
				for (const auto& syms : first) {
					precedence_table["$"][syms] = '<';
				}
			}

			for (int i = 0; i < prod.symbols.size() - 1; i++) {
				// ��������, ���������� �� ���� prod.symbols[i] � �������
				it_row = precedence_table.find(prod.symbols[i]);
				if (it_row != precedence_table.end()) {
					// ������ ������� � ��������� �������
					auto it_col = it_row->second.find(prod.symbols[i + 1]);
					if (it_col == it_row->second.end()) {
						// ������ ��� �� ��������� � ����� ���������
						precedence_table[prod.symbols[i]][prod.symbols[i + 1]] = '=';
						if (isNonterminal(prod.symbols[i + 1])) {
							//cout << "START ������ �����" << endl;
							set<string> visited;
							
							set<string> start = START(prod.symbols[i + 1], visited);
							for (const auto& syms : start) {
								// ������ ��� ��������� <�
								it_col = it_row->second.find(syms);
								if (it_col == it_row->second.end()) {
									// ������ ��� �� ��������� � ����� ���������
									precedence_table[prod.symbols[i]][syms] = '<';
								}
								else {
									if (it_col->second != '<') {
										// ������ ��� ���� ��������� � ��� �� '<' � ����� ���������� ��������
										cout << "��������: ��� ���� " << it_col->second << ", � ����� �������� <" << endl;
										cout << "�������� ����� " << prod.symbols[i] << " � " << syms << endl;
										return false;
									}
								}
							}
						}
						if (isNonterminal(prod.symbols[i])) {
							//cout << "END ������ �����" << endl;
							set<string> visited;
							set<string> end = END(prod.symbols[i], visited);
							for (const auto& syms : end) {
								//cout << "FIRST ������ �����" << endl;
								set<string> visited;
								set<string> first = FIRST(prod.symbols[i + 1], visited);
								for (const auto& syms_next : first) {
									// ������ ��� ��������� �>
									// ��������, ���������� �� ���� syms = END(prod.symbols[i]) � �������
									it_row = precedence_table.find(syms);
									if (it_row != precedence_table.end()) {
										// ������ ������� � ��������� �������
										it_col = it_row->second.find(syms_next);
										if (it_col == it_row->second.end()) {
											// ������ ��� �� ��������� � ����� ���������
											precedence_table[syms][syms_next] = '>';
										}
										else {
											if (it_col->second != '>') {
												// ������ ��� ���� ��������� � ��� �� '>' � ����� ���������� ��������
												cout << "��������: ��� ���� " << it_col->second << ", � ����� �������� >" << endl;
												cout << "�������� ����� " << syms << " � " << syms_next << endl;
												return false;
											}
										}
									}
									else {
										// ������ �� ������� � ����� ��������� �������
										precedence_table[syms][syms_next] = '>';
									}
								}

							}
						}
					}
					else {
						if (it_col->second != '=') {
							// ������ ��� ���� ��������� � ��� �� '=' � ����� ���������� ��������
							cout << "��������: ��� ���� " << it_col->second << ", � ����� �������� =" << endl;
							cout << "�������� ����� " << prod.symbols[i] << " � " << prod.symbols[i + 1] << endl;
							return false;
						}
					}
				}
				else {
					// ������ �� ������� � ����� ��������� �������
					precedence_table[prod.symbols[i]][prod.symbols[i + 1]] = '=';
					it_row = precedence_table.find(prod.symbols[i]);
					if (isNonterminal(prod.symbols[i + 1])) {
						//cout << "START ������ �����" << endl;
						set<string> visited;
						set<string> start = START(prod.symbols[i + 1], visited);
						for (const auto& syms : start) {
							// ������ ��� ��������� <�
							auto it_col = it_row->second.find(syms);
							if (it_col == it_row->second.end()) {
								// ������ ��� �� ��������� � ����� ���������
								precedence_table[prod.symbols[i]][syms] = '<';
							}
							else {
								if (it_col->second != '<') {
									// ������ ��� ���� ��������� � ��� �� '<' � ����� ���������� ��������
									cout << "��������: ��� ���� " << it_col->second << ", � ����� �������� <" << endl;
									cout << "�������� ����� " << prod.symbols[i] << " � " << syms << endl;
									return false;
								}
							}
						}
					}
					if (isNonterminal(prod.symbols[i])) {
						//cout << "END ������ �����" << endl;
						set<string> visited;
						set<string> end = END(prod.symbols[i], visited);
						for (const auto& syms : end) {
							//cout << "FIRST ������ �����" << endl;
							set<string> visited;
							set<string> first = FIRST(prod.symbols[i + 1], visited);
							for (const auto& syms_next : first) {
								// ������ ��� ��������� �>
								// ��������, ���������� �� ���� syms = END(prod.symbols[i]) � �������
								it_row = precedence_table.find(syms);
								if (it_row != precedence_table.end()) {
									// ������ ������� � ��������� �������
									auto it_col = it_row->second.find(syms_next);
									if (it_col == it_row->second.end()) {
										// ������ ��� �� ��������� � ����� ���������
										precedence_table[syms][syms_next] = '>';
									}
									else {
										if (it_col->second != '>') {
											// ������ ��� ���� ��������� � ��� �� '>' � ����� ���������� ��������
											cout << "��������: ��� ���� " << it_col->second << ", � ����� �������� >" << endl;
											cout << "�������� ����� " << syms << " � " << syms_next << endl;
											cout << "���������� �������� �� ����� ������������ END: ";
											for (const auto& elem : visited)
												cout << elem << " ";
											cout << endl;
											cout << "END(" << prod.symbols[i] << "): ";
											for (const auto& elem : end)
												cout << elem << " ";
											cout << endl;
											cout << "��������� ���� �� " << prod.symbols[i] << " � " << prod.symbols[i + 1];
											cout << endl;
											return false;
										}
									}
								}
								else {
									// ������ �� ������� � ����� ��������� �������
									precedence_table[syms][syms_next] = '>';
								}
							}

						}
					}
				}
			}
			set<string> visited;
			//cout << "END ������ �����" << endl;
			set<string> end = END(start_symbol, visited);
			for (const auto& syms : end) {
				it_row = precedence_table.find(syms);
				if (it_row != precedence_table.end()) {
					auto it_col = it_row->second.find("$");
					if (it_col == it_row->second.end()) {
						precedence_table[syms]["$"] = '>';
					}
					else {
						if (it_col->second != '>') {
							// ������ ��� ���� ��������� � ��� �� '>' � ����� ���������� ��������
							cout << "��������: ��� ���� " << it_col->second << ", � ����� �������� >" << endl;
							return false;
						}
					}
				}
				else {
					// ������ �� ������� � ����� ��������� �������
					precedence_table[syms]["$"] = '>';
				}
			}
		}
	}
	return true;
}

void SP_Analyzer::ifUseless() {
	for (const auto& nont : nonterminals)
		if (!isInSet(nont, terminals) && nont != "S") {
			cout << "������� ����������� ���������� " << nont << endl;
			flag_SP = false;
		}

}

void SP_Analyzer::ifLRecursion() {
	for (const auto& [lhs, rules] : productions) {
		for (const auto& prod : rules) {
			if (lhs == prod.symbols[0]) {
				cout << "������� ����� ��������: ";
				cout << lhs << " -> ";
				for (const auto& syms : prod.symbols) {
					cout << syms << ' ';
				}
				cout << endl;
				flag_SP = false;
			}
		}
	}
}

void SP_Analyzer::PrintPrecedenceTable() {
	table.open("precedence_table.txt");
	if (table.is_open()) {
		set<string> all_symbols;

		for (const auto& [row_symbol, row_map] : precedence_table) {
			all_symbols.insert(row_symbol);
			for (const auto& [col_symbol, _] : row_map) {
				all_symbols.insert(col_symbol);
			}
		}

		const int cell_width = 11;
		// �������� ��������� �������
		table << setw(cell_width) << " ";
		for (const auto& col : all_symbols) {
			if (col != "/*�����������*/")
				table << setw(cell_width) << col;
			else
				table << setw(cell_width) << "/*�*/";
		}
		table << endl;

		// �������� ������ �������
		for (const auto& row : all_symbols) {
			if (row != "/*�����������*/")
				table << setw(cell_width) << row;
			else
				table << setw(cell_width) << "/*�*/";

			for (const auto& col : all_symbols) {
				auto row_it = precedence_table.find(row);
				if (row_it != precedence_table.end()) {
					auto col_it = row_it->second.find(col);
					if (col_it != row_it->second.end()) {
						table << setw(cell_width) << col_it->second;
					}
					else {
						table << setw(cell_width) << '.';
					}
				}
				else {
					table << setw(cell_width) << '.';
				}
			}
			table << endl;
		}
	}
	else {
		table << "���� �� �������� :(" << endl;
	}
}