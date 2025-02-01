#include <windows.h>
#include <shellapi.h>

#include <AclUI.h>
#include <PrSht.h>
#include <sddl.h>
#include <Dbt.h> // DBT_DEVICEARRIVAL 

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <wchar.h>

#include <vaDbg.h>

#include "chjutils.h"

struct Const2Str_st
{
	int Const;
	const TCHAR *Str;
};

#define ITEM_Const2Str(macroname) { macroname, _T( #macroname ) }

template<size_t arsize>
const TCHAR *Const2Str(
	const Const2Str_st (&armap)[arsize], int Const, bool ret_known=false)
{
	for(int i=0; i<arsize; i++)
	{
		if(armap[i].Const==Const)
			return armap[i].Str;
	}

	return ret_known ? _T("Unknown") : NULL;
}

////

#include "winerrs.partial.cpp"

const TCHAR * Winerr2Str(DWORD winerr)
{
	return Const2Str(gar_Winerr2Str, winerr, true);
}

const TCHAR *WinerrStr(DWORD winerr)
{
	static TCHAR s_retbuf[80] = {};
	s_retbuf[0] = 0;

	if (winerr==0 || winerr==(DWORD)-1)
		winerr = GetLastError();

	const TCHAR *errstr = Const2Str(gar_Winerr2Str, winerr, false);

	if(errstr)
	{
		_sntprintf_s(s_retbuf, _TRUNCATE, _T("WinErr=%d (%s)"), winerr, errstr);
	}
	else
	{
		TCHAR szErrDesc[200] = {};
		DWORD retchars = FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, 
			winerr,
			MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), // LANGID
			szErrDesc, ARRAYSIZE(szErrDesc)-1,
			NULL); 

		if(retchars>0)
		{
			_sntprintf_s(s_retbuf, _TRUNCATE, _T("WinErr=%d, %s"), winerr, szErrDesc);
		}
		else
		{
			_sntprintf_s(s_retbuf, _TRUNCATE, 
				_T("WinErr=%d (FormatMessage does not known this error-code)"), 
				winerr);
		}
	}

	return s_retbuf;
}

const TCHAR *app_WinErrStr(DWORD winerr)
{
	return WinerrStr(winerr);
}


//////////////////////////////////////////////////////////////////////////

TCHAR *parse_cmdparam_TCHARs(
	const TCHAR *T_cmdline, bool single_param_as_literal,
	TCHAR outbuf[], int outbuflen, int *p_retlen,
	TCHAR out_szliteral[], int out_szliteral_buflen)
{
/*  T_cmdline should points to string from GetCommandLineA()/GetCommandLineW().

	If TCHAR is char (ANSI version), outbuf[] will be a char array.
	If TCHAR is WCHAR (Unicode version), outbuf[] will be a WCHAR array.

	Actual output TCHAR count will be returned in *p_retlen.

	If user pass more than one parameter, like this:

		EXENAME 41 42 7535 43 44

	Then each parameter will be considered a TCHAR in hex representation, 
	and user gets 5 TCHARs on return.

	So in ANSI version, user will get sth. equivalent to:

		const char *outbuf = "\x41\x42\x35\x43\x44";

	In Unicode version, user will get sth. equivalent to:
		
		const WCHAR *outbuf = "\x0041\x0042\x7535\x0043\x0044";

	If only one parameter is given, like this:

		EXENAME "AB cde"

	The string of "AB cde" (6 TCHARs) will be returned.

	// [2025-01-24] out_szliteral[] looks stupid, forgot why I once design that.

*/
	//const TCHAR *T_cmdline = GetCommandLine();
	const WCHAR *W_cmdline = nullptr;
	
	if(out_szliteral)
		out_szliteral[0] = '\0';

#ifdef UNICODE
	W_cmdline = T_cmdline;
#else
	WCHAR W_buf[1024] = {};
	W_cmdline = W_buf;
	MultiByteToWideChar(CP_ACP, 0, T_cmdline, -1, W_buf, ARRAYSIZE(W_buf));		
#endif

	int argc = 0;
	WCHAR **argv = CommandLineToArgvW(W_cmdline, &argc);
	// -- argv[0] is exepath itself.

	if(single_param_as_literal && argc==2)
	{
		// not hexform
#ifdef UNICODE
		_tcscpy_s(outbuf, outbuflen, argv[1]);
		int retlen = (int)wcslen(argv[1]);
#else
		int retlen = WideCharToMultiByte(CP_ACP, 0, argv[1], -1, outbuf, outbuflen, NULL, NULL);
#endif
		if(p_retlen)
			*p_retlen = retlen;

		if(out_szliteral)
			_tcscpy_s(out_szliteral, out_szliteral_buflen, outbuf);

		LocalFree(argv);
		return outbuf;
	}

	int cycles = min(outbuflen-1, argc-1);

	int i;
	for(i=1; i<=cycles; i++)
	{
		outbuf[i-1] = (TCHAR)wcstoul(argv[i], nullptr, 16);
	}

	// To make it caller friendly, we always append a NUL char, but don't count it.
	outbuf[cycles] = '\0';

	if(p_retlen)
		*p_retlen = cycles;

	LocalFree(argv);
	return outbuf;
}

