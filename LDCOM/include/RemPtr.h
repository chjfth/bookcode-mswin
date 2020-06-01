//------------------------------------------------------------------------
//  Remote Interface Smart Pointer Template Class
//------------------------------------------------------------------------
//  Copyright (c) 1997 Thuan L. Thai
//------------------------------------------------------------------------
#ifndef __REMOTE_INTERFACE_PTR_H__
#define __REMOTE_INTERFACE_PTR_H__

#ifndef _INC_COMDEF
   #include <comdef.h>
#endif _INC_COMDEF

template <class TInterface, const CLSID *pClassID, const IID* pInterfaceID>
class RemotePtr {
public:
   RemotePtr() 
      : m_bConnected(false), m_pInterface(NULL) { }

   RemotePtr(char *pServerName)
      : m_bConnected(false), m_pInterface(NULL) { Connect(pServerName); }

   RemotePtr(wchar_t *pServerName)
      : m_bConnected(false), m_pInterface(NULL) { Connect(pServerName); }

   ~RemotePtr() { Disconnect(); }

   void Connect(char *pServerName)
   {
      if (m_bConnected) { return; }

      _bstr_t bstrServerName;
      COSERVERINFO csi = { 0, NULL, NULL, 0 }; 
      if (strlen(pServerName)>0) {
         bstrServerName = pServerName; csi.pwszName = bstrServerName;
      }     
      Connect(csi);
   }

   void Connect(wchar_t *pServerName) 
   {
      if (m_bConnected) { return; }

      _bstr_t bstrServerName;
      COSERVERINFO csi = { 0, NULL, NULL, 0 }; 
      if (wcslen(pServerName)>0) {
         bstrServerName = pServerName; csi.pwszName = bstrServerName;
      }
      Connect(csi);
   }

   void Connect(const COSERVERINFO & csi) 
   {
      MULTI_QI mqi[] = { {pInterfaceID, NULL, S_OK} };
      HRESULT hr = CoCreateInstanceEx(*pClassID, NULL, 
          CLSCTX_SERVER, const_cast<COSERVERINFO*>(&csi), 
          sizeof(mqi)/sizeof(mqi[0]), mqi);
      if (FAILED(hr)) _com_issue_error(hr);
      if (FAILED(mqi[0].hr)) {
         m_pInterface = NULL;
         m_bConnected = false ;
         _com_issue_error(mqi[0].hr);
      }
      m_pInterface = reinterpret_cast<TInterface*>(mqi[0].pItf);
      m_bConnected = true ;
   }

   void Disconnect()
   {
      if (m_pInterface) {
         m_pInterface->Release() ;
         m_pInterface = NULL ;
         m_bConnected = false ;
      }
   }

   TInterface *operator->() 
   {
      if (!m_pInterface) { _com_issue_error(E_POINTER); }
      return m_pInterface; 
   }

private:
	// no copy and assignment allowed
	RemotePtr(const RemotePtr &rhs);
	RemotePtr &operator=(const RemotePtr &rhs);

private:
	bool		m_bConnected ;
	TInterface *m_pInterface ;
} ;

#endif // __REMOTE_INTERFACE_PTR_H__

