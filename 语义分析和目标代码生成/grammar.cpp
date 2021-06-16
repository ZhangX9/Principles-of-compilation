#include "grammar.h"
#include<map>



TableElement::TableElement()
{
	kind = -1;
	val = 0;
	level = 0;
	adr = 0;
	coderows = 0;
}
TableElement::TableElement(string name, int kind, int val, int level, int adr)
{
	this->name = name;
	this->kind = kind;
	this->val = val;
	this->level = level;
	this->adr = adr;
	coderows = 0;
}
void TableElement::clear()
{
	name.clear();
	kind = -1;
	val = 0;
	level = 0;
	adr = 0;
	coderows = 0;
}

vector<TableElement> table;//最多三层，建表
int LEVEL = 0;//LEVEL<3


code::code() { index = 0; }
code::code(string procname,string f,int l,int a)
{
	this->procname = procname;
	this->f = f;
	this->l = l;
	this->a = a;
	this->index = 0;
}
vector<code> object_code;//目标代码


WBCodeid::WBCodeid() { codeid = 0; }
WBCodeid::WBCodeid(string name, int codeid)
{
	this->name = name;
	this->codeid = codeid;
}
vector<WBCodeid> WriteBackCodeID;//记录需要回填的程序跳转地址，以便地址出现时方便回填

vector<node> Static_chain;//静态链
vector<WBCodeid> WriteBackChain;//记录需要回填的静态链

bool sign = true;//标记语法是否错误
bool sem_sign = true;//标记语义是否错误

//开始
void program_begin()
{
	if (word.size() <= 0)sign = false;
	p = 0;
	object_code.push_back(code("empty","goto", 0, 0));//object_code[0]不存储内容
	object_code.push_back(code("main","JMP", 0, 0));//object_code[1]存储main的开始地址
	program();
}

//程序  <程序>→<分程序>.
void program()
{
	Subprogram("main",0);
	if (p < (int)word.size() && word[p].name == ".")
	{
		p++;
		object_code.push_back(code("main","OPR", 0, 0));
		if (p < (int)word.size())sign = false;
	}
	else
	{
		sign = false;
	}
}

//分程序  <分程序>→[<常量说明部分>][<变量说明部分>][<过程说明部分>]<语句>
void Subprogram(string proc,int flag)
{
	if (p >= (int)word.size())return;
	if (sign == false)return;

	//常量说明部分
	if (p < (int)word.size() && word[p].name == "CONST")
	{
		p++;
		Constant_description();
	}
	//变量说明部分
	if (p < (int)word.size() && word[p].name == "VAR")
	{
		p++;
		Variable_description();
	}
	//过程说明部分
	if (p < (int)word.size() && word[p].name == "PROCEDURE")
	{
		p++;
		Process_description(proc,flag);
	}

	//开辟数据空间,首先寻找该过程部分之前声明的var变量的数量
	int varNumber = 0;
	int tableindex = 0;
	for (int i = (int)table.size() - 1; i >= 0; i--)
	{
		if (table[i].kind == VARIABLE && table[i].level == LEVEL)
		{
			varNumber++;
		}
		else if (table[i].kind==PROCEDURE && table[i].level == LEVEL - 1)
		{
			tableindex = i;
			break;
		}
	}
	object_code.push_back(code(proc, "INT", 0, 3 + varNumber));
	
	node nodetemp;
	nodetemp.name = proc;
	//填写外层
	if (LEVEL == 0)
	{
		nodetemp.father = -1;//main没有外层
	}
	else if (LEVEL - 1 == 0)
	{
		//外层为main
		WriteBackChain.push_back(WBCodeid("main", Static_chain.size()));
	}
	else if(LEVEL - 1 > 0)
	{
		//寻找其外层proc
		for (int j = tableindex - 1; j >= 0; j--)
		{
			//找到其外层的proc,存储当前程序在vector中的位置，等待回填
			if (table[j].kind == PROCEDURE && table[j].level == table[tableindex].level - 1)
			{
				WriteBackChain.push_back(WBCodeid(table[j].name, Static_chain.size()));
				break;
			}
		}
	}
	Static_chain.push_back(nodetemp);

	object_code[object_code.size() - 1].index = Static_chain.size() - 1;//proc在Static_chain中的位置
	//填写程序开始的位置
	int beginproc = object_code.size() - 1;//程序开始的位置
	if (proc == "main")object_code[1].a = beginproc;
	else
	{
		for (int i = (int)table.size() - 1; i >= 0; i--)
		{
			if (table[i].name==proc && table[i].kind == PROCEDURE)
			{
				table[i].coderows = beginproc;
				break;
			}
		}
	}
	//回填跳转到该程序的地址
	for (int i=0;i<(int)WriteBackCodeID.size();)
	{
		if (WriteBackCodeID[i].name == proc)
		{
			object_code[WriteBackCodeID[i].codeid].a = beginproc;
			WriteBackCodeID.erase(WriteBackCodeID.begin() + i);
		}
		else
		{
			i++;
		}
	}
	//回填外层
	for (int i = 0; i < (int)WriteBackChain.size();)
	{
		if (WriteBackChain[i].name == proc)
		{
			Static_chain[WriteBackChain[i].codeid].father = Static_chain.size() - 1;
			WriteBackChain.erase(WriteBackChain.begin() + i);
		}
		else i++;
	}

	//语句
	Statement(proc);
}

