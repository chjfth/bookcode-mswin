#include "shareinc.h"
#include "TokenMaster-helper.h"
#include "../chjutils/chjutils.h"

#include <mswin/VC2010_future.h>

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
	TCHAR sz_SidUse[40] = {};
	PTSTR pszUse = NULL;
	TCHAR szName[255]    = {};
	DWORD dwSize         = ARRAYSIZE(szName);
	TCHAR szDomName[255] = {};
	DWORD dwDomSize      = ARRAYSIZE(szDomName);

	// What is the SID type
	if (LookupAccountSid(NULL, psid, szName, &dwSize, szDomName, &dwDomSize, &snuUse)) 
	{
		switch ((int)snuUse) {

		case SidTypeUser:
			pszUse = TEXT("User SID (SidTypeUser)");
			break;

		case SidTypeGroup:
			pszUse = TEXT("Group SID (SidTypeGroup)");
			break;

		case SidTypeDomain:
			pszUse = TEXT("Domain SID (SidTypeDomain)");
			break;

		case SidTypeAlias:
			pszUse = TEXT("Alias SID (SidTypeAlias)");
			break;

		case SidTypeWellKnownGroup:
			pszUse = TEXT("Well-Known Group SID (SidTypeWellKnownGroup)");
			break;

		case SidTypeDeletedAccount:
			pszUse = TEXT("Deleted Account (SidTypeDeletedAccount)");
			break;

		case SidTypeInvalid:
			pszUse = TEXT("Invalid SID (SidTypeInvalid)");
			break;

		case SidTypeLabel:
			pszUse = TEXT("Integrity Level Label (SidTypeLabel)");
			break;

		case SidTypeLogonSession: 
			// For LoginVSid like S-1-5-5-0-448978 ,
			// Win7's LookupAccountSid() will fail.
			// Win10's LookupAccountSid() will success and report SidTypeLogonSession.
			pszUse = TEXT("(SidTypeLogonSession)");
			break;

		default:
			_sntprintf_s(sz_SidUse, _TRUNCATE, TEXT("Unknown SID (%d)"), snuUse);
			pszUse = sz_SidUse;
			break;
		}

	} else {

		// We couldn't look it up, maybe it is the logon SID.
		// Chjmemo: sth like: 
		//	S-1-5-5-0-448978
		//	S-1-5-5-0-213111592

		PSID_IDENTIFIER_AUTHORITY psia = GetSidIdentifierAuthority(psid);
		DWORD dwFirstSub = *GetSidSubAuthority(psid, 0);
		if (psia->Value[5] == 5 && dwFirstSub == 5)
			pszUse = TEXT("(Logon SID) // Chj: LogonVSid");
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

	pbufToken->Print(TEXT("Name:\t\t\"%s\"\r\n"), szName);

	pbufToken->Print(TEXT("Domain Name:\t\"%s\"\r\n"), szDomName);
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

	pbufToken->Print(DIVIDERL TEXT("GROUP SIDs (TokenGroups)\r\n"));
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

	pbufToken->Print(DIVIDERL TEXT("Privileges (TokenPrivileges)\r\n") DIVIDERL);

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

	pbufToken->Print(DIVIDERL TEXT("Token Owner (TokenOwner)\r\n") DIVIDERL);
	
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

	pbufToken->Print(DIVIDERL TEXT("Token Primary Group (TokenPrimaryGroup)\r\n") DIVIDERL);
	
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

	pbufToken->Print(DIVIDERL TEXT("Token Default DACL (TokenDefaultDacl)\r\n") DIVIDERL);

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
	
	pbufToken->Print(DIVIDERL TEXT("Token Source (TokenSource): %s\r\n") DIVIDERL, szName);
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

	pbufToken->Print(DIVIDERL TEXT("Token Type (TokenType):  "));

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

	pbufToken->Print(DIVIDERL TEXT("USER SID (TokenUser)\r\n") DIVIDERL);
	DumpSID(ptuUser->User.Sid, pbufToken);

	pbufToken->Print(TEXT("\r\n"));
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
		? TEXT("IsTokenRestricted() = TRUE\r\n")
		: TEXT("IsTokenRestricted() = FALSE\r\n"));
	pbufToken->Print(TEXT("Restricted SID Count:\t%lu\r\n"), ptgGroups->GroupCount);

	pbufToken->Print(TEXT("Restricted SIDs\r\n"));

	DWORD dwIndex = 0;
	for (; dwIndex < ptgGroups->GroupCount; dwIndex++) {

		pbufToken->Print(TEXT("Restricted-Sid #%d:\r\n"), dwIndex);

		DumpSID(ptgGroups->Groups[dwIndex].Sid, pbufToken);

		DumpSIDAttributes(ptgGroups->Groups[dwIndex].Attributes, pbufToken);

		// pbufToken->Print(DIVIDERS);
	}

	if(dwIndex==0) {
		pbufToken->Print(TEXT("  (none)\r\n"));
	}

	pbufToken->Print(DIVIDERL);
	return TRUE;
}

