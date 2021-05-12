#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<iostream>
#include<fstream>
using namespace std;

//13个关键字关键字 
extern const string k[13];
//11个运算符 
extern const string op[11];
//5个分隔符
extern const string sep[5];
//标识符--0  常数--30  出错--(-1)

struct Tuple
{
	string name;
	int type;
	int row;
	int line;
	Tuple(string name, int type, int r, int l);
};

//全局变量
extern int row, line;//保存行列数
extern int t, p;
extern string instring;//存输入
extern vector<Tuple> vec[35];//存储正确二元组
extern vector<Tuple> err;//存储错误
extern vector<Tuple> word;
extern string tnum, tch;



//分析函数
bool isletter(char x);//判断是否是字符
bool isdigit(char x);//判断是否是数字
bool iskey();//判断是否是关键字
void analysis();//分析函数
void AnalyIdent();//分析标识符或者关键字
void AnalyConst();//分析常数
void symbol();//分析符号
bool MoOutput();//输出
