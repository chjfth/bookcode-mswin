/******************************************************************************
Module:  TokenMaster.cpp
Notices: Copyright (c) 2000 Jeffrey Richter
******************************************************************************/

#define PRINTBUF_IMPL

#include "shareinc.h"
#include "DaclPage.h"
#include "TokenMaster-helper.h"
#include "../chjutils/chjutils.h"
#include "exe_version.h"

#include "Resource.h"

#define JULAYOUT_IMPL
#include <mswin/JULayout2.h>

#define AUTOBUF_IMPL
#include "..\ClassLib\AutoBuf.h"       // See Appendix B.

#define Combobox_EnableWideDrop_IMPL
#include <mswin/Combobox_EnableWideDrop.h>

#include <process.h>
#include <vaDbg.h>

HANDLE g_hSnapShot = NULL;
HANDLE g_hToken = NULL;

HWND g_hwndProcessCombo;
HWND g_hwndThreadCombo;
HWND g_hwndToken;
HWND g_hwndStatus;
HWND g_hwndEnablePrivileges;
HWND g_hwndEnableGroups;
HWND g_hwndDeletedPrivileges;
HWND g_hwndDisabledSids;
HWND g_hwndRestrictedSids;
HWND g_hwndLogonTypes;
HWND g_hwndLogonProviders;
HWND g_hwndImpersonationLevels;
HWND g_hwndTokenTypes;

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hInst;

///////////////////////////////////////////////////////////////////////////////

void SetDlgDefaultButton(HWND hwndDlg, UINT idDefault) 
{
	// Get that last default control
	UINT nOld = (UINT) SendMessage(hwndDlg, DM_GETDEFID, 0, 0);

	// Reset the current default push button to a regular button.
	if (HIWORD(nOld) == DC_HASDEFID)
	{
		SendDlgItemMessage(hwndDlg, LOWORD(nOld), BM_SETSTYLE, 
			BS_PUSHBUTTON, // make it a normal button
			(LPARAM) TRUE);
	}

	// Update the default push button's control ID.
	SendMessage(hwndDlg, DM_SETDEFID, idDefault, 0L);

	// Set the new style.
	SendDlgItemMessage(hwndDlg, idDefault, BM_SETSTYLE, 
		BS_DEFPUSHBUTTON, // make it a stand-out button
		(LPARAM) TRUE); 
}

void SmartDefaultButton(HWND hwndDlg, int idCtrl) 
{
	// chjmemo: For the input idCtrl, check whether it has mapping in nCtrlMap[],
	// If yes, set the target Uic(must be a "button") to have BS_DEFPUSHBUTTON style.
	// This caters the paradigm: When user shifts dialog focus to some Uic-F,
	// user want current default-push-button to be some Uic-B, -- so that, 
	// user pressing Enter-key would execute that Uic-B button.

	int nCtrlMap[][2] = {
		// Uic-F                  Uic-B
		{ IDC_PROCESSES,          IDB_DUMPTOKEN },
		{ IDC_THREADS,            IDB_DUMPTOKEN },
		{ IDC_IMPERSONATIONLEVEL, IDB_DUPLICATE },
		{ IDC_TOKENTYPE,          IDB_DUPLICATE },
		{ IDE_USERNAME,           IDB_LOGONUSER },
		{ IDE_PASSWORD,           IDB_LOGONUSER },
		{ IDC_LOGONTYPE,          IDB_LOGONUSER },
		{ IDC_LOGONPROVIDER,      IDB_LOGONUSER },
		{ IDE_FILENAME,           IDB_CREATEPROCESS },
		{ -1,                     -1 }
	};

	int nIndex = 0;
	while (nCtrlMap[nIndex][0] != -1) {
		if (nCtrlMap[nIndex][0] == idCtrl) {
			SetDlgDefaultButton(hwndDlg, nCtrlMap[nIndex][1]);
			break;
		}
		nIndex++;
	}
}

void Cmd_Processes(UINT codeNotify) 
{
	switch (codeNotify) {

	case CBN_DROPDOWN:
		RefreshSnapShot();
		guiPopulateProcessCombo();
		break;

	case CBN_SELCHANGE:
		guiPopulateThreadCombo();
		EnableWindow(g_hwndThreadCombo, ComboBox_GetCurSel(g_hwndProcessCombo));
		break;
	}
}

void Cmd_Threads(HWND hwnd, UINT codeNotify) 
{
	int nSel = ComboBox_GetCurSel(g_hwndThreadCombo);
	if (nSel == 0)
		SetDlgItemText(hwnd, IDB_DUMPTOKEN, TEXT("OpenProcessToken"));
	else
		SetDlgItemText(hwnd, IDB_DUMPTOKEN, TEXT("OpenThreadToken"));
}