//常量说明部分  <常量说明部分> → CONST<常量定义>{, <常量定义>};
void Constant_description()
{
	if (sign == false) return;
	int adr = 0;
	constant_define(adr);

	while (p < (int)word.size() && word[p].name == ",")
	{
		p++;
		adr++;
		constant_define(adr);
	}

	if (p < (int)word.size() && word[p].name == ";") { p++; }
	else { sign = false; return; }

}

//常量定义  <常量定义> → <标识符> = <无符号整数>
void constant_define(int adr)
{
	if (sign == false) return;
	if (p >= (int)word.size())return;

	TableElement consttemp;
	consttemp.kind = CONSTANT; consttemp.adr = adr; consttemp.level = LEVEL;

	if (word[p].type == 0)//标识符
	{
		//判断该标识符是否已经存在
		int i = findvar(word[p].name);
		if (i != -1) { sem_sign = false; }//如果已经存在则语义错误

		consttemp.name = word[p].name;
		p++;
		if (p < (int)word.size() && word[p].name == "=")
		{
			p++;
			if (p < (int)word.size() && word[p].type == 30)//无符号整数
			{
				consttemp.val = transToNumber(word[p].name);//转为数字
				table.push_back(consttemp);
				p++;
			}
			else { sign = false; return; }

		}
		else { sign = false; return; }
	}
	else { sign = false; return; }
}

//变量说明部分  <变量说明部分> → VAR<标识符>{, <标识符>};
void Variable_description()
{
	if (sign == false) return;
	int adr = 3;
	TableElement vartemp;
	vartemp.kind = VARIABLE; vartemp.adr = adr; vartemp.level = LEVEL;
	//标识符
	if (p < (int)word.size() && word[p].type == 0) 
	{
		
		int i = findvar(word[p].name);
		if (i != -1 && table[i].kind == CONSTANT) { sem_sign = false; }//该标识符是否已经被常量定义

		vartemp.name = word[p].name; 
		p++; 
		table.push_back(vartemp); 
	}
	else { sign = false; return; }

	while (p < (int)word.size() && word[p].name == ",")
	{
		vartemp.clear();
		adr++;
		vartemp.kind = VARIABLE; vartemp.adr = adr; vartemp.level = LEVEL;
		p++;
		if (p < (int)word.size() && word[p].type == 0) 
		{ 
			int i = findvar(word[p].name);
			if (i != -1 && table[i].kind == CONSTANT) { sem_sign = false; }//该标识符是否已经被常量定义

			vartemp.name = word[p].name; 
			p++; 
			table.push_back(vartemp); 
		}
		else { sign = false; return; }
	}

	if (p < (int)word.size() && word[p].name == ";") { p++; }
	else { sign = false; return; }
}

//过程说明部分  <过程说明部分> → <过程首部><分程序>; {<过程说明部分>}
void Process_description(string proc,int flag)
{
	if (sign == false)return;
	//if (sem_sign == false)return;

	flag++;
	if (flag > 3) { sign = false; return; }

	ProcedureHead();
	if (sign == false)return;
	else { proc = table[table.size() - 1].name; }
	Subprogram(proc, flag);

	if (p < (int)word.size() && word[p].name == ";")
	{
		LEVEL--;
		p++;
		object_code.push_back(code(proc,"OPR", 0, 0));
		while (p < (int)word.size() && word[p].name == "PROCEDURE")
		{
			p++;
			Process_description(proc,0);
		}
	}
	else { sign = false; return; }

}

