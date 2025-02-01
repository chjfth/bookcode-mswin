/******************************************************************************
Module:  AccessMaster.cpp
Notices: Copyright (c) 2000 Jeffrey Richter
******************************************************************************/


#include "..\CmnHdr.h"              /* See Appendix A. */
#include <assert.h>
#include <WindowsX.h>
#include <ACLAPI.h>
#include <ACLUI.h>
#include <sddl.h>
#include "Resource.h"

// Force linking against the ACLUI library
#pragma comment(lib, "ACLUI.lib")   

#define PRINTBUF_IMPL
#include "..\ClassLib\PrintBuf.h"

#define  JULAYOUT_IMPL
#include <mswin/JULayout2.h>

#include <vaDbg.h>
#include <itc/InterpretConst.h>

#include "../chjutils/chjutils.h"
#include "../chjutils/ch10-debug.h"

#include "AccessData.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

using namespace itc;

#define EXE_VERSION "1.1.0"

///////////////////////////////////////////////////////////////////////////////


void ReportError(PTSTR szFunction, ULONG lErr) 
{
	CPrintBuf prntBuf;
	prntBuf.Print(TEXT("The Function:  %s\r\n"), szFunction);
	prntBuf.Print(TEXT("Caused the following error - \r\n"));
	prntBuf.Print(TEXT("\r\n"));
   
	prntBuf.PrintError(lErr);
   
	MessageBox(NULL, prntBuf, TEXT("AccessMaster Error"), MB_OK);
}


///////////////////////////////////////////////////////////////////////////////


void UpdateObjDependentCtrls(HWND hwnd) {
   
   // Setup controls for selected object type
   HWND hwndCtrl = GetDlgItem(hwnd, IDC_TYPE);
   int nIndex = ComboBox_GetCurSel(hwndCtrl);

   SetDlgItemText(hwnd, IDE_USAGE, g_objMap[nIndex].m_pszUsageText);

   hwndCtrl = GetDlgItem(hwnd, IDE_NAME);
   EnableWindow(hwndCtrl, g_objMap[nIndex].m_fUseName);

   hwndCtrl = GetDlgItem(hwnd, IDE_HANDLE);
   EnableWindow(hwndCtrl, g_objMap[nIndex].m_fUseHandle);

   hwndCtrl = GetDlgItem(hwnd, IDE_PID);
   EnableWindow(hwndCtrl, g_objMap[nIndex].m_fUsePID);

   if (g_objMap[nIndex].m_fUsePID || g_objMap[nIndex].m_fUseHandle) {
      hwndCtrl = GetDlgItem(hwnd, IDR_HANDLE);
      EnableWindow(hwndCtrl, TRUE);
   } else {
      hwndCtrl = GetDlgItem(hwnd, IDR_HANDLE);
      EnableWindow(hwndCtrl, FALSE);
      CheckRadioButton(hwnd, IDR_NAME, IDR_HANDLE, IDR_NAME);
   }

   if (g_objMap[nIndex].m_fUseName) {
      hwndCtrl = GetDlgItem(hwnd, IDR_NAME);
      EnableWindow(hwndCtrl, TRUE);
   } else {
      hwndCtrl = GetDlgItem(hwnd, IDR_NAME);
      EnableWindow(hwndCtrl, FALSE);
      CheckRadioButton(hwnd, IDR_NAME, IDR_HANDLE, IDR_HANDLE);
   }
}


///////////////////////////////////////////////////////////////////////////////


BOOL Dlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) {

   chSETDLGICONS(hwnd, IDI_ACCESSMASTER);

   CheckDlgButton(hwnd, IDR_NAME, BST_CHECKED);

   TCHAR username[256] = {};
   DWORD bufchars = ARRAYSIZE(username);
   GetUserName(username, &bufchars);

   vaSetWindowText(hwnd, _T("v%s AccessMaster run as \"%s\""), _T(EXE_VERSION), username);

   // Set-up the object type combo
   int nIndex = chDIMOF(g_objMap);
   HWND hwndCtrl = GetDlgItem(hwnd, IDC_TYPE);
   while (nIndex-- != 0) {
      ComboBox_InsertString(hwndCtrl, 0, g_objMap[nIndex].m_pszComboText);
   }

   ComboBox_SetCurSel(hwndCtrl, 0);
   UpdateObjDependentCtrls(hwnd);

   return(TRUE);
}