void Cmd_LogonUser(HWND hwnd) 
{

	// Do we have a token?  If so, kill it.
	if (g_hToken != NULL) {
		CloseHandle(g_hToken);
		g_hToken = NULL;
	}

	// Get the credentials
	TCHAR szName[1024];
	GetDlgItemText(hwnd, IDE_USERNAME, szName, ARRAYSIZE(szName));

	TCHAR szPassword[1024];
	GetDlgItemText(hwnd, IDE_PASSWORD, szPassword, ARRAYSIZE(szPassword));

	int nLogonType = ComboBox_GetCurSel(g_hwndLogonTypes);
	nLogonType = (int)ComboBox_GetItemData(g_hwndLogonTypes, nLogonType);

	int nLogonProvider = ComboBox_GetCurSel(g_hwndLogonProviders);
	nLogonProvider = (int)ComboBox_GetItemData(g_hwndLogonProviders, nLogonProvider);

	DWORD dwStatus = 0;
	PTSTR szStatus = TEXT("User Logon Succeeded, Dumped Token");

	// Get a token for the credentials.
	if (LogonUser(szName, NULL, szPassword, nLogonType, nLogonProvider, &g_hToken))
	{
		// Success?  Display it
		guiDumpToken();

	} else {

		// Failure?  Clear the dialog box
		dwStatus = GetLastError();
		szStatus = TEXT("LogonUser");
		g_hToken = NULL;
		SetDlgItemText(hwnd, IDE_TOKEN, TEXT(""));
		ListBox_ResetContent(g_hwndEnablePrivileges);
		ListBox_ResetContent(g_hwndEnableGroups);
		ListBox_ResetContent(g_hwndDeletedPrivileges);
		ListBox_ResetContent(g_hwndDisabledSids);
		ListBox_ResetContent(g_hwndRestrictedSids);
	}

	// Display the status either way
	RefreshStatus(szStatus, dwStatus);
}


void Cmd_DuplicateToken(HWND hwnd) 
{
	DWORD dwStatus = 0;
	PTSTR szStatus = TEXT("Token Duplicated, Dumped Token");

	// Do we have a token?  If not, fail and complain
	if (g_hToken == NULL) {

		szStatus = TEXT("No Token.  A starting token is required");

	} else {

		HANDLE hOldToken = g_hToken;
		g_hToken = NULL;

		int nIndex = ComboBox_GetCurSel(g_hwndImpersonationLevels);
		SECURITY_IMPERSONATION_LEVEL nLevel = (SECURITY_IMPERSONATION_LEVEL)
			ComboBox_GetItemData( g_hwndImpersonationLevels, nIndex);

		nIndex = ComboBox_GetCurSel(g_hwndTokenTypes);
		TOKEN_TYPE nType = (TOKEN_TYPE)ComboBox_GetItemData(g_hwndTokenTypes, nIndex);

		// Copy our token
		HANDLE hNewToken;
		if (DuplicateTokenEx(hOldToken, TOKEN_ALL_ACCESS, NULL, nLevel, nType,
			&hNewToken) == TRUE) {

				g_hToken = hNewToken;
				guiDumpToken();

		} else {

			// Failure?  Clear the dialog box
			dwStatus = GetLastError();
			szStatus = TEXT("DuplicateTokenEx");
			SetDlgItemText(hwnd, IDE_TOKEN, TEXT(""));
			ListBox_ResetContent(g_hwndEnablePrivileges);
			ListBox_ResetContent(g_hwndEnableGroups);
			ListBox_ResetContent(g_hwndDeletedPrivileges);
			ListBox_ResetContent(g_hwndDisabledSids);
			ListBox_ResetContent(g_hwndRestrictedSids);
		}

		// No matter what, we kill the last token and take on the new
		CloseHandle(hOldToken);
	}

	// Display the status either way
	RefreshStatus(szStatus, dwStatus);
}

void Cmd_DumpToken(HWND hwnd) 
{
	// Get the token for the process
	guiGetToken(hwnd);
	if (g_hToken != NULL) {

		// Success? Display it
		guiDumpToken();

	} else {

		// No?   Then clear out the dialog box
		SetDlgItemText(hwnd, IDE_TOKEN, TEXT(""));
		ListBox_ResetContent(g_hwndEnablePrivileges);
		ListBox_ResetContent(g_hwndEnableGroups);
		ListBox_ResetContent(g_hwndDeletedPrivileges);
		ListBox_ResetContent(g_hwndDisabledSids);
		ListBox_ResetContent(g_hwndRestrictedSids);
	}
}

