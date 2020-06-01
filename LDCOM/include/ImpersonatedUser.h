//-------------------------------------------------------------------
// CImpersonatedUser object
//-------------------------------------------------------------------
// Copyright (c) 1997, Thuan L. Thai
//-------------------------------------------------------------------
#ifndef __CIMPERSONATEDUSER_H__
#define __CIMPERSONATEDUSER_H__

#include <lmcons.h> // DNLLEN, UNLEN
#include "evmsgocr.h"

void S_GetExeName(TCHAR *pszExeName);

class CImpersonatedUser {
public:
   CImpersonatedUser() ;
   ~CImpersonatedUser() ;

   void ObtainAccessToken(IServerSecurity *pSecure);

   PSID ObtainUserSID();
   const string & ObtainUserName() ;
   const string & ObtainDomainName() ;

   deque<PSID> * ObtainUserGroupSIDs() ;
   deque<string> * ObtainUserGroupNames() ;

   string UserGroupBySID(PSID pSid) ;

private:
   bool RetrieveDomainAndUserName() ;

private:
   // no copy and assignment allowed
   CImpersonatedUser(const CImpersonatedUser & rhs) ;
   CImpersonatedUser & operator=(const CImpersonatedUser & rhs) ;

private:
   HANDLE m_hAccessToken ;
   string m_strUserName ;
   string m_strDomainName ;
   deque<PSID> m_dequeGroupPSID ;
   unsigned char *m_pByteGroupsUserBelongsTo ;
   deque<string> m_dequeUserGroups ;
   unsigned char *m_pUserTokenInfo ;
} ;

//-------------------------------------------------------------------
// CEventLog object
//-------------------------------------------------------------------
// Copyright (c) 1997, Thuan L. Thai
//-------------------------------------------------------------------
class CEventLog {
public:
   CEventLog() : m_hEventSource(0)
   {
      TCHAR szExeName[MAX_PATH];
      S_GetExeName(szExeName);
      m_hEventSource = RegisterEventSource(NULL, szExeName);
   }

   ~CEventLog() 
   { 
      DeregisterEventSource(m_hEventSource); 
   }
   void LogError(TCHAR *pMessage, PSID pSid)
   {
      if (m_hEventSource != NULL) {
         // Write to event log
         ReportEvent(
            m_hEventSource, 
            EVENTLOG_ERROR_TYPE,
            0, 
            OCR_SECURITY_VIOLATION, 
            pSid, 
            1, 
            0, 
            (const TCHAR **) &pMessage, 
            NULL);
      }
   }

   static bool UpdateRegistry(bool bUpdate)
   {
      // Get the full path name name of the module
      TCHAR szModuleFileName[MAX_PATH] ;
      ::GetModuleFileName(_Module.GetModuleInstance(), 
                          szModuleFileName, MAX_PATH);
      // Get the name of the binary executable from the full name
      TCHAR szExeName[MAX_PATH];
      _tsplitpath(szModuleFileName, NULL, NULL, szExeName, NULL) ;

      // Add registery information to support event logging for this server.
      CRegKey EventLogKey ;
      TCHAR ServerEventKey[MAX_PATH] ;
      lstrcpy(ServerEventKey, TEXT("SYSTEM\\CurrentControlSet\\")
              TEXT("Services\\EventLog\\Application\\"));
      lstrcat(ServerEventKey, szExeName) ;

      long lRc = 0;
      if (bUpdate) {
         // Creating the key for our server.
         lRc = EventLogKey.Create(HKEY_LOCAL_MACHINE, ServerEventKey) ;
         if (lRc != ERROR_SUCCESS) return false ;

         // Adding the EventMessageFile to let the OS know where our messages are.
         lRc = EventLogKey.SetValue(szModuleFileName, TEXT("EventMessageFile")) ;
         if (lRc!=ERROR_SUCCESS) return false;

         // Set the types of events supported
         lRc = EventLogKey.SetValue( EVENTLOG_ERROR_TYPE | 
                     EVENTLOG_WARNING_TYPE |
                     EVENTLOG_INFORMATION_TYPE |
                     EVENTLOG_AUDIT_SUCCESS |
                     EVENTLOG_AUDIT_FAILURE, 
                     TEXT("TypesSupported") ) ;
         if (lRc!=ERROR_SUCCESS) return false ;
      } else {
         lRc=RegDeleteKey(HKEY_LOCAL_MACHINE, ServerEventKey);
         if (lRc!=ERROR_SUCCESS) return false ;
      }
      return true ;
   }

private:
   HANDLE   m_hEventSource;
};

#endif // __CIMPERSONATEDUSER_H__
