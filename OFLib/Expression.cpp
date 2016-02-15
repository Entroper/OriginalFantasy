#include "Expression.h"
#include "Script.h"
#include <cmath>

ScriptManager *Expression::scriptMan = 0;

int ExprBinaryOp::Evaluate()
{
	switch (op)
	{
	case '+':
		return lhs->Evaluate() + rhs->Evaluate();
	case '-':
		return lhs->Evaluate() - rhs->Evaluate();
	case '*':
		return lhs->Evaluate() * rhs->Evaluate();
	case '/':
		return lhs->Evaluate() / rhs->Evaluate();
	case '%':
		return lhs->Evaluate() % rhs->Evaluate();
	case '^':
		return (int)floor(pow((double)lhs->Evaluate(), rhs->Evaluate()) + 0.5);

	case '&':
		return lhs->Evaluate() && rhs->Evaluate();
	case '|':
		return lhs->Evaluate() || rhs->Evaluate();
	case '@': //xor
		return (lhs->Evaluate() || rhs->Evaluate()) && !(lhs->Evaluate() && rhs->Evaluate());

	case '=':
		return lhs->Evaluate() == rhs->Evaluate();
	case '_':
		return lhs->Evaluate() != rhs->Evaluate();
	case '>':
		return lhs->Evaluate() > rhs->Evaluate();
	case '<':
		return lhs->Evaluate() < rhs->Evaluate();
	case '.':
		return lhs->Evaluate() >= rhs->Evaluate();
	case ',':
		return lhs->Evaluate() <= rhs->Evaluate();

	default:
		string oops = "Invalid operator: ";
		oops += (char)op;
		throw ExprEvalException(oops);
	}
}

int ExprUnaryOp::Evaluate()
{
	switch(op)
	{
	case 'I':
	case 'G':
		return 0;

	case '!':
		return !operand->Evaluate();

	default:
		string oops = "Invalid operator: ";
		oops += (char)op;
		throw ExprEvalException(oops);
	}
}



string ExprBinaryOp::Serialize()
{
	string ret = "";
	ret += (char)EXPR_BINARY_OP; //type identifier
	ret += (char)op; //the operator itself
	ret += lhs->Serialize();
	ret += rhs->Serialize();
	return ret;
}

string ExprUnaryOp::Serialize()
{
	string ret = "";
	ret += (char)EXPR_UNARY_OP; //type identifier
	ret += (char)op; //the operator itself
	ret += operand->Serialize();
	return ret;
}

string ExprVar::Serialize()
{
	string ret = "";
	ret += (char)EXPR_VAR; //type identifier
	ret += varName; //add the variable's name
	ret += '\0'; //null-terminated
	return ret;
}

string ExprLiteral::Serialize()
{
	string ret = "";
	ret += (char)EXPR_LITERAL; //type identifier;
	//append the 4-byte value to the string one character at a time
	ret += (char) (value & 0x000000FF);
	ret += (char)((value & 0x0000FF00) >> 8);
	ret += (char)((value & 0x00FF0000) >> 16);
	ret += (char)((value & 0xFF000000) >> 24);
	return ret;
}



string ExprBinaryOp::ToString()
{
	string ret = "";
	//Write the operator in a user-friendly format.
	switch(op)
	{
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case '^':
	case '=':
	case '>':
	case '<':
		ret += (char)op;
		break;

	case '&':
		ret += "AND";
		break;
	case '|':
		ret += "OR";
		break;
	case '@':
		ret += "XOR";
		break;

	case '_':
		ret += "!=";
		break;
	case '.':
		ret += ">=";
		break;
	case ',':
		ret += "<=";

	default:
		string oops = "Invalid operator: ";
		oops += (char)op;
		throw ExprEvalException(oops);
	}

	//Now concatenate the rest of the expression.
	ret += " ";
	ret += lhs->ToString();
	ret += " ";
	ret += rhs->ToString();
	return ret;
}

string ExprUnaryOp::ToString()
{
	string ret = "";
	switch(op)
	{
	case 'I':
		ret += "hasItem";
		break;
	case 'G':
		ret += "hasGold";
		break;

	case '!':
		ret += "NOT";
		break;

	default:
		string oops = "Invalid operator: ";
		oops += (char)op;
		throw ExprEvalException(oops);
	}

	ret += " ";
	ret += operand->ToString();
	return ret;
}