#include"interpreter.h"

//13个关键字关键字      1      2        3         4      5     6    7     8      9      10     11    12      13
const string k[13] = { "CONST","VAR","PROCEDURE","BEGIN","END","ODD","IF","THEN","CALL","WHILE","DO","READ","WRITE" };
//11个运算符  =14  :=15  +16   -17  *18  /19  #20  <21  <=22   >23  >=24
const string op[11] = { "=",":=","+","-","*","/","#","<","<=",">",">=" };
//5个分隔符  ;--25  ,--26   .--27  (--28  )--29
const string sep[5] = { ";",",",".","(",")" };
//标识符--0  常数--30  出错--(-1)
//负号--40   正号---41

code::code() { }
code::code(string f, int l, int a)
{
	this->f = f;
	this->l = l;
	this->a = a;
}

vector<outfather> Static_chain;
vector<code> object_code;//目标代码
int p;//取指令


//从文件中得到各个proc的数据空间
void getNodeFromFile()
{
	ifstream file;
	//file.open("D:\\vs\\vs实验代码\\编译原理\\Pl0编译原理实验\\Pl0编译原理实验\\staticchain.txt", ios_base::in);
	file.open("staticchain.txt", ios_base::in);
	outfather empty;
	int tsize;
	file >> tsize;
	for(int i = 0; i < tsize; i++)
	{
		Static_chain.push_back(empty);
		file >> Static_chain[i].name >> Static_chain[i].father;
	}
	file >> tsize;
	file.close();

	getCodeFromFile(tsize);
}

//从文件中读取目标代码
void getCodeFromFile(int size)
{
	ifstream in;
	//in.open("D:\\vs\\vs实验代码\\编译原理\\Pl0编译原理实验\\Pl0编译原理实验\\code.txt", ios_base::in);
	in.open("code.txt", ios_base::in);
	code tempcode;
	object_code.push_back(tempcode);
	for(int i = 1; i < size; i++)
	{
		object_code.push_back(tempcode);
		in >> object_code[i].procname >> object_code[i].index;
		//in >> object_code[i].f >> object_code[i].l >> object_code[i].a;
	}
	in.close();

	ifstream in1;
	in1.open("program.code", ios_base::in);
	for (int i = 1; i < size; i++)
	{
		in1 >> object_code[i].f >> object_code[i].l >> object_code[i].a;
	}
	in1.close();


}




