//*******************************************************************
//*******************************************************************
//***  REUSE through Containment....
//*******************************************************************
//*******************************************************************

//-------------------------------------------------------------------
// Required macros
//-------------------------------------------------------------------
#define UNICODE         // UNICODE
#define _WIN32_DCOM     // DCOM

//-------------------------------------------------------------------
// includes
//-------------------------------------------------------------------
#include <assert.h>
#include <stdio.h>
#include <windows.h>
#include <initguid.h>
#include "..\..\..\idl\ocr.h"   // the reused intefaces
#include "..\idl\contain.h"     // the interface we're implementing

//-------------------------------------------------------------------
// global event telling use wen to shutdown our application
//-------------------------------------------------------------------
HANDLE g_hExitEvent;

//-------------------------------------------------------------------
// Component Level Reference count for the lifetime managment of 
// the whole component.
// These specific implementations are used for out-of-process servers
// There's a pair of functions like these that are used for
// in-process servers.
//-------------------------------------------------------------------
inline ULONG ComponentAddRef()
{
    ULONG ul = CoAddRefServerProcess();
    wprintf(TEXT("ComponentAddRef(%ld)\n"), ul);
    return ul ;
}

inline ULONG ComponentRelease()
{
   ULONG ul = CoReleaseServerProcess();

   // wait 3 second to test for REGCLS_MULTIPLEUSE
   // Sleep(3000);

   wprintf(TEXT("ComponentRelease(%ld)\n"), ul);
   if (ul==0) {
      SetEvent(g_hExitEvent);
   }
   return ul ;
}


//*******************************************************************
//*******************************************************************
//***  UTILITY FUNCTIONS
//*******************************************************************
//*******************************************************************
void DisplayStatus(wchar_t *pwszMsg, HRESULT hr)
{

   if (hr == S_OK) {
      wprintf(TEXT("%s\n"), pwszMsg);
      return;
   }

   if (HRESULT_FACILITY(hr) == FACILITY_WINDOWS) {
      hr = HRESULT_CODE(hr);
   }

   wchar_t *pwszStatus;
   FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
       FORMAT_MESSAGE_FROM_SYSTEM,
       NULL,
       hr,
       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
       (LPWSTR)&pwszStatus,
       0,
       NULL );

   wprintf(TEXT("%s: %s (ECode: %lx)\n"), pwszMsg, pwszStatus, hr);

   LocalFree(pwszStatus);
}

//-------------------------------------------------------------------
// Given a unicode string, get the number of bytes needed to host it
//-------------------------------------------------------------------
inline long ByteLen(wchar_t *pwsz) 
{
   return (sizeof(wchar_t)*(wcslen(pwsz)+1));
}

//-------------------------------------------------------------------
// Automatic registration; for out-of-process server support the
// -regserver option
//-------------------------------------------------------------------
void RegisterComponent()
{
   wchar_t wszKey[MAX_PATH];
   wchar_t wszValue[MAX_PATH];
   HKEY hKey = 0;

   // HKEY_CLASSES_ROOT\CLSID\{80DA2D21-C466-11d1-83B4-006008CDD9AE}
   //  @="Dictionary"
   wcscpy(wszKey, TEXT("CLSID\\{80DA2D21-C466-11d1-83B4-006008CDD9AE}"));
   RegCreateKey(HKEY_CLASSES_ROOT, wszKey, &hKey);
   wcscpy(wszValue, TEXT("Dictionary"));
   RegSetValueEx(hKey, 0, 0, REG_SZ, (BYTE*)wszValue, ByteLen(wszValue));

   //  "AppID"="{63B53C11-C46B-11d1-83B4-006008CDD9AE}"
   wcscpy(wszValue, TEXT("{63B53C11-C46B-11d1-83B4-006008CDD9AE}"));
   RegSetValueEx(hKey, TEXT("AppID"), 0, REG_SZ, 
                 (BYTE*)wszValue, ByteLen(wszValue));
   RegCloseKey(hKey);

   // HKEY_CLASSES_ROOT\CLSID\{80DA2D21-C466-11d1-83B4-006008CDD9AE}\LocalServer32
   //      @="...path...\contain.exe"
   wcscpy(wszKey, TEXT("CLSID\\{80DA2D21-C466-11d1-83B4-006008CDD9AE}\\")
          TEXT("LocalServer32"));
   RegCreateKey(HKEY_CLASSES_ROOT, wszKey, &hKey);
   GetModuleFileName(0, wszValue, MAX_PATH);
   RegSetValueEx(hKey, 0, 0, REG_SZ, (BYTE*)wszValue, ByteLen(wszValue));
   RegCloseKey(hKey);

   // HKEY_CLASSES_ROOT\AppID\contain.exe
   //      "AppID"="{63B53C11-C46B-11d1-83B4-006008CDD9AE}"
   wcscpy(wszKey, TEXT("AppID\\contain.exe"));
   RegCreateKey(HKEY_CLASSES_ROOT, wszKey, &hKey);
   wcscpy(wszValue, TEXT("{63B53C11-C46B-11d1-83B4-006008CDD9AE}"));
   RegSetValueEx(hKey, TEXT("AppID"), 0, REG_SZ, 
                 (BYTE*)wszValue, ByteLen(wszValue));
   RegCloseKey(hKey);

   // HKEY_CLASSES_ROOT\AppID\{63B53C11-C46B-11d1-83B4-006008CDD9AE}
   //      @="Dictionary"
   wcscpy(wszKey, TEXT("AppID\\{63B53C11-C46B-11d1-83B4-006008CDD9AE}"));
   RegCreateKey(HKEY_CLASSES_ROOT, wszKey, &hKey);
   wcscpy(wszValue, TEXT("Dictionary"));
   RegSetValueEx(hKey, 0, 0, REG_SZ, 
                 (BYTE*)wszValue, ByteLen(wszValue));
   RegCloseKey(hKey);
}

