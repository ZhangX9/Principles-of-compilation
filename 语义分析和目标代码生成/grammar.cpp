#include "grammar.h"
#include<map>



TableElement::TableElement()
{
	kind = -1;
	val = 0;
	level = 0;
	adr = 0;
	coderows = 0;
}
TableElement::TableElement(string name, int kind, int val, int level, int adr)
{
	this->name = name;
	this->kind = kind;
	this->val = val;
	this->level = level;
	this->adr = adr;
	coderows = 0;
}
void TableElement::clear()
{
	name.clear();
	kind = -1;
	val = 0;
	level = 0;
	adr = 0;
	coderows = 0;
}

vector<TableElement> table;//������㣬����
int LEVEL = 0;//LEVEL<3


code::code() { index = 0; }
code::code(string procname,string f,int l,int a)
{
	this->procname = procname;
	this->f = f;
	this->l = l;
	this->a = a;
	this->index = 0;
}
vector<code> object_code;//Ŀ�����


WBCodeid::WBCodeid() { codeid = 0; }
WBCodeid::WBCodeid(string name, int codeid)
{
	this->name = name;
	this->codeid = codeid;
}
vector<WBCodeid> WriteBackCodeID;//��¼��Ҫ����ĳ�����ת��ַ���Ա��ַ����ʱ�������

vector<node> Static_chain;//��̬��
vector<WBCodeid> WriteBackChain;//��¼��Ҫ����ľ�̬��

bool sign = true;//����﷨�Ƿ����
bool sem_sign = true;//��������Ƿ����

//��ʼ
void program_begin()
{
	if (word.size() <= 0)sign = false;
	p = 0;
	object_code.push_back(code("empty","goto", 0, 0));//object_code[0]���洢����
	object_code.push_back(code("main","JMP", 0, 0));//object_code[1]�洢main�Ŀ�ʼ��ַ
	program();
}

//����  <����>��<�ֳ���>.
void program()
{
	Subprogram("main",0);
	if (p < (int)word.size() && word[p].name == ".")
	{
		p++;
		object_code.push_back(code("main","OPR", 0, 0));
		if (p < (int)word.size())sign = false;
	}
	else
	{
		sign = false;
	}
}

//�ֳ���  <�ֳ���>��[<����˵������>][<����˵������>][<����˵������>]<���>
void Subprogram(string proc,int flag)
{
	if (p >= (int)word.size())return;
	if (sign == false)return;

	//����˵������
	if (p < (int)word.size() && word[p].name == "CONST")
	{
		p++;
		Constant_description();
	}
	//����˵������
	if (p < (int)word.size() && word[p].name == "VAR")
	{
		p++;
		Variable_description();
	}
	//����˵������
	if (p < (int)word.size() && word[p].name == "PROCEDURE")
	{
		p++;
		Process_description(proc,flag);
	}

	//�������ݿռ�,����Ѱ�Ҹù��̲���֮ǰ������var����������
	int varNumber = 0;
	int tableindex = 0;
	for (int i = (int)table.size() - 1; i >= 0; i--)
	{
		if (table[i].kind == VARIABLE && table[i].level == LEVEL)
		{
			varNumber++;
		}
		else if (table[i].kind==PROCEDURE && table[i].level == LEVEL - 1)
		{
			tableindex = i;
			break;
		}
	}
	object_code.push_back(code(proc, "INT", 0, 3 + varNumber));
	
	node nodetemp;
	nodetemp.name = proc;
	//��д���
	if (LEVEL == 0)
	{
		nodetemp.father = -1;//mainû�����
	}
	else if (LEVEL - 1 == 0)
	{
		//���Ϊmain
		WriteBackChain.push_back(WBCodeid("main", Static_chain.size()));
	}
	else if(LEVEL - 1 > 0)
	{
		//Ѱ�������proc
		for (int j = tableindex - 1; j >= 0; j--)
		{
			//�ҵ�������proc,�洢��ǰ������vector�е�λ�ã��ȴ�����
			if (table[j].kind == PROCEDURE && table[j].level == table[tableindex].level - 1)
			{
				WriteBackChain.push_back(WBCodeid(table[j].name, Static_chain.size()));
				break;
			}
		}
	}
	Static_chain.push_back(nodetemp);

	object_code[object_code.size() - 1].index = Static_chain.size() - 1;//proc��Static_chain�е�λ��
	//��д����ʼ��λ��
	int beginproc = object_code.size() - 1;//����ʼ��λ��
	if (proc == "main")object_code[1].a = beginproc;
	else
	{
		for (int i = (int)table.size() - 1; i >= 0; i--)
		{
			if (table[i].name==proc && table[i].kind == PROCEDURE)
			{
				table[i].coderows = beginproc;
				break;
			}
		}
	}
	//������ת���ó���ĵ�ַ
	for (int i=0;i<(int)WriteBackCodeID.size();)
	{
		if (WriteBackCodeID[i].name == proc)
		{
			object_code[WriteBackCodeID[i].codeid].a = beginproc;
			WriteBackCodeID.erase(WriteBackCodeID.begin() + i);
		}
		else
		{
			i++;
		}
	}
	//�������
	for (int i = 0; i < (int)WriteBackChain.size();)
	{
		if (WriteBackChain[i].name == proc)
		{
			Static_chain[WriteBackChain[i].codeid].father = Static_chain.size() - 1;
			WriteBackChain.erase(WriteBackChain.begin() + i);
		}
		else i++;
	}

	//���
	Statement(proc);
}

