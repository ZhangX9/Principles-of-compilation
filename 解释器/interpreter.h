#pragma once
#include<iostream>
#include<vector>
#include<stack>
#include<iomanip>
#include<fstream>
using namespace std;

//13���ؼ��ֹؼ��� 
extern const string k[13];
//11������� 
extern const string op[11];
//5���ָ���
extern const string sep[5];
//��ʶ��--0  ����--30  ����--(-1)

struct outfather
{
	string name;
	int father;
};

struct node
{
	string name;//��ǰ���������
	int index;//���׳���
	int StaOuter;//��̬���
	int DymOuter;//��̬���
	int backdis;//���ص�ַ
	vector<int> vec;//�洢����
	node() {}
};

//Ŀ�����ṹ��
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
extern vector<code> object_code;//Ŀ�����
extern int p;


void getNodeFromFile();//���ļ��еõ�����proc�����ݿռ�
void getCodeFromFile(int size);//���ļ��ж�ȡĿ�����


void interpreter();//����ִ�д���