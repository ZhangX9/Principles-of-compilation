#include"grammar.h"

string gram="";
bool sign = true;

//��ʼ
void program_begin()
{
	if (word.size() <= 0)sign = false;
	p = 0;
	gram += "PROGRAM(";
	program();
	gram += ")";
}

//����  <����>��<�ֳ���>.
void program()
{
	//p = 0;
	gram += "SUBPROG(";
	Subprogram(0);
	gram += ")";
	if (p < (int)word.size() && word[p].name == ".")
	{
		gram += ",."; p++;
		if (p < (int)word.size())sign = false;
	}
	else
	{
		sign = false; 
	}
}

//�ֳ���  <�ֳ���>��[<����˵������>][<����˵������>][<����˵������>]<���>
void Subprogram(int flag)
{
	if (p >= (int)word.size())return;
	if (sign == false)return;
	int i = 0;

	//����˵������
	if (p < (int)word.size() && word[p].name == "CONST")
	{
		//if (i != 0)gram += ",";
		p++;
		gram += "CONSTANTDECLARE(";
		Constant_description();
		gram += ")";
		i++;
	}
	//����˵������
	if (p < (int)word.size() && word[p].name == "VAR")
	{
		if (i != 0)gram += ",";
		p++;
		gram += "VARIABLEDECLARE(";
		Variable_description();
		gram += ")";
		i++;
	}
	//����˵������
	if (p < (int)word.size() && word[p].name == "PROCEDURE")
	{
		if (i != 0)gram += ",";
		p++;
		gram += "PROCEDUREDECLARE(";
		Process_description(flag);
		gram += ")";
		//if (flag > 0)gram += ",";
		i++;
	}
	//���
	if (i != 0)gram += ",";
	gram += "SENTENCE(";
	Statement();
	gram += ")";
}

//����˵������  <����˵������> �� CONST<��������>{, <��������>};
void Constant_description()
{
	if (sign == false) return;

	gram += "CONST,";
	gram += "CONSTANTDEFINE(";
	constant_define();
	gram += ")";

	while (p < (int)word.size() && word[p].name == ",") 
	{
		gram += ",COMMA,"; p++; 
		gram += "CONSTANTDEFINE(";
		constant_define(); 
		gram += ")";
	}

	if (p < (int)word.size() && word[p].name == ";") { gram += ",;"; p++; }
	else { sign = false; return; }

}

//����˵������  <����˵������> �� VAR<��ʶ��>{, <��ʶ��>};
void Variable_description()
{
	if (sign == false) return;

	gram += "VAR,";
	//��ʶ��
	if (p < (int)word.size() && word[p].type == 0) { gram += word[p].name; p++; }
	else { sign = false; return; }

	while (p < (int)word.size() && word[p].name == ",")
	{
		gram += ",COMMA,"; 
		p++;
		if (p < (int)word.size() && word[p].type == 0) { gram += word[p].name; p++; }
		else { sign = false; return; }
	}

	if (p < (int)word.size() && word[p].name == ";") { gram += ",;"; p++; }
	else { sign = false; return; }
	
}

//����˵������  <����˵������> �� <�����ײ�><�ֳ���>; {<����˵������>}
void Process_description(int flag)
{
	if (sign == false)return;
	flag++;
	if (flag > 3) { sign = false; return; }

	gram += "PROCEDUREHEAD(";
	ProcedureHead();
	gram += ")"; gram += ",";

	gram += "SUBPROG(";
	Subprogram(flag);
	gram += ")";
	if (p < (int)word.size() && word[p].name == ";")
	{
		gram += ",;";
		p++;
		while (p < (int)word.size() && word[p].name == "PROCEDURE")
		{
			p++;
			gram += ",";
			gram += "PROCEDUREDECLARE(";
			Process_description(0);
			gram += ")";
		}
	}
	else { sign = false; return; }
	
}

//�����ײ�  <�����ײ�> �� PROCEDURE <��ʶ��>;
void ProcedureHead()
{
	gram += "PROCEDURE,";
	if (p < (int)word.size() && word[p].type == 0)
	{
		gram += word[p].name; gram += ","; p++;
		if (p < (int)word.size() && word[p].name == ";")
		{
			gram += ";"; p++;
		}
		else { sign = false; return; }
	}
	else { sign = false; return; }
}

