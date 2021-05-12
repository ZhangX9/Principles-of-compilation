#pragma once
#include"morphology.h"

extern string gram;
extern bool sign;

void program_begin();//开始
void program();//程序
void Subprogram(int flag);//分程序
void Constant_description();//常量说明部分
void Variable_description();//变量说明部分
void Process_description(int flag);//过程说明部分
void ProcedureHead();//过程首部
void Statement();//语句
void constant_define();//常量定义
bool isfactor();//是否是因子
bool isAddSub();//加减运算符
bool isMulDiv();//;乘除运算符
bool isRelaOp();//关系运算符
void expression();//表达式
void term();//项
void factor();//因子
void condition();//条件
void assignment_sentence();//赋值语句
void condition_sentence();//条件语句
void loop_sentence();//循环语句
void process_sentence();//过程调用语句
void compound_sentence();//复合语句
void read_sentence();//读语句
void write_sentence();//写语句
void GraOutput();
