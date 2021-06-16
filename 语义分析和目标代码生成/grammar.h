#pragma once
#include "morphology.h"
#include<stack>
#include<iomanip>

//����
#define VARIABLE "VARIABLE"
#define CONSTANT "CONSTANT"
#define PROCEDURE "PROCEDURE"

//���Ԫ�صĸ�ʽ
struct TableElement
{
	string name;//����
	string kind;
	int val;//����ֵ
	int level;//�������ڲ���
	int adr;//������Ե�ַ
	int coderows;//������ʼλ��
	TableElement();
	TableElement(string name, int kind, int val, int level, int adr);
	void clear();
};
extern vector<TableElement> table;//����
extern int LEVEL;//LEVEL<3

//Ŀ�����ṹ��
struct code
{
	string procname;
	string f;
	int l;
	int a;
	int index;//�洢������static_chain�е�λ��
	code();
	code(string procname, string f, int l, int a);
};

extern vector<code> object_code;//Ŀ�����

//�����洢��Ҫ����Ĳ���
struct WBCodeid
{
	string name;
	int codeid;
	WBCodeid();
	WBCodeid(string name, int codeid);
};
extern vector<WBCodeid> WriteBackCodeID;//��¼��Ҫ����ĳ�����ת��ַ���Ա��ַ����ʱ�������

//�����洢��������
struct node
{
	string name;//��ǰ���������
	int father;//������
	node(){}
};
extern vector<node> Static_chain;
extern vector<WBCodeid> WriteBackChain;//��¼��Ҫ�������ĳ���


void program_begin();//��ʼ
void program();//����
void Subprogram(string proc,int flag);//�ֳ���
void Constant_description();//����˵������
void constant_define(int adr);//��������
void Variable_description();//����˵������
void Process_description(string proc,int flag);//����˵������
void ProcedureHead();//�����ײ�
void Statement(string proc);//���
bool isfactor();//�Ƿ�������
bool isAddSub();//�Ӽ������
bool isMulDiv();//;�˳������
bool isRelaOp();//��ϵ�����
void expression(string proc);//���ʽ
void term(string proc);//��
void factor(string proc);//����
void condition(string proc);//����
void assignment_sentence(string proc);//��ֵ���
void condition_sentence(string proc);//�������
void loop_sentence(string proc);//ѭ�����
void process_sentence(string proc);//���̵������
void compound_sentence(string proc);//�������
void read_sentence(string proc);//�����
void write_sentence(string proc);//д���
bool GraOutput();//�ж����

int transToNumber(string num);//�ַ���תΪ����
int findvar(string varname);//Ѱ�ұ�ʶ��
int findproc(string procname);//Ѱ��proc�Ƿ����
int findInProc(string proc);//Ѱ��proc��ڵ�ַ



