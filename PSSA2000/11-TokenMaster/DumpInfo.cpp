#include "shareinc.h"
#include "TokenMaster-helper.h"
#include "../chjutils/chjutils.h"

#if 0
// [2025-02-26] Chj: I'll use stock ConvertSidToStringSid() instead.
BOOL GetTextualSid(PSID pSid, PTSTR TextualSid, PDWORD pdwBufferLen) 
{
	BOOL fSuccess = FALSE;

	try {{

		// Test if Sid passed in is valid
		if (!IsValidSid(pSid))
			goto leave;

		// Obtain Sid identifier authority
		PSID_IDENTIFIER_AUTHORITY psia = GetSidIdentifierAuthority(pSid);

		// Obtain sid subauthority count
		DWORD dwSubAuthorities = *GetSidSubAuthorityCount(pSid);

		// Compute buffer length
		// S-SID_REVISION-+identifierauthority-+subauthorities-+NULL
		DWORD dwSidSize = (15 + 12 + (12 * dwSubAuthorities) + 1)
			* sizeof(TCHAR);

		// Check provided buffer length.
		// If not large enough, indicate proper size and SetLastError
		if (*pdwBufferLen < dwSidSize) {
			*pdwBufferLen = dwSidSize;
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			goto leave;
		}

		// prepare S-SID_REVISION-
		dwSidSize = wsprintf(TextualSid, TEXT("S-%lu-"), SID_REVISION);

		// Prepare Sid identifier authority
		if ((psia->Value[0] != 0) || (psia->Value[1] != 0)) {

			dwSidSize += wsprintf(TextualSid + lstrlen(TextualSid),
				TEXT("0x%02hx%02hx%02hx%02hx%02hx%02hx"),
				(USHORT) psia->Value[0], (USHORT) psia->Value[1],
				(USHORT) psia->Value[2], (USHORT) psia->Value[3],
				(USHORT) psia->Value[4], (USHORT) psia->Value[5]);

		} else {

			dwSidSize += wsprintf(TextualSid + lstrlen(TextualSid),
				TEXT("%lu"), (ULONG) (psia->Value[5])
				+ (ULONG) (psia->Value[4] << 8)
				+ (ULONG) (psia->Value[3] << 16)
				+ (ULONG) (psia->Value[2] << 24));
		}

		// Loop through sid subauthorities
		DWORD dwCounter;
		for (dwCounter = 0; dwCounter < dwSubAuthorities; dwCounter++) {
			dwSidSize += wsprintf(TextualSid + dwSidSize, TEXT("-%lu"),
				*GetSidSubAuthority(pSid, dwCounter));
		}

		fSuccess = TRUE;

	} leave:;
	} catch(...) {}

	return(fSuccess);
}
#endif