//����˵������  <����˵������> �� CONST<��������>{, <��������>};
void Constant_description()
{
	if (sign == false) return;
	int adr = 0;
	constant_define(adr);

	while (p < (int)word.size() && word[p].name == ",")
	{
		p++;
		adr++;
		constant_define(adr);
	}

	if (p < (int)word.size() && word[p].name == ";") { p++; }
	else { sign = false; return; }

}

//��������  <��������> �� <��ʶ��> = <�޷�������>
void constant_define(int adr)
{
	if (sign == false) return;
	if (p >= (int)word.size())return;

	TableElement consttemp;
	consttemp.kind = CONSTANT; consttemp.adr = adr; consttemp.level = LEVEL;

	if (word[p].type == 0)//��ʶ��
	{
		//�жϸñ�ʶ���Ƿ��Ѿ�����
		int i = findvar(word[p].name);
		if (i != -1) { sem_sign = false; }//����Ѿ��������������

		consttemp.name = word[p].name;
		p++;
		if (p < (int)word.size() && word[p].name == "=")
		{
			p++;
			if (p < (int)word.size() && word[p].type == 30)//�޷�������
			{
				consttemp.val = transToNumber(word[p].name);//תΪ����
				table.push_back(consttemp);
				p++;
			}
			else { sign = false; return; }

		}
		else { sign = false; return; }
	}
	else { sign = false; return; }
}

//����˵������  <����˵������> �� VAR<��ʶ��>{, <��ʶ��>};
void Variable_description()
{
	if (sign == false) return;
	int adr = 3;
	TableElement vartemp;
	vartemp.kind = VARIABLE; vartemp.adr = adr; vartemp.level = LEVEL;
	//��ʶ��
	if (p < (int)word.size() && word[p].type == 0) 
	{
		
		int i = findvar(word[p].name);
		if (i != -1 && table[i].kind == CONSTANT) { sem_sign = false; }//�ñ�ʶ���Ƿ��Ѿ�����������

		vartemp.name = word[p].name; 
		p++; 
		table.push_back(vartemp); 
	}
	else { sign = false; return; }

	while (p < (int)word.size() && word[p].name == ",")
	{
		vartemp.clear();
		adr++;
		vartemp.kind = VARIABLE; vartemp.adr = adr; vartemp.level = LEVEL;
		p++;
		if (p < (int)word.size() && word[p].type == 0) 
		{ 
			int i = findvar(word[p].name);
			if (i != -1 && table[i].kind == CONSTANT) { sem_sign = false; }//�ñ�ʶ���Ƿ��Ѿ�����������

			vartemp.name = word[p].name; 
			p++; 
			table.push_back(vartemp); 
		}
		else { sign = false; return; }
	}

	if (p < (int)word.size() && word[p].name == ";") { p++; }
	else { sign = false; return; }
}

