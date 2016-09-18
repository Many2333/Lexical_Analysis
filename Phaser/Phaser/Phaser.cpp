#include<iostream>
#include<string>
#include<stdio.h>
#include<fstream>
#include<string.h>
#include<iomanip>
#include<algorithm>
#include"Stack.h"
using namespace std;

ifstream grammer("Grammer1.txt");
ifstream source("Source.txt");

typedef struct {
	char	formula[100] = { '\0' };
}grammerElement;

grammerElement formulaSet[100];

int  grammarNum = 0;				//����ʽ��Ŀ
char terminals[200];				//�ս����
int terNum = 0;						//�ս���Ÿ���
char non_terminals[200];			//���ս����
int non_terNum = 0;					//���ս���Ÿ���
char leftStr[100];					//���в���ʽ��
char rightStr[100][100];			//���в���ʽ�Ҳ�
char FOLLOW[10][100] = { '\0' };	//������ʽ�󲿵�FOLLOW����
char FIRST[10][100] = { '\0' };		//���е������ŵ�FIRST����,����findSingleFIRST(i)�õ�
char SELECT[100][100] = { '\0' };	//����������ʽ��SELECT����
int list[200][200] = { 0 };			//������
int flag[100] = { 0 };
int isEps[10] = { 0 };

int _searchChar(char x[], char y) {
	if (strlen(x) == 0)	return -1;
	if (y == '@')	return -1;
	//if (y == '#')	return -1;
	for (int i = 0; i < (int)strlen(x); i++)
		if (x[i] == y)	return i;
	return -1;
}

bool _hasEps(char x[]) {
	for (int i = 0; i < (int)strlen(x); i++)
		if (x[i] == '@')	return true;
	return false;
}

bool _hasHash(char x[]) {
	for (int i = 0; i < (int)strlen(x); i++)
		if (x[i] == '#')	return true;
	return false;
}


void _clearFlag() {
	for (int i = 0; i < 100; i++)	flag[i] = 0;
}

int _getTail(char x[]) {
	for (int i = 0; i < 100; i++)
		if (x[i] == '\0')	return i;
	return 0;
}

int _getLength() {
	int length = 0;
	for (int i = 0; i < non_terNum; i++)
		for (int j = 0; FOLLOW[i][j] != '\0'; j++)
			length++;

	return length;
}

void chsCopy(char x[],char y[]) {
	for (int i = 0; y[i] != '\0'; i++) {
		if (_searchChar(x, y[i]) == -1 && y[i] != '@') {
			x[_getTail(x)] = y[i];
			//cout << y[i];
		}
	}
}

void getGrammer() {
	char buffer1[128] = { '\0' };
	grammer.getline(buffer1, sizeof(buffer1));
	for (int i = 0; buffer1[i] != '\0'; i++) {
		non_terminals[i] = buffer1[i];
		non_terminals[i+1] = '\0';
		non_terNum++;
	}

	char buffer2[128] = { '\0' };
	grammer.getline(buffer2, sizeof(buffer2));
	for (int i = 0; buffer2[i] != '\0'; i++) {
		terminals[i] = buffer2[i];
		terminals[i+1] = '\0';
		terNum++;
	}
	terminals[terNum] = '#';
	terminals[terNum + 1] = '\0';

	for (int i = 0; !grammer.eof(); i++) {
		grammer.getline(formulaSet[i].formula, sizeof(formulaSet[i].formula));
		grammarNum = i;
	}

	for (int i = 0; i < grammarNum; i++) {
		leftStr[i] = formulaSet[i].formula[0];
		for (int j = 3; formulaSet[i].formula[j] != '\0'; j++) {
			rightStr[i][j - 3] = formulaSet[i].formula[j];
			rightStr[i][j - 2] = '\0';
		}
	}
}

bool isDel(char ch) {
	int sum = 0;
	for (int i = 0; i < grammarNum; i++) {
		if (ch == leftStr[i]) {
			if (flag[i] != 1)	sum++;
		}
	}
	if (sum == 0)	return true;
	else return false;
}