///////////////////////////////////////////////////////////////////////////////


void HandleType(HWND hwnd, UINT codeNotify, HWND hwndCtl) {

   if (codeNotify == CBN_SELCHANGE)
      UpdateObjDependentCtrls(hwnd);
}


///////////////////////////////////////////////////////////////////////////////


void HandleRadio(HWND hwnd, UINT codeNotify, UINT nCtrl) {
   
   if (codeNotify == EN_SETFOCUS)
      CheckRadioButton(hwnd, IDR_NAME, IDR_HANDLE, nCtrl);
}


///////////////////////////////////////////////////////////////////////////////


BOOL FillInfo(HWND hwnd, ObjInf* pInfo) 
{   
	BOOL fReturn = FALSE;

	// Map object type to data block in the object map
	HWND hwndCtrl = GetDlgItem(hwnd, IDC_TYPE);
	int nIndex = ComboBox_GetCurSel(hwndCtrl);
	pInfo->m_pEntry = g_objMap + nIndex;

	// Copy the object's name into the info block for building the title text
	lstrcpy(pInfo->m_szObjectName, pInfo->m_pEntry->m_pszComboText);

	// Is it a named item?
	if (IsDlgButtonChecked(hwnd, IDR_NAME)) 
	{
		switch (pInfo->m_pEntry->m_nSpecificType) 
		{

		case AM_WINDOWSTATION:
			{ 
				// If window-station, we must translate the name to a handle
				HWINSTA hwinsta = NULL;
				GetDlgItemText(hwnd, IDE_NAME, pInfo->m_szName, 
					chDIMOF(pInfo->m_szName));
				// Get the maximum possible access
				hwinsta = OpenWindowStation(pInfo->m_szName, FALSE, 
					MAXIMUM_ALLOWED);

				if (hwinsta == NULL) // Still failed?
					ReportError(TEXT("OpenWindowStation"), GetLastError());
				else { // Otherwise finish title text
					fReturn = TRUE;
					pInfo->m_hHandle = (HANDLE) hwinsta;
					lstrcat(pInfo->m_szObjectName, TEXT("-"));
					lstrcat(pInfo->m_szObjectName, pInfo->m_szName); 
					pInfo->m_szName[0] = 0;
				}
			}
			break;

		case AM_DESKTOP:
			{ 
				// If desktop, we must translate the name to a handle
				HWINSTA hwinstaOld;
				HWINSTA hwinstaTemp;
				HDESK hdesk=NULL;
				PTSTR pszWinSta;
				PTSTR pszDesktop;
				int nIndex;

				GetDlgItemText(hwnd, IDE_NAME, pInfo->m_szName, 
					chDIMOF(pInfo->m_szName));
				pszWinSta = pInfo->m_szName;
				nIndex = lstrlen(pInfo->m_szName);

				// Parse the text for windowstation and desktop
				while (nIndex-- != 0) 
				{
					if (pInfo->m_szName[nIndex] == TEXT('\\') 
						|| pInfo->m_szName[nIndex] == TEXT('/')) {

							pInfo->m_szName[nIndex] = 0;
							break;
					}
				}

				// Desktop string
				nIndex++;
				pszDesktop = pInfo->m_szName + nIndex;
				// Open the windowstation
				hwinstaTemp = OpenWindowStation(pszWinSta, FALSE, 
					DESKTOP_ENUMERATE);
				if (hwinstaTemp != NULL) 
				{
					// Save the last one
					hwinstaOld = GetProcessWindowStation();
					SetProcessWindowStation(hwinstaTemp);
					// Get maximum access to the desktop
					hdesk = OpenDesktop(pszDesktop, 0, FALSE, 
						MAXIMUM_ALLOWED);
					if (hdesk == NULL)// failed?
						ReportError(TEXT("OpenDesktop"), GetLastError());
					else { // build title
						fReturn = TRUE; 
						pInfo->m_hHandle = (HANDLE) hdesk;
						lstrcat(pInfo->m_szObjectName, TEXT("-"));
						lstrcat(pInfo->m_szObjectName, pszDesktop);
						pInfo->m_szName[0] = 0;
					}

					// Close and reset window stations for the process
					CloseWindowStation(hwinstaTemp);
					SetProcessWindowStation(hwinstaOld);

				} else // Failed open winsta
					ReportError(TEXT("OpenWindowStation"), GetLastError());
			}
			break;

		default: // The rest of named objects work with GetNamedSecurity...
			GetDlgItemText(hwnd, IDE_NAME, pInfo->m_szName, 
				chDIMOF(pInfo->m_szName));
			lstrcat(pInfo->m_szObjectName, TEXT("-"));
			lstrcat(pInfo->m_szObjectName, pInfo->m_szName);
			fReturn = TRUE;
			break;
		}
	} 
	else 
	{ 
		// Is it a handle and or process id we are dealing with?

		// Get the actual numbers
		BOOL fTrans = 0;
		ULONG lPid = GetDlgItemInt(hwnd, IDE_PID, &fTrans, FALSE);
		
		TCHAR edtext[20] = {};
		GetDlgItemText(hwnd, IDE_HANDLE, edtext, ARRAYSIZE(edtext)-1);
		HANDLE hHandle = (HANDLE)_tcstoui64(edtext, NULL, 0);
		
		HANDLE hObj = NULL;

		switch (pInfo->m_pEntry->m_nSpecificType) 
		{

		case AM_THREAD: // Maximum access to the thread
			hObj = OpenThread(MAXIMUM_ALLOWED, FALSE, lPid);
			if (hObj == NULL) // None == failure
				ReportError(TEXT("OpenThread"), GetLastError());
			break;

		case AM_PROCESS: // Get maximum access to the process
			hObj = OpenProcess(MAXIMUM_ALLOWED, FALSE, lPid);
			if (hObj == NULL) // None == failure
				ReportError(TEXT("OpenProcess"), GetLastError());
			break;

		default: // The rest work with duplicate handle
			{
				HANDLE hProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, lPid);
				if (hProcess != NULL) 
				{
					// Get as much access as possible
					if (!DuplicateHandle(hProcess, hHandle, GetCurrentProcess(),
						&hObj, 
						0, // dwDesiredAccess, was MAXIMUM_ALLOWED, would get no right on Win7
						FALSE, 
						DUPLICATE_SAME_ACCESS // Chj fixed, was 0.
						)) 
					{
						ReportError(TEXT("DuplicateHandle"), GetLastError());
					}
				} 
				else {
					ReportError(TEXT("OpenProcess"), GetLastError());
				}
			}
			break;
		}

		if (hObj != NULL) {
			pInfo->m_hHandle = hObj;
			fReturn = TRUE;
		}
	}

	// Test object availability 
	if (fReturn) 
	{
		ULONG lErr;
		PSECURITY_DESCRIPTOR pSD = NULL;
		if (pInfo->m_szName[0] != 0) // Is it named
		{
			lErr = GetNamedSecurityInfo(pInfo->m_szName, 
				pInfo->m_pEntry->m_objType, DACL_SECURITY_INFORMATION, 
				NULL, NULL, NULL, NULL, &pSD);
		}
		else 
		{
			// Is it a handle case
			lErr = GetSecurityInfo(pInfo->m_hHandle, pInfo->m_pEntry->m_objType,
				DACL_SECURITY_INFORMATION, NULL, NULL, NULL, NULL, &pSD);
		}

		if ((lErr != ERROR_ACCESS_DENIED) && (lErr != ERROR_SUCCESS)){
			ReportError(TEXT("Get[Named]SecurityInfo"), lErr);
			fReturn = FALSE;
		}
		else {
			LocalFree(pSD);
		}
	}

	return(fReturn);
}


