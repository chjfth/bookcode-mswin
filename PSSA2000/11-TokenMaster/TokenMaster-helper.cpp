#include "shareinc.h"
#include "TokenMaster-helper.h"
#include "../chjutils/ch10-DumpSD.h"
#include "../chjutils/chjutils.h"

#include <vaDbg.h>

BOOL myEnablePrivilege(PTSTR szPriv, BOOL fEnabled) 
{
	HANDLE hToken   = NULL;
	BOOL   fSuccess = FALSE;

	try {{

		// First lookup the system unique luid for the privilege
		LUID luid = {};
		if (!LookupPrivilegeValue(NULL, szPriv, &luid)) {

			// If the name is bogus...
			goto leave;
		}

		// Then get the processes token
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) 
		{
			goto leave;
		}

		// Set up our token privileges "array" (in our case an array of one)
		TOKEN_PRIVILEGES tp = {};
		tp.PrivilegeCount           = 1;
		tp.Privileges[0].Luid       = luid;
		tp.Privileges[0].Attributes = fEnabled ? SE_PRIVILEGE_ENABLED : 0;

		// Adjust our token privileges by enabling or disabling this one
		if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) 
		{
			goto leave;
		}

		fSuccess = TRUE;

	} leave:;
	} catch(...) {}

	// Cleanup
	if (hToken != NULL)
		CloseHandle(hToken);

	return(fSuccess);
}


///////////////////////////////////////////////////////////////////////////////


HANDLE myOpenSystemProcess() 
{
	HANDLE hSnapshot = NULL;
	HANDLE hProc     = NULL;

	try {{

		// Get a snapshot of the processes in the system
		hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hSnapshot == NULL)
			goto leave;

		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(pe32);

		// Find the "System" process
		BOOL fProcess = Process32First(hSnapshot, &pe32);

		while (fProcess && (lstrcmpi(pe32.szExeFile, TEXT("SYSTEM")) != 0))
			fProcess = Process32Next(hSnapshot, &pe32);

		if (!fProcess)
			goto leave;    // Didn't find "System" process

		// Open the process with PROCESS_QUERY_INFORMATION access
		hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
		if (hProc == NULL)
			goto leave;

	} leave:;
	} catch(...) {}

	// Cleanup the snapshot
	if (hSnapshot != NULL)
		CloseHandle(hSnapshot);

	return(hProc);
}


///////////////////////////////////////////////////////////////////////////////


