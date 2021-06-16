#include"interpreter.h"

//13���ؼ��ֹؼ���      1      2        3         4      5     6    7     8      9      10     11    12      13
const string k[13] = { "CONST","VAR","PROCEDURE","BEGIN","END","ODD","IF","THEN","CALL","WHILE","DO","READ","WRITE" };
//11�������  =14  :=15  +16   -17  *18  /19  #20  <21  <=22   >23  >=24
const string op[11] = { "=",":=","+","-","*","/","#","<","<=",">",">=" };
//5���ָ���  ;--25  ,--26   .--27  (--28  )--29
const string sep[5] = { ";",",",".","(",")" };
//��ʶ��--0  ����--30  ����--(-1)
//����--40   ����---41

code::code() { }
code::code(string f, int l, int a)
{
	this->f = f;
	this->l = l;
	this->a = a;
}

vector<outfather> Static_chain;
vector<code> object_code;//Ŀ�����
int p;//ȡָ��


//���ļ��еõ�����proc�����ݿռ�
void getNodeFromFile()
{
	ifstream file;
	//file.open("D:\\vs\\vsʵ�����\\����ԭ��\\Pl0����ԭ��ʵ��\\Pl0����ԭ��ʵ��\\staticchain.txt", ios_base::in);
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

//���ļ��ж�ȡĿ�����
void getCodeFromFile(int size)
{
	ifstream in;
	//in.open("D:\\vs\\vsʵ�����\\����ԭ��\\Pl0����ԭ��ʵ��\\Pl0����ԭ��ʵ��\\code.txt", ios_base::in);
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




//����ִ�д���
void interpreter()
{
	vector<node> run;
	stack<int> cal;//����ʱ����ջ���������м���
	p = 1;
	while (p < (int)object_code.size())
	{
		if (object_code[p].f == "JMP")
		{
			//��ת��main��ʼ�ĵط����Ƚ�main����ջ��
			if (p == 1)
			{
				int tp = object_code[p].a;
				node temp;
				temp.name = object_code[tp].procname;//��¼��ǰ���������
				temp.index = object_code[tp].index;//���main��static_chain�е�λ��
				temp.StaOuter = -1;//main�ľ�̬��㲻����
				temp.DymOuter = -1;//main�Ķ�̬��㲻����
				temp.backdis = -1;//main�޷��ص�ַ
				run.push_back(temp);//��ջ
			}
			p = object_code[p].a;
		}
		else if (object_code[p].f == "INT")//Ϊ��ǰ���򿪱�������
		{
			for (int i = 0; i < object_code[p].a - 3; i++)
			{//����������
				run[run.size() - 1].vec.push_back(0);
			}
			p++;

		}
		else if (object_code[p].f == "JPC")//ջ��Ϊ0����ת��a
		{
			if (cal.top() == 0)p = object_code[p].a;
			else p++;
			cal.pop();
		}
		else if (object_code[p].f == "CAL")
		{
			int tp = object_code[p].a;//cal��proc��ʼ��λ��
			node temp;
			temp.name = object_code[tp].procname;//��¼��ǰ���������
			temp.index = object_code[tp].index;//���proc��static_chain�е�λ�ã�����Ѱ�������
			temp.DymOuter = run.size() - 1;//��¼�䶯̬��㣬һ���ǵ�ǰ����ջջ����proc
			temp.backdis = p + 1;//���ص�ַ
			//������ջ��Ѱ���侲̬���
			if (Static_chain[temp.index].father == -1)//���������
			{
				temp.StaOuter = -1;
			}
			else
			{
				for (int i = (int)run.size() - 1; i >= 0; i--)
				{
					//run[i]��static_chain�е�λ�õ��ڵ�ǰ���������static_chain�е�λ��
					if (run[i].index == Static_chain[temp.index].father)
					{
						temp.StaOuter = i;
						break;
					}
				}
			}
			run.push_back(temp);//��ջ
			p = object_code[p].a;//��ת
		}
		else if (object_code[p].f == "LOD")
		{
			int temp = run.size() - 1;//���Ϊ0���ڵ�ǰ��Ѱ������
			if (object_code[p].l >= 1)
			{
				for (int i = 0; i < object_code[p].l; i++)
				{
					temp = run[temp].StaOuter;
				}
			}
			cal.push(run[temp].vec[object_code[p].a - 3]);
			p++;//ָ�������ƶ�
		}
		else if (object_code[p].f == "STO")
		{
			int temp = run.size() - 1;//���Ϊ0���ڵ�ǰ��Ѱ������
			if (object_code[p].l >= 1)
			{
				for (int i = 0; i < object_code[p].l; i++)
				{
					temp = run[temp].StaOuter;
				}
			}
			run[temp].vec[object_code[p].a - 3] = cal.top();
			cal.pop();
			p++;//ָ�������ƶ�
		}
		else if (object_code[p].f == "LIT")//ȡ����
		{
			cal.push(object_code[p].a);
			p++;
		}
		else if (object_code[p].f == "OPR")
		{
			switch (object_code[p].a)
			{
				case 0://�˳�
				{
					if (run[run.size() - 1].backdis != -1)
					{
						p = run[run.size() - 1].backdis;
						p--;//switch����ʱ����p++
					}
					run.erase(run.begin() + run.size() - 1);//��ջ��ɾ��
					break;
				}
				case 6://�ж��Ƿ�������
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
				case 14://����
				{
					int t1 = cal.top();
					cal.pop();
					int t2 = cal.top();
					cal.pop();
					if (t2 == t1)cal.push(1);
					else cal.push(0);
					break;
				}
				case 16://��
				{
					int t1 = cal.top();
					cal.pop();
					int t2 = cal.top();
					cal.pop();
					cal.push(t2 + t1);
					break;
				}
				case 17://��
				{
					int t1 = cal.top();
					cal.pop();
					int t2 = cal.top();
					cal.pop();
					cal.push(t2 - t1);
					break;
				}
				case 18://��
				{
					int t1 = cal.top();
					cal.pop();
					int t2 = cal.top();
					cal.pop();
					cal.push(t2 * t1);
					break;
				}
				case 19://��
				{
					int t1 = cal.top();
					cal.pop();
					int t2 = cal.top();
					cal.pop();
					//��������Ϊ0
					if (t1 == 0) { exit(-1); }
					cal.push(t2 / t1);
					break;
				}
				case 20://������
				{
					int t1 = cal.top();
					cal.pop();
					int t2 = cal.top();
					cal.pop();
					if (t2 != t1)cal.push(1);
					else cal.push(0);
					break;

				}
				case 21://С��
				{
					int t1 = cal.top();
					cal.pop();
					int t2 = cal.top();
					cal.pop();
					if (t2 < t1)cal.push(1);
					else cal.push(0);
					break;
				}
				case 22://С�ڵ���
				{
					int t1 = cal.top();
					cal.pop();
					int t2 = cal.top();
					cal.pop();
					if (t2 <= t1)cal.push(1);
					else cal.push(0);
					break;
				}
				case 23://����
				{
					int t1 = cal.top();
					cal.pop();
					int t2 = cal.top();
					cal.pop();
					if (t2 > t1)cal.push(1);
					else cal.push(0);
					break;
				}
				case 24://���ڵ���
				{
					int t1 = cal.top();
					cal.pop();
					int t2 = cal.top();
					cal.pop();
					if (t2 >= t1)cal.push(1);
					else cal.push(0);
					break;
				}
				case 40://����
				{
					int t1 = cal.top();
					cal.pop();
					cal.push(-t1);
					break;
				}
				case 41://����
					break;
				default:
					break;
			}
			p++;
		}

	}
}