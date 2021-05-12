#include"grammar.h"

string gram="";
bool sign = true;

//开始
void program_begin()
{
	if (word.size() <= 0)sign = false;
	p = 0;
	gram += "PROGRAM(";
	program();
	gram += ")";
}

//程序  <程序>→<分程序>.
void program()
{
	//p = 0;
	gram += "SUBPROG(";
	Subprogram(0);
	gram += ")";
	if (p < (int)word.size() && word[p].name == ".")
	{
		gram += ",."; p++;
		if (p < (int)word.size())sign = false;
	}
	else
	{
		sign = false; 
	}
}

//分程序  <分程序>→[<常量说明部分>][<变量说明部分>][<过程说明部分>]<语句>
void Subprogram(int flag)
{
	if (p >= (int)word.size())return;
	if (sign == false)return;
	int i = 0;

	//常量说明部分
	if (p < (int)word.size() && word[p].name == "CONST")
	{
		//if (i != 0)gram += ",";
		p++;
		gram += "CONSTANTDECLARE(";
		Constant_description();
		gram += ")";
		i++;
	}
	//变量说明部分
	if (p < (int)word.size() && word[p].name == "VAR")
	{
		if (i != 0)gram += ",";
		p++;
		gram += "VARIABLEDECLARE(";
		Variable_description();
		gram += ")";
		i++;
	}
	//过程说明部分
	if (p < (int)word.size() && word[p].name == "PROCEDURE")
	{
		if (i != 0)gram += ",";
		p++;
		gram += "PROCEDUREDECLARE(";
		Process_description(flag);
		gram += ")";
		//if (flag > 0)gram += ",";
		i++;
	}
	//语句
	if (i != 0)gram += ",";
	gram += "SENTENCE(";
	Statement();
	gram += ")";
}

//常量说明部分  <常量说明部分> → CONST<常量定义>{, <常量定义>};
void Constant_description()
{
	if (sign == false) return;

	gram += "CONST,";
	gram += "CONSTANTDEFINE(";
	constant_define();
	gram += ")";

	while (p < (int)word.size() && word[p].name == ",") 
	{
		gram += ",COMMA,"; p++; 
		gram += "CONSTANTDEFINE(";
		constant_define(); 
		gram += ")";
	}

	if (p < (int)word.size() && word[p].name == ";") { gram += ",;"; p++; }
	else { sign = false; return; }

}

//变量说明部分  <变量说明部分> → VAR<标识符>{, <标识符>};
void Variable_description()
{
	if (sign == false) return;

	gram += "VAR,";
	//标识符
	if (p < (int)word.size() && word[p].type == 0) { gram += word[p].name; p++; }
	else { sign = false; return; }

	while (p < (int)word.size() && word[p].name == ",")
	{
		gram += ",COMMA,"; 
		p++;
		if (p < (int)word.size() && word[p].type == 0) { gram += word[p].name; p++; }
		else { sign = false; return; }
	}

	if (p < (int)word.size() && word[p].name == ";") { gram += ",;"; p++; }
	else { sign = false; return; }
	
}

//过程说明部分  <过程说明部分> → <过程首部><分程序>; {<过程说明部分>}
void Process_description(int flag)
{
	if (sign == false)return;
	flag++;
	if (flag > 3) { sign = false; return; }

	gram += "PROCEDUREHEAD(";
	ProcedureHead();
	gram += ")"; gram += ",";

	gram += "SUBPROG(";
	Subprogram(flag);
	gram += ")";
	if (p < (int)word.size() && word[p].name == ";")
	{
		gram += ",;";
		p++;
		while (p < (int)word.size() && word[p].name == "PROCEDURE")
		{
			p++;
			gram += ",";
			gram += "PROCEDUREDECLARE(";
			Process_description(0);
			gram += ")";
		}
	}
	else { sign = false; return; }
	
}

//过程首部  <过程首部> → PROCEDURE <标识符>;
void ProcedureHead()
{
	gram += "PROCEDURE,";
	if (p < (int)word.size() && word[p].type == 0)
	{
		gram += word[p].name; gram += ","; p++;
		if (p < (int)word.size() && word[p].name == ";")
		{
			gram += ";"; p++;
		}
		else { sign = false; return; }
	}
	else { sign = false; return; }
}

