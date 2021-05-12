#include"morphology.h"


//13个关键字关键字      1      2        3         4      5     6    7     8      9      10     11    12      13
const string k[13] = { "CONST","VAR","PROCEDURE","BEGIN","END","ODD","IF","THEN","CALL","WHILE","DO","READ","WRITE" };
//11个运算符  =14  :=15  +16   -17  *18  /19  #20  <21  <=22   >23  >=24
const string op[11] = { "=",":=","+","-","*","/","#","<","<=",">",">=" };
//5个分隔符  ;--25  ,--26   .--27  (--28  )--29
const string sep[5] = { ";",",",".","(",")" };
//标识符--0  常数--30  出错--(-1)

//全局变量
int row = 1, line = 1;//保存行列数
int t, p = 0;
string instring;//存输入
vector<Tuple> vec[35];//存储正确二元组
vector<Tuple> word;//存储整个单词
vector<Tuple> err;//存储错误
string tnum, tch;

Tuple::Tuple(string name, int type, int r, int l)
{

	this->name = name;
	this->type = type;
	this->row = r;
	this->line = l;
}

bool isletter(char x)
{
	return (x >= 'a' && x <= 'z' || x >= 'A' && x <= 'Z');
}

bool isdigit(char x)
{
	return x >= '0' && x <= '9';
}

bool iskey()//判断是否是关键字
{
	for (int i = 0; i < 13; i++)   //查关键字表
	{
		if (tch == k[i])
		{
			return i + 1;
		}
	}
	return false;
}

void analysis()//分析函数
{
	while (p < (int)instring.size() && (instring[p] == ' ' || instring[p] == '\n' || instring[p] == '\t' || instring[p] == '\r'))//不等于空格和回车则p++指向下一个
	{
		p++;
	}//执行完之后指向之后第一个不是空格的字符
	if (p >= (int)instring.size())return;

	if (isletter(instring[p]))AnalyIdent();
	else if (isdigit(instring[p])) AnalyConst();
	else symbol();
}

void AnalyIdent()//分析标识符或者关键字
{
	tch.clear();
	while (p < (int)instring.size() && (isletter(instring[p]) || isdigit(instring[p])))
	{
		tch.push_back(instring[p]);
		p++;
	}
	//p--;
	transform(tch.begin(), tch.end(), tch.begin(), ::toupper);
	t = iskey();
	if (t)
	{//是关键字
		vec[t].push_back(Tuple(tch, t, row, line));
		word.push_back(Tuple(tch, t, row, line));
	}
	else
	{//如果不是关键字
		if (tch.size() <= 10)
		{//是标识符
			vec[0].push_back(Tuple(tch, 0, row, line));
			word.push_back(Tuple(tch, 0, row, line));
		}
		else
		{//超出标识符的大小
			err.push_back(Tuple(tch, -1, row, line));
		}
	}
	line++;
}

void AnalyConst()
{
	tnum.clear();
	t = 30;
	while (p < (int)instring.size() && isdigit(instring[p]))
	{
		tnum.push_back(instring[p]);
		p++;
	}
	while (p < (int)instring.size() && (isletter(instring[p]) || isdigit(instring[p])))
	{//出错
		tnum.push_back(instring[p]);
		t = -1;
		p++;
	}
	//p--;
	transform(tnum.begin(), tnum.end(), tnum.begin(), ::toupper);
	if (t == -1)
	{
		err.push_back(Tuple(tnum, -1, row, line));
	}
	else
	{//是常数
		vec[30].push_back(Tuple(tnum, 30, row, line));
		word.push_back(Tuple(tnum, 30, row, line));
	}
	line++;
}