//����˵������  <����˵������> �� <�����ײ�><�ֳ���>; {<����˵������>}
void Process_description(string proc,int flag)
{
	if (sign == false)return;
	//if (sem_sign == false)return;

	flag++;
	if (flag > 3) { sign = false; return; }

	ProcedureHead();
	if (sign == false)return;
	else { proc = table[table.size() - 1].name; }
	Subprogram(proc, flag);

	if (p < (int)word.size() && word[p].name == ";")
	{
		LEVEL--;
		p++;
		object_code.push_back(code(proc,"OPR", 0, 0));
		while (p < (int)word.size() && word[p].name == "PROCEDURE")
		{
			p++;
			Process_description(proc,0);
		}
	}
	else { sign = false; return; }

}

//�����ײ�  <�����ײ�> �� PROCEDURE <��ʶ��>;
void ProcedureHead()
{
	TableElement protemp;
	protemp.kind = PROCEDURE; protemp.level = LEVEL;
	if (p < (int)word.size() && word[p].type == 0)
	{
		//�жϸñ�ʶ���Ƿ��ѱ�����
		int i = findproc(word[p].name);
		if (i != -1)sem_sign = false;//�ض���

		i = findvar(word[p].name);
		if (i != -1)sem_sign = false;//�ñ�ʶ��������Ϊ�˱������߳���

		protemp.name = word[p].name;
		p++;
		if (p < (int)word.size() && word[p].name == ";")
		{
			table.push_back(protemp);
			LEVEL++;
			p++;
		}
		else { sign = false; return; }
	}
	else { sign = false; return; }
}

//���  <���> �� <��ֵ���> | <�������> | <����ѭ�����> | <���̵������> | <�����> | <д���> | <�������> | <�����>
void Statement(string proc)
{
	if (sign == false) return;

	if (p < (int)word.size() && word[p].type == 0)
	{//��ֵ���
		assignment_sentence(proc);
	}
	else if (p < (int)word.size() && word[p].name == "WHILE")
	{//����ѭ�����
		loop_sentence(proc);
	}
	else if (p < (int)word.size() && word[p].name == "CALL")
	{//���̵������
		process_sentence(proc);
	}
	else if (p < (int)word.size() && word[p].name == "READ")
	{//�����
		read_sentence(proc);
	}
	else if (p < (int)word.size() && word[p].name == "WRITE")
	{//д���
		write_sentence(proc);
	}
	else if (p < (int)word.size() && word[p].name == "BEGIN")
	{//�������
		compound_sentence(proc);
	}
	else if (p < (int)word.size() && word[p].name == "IF")
	{//�������
		condition_sentence(proc);
	}
	else
	{
		//gram += "EMPTY";
	}

}

//�Ƿ�������  <����> �� <��ʶ��> | <�޷�������> | (<���ʽ>)
bool isfactor()
{
	if (p < (int)word.size() && (word[p].type == 0 || word[p].type == 30 || word[p].name == "("))return true;
	return false;
}

//�Ӽ������ < �Ӽ������ > �� + | -
bool isAddSub()
{
	if (p < (int)word.size() && (word[p].name == "+" || word[p].name == "-"))return true;
	return false;
}

//�˳������ <�˳������> �� * |/
bool isMulDiv()
{
	if (p < (int)word.size() && (word[p].name == "*" || word[p].name == "/"))return true;
	return false;
}

//��ϵ����� <��ϵ�����> �� = | # | <|<=|>|>=
bool isRelaOp()
{
	if (p >= (int)word.size())return false;

	if (word[p].name == "=" || word[p].name == "#")return true;
	else if (word[p].name == "<" || word[p].name == "<=")return true;
	else if (word[p].name == ">" || word[p].name == ">=")return true;

	return false;

}

