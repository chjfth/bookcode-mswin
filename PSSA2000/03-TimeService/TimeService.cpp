/******************************************************************************
Module:  TimeService.cpp
Notices: Copyright (c) 2000 Jeffrey Richter
******************************************************************************/


#include "..\CmnHdr.h"                 /* See Appendix A. */

#include "..\ClassLib\IOCP.h"          /* See Appendix B */
#include "..\ClassLib\EnsureCleanup.h" /* See Appendix B */

#include <vaDbg.h>
#include <mswin/WinSvc.itc.h>
#include <mswin/Dbt.itc.h>

#include "../chjutils/chjutils.h"

#define SERVICESTATUS_IMPL
#include "ServiceStatus.h"


//////////////////////////////////////////////////////////////////////////////


TCHAR g_szServiceName[] = TEXT("03-TimeService");
TCHAR g_szServiceDisplayName[] = TEXT("03-TimeService from book PSSA2000");

CServiceStatus g_ssTime;


//////////////////////////////////////////////////////////////////////////////


// The completion port wakes for 1 of 2 reasons:
enum COMPKEY_et { 
   CK_SERVICECONTROL,   // A service control code
   CK_PIPE              // A client connects to our pipe
};


//////////////////////////////////////////////////////////////////////////////


DWORD WINAPI TimeHandlerEx(DWORD dwControl, DWORD dwEventType, PVOID pvEventData, PVOID pvContext) 
{
	vaDbgTs(
		_T("[tid=%d]CH03-TimeService: In TimeHandlerEx() callback,\n")
		_T("  dwControl   = %s\n")
		_T("  dwEventType = %s")
		,
		GetCurrentThreadId(),
		ITCS(dwControl, SERVICE_CONTROL_xxx),
		ITCS(dwEventType, DBT_xxx)
		);

	DWORD dwReturn = ERROR_CALL_NOT_IMPLEMENTED;
	BOOL fPostControlToServiceThread = FALSE;

	switch (dwControl) 
	{
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:
		g_ssTime.SetUltimateState(SERVICE_STOPPED, 2000);
		fPostControlToServiceThread = TRUE;
		break;

	case SERVICE_CONTROL_PAUSE:
		g_ssTime.SetUltimateState(SERVICE_PAUSED, 2000);
		fPostControlToServiceThread = TRUE;
		break;

	case SERVICE_CONTROL_CONTINUE:
		g_ssTime.SetUltimateState(SERVICE_RUNNING, 2000);
		fPostControlToServiceThread = TRUE;
		break;

	case SERVICE_CONTROL_INTERROGATE:
		g_ssTime.ReportStatus();
		break;

	case SERVICE_CONTROL_PARAMCHANGE:
		break;

	case SERVICE_CONTROL_DEVICEEVENT:
	case SERVICE_CONTROL_HARDWAREPROFILECHANGE:
	case SERVICE_CONTROL_POWEREVENT:
		break;

	case 128:   // A user-define code just for testing
		// NOTE: Normally, a service shouldn't display UI
		MessageBox(NULL, TEXT("In HandlerEx processing user-defined code."),
			g_szServiceName, MB_OK | MB_SERVICE_NOTIFICATION);
		break;
	}

	if (fPostControlToServiceThread) 
	{
		// The Handler thread is very simple and executes very quickly because
		// it just passes the control code off to the ServiceMain thread.

		CIOCP* piocp = (CIOCP*) pvContext;
		piocp->PostStatus(CK_SERVICECONTROL, dwControl);
		dwReturn = NO_ERROR;
	}

	return(dwReturn);
}

//////////////////////////////////////////////////////////////////////////////

inline void reset_ovlp_keep_hEvent(OVERLAPPED &ovlp)
{
	HANDLE he = ovlp.hEvent;
	ZeroMemory(&ovlp, sizeof(ovlp));
	ovlp.hEvent = he;
}

