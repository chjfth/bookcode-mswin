#pragma once

#include "..\ClassLib\SecInfo.h"      // See Appendix B.

class CSecurityPage: public CSecInfo {
public:
	CSecurityPage(PSECURITY_DESCRIPTOR pSD);
	~CSecurityPage();

private:   
	PSECURITY_DESCRIPTOR m_pSD; 
	static SI_ACCESS m_siAccessAllRights[];

private:
	HRESULT WINAPI GetObjectInformation(PSI_OBJECT_INFO pObjectInfo);
	HRESULT WINAPI GetSecurity(SECURITY_INFORMATION RequestedInformation,
		PSECURITY_DESCRIPTOR* ppSecurityDescriptor, BOOL fDefault);
	STDMETHOD(SetSecurity) (SECURITY_INFORMATION SecurityInformation,
		PSECURITY_DESCRIPTOR pSecurityDescriptor);
	STDMETHOD(GetAccessRights) (const GUID* pguidObjectType, DWORD dwFlags,
		PSI_ACCESS* ppAccess, PULONG pcAccesses, PULONG piDefaultAccess);
	STDMETHOD(MapGeneric) (const GUID* pguidObjectType, PUCHAR pAceFlags,
		PACCESS_MASK pMask);
	HRESULT WINAPI GetInheritTypes(PSI_INHERIT_TYPE* ppInheritTypes, ULONG *pcInheritTypes);

	BOOL UseSDCopy(PSECURITY_DESCRIPTOR psd);
};