//-------------------------------------------------------------------
// Automatic unregistration; for out-of-process server support the
// -unregserver option
//-------------------------------------------------------------------
void UnregisterComponent()
{   
   long lRc = 0 ;
   lRc = RegDeleteKey(HKEY_CLASSES_ROOT, TEXT("CLSID\\") 
            TEXT("{80DA2D21-C466-11d1-83B4-006008CDD9AE}\\") 
            TEXT("LocalServer32"));
   DisplayStatus(TEXT("Unregistered LocalServer32"), lRc);
   lRc = RegDeleteKey(HKEY_CLASSES_ROOT, TEXT("CLSID\\") 
            TEXT("{80DA2D21-C466-11d1-83B4-006008CDD9AE}"));
   DisplayStatus(TEXT("Unregistered CLSID"), lRc);
   lRc = RegDeleteKey(HKEY_CLASSES_ROOT, TEXT("AppID\\") 
            TEXT("{63B53C11-C46B-11d1-83B4-006008CDD9AE}"));
   DisplayStatus(TEXT("Unregistered AppID"), lRc);
   lRc = RegDeleteKey(HKEY_CLASSES_ROOT, TEXT("AppID\\contain.exe"));
   DisplayStatus(TEXT("Unregistered contain.exe"), lRc);
}
 
//*******************************************************************
//*******************************************************************
//***  CONTAINMENT...
//***  Class CoDictionary - A component object that implements two
//***  interfaces:  IDictionary and ISpell.
//***  We are using containment to reuse the functionality of ISpell
//***  that's been implemented by the inner object.
//***  Therefore the ISpell methods implemented in this object
//***  will forward all calls to the inner object, thus reusing
//***  already implemented functionality.
//*******************************************************************
//*******************************************************************
class CoDictionary : public IDictionary, public ISpell {
public:
   // constructors/destructors
   // Code to support containment
   CoDictionary() : m_lRefCount(0), m_pISpell(0)
   { 
      ComponentAddRef(); 
      // creating the inner object
      CreateInnerObject();
   }

   // Code to support containment
   ~CoDictionary() 
   { 
      // releasing the interface of the inner object
      // that we've been keeping for reuse
      if (m_pISpell) { m_pISpell->Release(); }
      ComponentRelease(); 
   }

   // The factory will call this method to actually 
   // create this object
   static HRESULT CreateObject(LPUNKNOWN pUnkOuter, REFIID riid, 
                               void** ppv);
public: 
   // IUnknown Methods
   STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
   STDMETHODIMP_(ULONG) AddRef(void)
   { return InterlockedIncrement(&m_lRefCount); }
   STDMETHODIMP_(ULONG) Release(void)
   {
      long lCount = InterlockedDecrement(&m_lRefCount);
      if (lCount == 0) { delete this; } 
      return lCount; 
   }

   // IDictionary
   STDMETHODIMP LookUp() 
   { wprintf(TEXT("Lookup success...\n")); return S_OK; }

   // ISpell methods
   // Look... we're reusing the implementation an inner
   // binary object.  Basically forwarding the calls...
   STDMETHODIMP Check(wchar_t *pszWord, PossibleWords *pWords)
   { return m_pISpell->Check(pszWord, pWords); }

private:
   // function to create the inner object for reuse
   void CreateInnerObject();

private:
   LONG m_lRefCount;
   // we specifically want to reuse the ISpell interface of the
   // inner object -- using the containment technique.
   // keeping a reference to the inner, reused object
   ISpell *m_pISpell ;  
};

//-------------------------------------------------------------------
// Creating the inner object for reuse...
// CoMyEngine::CreateInnerObject
//-------------------------------------------------------------------
void CoDictionary::CreateInnerObject()
{
   MULTI_QI mqi[] = { {&IID_ISpell, NULL, S_OK} };

   //------------------------------------------------------------
   // Notice NULL as the second argument... reuse by containment!
   //------------------------------------------------------------

   HRESULT hr = CoCreateInstanceEx(CLSID_OcrEngine, 
        NULL,           // Reuse by CONTAINMENT
        CLSCTX_SERVER, 
        NULL,
        sizeof(mqi)/sizeof(mqi[0]), 
        mqi);
   if (SUCCEEDED(hr) && SUCCEEDED(mqi[0].hr)) {
      m_pISpell = reinterpret_cast<ISpell *>(mqi[0].pItf);
   } else {
      DisplayStatus(TEXT("Failed to create inner object..."), S_OK);
      assert(false);
   }
}
 