void Cmd_AdjustGroups()  
{

	PTOKEN_GROUPS ptgGroups = NULL;

	try {{

		// No token?  Give up
		if (g_hToken == NULL) {
			RefreshStatus(TEXT("No Token"), 0);
			goto leave;
		}

		// Allocate a buffer with the token information
		ptgGroups = (PTOKEN_GROUPS) myAllocateTokenInfo(g_hToken, TokenGroups);
		if (ptgGroups == NULL)
			goto leave;

		// Enumerate through the list box and find groups to enable in the token
		DWORD dwItem = ListBox_GetCount(g_hwndEnableGroups);
		while (dwItem-- != 0) {

			DWORD dwIndex = (DWORD) ListBox_GetItemData(g_hwndEnableGroups, dwItem); // chj to check
			BOOL fSel = ListBox_GetSel(g_hwndEnableGroups, dwItem);
			if (fSel)
				ptgGroups->Groups[dwIndex].Attributes |= SE_GROUP_ENABLED;
			else
				ptgGroups->Groups[dwIndex].Attributes &= (~SE_GROUP_ENABLED);
		}

		// Actually adjust the token groups for the token
		if (!AdjustTokenGroups(g_hToken, FALSE, ptgGroups, 0, NULL, NULL))
			RefreshStatus(TEXT("AdjustTokenGroups"), GetLastError());
		else
			guiDumpToken();   // Display the new token

	} leave:;
	} catch(...) {}

	// Cleanup
	if (ptgGroups != NULL)
		LocalFree(ptgGroups);
}

void Cmd_AdjustTokenPrivileges() 
{
	// No token?  Buh-Bye
	if (g_hToken == NULL) {
		RefreshStatus(TEXT("No Token"), 0);
		return;
	}

	// Get the token information for privileges
	PTOKEN_PRIVILEGES ptpPrivileges = 
		(PTOKEN_PRIVILEGES) myAllocateTokenInfo(g_hToken, TokenPrivileges);
	CEC_LocalFree cec_ptp = ptpPrivileges;
	if (ptpPrivileges == NULL)
		return;

	// Enumerate privileges to enable
	DWORD dwItem = ListBox_GetCount(g_hwndEnablePrivileges);
	while (dwItem-- != 0) {

		DWORD dwIndex = (DWORD) ListBox_GetItemData(g_hwndEnablePrivileges, dwItem);
		BOOL fSel = ListBox_GetSel(g_hwndEnablePrivileges, dwItem);
		if (fSel)
			ptpPrivileges->Privileges[dwIndex].Attributes |= SE_PRIVILEGE_ENABLED;
		else
			ptpPrivileges->Privileges[dwIndex].Attributes &= ~SE_PRIVILEGE_ENABLED;
	}

	// Adjust the privileges for the token
	if (!AdjustTokenPrivileges(g_hToken, FALSE, ptpPrivileges, 0, NULL, NULL))
		RefreshStatus(TEXT("AdjustTokenPrivileges"), GetLastError());
	else
		guiDumpToken();   // Display the new token
}


void Cmd_SetDACL(HWND hwnd) 
{
	PSECURITY_DESCRIPTOR pSD  = NULL;
	CSecurityPage*       pSec = NULL;

	PTSTR szStatus = TEXT("Default DACL Set");

	try {{

		// No token?  Outta here
		if (g_hToken == NULL) {
			szStatus = TEXT("No Token");
			goto leave;
		}

		// Get the default dacl for the token
		PTOKEN_DEFAULT_DACL ptdDacl = (PTOKEN_DEFAULT_DACL) myAllocateTokenInfo(
			g_hToken, TokenDefaultDacl);
		if (ptdDacl == NULL) {
			szStatus = TEXT("Unable to get default DACL");
			SetLastError(0);
			goto leave;
		}

		vaDbgTs(_T("In Cmd_SetDACL(), dumping the ACL from TokenDefaultDacl:"));
		CH10_DumpACL(ptdDacl->DefaultDacl, InterpretRights_Generic, nullptr); 

		// Allocate memory for an SD
		pSD = (PSECURITY_DESCRIPTOR) GlobalAlloc(GPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
		if (pSD == NULL) {
			szStatus = TEXT("GlobalAlloc");
			goto leave;
		}

		// Initialize it
		if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION)) {
			szStatus = TEXT("InitializeSecurityDescriptor");
			goto leave;
		}

		// Set the security descriptor DACL to the default DACL of the token
		if (!SetSecurityDescriptorDacl(pSD, TRUE, ptdDacl->DefaultDacl, FALSE)) {
			szStatus = TEXT("SetSecurityDescriptorDacl");
			goto leave;
		}

		// Create an instance of the CSecurityPage object with our SD.
		// This is derived from the ISecurityInformation interface defined by
		// the SDK.  It will be passed to the EditSecurity function to produce
		// the common ACL editor Dialog Box.
		pSec = new CSecurityPage(pSD);
		if (pSec == NULL)
			goto leave;

		// Common dialog box for ACL editing
		if (EditSecurity(hwnd, pSec) && pSec->IsModified())
			guiDumpToken(); // If success, then re-display

		SetLastError(0);

	} leave:;
	} catch(...) {}

	// Cleanup
	RefreshStatus(szStatus, GetLastError());

	if (pSD != NULL)
		GlobalFree(pSD);

	if (pSec != NULL)
		pSec->Release();
}


