#pragma once

// [2025-02-27] Chj: Use my new EnsureClnup.h to implement this.

#include <EnsureClnup_mswin.h>

MakeCleanupPtrClass_winapi(CEnsureCloseHandle, BOOL, CloseHandle, HANDLE)
MakeCleanupClass_winapi(CEnsureCloseFile, BOOL, CloseHandle, HANDLE, INVALID_HANDLE_VALUE)

MakeCleanupPtrClass_winapi(CEnsureLocalFree, HLOCAL, LocalFree, HLOCAL)
MakeCleanupPtrClass_winapi(CEnsureGlobalFree, HGLOBAL, GlobalFree, HGLOBAL)
MakeCleanupPtrClass_winapi(CEnsureRegCloseKey, LONG, RegCloseKey, HKEY)
MakeCleanupPtrClass_winapi(CEnsureCloseServiceHandle, BOOL, CloseServiceHandle, SC_HANDLE)
MakeCleanupPtrClass_winapi(CEnsureCloseWindowStation, BOOL, CloseWindowStation, HWINSTA)
MakeCleanupPtrClass_winapi(CEnsureCloseDesktop, BOOL, CloseDesktop, HDESK)
MakeCleanupPtrClass_winapi(CEnsureUnmapViewOfFile, BOOL, UnmapViewOfFile, LPCVOID)
MakeCleanupPtrClass_winapi(CEnsureFreeLibrary, BOOL, FreeLibrary, HMODULE)
