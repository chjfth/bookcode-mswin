/******************************************************************************
Module:  FileCopy-nock.cpp
Notices: Copyright (c) 2000 Jeffrey Richter
[2022.12] Updated by Chj:
* Report file copying speed on success.
* Report error loudly when error occurs during file-copy. File copying failure
  can be reproduced easily with a file on a network shared folder.

* "nock" means "no completion-key" parameter.
   We can do this because the big-tail OVERLAPPED struct(CIOReq) can hold 
   an extra value that represent the "completion-key".

******************************************************************************/


#include "..\CmnHdr.h"                    // See Appendix A.
#include <WindowsX.h>
#include <assert.h>

#include "..\ClassLib\IOCP.h"             // See Appendix B.
#include "..\ClassLib\EnsureCleanup.h"    // See Appendix B.

#include <vaDbg.h>
#include "..\chjutils\chjutils.h"

#include "Resource.h"


///////////////////////////////////////////////////////////////////////////////

#define BUFFSIZE              (64 * 1024) // The size of an I/O buffer
#define MAX_PENDING_IO_REQS   4           // The maximum # of of I/Os

// The signature values indicate the type of completed I/O.
enum WhichOp_et
{
	CK_Unset = 0,
	CK_READ = 1,
	CK_WRITE = 2,
};

#define DEBUG_ALL

// Each I/O Request needs an OVERLAPPED structure and a data buffer
class CIOReq : public OVERLAPPED {
public:
	CIOReq() {
		Internal = InternalHigh = 0;   
		Offset = OffsetHigh = 0;   
		hEvent = NULL;
		m_nBuffSize = 0;
		m_pvData = NULL;

		m_op = CK_Unset;
	}

	~CIOReq() {
		if (m_pvData != NULL)
			VirtualFree(m_pvData, 0, MEM_RELEASE);
	}

	BOOL AllocBuffer(SIZE_T nBuffSize) {
		m_nBuffSize = nBuffSize;
		m_pvData = VirtualAlloc(NULL, m_nBuffSize, MEM_COMMIT, PAGE_READWRITE);
		return(m_pvData != NULL);
	}

	BOOL Read(HANDLE hDevice, PLARGE_INTEGER pliOffset = NULL) {
		if (pliOffset != NULL) {
			Offset     = pliOffset->LowPart;
			OffsetHigh = pliOffset->HighPart;
		}
		m_op = CK_READ;
		return(::ReadFile(hDevice, m_pvData, (DWORD)m_nBuffSize, NULL, this));
	}

	BOOL Write(HANDLE hDevice, PLARGE_INTEGER pliOffset = NULL) {
		if (pliOffset != NULL) {
			Offset     = pliOffset->LowPart;
			OffsetHigh = pliOffset->HighPart;
		}
		m_op = CK_WRITE;
		return(::WriteFile(hDevice, m_pvData, (DWORD)m_nBuffSize, NULL, this));
	}

private:
	SIZE_T m_nBuffSize;
	PVOID  m_pvData;

public:
	// Chj mod:
	WhichOp_et m_op;
};


///////////////////////////////////////////////////////////////////////////////

static void vaDbgReadWriteResult(BOOL succ, const TCHAR *opstr)
{
#ifdef DEBUG_ALL
	if(succ)
		vaDbgTs(_T("%s success."), opstr);
	else 
		vaDbgTs(_T("%s error, winerr=%d."), opstr, GetLastError());
#endif 
}