///////////////////////////////////////////////////////////////////////////////


SI_INHERIT_TYPE CSecurityInformation::m_sPropagateType[] = // was named: m_siInheritType[]
{	// Chj modified, more rational
	{
		&m_guidNULL, 
		CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE,
		L"Apply to me, and to my children"
	},
	{
		&m_guidNULL, 
		CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE | INHERIT_ONLY_ACE, 
		L"Apply to my children, but not me"
	},
};


///////////////////////////////////////////////////////////////////////////////


HRESULT CSecurityInformation::QueryInterface(REFIID riid, PVOID* ppvObj) {

   HRESULT hr = E_NOINTERFACE;
   if ((riid == IID_ISecurityInformation) || (riid == IID_IUnknown)) {
      *ppvObj = this;
      AddRef();
      hr = S_OK;
   }
   return(hr);
}


///////////////////////////////////////////////////////////////////////////////


ULONG CSecurityInformation::AddRef() {
   
   m_nRef++;
   return(m_nRef);
}


///////////////////////////////////////////////////////////////////////////////


ULONG CSecurityInformation::Release() {
   
   ULONG nRef = --m_nRef;
   if (m_nRef == 0)
      delete this;
   return(nRef);
}


///////////////////////////////////////////////////////////////////////////////


HRESULT CSecurityInformation::GetObjectInformation(
	PSI_OBJECT_INFO pObjectInfo) 
{
	// We are doing both normal and advanced editing
	pObjectInfo->dwFlags = SI_EDIT_ALL  | SI_ADVANCED;

	// Is it a container?
	if (m_pInfo->m_pEntry->m_fIsContainer) {
		pObjectInfo->dwFlags  |= SI_CONTAINER;
	}

	// Is it a child?
	if (!m_pInfo->m_pEntry->m_fIsChild) {
		pObjectInfo->dwFlags  |= SI_NO_ACL_PROTECT;
	}

	pObjectInfo->hInstance = GetModuleHandle(NULL);
	pObjectInfo->pszServerName = NULL;
	pObjectInfo->pszObjectName = m_pInfo->m_szObjectName;

	vaDbgS(
		_T("System calls our CSecurityInformation::GetObjectInformation(), we return (to ACLUI) SI_OBJECT_INFO:\n")
		_T("  .pszObjectName = \"%s\"\n")
		_T("  .hInstance = 0x%p\n")
		_T("  .dwFlags = %s")
		, 
		pObjectInfo->pszObjectName,
		pObjectInfo->hInstance,
		ITCS(pObjectInfo->dwFlags, Aclui_SI_xxx)
		);

	return(S_OK);
}