BOOL myModifySecurity(HANDLE hKobj, DWORD dwAccess, 
	FUNC_InterpretRights procItr, void *itrctx) 
{
	// chjmemo: Modify hKobj's SD, so that I(current user) have dwAccess for that hKobj.

	PACL pAcl        = NULL;
	PACL pNewAcl     = NULL;
	PACL pSacl       = NULL;
	PSID pSidOwner   = NULL;
	PSID pSidPrimary = NULL;
	BOOL fSuccess    = TRUE;

	PSECURITY_DESCRIPTOR pSD = NULL;

	vaDbgTs(_T("==== Entering myModifySecurity(), hKobj=0x%08X, dwAccess=0x%X."), (int)hKobj, dwAccess);

	try {{

		// Find the length of the security object for the kernel object
		DWORD dwSDLength;
		if (GetKernelObjectSecurity(hKobj, DACL_SECURITY_INFORMATION, pSD, 0, &dwSDLength) 
			|| (GetLastError()!=ERROR_INSUFFICIENT_BUFFER))
			goto leave;

		// Allocate a buffer of that length
		pSD = LocalAlloc(LPTR, dwSDLength);
		if (pSD == NULL)
			goto leave;

		// Retrieve the kernel object
		if (!GetKernelObjectSecurity(hKobj, DACL_SECURITY_INFORMATION, pSD, dwSDLength, &dwSDLength))
			goto leave;

		// Get a pointer to the DACL of the SD
		BOOL fDaclPresent;
		BOOL fDaclDefaulted;
		if (!GetSecurityDescriptorDacl(pSD, &fDaclPresent, &pAcl,
			&fDaclDefaulted))
			goto leave;

		vaDbgTs(_T("==== Starting SD dump:"));
		CH10_DumpSD(pSD, procItr, itrctx);

		// Get the current user's name
		TCHAR szName[1024];
		DWORD dwLen = chDIMOF(szName);
		if (!GetUserName(szName, &dwLen))
			goto leave;

		// Build an EXPLICIT_ACCESS structure for the ace we wish to add.
		EXPLICIT_ACCESS ea = {};
		BuildExplicitAccessWithName(&ea, szName, dwAccess, GRANT_ACCESS, 0);
		ea.Trustee.TrusteeType = TRUSTEE_IS_USER;

		// We are allocating a new ACL with a new ace inserted.  The new
		// ACL must be LocalFree'd
		if (ERROR_SUCCESS != SetEntriesInAcl(1, &ea, pAcl, &pNewAcl)) 
		{
			pNewAcl = NULL;
			goto leave;
		}

		// Find the buffer sizes we would need to make our SD absolute
		pAcl               = NULL;
		dwSDLength         = 0;
		DWORD dwAclSize    = 0;
		DWORD dwSaclSize   = 0;
		DWORD dwSidOwnLen  = 0;
		DWORD dwSidPrimLen = 0;
		PSECURITY_DESCRIPTOR pAbsSD = NULL;
		if (MakeAbsoluteSD(pSD, pAbsSD, &dwSDLength, pAcl, &dwAclSize, pSacl,
			&dwSaclSize, pSidOwner, &dwSidOwnLen, pSidPrimary, &dwSidPrimLen)
			|| (GetLastError() != ERROR_INSUFFICIENT_BUFFER))
			goto leave;

		// Allocate the buffers
		pAcl = (PACL) LocalAlloc(LPTR, dwAclSize);
		pSacl = (PACL) LocalAlloc(LPTR, dwSaclSize);
		pSidOwner = (PSID) LocalAlloc(LPTR, dwSidOwnLen);
		pSidPrimary = (PSID) LocalAlloc(LPTR, dwSidPrimLen);
		pAbsSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR, dwSDLength);
		if (!(pAcl && pSacl && pSidOwner && pSidPrimary && pAbsSD))
			goto leave;

		// And actually make our SD absolute
		if (!MakeAbsoluteSD(pSD, pAbsSD, &dwSDLength, pAcl, &dwAclSize, pSacl,
			&dwSaclSize, pSidOwner, &dwSidOwnLen, pSidPrimary, &dwSidPrimLen))
			goto leave;

		// Now set the security descriptor DACL
		if (!SetSecurityDescriptorDacl(pAbsSD, fDaclPresent, pNewAcl, fDaclDefaulted))
			goto leave;

		// And set the security for the object
		if (!SetKernelObjectSecurity(hKobj, DACL_SECURITY_INFORMATION, pAbsSD))
			goto leave;

		fSuccess = TRUE;

		vaDbgTs(_T("==== Leaving myModifySecurity(), hKobj=0x%08X, dwAccess=0x%X."), (int)hKobj, dwAccess);
		vaDbgTs(_T("==== Ending  SD dump:"));
		CH10_DumpSD(pAbsSD, procItr, itrctx);

	} leave:;
	} catch(...) {}


	// Cleanup
	if (pNewAcl)
		LocalFree(pNewAcl);

	if (pSD)
		LocalFree(pSD);

	if (pAcl)
		LocalFree(pAcl);

	if (pSacl)
		LocalFree(pSacl);

	if (pSidOwner)
		LocalFree(pSidOwner);

	if (pSidPrimary)
		LocalFree(pSidPrimary);

	return(fSuccess);
}


///////////////////////////////////////////////////////////////////////////////


HANDLE myGetLSAToken() 
{
	// chjmemo: Get Local-System Account's token

	HANDLE hProcSys  = NULL;
	HANDLE hToken = NULL;

	try {{

		// Enable the SE_DEBUG_NAME privilege in our process token
		if (!myEnablePrivilege(SE_DEBUG_NAME, TRUE))
			goto leave;

		// Retrieve a handle to the "System" process
		hProcSys = myOpenSystemProcess();
		if (hProcSys == NULL)
			goto leave;

		// Open the process token with READ_CONTROL and WRITE_DAC access.  We
		// will use this access to modify the security of the token so that we
		// retrieve it again with a more complete set of rights.
		BOOL fResult = OpenProcessToken(hProcSys, READ_CONTROL | WRITE_DAC, &hToken);
		if (FALSE == fResult)  {
			hToken = NULL;
			goto leave;
		}

		// Add an ACE for the current user for the token.  This ACE will add
		// TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_QUERY rights.
		if (! myModifySecurity(hToken, TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_QUERY,
				InterpretRights_Token, nullptr)) 
		{
			CloseHandle(hToken);
			hToken = NULL;
			goto leave;
		}

		CloseHandle(hToken); // Fix Jeffrey's bug: we should close the old token handle.
		hToken = nullptr;

		// Reopen the process token now that we have added the rights to
		// query the token, duplicate it, and assign it.
		fResult = OpenProcessToken(hProcSys, 
			TOKEN_QUERY | TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY, 
			&hToken);
		if (FALSE == fResult)  {
			hToken = NULL;
			goto leave;
		}

	} leave:;
	} catch(...) {}

	// Close the System process handle
	if (hProcSys != NULL)
		CloseHandle(hProcSys);

	return(hToken);
}