void symbol()
{
	//"=",":=","+","-","*","/","#","<","<=",">",">=" 
	//";",",",".","(",")"
	char c1 = instring[p];
	if (c1 == '=')
	{
		vec[14].push_back(Tuple("=", 14, row, line));
		word.push_back(Tuple("=", 14, row, line));
	}
	else if (c1 == ':')
	{
		p++;
		if (p < (int)instring.size())
		{
			char c2 = instring[p];
			if (c2 == '=') 
			{ 
				vec[15].push_back(Tuple(":=", 15, row, line)); 
				word.push_back(Tuple(":=", 15, row, line));
			}
			else 
			{ 
				p--; err.push_back(Tuple(":", -1, row, line)); 
			}
		}
		else 
		{ 
			p--; err.push_back(Tuple(":", -1, row, line)); 
		}
	}
	else if (c1 == '+')
	{
		vec[16].push_back(Tuple("+", 16, row, line));
		word.push_back(Tuple("+", 16, row, line));
	}
	else if (c1 == '-')
	{
		vec[17].push_back(Tuple("-", 17, row, line));
		word.push_back(Tuple("-", 17, row, line));
	}
	else if (c1 == '*')
	{
		vec[18].push_back(Tuple("*", 18, row, line));
		word.push_back(Tuple("*", 18, row, line));
	}
	else if (c1 == '/')
	{
		vec[19].push_back(Tuple("/", 19, row, line));
		word.push_back(Tuple("/", 19, row, line));
	}
	else if (c1 == '#')
	{
		vec[20].push_back(Tuple("#", 20, row, line));
		word.push_back(Tuple("#", 20, row, line));
	}
	else if (c1 == '<')
	{
		p++;
		if (p < (int)instring.size())
		{
			char c2 = instring[p];
			if (c2 == '=') 
			{ 
				vec[22].push_back(Tuple("<=", 22, row, line)); 
				word.push_back(Tuple("<=", 22, row, line));
			}
			else 
			{ 
				p--; vec[21].push_back(Tuple("<", 21, row, line)); 
				word.push_back(Tuple("<", 21, row, line));
			}
		}
		else 
		{ 
			p--; vec[21].push_back(Tuple("<", 21, row, line)); 
			word.push_back(Tuple("<", 21, row, line));
		}
	}
	else if (c1 == '>')
	{
		p++;
		if (p < (int)instring.size())
		{
			char c2 = instring[p];
			if (c2 == '=') 
			{ 
				vec[24].push_back(Tuple(">=", 24, row, line)); 
				word.push_back(Tuple(">=", 24, row, line));
			}
			else 
			{ 
				p--; vec[23].push_back(Tuple(">", 23, row, line)); 
				word.push_back(Tuple(">", 23, row, line));
			}
		}
		else 
		{ 
			p--; vec[23].push_back(Tuple(">", 23, row, line)); 
			word.push_back(Tuple(">", 23, row, line));
		}
	}
	else if (c1 == ';')
	{
		vec[25].push_back(Tuple(";", 25, row, line));
		word.push_back(Tuple(";", 25, row, line));
	}
	else if (c1 == ',')
	{
		vec[26].push_back(Tuple(",", 26, row, line));
		word.push_back(Tuple(",", 26, row, line));
	}
	else if (c1 == '.')
	{
		vec[27].push_back(Tuple(".", 27, row, line));
		word.push_back(Tuple(".", 27, row, line));
	}
	else if (c1 == '(')
	{
		vec[28].push_back(Tuple("(", 28, row, line));
		word.push_back(Tuple("(", 28, row, line));
	}
	else if (c1 == ')')
	{
		vec[29].push_back(Tuple(")", 29, row, line));
		word.push_back(Tuple(")", 29, row, line));
	}
	else
	{
		string temp;
		temp.push_back(c1);
		transform(temp.begin(), temp.end(), temp.begin(), ::toupper);
		err.push_back(Tuple(temp, -1, row, line));
	}
	p++;
	line++;
}

bool MoOutput()
{
	if (err.size() != 0)
	{
		cout << "Lexical Error" << endl;
		return false;
	}
	return true;
}