///////////////////////////////////////////////////////////////////////////////

HRESULT CSecurityInformation::GetSecurity(
	SECURITY_INFORMATION RequestedInformation, 
	PSECURITY_DESCRIPTOR* ppSecurityDescriptor, BOOL fDefault) 
{
	assert(!fDefault);

	HRESULT hr = 1;
	PSECURITY_DESCRIPTOR pSD = NULL;

	vaDbgS(
		_T("System calls our CSecurityInformation::GetSecurity(), requesting:\n")
		_T("  RequestedInformation = %s")
		, 
		ITCSv(RequestedInformation, xxx_SECURITY_INFORMATION)
		);

	// Get security information
	ULONG lErr;
	if (m_pInfo->m_szName[0] != 0) // Is it named
	{
		lErr = GetNamedSecurityInfo(m_pInfo->m_szName, 
			m_pInfo->m_pEntry->m_objType, RequestedInformation, NULL, NULL, 
			NULL, NULL, &pSD);
	}
	else // Is it a hHandle case
	{
		lErr = GetSecurityInfo(m_pInfo->m_hHandle, m_pInfo->m_pEntry->m_objType,
			RequestedInformation, NULL, NULL, NULL, NULL, &pSD);
	}

	// No matter what, we still display security information
	if (lErr != ERROR_SUCCESS)
	{ 
		// Failure produces an empty SD
		ReportError(TEXT("GetNamedSecurityInfo"), lErr);
		MessageBox(NULL, 
			TEXT("An error occurred retrieving security information for this object,\r\n")
			TEXT("possibly due to insufficient access rights.\r\n")
			TEXT("\r\n")
			TEXT("AccessMaster has created an empty security descriptor for editing.\r\n")
			,
			TEXT("AccessMaster Notice"), 
			MB_OK);
	}
	else 
	{
		hr = S_OK;
		*ppSecurityDescriptor = pSD;

		// Debug info below
		TCHAR sz_which_object[MAX_PATH+20] = {};
		if(m_pInfo->m_szName[0] != 0)
			_sntprintf_s(sz_which_object, _TRUNCATE, _T("object-name: \"%s\""), m_pInfo->m_szName);
		else
			_sntprintf_s(sz_which_object, _TRUNCATE, _T("object-handle: 0x%p (as in our process)"), m_pInfo->m_hHandle);
		vaDbgS(
			_T("We return (to ACLUI) security info about :\n")
			_T("  %s\n")
			_T("  object-type: %s\n")
			_T("Dump SD below:")
			,
			sz_which_object,
			ITCS(m_pInfo->m_pEntry->m_objType, SE_xxx_objectType)
			);
		CH10_DumpSD(pSD);
	}

	return(hr);
}