//////////////////////////////////////////////////////////////////////////

void functionbody_NeedDebugInfo()
{
	SECURITY_DESCRIPTOR sd = {};
	SID sid = {};
}

TCHAR * SID2Repr(PSID pvSid, TCHAR buf[], int buflen)
{
	// Convert an SID to text Representation, like:
	// todo: add example 

	if(!pvSid)
	{
		_sntprintf_s(buf, buflen, _TRUNCATE, _T("( none )"));
		return buf;
	}

	if(!IsValidSid(pvSid))
	{
		_sntprintf_s(buf, buflen, _TRUNCATE, _T("( invalid! )"));
		return buf;
	}

	BOOL succ = 0;
	SID *pSid = (SID*)pvSid;
	TCHAR *pszSid = nullptr;

	succ = ConvertSidToStringSid(pvSid, &pszSid);
	assert(succ);


	const int BUFSZ1 = 1024;
	TCHAR szTrusteeNam[BUFSZ1] = {}, szTrusteeDom[BUFSZ1];
	SID_NAME_USE sidType = SidTypeUnknown; // any, as output
	DWORD olen1 = BUFSZ1, olen2 = BUFSZ1;
	succ = LookupAccountSid(NULL, pvSid, szTrusteeNam, &olen1, szTrusteeDom, &olen2, &sidType);

	if(szTrusteeNam[0])
	{
		// corresponding name found.
		assert(succ);

		_sntprintf_s(buf, buflen, _TRUNCATE,
			_T("%s ( %s\\%s )")
			,
			pszSid, 
			szTrusteeDom[0] ? szTrusteeDom : _T("."), 
			szTrusteeNam
			);
	}
	else
	{
		DWORD winerr = GetLastError();
		assert(!succ && winerr==ERROR_NONE_MAPPED);
		_sntprintf_s(buf, buflen, _TRUNCATE, _T("( no mapped name )"));
	}

	LocalFree(pszSid);
	return buf;
}

TCHAR * ACL2ReprShort(BOOL isPresent, PACL pAcl, TCHAR *buf, int buflen)
{
	if(!isPresent)
	{
		_sntprintf_s(buf, buflen, _TRUNCATE, _T("(none)"));
		return buf;
	}
	else if(!pAcl)
	{
		_sntprintf_s(buf, buflen, _TRUNCATE, _T("(Null-ACL)"));
		return buf;
	}
	else
	{
		_sntprintf_s(buf, buflen, _TRUNCATE, _T("(non-Null)"));
		return buf;
	}

	return nullptr; // Guard for future code-adjusting
}


