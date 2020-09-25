//*******************************************************************
//*******************************************************************
//***  REUSE in COM - Aggregation Demo
//***  This COM server will make use of the inner object via
//***  Aggregation!!!
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
#include "..\idl\aggregate.h"   // the interface we're implementing

//-------------------------------------------------------------------
// global event telling use wen to shutdown our application
//-------------------------------------------------------------------
HANDLE g_hExitEvent;

//-------------------------------------------------------------------
// Component Level Reference count for the lifetime management of 
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

   // HKEY_CLASSES_ROOT\CLSID\{A0F3554A-C5C1-11d1-9150-006008052F2D}
   //  @="Thesaurus"
   wcscpy(wszKey, TEXT("CLSID\\{A0F3554A-C5C1-11d1-9150-006008052F2D}"));
   RegCreateKey(HKEY_CLASSES_ROOT, wszKey, &hKey);
   wcscpy(wszValue, TEXT("Thesaurus"));
   RegSetValueEx(hKey, 0, 0, REG_SZ, (BYTE*)wszValue, ByteLen(wszValue));

   //  "AppID"="{63B53C11-C46B-11d1-83B4-006008CDD9AE}"
   wcscpy(wszValue, TEXT("{63B53C11-C46B-11d1-83B4-006008CDD9AE}"));
   RegSetValueEx(hKey, TEXT("AppID"), 0, REG_SZ, 
                 (BYTE*)wszValue, ByteLen(wszValue));
   RegCloseKey(hKey);

   // HKEY_CLASSES_ROOT\CLSID\{A0F3554A-C5C1-11d1-9150-006008052F2D}\LocalServer32
   //      @="...path...\aggregate.exe"
   wcscpy(wszKey, TEXT("CLSID\\{A0F3554A-C5C1-11d1-9150-006008052F2D}\\")
          TEXT("LocalServer32"));
   RegCreateKey(HKEY_CLASSES_ROOT, wszKey, &hKey);
   GetModuleFileName(0, wszValue, MAX_PATH);
   RegSetValueEx(hKey, 0, 0, REG_SZ, (BYTE*)wszValue, ByteLen(wszValue));
   RegCloseKey(hKey);

   // HKEY_CLASSES_ROOT\AppID\aggregate.exe
   //      "AppID"="{63B53C11-C46B-11d1-83B4-006008CDD9AE}"
   wcscpy(wszKey, TEXT("AppID\\aggregate.exe"));
   RegCreateKey(HKEY_CLASSES_ROOT, wszKey, &hKey);
   wcscpy(wszValue, TEXT("{63B53C11-C46B-11d1-83B4-006008CDD9AE}"));
   RegSetValueEx(hKey, TEXT("AppID"), 0, REG_SZ, 
                 (BYTE*)wszValue, ByteLen(wszValue));
   RegCloseKey(hKey);

   // HKEY_CLASSES_ROOT\AppID\{63B53C11-C46B-11d1-83B4-006008CDD9AE}
   //      @="Thesaurus"
   wcscpy(wszKey, TEXT("AppID\\{63B53C11-C46B-11d1-83B4-006008CDD9AE}"));
   RegCreateKey(HKEY_CLASSES_ROOT, wszKey, &hKey);
   wcscpy(wszValue, TEXT("Thesaurus"));
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
            TEXT("{A0F3554A-C5C1-11d1-9150-006008052F2D}\\") 
            TEXT("LocalServer32"));
   DisplayStatus(TEXT("Unregistered LocalServer32"), lRc);
   lRc = RegDeleteKey(HKEY_CLASSES_ROOT, TEXT("CLSID\\") 
            TEXT("{A0F3554A-C5C1-11d1-9150-006008052F2D}"));
   DisplayStatus(TEXT("Unregistered CLSID"), lRc);
   lRc = RegDeleteKey(HKEY_CLASSES_ROOT, TEXT("AppID\\") 
            TEXT("{63B53C11-C46B-11d1-83B4-006008CDD9AE}"));
   DisplayStatus(TEXT("Unregistered AppID"), lRc);
   lRc = RegDeleteKey(HKEY_CLASSES_ROOT, TEXT("AppID\\aggregate.exe"));
   DisplayStatus(TEXT("Unregistered aggregate.exe"), lRc);
}
 
//*******************************************************************
//*******************************************************************
//***  AGGREGATION...
//***  Class CoThesaurus - A component object that implements just
//***  one interface:  IThesaurus.
//***  We are using aggregation to reuse the functionality of ISpell
//***  and IOcr that's been implemented by the inner object.
//***  If we happen to detect request for IOcr or ISpell, we 
//***  will forward these calls to the inner object, thus reusing
//***  already implemented functionality.
//*******************************************************************
//*******************************************************************
class CoThesaurus : public IThesaurus {
public:
   // constructors/destructors
   // Code to support aggregation
   CoThesaurus() : m_lRefCount(0), m_pUnkAggregate(0)
   { 
      ComponentAddRef(); 
      // creating the inner, reused object
      CreateInnerObject();
   }