void Cmd_Browse(HWND hwnd) 
{
	// Get current filename text
	TCHAR szFileBuf[MAX_PATH] = {};
	GetDlgItemText(hwnd, IDE_FILENAME, szFileBuf, ARRAYSIZE(szFileBuf));

	// Use common dialog to fetch a filename
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner   = hwnd;
	ofn.lpstrFilter = TEXT("Executables\000*.exe\000All Files\000*.*\000");
	ofn.lpstrFile   = szFileBuf;
	ofn.nMaxFile    = ARRAYSIZE(szFileBuf);
	ofn.lpstrTitle  = TEXT("Select a File Launchable by CreateProcessAsUser");
	ofn.Flags       = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NONETWORKBUTTON
		| OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt = TEXT("EXE");
	
	if (GetOpenFileName(&ofn))
		SetDlgItemText(hwnd, IDE_FILENAME, szFileBuf);
}


void Cmd_CreateProcessAsUser(HWND hwnd) 
{
	// No token?  Adios! (Spanish Bye)
	if (g_hToken == NULL) {
		RefreshStatus(TEXT("No Token"), 0);
		return;
	}

	// Get current filename text
	TCHAR szFileBuf[MAX_PATH] = {};
	GetDlgItemText(hwnd, IDE_FILENAME, szFileBuf, chDIMOF(szFileBuf));

	STARTUPINFO si = {sizeof(si)};
	PROCESS_INFORMATION pi = {};

	// Create a new process with our current token
	if (CreateProcessAsUser(g_hToken, NULL, szFileBuf, 
		NULL, NULL, // process and thread secu-attr
		FALSE, // bInheritHandles
		0,     // dwCreationFlags
		NULL, NULL, &si, &pi)) 
	{
		DWORD msec_start = TrueGetMillisec();

		vaPrintStatus(_T("CreateProcessAsUser() success, child PID=%d."), pi.hProcess);

		// [2025-03-29] v1.4.1: Wait 1 second to see if the child process runs to end.
		// If so, report child process's exit code.
		// If we pass a narrow-sense restricted-token to CreateProcessAsUser(), trying
		// to spawn a C:\Windows\System\CMD.EXE process, we bump into this situation.

		DWORD waitret = WaitForSingleObject(pi.hProcess, 1000);
		if(waitret==WAIT_OBJECT_0)
		{
			DWORD exitcode = 0;
			GetExitCodeProcess(pi.hProcess, &exitcode);

			DWORD msec_end = TrueGetMillisec();
			int msec_used = msec_end - msec_start;
			vaPrintStatus(
				_T("Child process creation success, but exits in less than %d milliseconds. ")
				_T("Child process exit code is %d (0x%X).")
				, msec_used, exitcode, exitcode);
		}

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	} 
	else 
	{
		RefreshStatus(TEXT("CreateProcessAsUser"), GetLastError());
	}
}


void Cmd_AddRestricted(HWND hwnd) 
{
	// Add the new name to the restricted sids list control
	TCHAR szName[1024];
	if (g_hToken && GetAccountName(hwnd, szName, chDIMOF(szName), TRUE, TRUE))
	{
		ListBox_AddString(g_hwndRestrictedSids, szName);
	}
}

void Cmd_RemoveRestricted() 
{
	int nIndex = ListBox_GetCurSel(g_hwndRestrictedSids);
	if (nIndex != LB_ERR)
		ListBox_DeleteString(g_hwndRestrictedSids, nIndex);
}


void Cmd_SetOwnerGroup(HWND hwnd, BOOL fOwner) 
{
	PTSTR szStatus = TEXT("Item Set");
	PSID  psid     = NULL;

	try {{

		// No token?  Scram!
		if (g_hToken == NULL) {
			szStatus = TEXT("No Token");
			SetLastError(0);
			goto leave;
		}

		// Get an account name
		TCHAR szName[1024];
		if (!GetAccountName(hwnd, szName, chDIMOF(szName), TRUE, TRUE))  {
			szStatus = TEXT("User Cancelled Item Selection");
			SetLastError(0);
			goto leave;
		}

		// Get the Sid for the name
		DWORD dwSize = 0;
		TCHAR szDomainName[1024];
		DWORD dwDomSize = chDIMOF(szDomainName);
		SID_NAME_USE sidUse;
		LookupAccountName(NULL, szName, NULL, &dwSize, szDomainName, &dwDomSize,
			&sidUse);

		psid = (PSID) GlobalAlloc(GPTR, dwSize);
		if (psid == NULL) {
			szStatus = TEXT("GlobalAlloc");
			goto leave;
		}

		if (!LookupAccountName(NULL, szName, psid, &dwSize, szDomainName,
			&dwDomSize, &sidUse)) {
				szStatus = TEXT("LookupAccountName");
				goto leave;
		}

		// Set the token information using the TOKEN_OWNER structure,
		TOKEN_OWNER to;
		to.Owner = psid;
		if (!SetTokenInformation(g_hToken, fOwner ? TokenOwner :
			TokenPrimaryGroup, &to, sizeof(to))) {
				szStatus = TEXT("SetTokenInformation");
				goto leave;
		}

		guiDumpToken();

		SetLastError(0);

	} leave:;
	} catch(...) {}

	// Cleanup
	RefreshStatus(szStatus, GetLastError());

	if (psid != NULL)
		GlobalFree(psid);
}