void WINAPI TimeServiceMain(DWORD dwArgc, PTSTR* pszArgv) 
{
	vaDbgTs(_T("[tid=%d]CH03-TimeService: In TimeServiceMain()."),
		GetCurrentThreadId());

	COMPKEY_et CompKey = CK_SERVICECONTROL;
	DWORD dwControl = SERVICE_CONTROL_CONTINUE;
	CEnsureCloseFile hpipe;
	OVERLAPPED o = {}, *po = nullptr;
	SYSTEMTIME st = {};
	DWORD dwNumBytes = 0;
	BOOL succ = 0;
	DWORD winerr = 0;
	int MaxPipeInstances = 1; // var to debug on the fly
	DWORD pipe_openmode = PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED;
	pipe_openmode |= FILE_FLAG_FIRST_PIPE_INSTANCE; // since Win2000 SP2

	// Create the completion port and save its handle in a global
	// variable so that the Handler function can access it.
	CIOCP iocp(0);

	g_ssTime.Initialize(g_szServiceName, TimeHandlerEx, (PVOID) &iocp, TRUE);

	g_ssTime.AcceptControls(
		SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE);

	do 
	{
		switch (CompKey) 
		{
		case CK_SERVICECONTROL:
			// We got a new control code
			switch (dwControl) 
			{
			case SERVICE_CONTROL_CONTINUE:
				
				// While running, create a pipe that clients can connect to.
				hpipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\TimeService"), 
					pipe_openmode,
					PIPE_TYPE_BYTE, 
					MaxPipeInstances, 
					sizeof(st), sizeof(st), 
					1000, NULL);

				// Chj: Check for error. (pipe-namespace creation fail)
				// May due to another TimeService.exe process has occupied the pipe-namespace.
				//
				if(hpipe==INVALID_HANDLE_VALUE)
				{
					winerr = GetLastError();
					if(g_ssTime.IsDebugMode())
					{
						// Only show UI when not running as service.
						vaMsgBox(NULL, MB_OK|MB_ICONEXCLAMATION, nullptr,
							_T("CreateNamedPipe() fail, %s"), WinerrStr(winerr));
					}
					else
					{
						vaDbgTs(_T("03-TimeService: CreateNamedPipe() fail, %s"), WinerrStr(winerr));
					}
					exit(5);
				}

				// Associate the pipe with the completion port
				iocp.AssociateDevice(hpipe, CK_PIPE);

				// Pend an asynchronous connect against the pipe
				reset_ovlp_keep_hEvent(o);
				succ = ConnectNamedPipe(hpipe, &o);
				g_ssTime.ReportUltimateState();
				break;

			case SERVICE_CONTROL_PAUSE:
			case SERVICE_CONTROL_STOP:

				// When not running, close the pipe so clients can't connect
				hpipe.Cleanup();
				g_ssTime.ReportUltimateState();
				break;

			}
			break; // break out of `CompKey`, not `for`

		case CK_PIPE:
			if (hpipe.IsValid()) 
			{
				// We got a client request: Send our current time to the client
				GetSystemTime(&st);
				succ = WriteFile(hpipe, &st, sizeof(st), &dwNumBytes, NULL);
				// -- Chj memo: We're a bit lazy by not using OVLP in this WriteFile.
				// It's OK bcz we just write a small amount of bytes, and those bytes
				// will go into sender-buffer, so we're not likely to get blocked in I/O.
				
				succ = FlushFileBuffers(hpipe);
				succ = DisconnectNamedPipe(hpipe);

				// Allow another client to connect 
				reset_ovlp_keep_hEvent(o);
				succ = ConnectNamedPipe(hpipe, &o);
			} 
			else 
			{
				// We get here when the pipe is closed
			}
		}

		if (g_ssTime != SERVICE_STOPPED) 
		{
			// Sleep until a control code comes in or a client connects

			ULONG_PTR ulptr = 0;
			succ = iocp.GetStatus(&ulptr, &dwNumBytes, &po);
			CompKey = (COMPKEY_et)ulptr;

			if(!succ)
			{
				DWORD bytesdone = 0;
				SetLastError(0); // deliberate, will show that GetOverlappedResult re-assign WinErr. 
				succ = GetOverlappedResult(hpipe, po, &bytesdone, TRUE);
				vaDbgTs(
					_T("[INFO] CH03 TimeService.cpp: iocp.GetStatus() fail, %s")
					_T("  -- this error can be ignored.")
					, 
					WinerrStr());
			}
			
			dwControl = dwNumBytes;
		}
	} while (g_ssTime != SERVICE_STOPPED);

}


//////////////////////////////////////////////////////////////////////////////