   // Code to support aggregation
   ~CoThesaurus() 
   { 
       if (m_pUnkAggregate) { m_pUnkAggregate->Release(); }
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

   // IThesaurus
   STDMETHODIMP LookUp() 
   { wprintf(TEXT("Lookup success...\n")); return S_OK; }

private:
   // function to create the inner object for reuse
   void CreateInnerObject();

private:
   LONG m_lRefCount;
   // support aggregation...
   // the unknown of the object we're aggregating
   // must be IUnknown for aggregation...
   IUnknown *m_pUnkAggregate;  
};

//-------------------------------------------------------------------
// Creating the inner object for reuse...
// CoMyEngine::CreateInnerObject
//-------------------------------------------------------------------
void CoThesaurus::CreateInnerObject()
{
   // support aggregation...
   // The aggregation law of COM require us to create
   // the inner object, and requesting only for IUnknown
   // and nothing else!!!
   MULTI_QI mqi[] = { {&IID_IUnknown, NULL, S_OK} };

   // Notice "this" as the second argument... 
   // reuse by aggregation!!!
   // Outer unknown is this object--uses the inner object.
   // This argument is the magic that get's our 
   // object (CLSID_Thesaurus) to reuse the 
   // inner object (CLSID_OcrEngine)

   // Special support here in case the inner object
   // code happened to cause a delete of this object.
   // Remember at this moment, the reference count
   // that we are keeping for this object is 0, since
   // we're still in the constructor.
   // Here's a potential probelm:  The inner object
   // QIs for one of our (this object's) interfaces
   // (refcount=1) and releases it (refcount=0),
   // causing a premature delete of this object.
   // To prevent this, we temporarily increment and decrement our
   // reference count.  However, don't call Release, since
   // it deletes our object.  You'd have to use this technique:
   InterlockedIncrement(&m_lRefCount);
   HRESULT hr = CoCreateInstanceEx(CLSID_OcrEngine, 
        this, 
        CLSCTX_SERVER, 
        NULL,
        sizeof(mqi)/sizeof(mqi[0]), 
        mqi);
   InterlockedDecrement(&m_lRefCount);

   if (SUCCEEDED(hr) && SUCCEEDED(mqi[0].hr)) {
      m_pUnkAggregate = reinterpret_cast<IUnknown *>(mqi[0].pItf);
   } else {
      DisplayStatus(TEXT("Failed to create inner object..."), S_OK);
      wprintf(TEXT("You need to use the Aggregatable")
                TEXT("Version of CLSID_OcrEngine\n")
                TEXT("It lives in code\\basics\\reuse\\inner\n")
                TEXT("Version of CLSID_OcrEngine\n"));
      assert(false);
   }
}
 

//-------------------------------------------------------------------
// CoThesaurus::CreateObject - static function to create an
// CoThesaurus object
//-------------------------------------------------------------------
HRESULT CoThesaurus::CreateObject(LPUNKNOWN pUnkOuter, 
                                  REFIID riid, 
                                  void** ppv)
{
   *ppv = NULL;

   // CoThesaurus doesn't support aggregation
   if (pUnkOuter != NULL) { return CLASS_E_NOAGGREGATION; }

   CoThesaurus * pThesaurus = new CoThesaurus;
   if (pThesaurus == NULL) { return E_OUTOFMEMORY; }

   HRESULT hr = pThesaurus->QueryInterface(riid, ppv);
   if (FAILED(hr)) { delete pThesaurus; }

   return hr;
}

//-------------------------------------------------------------------
// CoThesaurus::QueryInterface
//-------------------------------------------------------------------
STDMETHODIMP
CoThesaurus::QueryInterface(REFIID riid, void** ppv)
{
   if (ppv==NULL) { return E_INVALIDARG; }

   if (riid==IID_IUnknown) {
      *ppv= static_cast<IThesaurus *>(this);
   } else if (riid==IID_IThesaurus) {
      *ppv= static_cast<IThesaurus *>(this);
   }  else if (riid==IID_IOcr||riid==IID_ISpell) {
      // Support Aggregation...
      // interfaces belong to the reused object
      // let the inner object handle this request!
      return m_pUnkAggregate->QueryInterface(riid, ppv) ;
   } else {
      *ppv=NULL; return E_NOINTERFACE ;
   }

   reinterpret_cast<IUnknown *>(*ppv)->AddRef();

   return S_OK;
}

//*******************************************************************
//*******************************************************************
//***  Class Factory: manufacturing CoThesaurus objects
//*******************************************************************
//*******************************************************************
class CoThesaurusFactory : public IClassFactory 
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
     // call CoThesaurus's static function to create
     // a CoThesaurus component object
     return CoThesaurus::CreateObject(pUnkOuter, riid, ppv);
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
CoThesaurusFactory g_ThesaurusClassFactory;

//-------------------------------------------------------------------
//  CoThesaurusFactory::QueryInterface
//-------------------------------------------------------------------
STDMETHODIMP 
CoThesaurusFactory::QueryInterface(REFIID riid, void** ppv)
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

   // Register Thesaurus Class Factory with COM
   DWORD dwRegister;
   hr = CoRegisterClassObject(CLSID_Thesaurus, 
                              &g_ThesaurusClassFactory,
                              CLSCTX_SERVER, 
                              REGCLS_MULTIPLEUSE, 
                              &dwRegister);
   assert(SUCCEEDED(hr));
   // we don't have to do this, but it's a good habit
   g_ThesaurusClassFactory.Release();  

   // sit and wait until CoThesaurus object is used
   WaitForSingleObject(g_hExitEvent, INFINITE);

   // revoke the factory from public view
   CoRevokeClassObject(dwRegister);

   // uninitialize
   CoUninitialize();

   DisplayStatus(TEXT("Server shutting down in 5 seconds..."), S_OK);

   Sleep(5000);
}