void DumpTokenOrigin(HANDLE hToken, CPrintBuf* pbufToken)
{
	pbufToken->Print(DIVIDERL);

	TOKEN_ORIGIN *pto = (TOKEN_ORIGIN*)myAllocateTokenInfo(hToken, TokenOrigin);
	CEC_LocalFree cec_pto = pto;
	if(!pto)
	{
		pbufToken->Print(_T("Get TokenOrigin fail, WinErr=%s\r\n"), ITCS_WinError);
		return;
	}

	pbufToken->Print(_T("[TokenOrigin]  OriginatingLogonSession = 0x%X:%08X (=%I64u)\r\n"), 
		pto->OriginatingLogonSession.HighPart, pto->OriginatingLogonSession.LowPart,
		*(unsigned __int64*)&pto->OriginatingLogonSession);

	pbufToken->Print(DIVIDERL);
}

static void DumpToken_report_error(const TCHAR *errname, CPrintBuf* pbufToken)
{
	DWORD winerr = GetLastError();
	if(winerr==ERROR_INVALID_PARAMETER) {
		pbufToken->Print(_T("[%s] not supported.\r\n"), errname);
	} else {
		pbufToken->Print(_T("[%s] fail, WinErr=%s\r\n"), errname, ITCSv(winerr, WinError));
	}
}

#pragma warning(push)
#pragma warning(disable:4127) // conditional expression is constant: do{ ... }while(0);