void Cmd_CreateRestrictedToken() 
{
	PSID_AND_ATTRIBUTES  psidToDisableAttrib  = NULL;
	PLUID_AND_ATTRIBUTES pluidPrivAttribs     = NULL;
	PSID_AND_ATTRIBUTES  psidToRestrictAttrib = NULL;

	DWORD dwIndex = 0;
	DWORD DisableCount  = 0;
	DWORD RestrictCount = 0;

	PTSTR szStatus = TEXT("Restricted Token Created");

	try {{

		// No Token?  Gotta run...
		if (g_hToken == NULL) {
			szStatus = TEXT("No Token");
			SetLastError(0);
			goto leave;
		}

		// How big of a structure do I allocate for restricted SIDs
		RestrictCount = ListBox_GetCount(g_hwndRestrictedSids);
		psidToRestrictAttrib = (PSID_AND_ATTRIBUTES) LocalAlloc(LPTR,
			RestrictCount * sizeof(SID_AND_ATTRIBUTES));
		if (psidToRestrictAttrib == NULL) {
			szStatus = TEXT("LocalAlloc");
			goto leave;
		}

		ZeroMemory(psidToRestrictAttrib, RestrictCount
			* sizeof(SID_AND_ATTRIBUTES));

		DWORD dwData;
		DWORD dwSidSize;
		DWORD dwDomainSize;
		TCHAR szBuffer[1024];
		TCHAR szDomain[1024];
		PSID  pSid;

		SID_NAME_USE sidNameUse;

		// For each sid, we find the sid and add it to our array
		for (dwIndex = 0; dwIndex < RestrictCount; dwIndex++) {

			dwData = ListBox_GetText(g_hwndRestrictedSids, dwIndex, szBuffer);
			if (dwData == LB_ERR) {
				dwIndex--;
				break;
			}

			dwSidSize = 0;
			dwDomainSize = chDIMOF(szDomain);

			// Size of SID?
			LookupAccountName(NULL, szBuffer, NULL, &dwSidSize, szDomain,
				&dwDomainSize, &sidNameUse);
			pSid = LocalAlloc(LPTR, dwSidSize);
			if (pSid == NULL) {

				szStatus = TEXT("LocalAlloc");
				goto leave;
			}

			// Get the SID
			if (!LookupAccountName(NULL, szBuffer, pSid, &dwSidSize, szDomain,
				&dwDomainSize, &sidNameUse)) {

					szStatus = TEXT("LookupAccountName");
					goto leave;
			}

			psidToRestrictAttrib[dwIndex].Sid = pSid;
			psidToRestrictAttrib[dwIndex].Attributes = 0;
		}
		RestrictCount = dwIndex;

		// How much memory do we need for our disabled SIDS?
		DisableCount = ListBox_GetCount(g_hwndDisabledSids);
		psidToDisableAttrib = (PSID_AND_ATTRIBUTES)
			LocalAlloc(LPTR, DisableCount * sizeof(SID_AND_ATTRIBUTES));     
		if (psidToDisableAttrib == NULL) {

			szStatus = TEXT("LocalAlloc");
			goto leave;
		}
		ZeroMemory(psidToDisableAttrib,
			DisableCount * sizeof(SID_AND_ATTRIBUTES));

		DWORD dwEnd = DisableCount;
		DisableCount = 0;

		// For each one, add it to our array
		for (dwIndex = 0; dwIndex < dwEnd; dwIndex++) 
		{
			if (ListBox_GetSel(g_hwndDisabledSids, dwIndex)) {

				dwData = ListBox_GetText(g_hwndDisabledSids, dwIndex, szBuffer);
				if (dwData == LB_ERR) {

					dwIndex--;
					break;
				}

				dwSidSize = 0;
				dwDomainSize = chDIMOF(szDomain);

				// SID size?
				LookupAccountName(NULL, szBuffer, NULL, &dwSidSize, szDomain,
					&dwDomainSize, &sidNameUse);
				pSid = LocalAlloc(LPTR, dwSidSize);
				if (pSid == NULL) {

					szStatus = TEXT("LocalAlloc");
					goto leave;
				}
				// Get the SID
				if (!LookupAccountName(NULL, szBuffer, pSid, &dwSidSize,
					szDomain, &dwDomainSize, &sidNameUse)) {

						szStatus = TEXT("LookupAccountName");
						goto leave;
				}

				psidToDisableAttrib[DisableCount].Sid = pSid;
				psidToDisableAttrib[DisableCount].Attributes = 0;
				DisableCount++;
			}
		}

		// Now we find out how much memory we need for our privileges
		DWORD dwPrivSize = ListBox_GetCount(g_hwndDeletedPrivileges);
		pluidPrivAttribs = (PLUID_AND_ATTRIBUTES) LocalAlloc(LPTR, dwPrivSize
			* sizeof(LUID_AND_ATTRIBUTES));
		if (pluidPrivAttribs == NULL) {
			szStatus = TEXT("LocalAlloc");
			goto leave;
		}
		ZeroMemory(pluidPrivAttribs, dwPrivSize * sizeof(LUID_AND_ATTRIBUTES));

		// Add the privileges that are to be removed from the token to the list
		dwEnd = dwPrivSize;
		dwPrivSize = 0;
		for (dwIndex = 0; dwIndex < dwEnd; dwIndex++) {

			if (ListBox_GetSel(g_hwndDeletedPrivileges, dwIndex)) {

				dwData = ListBox_GetText(g_hwndDeletedPrivileges, dwIndex, szBuffer);
				if (dwData == LB_ERR) {
					dwIndex--;
					break;
				}

				LUID luid;
				if (!LookupPrivilegeValue(NULL, szBuffer, &luid)) {
					szStatus = TEXT("LookupPrivilegeValue");
					goto leave;
				}

				pluidPrivAttribs[dwPrivSize].Luid = luid;
				pluidPrivAttribs[dwPrivSize].Attributes = 0;
				dwPrivSize++;
			}
		}

		// Attempt to create restricted token with the structures we built
		HANDLE hNewToken = NULL;
		if (!CreateRestrictedToken(g_hToken, 0, DisableCount,
			psidToDisableAttrib, dwPrivSize, pluidPrivAttribs, RestrictCount,
			psidToRestrictAttrib, &hNewToken)) {
				szStatus = TEXT("CreateRestrictedToken");
				goto leave;
		}

		// Close out our old token
		CloseHandle(g_hToken);

		// This is our new token
		g_hToken = hNewToken;

		// Display it
		guiDumpToken();

		SetLastError(0);

	} leave:;
	} catch(...) {}

	RefreshStatus(szStatus, GetLastError());

	// We have to loop to remove all of those sids we allocated
	if (psidToDisableAttrib != NULL) {
		for (dwIndex = 0; dwIndex < DisableCount; dwIndex++)
			if (psidToDisableAttrib[dwIndex].Sid != NULL)
				LocalFree(psidToDisableAttrib[dwIndex].Sid);
		LocalFree(psidToDisableAttrib);
	}

	// The privileges we can just free
	if (pluidPrivAttribs != NULL)
		LocalFree(pluidPrivAttribs);

	// More looping to free up sids we allocated
	if (psidToRestrictAttrib != NULL) {
		for (dwIndex = 0; dwIndex < RestrictCount; dwIndex++)
			if (psidToRestrictAttrib[dwIndex].Sid != NULL)
				LocalFree(psidToRestrictAttrib[dwIndex].Sid);
		LocalFree(psidToRestrictAttrib);
	}
}

