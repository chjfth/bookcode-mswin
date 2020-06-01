//-------------------------------------------------------------------
// CImpersonatedUser object
//-------------------------------------------------------------------
// Copyright (c) 1997, Thuan L. Thai
//-------------------------------------------------------------------

#include "stdafx.h"
#include "ImpersonatedUser.h"

//*******************************************************************
//*******************************************************************
//*******************************************************************
//*******************************************************************
//*******************************************************************
//*******************************************************************

void S_GetExeName(TCHAR *pszExeName)
{
   // Get the full path name name of the module
   TCHAR szModuleFileName[MAX_PATH] ;
   GetModuleFileName(_Module.GetModuleInstance(), szModuleFileName, MAX_PATH);
   // Get the name of the binary executable from the full name.
   _tsplitpath(szModuleFileName, NULL, NULL, pszExeName, NULL) ;
}

//-------------------------------------------------------------------
// CImpersonatedUser::CImpersonatedUser()
//-------------------------------------------------------------------
CImpersonatedUser::CImpersonatedUser()
   :  m_hAccessToken(NULL),
      m_strUserName(""),
      m_strDomainName(""),
      m_dequeGroupPSID(),
      m_pByteGroupsUserBelongsTo(0),
      m_dequeUserGroups(),
      m_pUserTokenInfo(0)
{
}

//-------------------------------------------------------------------
// CImpersonatedUser::~CImpersonatedUser()
//-------------------------------------------------------------------
CImpersonatedUser::~CImpersonatedUser()
{
   if (m_hAccessToken) { ::CloseHandle(m_hAccessToken) ; }
   if (m_pByteGroupsUserBelongsTo) { free(m_pByteGroupsUserBelongsTo) ; }
   if (m_pUserTokenInfo) { free(m_pUserTokenInfo) ; }
}

//-------------------------------------------------------------------
// get the user SID
//-------------------------------------------------------------------
PSID
CImpersonatedUser::ObtainUserSID()
{
   if (m_pUserTokenInfo==NULL) {
      RetrieveDomainAndUserName() ;
   }
   return (((PTOKEN_USER) m_pUserTokenInfo)->User.Sid) ;
}

//-------------------------------------------------------------------
// get the user name
//-------------------------------------------------------------------
const string & 
CImpersonatedUser::ObtainUserName()
{
   if (m_strUserName.empty()) {
      RetrieveDomainAndUserName() ;
   }
   return m_strUserName ;
}

//-------------------------------------------------------------------
// get the domain name of user
//-------------------------------------------------------------------
const string & 
CImpersonatedUser::ObtainDomainName()
{
   if (m_strDomainName.empty()) {
      RetrieveDomainAndUserName() ;
   }
   return m_strDomainName ;
}

//-------------------------------------------------------------------
// get the SIDS for all the groups that the client user 
// belongs to
//-------------------------------------------------------------------
deque<PSID> * 
CImpersonatedUser::ObtainUserGroupSIDs()
{
   if (m_dequeGroupPSID.empty() && m_hAccessToken) {
      // Inquire on the size of the buffer
      DWORD dwTokenInfoBufferSize = 0 ;
      ::GetTokenInformation( m_hAccessToken, TokenGroups, 
            0, 0, &dwTokenInfoBufferSize ) ;

      // Allocate memory to hold the buffer
      unsigned char *m_pByteGroupsUserBelongsTo = 
         (unsigned char*) malloc(dwTokenInfoBufferSize) ;

      // Now get the buffer 
      BOOL bRc = ::GetTokenInformation( m_hAccessToken, TokenGroups, 
                  m_pByteGroupsUserBelongsTo, dwTokenInfoBufferSize,
                  &dwTokenInfoBufferSize ) ;
      if (!bRc) { return NULL ; }

      DWORD dwGroupCount = ((TOKEN_GROUPS*)m_pByteGroupsUserBelongsTo)->GroupCount ;

      for (int i=0; i< dwGroupCount; i++) {
         PSID pSid = ((TOKEN_GROUPS*)m_pByteGroupsUserBelongsTo)->Groups[i].Sid ;
         if (IsValidSid(pSid)) {
            // Creating deque of group SIDs
            m_dequeGroupPSID.push_back( pSid ) ;
            string strUserGroup = UserGroupBySID(pSid) ;
            if (strUserGroup!="") {
               // Creating deque of group names
               m_dequeUserGroups.push_back(strUserGroup) ;
            }
         }
      }  // for
   }

   return ( &m_dequeGroupPSID ) ;
}

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
deque<string> *
CImpersonatedUser::ObtainUserGroupNames()
{
   if (m_dequeUserGroups.size()==0) {
      ObtainUserGroupSIDs() ;
   }

   return &m_dequeUserGroups;
}