BOOL myRunAsUser(PTSTR pszEXE, PTSTR pszUserName, PTSTR pszPassword, PTSTR pszDesktop) 
{
	HANDLE hToken   = NULL;
	BOOL   fProcess = FALSE;
	BOOL   fSuccess = FALSE;

	PROCESS_INFORMATION pi = {NULL, NULL, 0, 0};

	try {{

		if (pszUserName == NULL) {

			hToken = myGetLSAToken();
			if (hToken == NULL)
				goto leave;

		} else {

			if (!LogonUser(pszUserName, NULL, pszPassword,
				LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, &hToken))
				goto leave;
		}

		STARTUPINFO si;
		si.cb          = sizeof(si);
		si.lpDesktop   = pszDesktop;
		si.lpTitle     = NULL;
		si.dwFlags     = 0;
		si.cbReserved2 = 0;
		si.lpReserved  = NULL;
		si.lpReserved2 = NULL;

		fProcess = CreateProcessAsUser(hToken, NULL, pszEXE, NULL, NULL, FALSE,
			0, NULL, NULL, &si, &pi);
		if (!fProcess)
			goto leave;

		fSuccess = TRUE;

	} leave:;
	} catch(...) {}

	if (hToken != NULL)
		CloseHandle(hToken);

	if (fProcess) {
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	return(fSuccess);
}

BOOL TryRelaunch() 
{
	BOOL fSuccess = FALSE;

	try {{

		TCHAR szFilename[MAX_PATH + 50];
		if (!GetModuleFileName(NULL, szFilename, chDIMOF(szFilename)))
			goto leave;

		fSuccess = myRunAsUser(szFilename, NULL, NULL, TEXT("Winsta0\\Default"));

	} leave:;
	} catch(...) {}

	return(fSuccess);
}


BOOL myGetUserSID(PSID psid, BOOL fAllowImpersonate, PDWORD pdwSize) 
{
	BOOL   fSuccess = FALSE;
	HANDLE hToken   = NULL;

	try {{

		if (fAllowImpersonate)
			if (!OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, TRUE, &hToken))
				hToken = NULL;

		if (hToken == NULL)
			if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
				goto leave;

		DWORD dwSize;
		if (!GetTokenInformation(hToken, TokenUser, psid, *pdwSize, &dwSize))
			goto leave;

		fSuccess = TRUE;

	} leave:;
	} catch(...) {}

	if (hToken != NULL)
		CloseHandle(hToken);

	return(fSuccess);
}


void RefreshStatus(PTSTR szStatus, DWORD dwLastError) 
{
	CPrintBuf* pbufStatus = NULL;

	try {{

		if (szStatus == NULL)
			goto leave;

		// Clear the text
		SetWindowText(g_hwndStatus, TEXT(""));

		// Using the CPrintBuf class to buffer printing to the window
		pbufStatus = new CPrintBuf;
		if (pbufStatus == NULL)
			goto leave;

		pbufStatus->Print(TEXT("Token Master, Status - "));
		pbufStatus->Print(szStatus);

		if (dwLastError != 0)   {
			pbufStatus->Print(TEXT(": "));
			pbufStatus->PrintError(dwLastError);
		}

	} leave:;
	} catch(...) {}

	if (pbufStatus != NULL) {
		SetWindowText(g_hwndStatus, *pbufStatus);
		delete pbufStatus;
	}
}


PVOID myAllocateTokenInfo(HANDLE hToken, TOKEN_INFORMATION_CLASS tokenClass) 
{
	// Caller should LocalFree() the returned pointer.

	PVOID pvBuffer  = NULL;
	PTSTR pszStatus = NULL;
	DWORD dwStatus;

	try {{

		// Initial buffer size
		DWORD dwSize   = 0;
		BOOL  fSuccess = FALSE;
		do {

			// Do we have a size now?
			if (dwSize != 0) {

				// If we already have a buffer, free it
				if (pvBuffer != NULL)
					LocalFree(pvBuffer);

				// Allocate a new buffer
				pvBuffer = LocalAlloc(LPTR, dwSize);
				if (pvBuffer == NULL) {
					pszStatus = TEXT("LocalAlloc");
					goto leave;
				}
			}

			// Try again
			fSuccess = GetTokenInformation(hToken, tokenClass, pvBuffer, dwSize,
				&dwSize);
			// while it is failing on ERROR_INSUFFICIENT_BUFFER
		} while (!fSuccess && (GetLastError() == ERROR_INSUFFICIENT_BUFFER));

		// If we failed for some other reason then back out
		if (!fSuccess) {
			dwStatus = GetLastError();
			pszStatus = TEXT("GetTokenInformation");
			if (pvBuffer != NULL) {
				LocalFree(pvBuffer);
				pvBuffer = NULL;
			}
			SetLastError(dwStatus);
			goto leave;
		}

		SetLastError(0);

	} leave:;
	} catch(...) {}

	dwStatus = GetLastError();
	RefreshStatus(pszStatus, dwStatus);
	SetLastError(dwStatus);

	// Return locally allocated buffer
	return(pvBuffer);
}