void DumpSID(PSID _psid, CPrintBuf* pbufToken) 
{
	SID *psid = (SID*)_psid;

	SID_NAME_USE snuUse = SidTypeInvalid; // neg-init
	PTSTR pszUse = NULL;
	TCHAR szName[255]    = {};
	DWORD dwSize         = ARRAYSIZE(szName);
	TCHAR szDomName[255] = {};
	DWORD dwDomSize      = ARRAYSIZE(szDomName);

	// What is the SID type
	if (LookupAccountSid(NULL, psid, szName, &dwSize, szDomName, &dwDomSize, &snuUse)) 
	{
		switch (snuUse) {

		case SidTypeUser:
			pszUse = TEXT("User SID");
			break;

		case SidTypeGroup:
			pszUse = TEXT("Group SID");
			break;

		case SidTypeDomain:
			pszUse = TEXT("Domain SID");
			break;

		case SidTypeAlias:
			pszUse = TEXT("Alias SID");
			break;

		case SidTypeWellKnownGroup:
			pszUse = TEXT("Well-Known Group SID");
			break;

		case SidTypeDeletedAccount:
			pszUse = TEXT("Deleted Account");
			break;

		case SidTypeInvalid:
			pszUse = TEXT("Invalid SID");
			break;

		case SidTypeLabel:
			pszUse = TEXT("Integrity Level Label");
			break;

		default:
			pszUse = TEXT("Unknown SID");
			break;
		}

	} else {

		// We couldn't look it up, maybe it is the logon SID.
		// Chjmemo: sth like: S-1-5-5-0-448978

		PSID_IDENTIFIER_AUTHORITY psia = GetSidIdentifierAuthority(psid);
		DWORD dwFirstSub = *GetSidSubAuthority(psid, 0);
		if (dwFirstSub == 5 && psia->Value[6] == 5)
			pszUse = TEXT("(Logon SID)");
		else
			pszUse = TEXT("");
	}

	// Get the textual SID
	TCHAR *pszSIDText = NULL;
	ConvertSidToStringSid(psid, &pszSIDText);
	CEC_LocalFree cec_sidtext = pszSIDText;

	// Output the things we have learned
	pbufToken->Print(TEXT("SID:\t\t%s\r\n"), pszSIDText);

	pbufToken->Print(TEXT("Use:\t\t%s\r\n"), pszUse);

	pbufToken->Print(TEXT("\r\nName:\t\t\"%s\"\r\n"), szName);

	pbufToken->Print(TEXT("\r\nDomain Name:\t\"%s\"\r\n"), szDomName);

	pbufToken->Print(TEXT("\r\n"));
}

void DumpSIDAttributes(DWORD dwAttrib, CPrintBuf* pbufToken) 
{
	pbufToken->Print(TEXT("SID Attribs:\r\n"));

	if (dwAttrib & SE_GROUP_MANDATORY)
		pbufToken->Print(TEXT("\t\tSE_GROUP_MANDATORY\r\n"));

	if (dwAttrib & SE_GROUP_ENABLED_BY_DEFAULT)
		pbufToken->Print(TEXT("\t\tSE_GROUP_ENABLED_BY_DEFAULT\r\n"));

	if (dwAttrib & SE_GROUP_ENABLED)
		pbufToken->Print(TEXT("\t\tSE_GROUP_ENABLED\r\n"));

	if (dwAttrib & SE_GROUP_OWNER)
		pbufToken->Print(TEXT("\t\tSE_GROUP_OWNER\r\n"));

	if (dwAttrib & SE_GROUP_USE_FOR_DENY_ONLY)
		pbufToken->Print(TEXT("\t\tSE_GROUP_USE_FOR_DENY_ONLY\r\n"));

	if (dwAttrib & SE_GROUP_INTEGRITY) // Vista+
		pbufToken->Print(TEXT("\t\tSE_GROUP_INTEGRITY\r\n"));

	if (dwAttrib & SE_GROUP_INTEGRITY_ENABLED) // Vista+
		pbufToken->Print(TEXT("\t\tSE_GROUP_INTEGRITY_ENABLED\r\n"));

	if (dwAttrib & SE_GROUP_LOGON_ID)
		pbufToken->Print(TEXT("\t\tSE_GROUP_LOGON_ID\r\n"));

	if (dwAttrib & SE_GROUP_RESOURCE) // Vista+
		pbufToken->Print(TEXT("\t\tSE_GROUP_RESOURCE\r\n"));
}

BOOL DumpTokenGroups(HANDLE hToken, CPrintBuf* pbufToken) 
{
//	BOOL          fSuccess  = FALSE;
	PTOKEN_GROUPS ptgGroups = (PTOKEN_GROUPS) myAllocateTokenInfo(hToken, TokenGroups);
	CEC_LocalFree cec_ptg = ptgGroups;
	if (ptgGroups == NULL)
		return FALSE;

	pbufToken->Print(DIVIDERL TEXT("GROUP SIDs\r\n"));
	pbufToken->Print(TEXT("Group Count:\t%lu"), ptgGroups->GroupCount);
	pbufToken->Print(TEXT("\r\n") DIVIDERL);

	// Dump the SID and Attributes for each group
	DWORD dwIndex = 0;
	for (; dwIndex < ptgGroups->GroupCount; dwIndex++) 
	{
		pbufToken->Print(TEXT("Sid #%d\r\n"), dwIndex);
		
		DumpSID(ptgGroups->Groups[dwIndex].Sid, pbufToken);
		
		DumpSIDAttributes(ptgGroups->Groups[dwIndex].Attributes, pbufToken);
		
		pbufToken->Print(DIVIDERS);
	}

	return TRUE;
}

