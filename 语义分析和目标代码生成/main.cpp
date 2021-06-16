#include"grammar.h"
//#include "cratetable.h"
//#include"morphology.h"

int main()
{
	//FILE* stream1;
	//freopen_s(&stream1, "int.txt", "r", stdin);
	while (getline(cin, instring))
	{
		p = 0;
		while (p < (int)instring.size())
		{
			analysis();//´Ê·¨·ÖÎö
		}
		row++;
		line = 1;
		p = 0;
		instring.clear();
	}
	//fclose(stream1);
	if (MoOutput())
	{
		program_begin();
		if (!GraOutput())
		{
			return -1;
		}
	}
	return 0;
}

