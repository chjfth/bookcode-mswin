#include <stdio.h>
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
	char tmessage[1000];


	if((message1 == NULL) && (message2 == NULL))
		return;

	if((message1 != NULL) && (message2 != NULL))
	{
		strcpy_s(tmessage, message1);
		strcat_s(tmessage, message2);
	}
	else
	{
		if(message1 != NULL) 
			strcpy_s(tmessage, message1);

		if(message2 != NULL) 
			strcpy_s(tmessage, message2);
	}


	if(logfile_start_flag == TRUE)
	{
		fopen_s(&fp, "rrlogfile.txt","w");
		fprintf( fp, "%s\n\n", "RR Logfile");
	}
	else
	{
		fopen_s(&fp, "rrlogfile.txt","a");
	}

	logfile_start_flag = FALSE;

	if(fp == NULL)
	{     
		MessageBoxA(hwnd, "Can't load logfile", "File Error", MB_OK);
		fclose(fp);
		return;
	}

	fprintf( fp, " %s\n", tmessage );


	if(message_mode != LOGFILE_ONLY)
	{
		HDC hdc=GetDC(hwnd);
		TextOutA(hdc,PrintMsgX,PrintMsgY, tmessage,strlen(tmessage));
		PrintMsgY +=20;
		ReleaseDC(hwnd,hdc);
	}

	fclose(fp);
}


void PrintMemAllocated(int mem, const TCHAR *message)
{
	FILE *fp = NULL;
	char buffer[1000];
	char buffer2[1000];
	int mem_kb;

	if(logfile_start_flag == TRUE)
	{
		fopen_s(&fp, "rrlogfile.txt","w");
		fprintf( fp, "%s\n\n", "RR Logfile");
	}
	else
	{
		fopen_s(&fp, "rrlogfile.txt","a");
	}

	logfile_start_flag = FALSE;

	if(fp == NULL)
	{     
		MessageBoxA(NULL,"Can't load logfile","File Error",MB_OK);
		fclose(fp);
		return;
	}

	strcpy_s(buffer, "memory allocated for ");
	strcat_s(buffer, message);
	strcat_s(buffer, " = ");

	mem_kb = mem / 1024;
	_itoa_s(mem_kb, buffer2, 10);
	strcat_s(buffer, buffer2);
	strcat_s(buffer, " KB");

	fprintf( fp, " %s\n", buffer );

	total_allocated_memory_count += mem;

	fclose(fp);
}
