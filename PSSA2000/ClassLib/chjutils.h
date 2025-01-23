#ifndef __chjutils_h_20250123_
#define __chjutils_h_20250123_

#include <EnsureClnup_mswin.h>



MakeCleanupPtrClass_winapi(Cec_LsaClose, NTSTATUS, LsaClose, LSA_HANDLE)
// -- for LsaOpenPolicy

MakeCleanupPtrClass_winapi(Cec_LsaFreeMemory, NTSTATUS, LsaFreeMemory, PVOID)
// -- for LsaLookupNames2

MakeCleanupPtrClass_winapi(Cec_LocalFree, HLOCAL, LocalFree, HLOCAL)
// -- for ConvertSidToStringSid

MakeCleanupPtrClass_winapi(Cec_FreeSid, PVOID, FreeSid, PSID)
// -- for AllocateAndInitializeSid

MakeCleanupPtrClass_winapi(Cec_NetApiBufferFree, NET_API_STATUS, NetApiBufferFree, PVOID)


#endif