//���  <���> �� <��ֵ���> | <�������> | <����ѭ�����> | <���̵������> | <�����> | <д���> | <�������> | <�����>
void Statement()
{
	if (sign == false) return;

	if (p < (int)word.size() && word[p].type == 0)
	{//��ֵ���
		gram += "ASSIGNMENT(";
		assignment_sentence();
		gram += ")";
	}
	else if (p < (int)word.size() && word[p].name == "WHILE")
	{//����ѭ�����
		gram += "WHILESENTENCE(";
		loop_sentence();
		gram += ")";
	}
	else if (p < (int)word.size() && word[p].name == "CALL")
	{//���̵������
		gram += "CALLSENTENCE(";
		process_sentence();
		gram += ")";
	}
	else if(p < (int)word.size() && word[p].name == "READ")
	{//�����
		gram += "READSENTENCE(";
		read_sentence();
		gram += ")";
	}
	else if (p < (int)word.size() && word[p].name == "WRITE")
	{//д���
		gram += "WRITESENTENCE(";
		write_sentence();
		gram += ")";
	}
	else if (p < (int)word.size() && word[p].name == "BEGIN")
	{//�������
		gram += "COMBINED(";
		compound_sentence();
		gram += ")";
	}
	else if (p < (int)word.size() && word[p].name == "IF")
	{//�������
		gram += "IFSENTENCE(";
		condition_sentence();
		gram += ")";
	}
	else
	{
		gram += "EMPTY";
	}

}