void DumpTokenPrivileges(HANDLE hToken, CPrintBuf* pbufToken) 
{
	PTOKEN_PRIVILEGES ptpPrivileges = 
		(PTOKEN_PRIVILEGES) myAllocateTokenInfo(hToken, TokenPrivileges);
	CEC_LocalFree cec_ptp = ptpPrivileges;
	if (ptpPrivileges == NULL)
		return;

	pbufToken->Print(DIVIDERL TEXT("Privileges\r\n") DIVIDERL);

	DWORD dwIndex = 0;
	for (; dwIndex < ptpPrivileges->PrivilegeCount; dwIndex++) 
	{
		// Get the privilege name and print it with attribute information
		TCHAR szName[255] = {};
		DWORD dwSize = ARRAYSIZE(szName);
		if (LookupPrivilegeName(NULL,
			&(ptpPrivileges->Privileges[dwIndex].Luid), szName, &dwSize)) 
		{
			pbufToken->Print(szName);
			if (ptpPrivileges->Privileges[dwIndex].Attributes & SE_PRIVILEGE_ENABLED_BY_DEFAULT)
				pbufToken->Print(TEXT("\r\n\t") TEXT("SE_PRIVILEGE_ENABLED_BY_DEFAULT"));

			if (ptpPrivileges->Privileges[dwIndex].Attributes & SE_PRIVILEGE_ENABLED)
				pbufToken->Print(TEXT("\r\n\tSE_PRIVILEGE_ENABLED"));

			if (ptpPrivileges->Privileges[dwIndex].Attributes & SE_PRIVILEGE_REMOVED) // Vista+
				pbufToken->Print(TEXT("\r\n\tSE_PRIVILEGE_REMOVED"));

			if (ptpPrivileges->Privileges[dwIndex].Attributes & SE_PRIVILEGE_USED_FOR_ACCESS)
				pbufToken->Print(TEXT("\r\n\tSE_PRIVILEGE_USED_FOR_ACCESS"));

			pbufToken->Print(TEXT("\r\n"));
		}
	}
}


///////////////////////////////////////////////////////////////////////////////


void DumpTokenOwner(HANDLE hToken, CPrintBuf* pbufToken) 
{
	PTOKEN_OWNER ptoOwner = (PTOKEN_OWNER) myAllocateTokenInfo(hToken, TokenOwner);
	CEC_LocalFree cec_pto = ptoOwner;
	if (ptoOwner == NULL)
		return;

	pbufToken->Print(DIVIDERL TEXT("Token Owner\r\n") DIVIDERL);
	
	DumpSID(ptoOwner->Owner, pbufToken);
}


///////////////////////////////////////////////////////////////////////////////


void DumpTokenPrimaryGroup(HANDLE hToken, CPrintBuf* pbufToken) 
{
	PTOKEN_PRIMARY_GROUP ptgGroup = 
		(PTOKEN_PRIMARY_GROUP) myAllocateTokenInfo(hToken, TokenPrimaryGroup);
	CEC_LocalFree cec_ptg = ptgGroup;
	if (ptgGroup == NULL)
		return;

	pbufToken->Print(DIVIDERL TEXT("Token Primary Group\r\n") DIVIDERL);
	
	DumpSID(ptgGroup->PrimaryGroup, pbufToken);
}


///////////////////////////////////////////////////////////////////////////////


