#include "shareinc.h"

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

void DumpSID(PSID psid, CPrintBuf* pbufToken) 
{
	PTSTR szSIDText = NULL;

	try {{

		SID_NAME_USE snuUse;
		PTSTR pszUse;
		TCHAR szName[255]    = {TEXT("[Logon SID]")};
		DWORD dwSize         = chDIMOF(szName);
		TCHAR szDomName[255] = {TEXT("")};
		DWORD dwDomSize      = chDIMOF(szDomName);

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

			default:
				pszUse = TEXT("Unknown SID");
				break;
			}

		} else {

			// We couldn't look it up, maybe it is the logon SID
			PSID_IDENTIFIER_AUTHORITY psia = GetSidIdentifierAuthority(psid);
			DWORD dwFirstSub = *GetSidSubAuthority(psid, 0);
			if (dwFirstSub == 5 && psia->Value[6] == 5)
				pszUse = TEXT("Logon SID");
			else
				pszUse = TEXT("");
		}

		// Get the textual SID
		dwSize = 0;
		GetTextualSid(psid, szSIDText, &dwSize);
		szSIDText = (PTSTR) LocalAlloc(LPTR, dwSize);
		if (szSIDText == NULL)
			goto leave;
		if (!GetTextualSid(psid, szSIDText, &dwSize))
			goto leave;

		// Output the things we have learned
		pbufToken->Print(TEXT("SID:\t\t"));
		pbufToken->Print(szSIDText);

		pbufToken->Print(TEXT("\r\nUse:\t\t"));
		pbufToken->Print(pszUse);

		pbufToken->Print(TEXT("\r\nName:\t\t"));
		pbufToken->Print(szName);

		pbufToken->Print(TEXT("\r\nDomain Name:\t"));
		pbufToken->Print(szDomName);

		pbufToken->Print(TEXT("\r\n\r\n"));

	} leave:;
	} catch(...) {}

	if (szSIDText != NULL)
		LocalFree(szSIDText);
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

	if (dwAttrib & SE_GROUP_LOGON_ID)
		pbufToken->Print(TEXT("\t\tSE_GROUP_LOGON_ID\r\n"));
}

BOOL DumpTokenGroups(HANDLE hToken, CPrintBuf* pbufToken) 
{
	BOOL          fSuccess  = FALSE;
	PTOKEN_GROUPS ptgGroups = NULL;

	try {{

		// Get token information
		ptgGroups = (PTOKEN_GROUPS) AllocateTokenInfo(hToken, TokenGroups);
		if (ptgGroups == NULL)
			goto leave;

		pbufToken->Print(DIVIDERL TEXT("GROUP SIDS\r\n"));
		pbufToken->Print(TEXT("Group Count:\t%lu"), ptgGroups->GroupCount);
		pbufToken->Print(TEXT("\r\n") DIVIDERL);

		// Dump the SID and Attributes for each group
		DWORD dwIndex = ptgGroups->GroupCount;
		for (dwIndex = 0; dwIndex < ptgGroups->GroupCount; dwIndex++) 
		{
			pbufToken->Print(TEXT("Sid #%d\r\n"), dwIndex);
			DumpSID(ptgGroups->Groups[dwIndex].Sid, pbufToken);
			DumpSIDAttributes(ptgGroups->Groups[dwIndex].Attributes, pbufToken);
			pbufToken->Print(DIVIDERS);
		}

		fSuccess = TRUE;

	} leave:;
	} catch(...) {}

	if (ptgGroups != NULL)
		LocalFree(ptgGroups);

	return(fSuccess);
}

void DumpTokenPrivileges(HANDLE hToken, CPrintBuf* pbufToken) 
{
	PTOKEN_PRIVILEGES ptpPrivileges = NULL;

	try {{

		// Get the token privilege information
		ptpPrivileges = (PTOKEN_PRIVILEGES)
			AllocateTokenInfo(hToken, TokenPrivileges);
		if (ptpPrivileges == NULL)
			goto leave;

		pbufToken->Print(DIVIDERL TEXT("Priveleges\r\n"));
		pbufToken->Print(DIVIDERL);

		DWORD dwIndex;
		for (dwIndex = 0; dwIndex < ptpPrivileges->PrivilegeCount; dwIndex++) 
		{
			// Get the privilege name and print it with attribute information
			TCHAR szName[255];
			DWORD dwSize = chDIMOF(szName);
			if (LookupPrivilegeName(NULL,
				&(ptpPrivileges->Privileges[dwIndex].Luid), szName, &dwSize)) 
			{
				pbufToken->Print(szName);
				if (ptpPrivileges->Privileges[dwIndex].Attributes
					& SE_PRIVILEGE_ENABLED_BY_DEFAULT)
					pbufToken->Print(TEXT("\r\n\t")
					TEXT("SE_PRIVILEGE_ENABLED_BY_DEFAULT"));

				if (ptpPrivileges->Privileges[dwIndex].Attributes
					& SE_PRIVILEGE_ENABLED)
					pbufToken->Print(TEXT("\r\n\tSE_PRIVILEGE_ENABLED"));

				if (ptpPrivileges->Privileges[dwIndex].Attributes
					& SE_PRIVILEGE_USED_FOR_ACCESS)
					pbufToken->Print(TEXT("\r\n\tSE_PRIVILEGE_USED_FOR_ACCESS"));

				pbufToken->Print(TEXT("\r\n"));
			}
		}

	} leave:;
	} catch(...) {}

	if (ptpPrivileges != NULL)
		LocalFree(ptpPrivileges);
}


