#include"grammar.h"
//#include"morphology.h"

int main()
{
	//FILE* stream1;
	//freopen_s(&stream1, "in.txt", "r", stdin);
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

	if (MoOutput())
	{
		program_begin();
		GraOutput();
	}
	return 0;
}

/*
procedure f1;
begin
call c
end;
begin end.
*/