///////////////////////////////////////////////////////////////////////////////


HRESULT CSecurityInformation::GetAccessRights(const GUID* pguidObjectType,
	DWORD dwFlags, PSI_ACCESS* ppAccess, ULONG* pcAccesses, 
	ULONG* piDefaultAccess) 
{
	// chj: 
	// pguidObjectType is m_guidNULL.
	//
	// dwFlags==0, when the "starting" ALCUI is displayed.
	// dwFlags==SI_ADVANCED, when user clicks [Advanced] button.

	vaDbgS(
		_T("System calls our CSecurityInformation::GetAccessRights(), passing in:\n")
		_T("  dwFlags = %s")
		, 
		ITCS(dwFlags, Aclui_SI_xxx)
		);

	// If the binary check box was set, we show only raw binary ACE information
	if (m_fBinary) {

		*pcAccesses = 32;
		*ppAccess = m_siAccessBinaryRights;

		*piDefaultAccess = 0; // chj: better set a value

	} else { // Otherwise locate the appropriate block of specific rights

		// See AccessData.H header file
		*ppAccess = m_siAccessAllRights[m_pInfo->m_pEntry->m_nSpecificType];
		*pcAccesses = 0;
		
		while ((*ppAccess)[*pcAccesses].mask != 0)
			(*pcAccesses)++;
		
		*piDefaultAccess = 0;
	}
	return(S_OK);
}


///////////////////////////////////////////////////////////////////////////////


HRESULT CSecurityInformation::MapGeneric(const GUID* pguidObjectType, 
      UCHAR* pAceFlags, ACCESS_MASK* pMask) 
{
   return(S_OK);
}


///////////////////////////////////////////////////////////////////////////////


HRESULT CSecurityInformation::GetInheritTypes(PSI_INHERIT_TYPE* ppPropagTypes,
	ULONG* pcPropagTypes) 
{
	if (m_pInfo->m_pEntry->m_fIsContainer) 
	{
		// If it is a container (e.g. a NTFS directory)...

		*pcPropagTypes = ARRAYSIZE(m_sPropagateType);

		*ppPropagTypes = m_sPropagateType;
	} 
	else 
	{ 
		// If it is not container (e.g. a NTFS file), propagation is meaningless
		*ppPropagTypes = NULL;
		*pcPropagTypes = 0;
	}

	vaDbgS(_T("System calls our CSecurityInformation::GetInheritTypes(). We return %d propagation-types."),
		*pcPropagTypes);

	for(int i=0; i<(int)*pcPropagTypes; i++)
	{
		vaDbgS(
			_T("  Propagation-type #%d:\n")
			_T("    SI_INHERIT_TYPE.pszName = %s\n")
			_T("    SI_INHERIT_TYPE.dwFlags = %s")
			, 
			i+1,
			(*ppPropagTypes)[i].pszName,
			ITCS((*ppPropagTypes)[i].dwFlags, xxx_ACE_flags)
			);
	}

	return(S_OK);
}


