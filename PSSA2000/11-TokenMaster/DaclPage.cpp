#include <AclUI.h>

#define SECINFO_IMPL
#include "..\ClassLib\SecInfo.h"      // See Appendix B.

#include "DaclPage.h"

extern HANDLE g_hToken;


HRESULT CSecurityPage::GetObjectInformation(PSI_OBJECT_INFO pObjectInfo) 
{
	pObjectInfo->dwFlags = SI_EDIT_PERMS;
	pObjectInfo->hInstance = GetModuleHandle(NULL);
	pObjectInfo->pszServerName = NULL; 
	pObjectInfo->pszObjectName = L"Default DACL";

	return(S_OK);
}


///////////////////////////////////////////////////////////////////////////////


HRESULT CSecurityPage::SetSecurity(SECURITY_INFORMATION SecurityInformation,
	PSECURITY_DESCRIPTOR pSecurityDescriptor) 
{
	HRESULT hResult = 1;

	try {{

		if (g_hToken == NULL)
			goto leave;

		BOOL fBool;
		TOKEN_DEFAULT_DACL tokenDacl;
		if (!GetSecurityDescriptorDacl(pSecurityDescriptor, &fBool,
			&(tokenDacl.DefaultDacl), &fBool))
			goto leave;

		if (!SetTokenInformation(g_hToken, TokenDefaultDacl, &tokenDacl,
			sizeof(tokenDacl)))
			goto leave;

		Modified();
		hResult = S_OK;

	} leave:;
	} catch(...) {}

	return(hResult);
}


///////////////////////////////////////////////////////////////////////////////


SI_ACCESS CSecurityPage::m_siAccessAllRights[] = { //"winnt.h"
	{&m_guidNULL, GENERIC_ALL,     L"GENERIC_ALL",     SI_ACCESS_GENERAL },
	{&m_guidNULL, GENERIC_READ,    L"GENERIC_READ",    SI_ACCESS_GENERAL },
	{&m_guidNULL, GENERIC_WRITE,   L"GENERIC_WRITE",   SI_ACCESS_GENERAL },
	{&m_guidNULL, GENERIC_EXECUTE, L"GENERIC_EXECUTE", SI_ACCESS_GENERAL },

	{&m_guidNULL, DELETE,       L"DELETE",       SI_ACCESS_GENERAL },
	{&m_guidNULL, READ_CONTROL, L"READ_CONTROL", SI_ACCESS_GENERAL },
	{&m_guidNULL, WRITE_DAC,    L"WRITE_DAC",    SI_ACCESS_GENERAL },
	{&m_guidNULL, WRITE_OWNER,  L"WRITE_OWNER",  SI_ACCESS_GENERAL },
	{&m_guidNULL, SYNCHRONIZE,  L"SYNCHRONIZE",  SI_ACCESS_GENERAL },

	{&m_guidNULL, STANDARD_RIGHTS_REQUIRED, L"STANDARD_RIGHTS_REQUIRED", SI_ACCESS_GENERAL }, 
	{&m_guidNULL, STANDARD_RIGHTS_READ,     L"STANDARD_RIGHTS_READ",     SI_ACCESS_GENERAL },
	{&m_guidNULL, STANDARD_RIGHTS_WRITE,    L"STANDARD_RIGHTS_WRITE",    SI_ACCESS_GENERAL },
	{&m_guidNULL, STANDARD_RIGHTS_EXECUTE,  L"STANDARD_RIGHTS_EXECUTE",  SI_ACCESS_GENERAL }, 
	{&m_guidNULL, STANDARD_RIGHTS_ALL,      L"STANDARD_RIGHTS_ALL",      SI_ACCESS_GENERAL }, 
	{&m_guidNULL, SPECIFIC_RIGHTS_ALL,      L"SPECIFIC_RIGHTS_ALL",      SI_ACCESS_GENERAL },

	{&m_guidNULL, ACCESS_SYSTEM_SECURITY, L"ACCESS_SYSTEM_SECURITY", SI_ACCESS_GENERAL },
	{&m_guidNULL, MAXIMUM_ALLOWED,        L"MAXIMUM_ALLOWED",        SI_ACCESS_GENERAL }
};


///////////////////////////////////////////////////////////////////////////////


HRESULT CSecurityPage::GetAccessRights(const GUID* pguidObjectType,
	DWORD dwFlags, PSI_ACCESS* ppAccess, PULONG pcAccesses,
	PULONG piDefaultAccess) 
{
	// Skip the Generic Access
	*ppAccess = m_siAccessAllRights;
	*pcAccesses = chDIMOF(m_siAccessAllRights);
	*piDefaultAccess = 0;

	return(S_OK);
}


///////////////////////////////////////////////////////////////////////////////


HRESULT CSecurityPage::MapGeneric(const GUID* pguidObjectType,
	PUCHAR pAceFlags, ACCESS_MASK* pMask) 
{
	return(S_OK);
}


///////////////////////////////////////////////////////////////////////////////


CSecurityPage::CSecurityPage(PSECURITY_DESCRIPTOR pSD) 
{   
	m_pSD = NULL;
	UseSDCopy(pSD);
}


CSecurityPage::~CSecurityPage() 
{
	if (m_pSD != NULL) {
		LocalFree(m_pSD);
	}
} 


///////////////////////////////////////////////////////////////////////////////


BOOL CSecurityPage::UseSDCopy(PSECURITY_DESCRIPTOR pSD) 
{    
	__try {
		if (pSD == NULL) __leave; 
		if (m_pSD != NULL) {
			LocalFree(m_pSD);
			m_pSD = NULL;
		}

		m_pSD = LocalAllocSDCopy(pSD);

	}
	__finally {
	} 
	return(m_pSD != NULL);
}


///////////////////////////////////////////////////////////////////////////////


HRESULT CSecurityPage::GetSecurity(SECURITY_INFORMATION RequestedInformation, 
	PSECURITY_DESCRIPTOR* ppSecurityDescriptor, BOOL fDefault) 
{

	HRESULT hr = 1;
	*ppSecurityDescriptor = m_pSD;
	if (m_pSD != NULL)
		hr = S_OK;
	m_pSD = NULL;
	return(hr);
}


///////////////////////////////////////////////////////////////////////////////


HRESULT CSecurityPage::GetInheritTypes(PSI_INHERIT_TYPE* ppInheritTypes, ULONG* pcInheritTypes) 
{
	*ppInheritTypes = NULL; 
	*pcInheritTypes = 0;
	return(S_OK);
}
