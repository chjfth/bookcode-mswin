#pragma once


void DumpSID(PSID psid, CPrintBuf* pbufToken);

void DumpSIDAttributes(DWORD dwAttrib, CPrintBuf* pbufToken);

BOOL DumpTokenGroups(HANDLE hToken, CPrintBuf* pbufToken);

void DumpTokenPrivileges(HANDLE hToken, CPrintBuf* pbufToken);

void DumpTokenOwner(HANDLE hToken, CPrintBuf* pbufToken);

void DumpTokenPrimaryGroup(HANDLE hToken, CPrintBuf* pbufToken);

void DumpTokenDefaultDacl(HANDLE hToken, CPrintBuf* pbufToken);

void DumpTokenSource(HANDLE hToken, CPrintBuf* pbufToken);

void DumpTokenImpersonationLevel(HANDLE hToken, CPrintBuf* pbufToken);

void DumpTokenType(HANDLE hToken, CPrintBuf* pbufToken);

BOOL DumpTokenRestrictedSids(HANDLE hToken, CPrintBuf* pbufToken);

void guiDumpToken();