///////////////////////////////////////////////////////////////////////////////


void DumpTokenOwner(HANDLE hToken, CPrintBuf* pbufToken) 
{
	PTOKEN_OWNER ptoOwner = NULL;

	try {{

		ptoOwner = (PTOKEN_OWNER) AllocateTokenInfo(hToken, TokenOwner);
		if (ptoOwner == NULL)
			goto leave;

		pbufToken->Print(DIVIDERL TEXT("Token Owner\r\n"));
		pbufToken->Print(DIVIDERL);
		DumpSID(ptoOwner->Owner, pbufToken);

	} leave:;
	} catch(...) {}

	if (ptoOwner != NULL)
		LocalFree(ptoOwner);
}


///////////////////////////////////////////////////////////////////////////////


void DumpTokenPrimaryGroup(HANDLE hToken, CPrintBuf* pbufToken) 
{
	PTOKEN_OWNER ptgGroup = NULL;

	try {{

		ptgGroup = (PTOKEN_OWNER) AllocateTokenInfo(hToken, TokenPrimaryGroup);
		if (ptgGroup == NULL)
			goto leave;

		pbufToken->Print(DIVIDERL TEXT("Token Primary Group\r\n"));
		pbufToken->Print(DIVIDERL);
		DumpSID(ptgGroup->Owner, pbufToken);

	} leave:;
	} catch(...) {}

	if (ptgGroup != NULL)
		LocalFree(ptgGroup);
}


///////////////////////////////////////////////////////////////////////////////