void CH10_DumpACL( PACL pACL )
{
	// Due to using ITCS(), we cannot use __try{} here.

	if (pACL == NULL){
		vaDbgS(TEXT("NULL DACL"));
		return;
	}

	ACL_SIZE_INFORMATION aclSize = {0};
	if (!GetAclInformation(pACL, &aclSize, sizeof(aclSize), AclSizeInformation))
		return;

	vaDbgS(TEXT("ACL ACE count: %d %s"), 
		aclSize.AceCount, 
		aclSize.AceCount==0 ? _T("(=empty ACL)") : _T("")
		);

	for (ULONG lIndex = 0;lIndex < aclSize.AceCount;lIndex++)
	{
		ACCESS_ALLOWED_ACE* pACE;
		if (!GetAce(pACL, lIndex, (PVOID*)&pACE))
			return;

		vaDbgS(TEXT("ACE #%d/%d :"), lIndex+1, aclSize.AceCount);

		PSID pSID = PSIDFromPACE(pACE);
		TCHAR szSidRepr[100] = {};
		SID2Repr(pSID, szSidRepr, ARRAYSIZE(szSidRepr));
		vaDbgS(TEXT("  ACE SID = %s"), szSidRepr);

		vaDbgS(TEXT("  ACE Type = %s"), ITCSv(pACE->Header.AceType, xxx_ACE_TYPE));
		vaDbgS(TEXT("  ACE Flags = %s"), ITCSv(pACE->Header.AceFlags, xxx_ACE_flags));

		TCHAR bitbufs[40] = {};
		ULONG lIndex2 = (ULONG)1<<31;
		for(int i=0; i<32; i++){
			bitbufs[i] = ((pACE->Mask & lIndex2) != 0)?TEXT('1'):TEXT('0');
			lIndex2>>=1;
		}
		vaDbgS(TEXT("  ACE Mask (31->0) = %s"), bitbufs);
	}
}

static bool IsSameBool(BOOL a, BOOL b)
{
	return (a ^ b)==0;
}

void CH10_DumpSD( PSECURITY_DESCRIPTOR pvsd )
{
	BOOL succ = 0;
	SECURITY_DESCRIPTOR *psd = (SECURITY_DESCRIPTOR*)pvsd;
	const int BUFSIZ1 = 1024;

	// NOTE: SECURITY_DESCRIPTOR's ptr-member(.Owner, .Dacl etc) may be a *offset value*,
	// or a real pointer; this is determined by SE_SELF_RELATIVE(0x8000) flag.
	// To get the real pointers, we have call respective GetSecurityDescriptorXXX APIs.

	PACL pDACL = nullptr, pSACL = nullptr;
	BOOL fPresentDacl=0, fPresentSacl=0;
	BOOL fDefaultedDacl=0, fDefaultedSacl;
	succ = GetSecurityDescriptorDacl(pvsd, &fPresentDacl, &pDACL, &fDefaultedDacl);
	assert( IsSameBool(fPresentDacl, (psd->Control&SE_DACL_PRESENT)?TRUE:FALSE) );
	succ = GetSecurityDescriptorSacl(pvsd, &fPresentSacl, &pSACL, &fDefaultedSacl);
	assert( IsSameBool(fPresentSacl, (psd->Control&SE_SACL_PRESENT)?TRUE:FALSE) );
	//
	PSID psidOwner = nullptr, psidGroup = nullptr;
	BOOL fOwnerDefaulted=0, fGroupDefaulted=0;
	succ = GetSecurityDescriptorOwner(pvsd, &psidOwner, &fOwnerDefaulted);
	assert(succ);
	succ = GetSecurityDescriptorGroup(pvsd, &psidGroup, &fGroupDefaulted);
	assert(succ);

	DWORD sdlen = GetSecurityDescriptorLength(pvsd);

	TCHAR szOwnerRepr[BUFSIZ1]={}, szGroupRepr[BUFSIZ1]={};
	const int BUF20=20; TCHAR szDACL[BUF20]={}, szSACL[BUF20]={};
	vaDbgS(
		_T("SD Dump on (0x%p), .Revision=%d, length=%d\n")
		_T("  Control(flags) = %s\n")
		_T("  OwnerSID = %s\n")
		_T("  GroupSID = %s\n")
		_T("  DACL: %s ; SACL: %s")
		,
		(void*)psd, psd->Revision, sdlen,
		ITCSv(psd->Control, SE_xxx_sdControl),
		SID2Repr(psidOwner, szOwnerRepr, BUFSIZ1), 
		SID2Repr(psidGroup, szGroupRepr, BUFSIZ1),
		ACL2ReprShort(fPresentDacl, pDACL, szDACL, BUF20), ACL2ReprShort(fPresentSacl, pSACL, szSACL, BUF20)
		);

	if(pDACL)
	{
		vaDbgS(_T("Dump DACL below:"));
		CH10_DumpACL(pDACL);
	}
	if(pSACL)
	{
		vaDbgS(_T("Dump SACL below:"));
		CH10_DumpACL(pSACL);
	}
}

