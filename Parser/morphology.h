#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<iostream>
#include<fstream>
using namespace std;

//13���ؼ��ֹؼ��� 
extern const string k[13];
//11������� 
extern const string op[11];
//5���ָ���
extern const string sep[5];
//��ʶ��--0  ����--30  ����--(-1)

struct Tuple
{
	string name;
	int type;
	int row;
	int line;
	Tuple(string name, int type, int r, int l);
};

//ȫ�ֱ���
extern int row, line;//����������
extern int t, p;
extern string instring;//������
extern vector<Tuple> vec[35];//�洢��ȷ��Ԫ��
extern vector<Tuple> err;//�洢����
extern vector<Tuple> word;
extern string tnum, tch;



//��������
bool isletter(char x);//�ж��Ƿ����ַ�
bool isdigit(char x);//�ж��Ƿ�������
bool iskey();//�ж��Ƿ��ǹؼ���
void analysis();//��������
void AnalyIdent();//������ʶ�����߹ؼ���
void AnalyConst();//��������
void symbol();//��������
bool MoOutput();//���
