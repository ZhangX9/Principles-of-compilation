#pragma once
#include"morphology.h"

extern string gram;
extern bool sign;

void program_begin();//��ʼ
void program();//����
void Subprogram(int flag);//�ֳ���
void Constant_description();//����˵������
void Variable_description();//����˵������
void Process_description(int flag);//����˵������
void ProcedureHead();//�����ײ�
void Statement();//���
void constant_define();//��������
bool isfactor();//�Ƿ�������
bool isAddSub();//�Ӽ������
bool isMulDiv();//;�˳������
bool isRelaOp();//��ϵ�����
void expression();//���ʽ
void term();//��
void factor();//����
void condition();//����
void assignment_sentence();//��ֵ���
void condition_sentence();//�������
void loop_sentence();//ѭ�����
void process_sentence();//���̵������
void compound_sentence();//�������
void read_sentence();//�����
void write_sentence();//д���
void GraOutput();