//过程首部  <过程首部> → PROCEDURE <标识符>;
void ProcedureHead()
{
	TableElement protemp;
	protemp.kind = PROCEDURE; protemp.level = LEVEL;
	if (p < (int)word.size() && word[p].type == 0)
	{
		//判断该标识符是否已被定义
		int i = findproc(word[p].name);
		if (i != -1)sem_sign = false;//重定义

		i = findvar(word[p].name);
		if (i != -1)sem_sign = false;//该标识符被定义为了变量或者常量

		protemp.name = word[p].name;
		p++;
		if (p < (int)word.size() && word[p].name == ";")
		{
			table.push_back(protemp);
			LEVEL++;
			p++;
		}
		else { sign = false; return; }
	}
	else { sign = false; return; }
}

//语句  <语句> → <赋值语句> | <条件语句> | <当型循环语句> | <过程调用语句> | <读语句> | <写语句> | <复合语句> | <空语句>
void Statement(string proc)
{
	if (sign == false) return;

	if (p < (int)word.size() && word[p].type == 0)
	{//赋值语句
		assignment_sentence(proc);
	}
	else if (p < (int)word.size() && word[p].name == "WHILE")
	{//当型循环语句
		loop_sentence(proc);
	}
	else if (p < (int)word.size() && word[p].name == "CALL")
	{//过程调用语句
		process_sentence(proc);
	}
	else if (p < (int)word.size() && word[p].name == "READ")
	{//读语句
		read_sentence(proc);
	}
	else if (p < (int)word.size() && word[p].name == "WRITE")
	{//写语句
		write_sentence(proc);
	}
	else if (p < (int)word.size() && word[p].name == "BEGIN")
	{//复合语句
		compound_sentence(proc);
	}
	else if (p < (int)word.size() && word[p].name == "IF")
	{//条件语句
		condition_sentence(proc);
	}
	else
	{
		//gram += "EMPTY";
	}

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
void expression(string proc)
{
	if (sign == false) return;

	int temp1 = -1;
	if (isAddSub()) { temp1 = p; p++; }
	if (isfactor())
	{
		term(proc);
		if (temp1 != -1)
		{
			if (word[temp1].name == "-")object_code.push_back(code(proc, "OPR", 0, 40));//负号
			else object_code.push_back(code(proc, "OPR", 0, 41));//正号
		}

		while (isAddSub())
		{
			int tempp = p;
			p++;
			if (isfactor())
			{
				term(proc);
				object_code.push_back(code(proc, "OPR", 0, word[tempp].type));//加减运算符
			}
			else { sign = false; return; }
		}
	}
	else { sign = false; return; }	
}

//因子 //<因子> → <标识符> | <无符号整数> | (<表达式>)
void factor(string proc)
{
	if (p < (int)word.size() && word[p].name == "(")
	{
		p++;
		expression(proc);
		if (p < (int)word.size() && word[p].name == ")") { p++; }
		else { sign = false; return; }

	}
	else if (p < (int)word.size() && (word[p].type == 0))
	{
		int i = findvar(word[p].name);
		if (i == -1)sem_sign = false;
		else if (table[i].kind == VARIABLE)
		{
			object_code.push_back(code(proc, "LOD", abs(LEVEL - table[i].level), table[i].adr));

		}
		else if (table[i].kind == CONSTANT)object_code.push_back(code(proc, "LIT", 0, table[i].val));
		else sem_sign = false;//kkkk
		p++;
	}
	else if (p < (int)word.size() && word[p].type == 30)
	{
		int val = transToNumber(word[p].name);
		object_code.push_back(code(proc, "LIT", 0, val));
		p++;
	}
	else { sign = false; return; }
}

//项 <项> → <因子>{<乘除运算符><因子>}
void term(string proc)
{
	if (sign == false) return;
	factor(proc);
	while (isMulDiv())
	{
		int temp1 = p;
		p++;
		if (isfactor())
		{
			factor(proc);
			object_code.push_back(code(proc, "OPR", 0, word[temp1].type));//乘除运算符
		}
		else { sign = false; return; }
	}
}

//赋值语句  <赋值语句> → <标识符> := <表达式>
void assignment_sentence(string proc)
{
	if (sign == false)return;
	
	//判断标识符是否存在
	int i = findvar(word[p].name);
	p++;
	if (p < (int)word.size() && word[p].name == ":=")
	{
		p++;
		expression(proc);
		if (i == -1||table[i].kind == CONSTANT)sem_sign = false;
		else { object_code.push_back(code(proc, "STO", abs(LEVEL - table[i].level), table[i].adr)); }//写回
	}
	else { sign = false; return; }

}

//条件  <条件> → <表达式><关系运算符><表达式> | ODD<表达式>
void condition(string proc)
{
	if (p >= (int)word.size())return;
	if (sign == false) return;

	if (p < (int)word.size() && word[p].name == "ODD")
	{
		int temp = p;//记录odd的索引
		p++;
		expression(proc);
		object_code.push_back(code(proc, "OPR", 0, word[temp].type));
	}
	else
	{
		expression(proc);
		if (isRelaOp())
		{
			int temp = p;//记录关系运算符的索引
			p++;
			expression(proc);
			object_code.push_back(code(proc, "OPR", 0, word[temp].type));
		}
		else { sign = false; return; }
	}

}

//条件语句  <条件语句> → IF<条件>THEN<语句>
void condition_sentence(string proc)
{
	if (sign == false) return;
	p++;
	condition(proc);
	object_code.push_back(code(proc, "JPC", 0, 0));//条件不满足时跳转
	int id = object_code.size() - 1;//记录该条指令的代码号
	if (p < (int)word.size() && word[p].name == "THEN")
	{
		p++;
		Statement(proc);
		object_code[id].a = object_code.size();//填写之前的跳转地址
	}
	else { sign = false;  return; }

}

//循环语句  <当型循环语句> → WHILE<条件>DO<语句>
void loop_sentence(string proc)
{
	if (sign == false) return;

	p++;
	int whileid = object_code.size();//while循环的入口
	condition(proc);
	object_code.push_back(code(proc, "JPC", 0, 0));//条件不满足时跳出循环
	int id = object_code.size() - 1;//记录该条指令的代码号
	if (p < (int)word.size() && word[p].name == "DO")
	{
		p++;
		Statement(proc);
		object_code.push_back(code(proc, "JMP", 0, whileid));//跳回循环入口
		object_code[id].a = object_code.size();//填写之前的跳转地址
	}
	else { sign = false; return; }

}

//过程调用语句  <过程调用语句> → CALL<标识符>
void process_sentence(string proc)
{
	if (sign == false) return;

	p++;
	if (p < (int)word.size() && word[p].type == 0)
	{
		int i = findproc(word[p].name);
		if (i == -1)sem_sign = false;//该过程调用不存在
		else//该调用的过程存在
		{
			int j = findInProc(word[p].name);
			if (j == -1)
			{
				//跳转地址暂填0
				object_code.push_back(code(proc, "CAL", abs(LEVEL - table[i].level), 0));
				//跳转地址不知道，等待回填
				WriteBackCodeID.push_back(WBCodeid(word[p].name, (int)object_code.size() - 1));
				
			}
			else object_code.push_back(code(proc, "CAL", abs(LEVEL - table[i].level), j));//跳转地址已知，直接填写
		}
		p++;
	}
	else { sign = false; return; }
}

//复合语句  <复合语句> → BEGIN<语句>{ ; <语句> } END
void compound_sentence(string proc)
{
	if (sign == false) return;

	p++;
	Statement(proc);
	while (p < (int)word.size() && word[p].name == ";")
	{
		p++;
		Statement(proc);
	}

	if (p < (int)word.size() && word[p].name == "END")
	{
		p++;
	}
	else { sign = false; return; }

}

//读语句 <读语句> → READ(<标识符>{, <标识符>})
void read_sentence(string proc)
{
	if (sign == false)return;

	int temp = p;//存储read的索引
	object_code.push_back(code(proc, "OPR", 0, word[p].type));//read操作

	p++;
	if (p < (int)word.size() && word[p].name == "(")
	{
		p++;
		if (p < (int)word.size() && word[p].type == 0)
		{
			//查找标识符是否存在
			int i = findvar(word[p].name);
			//不存在，常量不能被更改
			if (i == -1 || table[i].kind == CONSTANT)sem_sign = false;
			else 
			{ 
				//不是常量可以更改
				object_code.push_back(code(proc, "STO", abs(LEVEL - table[i].level), table[i].adr)); 
			}

			p++;
			while (p < (int)word.size() && word[p].name == ",")
			{
				p++;
				object_code.push_back(code(proc, "OPR", 0, word[temp].type));//read操作
				if (p < (int)word.size() && word[p].type == 0)
				{
					//查找标识符是否存在
					i = findvar(word[p].name);
					//不存在，常量不能被更改
					if (i == -1 || table[i].kind == CONSTANT)sem_sign = false;
					else 
					{ 
						//不是常量可以更改
						object_code.push_back(code(proc, "STO", abs(LEVEL - table[i].level), table[i].adr)); 
					}
					p++;
				}
				else { sign = false; return; }
			}
		}
		else { sign = false;  return; }

		if (p < (int)word.size() && word[p].name == ")") { p++; }
		else { sign = false; return; }
	}
	else { sign = false; return; }

}

//写语句 < 写语句 > → WRITE(<标识符>{, <标识符>})
void write_sentence(string proc)
{
	if (sign == false) return;
	int temp = p;

	p++;
	if (p < (int)word.size() && word[p].name == "(")
	{
		p++;
		if (p < (int)word.size() && word[p].type == 0)
		{
			//查找标识符是否存在
			int i = findvar(word[p].name);
			if (i == -1)sem_sign = false;//变量不存在
			else if (table[i].kind == VARIABLE)
			{
				object_code.push_back(code(proc, "LOD", abs(LEVEL - table[i].level), table[i].adr));//取数

			}
			else if (table[i].kind == CONSTANT)object_code.push_back(code(proc, "LIT", 0, table[i].val));//取常量
			else sem_sign = false;

			object_code.push_back(code(proc, "OPR", 0, word[temp].type));//write操作

			p++;
			while (p < (int)word.size() && word[p].name == ",")
			{
				p++;
				if (p < (int)word.size() && word[p].type == 0)
				{
					//查找标识符是否存在
					i = findvar(word[p].name);
					if (i == -1)sem_sign = false;//变量不存在
					else if (table[i].kind == VARIABLE)
					{
						object_code.push_back(code(proc, "LOD", abs(LEVEL - table[i].level), table[i].adr));//取变量

					}
					else if (table[i].kind == CONSTANT)object_code.push_back(code(proc, "LIT", 0, table[i].val));//取常量
					else sem_sign = false;

					object_code.push_back(code(proc, "OPR", 0, word[temp].type));//write操作

					p++;
				}
				else { sign = false; return; }
			}
		}
		else { sign = false; return; }

		if (p < (int)word.size() && word[p].name == ")") { p++; }
		else { sign = false; return; }
	}
	else { sign = false; return; }
}

//判断并输出
bool GraOutput()
{
	if (sign == false)
	{
		cout << "Syntax Error" << endl;
		return false;
	}

	if (sem_sign == false)
	{
		cout << "Semantic Error" << endl;
		return false;
	}

	ofstream file;
	file.open("staticchain.txt", ios_base::out);
	file << Static_chain.size() << endl;
	for (int i = 0; i < (int)Static_chain.size(); i++)
	{
		file << Static_chain[i].name << " " << Static_chain[i].father;
		file << endl;
	}
	file << object_code.size();
	file.close();

	ofstream ofs;
	ofs.open("code.txt", ios_base::out);
	for (int i = 1; i < (int)object_code.size(); i++)
	{
		ofs <<object_code[i].procname;
		ofs << " " << object_code[i].index;
		//ofs << " " << object_code[i].f;
		//ofs << " " << object_code[i].l;
		//ofs << " " << object_code[i].a;
		ofs << endl;
		cout << object_code[i].f << " " << object_code[i].l << " " << object_code[i].a << endl;
	}
	ofs.close();


	return true;

}

//字符串转为数字
int transToNumber(string num)
{
	int ans = 0;
	for (int i = 0; i < (int)num.size(); i++)
	{
		ans = ans * 10 + (num[i] - '0');
	}
	return ans;
}

//寻找标识符
int findvar(string varname)
{
	int level = LEVEL;

	for (int i = table.size() - 1; i >= 0; i--)
	{
		if (table[i].kind == PROCEDURE && table[i].level == level - 1)
		{
			level--;
		}
		else if (table[i].name == varname && table[i].level == level &&(table[i].kind==VARIABLE||table[i].kind==CONSTANT))
		{
			return i;
		}
	}
	return -1;
}

//寻找proc是否存在
int findproc(string procname)
{
	int level = LEVEL;
	for (int i = (int)table.size() - 1; i >= 0; i--)
	{
		if (table[i].name == procname && table[i].kind == PROCEDURE && (table[i].level == level || table[i].level == level - 1))
		{
			return i;
		}
		else if (table[i].name != procname && table[i].kind == PROCEDURE && table[i].level == level - 1)
		{
			level--;
		}
	}
	return -1;

}

//寻找proc入口地址
int findInProc(string proc)
{
	for (int i = object_code.size() - 1; i >= 1; i--)
	{
		if (object_code[i].f == "INT" && object_code[i].procname == proc)
		{
			return i;
		}
	}
	return -1;
}