int myInstallService() 
{
	DWORD winerr = 0;

	// Open the SCM on this machine.
	CEnsureCloseServiceHandle hSCM = 
		OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

	if(hSCM.IsInvalid())
	{
		vaMsgBox(NULL, MB_OK|MB_ICONERROR, nullptr, _T("OpenSCManager() fail, %s"), WinerrStr());
		return 4;
	}

	// Get our full pathname
	TCHAR szModulePath[_MAX_PATH] = {};
	GetModuleFileName(NULL, szModulePath, chDIMOF(szModulePath)-1);

	TCHAR szCmdLine[_MAX_PATH+20] = {}; // add "/service" param
	_sntprintf_s(szCmdLine, _TRUNCATE, _T("\"%s\" /service"), szModulePath);

	vaDbgTs(
		_T("Will call CreateService() with:\n")
		_T("  lpServiceName = %s\n")
		_T("  lpDisplayName = %s\n")
		_T("  lpBinaryPathName = %s\n")
		,
		g_szServiceName,
		g_szServiceDisplayName,
		szCmdLine
		);

	// Add this service to the SCM's database.
	CEnsureCloseServiceHandle hService = 
		CreateService(hSCM, g_szServiceName, g_szServiceDisplayName,
		SERVICE_CHANGE_CONFIG, SERVICE_WIN32_OWN_PROCESS, 
		SERVICE_DEMAND_START, SERVICE_ERROR_IGNORE,
		szCmdLine, NULL, NULL, NULL, NULL, NULL);

	if(hService.IsValid())
	{
		vaDbgTs(_T("CreateService() success."));
	}
	else
	{
		winerr = GetLastError();
		vaDbgTs(_T("CreateService() fail, %s"), WinerrStr(winerr));
		vaMsgBox(NULL, MB_OK|MB_ICONERROR, nullptr, _T("CreateService() fail, %s"), WinerrStr(winerr));
		return 4;
	}

	SERVICE_DESCRIPTION sd = 
	{ 
		// This is the service "description" text that is shown in services.msc .
		TEXT("TimeService sample program from PSSA2000 book, CH03")
	};
	ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &sd);

	return 0;
}


int myRemoveService() 
{
	// Open the SCM on this machine.
	CEnsureCloseServiceHandle hSCM = 
		OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);

	if(hSCM.IsInvalid())
	{
		vaMsgBox(NULL, MB_OK|MB_ICONERROR, nullptr, _T("OpenSCManager() fail, %s"), WinerrStr());
		return 4;
	}

	// Open this service for DELETE access
	CEnsureCloseServiceHandle hService = 
		OpenService(hSCM, g_szServiceName, DELETE);

	if(hService.IsInvalid())
	{
		vaMsgBox(NULL, MB_OK|MB_ICONERROR, nullptr, _T("OpenService() fail, %s"), WinerrStr());
		return 4;
	}

	// Remove this service from the SCM's database.
	BOOL succ = DeleteService(hService);
	if(!succ)
	{
		vaMsgBox(NULL, MB_OK|MB_ICONERROR, nullptr, _T("DeleteService() fail, %s"), WinerrStr());
		return 4;
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////////


int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR pszCmdLine, int) 
{
	int nArgc = __argc;
#ifdef UNICODE
	PCTSTR *ppArgv = (PCTSTR*) CommandLineToArgvW(GetCommandLine(), &nArgc);
#else
	PCTSTR *ppArgv = (PCTSTR*) __argv;
#endif

	vaDbgTs(_T("[tid=%d]CH03-TimeService: In _tWinMain().")
		_T("  pszCmdLine = %s")
		,
		GetCurrentThreadId(), pszCmdLine);

	int exitcode = 0;

	if (nArgc < 2) 
	{
		MessageBox(NULL, 
			TEXT("Programming Server-Side Applications for Microsoft Windows: ")
			TEXT("Time Service Sample\n\n")
			TEXT("Usage: TimeService.exe [/install] [/remove] [/debug] [/service]")
			TEXT("\n")
			TEXT("   /install\t\tInstalls the service in the SCM's database.\n")
			TEXT("   /remove\t\tRemoves the service from the SCM's database.\n")
			TEXT("   /debug\t\tRuns as a normal process for debugging.")
			TEXT("   /service\t\tRun as a service (should only be set in the SCM's database)")
			,
			g_szServiceName, MB_OK);
	} 
	else 
	{
		for (int i = 1; i < nArgc; i++) 
		{
			if ((ppArgv[i][0] == TEXT('-')) || (ppArgv[i][0] == TEXT('/'))) 
			{
				// Command line switch
				if (lstrcmpi(&ppArgv[i][1], TEXT("install")) == 0) 
					exitcode = myInstallService();

				if (lstrcmpi(&ppArgv[i][1], TEXT("remove"))  == 0)
					exitcode = myRemoveService();

				if (lstrcmpi(&ppArgv[i][1], TEXT("debug"))   == 0) 
				{
					g_ssTime.SetDebugMode();

					// Execute the service code
					TimeServiceMain(0, NULL);
				}

				if (lstrcmpi(&ppArgv[i][1], TEXT("service")) == 0) 
				{
					// Connect to the service control dispatcher
					SERVICE_TABLE_ENTRY ServiceTable[] = {
						{ g_szServiceName, TimeServiceMain },
						{ NULL,            NULL }   // End of list
					};
					chVERIFY(StartServiceCtrlDispatcher(ServiceTable));
				}
			}
		}
	}

#ifdef UNICODE
	HeapFree(GetProcessHeap(), 0, (PVOID) ppArgv);
#endif
	return exitcode;
}


///////////////////////////////// End Of File /////////////////////////////////