//-------------------------------------------------------------------
// impersonate and obtain the access token of executing thread
//-------------------------------------------------------------------
void
CImpersonatedUser::ObtainAccessToken(IServerSecurity *pSecure)
{
   // Get the access token of the current thread first.
   // The current thread is running in the caller's security context.
   // Thus, we have the access token of the caller.
   HANDLE hThread = ::GetCurrentThread() ;
   if (hThread==NULL) {
      return ;
   }
   // You have to impersonate the caller before you can get 
   // the access token of the caller.
   // This also requires the the caller has granted the server
   // impersonating rights.
   HRESULT hr = pSecure->ImpersonateClient(); 
   if (!::OpenThreadToken(hThread, TOKEN_QUERY, FALSE, &m_hAccessToken)) {
      _com_issue_error(GetLastError());
   }
   hr = pSecure->RevertToSelf();
}

//-------------------------------------------------------------------
// returns a group name given a group SID
//-------------------------------------------------------------------
string
CImpersonatedUser::UserGroupBySID( PSID pSid ) 
{
   DWORD dwGroupNameSize = GNLEN ;
   DWORD dwDomainNameSize = DNLEN ;
   TCHAR szGroupName[GNLEN+1];
   TCHAR szDomainName[DNLEN+1];
   SID_NAME_USE snu ;

   if (!::LookupAccountSid(
            NULL, 
            pSid,
            szGroupName,
            &dwGroupNameSize,
            szDomainName,
            &dwDomainNameSize,
            &snu)) {
      return "" ;
   } else {
      string strGroupName ;
#ifdef UNICODE
      USES_CONVERSION;
      strGroupName = string(W2A(szGroupName)) + string("[") + 
         string(W2A(szDomainName)) + string("]");
#else
      strGroupName = string(szGroupName) + string("[") + 
         string(szDomainName) + string("]");
#endif
      return strGroupName ;
   }
}



//*******************************************************************
//*******************************************************************
//*******************************************************************
//***  private support functions
//*******************************************************************
//*******************************************************************
//*******************************************************************

//-------------------------------------------------------------------
// get the domain and user names, looking up account information
//-------------------------------------------------------------------
bool
CImpersonatedUser::RetrieveDomainAndUserName()
{
   BOOL bRc = FALSE ;
   if (m_hAccessToken) {
      DWORD dwTokenInfoBufferSize = 0;
      // Inquire on the size of the buffer
      ::GetTokenInformation(
               m_hAccessToken,
               TokenUser,
               0,
               0,
               &dwTokenInfoBufferSize) ;

      // Allocate memory to hold token buffer
      m_pUserTokenInfo = (unsigned char*) malloc (dwTokenInfoBufferSize) ;

      // Now get the token buffer.
      bRc = ::GetTokenInformation(
               m_hAccessToken,
               TokenUser,
               m_pUserTokenInfo,
               dwTokenInfoBufferSize,
               &dwTokenInfoBufferSize) ;
      if (!bRc) return false ;

      DWORD dwUserNameSize = UNLEN, dwDomainNameSize = DNLEN ;
      TCHAR szUserName[UNLEN+1], szDomainName[DNLEN+1];
      SID_NAME_USE snu ;

      bRc = ::LookupAccountSid(
               NULL, 
               ((PTOKEN_USER) m_pUserTokenInfo)->User.Sid,
               szUserName,
               &dwUserNameSize,
               szDomainName,
               &dwDomainNameSize,
               &snu) ;
      if (!bRc) return false ;

#ifdef UNICODE
      USES_CONVERSION;
      m_strUserName = W2A(szUserName) ;
      m_strDomainName = W2A(szDomainName) ;
#else
      m_strUserName = szUserName ;
      m_strDomainName = szDomainName ;
#endif

      bRc = TRUE ;
   }
   return (bRc?true:false) ;
}

