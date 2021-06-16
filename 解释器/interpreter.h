#pragma once
#include<iostream>
#include<vector>
#include<stack>
#include<iomanip>
#include<fstream>
using namespace std;

//13个关键字关键字 
extern const string k[13];
//11个运算符 
extern const string op[11];
//5个分隔符
extern const string sep[5];
//标识符--0  常数--30  出错--(-1)

struct outfather
{
	string name;
	int father;
};

struct node
{
	string name;//当前程序的名字
	int index;//父亲程序
	int StaOuter;//静态外层
	int DymOuter;//动态外层
	int backdis;//返回地址
	vector<int> vec;//存储变量
	node() {}
};

//目标代码结构体
struct code
{
	string procname;
	string f;
	int l;
	int a;
	int index;
	code();
	code(string f, int l, int a);
};

extern vector<outfather> Static_chain;
extern vector<code> object_code;//目标代码
extern int p;


void getNodeFromFile();//从文件中得到各个proc的数据空间
void getCodeFromFile(int size);//从文件中读取目标代码


void interpreter();//解释执行代码