//��������  <��������> �� < ��ʶ�� >= <�޷�������>
void constant_define()
{
	if (sign == false) return; 

	if (p >= (int)word.size())return;

	if (word[p].type == 0)
	{//��ʶ��
		gram += word[p].name; gram += ","; p++;
		//=
		if (p < (int)word.size() && word[p].name == "=")
		{
			gram += "="; gram += ","; p++;
			//�޷�������
			if (p < (int)word.size() && word[p].type == 30) { gram += word[p].name; p++; }
			else { sign = false; return; }

		}
		else { sign = false;return; }
	}
	else { sign = false; return; }
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
void expression()
{
	if (sign == false) return;

	if (isAddSub()){ gram += word[p].name; gram += ","; p++; }

	if (isfactor())
	{
		gram += "ITEM(";
		term();
		gram += ")";

		while (isAddSub())
		{
			gram += ","; gram += word[p].name; gram += ","; p++;
			if (isfactor())
			{
				gram += "ITEM(";
				term();
				gram += ")";
			}
			else { sign = false; return; }
		}
	}
	else
	{
		sign = false;
		return;
	}
}

//���� //<����> �� <��ʶ��> | <�޷�������> | (<���ʽ>)
void factor()
{
	if (p < (int)word.size() && word[p].name == "(")
	{
		gram += "LP"; gram += ","; p++;
		gram += "EXPRESSION(";
		expression();
		gram += ")"; gram += ",";
		if (p < (int)word.size() && word[p].name == ")") { gram += "RP"; p++; }
		else { sign = false; return; }

	}
	else if (p < (int)word.size() && (word[p].type == 0 || word[p].type == 30))
	{ 
		gram += word[p].name; p++;
	}
	else { sign = false; return; }
}

//�� <��> �� <����>{<�˳������><����>}
void term()
{
	if (sign == false) return;

	gram += "FACTOR(";
	factor();
	gram += ")";

	while (isMulDiv())
	{
		gram += ","; gram += word[p].name; gram += ","; p++;
		if (isfactor())
		{
			gram += "FACTOR(";
			factor();
			gram += ")";
		}
		else { sign = false; return; }
	}
}

//��ֵ���  <��ֵ���> �� <��ʶ��>: = <���ʽ>
void assignment_sentence()
{
	if (sign == false)return; 

	gram += word[p].name; gram += ","; p++;
	if (p < (int)word.size() && word[p].name == ":=")
	{
		gram += word[p].name; gram += ","; p++;
		gram += "EXPRESSION(";
		expression();
		gram += ")";
	}
	else { sign = false; return; }

}

//����  <����> �� <���ʽ><��ϵ�����><���ʽ> | ODD<���ʽ>
void condition()
{
	if (p >= (int)word.size())return;
	if (sign == false) return; 

	if (p < (int)word.size() && word[p].name == "ODD")
	{
		gram += word[p].name, gram += ",", p++;
		gram += "EXPRESSION(";
		expression();
		gram += ")";
	}
	else
	{
		gram += "EXPRESSION(";
		expression();
		gram += ")";
		if (isRelaOp())
		{
			gram += ",", gram += word[p].name, gram += ",", p++;
			gram += "EXPRESSION(";
			expression();
			gram += ")";
		}
		else { sign = false; return; }
	}
	
}

//�������  <�������> �� IF<����>THEN<���>
void condition_sentence()
{
	if (sign == false) return; 

	gram += "IF"; gram += ","; p++;
	gram += "CONDITION(";
	condition();
	gram += ")"; gram += ",";
	if (p < (int)word.size() && word[p].name == "THEN")
	{
		gram += "THEN"; gram += ","; p++;
		gram += "SENTENCE(";
		Statement();
		gram += ")";
	}
	else { sign = false;  return; }

}

//ѭ�����  <����ѭ�����> �� WHILE<����>DO<���>
void loop_sentence()
{
	if (sign == false) return; 

	gram += "WHILE"; gram += ","; p++;
	gram += "CONDITION(";
	condition();
	gram += ")"; gram += ",";
	if (p < (int)word.size() && word[p].name == "DO")
	{
		gram += "DO"; gram += ","; p++;
		gram += "SENTENCE(";
		Statement();
		gram += ")";
	}
	else { sign = false; return; }

}

//���̵������  <���̵������> �� CALL<��ʶ��>
void process_sentence()
{
	if (sign == false) return; 

	gram += "CALL"; gram += ","; p++;
	if (p < (int)word.size() && word[p].type == 0)
	{
		gram += word[p].name;
		p++;
	}
	else { sign = false; return; }
}

//�������  <�������> �� BEGIN<���>{ ; <���> } END
void compound_sentence()
{
	if (sign == false) return; 

	gram += "BEGIN,"; p++;
	gram += "SENTENCE(";
	Statement();
	gram += ")";
	while (p < (int)word.size() && word[p].name == ";")
	{
		gram += ","; gram += word[p].name; gram += ","; p++;
		gram += "SENTENCE(";
		Statement();
		gram += ")";
	}

	if (p < (int)word.size() && word[p].name == "END")
	{
		gram += ","; gram += "END"; p++;
	}
	else { sign = false; return; }

}

//����� <�����> �� READ(<��ʶ��>{, <��ʶ��>})
void read_sentence()
{
	if (sign == false)return; 

	gram += "READ"; gram += ","; p++;
	if (p < (int)word.size() && word[p].name == "(")
	{
		gram += "LP"; gram += ","; p++;
		if (p < (int)word.size() && word[p].type == 0)
		{
			gram += word[p].name; p++;

			while (p < (int)word.size() && word[p].name == ",")
			{
				gram += ","; gram += "COMMA"; gram += ","; p++;
				if (p < (int)word.size() && word[p].type == 0)
				{
					gram += word[p].name; p++;
				}
				else { sign = false; return; }
			}
		}
		else { sign = false;  return; }

		if (p < (int)word.size() && word[p].name == ")") { gram += ",RP"; p++; }
		else { sign = false; return; }
	}
	else { sign = false; return; }

}

//д��� < д��� > �� WRITE(<��ʶ��>{, <��ʶ��>})
void write_sentence()
{
	if (sign == false) return; 

	gram += "WRITE"; gram += ","; p++;
	if (p < (int)word.size() && word[p].name == "(")
	{
		gram += "LP"; gram += ","; p++;
		if (p < (int)word.size() && word[p].type == 0)
		{
			gram += word[p].name; p++;

			while (p < (int)word.size() && word[p].name == ",")
			{
				gram += ","; gram += "COMMA"; gram += ",";
				if (p < (int)word.size() && word[p].type == 0)
				{
					gram += word[p].name; p++;
				}
				else { sign = false; return; }
			}
		}
		else { sign = false; return; }

		if (p < (int)word.size() && word[p].name == ")") { gram += ",RP"; p++; }
		else { sign = false; return; }
	}
	else { sign = false; return; }
}

//�жϲ����
void GraOutput()
{
	//cout << gram << endl;
	if (sign == false)
	{
		cout << "Syntax Error" << endl;
	}
	else
	{
		cout << gram << endl;
	}
}