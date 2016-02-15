/*****************************************************************************
* Original Fantasy
* Author: Michael Bennett
* Copyright 2007
*
* Expression.h
* Defines classes for parsing and evaluating numerical and logical
* expressions in the event scripting system.  Extensive documentation on the
* file format is contained in Scripts.txt.
*****************************************************************************/

#ifndef EXPRESSION_H
#define EXPRESSION_H
#include "Script.h"
#include <string>
using namespace std;

//class ScriptManager;

/* Expressions are stored as tree structures, with the leaves containing
   either constant values or variables, and parent nodes containing operators.
   Expression is the abstract base class to describe a node in the tree, and
   the derived classes implement evaluators for each node type. */
class Expression
{
protected:
	static ScriptManager *scriptMan;

public:
	virtual int Evaluate() = 0; //evaluate the expression and return the result
	virtual string Serialize() = 0; //put the expression in the format described in Scripts.txt
	virtual string ToString() = 0; //write the expression in prefix notation

	static void Initialize(ScriptManager *scriptMan) { Expression::scriptMan = scriptMan; }
};

enum ExprOp
{
	//Arithmetic operators.
	EXPR_OP_PLUS = '+',
	EXPR_OP_MINUS = '-',
	EXPR_OP_MULT = '*',
	EXPR_OP_DIV = '/',
	EXPR_OP_MOD = '%',
	EXPR_OP_EXP = '^',

	//Logical operators.
	EXPR_OP_AND = '&',
	EXPR_OP_OR = '|',
	EXPR_OP_NOT = '!', //this one is unary
	EXPR_OP_XOR = '@',

	//Equality-inequality operators.
	EXPR_OP_EQ = '=',
	EXPR_OP_NEQ = '_',
	EXPR_OP_GT = '>',
	EXPR_OP_LT = '<',
	EXPR_OP_GE = '.',
	EXPR_OP_LE = ',',

	//Game-specific operators.
	EXPR_OP_ITEM = 'I',
	EXPR_OP_GOLD = 'G'
};

enum ExprType
{
	EXPR_LITERAL = 0,
	EXPR_VAR = 1,
	EXPR_BINARY_OP = 2,
	EXPR_UNARY_OP = 3
};

class ExprBinaryOp : public Expression
{
private:
	ExprOp op;
	Expression *lhs, *rhs;

public:
	ExprBinaryOp(ExprOp op, Expression *lhs, Expression *rhs) {
		this->op = op; this->lhs = lhs; this->rhs = rhs; }
	~ExprBinaryOp() { delete lhs; delete rhs; }

	virtual int Evaluate();
	virtual string Serialize();
	virtual string ToString();
};

class ExprUnaryOp : public Expression
{
private:
	ExprOp op;
	Expression *operand;

public:
	ExprUnaryOp(ExprOp op, Expression *operand) {
		this->op = op; this->operand = operand; }
	~ExprUnaryOp() { delete operand; }

	virtual int Evaluate();
	virtual string Serialize();
	virtual string ToString();
};

class ExprVar : public Expression
{
private:
	string varName;

public:
	ExprVar(string varName) { this->varName = varName; }

	virtual int Evaluate() { return scriptMan->GetVarValue(varName); }
	virtual string Serialize();
	virtual string ToString() { return varName; }
};

class ExprLiteral : public Expression
{
private:
	int value;

public:
	ExprLiteral(int value) { this->value = value; }

	virtual int Evaluate() { return value; }
	virtual string Serialize();
	virtual string ToString() { char buff[12]; return itoa(value, buff, 10); }
};



class ExprEvalException
{
public:
	string error;
	ExprEvalException(string error) { this->error = error; }
};



#endif