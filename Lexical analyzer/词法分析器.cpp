#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
using namespace std;

//13���ؼ��ֹؼ���      1      2        3         4      5     6    7     8      9      10     11    12      13
const string k[] = { "CONST","VAR","PROCEDURE","BEGIN","END","ODD","IF","THEN","CALL","WHILE","DO","READ","WRITE" };
//11�������  =14  :=15  +16   -17  *18  /19  #20  <21  <=22   >23  >=24
const char* op[] = { "=",":=","+","-","*","/","#","<","<=",">",">=" };
//5���ָ���  ;--25  ,--26   .--27  (--28  )--29
const char* sep[] = { ";",",",".","(",")" };
//��ʶ��--0  ����--30  ����--(-1)
struct Tuple
{
	string name;
	int type;
	int row;
	int line;
	Tuple(string name, int type, int r, int l)
	{
		this->name = name;
		this->type = type;
		this->row = r;
		this->line = l;
	}
};



//ȫ�ֱ���
int row = 1, line = 1;//����������
int t, p = 0;
string instring;//������
vector<Tuple> vec;//�洢��ȷ��Ԫ��
vector<Tuple> err;//�洢����
string tnum, tch;

//��������
bool isletter(char x);//�ж��Ƿ����ַ�
bool isdigit(char x);//�ж��Ƿ�������
bool iskey();//�ж��Ƿ��ǹؼ���
void analysis();//��������
void AnalyIdent();//������ʶ�����߹ؼ���
void AnalyConst();//��������
void symbol();//��������
void output();//���


int main()
{
	while (getline(cin,instring))
	{
		p = 0;
		while (p < instring.size())
		{
			analysis();
		}
		row++;
		line = 1;
		p = 0;
		instring.clear();
	}
	output();
	return 0;
}

bool isletter(char x)
{
	return (x >= 'a' && x <= 'z' || x >= 'A' && x <= 'Z');
}

bool isdigit(char x)
{
	return x >= '0' && x <= '9';
}

bool iskey()//�ж��Ƿ��ǹؼ���
{
	for (int i = 0; i < 13; i++)   //��ؼ��ֱ�
	{
		if (tch == k[i])
		{
			return i + 1;
		}
	}
	return false;
}

void analysis()//��������
{
	while (p < instring.size() && (instring[p] == ' ' || instring[p] == '\n'|| instring[p] == '\t'|| instring[p] == '\r'))//�����ڿո�ͻس���p++ָ����һ��
	{
		p++;
	}//ִ����֮��ָ��֮���һ�����ǿո���ַ�
	if (p >= instring.size())return;

	if (isletter(instring[p]))
	{
		AnalyIdent();
	}
	else if (isdigit(instring[p]))
	{
		AnalyConst();
	}
	else
	{
		symbol();
	}
}

void AnalyIdent()//������ʶ�����߹ؼ���
{
	tch.clear();
	while (p < instring.size() && (isletter(instring[p]) || isdigit(instring[p])))
	{
		tch.push_back(instring[p]);
		p++;
	}
	//p--;
	transform(tch.begin(), tch.end(), tch.begin(), ::toupper);
	t = iskey();
	if (t)
	{//�ǹؼ���
		vec.push_back(Tuple(tch, t, row, line));
	}
	else
	{//������ǹؼ���
		if (tch.size() <= 10)
		{//�Ǳ�ʶ��
			vec.push_back(Tuple(tch, 0, row, line));
		}
		else
		{//������ʶ���Ĵ�С
			err.push_back(Tuple(tch, -1, row, line));
		}
	}
	line++;
}

void AnalyConst()
{
	tnum.clear();
	t = 30;
	while (p < instring.size() && isdigit(instring[p]))
	{
		tnum.push_back(instring[p]);
		p++;
	}
	while (p < instring.size() && (isletter(instring[p]) || isdigit(instring[p])))
	{//����
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
	{//�ǳ���
		vec.push_back(Tuple(tnum, 30, row, line));
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
		vec.push_back(Tuple("=", 14, row, line));
	}
	else if (c1 == ':')
	{
		p++;
		if (p < instring.size())
		{
			char c2 = instring[p];
			if (c2 == '=')vec.push_back(Tuple(":=", 15, row, line));
			else { p--; err.push_back(Tuple(":", -1, row, line)); }
		}
		else { p--; err.push_back(Tuple(":", -1, row, line)); }
	}
	else if (c1 == '+')
	{
		vec.push_back(Tuple("+", 16, row, line));
	}
	else if (c1 == '-')
	{
		vec.push_back(Tuple("-", 17, row, line));
	}
	else if (c1 == '*')
	{
		vec.push_back(Tuple("*", 18, row, line));
	}
	else if (c1 == '/')
	{
		vec.push_back(Tuple("/", 19, row, line));
	}
	else if (c1 == '#')
	{
		vec.push_back(Tuple("#", 20, row, line));
	}
	else if (c1 == '<')
	{
		p++;
		if (p < instring.size())
		{
			char c2 = instring[p];
			if (c2 == '=')vec.push_back(Tuple("<=", 22, row, line));
			else { p--; vec.push_back(Tuple("<", 21, row, line)); }
		}
		else { p--; vec.push_back(Tuple("<", 21, row, line)); }
	}
	else if (c1 == '>')
	{
		p++;
		if (p < instring.size())
		{
			char c2 = instring[p];
			if (c2 == '=')vec.push_back(Tuple(">=", 24, row, line));
			else { p--; vec.push_back(Tuple(">", 23, row, line)); }
		}
		else { p--; vec.push_back(Tuple(">", 23, row, line)); }
	}
	else if (c1 == ';')
	{
		vec.push_back(Tuple(";", 25, row, line));
	}
	else if (c1 == ',')
	{
		vec.push_back(Tuple(",", 26, row, line));
	}
	else if (c1 == '.')
	{
		vec.push_back(Tuple(".", 27, row, line));
	}
	else if (c1 == '(')
	{
		vec.push_back(Tuple("(", 28, row, line));
	}
	else if (c1 == ')')
	{
		vec.push_back(Tuple(")", 29, row, line));
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

void output()
{
	if (err.size() != 0)
	{
		cout << "Lexical Error" << endl;
	}
	else
	{

		int n = vec.size();
		for (int i = 0; i < n; i++)
		{
			if (vec[i].type >= 1 && vec[i].type <= 29)
			{
				cout << vec[i].name << endl;
			}
			else if (vec[i].type == 0)
			{
				cout << "IDENTIFIER " << vec[i].name << endl;
			}
			else if (vec[i].type == 30)
			{

				string name;
				int j = 0;
				int tsize = vec[i].name.size();
				for (; j < tsize;)
				{
					if (vec[i].name[j] == '0')j++;
					else break;
				}
				name = vec[i].name.substr(j, vec[i].name.size() - j);
				if (name.size() == 0)
				{
					name = "0";
				}
				vec[i].name = name;

				cout << "NUMBER " << name << endl;
			}
		}
	}
}

