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

#include "InterpretConst.h"

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


#undef _tprintf
#define _tprintf xxx

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

		vaDbgS(TEXT("  ACE Type = %s"), ITCS1(pACE->Header.AceType, itc_ACE_TYPE));
		vaDbgS(TEXT("  ACE Flags = %s"), ITCS1(pACE->Header.AceFlags, itc_ACE_FLAGS));

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
		ITCS1(psd->Control, itc_SECURITY_DESCRIPTOR_CONTROL),
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



//////////////////////////////////////////////////////////////////////////



static const Enum2Val_st e2v_CSecurityInformation_PropertySheetPageCallback_uMsg[] =
{
	ITC_NAMEPAIR(PSPCB_CREATE),
	ITC_NAMEPAIR(PSPCB_RELEASE),
	ITC_NAMEPAIR(PSPCB_SI_INITDIALOG),
};
CInterpretConst itc_CSecurityInformation_PropertySheetPageCallback_uMsg(
	e2v_CSecurityInformation_PropertySheetPageCallback_uMsg, 
	ARRAYSIZE(e2v_CSecurityInformation_PropertySheetPageCallback_uMsg));
//
static const Enum2Val_st e2v_CSecurityInformation_PropertySheetPageCallback_uPage[] =
{
	ITC_NAMEPAIR(SI_PAGE_PERM),
	ITC_NAMEPAIR(SI_PAGE_ADVPERM),
	ITC_NAMEPAIR(SI_PAGE_AUDIT),
	ITC_NAMEPAIR(SI_PAGE_OWNER),
	ITC_NAMEPAIR(SI_PAGE_EFFECTIVE),
	ITC_NAMEPAIR(SI_PAGE_TAKEOWNERSHIP),
};
CInterpretConst itc_CSecurityInformation_PropertySheetPageCallback_uPage(
	e2v_CSecurityInformation_PropertySheetPageCallback_uPage, 
	ARRAYSIZE(e2v_CSecurityInformation_PropertySheetPageCallback_uPage));
//
static const Bitfield2Val_st b2v_SECURITY_INFORMATION[] =
{
	ITC_NAMEPAIR(OWNER_SECURITY_INFORMATION           ),
	ITC_NAMEPAIR(GROUP_SECURITY_INFORMATION           ),
	ITC_NAMEPAIR(DACL_SECURITY_INFORMATION            ),
	ITC_NAMEPAIR(SACL_SECURITY_INFORMATION            ),
	ITC_NAMEPAIR(LABEL_SECURITY_INFORMATION           ),
	ITC_NAMEPAIR(PROTECTED_DACL_SECURITY_INFORMATION  ),
	ITC_NAMEPAIR(PROTECTED_SACL_SECURITY_INFORMATION  ),
	ITC_NAMEPAIR(UNPROTECTED_DACL_SECURITY_INFORMATION),
	ITC_NAMEPAIR(UNPROTECTED_SACL_SECURITY_INFORMATION),
};
CInterpretConst itc_SECURITY_INFORMATION(
	b2v_SECURITY_INFORMATION, ARRAYSIZE(b2v_SECURITY_INFORMATION),
	_T("0x%08X"));
//
static const Enum2Val_st e2v_SE_OBJECT_TYPE[] =
{
	ITC_NAMEPAIR(SE_UNKNOWN_OBJECT_TYPE),
	ITC_NAMEPAIR(SE_FILE_OBJECT),
	ITC_NAMEPAIR(SE_SERVICE),
	ITC_NAMEPAIR(SE_PRINTER),
	ITC_NAMEPAIR(SE_REGISTRY_KEY),
	ITC_NAMEPAIR(SE_LMSHARE),
	ITC_NAMEPAIR(SE_KERNEL_OBJECT),
	ITC_NAMEPAIR(SE_WINDOW_OBJECT),
	ITC_NAMEPAIR(SE_DS_OBJECT),
	ITC_NAMEPAIR(SE_DS_OBJECT_ALL),
	ITC_NAMEPAIR(SE_PROVIDER_DEFINED_OBJECT),
	ITC_NAMEPAIR(SE_WMIGUID_OBJECT),
	ITC_NAMEPAIR(SE_REGISTRY_WOW64_32KEY),
};
CInterpretConst itc_SE_OBJECT_TYPE(
	e2v_SE_OBJECT_TYPE, ARRAYSIZE(e2v_SE_OBJECT_TYPE)
	);
