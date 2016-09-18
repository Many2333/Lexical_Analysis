#include<iostream>
#include<string>
#include<stdio.h>
#include<fstream>
#include<string.h>
#include<iomanip>
#include<algorithm>
using namespace std;

ifstream grammer("Grammer1.txt");
ifstream source("Source.txt");
string keywords[50];
char non_terminals[50], terminals[50], specials[50], limiters[50];
int keysNum, non_tersNum, tersNum, specsNum, limsNum;
int DFA[5][50];

void getKeywords() {
	char buffer[128] = { '\0' };
	grammer.getline(buffer, sizeof(buffer));

	for (int i = 0, j = 0; buffer[i] != '\0'; i++) {
		if (buffer[i] == ' ')	j++;
		else keywords[j].append(1, buffer[i]);
		keysNum = j + 1;
	}
}

void getNon_terminals() {
	non_tersNum = 0;
	char buffer[128] = { '\0' };
	grammer.getline(buffer, sizeof(buffer));

	for (int i = 0; buffer[i] != '\0'; i++) {
		non_terminals[i] = buffer[i];
		non_tersNum++;
	}
}

void getTerminals() {
	tersNum = 0;
	char buffer[128] = { '\0' };
	grammer.getline(buffer, sizeof(buffer));

	for (int i = 0; buffer[i] != '\0'; i++) {
		terminals[i] = buffer[i];
		tersNum++;
	}
}

void getSpecials() {
	specsNum = 0;
	char buffer[128] = { '\0' };
	grammer.getline(buffer, sizeof(buffer));

	for (int i = 0; buffer[i] != '\0'; i++) {
		specials[i] = buffer[i];
		specsNum++;
	}
}

void getLimiters() {
	limsNum = 0;
	char buffer[128] = { '\0' };
	grammer.getline(buffer, sizeof(buffer));

	for (int i = 0; buffer[i] != '\0'; i++) {
		limiters[i] = buffer[i];
		limsNum++;
	}
}

int _search(char x[], char y) {
	for (int i = 0; i < 50; i++)
		if (x[i] == y)	return i;
	return -1;
}

int _search(string x[], string y) {
	for (int i = 0; i < keysNum; i++)
		if (x[i] == y)	return i;
	return -1;
}

void DFAinit() {
	getKeywords();
	getNon_terminals();
	getTerminals();
	getSpecials();
	getLimiters();

	while (!grammer.eof()) {
		char buffer[128] = { '\0' };
		grammer.getline(buffer, sizeof(buffer));
		if (buffer[3] != '\0')
			DFA[_search(non_terminals, buffer[0])][_search(terminals, buffer[2])] = _search(non_terminals, buffer[3]);
		else
			DFA[_search(non_terminals, buffer[0])][_search(terminals, buffer[2])] = 5;
	}
}


int main() {
	DFAinit();
	int row = 0;

	while (!source.eof()) {
		char buffer[128] = { '\0' };
		source.getline(buffer, sizeof(buffer));
		cout << row + 1 << ':' << endl;

		int i = 0, state = 0;
		string str = "";
		while (buffer[i] != '\0') {
			//cout << buffer[i];
			if (buffer[i] == ' ') {
				i++;
				continue;
			}

			if (_search(specials, buffer[i]) != -1) {
				cout << buffer[i] << ' ' << "特殊符号" << endl;
				str = "";
				i++;
				continue;
			}

			if (_search(limiters, buffer[i]) != -1) {
				cout << buffer[i] << ' ' << "界符" << endl;
				str = "";
				i++;
				continue;
			}

			if (buffer[i] >= '0' && buffer[i] <= '9') {
				state = DFA[state][_search(terminals, buffer[i])];
				str.append(1, buffer[i]);
				i++;

				if (buffer[i] < '0' || buffer[i] > '9') {
					if (state == 5 || DFA[state][_search(terminals, '_')] == 5)	cout << str << ' ' << "常数" << endl;
					str = "";
					state = 0;
				}
				continue;
			}

			if (buffer[i] >= 'a' && buffer[i] <= 'z') {
				state = DFA[state][_search(terminals, buffer[i])];
				str.append(1, buffer[i]);
				i++;

				if (buffer[i] < 'a' || buffer[i] > 'z') {
					if (state == 5 || DFA[state][_search(terminals, '_')] == 5) {
						if (_search(keywords, str) != -1) {
							cout << str << ' ' << "关键字" << endl;
							str = "";
							state = 0;
						}
						else {
							cout << str << ' ' << "标识符" << endl;
							str = "";
							state = 0;
						}
					}
				}
				continue;
			}
		}

		cout << endl;
		row++;
	}

	int a;
	cin >> a;

	return 0;
}