///////////////////////////////////////////////////////////////////////////////

void guiUpdatePrivileges() 
{
	PTOKEN_PRIVILEGES ptpPrivileges = NULL;

	PTSTR pszName     = NULL;
	PTSTR pszDispName = NULL;

	// Clear out both list boxes
	ListBox_ResetContent(g_hwndEnablePrivileges);
	ListBox_ResetContent(g_hwndDeletedPrivileges);

	try {{

		// No token?  Then we fly
		if (g_hToken == NULL)
			goto leave;

		// Get that token-privilege information
		ptpPrivileges = (PTOKEN_PRIVILEGES) myAllocateTokenInfo(g_hToken,
			TokenPrivileges);
		if (ptpPrivileges == NULL)
			goto leave;

		// Iterate through the privileges
		DWORD dwIndex;
		for (dwIndex = 0; dwIndex < ptpPrivileges->PrivilegeCount; dwIndex++) 
		{
			// Get size of the name
			DWORD dwSize = 0;
			LookupPrivilegeName(NULL, &(ptpPrivileges->Privileges[dwIndex].Luid),
				pszName, &dwSize);
			pszName = (PTSTR) LocalAlloc(LPTR, dwSize * sizeof(TCHAR));
			if (pszName == NULL)
				goto leave;

			// Get the name itself
			if (!LookupPrivilegeName(NULL,
				&(ptpPrivileges->Privileges[dwIndex].Luid), pszName, &dwSize))
				goto leave;

			// Get the display name size
			DWORD dwDispSize = 0;
			DWORD dwLangID;
			LookupPrivilegeDisplayName(NULL, pszName, NULL, &dwDispSize,
				&dwLangID);
			pszDispName = (PTSTR) LocalAlloc(LPTR, (dwDispSize + dwSize + 3)
				* sizeof(TCHAR));
			if (pszDispName == NULL)
				goto leave;

			// Create the composite string
			lstrcpy(pszDispName, pszName);
			lstrcat(pszDispName, TEXT("--"));

			// Get the display name
			if (!LookupPrivilegeDisplayName(NULL, pszName, pszDispName
				+ lstrlen(pszDispName), &dwDispSize, &dwLangID))
				goto leave;

			// Add the string to the enable/disable privilege list
			DWORD_PTR dwItem = SendMessage(g_hwndEnablePrivileges, LB_ADDSTRING, 0,
				(LPARAM) pszDispName);

			// Now the item data
			SendMessage(g_hwndEnablePrivileges, LB_SETITEMDATA, dwItem,
				(LPARAM) dwIndex);

			// If it is enabled then enable it in the list box
			if (ptpPrivileges->Privileges[dwIndex].Attributes
				& SE_PRIVILEGE_ENABLED)
				SendMessage(g_hwndEnablePrivileges, LB_SETSEL, TRUE, dwItem);

			// Now the deleted privileges
			dwItem = SendMessage(g_hwndDeletedPrivileges, LB_ADDSTRING, 0,
				(LPARAM) pszName);
			SendMessage(g_hwndDeletedPrivileges, LB_SETITEMDATA, dwItem,
				(LPARAM) dwIndex);

			// Free up our recurring buffers
			LocalFree(pszName);
			pszName = NULL;

			LocalFree(pszDispName);
			pszDispName = NULL;
		}

	} leave:;
	} catch(...) {}

	// Cleanup
	if (ptpPrivileges != NULL)
		LocalFree(ptpPrivileges);

	if (pszName != NULL)
		LocalFree(pszName);

	if (pszDispName != NULL)
		LocalFree(pszDispName);
}


///////////////////////////////////////////////////////////////////////////////