void allEpsilion() {
	_clearFlag();
	
	for (int i = 0; i < grammarNum; i++) {
		for (int j = 0; rightStr[i][j] != '\0'; j++) {
			if (_searchChar(terminals, rightStr[i][j]) != -1) {
				flag[i] = 1;
				break;
			}
		}
	}
	for (int i = 0; i < non_terNum; i++) {
		if (isDel(non_terminals[i]))	isEps[i] = -1;
	}//ɾ�������Ҳ������ս���Ĳ���ʽ
	for (int i = 0; i < grammarNum; i++) {
		if (flag[i] == 1)	continue;
		if (rightStr[i][0] == '@'&&strlen(rightStr[i]) == 1) {
			isEps[_searchChar(non_terminals, leftStr[i])] = 1;
			for (int j = 0; j < grammarNum; j++)
				if (leftStr[j] == leftStr[i])	flag[j] = 1;
		}
	}//ɾ���Ҳ�Ϊ�յķ��ս�������в���ʽ
	for (int i = 0; ; i++) {
		//cout << i << endl;
		int a = 0;
		for (int j = 0; j < grammarNum; j++)
			if (flag[j] == 0)	a = 1;
		if (a == 1)	break;

		if (i == grammarNum)	i = 0;
		if (flag[i] == 1)	continue;
		int changes = 0;
		for (int j = 0; rightStr[i][j] != '\0'; j++) {
			int n = _searchChar(non_terminals, rightStr[i][j]);
			if (n != -1) {
				if (isEps[n] == -1)	flag[i] = 1;
				if (isDel(_searchChar(non_terminals,leftStr[i]))) {
					isEps[_searchChar(non_terminals,leftStr[i])] = -1;
					changes++;
				}
			}
		}
		flag[i] = 1;
		if (changes != 0)	continue;
		
		if (isEps[_searchChar(non_terminals, leftStr[i])] != 1) {
			isEps[_searchChar(non_terminals, leftStr[i])] = 1;
			changes++;
		}
	}
}//������п��Եõ��յķ���

void getFIRST() {
	getGrammer();
	allEpsilion();
	_clearFlag();

	while(1) {
		int length = 0;
		for (int i = 0; i < non_terNum; i++) {
			length += _getTail(FIRST[i]);

			if (isEps[i] == 1)	FIRST[i][0] = '@';
			for (int j = 0; j < grammarNum; j++) {
				if (flag[j] == 1 || rightStr[j][0] == '@')	continue;
				if (non_terminals[i] == leftStr[j]) {
					if (_searchChar(non_terminals, rightStr[j][0]) == -1) {
						if (_searchChar(FIRST[i], rightStr[j][0]) != -1)	continue;
						flag[j]++;
						FIRST[i][_getTail(FIRST[i])] = rightStr[j][0];
						//cout << non_terminals[i] << ':' << FIRST[i][_getTail(FIRST[i])-1] << endl;
						continue;
					}	
					else {
						int flag = 1;
						for (int k = 0; rightStr[j][k] != '\0' && _searchChar(non_terminals, rightStr[j][k] != -1); k++) {
							chsCopy(FIRST[i], FIRST[_searchChar(non_terminals, rightStr[j][k])]);
							if (isEps[_searchChar(non_terminals, rightStr[j][k])] != 1) {
								flag = 0;
								break;
							}
						}
						if (flag == 1 && !_hasEps(FIRST[i]))	FIRST[i][_getTail(FIRST[i])] = '@';
					}
				}
			}
		}
		for (int i = 0; i < non_terNum; i++)
			length -= _getTail(FIRST[i]);
		if (length == 0)	break;
	}
}