void DumpTokenDefaultDacl(HANDLE hToken, CPrintBuf* pbufToken) 
{
	PTOKEN_DEFAULT_DACL ptdDacl  = NULL;
//	PEXPLICIT_ACCESS    pEntries = NULL; // no use

	// Get the DACL info
	ptdDacl = (PTOKEN_DEFAULT_DACL) myAllocateTokenInfo(hToken, TokenDefaultDacl);
	CEC_LocalFree cec_ptd = ptdDacl;
	if (ptdDacl == NULL)
		return;

	pbufToken->Print(DIVIDERL TEXT("Token Default DACL\r\n") DIVIDERL);

	// Get ACL size information
	ACL_SIZE_INFORMATION sizeInfo = {};
	if (!GetAclInformation(ptdDacl->DefaultDacl, &sizeInfo, sizeof(sizeInfo), AclSizeInformation))
		return;

	// Iterate through the ACEs
	DWORD dwCount = sizeInfo.AceCount;
	DWORD dwIndex;
	for (dwIndex = 0; dwIndex < dwCount; dwIndex++) 
	{
		// Get the ACE by index
		PACCESS_ALLOWED_ACE paceAllowed = NULL;
		if (GetAce(ptdDacl->DefaultDacl, dwIndex, (PVOID*)&paceAllowed)) 
		{
			// Output what type of ACE it is
			switch (paceAllowed->Header.AceFlags) {

			case ACCESS_ALLOWED_OBJECT_ACE_TYPE:
				pbufToken->Print(TEXT("ACCESS ALLOWED OBJECT"));
				break;

			case ACCESS_ALLOWED_ACE_TYPE:
				pbufToken->Print(TEXT("ACCESS ALLOWED"));
				break;

			case ACCESS_DENIED_ACE_TYPE:
				pbufToken->Print(TEXT("ACCESS DENIED"));
				break;

			case ACCESS_DENIED_OBJECT_ACE_TYPE:
				pbufToken->Print(TEXT("ACCESS DENIED OBJECT"));
				break;
			}

			// Chj: Here we should dump the SID text, bcz some SID results in ERROR_NONE_MAPPED.
			TCHAR *psidtext = NULL;
			ConvertSidToStringSid(&(paceAllowed->SidStart), &psidtext);
			CEC_LocalFree cec_psidtext = psidtext;
			pbufToken->Print(_T("\t SID=%s "), psidtext);

			// Get the account name associated with the ace and output it
			TCHAR szName[255] = {};
			TCHAR szDomain[255] = {};
			DWORD dwSize       = ARRAYSIZE(szName);
			DWORD dwDomainSize = ARRAYSIZE(szDomain);

			SID_NAME_USE snuUse = SidTypeInvalid; // neg-init
			BOOL succ = LookupAccountSid(NULL, &(paceAllowed->SidStart), szName, &dwSize,
				szDomain, &dwDomainSize, &snuUse);
			if(succ)
			{
				TCHAR szAccount[255] = {};
				Combine_DomAndName(szAccount, szDomain, szName);
				pbufToken->Print(TEXT("(\"%s\")"), szAccount);
			}

			// Now we output the access mask in binary
			pbufToken->Print(TEXT("\r\nACCESS_MASK:  "));
			DWORD dwMask = paceAllowed->Mask;
			DWORD dwIndex2;
			
			for (dwIndex2 = 0; dwIndex2 < 32; dwIndex2++) {
				if (dwMask&(1 << 31))
					pbufToken->Print(TEXT("1"));
				else
					pbufToken->Print(TEXT("0"));
				dwMask = (dwMask << 1);
			}

			// Determine the bits that map to standard access rights and print
			// the flag name
			pbufToken->Print(TEXT("\r\n"));
			if (paceAllowed->Mask & (1 << 16))
				pbufToken->Print(TEXT("\tDELETE\r\n"));
			if (paceAllowed->Mask & (1 << 17))
				pbufToken->Print(TEXT("\tREAD_CONTROL\r\n"));
			if (paceAllowed->Mask & (1 << 18))
				pbufToken->Print(TEXT("\tWRITE_DAC\r\n"));
			if (paceAllowed->Mask & (1 << 19))
				pbufToken->Print(TEXT("\tWRITE_OWNER\r\n"));
			if (paceAllowed->Mask & (1 << 20))
				pbufToken->Print(TEXT("\tSYNCHRONIZE\r\n"));
			if (paceAllowed->Mask & (1 << 24))
				pbufToken->Print(TEXT("\tACCESS_SYSTEM_SECURITY\r\n"));
			if (paceAllowed->Mask & (1 << 25))
				pbufToken->Print(TEXT("\tMAXIMUM_ALLOWED\r\n"));
			if (paceAllowed->Mask & (1 << 28))
				pbufToken->Print(TEXT("\tGENERIC_ALL\r\n"));
			if (paceAllowed->Mask & (1 << 29))
				pbufToken->Print(TEXT("\tGENERIC_EXECUTE\r\n"));
			if (paceAllowed->Mask & (1 << 30))
				pbufToken->Print(TEXT("\tGENERIC_WRITE\r\n"));
			if (paceAllowed->Mask & (1 << 31))
				pbufToken->Print(TEXT("\tGENERIC_READ\r\n"));
			pbufToken->Print(TEXT("\r\n"));
		}
	}

	pbufToken->Print(TEXT("\r\n"));
}