//
static const Enum2Val_st e2v_ACE_TYPE[] =
{
	ITC_NAMEPAIR(ACCESS_ALLOWED_ACE_TYPE                 ),
	ITC_NAMEPAIR(ACCESS_DENIED_ACE_TYPE                  ),
	ITC_NAMEPAIR(SYSTEM_AUDIT_ACE_TYPE                   ),
	ITC_NAMEPAIR(SYSTEM_ALARM_ACE_TYPE                   ),
	ITC_NAMEPAIR(ACCESS_ALLOWED_COMPOUND_ACE_TYPE        ),
	ITC_NAMEPAIR(ACCESS_ALLOWED_OBJECT_ACE_TYPE          ),
	ITC_NAMEPAIR(ACCESS_DENIED_OBJECT_ACE_TYPE           ),
	ITC_NAMEPAIR(SYSTEM_AUDIT_OBJECT_ACE_TYPE            ),
	ITC_NAMEPAIR(SYSTEM_ALARM_OBJECT_ACE_TYPE            ),
	ITC_NAMEPAIR(ACCESS_ALLOWED_CALLBACK_ACE_TYPE        ),
	ITC_NAMEPAIR(ACCESS_DENIED_CALLBACK_ACE_TYPE         ),
	ITC_NAMEPAIR(ACCESS_ALLOWED_CALLBACK_OBJECT_ACE_TYPE ),
	ITC_NAMEPAIR(ACCESS_DENIED_CALLBACK_OBJECT_ACE_TYPE  ),
	ITC_NAMEPAIR(SYSTEM_AUDIT_CALLBACK_ACE_TYPE          ),
	ITC_NAMEPAIR(SYSTEM_ALARM_CALLBACK_ACE_TYPE          ),
	ITC_NAMEPAIR(SYSTEM_AUDIT_CALLBACK_OBJECT_ACE_TYPE   ),
	ITC_NAMEPAIR(SYSTEM_ALARM_CALLBACK_OBJECT_ACE_TYPE   ),
	ITC_NAMEPAIR(SYSTEM_MANDATORY_LABEL_ACE_TYPE         ),
};
CInterpretConst itc_ACE_TYPE(e2v_ACE_TYPE, ARRAYSIZE(e2v_ACE_TYPE),
	_T("0x%04X"));
//
static const Bitfield2Val_st b2v_ACE_FLAGS[] =
{
	ITC_NAMEPAIR(OBJECT_INHERIT_ACE         ),
	ITC_NAMEPAIR(CONTAINER_INHERIT_ACE      ),
	ITC_NAMEPAIR(NO_PROPAGATE_INHERIT_ACE   ),
	ITC_NAMEPAIR(INHERIT_ONLY_ACE           ),
	ITC_NAMEPAIR(INHERITED_ACE              ),
	ITC_NAMEPAIR(SUCCESSFUL_ACCESS_ACE_FLAG ),
	ITC_NAMEPAIR(FAILED_ACCESS_ACE_FLAG     ),
};
CInterpretConst itc_ACE_FLAGS(b2v_ACE_FLAGS, ARRAYSIZE(b2v_ACE_FLAGS),
	_T("0x%02X"));
//
static const Bitfield2Val_st b2v_SECURITY_DESCRIPTOR_CONTROL[] =
{
	ITC_NAMEPAIR(SE_OWNER_DEFAULTED      ),
	ITC_NAMEPAIR(SE_GROUP_DEFAULTED      ),
	ITC_NAMEPAIR(SE_DACL_PRESENT         ),
	ITC_NAMEPAIR(SE_DACL_DEFAULTED       ),
	ITC_NAMEPAIR(SE_SACL_PRESENT         ),
	ITC_NAMEPAIR(SE_SACL_DEFAULTED       ),
	ITC_NAMEPAIR(SE_DACL_AUTO_INHERIT_REQ),
	ITC_NAMEPAIR(SE_SACL_AUTO_INHERIT_REQ),
	ITC_NAMEPAIR(SE_DACL_AUTO_INHERITED  ),
	ITC_NAMEPAIR(SE_SACL_AUTO_INHERITED  ),
	ITC_NAMEPAIR(SE_DACL_PROTECTED       ),
	ITC_NAMEPAIR(SE_SACL_PROTECTED       ),
	ITC_NAMEPAIR(SE_RM_CONTROL_VALID     ),
	ITC_NAMEPAIR(SE_SELF_RELATIVE        ),
};
CInterpretConst itc_SECURITY_DESCRIPTOR_CONTROL(
	b2v_SECURITY_DESCRIPTOR_CONTROL, ARRAYSIZE(b2v_SECURITY_DESCRIPTOR_CONTROL),
	_T("0x%04X"));