void guiUpdateGroups() 
{
	DWORD winerr = 0;
	PTOKEN_GROUPS ptgGroups = NULL;

	// Clear out both list boxes
	ListBox_ResetContent(g_hwndEnableGroups);
	ListBox_ResetContent(g_hwndDisabledSids);

	if (g_hToken == NULL)
		return;

	// Get that group information
	ptgGroups = (PTOKEN_GROUPS) myAllocateTokenInfo(g_hToken, TokenGroups);
	Cec_LocalFree cec_tokengroups = ptgGroups;
	if (ptgGroups == NULL)
		return;

	// Iterate through them
	DWORD dwIndex;
	for (dwIndex = 0; dwIndex < ptgGroups->GroupCount; dwIndex++)
	{
		SID_AND_ATTRIBUTES &sidaa = ptgGroups->Groups[dwIndex];
		SID &sid = *(SID*)(sidaa.Sid);

		TCHAR *sidtext = nullptr;
		ConvertSidToStringSid(&sid, &sidtext);
		Cec_LocalFree cec_sidtext = sidtext;

		SID_NAME_USE sNameUse = SidTypeInvalid; // neg-init
		AutoTCHARs SidName, SidDomain;
		do {
			winerr = LookupAccountSid(NULL, &sid, 
				SidName, SidName,
				SidDomain, SidDomain,
				&sNameUse);
		} while(!winerr && Is_LessBuffer(winerr));

		if(winerr)
		{
			vaDbgTs(
				_T("In guiUpdateGroups(): LookupAccountSid() winerr=%d\r\n")
				_T("    SID to lookup: %s")
				,
				winerr,	sidtext);
			return;
		}

		// Make the composite string
		TCHAR szCompositeName[1024] = {};
		_sntprintf_s(szCompositeName, _TRUNCATE, _T("%s\\%s"), (TCHAR*)SidDomain, (TCHAR*)SidName);

		// If it is neither mandatory nor the logon ID then add it to
		// the enable/disable list box
		DWORD_PTR dwItem = 0;
		if (!((sidaa.Attributes & SE_GROUP_MANDATORY) || (sidaa.Attributes & SE_GROUP_LOGON_ID))) 
		{
			// Add the string to the list box
			dwItem = ListBox_AddString(g_hwndEnableGroups, szCompositeName);
			ListBox_SetItemData(g_hwndEnableGroups, dwItem, dwIndex);
					
			if (ptgGroups->Groups[dwIndex].Attributes & SE_GROUP_ENABLED)
				ListBox_SetSel(g_hwndEnableGroups, TRUE, dwItem);
		}

		// Add to the CreateRestrictedToken list
		dwItem = ListBox_AddString(g_hwndDisabledSids, szCompositeName);
		ListBox_SetItemData(g_hwndDisabledSids, dwItem, dwIndex);

	} // for dwIndex++
}


///////////////////////////////////////////////////////////////////////////////


void RefreshSnapShot() 
{
	// If we already have one, close it
	if (g_hSnapShot != NULL)
		CloseHandle(g_hSnapShot);

	vaDbgTs(_T("RefreshSnapShot()"));

	g_hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD, 0);
}


///////////////////////////////////////////////////////////////////////////////


void guiPopulateProcessCombo() 
{
	// chjmemo: Implicit input: `g_hSnapShot`

	// Get the ID of our last selection so that the selection will "stick"
	LRESULT idxItem = ComboBox_GetCurSel(g_hwndProcessCombo);
	DWORD dwLastUsedPID = (DWORD) ComboBox_GetItemData(g_hwndProcessCombo, idxItem);

	// Clear the combo box
	ComboBox_ResetContent(g_hwndProcessCombo);

	int count = 0;

	// No snapshot means we empty the combo
	if (g_hSnapShot != NULL) 
	{
		// Iterate through the process list adding them to the combo
		PROCESSENTRY32 pentry = { sizeof(pentry) };
		BOOL fIsProcess = Process32First(g_hSnapShot, &pentry);
		while (fIsProcess) 
		{
			count++;

			if (pentry.th32ProcessID != 0)
			{
				TCHAR szitem[200] = {};
				_sntprintf_s(szitem, _TRUNCATE, _T("%s (%d)"), 
					pentry.szExeFile, pentry.th32ProcessID);
				idxItem = ComboBox_AddString(g_hwndProcessCombo, szitem);
			}
			else 
			{
				// Special Case... The Idle Process has a zero ID
				idxItem = ComboBox_AddString(g_hwndProcessCombo, _T("[System Idle Process]"));
				ComboBox_SetCurSel(g_hwndProcessCombo, idxItem);
			}

			// Set the item data to the processes ID
			ComboBox_SetItemData(g_hwndProcessCombo, idxItem, pentry.th32ProcessID);

			// If the process ID matches the last one, we found it
			if (pentry.th32ProcessID == dwLastUsedPID)
				ComboBox_SetCurSel(g_hwndProcessCombo, idxItem);

			fIsProcess = Process32Next(g_hSnapShot, &pentry);
		}

		vaDbgTs(_T("PopulateProcessCombo(): Found %d processes."), count);
	}
}


