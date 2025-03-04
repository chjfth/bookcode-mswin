#pragma once

#include <windows.h>
#include <ACLAPI.h>
#include <ACLUI.h>
#include <sddl.h>

#include <tchar.h>
#include <assert.h>



#define PSIDFromPACE(pACE) ((PSID)(&((pACE)->SidStart))) // CH10.2

TCHAR* SID2Repr(PSID pvSid, TCHAR buf[], int buflen);


typedef TCHAR* FUNC_InterpretRights(DWORD RightBits, void *userctx);
// -- RightBits refers to ACCESS_ALLOWED_ACE.Mask
//    The returned string pointer should be C++-deleted by caller

void CH10_DumpACL( PACL pACL, 
	FUNC_InterpretRights *procItr=nullptr, void *userctx=nullptr);

void CH10_DumpSD(PSECURITY_DESCRIPTOR pvsd, 
	FUNC_InterpretRights *procItr=nullptr, void *userctx=nullptr);