//
static const Bitfield2Val_st b2v_SI_ACCESS_flags[] =
{
	ITC_NAMEPAIR(SI_ACCESS_SPECIFIC  ),
	ITC_NAMEPAIR(SI_ACCESS_GENERAL   ),
	ITC_NAMEPAIR(SI_ACCESS_CONTAINER ), // general access, container-only
	ITC_NAMEPAIR(SI_ACCESS_PROPERTY  ),
	ITC_NAMEPAIR(CONTAINER_INHERIT_ACE),
	ITC_NAMEPAIR(INHERIT_ONLY_ACE),
	ITC_NAMEPAIR(OBJECT_INHERIT_ACE),
};
CInterpretConst itc_SI_ACCESS_flags(
	b2v_SI_ACCESS_flags, ARRAYSIZE(b2v_SI_ACCESS_flags),
	_T("0x%08X"));
//
static const Bitfield2Val_st b2v_SI_OBJECT_INFO_flags[] =
{
//	ITC_NAMEPAIR(SI_EDIT_PERMS               ), // 0x00000000L // always implied
	ITC_NAMEPAIR(SI_EDIT_OWNER               ), // 0x00000001L
	ITC_NAMEPAIR(SI_EDIT_AUDITS              ), // 0x00000002L
	ITC_NAMEPAIR(SI_CONTAINER                ), // 0x00000004L
	ITC_NAMEPAIR(SI_READONLY                 ), // 0x00000008L
	ITC_NAMEPAIR(SI_ADVANCED                 ), // 0x00000010L
	ITC_NAMEPAIR(SI_RESET                    ), // 0x00000020L //equals to SI_RESET_DACL|SI_RESET_SACL|SI_RESET_OWNER
	ITC_NAMEPAIR(SI_OWNER_READONLY           ), // 0x00000040L
	ITC_NAMEPAIR(SI_EDIT_PROPERTIES          ), // 0x00000080L
	ITC_NAMEPAIR(SI_OWNER_RECURSE            ), // 0x00000100L
	ITC_NAMEPAIR(SI_NO_ACL_PROTECT           ), // 0x00000200L
	ITC_NAMEPAIR(SI_NO_TREE_APPLY            ), // 0x00000400L
	ITC_NAMEPAIR(SI_PAGE_TITLE               ), // 0x00000800L
	ITC_NAMEPAIR(SI_SERVER_IS_DC             ), // 0x00001000L
	ITC_NAMEPAIR(SI_RESET_DACL_TREE          ), // 0x00004000L
	ITC_NAMEPAIR(SI_RESET_SACL_TREE          ), // 0x00008000L
	ITC_NAMEPAIR(SI_OBJECT_GUID              ), // 0x00010000L
	ITC_NAMEPAIR(SI_EDIT_EFFECTIVE           ), // 0x00020000L
	ITC_NAMEPAIR(SI_RESET_DACL               ), // 0x00040000L
	ITC_NAMEPAIR(SI_RESET_SACL               ), // 0x00080000L
	ITC_NAMEPAIR(SI_RESET_OWNER              ), // 0x00100000L
	ITC_NAMEPAIR(SI_NO_ADDITIONAL_PERMISSION ), // 0x00200000L
	ITC_NAMEPAIR(SI_VIEW_ONLY                ), // 0x00400000L
	ITC_NAMEPAIR(SI_PERMS_ELEVATION_REQUIRED ), // 0x01000000L
	ITC_NAMEPAIR(SI_AUDITS_ELEVATION_REQUIRED), // 0x02000000L
	ITC_NAMEPAIR(SI_OWNER_ELEVATION_REQUIRED ), // 0x04000000L
	ITC_NAMEPAIR(SI_MAY_WRITE                ), // 0x10000000L //not sure if user can write permission
};
CInterpretConst itc_SI_OBJECT_INFO_flags(
	b2v_SI_OBJECT_INFO_flags, ARRAYSIZE(b2v_SI_OBJECT_INFO_flags),
	_T("0x%08X"));