static void try_OpenFile(const TCHAR *filepath)
{
	HANDLE hfile = CreateFile(filepath, 
		FILE_GENERIC_READ|FILE_GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL, // secu-attr
		OPEN_ALWAYS, // we want to open existing file
		0,
		NULL);
	CEC_FILEHANDLE cec_hfile = hfile;
	if(hfile==INVALID_HANDLE_VALUE)
	{
		vaDbgTs(_T("CreateFile(\"%s\",... OPEN_ALWAYS ...) fail with winerr=%s"), 
			filepath, ITCS_WinError);
		return;
	}

	vaDbgTs(_T("CreateFile(\"%s\",... OPEN_ALWAYS ...) success."), filepath);
}

static unsigned __stdcall _thread_TestToken(void *ctx) 
{
	BOOL succ = ImpersonateLoggedOnUser(g_hToken);
	if(!succ) 
	{
		vaDbgTs(_T("ImpersonateLoggedOnUser(0x%X) fail with winerr=%s"), (UINT)g_hToken, ITCS_WinError);
		return 0x44;
	}

	vaDbgTs(_T("ImpersonateLoggedOnUser(0x%X) success"), (UINT)g_hToken);

	try_OpenFile(_T("D:\\_testPSSA2000\\fileA.txt"));
	try_OpenFile(_T("D:\\_testPSSA2000\\fileB.txt"));
	try_OpenFile(_T("D:\\_testPSSA2000\\fileC.txt"));

	try_OpenFile(_T("D:\\_testPSSA2000\\only-grpBar.txt"));
	try_OpenFile(_T("D:\\_testPSSA2000\\Users-and-grpBar.txt"));
	try_OpenFile(_T("D:\\_testPSSA2000\\Users-deny-grpBar.txt"));

	RevertToSelf();

	return 0;
}

