#pragma once

#include <windows.h>
#include <ACLAPI.h>
#include <ACLUI.h>
#include <sddl.h>

#include <tchar.h>
#include <assert.h>

#define PSIDFromPACE(pACE) ((PSID)(&((pACE)->SidStart))) // CH10.2

TCHAR * SID2Repr(PSID pvSid, TCHAR buf[], int buflen);

void CH10_DumpACL( PACL pACL );
void CH10_DumpSD( PSECURITY_DESCRIPTOR pvsd );