//���ʽ  <���ʽ> ��[+| -]<��>{<�Ӽ������><��>}
void expression(string proc)
{
	if (sign == false) return;

	int temp1 = -1;
	if (isAddSub()) { temp1 = p; p++; }
	if (isfactor())
	{
		term(proc);
		if (temp1 != -1)
		{
			if (word[temp1].name == "-")object_code.push_back(code(proc, "OPR", 0, 40));//����
			else object_code.push_back(code(proc, "OPR", 0, 41));//����
		}

		while (isAddSub())
		{
			int tempp = p;
			p++;
			if (isfactor())
			{
				term(proc);
				object_code.push_back(code(proc, "OPR", 0, word[tempp].type));//�Ӽ������
			}
			else { sign = false; return; }
		}
	}
	else { sign = false; return; }	
}

//���� //<����> �� <��ʶ��> | <�޷�������> | (<���ʽ>)
void factor(string proc)
{
	if (p < (int)word.size() && word[p].name == "(")
	{
		p++;
		expression(proc);
		if (p < (int)word.size() && word[p].name == ")") { p++; }
		else { sign = false; return; }

	}
	else if (p < (int)word.size() && (word[p].type == 0))
	{
		int i = findvar(word[p].name);
		if (i == -1)sem_sign = false;
		else if (table[i].kind == VARIABLE)
		{
			object_code.push_back(code(proc, "LOD", abs(LEVEL - table[i].level), table[i].adr));

		}
		else if (table[i].kind == CONSTANT)object_code.push_back(code(proc, "LIT", 0, table[i].val));
		else sem_sign = false;//kkkk
		p++;
	}
	else if (p < (int)word.size() && word[p].type == 30)
	{
		int val = transToNumber(word[p].name);
		object_code.push_back(code(proc, "LIT", 0, val));
		p++;
	}
	else { sign = false; return; }
}

//�� <��> �� <����>{<�˳������><����>}
void term(string proc)
{
	if (sign == false) return;
	factor(proc);
	while (isMulDiv())
	{
		int temp1 = p;
		p++;
		if (isfactor())
		{
			factor(proc);
			object_code.push_back(code(proc, "OPR", 0, word[temp1].type));//�˳������
		}
		else { sign = false; return; }
	}
}

//��ֵ���  <��ֵ���> �� <��ʶ��> := <���ʽ>
void assignment_sentence(string proc)
{
	if (sign == false)return;
	
	//�жϱ�ʶ���Ƿ����
	int i = findvar(word[p].name);
	p++;
	if (p < (int)word.size() && word[p].name == ":=")
	{
		p++;
		expression(proc);
		if (i == -1||table[i].kind == CONSTANT)sem_sign = false;
		else { object_code.push_back(code(proc, "STO", abs(LEVEL - table[i].level), table[i].adr)); }//д��
	}
	else { sign = false; return; }

}

//����  <����> �� <���ʽ><��ϵ�����><���ʽ> | ODD<���ʽ>
void condition(string proc)
{
	if (p >= (int)word.size())return;
	if (sign == false) return;

	if (p < (int)word.size() && word[p].name == "ODD")
	{
		int temp = p;//��¼odd������
		p++;
		expression(proc);
		object_code.push_back(code(proc, "OPR", 0, word[temp].type));
	}
	else
	{
		expression(proc);
		if (isRelaOp())
		{
			int temp = p;//��¼��ϵ�����������
			p++;
			expression(proc);
			object_code.push_back(code(proc, "OPR", 0, word[temp].type));
		}
		else { sign = false; return; }
	}

}

//�������  <�������> �� IF<����>THEN<���>
void condition_sentence(string proc)
{
	if (sign == false) return;
	p++;
	condition(proc);
	object_code.push_back(code(proc, "JPC", 0, 0));//����������ʱ��ת
	int id = object_code.size() - 1;//��¼����ָ��Ĵ����
	if (p < (int)word.size() && word[p].name == "THEN")
	{
		p++;
		Statement(proc);
		object_code[id].a = object_code.size();//��д֮ǰ����ת��ַ
	}
	else { sign = false;  return; }

}