///////////////////////////////////////////////////////////////////////////////


void DumpTokenSource(HANDLE hToken, CPrintBuf* pbufToken) 
{
	PTOKEN_SOURCE ptsSource = (PTOKEN_SOURCE) myAllocateTokenInfo(hToken, TokenSource);
	CEC_LocalFree cec_pts = ptsSource;
	if (ptsSource == NULL)
		return;

	TCHAR szName[TOKEN_SOURCE_LENGTH+1] = {}; // TOKEN_SOURCE_LENGTH=8
	int nIndex = TOKEN_SOURCE_LENGTH;
	while (nIndex-- != 0) // copy the 8 Chars ( char -> TCHAR )
		szName[nIndex] = ptsSource->SourceName[nIndex];

	szName[TOKEN_SOURCE_LENGTH] = '\0';
	
	pbufToken->Print(DIVIDERL TEXT("Token Source: %s\r\n") DIVIDERL, szName);
	// -- [2025-02-26] Chj sees "User32"
}


///////////////////////////////////////////////////////////////////////////////


void DumpTokenImpersonationLevel(HANDLE hToken, CPrintBuf* pbufToken) 
{
	PSECURITY_IMPERSONATION_LEVEL psilImpersonation = 
		(PSECURITY_IMPERSONATION_LEVEL) myAllocateTokenInfo(hToken, TokenImpersonationLevel);
	CEC_LocalFree cec_psil = psilImpersonation;
	if (psilImpersonation == NULL)
		return;

	pbufToken->Print(DIVIDERL TEXT("Token Impersonation Level:  "));

	switch (*psilImpersonation) {
	case SecurityAnonymous:
		pbufToken->Print(TEXT("Anonymous"));
		break;

	case SecurityIdentification:
		pbufToken->Print(TEXT("Identification"));
		break;

	case SecurityImpersonation:
		pbufToken->Print(TEXT("Impersonation"));
		break;

	case SecurityDelegation:
		pbufToken->Print(TEXT("Delegation"));
		break;
	}

	pbufToken->Print(TEXT("\r\n") DIVIDERL);
}


///////////////////////////////////////////////////////////////////////////////


void DumpTokenType(HANDLE hToken, CPrintBuf* pbufToken) 
{
	PTOKEN_TYPE pttType = (PTOKEN_TYPE) myAllocateTokenInfo(hToken, TokenType);
	CEC_LocalFree cec_ptt = pttType;
	if (pttType == NULL)
		return;

	pbufToken->Print(DIVIDERL TEXT("Token Type:  "));

	BOOL fImpersonation = FALSE;
	switch (*pttType) {
	case TokenPrimary:
		pbufToken->Print(TEXT("Primary"));
		break;

	case TokenImpersonation:
		pbufToken->Print(TEXT("Impersonation"));
		fImpersonation = TRUE;
		break;
	}

	pbufToken->Print(TEXT("\r\n") DIVIDERL);

	if (fImpersonation)
		DumpTokenImpersonationLevel(hToken, pbufToken);
}