///////////////////////////////////////////////////////////////////////////////


HRESULT CSecurityInformation::PropertySheetPageCallback(HWND hwnd, UINT uMsg, 
	SI_PAGE_TYPE uPage) 
{
	vaDbgS(_T("PSPcallback: hwnd=0x%08X , MSG=%s , Pagetype=%s"), 
		hwnd,
		ITCS(uMsg, Aclui_PSPCB_xxx), 
		ITCS(uPage, Aclui_SI_PAGE_xxx)
		);

	if(uMsg==PSPCB_SI_INITDIALOG)
	{
		assert(hwnd);
		HWND hwndPrsht = GetParent(hwnd); // Yes, we should operate on its parent-window
		
		bool succ = JULayout::PropSheetProc(hwndPrsht, PSCB_INITIALIZED_1, 0); // [2025-01-23] Pending: should pass in uMsg as 2nd param
		
		vaDbgS(L"JULayout::PropSheetProc()=%s", succ?L"success":L"fail");
	}

	return(S_OK);
}


///////////////////////////////////////////////////////////////////////////////


HRESULT CSecurityInformation::SetSecurity(
	SECURITY_INFORMATION SecurityInformation,
	PSECURITY_DESCRIPTOR pSecurityDescriptor) 
{
	HRESULT hr = 1;

	// Get the Dacl
	PACL pDACL = NULL;
	BOOL fPresentDacl=0, fPresentSacl=0;
	BOOL fDefaultedDacl=0, fDefaultedSacl, fDefaultedOwner=0, fDefaultedGroup;
	GetSecurityDescriptorDacl(pSecurityDescriptor, &fPresentDacl, &pDACL, &fDefaultedDacl);

	// Get the SACL
	PACL pSACL = NULL;
	GetSecurityDescriptorSacl(pSecurityDescriptor, &fPresentSacl, &pSACL, &fDefaultedSacl);

	// Get the owner
	PSID psidOwner = NULL;
	GetSecurityDescriptorOwner(pSecurityDescriptor, &psidOwner, &fDefaultedOwner);

	// Get the group
	PSID psidGroup = NULL;
	GetSecurityDescriptorGroup(pSecurityDescriptor, &psidGroup, &fDefaultedGroup);

	// Find out if DACL and SACL inherit from parent objects
	SECURITY_DESCRIPTOR_CONTROL sdCtrl = NULL;
	ULONG ulRevision;
	GetSecurityDescriptorControl(pSecurityDescriptor, &sdCtrl, &ulRevision);

	// Dump debug:
	vaDbgS(
		_T("System calls our CSecurityInformation::SetSecurity(), passing in:\n")
		_T("  SecurityInformation = %s\n")
		_T("  SD Dump below:")
		, 
		ITCS(SecurityInformation, xxx_SECURITY_INFORMATION)
		);
	CH10_DumpSD(pSecurityDescriptor);

	if (sdCtrl & SE_DACL_PROTECTED)
		SecurityInformation |= PROTECTED_DACL_SECURITY_INFORMATION;
	else
		SecurityInformation |= UNPROTECTED_DACL_SECURITY_INFORMATION;

	if (sdCtrl & SE_SACL_PROTECTED)
		SecurityInformation |= PROTECTED_SACL_SECURITY_INFORMATION;
	else
		SecurityInformation |= UNPROTECTED_SACL_SECURITY_INFORMATION;

	// Set the security
	ULONG lErr = 0;
	if (m_pInfo->m_szName[0] != 0) // Is it named
	{
		lErr = SetNamedSecurityInfo(m_pInfo->m_szName, 
			m_pInfo->m_pEntry->m_objType, SecurityInformation, psidOwner, 
			psidGroup, pDACL, pSACL);
	}
	else // Is it a hHandle case
	{
		lErr = SetSecurityInfo(m_pInfo->m_hHandle, m_pInfo->m_pEntry->m_objType,
			SecurityInformation, psidOwner, psidGroup, pDACL, pSACL);
	}

	// Report error
	if (lErr != ERROR_SUCCESS)
		ReportError(TEXT("SetNamedSecurityInfo or SetSecurityInfo"), lErr);
	else
		hr = S_OK;

	return(hr);
}