//语句  <语句> → <赋值语句> | <条件语句> | <当型循环语句> | <过程调用语句> | <读语句> | <写语句> | <复合语句> | <空语句>
void Statement()
{
	if (sign == false) return;

	if (p < (int)word.size() && word[p].type == 0)
	{//赋值语句
		gram += "ASSIGNMENT(";
		assignment_sentence();
		gram += ")";
	}
	else if (p < (int)word.size() && word[p].name == "WHILE")
	{//当型循环语句
		gram += "WHILESENTENCE(";
		loop_sentence();
		gram += ")";
	}
	else if (p < (int)word.size() && word[p].name == "CALL")
	{//过程调用语句
		gram += "CALLSENTENCE(";
		process_sentence();
		gram += ")";
	}
	else if(p < (int)word.size() && word[p].name == "READ")
	{//读语句
		gram += "READSENTENCE(";
		read_sentence();
		gram += ")";
	}
	else if (p < (int)word.size() && word[p].name == "WRITE")
	{//写语句
		gram += "WRITESENTENCE(";
		write_sentence();
		gram += ")";
	}
	else if (p < (int)word.size() && word[p].name == "BEGIN")
	{//复合语句
		gram += "COMBINED(";
		compound_sentence();
		gram += ")";
	}
	else if (p < (int)word.size() && word[p].name == "IF")
	{//条件语句
		gram += "IFSENTENCE(";
		condition_sentence();
		gram += ")";
	}
	else
	{
		gram += "EMPTY";
	}

}

//常量定义  <常量定义> → < 标识符 >= <无符号整数>
void constant_define()
{
	if (sign == false) return; 

	if (p >= (int)word.size())return;

	if (word[p].type == 0)
	{//标识符
		gram += word[p].name; gram += ","; p++;
		//=
		if (p < (int)word.size() && word[p].name == "=")
		{
			gram += "="; gram += ","; p++;
			//无符号整数
			if (p < (int)word.size() && word[p].type == 30) { gram += word[p].name; p++; }
			else { sign = false; return; }

		}
		else { sign = false;return; }
	}
	else { sign = false; return; }
}

//是否是因子  <因子> → <标识符> | <无符号整数> | (<表达式>)
bool isfactor()
{
	if (p < (int)word.size() && (word[p].type == 0 || word[p].type == 30 || word[p].name == "("))return true;
	return false;
}

//加减运算符 < 加减运算符 > → + | -
bool isAddSub()
{
	if (p < (int)word.size() && (word[p].name == "+" || word[p].name == "-"))return true;
	return false;
}

//乘除运算符 <乘除运算符> → * |/
bool isMulDiv()
{
	if (p < (int)word.size() && (word[p].name == "*" || word[p].name == "/"))return true;
	return false;
}

//关系运算符 <关系运算符> → = | # | <|<=|>|>=
bool isRelaOp()
{
	if (p >= (int)word.size())return false;

	if (word[p].name == "=" || word[p].name == "#")return true;
	else if (word[p].name == "<" || word[p].name == "<=")return true;
	else if (word[p].name == ">" || word[p].name == ">=")return true;
	
	return false;

}

//表达式  <表达式> →[+| -]<项>{<加减运算符><项>}
void expression()
{
	if (sign == false) return;

	if (isAddSub()){ gram += word[p].name; gram += ","; p++; }

	if (isfactor())
	{
		gram += "ITEM(";
		term();
		gram += ")";

		while (isAddSub())
		{
			gram += ","; gram += word[p].name; gram += ","; p++;
			if (isfactor())
			{
				gram += "ITEM(";
				term();
				gram += ")";
			}
			else { sign = false; return; }
		}
	}
	else
	{
		sign = false;
		return;
	}
}

//因子 //<因子> → <标识符> | <无符号整数> | (<表达式>)
void factor()
{
	if (p < (int)word.size() && word[p].name == "(")
	{
		gram += "LP"; gram += ","; p++;
		gram += "EXPRESSION(";
		expression();
		gram += ")"; gram += ",";
		if (p < (int)word.size() && word[p].name == ")") { gram += "RP"; p++; }
		else { sign = false; return; }

	}
	else if (p < (int)word.size() && (word[p].type == 0 || word[p].type == 30))
	{ 
		gram += word[p].name; p++;
	}
	else { sign = false; return; }
}

//项 <项> → <因子>{<乘除运算符><因子>}
void term()
{
	if (sign == false) return;

	gram += "FACTOR(";
	factor();
	gram += ")";

	while (isMulDiv())
	{
		gram += ","; gram += word[p].name; gram += ","; p++;
		if (isfactor())
		{
			gram += "FACTOR(";
			factor();
			gram += ")";
		}
		else { sign = false; return; }
	}
}

//赋值语句  <赋值语句> → <标识符>: = <表达式>
void assignment_sentence()
{
	if (sign == false)return; 

	gram += word[p].name; gram += ","; p++;
	if (p < (int)word.size() && word[p].name == ":=")
	{
		gram += word[p].name; gram += ","; p++;
		gram += "EXPRESSION(";
		expression();
		gram += ")";
	}
	else { sign = false; return; }

}