//ѭ�����  <����ѭ�����> �� WHILE<����>DO<���>
void loop_sentence(string proc)
{
	if (sign == false) return;

	p++;
	int whileid = object_code.size();//whileѭ�������
	condition(proc);
	object_code.push_back(code(proc, "JPC", 0, 0));//����������ʱ����ѭ��
	int id = object_code.size() - 1;//��¼����ָ��Ĵ����
	if (p < (int)word.size() && word[p].name == "DO")
	{
		p++;
		Statement(proc);
		object_code.push_back(code(proc, "JMP", 0, whileid));//����ѭ�����
		object_code[id].a = object_code.size();//��д֮ǰ����ת��ַ
	}
	else { sign = false; return; }

}

//���̵������  <���̵������> �� CALL<��ʶ��>
void process_sentence(string proc)
{
	if (sign == false) return;

	p++;
	if (p < (int)word.size() && word[p].type == 0)
	{
		int i = findproc(word[p].name);
		if (i == -1)sem_sign = false;//�ù��̵��ò�����
		else//�õ��õĹ��̴���
		{
			int j = findInProc(word[p].name);
			if (j == -1)
			{
				//��ת��ַ����0
				object_code.push_back(code(proc, "CAL", abs(LEVEL - table[i].level), 0));
				//��ת��ַ��֪�����ȴ�����
				WriteBackCodeID.push_back(WBCodeid(word[p].name, (int)object_code.size() - 1));
				
			}
			else object_code.push_back(code(proc, "CAL", abs(LEVEL - table[i].level), j));//��ת��ַ��֪��ֱ����д
		}
		p++;
	}
	else { sign = false; return; }
}

//�������  <�������> �� BEGIN<���>{ ; <���> } END
void compound_sentence(string proc)
{
	if (sign == false) return;

	p++;
	Statement(proc);
	while (p < (int)word.size() && word[p].name == ";")
	{
		p++;
		Statement(proc);
	}

	if (p < (int)word.size() && word[p].name == "END")
	{
		p++;
	}
	else { sign = false; return; }

}

//����� <�����> �� READ(<��ʶ��>{, <��ʶ��>})
void read_sentence(string proc)
{
	if (sign == false)return;

	int temp = p;//�洢read������
	object_code.push_back(code(proc, "OPR", 0, word[p].type));//read����

	p++;
	if (p < (int)word.size() && word[p].name == "(")
	{
		p++;
		if (p < (int)word.size() && word[p].type == 0)
		{
			//���ұ�ʶ���Ƿ����
			int i = findvar(word[p].name);
			//�����ڣ��������ܱ�����
			if (i == -1 || table[i].kind == CONSTANT)sem_sign = false;
			else 
			{ 
				//���ǳ������Ը���
				object_code.push_back(code(proc, "STO", abs(LEVEL - table[i].level), table[i].adr)); 
			}

			p++;
			while (p < (int)word.size() && word[p].name == ",")
			{
				p++;
				object_code.push_back(code(proc, "OPR", 0, word[temp].type));//read����
				if (p < (int)word.size() && word[p].type == 0)
				{
					//���ұ�ʶ���Ƿ����
					i = findvar(word[p].name);
					//�����ڣ��������ܱ�����
					if (i == -1 || table[i].kind == CONSTANT)sem_sign = false;
					else 
					{ 
						//���ǳ������Ը���
						object_code.push_back(code(proc, "STO", abs(LEVEL - table[i].level), table[i].adr)); 
					}
					p++;
				}
				else { sign = false; return; }
			}
		}
		else { sign = false;  return; }

		if (p < (int)word.size() && word[p].name == ")") { p++; }
		else { sign = false; return; }
	}
	else { sign = false; return; }

}