///////////////////////////////////////////////////////////////////////////////


void HandleEdit(HWND hwnd) 
{   
	// Maintains information about the object whose security we are editing
	ObjInf info = { 0 };

	// Fill the info structure with info from the UI
	if (FillInfo(hwnd, &info)) 
	{
		// Create instance of class derived from interface ISecurityInformation 
		CSecurityInformation* pSec = new CSecurityInformation(&info, 
			IsDlgButtonChecked(hwnd, IDC_BINARY) == BST_CHECKED);

		// Common dialog box for ACL editing
		EditSecurity(hwnd, pSec);
		if (pSec != NULL)
			pSec->Release();
	}

	// Cleanup if we had opened a handle before.
	// Chj fixed: Even if FillInfo() failed, we need to clean-up.
	if (info.m_szName[0] == 0) 
	{
		switch (info.m_pEntry->m_nSpecificType) 
		{

		case AM_FILE:
		case AM_PROCESS:
		case AM_THREAD:
		case AM_JOB:
		case AM_SEMAPHORE:
		case AM_EVENT:
		case AM_MUTEX:
		case AM_MAPPING:
		case AM_TIMER:
		case AM_TOKEN:
		case AM_NAMEDPIPE:
		case AM_ANONPIPE:
			CloseHandle(info.m_hHandle);
			break;

		case AM_REGISTRY:
			RegCloseKey((HKEY) info.m_hHandle);
			break;

		case AM_WINDOWSTATION:
			CloseWindowStation((HWINSTA) info.m_hHandle);
			break;

		case AM_DESKTOP:
			CloseDesktop((HDESK) info.m_hHandle);
			break;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////


void Dlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) 
{
	switch (id) {

#if 0
	case IDCANCEL:
		EndDialog(hwnd, 0);
		break;
#endif

	case IDC_TYPE:
		HandleType(hwnd, codeNotify, hwndCtl);
		break;

	case IDE_PID:
	case IDE_HANDLE:
		HandleRadio(hwnd, codeNotify, IDR_HANDLE);
		break;

	case IDE_NAME:
		HandleRadio(hwnd, codeNotify, IDR_NAME);
		break;

	case IDB_EDIT:
		HandleEdit(hwnd);
		break;
	}
}

void Dlg_OnSysCommand(HWND hwnd, UINT cmd, int x, int y) 
{
	if(cmd==SC_CLOSE)
	{
		EndDialog(hwnd, 0);
	}
}

///////////////////////////////////////////////////////////////////////////////


INT_PTR WINAPI Dlg_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		HANDLE_MSG(hwnd, WM_INITDIALOG, Dlg_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, Dlg_OnCommand);
		
		// [2022-11-23] Jeffrey's chHANDLE_DLGMSG() returns TRUE to Windows 
		// dialog-box manager, which caused a weird problem:
		// We would not be able to move the dialog-box with mouse.
		// Using HANDLE_MSG() is good, who returns 0 to dialog-box manager.
		HANDLE_MSG(hwnd, WM_SYSCOMMAND, Dlg_OnSysCommand); 
	}
	return(FALSE);
}


///////////////////////////////////////////////////////////////////////////////


int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR pszCmdLine, int) {

   DialogBox(hinstExe, MAKEINTRESOURCE(IDD_DIALOG), NULL, Dlg_Proc);
   return(0);
}


///////////////////////////////// End of File /////////////////////////////////