BOOL in_FileCopy(PCTSTR pszFileSrc, PCTSTR pszFileDst, 
	LARGE_INTEGER &liFileSizeSrc, DWORD *pWinErr) 
{
	*pWinErr = 0; 
	LARGE_INTEGER liFileSizeDst = { 0 };

	// Open the source file without buffering & get its size
	CEnsureCloseFile hfileSrc = CreateFile(pszFileSrc, GENERIC_READ, 
		FILE_SHARE_READ, NULL, OPEN_EXISTING, 
		FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED, 
		NULL);
	if (hfileSrc.IsInvalid()) 
		return FALSE;

	// Get the file's size
	GetFileSizeEx(hfileSrc, &liFileSizeSrc);

	// Non-buffered I/O requires sector-sized transfers.
	// I'll use buffer-size transfers since it's easier to calculate.
	liFileSizeDst.QuadPart = chROUNDUP(liFileSizeSrc.QuadPart, BUFFSIZE);

	// Open the destination file without buffering & set its size
	CEnsureCloseFile hfileDst = CreateFile(pszFileDst, GENERIC_WRITE, 
		0, NULL, CREATE_ALWAYS, 
		FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED, 
		hfileSrc);
	if (hfileDst.IsInvalid()) 
		return FALSE;

	// File systems extend files synchronously. Extend the destination file 
	// now so that I/Os execute asynchronously improving performance.
	SetFilePointerEx(hfileDst, liFileSizeDst, NULL, FILE_BEGIN);
	SetEndOfFile(hfileDst);

	// Create an I/O completion port and associate the files with it.
	CIOCP iocp(0);
	iocp.AssociateDevice(hfileSrc, NULL);  // Read from source file
	iocp.AssociateDevice(hfileDst, NULL); // Write to destination file

	// Initialize record-keeping variables
	CIOReq ior[MAX_PENDING_IO_REQS];
	LARGE_INTEGER liNextReadOffset = { 0 };
	int nReadsInProgress  = 0;
	int nWritesInProgress = 0;

	// Prime the file copy engine by simulating that writes have completed.
	// This causes read operations to be issued.
	for (int nIOReq = 0; nIOReq < chDIMOF(ior); nIOReq++) 
	{

		// Each I/O request requires a data buffer for transfers
		chVERIFY(ior[nIOReq].AllocBuffer(BUFFSIZE));
		nWritesInProgress++;
		ior[nIOReq].m_op = CK_WRITE;
		iocp.PostStatus(NULL, 0, &ior[nIOReq]);
	}

	// Loop while outstanding I/O requests still exist
	while ((nReadsInProgress > 0) || (nWritesInProgress > 0)) 
	{
		// Suspend the thread until an I/O completes
		DWORD dwNumBytes;
		CIOReq* pior;
		BOOL succ = iocp.GetStatus(NULL, &dwNumBytes, (OVERLAPPED**) &pior, INFINITE);

		WhichOp_et CompKey = pior->m_op;

		TCHAR timebuf[40];
#ifdef DEBUG_ALL
		vaDbgTs(_T("%s iocp.GetStatus(%s)=%s"), 
			now_timestr(timebuf, ARRAYSIZE(timebuf)), 
			CompKey==CK_READ ? _T("read") : _T("WRITE"),
			succ ? _T("succ") : _T("fail"));
#else
		if(!succ)
		{
			DWORD nowerr = GetLastError();

			if(*pWinErr==0)
				*pWinErr = nowerr; // record first error
			
			vaDbgTs(_T("%s iocp.GetStatus(%s) fail. %s"), 
				now_timestr(timebuf, ARRAYSIZE(timebuf)), 
				CompKey==CK_READ ? _T("read") : _T("WRITE"),
				app_WinErrStr(nowerr));
		}
#endif

		switch (CompKey) {
		case CK_READ:  // Read completed, write to destination
			nReadsInProgress--;

			if(!*pWinErr)
			{
				succ = pior->Write(hfileDst);  // Write to same offset as just read
				nWritesInProgress++;

				vaDbgReadWriteResult(succ, _T("WriteFile"));
			}
			
			break;

		case CK_WRITE: // Write completed, read from source
			nWritesInProgress--;
			
			if(*pWinErr)
				break;
			
			if (liNextReadOffset.QuadPart < liFileSizeDst.QuadPart) {
				// Not EOF, read the next block of data from the source file.
				succ = pior->Read(hfileSrc, &liNextReadOffset);
				nReadsInProgress++;
				liNextReadOffset.QuadPart += BUFFSIZE; // Advance source offset

				vaDbgReadWriteResult(succ, _T("ReadFile"));
			}
			break;
		
		default:
			assert(("Wrong CompKey value!", 0));
		}

	} // while

	return *pWinErr ? FALSE : TRUE;
}