BOOL DumpTokenUser(HANDLE hToken, CPrintBuf* pbufToken) 
{
	PTOKEN_USER ptuUser  = (PTOKEN_USER) myAllocateTokenInfo(hToken, TokenUser);
	CEC_LocalFree cec_ptu = ptuUser;
	if (ptuUser == NULL)
		return FALSE;

	pbufToken->Print(DIVIDERL TEXT("USER SID\r\n") DIVIDERL TEXT("\r\n"));
	DumpSID(ptuUser->User.Sid, pbufToken);

	return TRUE;
}

BOOL DumpTokenRestrictedSids(HANDLE hToken, CPrintBuf* pbufToken) 
{
	BOOL fRestricted = IsTokenRestricted(hToken);

	PTOKEN_GROUPS ptgGroups = (PTOKEN_GROUPS) myAllocateTokenInfo(hToken, TokenRestrictedSids);
	CEC_LocalFree cec_ptg = ptgGroups;
	if (ptgGroups == NULL)
		return FALSE;

	pbufToken->Print(DIVIDERL);
	pbufToken->Print(fRestricted 
		? TEXT("Token is Restricted\r\n")
		: TEXT("Token is not Restricted\r\n"));
	pbufToken->Print(TEXT("Restricted SID Count:\t%lu\r\n"), ptgGroups->GroupCount);

	pbufToken->Print(TEXT("Restricted SIDs\r\n"));

	DWORD dwIndex = 0;
	for (; dwIndex < ptgGroups->GroupCount; dwIndex++) {
		pbufToken->Print(TEXT("  Sid #%d\r\n"), dwIndex);
		DumpSIDAttributes(ptgGroups->Groups[dwIndex].Attributes, pbufToken);
		DumpSID(ptgGroups->Groups[dwIndex].Sid, pbufToken);
		// pbufToken->Print(DIVIDERS);
	}

	if(dwIndex==0) {
		pbufToken->Print(TEXT("  (none)\r\n"));
	}

	pbufToken->Print(DIVIDERL);
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////


void guiDumpToken() 
{
	CPrintBuf* pbufToken = NULL;

	try {{

		// No token?  Then slip out the back jack
		if (g_hToken == NULL)
			goto leave;

		// Update other controls
		
		guiUpdatePrivileges();
		
		guiUpdateGroups();
		
		ListBox_ResetContent(g_hwndRestrictedSids);

		// Create a print buf object for buffering output to the edit control.
		// This class and usage instructions can be found in the "CmnCls.h"
		pbufToken = new CPrintBuf;
		if (pbufToken == NULL)
			goto leave;

		// Chj: Show token handle value.
		pbufToken->Print(_T("Token handle value: 0x%X\r\n"), PtrToUint(g_hToken));

		// Display the token user
		if (!DumpTokenUser(g_hToken, pbufToken))
			pbufToken->Print(TEXT("Unable to dump Token User\r\n"));

		// Display the token groups
		if (!DumpTokenGroups(g_hToken, pbufToken))
			pbufToken->Print(TEXT("Unable to dump Token Groups\r\n"));

		// Display the token privileges
		DumpTokenPrivileges(g_hToken, pbufToken);

		// Display the token owner
		DumpTokenOwner(g_hToken, pbufToken);

		// Display the token primary group
		DumpTokenPrimaryGroup(g_hToken, pbufToken);

		// Display the token default DACL
		DumpTokenDefaultDacl(g_hToken, pbufToken);

		// Display the token source
		DumpTokenSource(g_hToken, pbufToken);

		// Display the token type
		DumpTokenType(g_hToken, pbufToken);

		// Display the token restricted sids
		if (!DumpTokenRestrictedSids(g_hToken, pbufToken))
			pbufToken->Print(TEXT("Unable to dump Token Restricted Sids\r\n"));

	} leave:;
	} catch(...) {}

	// If we have a buffer object
	if (pbufToken != NULL) {

		// Then output its contents to our edit control
		SetWindowText(g_hwndToken, *pbufToken);

		// Then delete it
		delete pbufToken;
	}

}
