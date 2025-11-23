#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	
	_tprintf(_T("Hello, Chapter3_Ex1_chj!\n"));
	_tprintf(_T("sizeof(TCHAR)=%d\n"), (int)sizeof(TCHAR));
	return 0;
}