void Cmd_TestThreadToken() // [2025-03-29] Chj, since 1.4.0
{
	if (g_hToken == NULL) {
		vaMsgBox(NULL, MB_OK|MB_ICONWARNING, NULL, _T("No Token yet, nothing to do"));
		return;
	}

	HANDLE hThread = chBEGINTHREADEX(NULL, 0, 
		_thread_TestToken, nullptr,
		0, NULL);

	if(!hThread) {
		vaMsgBox(NULL, MB_OK|MB_ICONERROR, NULL,
			_T("Creating thread fail, WinErr=%s."), ITCS_WinError);
		return;
	}

	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);

	vaMsgBox(NULL, MB_OK, NULL, 
		_T("Cmd_TestThreadToken() done.\r\n")
		_T("Use DbgView to see what has happened. ")
		);
}


void Dlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) 
{

	// Special handling for focus notifications
	switch (codeNotify) {

	case EN_SETFOCUS:
	case CBN_SETFOCUS:
		SmartDefaultButton(hwnd, id);
		break;

	case EN_KILLFOCUS:
	case CBN_KILLFOCUS:
		SetDlgDefaultButton(hwnd, IDB_NODEFAULT);
		break;
	}

	// Forward WM_COMMAND messages to specific Cmd_ functions
	switch (id) {

	case IDC_PROCESSES:
		Cmd_Processes(codeNotify);
		break;

	case IDC_THREADS:
		Cmd_Threads(hwnd, codeNotify);
		break;

	case IDB_DUMPTOKEN:
		Cmd_DumpToken(hwnd);
		break;

	case IDB_ADJUSTTOKENPRIVILEGES:
		Cmd_AdjustTokenPrivileges();
		break;

	case IDB_ADJUSTTOKENGROUPS:
		Cmd_AdjustGroups();
		break;

	case IDB_SETDACL:
		Cmd_SetDACL(hwnd);
		break;

	case IDB_BROWSE:
		Cmd_Browse(hwnd);
		break;

	case IDB_CREATEPROCESS:
		Cmd_CreateProcessAsUser(hwnd);
		break;

	case IDB_ADDRESTRICTED:
		Cmd_AddRestricted(hwnd);
		break;

	case IDB_REMOVERESTRICTED:
		Cmd_RemoveRestricted();
		break;

	case IDB_SETOWNER:
		Cmd_SetOwnerGroup(hwnd, TRUE);
		break;

	case IDB_SETGROUP:
		Cmd_SetOwnerGroup(hwnd, FALSE);
		break;

	case IDB_CREATERESTRICTEDTOKEN:
		Cmd_CreateRestrictedToken();
		break;

	case IDB_TestThreadToken:
		Cmd_TestThreadToken();
		break;

	case IDB_LOGONUSER:
		Cmd_LogonUser(hwnd);
		break;

	case IDB_DUPLICATE:
		Cmd_DuplicateToken(hwnd);
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////

void Dlg_OnDestroy(HWND hwnd) {

	// If we have a process snapshot... Close it up
	if (g_hSnapShot != NULL)
		CloseHandle(g_hSnapShot);
}

static void Dlg_EnableJULayout(HWND hdlg)
{
	JULayout *jul = JULayout::EnableJULayout(hdlg);

	const int x30=30, x45=45, x60=60, x80=80, x100=100;
	const int y50=50, y70=60, y85=75, y100=100;

	//// Right-side of GUI ////

	// Create a process with the current token (groupbox area)
	jul->AnchorControl(x60,0, x100,0, IDS_CREATEPROCESS);
	jul->AnchorControl(x60,0, x100,0, IDE_FILENAME);
	jul->AnchorControl(x100,0, x100,0, IDB_BROWSE);
	jul->AnchorControl(x60,0, x60,0, IDB_CREATEPROCESS);

	// Enable or Disable token groups (groupbox area)
	jul->AnchorControls(x60,0, x100,y50, IDS_ADJUSTGROUPS, IDL_ENABLEGROUPS, -1);
	jul->AnchorControl(x80,y50, x80,y50, IDB_ADJUSTTOKENGROUPS);

	// Enable or Disable token privileges (groupbox area)
	jul->AnchorControls(x60,y50, x100,y100, IDS_ADJUSTPRIV, IDL_ENABLEPRIVILEGES, -1);
	jul->AnchorControl(x80,y100, x80,y100, IDB_ADJUSTTOKENPRIVILEGES);

	// SetTokenInformation buttons (groupbox area)
	jul->AnchorControl(x60,y100, x100,y100, IDS_SETTOKENINFORMATION);
	jul->AnchorControls(x80,y100, x80,y100, IDB_SETOWNER, IDB_SETGROUP, IDB_SETDACL, -1);

	//// Left-side of GUI ////

	jul->AnchorControl(0,0, x60,0, IDE_STATUS);  // Status readonly editbox
	jul->AnchorControl(0,0, x60,y70, IDE_TOKEN); // Token-info readonly editbox

	// Create a restricted token (groupbox area)
	jul->AnchorControl(0,y70, x60,y100, IDS_CREATEARESTRICTEDTOKEN); // the outter groupbox
	//
	jul->AnchorControl(0,y70, x30,y70, IDS_DELETEDPRIVILEGES);
	jul->AnchorControl(0,y70, x30,y85, IDL_DELETEDPRIVILEGES);
	//
	jul->AnchorControl(0,y85, x30,y85,  IDS_DISABLEDSIDS);
	jul->AnchorControl(0,y85, x30,y100, IDL_DISABLEDSIDS);
	//
	jul->AnchorControls(x30,y70, x60,y100, IDS_ADDREMOVERESTRICTED, IDL_RESTRICTEDSIDS, -1);
	jul->AnchorControls(x45,y100, x45,y100, IDB_ADDRESTRICTED, IDB_REMOVERESTRICTED, IDB_CREATERESTRICTEDTOKEN, 
		IDB_TestThreadToken ,-1);
}


BOOL Dlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) 
{
	chSETDLGICONS(hwnd, IDI_TOKENMASTER);

	Dlg_EnableJULayout(hwnd);
	Dlgbox_EnableComboboxWideDrop(hwnd);

	// Store some window handles
	g_hwndProcessCombo = GetDlgItem(hwnd, IDC_PROCESSES);
	g_hwndThreadCombo = GetDlgItem(hwnd, IDC_THREADS);
	g_hwndToken = GetDlgItem(hwnd, IDE_TOKEN);
	g_hwndStatus = GetDlgItem(hwnd, IDE_STATUS);
	g_hwndEnablePrivileges = GetDlgItem(hwnd, IDL_ENABLEPRIVILEGES);
	g_hwndEnableGroups = GetDlgItem(hwnd, IDL_ENABLEGROUPS);
	g_hwndDeletedPrivileges = GetDlgItem(hwnd, IDL_DELETEDPRIVILEGES);
	g_hwndDisabledSids = GetDlgItem(hwnd, IDL_DISABLEDSIDS);
	g_hwndRestrictedSids = GetDlgItem(hwnd, IDL_RESTRICTEDSIDS);
	g_hwndLogonTypes = GetDlgItem(hwnd, IDC_LOGONTYPE);
	g_hwndLogonProviders = GetDlgItem(hwnd, IDC_LOGONPROVIDER);
	g_hwndImpersonationLevels = GetDlgItem(hwnd, IDC_IMPERSONATIONLEVEL);
	g_hwndTokenTypes = GetDlgItem(hwnd, IDC_TOKENTYPE);

	SetDlgItemText(hwnd, IDE_FILENAME, _T("C:\\Windows\\System32\\cmd.exe"));

	// Refresh the Process snapshot and combo boxes
	RefreshSnapShot();
	guiPopulateProcessCombo();
	guiPopulateStaticCombos();

	// Status... Who is this program running as?
	TCHAR szUserName[1024] = {}, szStatus[1024] = {};
	DWORD nUserName = ARRAYSIZE(szUserName);
	GetUserName(szUserName, &nUserName);

	_sntprintf_s(szStatus, _TRUNCATE, TEXT("Token Master running as %s"), szUserName);
	RefreshStatus(szStatus, NOERROR);

	// Prepare dialogbox window title (will show whether RunAsAdmin).
	//
	TCHAR *pszRunAs = _T("");
	if( _tcscmp(szUserName, _T("SYSTEM"))==0 )
		pszRunAs = _T("(Run as SYSTEM)");
	else if(IsUserAnAdmin())
		pszRunAs = _T("(RunAsAdmin)");
	//
	vaSetWindowText(hwnd, _T("TokenMaster v%s %s"), _T(EXE_VERSION), pszRunAs);

	return(TRUE);
}

void Dlg_OnClose(HWND hwnd) {

	EndDialog(hwnd, 0);
}

INT_PTR WINAPI Dlg_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{

	switch (uMsg) {
		HANDLE_MSG(hwnd, WM_INITDIALOG, Dlg_OnInitDialog);
		HANDLE_MSG(hwnd, WM_CLOSE, Dlg_OnClose);
		HANDLE_MSG(hwnd, WM_COMMAND, Dlg_OnCommand);
		HANDLE_MSG(hwnd, WM_DESTROY, Dlg_OnDestroy);
	}

	return(FALSE);
}

///////////////////////////////////////////////////////////////////////////////

int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR pszCmdLine, int) 
{

	TCHAR szUserName[256] = { 0 };
	DWORD dwSize = chDIMOF(szUserName);

	GetUserName(szUserName, &dwSize);

	try {{  // Jeffrey-style dual-wrap try/leave/catch

		if ((lstrcmpi(szUserName, TEXT("System")) != 0)
			&& (lstrcmpi(pszCmdLine, TEXT("NoRelaunch")) != 0))
		{
			if (TryRelaunch())
				goto leave;
		}

		g_hInst = hinstExe;
		DialogBox(hinstExe, MAKEINTRESOURCE(IDD_DUMPTOKEN), NULL, Dlg_Proc);

	} leave:;
	} catch(...) {}

	return(0);
}

///////////////////////////////// End of File /////////////////////////////////
