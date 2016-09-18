#pragma once
#include<iostream>
using namespace std;
class Stack {

private:	
	char x[200];
	int point = 0;

public:
	void push(char a);
	char pop();
	char top();
	void surf();
};

void Stack::push(char a) {
	point++;
	x[point] = a;
	//Stack::surf();
	//cout << Stack::top();
}

char Stack::pop() {
	if (point == 0)	return '\0';
	
	char ch = x[point];
	//cout << ch << endl;
	point--;
	return ch;
}

char Stack::top() {
	if (point == 0)	return '\0';
	return x[point];
}

void Stack::surf() {
	for (int i = 1; i < 6; i++)
		if (i <= point)	cout << x[i];
		else cout << ' ';
}