void guiPopulateThreadCombo() 
{
	// Get process id
	LRESULT idxIndex = ComboBox_GetCurSel(g_hwndProcessCombo);
	DWORD_PTR dwID   = ComboBox_GetItemData(g_hwndProcessCombo, idxIndex);

	// We want the selected thread to stick, if possible
	idxIndex = ComboBox_GetCurSel(g_hwndThreadCombo);
	DWORD_PTR dwLastUsedThreadID = ComboBox_GetItemData(g_hwndThreadCombo, idxIndex);

	ComboBox_ResetContent(g_hwndThreadCombo);

	// Add that "No Thread" option
	idxIndex = ComboBox_AddString(g_hwndThreadCombo, TEXT("[No Thread]"));
	ComboBox_SetItemData(g_hwndThreadCombo, idxIndex, 0);
	ComboBox_SetCurSel(g_hwndThreadCombo, 0);

	if (g_hSnapShot != NULL) 
	{
		// Iterate through the threads adding them
		TCHAR szBuf[256];
		THREADENTRY32 entry = {sizeof(entry)};
		BOOL fIsThread = Thread32First(g_hSnapShot, &entry);

		while (fIsThread) 
		{
			if (entry.th32OwnerProcessID == dwID) 
			{
				_sntprintf_s(szBuf, _TRUNCATE, TEXT("ID = %d"), entry.th32ThreadID);
				idxIndex = ComboBox_AddString(g_hwndThreadCombo, szBuf);
				ComboBox_SetItemData(g_hwndThreadCombo, idxIndex, entry.th32ThreadID);

				// Last thread selected?  If so, reselect
				if (entry.th32ThreadID == dwLastUsedThreadID)
					ComboBox_SetCurSel(g_hwndThreadCombo, idxIndex);
			}
			fIsThread = Thread32Next(g_hSnapShot, &entry);
		}
	}
}


void guiPopulateStaticCombos() 
{
	// LogonUser() dwLogonType: LOGON32_LOGON_xxx

	INT_PTR nIndex = ComboBox_AddString(g_hwndLogonTypes, _T("Batch"));
	ComboBox_SetItemData(g_hwndLogonTypes, nIndex, LOGON32_LOGON_BATCH);

	nIndex = ComboBox_AddString(g_hwndLogonTypes, _T("Network"));
	ComboBox_SetItemData(g_hwndLogonTypes, nIndex, LOGON32_LOGON_NETWORK);

	nIndex = ComboBox_AddString(g_hwndLogonTypes, _T("Network Cleartext"));
	ComboBox_SetItemData(g_hwndLogonTypes, nIndex, LOGON32_LOGON_NETWORK_CLEARTEXT);

	nIndex = ComboBox_AddString(g_hwndLogonTypes, _T("New Credentials"));
	ComboBox_SetItemData(g_hwndLogonTypes, nIndex, LOGON32_LOGON_NEW_CREDENTIALS);

	nIndex = ComboBox_AddString(g_hwndLogonTypes, _T("Service"));
	ComboBox_SetItemData(g_hwndLogonTypes, nIndex, LOGON32_LOGON_SERVICE);

	nIndex = ComboBox_AddString(g_hwndLogonTypes, _T("Unlock"));
	ComboBox_SetItemData(g_hwndLogonTypes, nIndex, LOGON32_LOGON_UNLOCK);

	nIndex = ComboBox_AddString(g_hwndLogonTypes, _T("Interactive"));
	ComboBox_SetItemData(g_hwndLogonTypes, nIndex, LOGON32_LOGON_INTERACTIVE);

	ComboBox_SetCurSel(g_hwndLogonTypes, nIndex);

	// LogonUser() dwLogonProvider: LOGON32_LOGON_xxx 

	nIndex = ComboBox_AddString(g_hwndLogonProviders, _T("Windows 2000"));
	ComboBox_SetItemData(g_hwndLogonProviders, nIndex, LOGON32_PROVIDER_WINNT50);

	nIndex = ComboBox_AddString(g_hwndLogonProviders, _T("Windows NT 4.0"));
	ComboBox_SetItemData(g_hwndLogonProviders, nIndex, LOGON32_PROVIDER_WINNT40);

	nIndex = ComboBox_AddString(g_hwndLogonProviders,  _T("Windows NT 3.5"));
	ComboBox_SetItemData(g_hwndLogonProviders, nIndex, LOGON32_PROVIDER_WINNT35);

	nIndex = ComboBox_AddString(g_hwndLogonProviders, _T("Default"));
	ComboBox_SetItemData(g_hwndLogonProviders, nIndex, LOGON32_PROVIDER_DEFAULT);
	
	ComboBox_SetCurSel(g_hwndLogonProviders, nIndex);

	// DuplicateTokenEx(): SECURITY_IMPERSONATION_LEVEL

	nIndex = ComboBox_AddString(g_hwndImpersonationLevels, _T("SecurityAnonymous"));
	ComboBox_SetItemData(g_hwndImpersonationLevels, nIndex, SecurityAnonymous);

	nIndex = ComboBox_AddString(g_hwndImpersonationLevels, _T("SecurityIdentification"));
	ComboBox_SetItemData(g_hwndImpersonationLevels, nIndex, SecurityIdentification);

	nIndex = ComboBox_AddString(g_hwndImpersonationLevels, _T("SecurityDelegation"));
	ComboBox_SetItemData(g_hwndImpersonationLevels, nIndex, SecurityDelegation);

	nIndex = ComboBox_AddString(g_hwndImpersonationLevels, _T("SecurityImpersonation"));
	ComboBox_SetItemData(g_hwndImpersonationLevels, nIndex, SecurityImpersonation);

	ComboBox_SetCurSel(g_hwndImpersonationLevels, nIndex);

	// DuplicateTokenEx(): TOKEN_TYPE

	nIndex = ComboBox_AddString(g_hwndTokenTypes, _T("Impersonation"));
	ComboBox_SetItemData(g_hwndTokenTypes, nIndex, TokenImpersonation);

	nIndex = ComboBox_AddString(g_hwndTokenTypes, _T("Primary"));
	ComboBox_SetItemData(g_hwndTokenTypes, nIndex, TokenPrimary);
	
	ComboBox_SetCurSel(g_hwndTokenTypes, nIndex);
}