BOOL FileCopy(PCTSTR pszFileSrc, PCTSTR pszFileDst, 
	__int64 *pFilesize, DWORD *pWinErr) 
{
	BOOL fOk = FALSE;    // Assume file copy fails
	LARGE_INTEGER liFileSizeSrc = { 0 };

	try 
	{
		fOk = in_FileCopy(pszFileSrc, pszFileDst, 
			liFileSizeSrc, // output file size on return
			pWinErr);
		*pFilesize = liFileSizeSrc.QuadPart;
	}
	catch (...) {
	}

	if (fOk) 
	{
		// The destination file size is a multiple of the page size. Open the
		// file WITH buffering to shrink its size to the source file's size.
		CEnsureCloseFile hfileDst = CreateFile(pszFileDst, GENERIC_WRITE, 
			0, NULL, OPEN_EXISTING, 0, NULL);
		if (hfileDst.IsValid()) {

			SetFilePointerEx(hfileDst, liFileSizeSrc, NULL, FILE_BEGIN);
			SetEndOfFile(hfileDst);
		}
	}

	return(fOk);
}


///////////////////////////////////////////////////////////////////////////////


BOOL Dlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) 
{
   chSETDLGICONS(hwnd, IDI_FileCopy);

   // Disable Copy button since no file is selected yet.
   EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);
   return(TRUE);
}


///////////////////////////////////////////////////////////////////////////////


void Dlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) 
{
	TCHAR szPathname[_MAX_PATH];

	switch (id) 
	{{
	case IDCANCEL:
		EndDialog(hwnd, id);
		break;

	case IDOK:
	{
		// Copy the source file to the destination file.
		Static_GetText(GetDlgItem(hwnd, IDC_SRCFILE), 
			szPathname, sizeof(szPathname));
		SetCursor(LoadCursor(NULL, IDC_WAIT));

		DWORD msec_start = GetTickCount();

		__int64 filesize = 0;
		DWORD winerr = 0;
		BOOL succ = FileCopy(szPathname, TEXT("FileCopy-nock.cpy"), &filesize, &winerr);

		DWORD msec_used = GetTickCount() - msec_start;

		if(succ)
		{
			// Show time-used and speed.
			if(msec_used==0)
			{
				vaMsgBox(NULL, MB_OK, nullptr, TEXT("File Copy Successful. Time used: 0 msec."));
			}
			else
			{
				vaMsgBox(NULL, MB_OK, nullptr, TEXT("File Copy Successful. \n\n")
					TEXT("Time used: %d.%03d seconds, %g MB/s")
					,
					msec_used/1000, msec_used%1000, (double)filesize/1000/msec_used
					);
			}
		}
		else
		{
			vaMsgBox(NULL, MB_OK|MB_ICONERROR, nullptr,
				_T("File Copy Failed. %s"), app_WinErrStr(winerr));
		}

		break;
	}

	case IDC_PATHNAME:
		OPENFILENAME ofn = { OPENFILENAME_SIZE_VERSION_400 };
		ofn.hwndOwner = hwnd;
		ofn.lpstrFilter = TEXT("*.*\0");
		lstrcpy(szPathname, TEXT("*.*"));
		ofn.lpstrFile = szPathname;
		ofn.nMaxFile = chDIMOF(szPathname);
		ofn.lpstrTitle = TEXT("Select file to copy");
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
		BOOL fOk = GetOpenFileName(&ofn);
		if (fOk) {
			// Show user the source file's size
			Static_SetText(GetDlgItem(hwnd, IDC_SRCFILE), szPathname);
			CEnsureCloseFile hfile = CreateFile(szPathname, 0, 0, NULL, 
				OPEN_EXISTING, 0, NULL);
			if (hfile.IsValid()) {
				LARGE_INTEGER liFileSize;
				GetFileSizeEx(hfile, &liFileSize);
				// NOTE: Only shows bottom 32-bits of size
				SetDlgItemInt(hwnd, IDC_SRCFILESIZE, liFileSize.LowPart, FALSE);
			}
		}
		EnableWindow(GetDlgItem(hwnd, IDOK), fOk);
		break;
	}}
}


///////////////////////////////////////////////////////////////////////////////


INT_PTR WINAPI Dlg_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
		chHANDLE_DLGMSG(hwnd, WM_INITDIALOG, Dlg_OnInitDialog);
		chHANDLE_DLGMSG(hwnd, WM_COMMAND,    Dlg_OnCommand);
	}
	return(FALSE);
}


///////////////////////////////////////////////////////////////////////////////


int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR pszCmdLine, int) {

	DialogBox(hinstExe, MAKEINTRESOURCE(IDD_FileCopy), NULL, Dlg_Proc);
	return(0);
}


//////////////////////////////// End of File //////////////////////////////////