void DumpTokenDefaultDacl(HANDLE hToken, CPrintBuf* pbufToken) 
{
	PTOKEN_DEFAULT_DACL ptdDacl  = NULL;
	PEXPLICIT_ACCESS    pEntries = NULL;

	try {{

		// Get the DACL info
		ptdDacl = (PTOKEN_DEFAULT_DACL) AllocateTokenInfo(hToken,
			TokenDefaultDacl);
		if (ptdDacl == NULL)
			goto leave;

		pbufToken->Print(DIVIDERL TEXT("Token Default DACL\r\n"));
		pbufToken->Print(DIVIDERL);

		// Get ACL size information
		ACL_SIZE_INFORMATION sizeInfo;
		if (!GetAclInformation(ptdDacl->DefaultDacl, &sizeInfo, sizeof(sizeInfo),
			AclSizeInformation))
			goto leave;

		// Iterate through the aces
		DWORD dwCount = sizeInfo.AceCount;
		DWORD dwIndex;
		for (dwIndex = 0; dwIndex < dwCount; dwIndex++) 
		{
			// Get the ACE by index
			PACCESS_ALLOWED_ACE paceAllowed;
			if (GetAce(ptdDacl->DefaultDacl, dwIndex, (PVOID*) &paceAllowed)) 
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

				// Get the account name associated with the ace and output it
				TCHAR szName[255];
				TCHAR szDomain[255];

				DWORD dwSize       = chDIMOF(szName);
				DWORD dwDomainSize = chDIMOF(szDomain);

				SID_NAME_USE snuUse;
				LookupAccountSid(NULL, &(paceAllowed->SidStart), szName, &dwSize,
					szDomain, &dwDomainSize, &snuUse);

				TCHAR szAccount[255];
				lstrcpy(szAccount, szDomain);
				lstrcat(szAccount, TEXT("/"));
				lstrcat(szAccount, szName);

				pbufToken->Print(TEXT("\t"));
				pbufToken->Print(szAccount);

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

	} leave:;
	} catch(...) {}

	if (ptdDacl != NULL)
		LocalFree(ptdDacl);

	if (pEntries != NULL)
		LocalFree(pEntries);
}


///////////////////////////////////////////////////////////////////////////////


void DumpTokenSource(HANDLE hToken, CPrintBuf* pbufToken) 
{
	PTOKEN_SOURCE ptsSource = NULL;

	try {{

		ptsSource = (PTOKEN_SOURCE) AllocateTokenInfo(hToken, TokenSource);
		if (ptsSource == NULL)
			goto leave;

		TCHAR szName[9];
		int nIndex = 8;
		while (nIndex-- != 0)
			szName[nIndex] = ptsSource->SourceName[nIndex];

		szName[8] = 0;
		pbufToken->Print(DIVIDERL TEXT("Token Source:  "));
		pbufToken->Print(szName);
		pbufToken->Print(TEXT("\r\n") DIVIDERL);

	} leave:;
	} catch(...) {}

	if (ptsSource != NULL)
		LocalFree(ptsSource);
}


///////////////////////////////////////////////////////////////////////////////


void DumpTokenImpersonationLevel(HANDLE hToken, CPrintBuf* pbufToken) 
{
	PSECURITY_IMPERSONATION_LEVEL psilImpersonation = NULL;

	try {{

		psilImpersonation = (PSECURITY_IMPERSONATION_LEVEL) AllocateTokenInfo(
			hToken, TokenImpersonationLevel);
		if (psilImpersonation == NULL)
			goto leave;

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

	} leave:;
	} catch(...) {}

	if (psilImpersonation != NULL)
		LocalFree(psilImpersonation );
}


///////////////////////////////////////////////////////////////////////////////


void DumpTokenType(HANDLE hToken, CPrintBuf* pbufToken) 
{
	PTOKEN_TYPE pttType = NULL;

	try {{

		pttType = (PTOKEN_TYPE) AllocateTokenInfo(hToken, TokenType);
		if (pttType == NULL)
			goto leave;

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

	} leave:;
	} catch(...) {}

	if (pttType != NULL)
		LocalFree(pttType);
}

BOOL DumpTokenUser(HANDLE hToken, CPrintBuf* pbufToken) 
{
	BOOL        fSuccess = FALSE;
	PTOKEN_USER ptuUser  = NULL;

	try {{

		// Get the token information
		ptuUser = (PTOKEN_USER) AllocateTokenInfo(hToken, TokenUser);
		if (ptuUser == NULL)
			goto leave;

		pbufToken->Print(DIVIDERL TEXT("USER SID\r\n") DIVIDERL
			TEXT("\r\n"));
		DumpSID(ptuUser->User.Sid, pbufToken);

		fSuccess = TRUE;

	} leave:;
	} catch(...) {}

	if (ptuUser != NULL)
		LocalFree(ptuUser);

	return(fSuccess);
}

BOOL DumpTokenRestrictedSids(HANDLE hToken, CPrintBuf* pbufToken) 
{
	BOOL          fSuccess  = FALSE;
	PTOKEN_GROUPS ptgGroups = NULL;

	try {{

		BOOL fRestricted = IsTokenRestricted(hToken);
		ptgGroups = (PTOKEN_GROUPS) AllocateTokenInfo(hToken,
			TokenRestrictedSids);
		if (ptgGroups == NULL)
			goto leave;

		pbufToken->Print(DIVIDERL);
		pbufToken->Print(fRestricted ? TEXT("Token is Restricted\r\n") :
			TEXT("Token is not Restricted\r\n"));
		pbufToken->Print(TEXT("Restricted SID Count:\t%lu\r\n"),
			ptgGroups->GroupCount);
		pbufToken->Print(TEXT("Restricted SIDs\r\n"));
		pbufToken->Print(DIVIDERL);

		DWORD dwIndex = ptgGroups->GroupCount;
		for (dwIndex = 0; dwIndex < ptgGroups->GroupCount; dwIndex++) {
			pbufToken->Print(TEXT("Sid #%d\r\n"), dwIndex);
			DumpSIDAttributes(ptgGroups->Groups[dwIndex].Attributes,
				pbufToken);
			DumpSID(ptgGroups->Groups[dwIndex].Sid, pbufToken);
			pbufToken->Print(DIVIDERS);
		}

		fSuccess = TRUE;

	} leave:;
	} catch(...) {}

	if (ptgGroups != NULL)
		LocalFree(ptgGroups);

	return(fSuccess);
}


///////////////////////////////////////////////////////////////////////////////


void DumpToken() 
{
	CPrintBuf* pbufToken = NULL;

	try {{

		// No token?  Then slip out the back jack
		if (g_hToken == NULL)
			goto leave;

		// Update other controls
		UpdatePrivileges();
		UpdateGroups();
		SendMessage(g_hwndRestrictedSids, LB_RESETCONTENT, 0, 0);

		// Create a print buf object for buffering output to the edit control.
		// This class and usage instructions can be found in the "CmnCls.h"
		pbufToken = new CPrintBuf;
		if (pbufToken == NULL)
			goto leave;

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

		// Display the token default dacl
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
