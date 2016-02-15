#ifndef SCRIPT_H
#define SCRIPT_H
#include <string>
#include <fstream>
using namespace std;

class Expression;

class ScriptManager
{
public:
	int GetVarValue(string varName) { return 0; }
	Expression *BuildExpression(ifstream &in);
};

class ExprParseException
{
public:
	string error;
	ExprParseException(string error) { this->error = error; }
};

#endif