///////////////////////////////////////////////////////////////////////////////


void guiGetToken(HWND hwnd) 
{
	// chjmemo: This function will modify g_hToken.

	DWORD  dwStatus;
	HANDLE hThread    = NULL;
	HANDLE hProcess   = NULL;
	HANDLE hToken     = NULL;
	PTSTR  pszStatus  = TEXT("Dumped Process Token");

	PSECURITY_DESCRIPTOR pSD = NULL;

	if (g_hToken != NULL) {
		CloseHandle(g_hToken);
		g_hToken = NULL;
	}

	try 
	{{
		// Find the process ID
		LRESULT idxIndex = ComboBox_GetCurSel(g_hwndProcessCombo);
		DWORD dwProcessID = (DWORD)ComboBox_GetItemData(g_hwndProcessCombo, idxIndex);

		// Get the thread ID
		idxIndex = ComboBox_GetCurSel(g_hwndThreadCombo);
		DWORD dwThreadID = (DWORD)ComboBox_GetItemData(g_hwndThreadCombo, idxIndex);

		// Open the thread if we can
		hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, dwThreadID);

		// Thread?
		if (hThread != NULL) 
		{
			// Get its token
			if (!OpenThreadToken(hThread, TOKEN_READ|TOKEN_QUERY_SOURCE, TRUE, &hToken)) 
			{
				hToken = NULL;
				if (GetLastError() == ERROR_NO_TOKEN) {

					// Not a critical error, the thread doesn't have a token
					pszStatus = TEXT("Thread does not have a token, dumping process token");
					SetLastError(0);

				} else {

					pszStatus = TEXT("OpenThreadToken");
					goto leave;
				}
			}
		}

		// So we don't have a token yet, lets get it from the process
		if (hToken == NULL) 
		{
			// Get the handle to the process
			hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessID);
			if (hProcess == NULL) {
				pszStatus = TEXT("OpenProcess");
				goto leave;
			}

			// Get the token (All access so we can change and launch things
			if (!OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken)) {
				hToken = NULL;
				pszStatus = TEXT("OpenProcessToken");
				goto leave;
			}
		}

		// Get memory for an SD
		pSD = (PSECURITY_DESCRIPTOR) GlobalAlloc(GPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);

		if (pSD == NULL) {
			pszStatus = TEXT("GlobalAlloc");
			goto leave;
		}

		// Initialize it
		if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION)) {
			pszStatus = TEXT("InitializeSecurityDescriptor");
			goto leave;
		}

		// Add a NULL DACL to the security descriptor..
		if (!SetSecurityDescriptorDacl(pSD, TRUE, (PACL)NULL, FALSE)) {
			pszStatus = TEXT("SetSecurityDescriptorDacl");
			goto leave;
		}

		// We made the security descriptor just in case they want a duplicate.
		// We make the duplicate have all access to everyone.
		SECURITY_ATTRIBUTES sa;
		sa.nLength              = sizeof(sa);
		sa.lpSecurityDescriptor = pSD;
		sa.bInheritHandle       = TRUE;

		// If the user chooses not to copy the token, then changes made to it
		// will effect the owning process
		if (IDNO == MessageBox(hwnd, TEXT("Would you like to make a copy of ")
			TEXT("this process token?\n(Selecting \"No\" will cause the ")
			TEXT("\"AdjustToken\" and \"SetToken\"\nfeatures to affect the ")
			TEXT("owning process.) "), TEXT("Duplicate Token?"), MB_YESNO)) 
		{
			g_hToken = hToken;
		}  
		else 
		{
			// Duplicate the token
			if (!DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, &sa,
				SecurityImpersonation, TokenPrimary, &g_hToken)) 
			{
				g_hToken = NULL;
				pszStatus = TEXT("DuplicateTokenEx");
				goto leave;
			}
		}

		SetLastError(0);

	} leave:;
	} catch(...) {}

	// Status and Cleanup
	dwStatus = GetLastError();

	if (hToken && hToken != g_hToken)
		CloseHandle(hToken);

	if (hProcess != NULL)
		CloseHandle(hProcess);

	if (hThread != NULL)
		CloseHandle(hThread);

	if (pSD != NULL)
		GlobalFree(pSD);

	RefreshStatus(pszStatus, dwStatus);
}


