
#include "stdafx.h"
#include "mathscript.h"
#include "mathparser.h"
#include "mathinterpreter.h"
using namespace std;

void main()
{
	mathscript::cMathInterpreter interpreter;
	mathscript::cMathParser parser;

	if (!parser.Read("test.txt"))
	{
		cout << "error parse" << endl;
		return;
	}

	interpreter.Excute(parser.m_stmt);

	cout << "a = " << script::g_symbols["a"].fVal << endl;
	cout << "b = " << script::g_symbols["b"].fVal << endl;


}
