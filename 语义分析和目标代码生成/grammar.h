#pragma once
#include "morphology.h"
#include<stack>
#include<iomanip>

//类型
#define VARIABLE "VARIABLE"
#define CONSTANT "CONSTANT"
#define PROCEDURE "PROCEDURE"

//表格元素的格式
struct TableElement
{
	string name;//名字
	string kind;
	int val;//变量值
	int level;//变量所在层数
	int adr;//变量相对地址
	int coderows;//函数起始位置
	TableElement();
	TableElement(string name, int kind, int val, int level, int adr);
	void clear();
};
extern vector<TableElement> table;//建表
extern int LEVEL;//LEVEL<3

//目标代码结构体
struct code
{
	string procname;
	string f;
	int l;
	int a;
	int index;//存储程序在static_chain中的位置
	code();
	code(string procname, string f, int l, int a);
};

extern vector<code> object_code;//目标代码

//用来存储需要回填的部分
struct WBCodeid
{
	string name;
	int codeid;
	WBCodeid();
	WBCodeid(string name, int codeid);
};
extern vector<WBCodeid> WriteBackCodeID;//记录需要回填的程序跳转地址，以便地址出现时方便回填

//用来存储程序的外层
struct node
{
	string name;//当前程序的名字
	int father;//外层程序
	node(){}
};
extern vector<node> Static_chain;
extern vector<WBCodeid> WriteBackChain;//记录需要回填外层的程序


void program_begin();//开始
void program();//程序
void Subprogram(string proc,int flag);//分程序
void Constant_description();//常量说明部分
void constant_define(int adr);//常量定义
void Variable_description();//变量说明部分
void Process_description(string proc,int flag);//过程说明部分
void ProcedureHead();//过程首部
void Statement(string proc);//语句
bool isfactor();//是否是因子
bool isAddSub();//加减运算符
bool isMulDiv();//;乘除运算符
bool isRelaOp();//关系运算符
void expression(string proc);//表达式
void term(string proc);//项
void factor(string proc);//因子
void condition(string proc);//条件
void assignment_sentence(string proc);//赋值语句
void condition_sentence(string proc);//条件语句
void loop_sentence(string proc);//循环语句
void process_sentence(string proc);//过程调用语句
void compound_sentence(string proc);//复合语句
void read_sentence(string proc);//读语句
void write_sentence(string proc);//写语句
bool GraOutput();//判断输出

int transToNumber(string num);//字符串转为数字
int findvar(string varname);//寻找标识符
int findproc(string procname);//寻找proc是否存在
int findInProc(string proc);//寻找proc入口地址