BOOL GetAccountName(HWND hwnd, PTSTR szBuf, DWORD dwSize, BOOL fAllowGroups,
	BOOL fAllowUsers) 
{
	BOOL fSuccess      = FALSE;
	BOOL fGotStgMedium = FALSE;

	STGMEDIUM stgmedium = {TYMED_HGLOBAL, NULL, NULL};

	IDsObjectPicker* pdsObjectPicker = NULL;
	IDataObject*     pdoNames        = NULL;

	*szBuf = 0;

	try {{

		// Yes we are using COM
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		if (FAILED(hr))
			goto leave;

		// Create an instance of the object picker.
		hr = CoCreateInstance(CLSID_DsObjectPicker, NULL, CLSCTX_INPROC_SERVER,
			IID_IDsObjectPicker, (void**) &pdsObjectPicker);
		if (FAILED(hr))
			goto leave;

		// Initialize the DSOP_SCOPE_INIT_INFO array.
		DSOP_SCOPE_INIT_INFO dsopScopeInitInfo;
		ZeroMemory(&dsopScopeInitInfo, sizeof(dsopScopeInitInfo));

		// The scope in our case is the the current system + downlevel domains
		dsopScopeInitInfo.cbSize = sizeof(DSOP_SCOPE_INIT_INFO);
		dsopScopeInitInfo.flType = DSOP_SCOPE_TYPE_TARGET_COMPUTER
			| DSOP_SCOPE_TYPE_DOWNLEVEL_JOINED_DOMAIN
			| DSOP_SCOPE_TYPE_USER_ENTERED_DOWNLEVEL_SCOPE;

		// What are we selecting?
		dsopScopeInitInfo.FilterFlags.flDownlevel = 0;

		if (fAllowGroups)
			dsopScopeInitInfo.FilterFlags.flDownlevel |=
			DSOP_DOWNLEVEL_FILTER_ALL_WELLKNOWN_SIDS
			| DSOP_DOWNLEVEL_FILTER_LOCAL_GROUPS
			| DSOP_DOWNLEVEL_FILTER_GLOBAL_GROUPS;

		if (fAllowUsers)
			dsopScopeInitInfo.FilterFlags.flDownlevel |=
			DSOP_DOWNLEVEL_FILTER_USERS;

		// Initialize the DSOP_INIT_INFO structure.
		DSOP_INIT_INFO dsopInitInfo;
		ZeroMemory(&dsopInitInfo, sizeof(dsopInitInfo));
		dsopInitInfo.cbSize            = sizeof(dsopInitInfo);
		dsopInitInfo.pwzTargetComputer = NULL; // Target is the local computer
		dsopInitInfo.cDsScopeInfos     = 1;
		dsopInitInfo.aDsScopeInfos     = &dsopScopeInitInfo;
		dsopInitInfo.flOptions         = 0;

		// Initialize the object picker instance.
		hr = pdsObjectPicker->Initialize(&dsopInitInfo);
		if (FAILED(hr))
			goto leave;

		// Invoke the modal dialog where the user selects the user or group
		hr = pdsObjectPicker->InvokeDialog(hwnd, &pdoNames);
		if (FAILED(hr))
			goto leave;

		if (hr == S_OK) {

			// Get the global memory block containing the user's selections.
			FORMATETC formatetc = {(CLIPFORMAT)
				RegisterClipboardFormat(CFSTR_DSOP_DS_SELECTION_LIST), NULL,
				DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
			hr = pdoNames->GetData(&formatetc, &stgmedium);
			if (FAILED(hr))
				goto leave;
			fGotStgMedium = TRUE;

			// Retrieve pointer to DS_SELECTION_LIST structure.
			PDS_SELECTION_LIST pdsSelList = NULL;
			pdsSelList = (PDS_SELECTION_LIST) GlobalLock(stgmedium.hGlobal);
			if (pdsSelList == NULL)
				goto leave;

			// We only allowed the selection of one, so our list is 1 or 0
			if (pdsSelList->cItems == 1) {

				// Copy the account name to the buffer passed in
				lstrcpynW(szBuf, pdsSelList->aDsSelection->pwzName, dwSize);
				fSuccess = TRUE;
			}

		}

	} leave:;
	} catch(...) {}

	if (fGotStgMedium)   {

		// Unlock that buffer
		GlobalUnlock(stgmedium.hGlobal);

		// Release the data
		ReleaseStgMedium(&stgmedium);
	}

	// Release the picker
	if (pdsObjectPicker != NULL)
		pdsObjectPicker->Release();

	// Release the data
	if (pdoNames != NULL)
		pdoNames->Release();

	// Done with COM for the moment
	CoUninitialize();

	return(fSuccess);
}

