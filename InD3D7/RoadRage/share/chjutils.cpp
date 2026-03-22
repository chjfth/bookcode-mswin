#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include "world.hpp"
#include "chjutils.h"

extern BOOL logfile_start_flag;
extern BOOL RR_exit_debug_flag;
extern int total_allocated_memory_count;


static int PrintMsgX = 10;
static int PrintMsgY = 10;


void PrintMessage(HWND hwnd, const TCHAR *message1, const TCHAR *message2, int message_mode)
{
	FILE *fp = NULL;
	TCHAR tmessage[1000];


	if((message1 == NULL) && (message2 == NULL))
		return;

	if((message1 != NULL) && (message2 != NULL))
	{
		_tcscpy_s(tmessage, message1);
		_tcscat_s(tmessage, message2);
	}
	else
	{
		if(message1 != NULL) 
			_tcscpy_s(tmessage, message1);

		if(message2 != NULL) 
			_tcscpy_s(tmessage, message2);
	}


	if(logfile_start_flag == TRUE)
	{
		_tfopen_s(&fp, _T("rrlogfile.txt"), _T("w"));
		_ftprintf_s( fp, _T("%s\n\n"), _T("RR Logfile"));
	}
	else
	{
		_tfopen_s(&fp, _T("rrlogfile.txt"), _T("a"));
	}

	logfile_start_flag = FALSE;

	if(fp == NULL)
	{     
		MessageBox(hwnd, _T("Can't load logfile"), _T("File Error"), MB_OK);
		fclose(fp);
		return;
	}

	_ftprintf( fp, _T(" %s\n"), tmessage );


	if(message_mode != LOGFILE_ONLY)
	{
		HDC hdc=GetDC(hwnd);
		TextOut(hdc,PrintMsgX,PrintMsgY, tmessage, _tcslen(tmessage));
		PrintMsgY +=20;
		ReleaseDC(hwnd,hdc);
	}

	fclose(fp);
}


void PrintMemAllocated(int mem, const TCHAR *message)
{
	FILE *fp = NULL;
	TCHAR buffer[1000];
	TCHAR buffer2[1000];
	int mem_kb;

	if(logfile_start_flag == TRUE)
	{
		_tfopen_s(&fp, _T("rrlogfile.txt"), _T("w"));
		_ftprintf_s( fp, _T("%s\n\n"), _T("RR Logfile"));
	}
	else
	{
		_tfopen_s(&fp, _T("rrlogfile.txt"), _T("a"));
	}

	logfile_start_flag = FALSE;

	if(fp == NULL)
	{     
		MessageBox(NULL, _T("Can't load logfile"), _T("File Error"),MB_OK);
		fclose(fp);
		return;
	}

	_tcscpy_s(buffer, _T("memory allocated for "));
	_tcscat_s(buffer, message);
	_tcscat_s(buffer, _T(" = "));

	mem_kb = mem / 1024;
	_itot_s(mem_kb, buffer2, 10);
	_tcscat_s(buffer, buffer2);
	_tcscat_s(buffer, _T(" KB"));

	_ftprintf( fp, _T(" %s\n"), buffer );

	total_allocated_memory_count += mem;

	fclose(fp);
}
