#include "Script.h"
#include "Expression.h"

Expression *ScriptManager::BuildExpression(ifstream &in)
{
	ExprType type = (ExprType)0;
	in.read((char *)&type, 1);
	ExprOp op = (ExprOp)0;
	string varName = "";
	Expression *lhs = 0, *rhs = 0;
	switch(type)
	{
	case EXPR_LITERAL:
		int value;
		in.read((char *)&value, 4);
		return new ExprLiteral(value);

	case EXPR_VAR:
		getline(in, varName, '\0');
		return new ExprVar(varName);

	case EXPR_UNARY_OP:
		in.read((char *)&op, 1);
		return new ExprUnaryOp(op, BuildExpression(in));

	case EXPR_BINARY_OP:
		in.read((char *)&op, 1);
		lhs = BuildExpression(in);
		rhs = BuildExpression(in);
		return new ExprBinaryOp(op, lhs, rhs);

	default:
		string oops = "Invalid opcode: ";
		oops += (char)type;
		throw ExprParseException(oops);
	}
}