//解释执行代码
void interpreter()
{
	vector<node> run;
	stack<int> cal;//运行时数据栈，用来进行计算
	p = 1;
	while (p < (int)object_code.size())
	{
		if (object_code[p].f == "JMP")
		{
			//跳转至main开始的地方，先将main放入栈中
			if (p == 1)
			{
				int tp = object_code[p].a;
				node temp;
				temp.name = object_code[tp].procname;//记录当前程序的名字
				temp.index = object_code[tp].index;//标记main在static_chain中的位置
				temp.StaOuter = -1;//main的静态外层不存在
				temp.DymOuter = -1;//main的动态外层不存在
				temp.backdis = -1;//main无返回地址
				run.push_back(temp);//入栈
			}
			p = object_code[p].a;
		}
		else if (object_code[p].f == "INT")//为当前程序开辟数据区
		{
			for (int i = 0; i < object_code[p].a - 3; i++)
			{//开辟数据区
				run[run.size() - 1].vec.push_back(0);
			}
			p++;

		}
		else if (object_code[p].f == "JPC")//栈顶为0则跳转至a
		{
			if (cal.top() == 0)p = object_code[p].a;
			else p++;
			cal.pop();
		}
		else if (object_code[p].f == "CAL")
		{
			int tp = object_code[p].a;//cal的proc开始的位置
			node temp;
			temp.name = object_code[tp].procname;//记录当前程序的名字
			temp.index = object_code[tp].index;//标记proc在static_chain中的位置，方便寻找其外层
			temp.DymOuter = run.size() - 1;//记录其动态外层，一定是当前运行栈栈顶的proc
			temp.backdis = p + 1;//返回地址
			//在运行栈中寻找其静态外层
			if (Static_chain[temp.index].father == -1)//不存在外层
			{
				temp.StaOuter = -1;
			}
			else
			{
				for (int i = (int)run.size() - 1; i >= 0; i--)
				{
					//run[i]在static_chain中的位置等于当前程序外层在static_chain中的位置
					if (run[i].index == Static_chain[temp.index].father)
					{
						temp.StaOuter = i;
						break;
					}
				}
			}
			run.push_back(temp);//入栈
			p = object_code[p].a;//跳转
		}
		else if (object_code[p].f == "LOD")
		{
			int temp = run.size() - 1;//层差为0，在当前层寻找数据
			if (object_code[p].l >= 1)
			{
				for (int i = 0; i < object_code[p].l; i++)
				{
					temp = run[temp].StaOuter;
				}
			}
			cal.push(run[temp].vec[object_code[p].a - 3]);
			p++;//指令向下移动
		}
		else if (object_code[p].f == "STO")
		{
			int temp = run.size() - 1;//层差为0，在当前层寻找数据
			if (object_code[p].l >= 1)
			{
				for (int i = 0; i < object_code[p].l; i++)
				{
					temp = run[temp].StaOuter;
				}
			}
			run[temp].vec[object_code[p].a - 3] = cal.top();
			cal.pop();
			p++;//指令向下移动
		}
		else if (object_code[p].f == "LIT")//取常量
		{
			cal.push(object_code[p].a);
			p++;
		}
		else if (object_code[p].f == "OPR")
		{
			switch (object_code[p].a)
			{
				case 0://退出
				{
					if (run[run.size() - 1].backdis != -1)
					{
						p = run[run.size() - 1].backdis;
						p--;//switch结束时会有p++
					}
					run.erase(run.begin() + run.size() - 1);//从栈中删除
					break;
				}
				case 6://判断是否是奇数
				{
					int t1 = cal.top();
					cal.pop();
					if (t1 % 2 == 1)cal.push(1);
					else cal.push(0);
					break;
				}
				case 12://read
				{
					int t1;
					cin >> t1;
					cal.push(t1);
					break;
				}
				case 13://write
				{
					cout << cal.top() << endl;
					cal.pop();
					break;
				}
				case 14://等于
				{
					int t1 = cal.top();
					cal.pop();
					int t2 = cal.top();
					cal.pop();
					if (t2 == t1)cal.push(1);
					else cal.push(0);
					break;
				}
				case 16://加
				{
					int t1 = cal.top();
					cal.pop();
					int t2 = cal.top();
					cal.pop();
					cal.push(t2 + t1);
					break;
				}
				case 17://减
				{
					int t1 = cal.top();
					cal.pop();
					int t2 = cal.top();
					cal.pop();
					cal.push(t2 - t1);
					break;
				}
				case 18://乘
				{
					int t1 = cal.top();
					cal.pop();
					int t2 = cal.top();
					cal.pop();
					cal.push(t2 * t1);
					break;
				}
				case 19://除
				{
					int t1 = cal.top();
					cal.pop();
					int t2 = cal.top();
					cal.pop();
					//除数不能为0
					if (t1 == 0) { exit(-1); }
					cal.push(t2 / t1);
					break;
				}
				case 20://不等于
				{
					int t1 = cal.top();
					cal.pop();
					int t2 = cal.top();
					cal.pop();
					if (t2 != t1)cal.push(1);
					else cal.push(0);
					break;

				}
				case 21://小于
				{
					int t1 = cal.top();
					cal.pop();
					int t2 = cal.top();
					cal.pop();
					if (t2 < t1)cal.push(1);
					else cal.push(0);
					break;
				}
				case 22://小于等于
				{
					int t1 = cal.top();
					cal.pop();
					int t2 = cal.top();
					cal.pop();
					if (t2 <= t1)cal.push(1);
					else cal.push(0);
					break;
				}
				case 23://大于
				{
					int t1 = cal.top();
					cal.pop();
					int t2 = cal.top();
					cal.pop();
					if (t2 > t1)cal.push(1);
					else cal.push(0);
					break;
				}
				case 24://大于等于
				{
					int t1 = cal.top();
					cal.pop();
					int t2 = cal.top();
					cal.pop();
					if (t2 >= t1)cal.push(1);
					else cal.push(0);
					break;
				}
				case 40://负号
				{
					int t1 = cal.top();
					cal.pop();
					cal.push(-t1);
					break;
				}
				case 41://正号
					break;
				default:
					break;
			}
			p++;
		}

	}
}