//条件  <条件> → <表达式><关系运算符><表达式> | ODD<表达式>
void condition()
{
	if (p >= (int)word.size())return;
	if (sign == false) return; 

	if (p < (int)word.size() && word[p].name == "ODD")
	{
		gram += word[p].name, gram += ",", p++;
		gram += "EXPRESSION(";
		expression();
		gram += ")";
	}
	else
	{
		gram += "EXPRESSION(";
		expression();
		gram += ")";
		if (isRelaOp())
		{
			gram += ",", gram += word[p].name, gram += ",", p++;
			gram += "EXPRESSION(";
			expression();
			gram += ")";
		}
		else { sign = false; return; }
	}
	
}

//条件语句  <条件语句> → IF<条件>THEN<语句>
void condition_sentence()
{
	if (sign == false) return; 

	gram += "IF"; gram += ","; p++;
	gram += "CONDITION(";
	condition();
	gram += ")"; gram += ",";
	if (p < (int)word.size() && word[p].name == "THEN")
	{
		gram += "THEN"; gram += ","; p++;
		gram += "SENTENCE(";
		Statement();
		gram += ")";
	}
	else { sign = false;  return; }

}

//循环语句  <当型循环语句> → WHILE<条件>DO<语句>
void loop_sentence()
{
	if (sign == false) return; 

	gram += "WHILE"; gram += ","; p++;
	gram += "CONDITION(";
	condition();
	gram += ")"; gram += ",";
	if (p < (int)word.size() && word[p].name == "DO")
	{
		gram += "DO"; gram += ","; p++;
		gram += "SENTENCE(";
		Statement();
		gram += ")";
	}
	else { sign = false; return; }

}

//过程调用语句  <过程调用语句> → CALL<标识符>
void process_sentence()
{
	if (sign == false) return; 

	gram += "CALL"; gram += ","; p++;
	if (p < (int)word.size() && word[p].type == 0)
	{
		gram += word[p].name;
		p++;
	}
	else { sign = false; return; }
}

//复合语句  <复合语句> → BEGIN<语句>{ ; <语句> } END
void compound_sentence()
{
	if (sign == false) return; 

	gram += "BEGIN,"; p++;
	gram += "SENTENCE(";
	Statement();
	gram += ")";
	while (p < (int)word.size() && word[p].name == ";")
	{
		gram += ","; gram += word[p].name; gram += ","; p++;
		gram += "SENTENCE(";
		Statement();
		gram += ")";
	}

	if (p < (int)word.size() && word[p].name == "END")
	{
		gram += ","; gram += "END"; p++;
	}
	else { sign = false; return; }

}

//读语句 <读语句> → READ(<标识符>{, <标识符>})
void read_sentence()
{
	if (sign == false)return; 

	gram += "READ"; gram += ","; p++;
	if (p < (int)word.size() && word[p].name == "(")
	{
		gram += "LP"; gram += ","; p++;
		if (p < (int)word.size() && word[p].type == 0)
		{
			gram += word[p].name; p++;

			while (p < (int)word.size() && word[p].name == ",")
			{
				gram += ","; gram += "COMMA"; gram += ","; p++;
				if (p < (int)word.size() && word[p].type == 0)
				{
					gram += word[p].name; p++;
				}
				else { sign = false; return; }
			}
		}
		else { sign = false;  return; }

		if (p < (int)word.size() && word[p].name == ")") { gram += ",RP"; p++; }
		else { sign = false; return; }
	}
	else { sign = false; return; }

}

//写语句 < 写语句 > → WRITE(<标识符>{, <标识符>})
void write_sentence()
{
	if (sign == false) return; 

	gram += "WRITE"; gram += ","; p++;
	if (p < (int)word.size() && word[p].name == "(")
	{
		gram += "LP"; gram += ","; p++;
		if (p < (int)word.size() && word[p].type == 0)
		{
			gram += word[p].name; p++;

			while (p < (int)word.size() && word[p].name == ",")
			{
				gram += ","; gram += "COMMA"; gram += ",";
				if (p < (int)word.size() && word[p].type == 0)
				{
					gram += word[p].name; p++;
				}
				else { sign = false; return; }
			}
		}
		else { sign = false; return; }

		if (p < (int)word.size() && word[p].name == ")") { gram += ",RP"; p++; }
		else { sign = false; return; }
	}
	else { sign = false; return; }
}

//判断并输出
void GraOutput()
{
	//cout << gram << endl;
	if (sign == false)
	{
		cout << "Syntax Error" << endl;
	}
	else
	{
		cout << gram << endl;
	}
}