//д��� < д��� > �� WRITE(<��ʶ��>{, <��ʶ��>})
void write_sentence(string proc)
{
	if (sign == false) return;
	int temp = p;

	p++;
	if (p < (int)word.size() && word[p].name == "(")
	{
		p++;
		if (p < (int)word.size() && word[p].type == 0)
		{
			//���ұ�ʶ���Ƿ����
			int i = findvar(word[p].name);
			if (i == -1)sem_sign = false;//����������
			else if (table[i].kind == VARIABLE)
			{
				object_code.push_back(code(proc, "LOD", abs(LEVEL - table[i].level), table[i].adr));//ȡ��

			}
			else if (table[i].kind == CONSTANT)object_code.push_back(code(proc, "LIT", 0, table[i].val));//ȡ����
			else sem_sign = false;

			object_code.push_back(code(proc, "OPR", 0, word[temp].type));//write����

			p++;
			while (p < (int)word.size() && word[p].name == ",")
			{
				p++;
				if (p < (int)word.size() && word[p].type == 0)
				{
					//���ұ�ʶ���Ƿ����
					i = findvar(word[p].name);
					if (i == -1)sem_sign = false;//����������
					else if (table[i].kind == VARIABLE)
					{
						object_code.push_back(code(proc, "LOD", abs(LEVEL - table[i].level), table[i].adr));//ȡ����

					}
					else if (table[i].kind == CONSTANT)object_code.push_back(code(proc, "LIT", 0, table[i].val));//ȡ����
					else sem_sign = false;

					object_code.push_back(code(proc, "OPR", 0, word[temp].type));//write����

					p++;
				}
				else { sign = false; return; }
			}
		}
		else { sign = false; return; }

		if (p < (int)word.size() && word[p].name == ")") { p++; }
		else { sign = false; return; }
	}
	else { sign = false; return; }
}

//�жϲ����
bool GraOutput()
{
	if (sign == false)
	{
		cout << "Syntax Error" << endl;
		return false;
	}

	if (sem_sign == false)
	{
		cout << "Semantic Error" << endl;
		return false;
	}

	ofstream file;
	file.open("staticchain.txt", ios_base::out);
	file << Static_chain.size() << endl;
	for (int i = 0; i < (int)Static_chain.size(); i++)
	{
		file << Static_chain[i].name << " " << Static_chain[i].father;
		file << endl;
	}
	file << object_code.size();
	file.close();

	ofstream ofs;
	ofs.open("code.txt", ios_base::out);
	for (int i = 1; i < (int)object_code.size(); i++)
	{
		ofs <<object_code[i].procname;
		ofs << " " << object_code[i].index;
		//ofs << " " << object_code[i].f;
		//ofs << " " << object_code[i].l;
		//ofs << " " << object_code[i].a;
		ofs << endl;
		cout << object_code[i].f << " " << object_code[i].l << " " << object_code[i].a << endl;
	}
	ofs.close();


	return true;

}

//�ַ���תΪ����
int transToNumber(string num)
{
	int ans = 0;
	for (int i = 0; i < (int)num.size(); i++)
	{
		ans = ans * 10 + (num[i] - '0');
	}
	return ans;
}

//Ѱ�ұ�ʶ��
int findvar(string varname)
{
	int level = LEVEL;

	for (int i = table.size() - 1; i >= 0; i--)
	{
		if (table[i].kind == PROCEDURE && table[i].level == level - 1)
		{
			level--;
		}
		else if (table[i].name == varname && table[i].level == level &&(table[i].kind==VARIABLE||table[i].kind==CONSTANT))
		{
			return i;
		}
	}
	return -1;
}

//Ѱ��proc�Ƿ����
int findproc(string procname)
{
	int level = LEVEL;
	for (int i = (int)table.size() - 1; i >= 0; i--)
	{
		if (table[i].name == procname && table[i].kind == PROCEDURE && (table[i].level == level || table[i].level == level - 1))
		{
			return i;
		}
		else if (table[i].name != procname && table[i].kind == PROCEDURE && table[i].level == level - 1)
		{
			level--;
		}
	}
	return -1;

}

//Ѱ��proc��ڵ�ַ
int findInProc(string proc)
{
	for (int i = object_code.size() - 1; i >= 1; i--)
	{
		if (object_code[i].f == "INT" && object_code[i].procname == proc)
		{
			return i;
		}
	}
	return -1;
}