#define DUMPTOKEN_RETURN_ON_ERROR(errname) do { \
	if(!p) { \
		DumpToken_report_error(_T(#errname), pbufToken);  \
		return; \
	} \
}while(0)

void DumpTokenSessionId(HANDLE hToken, CPrintBuf* pbufToken)
{
	pbufToken->Print(DIVIDERL);

	DWORD *p = (DWORD*)myAllocateTokenInfo(hToken, TokenSessionId);
	CEC_LocalFree cec = p;
	DUMPTOKEN_RETURN_ON_ERROR(TokenSessionId);

	pbufToken->Print(_T("[TokenSessionId] = %u (TsSessionId)\r\n"), *p);

	pbufToken->Print(DIVIDERL);
}

void DumpTokenElevation(HANDLE hToken, CPrintBuf* pbufToken)
{
	pbufToken->Print(DIVIDERL);

	TOKEN_ELEVATION *p = (TOKEN_ELEVATION*)myAllocateTokenInfo(hToken, TokenElevation);
	CEC_LocalFree cec = p;
	DUMPTOKEN_RETURN_ON_ERROR(TokenElevation);

	pbufToken->Print(_T("[TokenElevation] TokenIsElevated = %s\r\n"), 
		p->TokenIsElevated ? _T("TRUE") : _T("FALSE"));

	pbufToken->Print(DIVIDERL);
}

void DumpTokenElevationType(HANDLE hToken, CPrintBuf* pbufToken)
{
	pbufToken->Print(DIVIDERL);

	TOKEN_ELEVATION_TYPE *p = 
		(TOKEN_ELEVATION_TYPE *)myAllocateTokenInfo(hToken, TokenElevationType);
	CEC_LocalFree cec = p;
	DUMPTOKEN_RETURN_ON_ERROR(TokenElevationType);

	pbufToken->Print(_T("[TokenElevationType] = %s\r\n"), ITCSv(*p, TokenElevationTypeXXX));

	pbufToken->Print(DIVIDERL);
}

void DumpTokenIntegrityLevel(HANDLE hToken, CPrintBuf* pbufToken)
{
	pbufToken->Print(DIVIDERL);

	TOKEN_MANDATORY_LABEL *p = 
		(TOKEN_MANDATORY_LABEL*)myAllocateTokenInfo(hToken, TokenIntegrityLevel);
	CEC_LocalFree cec = p;
	DUMPTOKEN_RETURN_ON_ERROR(TokenIntegrityLevel);

	TCHAR *strSid = NULL;
	ConvertSidToStringSid(p->Label.Sid, &strSid);
	CEC_LocalFree cec_strSid = strSid;

	TCHAR sidrepr[200] = {};
	SID2Repr(p->Label.Sid, sidrepr, ARRAYSIZE(sidrepr));

	pbufToken->Print(_T("[TokenIntegrityLevel] %s\r\n"), sidrepr);
	pbufToken->Print(_T("    SID.Attributes = 0x%X , %s\r\n"), 
		p->Label.Attributes, ITCSv(p->Label.Attributes, SE_GROUP_xxx));

	pbufToken->Print(DIVIDERL);
}

void DumpTokenMandatoryPolicy(HANDLE hToken, CPrintBuf* pbufToken)
{
	pbufToken->Print(DIVIDERL);

	TOKEN_MANDATORY_POLICY *p = 
		(TOKEN_MANDATORY_POLICY *)myAllocateTokenInfo(hToken, TokenMandatoryPolicy);
	CEC_LocalFree cec = p;
	DUMPTOKEN_RETURN_ON_ERROR(TokenMandatoryPolicy);

	pbufToken->Print(_T("[TokenMandatoryPolicy] = %s\r\n"), 
		ITCSv(p->Policy, TOKEN_MANDATORY_POLICY_xxx));

	pbufToken->Print(DIVIDERL);

}

void DumpTokenLogonSid(HANDLE hToken, CPrintBuf* pbufToken)
{
	pbufToken->Print(DIVIDERL);

	TOKEN_GROUPS *p = (TOKEN_GROUPS*)myAllocateTokenInfo(hToken, TokenLogonSid);
	CEC_LocalFree cec = p;
	DUMPTOKEN_RETURN_ON_ERROR(TokenLogonSid);

	pbufToken->Print(_T("[TokenLogonSid] GroupCount = %d\r\n"), p->GroupCount);

	for(DWORD i=0; i<p->GroupCount; i++)
	{
		SID_AND_ATTRIBUTES &saa = p->Groups[i];

		TCHAR sidrepr[200] = {};
		SID2Repr(saa.Sid, sidrepr, ARRAYSIZE(sidrepr));
		pbufToken->Print(_T("    #%d.Sid = %s\r\n"), i, sidrepr);
		pbufToken->Print(_T("    #%d.Attributes = 0x%X %s\r\n"), i, 
			saa.Attributes, ITCSv(saa.Attributes, SE_GROUP_xxx));
	}

/*	On Win10.22H2, typically 1 group is output:
	[TokenLogonSid] GroupCount = 1
		#0.Sid = S-1-5-5-0-673378 ( "NT AUTHORITY\LogonSessionId_0_673378" )
		#0.Attributes = 0xC0000007 
			SE_GROUP_MANDATORY(0x01)|
			SE_GROUP_ENABLED_BY_DEFAULT(0x02)|
			SE_GROUP_ENABLED(0x04)|
			SE_GROUP_LOGON_ID(0xC0000000)
*/
	pbufToken->Print(DIVIDERL);
}

void DumpTokenStatistics(HANDLE hToken, CPrintBuf* pbufToken)
{
	pbufToken->Print(DIVIDERL);

	TCHAR tbuf[200] = {};

	TOKEN_STATISTICS  *p = 
		(TOKEN_STATISTICS *)myAllocateTokenInfo(hToken, TokenStatistics);
	CEC_LocalFree cec = p;
	DUMPTOKEN_RETURN_ON_ERROR(TokenStatistics);

	pbufToken->Print(_T("[TokenStatistics]\r\n"));
	pbufToken->Print(_T("    .TokenId = 0x%I64X\r\n"), *(UINT64*)&p->TokenId);
	pbufToken->Print(_T("    .AuthenticationId(LogonSessid) = 0x%X:%08X\r\n"), 
		p->AuthenticationId.HighPart, p->AuthenticationId.LowPart);

	pbufToken->Print(_T("    .ExpirationTime: %s\r\n"),
		p->ExpirationTime.QuadPart==INT64_MAX 
			? _T("(No expire)") 
			: format_wetime_as_localtime(p->ExpirationTime, tbuf, ARRAYSIZE(tbuf))
		);

	pbufToken->Print(_T("    .TokenType = %s\r\n"), ITCSv(p->TokenType, itc_TOKEN_TYPE));
	pbufToken->Print(_T("    .ImpersonationLevel = %s\r\n"), ITCSv(p->ImpersonationLevel, itc_SECURITY_IMPERSONATION_LEVEL));

	pbufToken->Print(_T("    .DynamicCharged   = %d bytes\r\n"), p->DynamicCharged);
	pbufToken->Print(_T("    .DynamicAvailable = %d bytes\r\n"), p->DynamicAvailable);
	pbufToken->Print(_T("    .GroupCount = %d\r\n"), p->GroupCount);
	pbufToken->Print(_T("    .PrivilegeCount = %d\r\n"), p->PrivilegeCount);
	pbufToken->Print(_T("    .ModifiedId = %I64u\r\n"), *(UINT64*)&p->ModifiedId);

	pbufToken->Print(DIVIDERL);
}


bool DumpTokenLinkedToken(HANDLE hToken, CPrintBuf* pbufToken, CPrintBuf* pbufLinked, int nRecurse)
{
	extern void text_DumpToken(HANDLE hToken, CPrintBuf *pbufToken, int nRecurse);

	pbufToken->Print(DIVIDERL);

	TOKEN_LINKED_TOKEN *p = 
		(TOKEN_LINKED_TOKEN*)myAllocateTokenInfo(hToken, TokenLinkedToken);
	CEC_LocalFree cec = p;
	if(!p) {
		// WinXP does not support this.
		DumpToken_report_error(_T("TokenLinkedToken"), pbufToken);
		return false;
	}

	pbufToken->Print(_T("[TokenLinkedToken] LinkedToken(handle) = 0x%X\r\n"), p->LinkedToken);
	
	if(nRecurse>0) // user wants recursive dump
		text_DumpToken(p->LinkedToken, pbufLinked, nRecurse-1);

	CloseHandle(p->LinkedToken);

	pbufToken->Print(DIVIDERL);
	return true;
}


#if 0 // as template
void DumpToken_xxx(HANDLE hToken, CPrintBuf* pbufToken)
{
	pbufToken->Print(DIVIDERL);

	DATE_TYPE *p = (DATE_TYPE*)myAllocateTokenInfo(hToken, TokenXXX);
	CEC_LocalFree cec = p;
	DUMPTOKEN_RETURN_ON_ERROR(xxx);

	pbufToken->Print(_T("[] = %s\r\n"), xxx);

	pbufToken->Print(DIVIDERL);
}
#endif

#pragma warning(pop)

///////////////////////////////////////////////////////////////////////////////

void text_DumpToken(HANDLE hToken, CPrintBuf *pbufToken, int nRecurseLinkedToken)
{
	// Chj: Show token handle value.
	pbufToken->Print(_T("Token handle value: 0x%X\r\n"), PtrToUint(hToken));

	// Display the token user
	if (!DumpTokenUser(hToken, pbufToken))
		pbufToken->Print(TEXT("Unable to dump Token User\r\n"));

	// Display the token groups
	if (!DumpTokenGroups(hToken, pbufToken))
		pbufToken->Print(TEXT("Unable to dump Token Groups\r\n"));

	// Display the token privileges
	DumpTokenPrivileges(hToken, pbufToken);

	// Display the token owner
	DumpTokenOwner(hToken, pbufToken);

	// Display the token primary group
	DumpTokenPrimaryGroup(hToken, pbufToken);

	// Display the token default DACL
	DumpTokenDefaultDacl(hToken, pbufToken);

	// Display the token source
	DumpTokenSource(hToken, pbufToken);

	// Display the token type
	DumpTokenType(hToken, pbufToken);

	// Display the token restricted sids
	if (!DumpTokenRestrictedSids(hToken, pbufToken))
		pbufToken->Print(TEXT("Unable to dump Token Restricted Sids\r\n"));

	DumpTokenSessionId(hToken, pbufToken);

	DumpTokenOrigin(hToken, pbufToken);

	DumpTokenElevation(hToken, pbufToken);
	DumpTokenElevationType(hToken, pbufToken);

	DumpTokenIntegrityLevel(hToken, pbufToken);
	DumpTokenMandatoryPolicy(hToken, pbufToken);

	DumpTokenLogonSid(hToken, pbufToken);

	DumpTokenStatistics(hToken, pbufToken);

	if(nRecurseLinkedToken>0)
	{
		CPrintBuf bufLinked; // as output param to DumpTokenLinkedToken()
		bufLinked.Print(_T("Linked-token dumping:\r\n"));
		
		bool isok = DumpTokenLinkedToken(hToken, pbufToken, &bufLinked, nRecurseLinkedToken);
		if(isok)
		{
			// Print the result to debug channel:
			pbufToken->Print(_T("Dumping linked-token to debug-channel.\r\n"));
			vaDbgTs(_T("%s\r\n"), (PCTSTR)bufLinked);
		}
	}
}

void guiDumpToken() 
{
	CPrintBuf* pbufToken = NULL;
	HANDLE hToken = g_hToken;

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

		text_DumpToken(hToken, pbufToken, 1); 
		// -- [2025-03-31] Chj: For an Admin-group user, if we pass nRecurseLinkedToken=2 or 
		// more, we'll see that TokenLinkedToken will always succeed, returning Token-handles
		// alternatingly referring to the filtered-token(non-Admin) and the full-token(Admin).

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