void getFOLLOW() {
	getFIRST();

	FOLLOW[0][0] = '#';
	
	while (1) {
		int length = _getLength();
			

		for (int i = 0; i < grammarNum; i++) {

			for (int j = 0; rightStr[i][j] != '\0'; j++) {
				if (_searchChar(non_terminals, rightStr[i][j]) != -1) {
					char tmp = rightStr[i][j+1];
					int m = _searchChar(non_terminals, rightStr[i][j]);
					if (tmp == '\0') {
						chsCopy(FOLLOW[m], FOLLOW[_searchChar(non_terminals, leftStr[i])]);
					}
					if (_searchChar(terminals, tmp) != -1) {
						char chs[] = { tmp };
						chsCopy(FOLLOW[m], chs);
					}
					if (_searchChar(non_terminals, tmp) != -1) {
						int n = _searchChar(non_terminals, tmp);
						chsCopy(FOLLOW[m], FIRST[n]);
						if(isEps[n]==1)	chsCopy(FOLLOW[m], FOLLOW[_searchChar(non_terminals, leftStr[i])]);
					}
				}
			}	
		}
		
		if (length == _getLength())	break;

	}
}

void getSELECT() {
	getFOLLOW();

	bool flag;
	for (int i = 0; i < grammarNum; i++) {
		flag = true;
		for (int j = 0; rightStr[i][j] != '\0'; j++) {
			char tmp = rightStr[i][j];
			if (tmp == '@')	continue;
			if (_searchChar(terminals, tmp) != -1)	flag = false;
			else if (isEps[_searchChar(non_terminals, tmp)] != 1)	flag = false;
			if (!flag)	break;
		}

		if (flag) {
			//cout << formulaSet[i].formula << ':';
			for (int j = 0; rightStr[i][j] != '\0'; j++) {
				if (_searchChar(non_terminals, rightStr[i][j]) != -1);
					chsCopy(SELECT[i], FIRST[_searchChar(non_terminals, rightStr[i][j])]);
			}
			chsCopy(SELECT[i],FOLLOW[_searchChar(non_terminals, leftStr[i])]);
		}
		else {
			if (_searchChar(non_terminals, rightStr[i][0]) == -1) {
				char ch[] = { rightStr[i][0],'\0' };
				chsCopy(SELECT[i], ch);
			}
			else {
				for (int j = 0; rightStr[i][j] != '\0'; j++) {
					int m = _searchChar(non_terminals, rightStr[i][j]);
					if (_searchChar(non_terminals, rightStr[i][j]) != -1)
						chsCopy(SELECT[i], FIRST[m]);
					if (isEps[m] != 1)	break;
				}
			}
		}
	}
}

void getList() {
	getSELECT();
	for (int i = 0; i < grammarNum; i++) {
		for (int j = 0; j<terNum+1; j++) {
			if (_searchChar(SELECT[i], terminals[j]) != -1)
				list[_searchChar(non_terminals, leftStr[i])][j] = i+1;
		}
	}
}

void analysis() {
	getList();
	char code[128] = { '\0' };
	source.getline(code,sizeof(code));
	code[_getTail(code)] = '#';
	
	cout << endl;
	cout << "���봮��" << code;
	cout << endl;
	cout << "����          ����ջ            �������봮          ���ò���ʽ" << endl;

	Stack stack;
	int p = 0, step = 1;
	stack.push('#');
	stack.push(non_terminals[0]);
	while (1) {
		cout << step << "             ";
		stack.surf();
		cout << "             ";
		for (int i = 0; i < 15; i++)
			if (p + i < strlen(code))	cout << code[p + i];
			else cout << ' ';
		if (_searchChar(terminals, stack.top()) != -1) {
			if (stack.top() == '#') {
				cout << "     ����" << endl;
				break;
			}
			else {
				if (stack.top() == code[p]) {
					step++;
					cout << "     ƥ�䣬��ջ" << endl;
					stack.pop();
					code[p] = ' ';
					p++;
				}
			}
		}
		else {
			int m = _searchChar(non_terminals, stack.top());
			int n = _searchChar(terminals, code[p]);
			int num = list[m][n] - 1;

			if (num == -1) {
				cout << "     ����"<< endl;
				break;
			}
			
			if (rightStr[num][0] == '@') {
				step++;
				stack.pop();
				cout << "     " << formulaSet[num].formula << endl;
			}	
			else {
				step++;
				stack.pop();
				
				cout << "     " << formulaSet[num].formula << endl;

				for (int i = strlen(rightStr[num]) - 1; i >= 0; i--)
					stack.push(rightStr[num][i]);
			}
		}
	}
}


int main() {

	analysis();
	int a;
	cin >> a;
	return 0;
}