//
static const Bitfield2Val_st b2v_SI_INHERIT_TYPE_flags[] =
{
	ITC_NAMEPAIR(CONTAINER_INHERIT_ACE),
	ITC_NAMEPAIR(INHERIT_ONLY_ACE),
	ITC_NAMEPAIR(OBJECT_INHERIT_ACE),
};
CInterpretConst itc_SI_INHERIT_TYPE_flags(
	b2v_SI_INHERIT_TYPE_flags, ARRAYSIZE(b2v_SI_INHERIT_TYPE_flags),
	_T("0x%02X"));


static const Enum2Val_st e2v_SERVICE_CONTROL[] =
{
	ITC_NAMEPAIR(SERVICE_CONTROL_STOP                   ),
	ITC_NAMEPAIR(SERVICE_CONTROL_PAUSE                  ),
	ITC_NAMEPAIR(SERVICE_CONTROL_CONTINUE               ),
	ITC_NAMEPAIR(SERVICE_CONTROL_INTERROGATE            ),
	ITC_NAMEPAIR(SERVICE_CONTROL_SHUTDOWN               ),
	ITC_NAMEPAIR(SERVICE_CONTROL_PARAMCHANGE            ),
	ITC_NAMEPAIR(SERVICE_CONTROL_NETBINDADD             ),
	ITC_NAMEPAIR(SERVICE_CONTROL_NETBINDREMOVE          ),
	ITC_NAMEPAIR(SERVICE_CONTROL_NETBINDENABLE          ),
	ITC_NAMEPAIR(SERVICE_CONTROL_NETBINDDISABLE         ),
	ITC_NAMEPAIR(SERVICE_CONTROL_DEVICEEVENT            ),
	ITC_NAMEPAIR(SERVICE_CONTROL_HARDWAREPROFILECHANGE  ),
	ITC_NAMEPAIR(SERVICE_CONTROL_POWEREVENT             ),
	ITC_NAMEPAIR(SERVICE_CONTROL_SESSIONCHANGE          ),
	ITC_NAMEPAIR(SERVICE_CONTROL_PRESHUTDOWN            ),
	ITC_NAMEPAIR(SERVICE_CONTROL_TIMECHANGE             ),
	ITC_NAMEPAIR(SERVICE_CONTROL_TRIGGEREVENT           ),
};
CInterpretConst itc_SERVICE_CONTROL(e2v_SERVICE_CONTROL, ARRAYSIZE(e2v_SERVICE_CONTROL),
	_T("0x%02X"));

static const Enum2Val_st e2v_DBT[] =
{
	ITC_NAMEPAIR(DBT_DEVICEARRIVAL           ),
	ITC_NAMEPAIR(DBT_DEVICEQUERYREMOVE       ),
	ITC_NAMEPAIR(DBT_DEVICEQUERYREMOVEFAILED ),
	ITC_NAMEPAIR(DBT_DEVICEREMOVEPENDING     ),
	ITC_NAMEPAIR(DBT_DEVICEREMOVECOMPLETE    ),
	ITC_NAMEPAIR(DBT_DEVICETYPESPECIFIC      ),
	ITC_NAMEPAIR(DBT_CUSTOMEVENT             ),
/* 
	[2025-01-25] Weird, these four are missing from Win10 SDK.
	D:\WinKits\10\Include\10.0.22621.0\um\Dbt.h

	ITC_NAMEPAIR(DBT_DEVINSTENUMERATED       ),
	ITC_NAMEPAIR(DBT_DEVINSTSTARTED          ),
	ITC_NAMEPAIR(DBT_DEVINSTREMOVED          ),
	ITC_NAMEPAIR(DBT_DEVINSTPROPERTYCHANGED  ),
*/
};
CInterpretConst itc_DBT(e2v_DBT, ARRAYSIZE(e2v_DBT),
	_T("0x%04X"));