//-------------------------------------------------------------------
// CoDictionary::CreateObject - static function to create an
// CoDictionary object
//-------------------------------------------------------------------
HRESULT CoDictionary::CreateObject(LPUNKNOWN pUnkOuter, 
                                   REFIID riid, 
                                   void** ppv)
{
   *ppv = NULL;

   // CoDictionary doesn't support aggregation
   if (pUnkOuter != NULL) { return CLASS_E_NOAGGREGATION; }

   CoDictionary * pDictionary = new CoDictionary;
   if (pDictionary == NULL) { return E_OUTOFMEMORY; }

   HRESULT hr = pDictionary->QueryInterface(riid, ppv);
   if (FAILED(hr)) { delete pDictionary; }

   return hr;
}

//-------------------------------------------------------------------
// CoDictionary::QueryInterface
//-------------------------------------------------------------------
STDMETHODIMP
CoDictionary::QueryInterface(REFIID riid, void** ppv)
{
   if (ppv==NULL) { return E_INVALIDARG; }

   if (riid==IID_IUnknown) {
      *ppv= static_cast<IDictionary *>(this);
   } else if (riid==IID_IDictionary) {
      *ppv= static_cast<IDictionary *>(this);
   }  else if (riid==IID_ISpell) {
      *ppv= static_cast<ISpell *>(this);
   } else {
      *ppv=NULL; return E_NOINTERFACE ;
   }

   reinterpret_cast<IUnknown *>(*ppv)->AddRef();

   return S_OK;
}

//*******************************************************************
//*******************************************************************
//***  Class Factory: manufacturing CoDictionary objects
//*******************************************************************
//*******************************************************************
class CoDictionaryFactory : public IClassFactory 
{
public:
   // IUnknown Methods
   STDMETHODIMP QueryInterface (REFIID riid, void** ppv);
   STDMETHODIMP_(ULONG) AddRef(void)
   { return 1; }
   STDMETHODIMP_(ULONG) Release(void) 
   { return 1; }

   // IClassFactory Methods
   STDMETHODIMP CreateInstance(LPUNKNOWN pUnkOuter, 
                               REFIID riid, 
                               void **ppv)
  {
     // call CoDictionary's static function to create
     // a CoDictionary component object
     return CoDictionary::CreateObject(pUnkOuter, riid, ppv);
  }

   STDMETHODIMP LockServer(BOOL fLock)
   {
      if (fLock) {
         ComponentAddRef();
      } else {
         ComponentRelease();
      }
      return S_OK;        
   }   
};

//-------------------------------------------------------------------
//  Singleton factory instance that manufacture CoCorEngine
//  component objects
//-------------------------------------------------------------------
CoDictionaryFactory g_DictionaryClassFactory;

//-------------------------------------------------------------------
//  CoDictionaryFactory::QueryInterface
//-------------------------------------------------------------------
STDMETHODIMP 
CoDictionaryFactory::QueryInterface(REFIID riid, void** ppv)
{
   if (ppv==NULL) { return E_INVALIDARG; }

   if (riid==IID_IUnknown) {
      *ppv= static_cast<IClassFactory *>(this);
   } else if (riid==IID_IClassFactory) {
      *ppv= static_cast<IClassFactory *>(this);
   } else {
      *ppv=NULL; return E_NOINTERFACE ;
   }

   reinterpret_cast<IUnknown *>(*ppv)->AddRef();

   return S_OK;
}

//*******************************************************************
//*******************************************************************
//***  Main Program
//*******************************************************************
//*******************************************************************
void main(int argc, char **argv)
{
   DisplayStatus(TEXT("Server: Started"), S_OK);

   g_hExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
   assert(g_hExitEvent);

   // registration if required
   if (argc > 1) {
       if (_stricmp(argv[1], "-RegServer")==0){
           RegisterComponent();
           DisplayStatus(TEXT("Registered..."), S_OK);
           return ;
       }
       if (_stricmp(argv[1], "-UnRegServer")==0){
           UnregisterComponent();
           DisplayStatus(TEXT("Unregistered..."), S_OK);
           return ;
       }
   }

   // Init COM
   HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
   assert(SUCCEEDED(hr));

   // Register Dictionary Class Factory with COM
   DWORD dwRegister;
   hr = CoRegisterClassObject(CLSID_Dictionary, 
                              &g_DictionaryClassFactory,
                              CLSCTX_SERVER, 
                              REGCLS_MULTIPLEUSE, 
                              &dwRegister);
   assert(SUCCEEDED(hr));
  // we don't have to do this, but it's a good habit
   g_DictionaryClassFactory.Release();  

   // sit and wait until CoDictionary object is used
   WaitForSingleObject(g_hExitEvent, INFINITE);

  // revoke the factory from public view
   CoRevokeClassObject(dwRegister);

  // uninitialize
   CoUninitialize();

   DisplayStatus(TEXT("Server shutting down in 5 seconds..."), S_OK);

   Sleep(5000);
}
