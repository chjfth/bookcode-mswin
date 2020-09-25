//*******************************************************************
//*******************************************************************
//***  Implementation of the CoOcrEngine component object
//***  that supports aggregation
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
#include "..\..\idl\ocr.h"

//-------------------------------------------------------------------
// global event telling use wen to shutdown our application
//-------------------------------------------------------------------
HANDLE g_hExitEvent;
wchar_t g_wszModuleName[MAX_PATH];

// Number of living objects
LONG g_lComponentRefCounts = 0; // counts living objects and locks

//-------------------------------------------------------------------
// Component Level Reference count for the lifetime management of 
// the whole component.
// These specific implementations are used for in-process servers
// There are a pair of functions like these that are used for
// out-of-process servers.
// Return values for debugging purpose only 
//-------------------------------------------------------------------
inline ULONG ComponentAddRef()
{
   return (InterlockedIncrement(&g_lComponentRefCounts));
}

inline ULONG ComponentRelease()  
{
   return (InterlockedDecrement(&g_lComponentRefCounts));
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
// Automatic registration; for in-process server, do:
//    regsvr32.exe xxx.dll
//-------------------------------------------------------------------
void RegisterServerToSystem()
{
   wchar_t wszKey[MAX_PATH];
   wchar_t wszValue[MAX_PATH];
   HKEY hKey = 0;

   // CLSID_OcrEngine = {DF22A6B2-A58A-11d1-ABCC-00207810D5FE}
   // HKEY_CLASSES_ROOT\CLSID\{DF22A6B2-A58A-11d1-ABCC-00207810D5FE}
   //      @="OcrEngine"
   wcscpy(wszKey, TEXT("CLSID\\{DF22A6B2-A58A-11d1-ABCC-00207810D5FE}"));
   RegCreateKey(HKEY_CLASSES_ROOT, wszKey, &hKey);
   wcscpy(wszValue, TEXT("OcrEngine"));
   RegSetValueEx(hKey, 0, 0, REG_SZ, (BYTE*)wszValue, ByteLen(wszValue));
   RegCloseKey(hKey);

   // HKEY_CLASSES_ROOT\CLSID\{DF22A6B2-A58A-11d1-ABCC-00207810D5FE}\InprocServer32
   //      @="E:\DCOM\CODE\Essentials\inproc\DEBUG\inproc.dll"
   //      "ThreadingModel" = "Both"
   wcscpy(wszKey, TEXT("CLSID\\{DF22A6B2-A58A-11d1-ABCC-00207810D5FE}")
        TEXT("\\InprocServer32"));
   RegCreateKey(HKEY_CLASSES_ROOT, wszKey, &hKey);
   wcscpy(wszValue, g_wszModuleName);
   RegSetValueEx(hKey, 0, 0, REG_SZ, (BYTE*)wszValue, ByteLen(wszValue));
   wcscpy(wszValue, TEXT("Both"));
   RegSetValueEx(hKey, TEXT("ThreadingModel"), 0, REG_SZ, 
                 (BYTE*)wszValue, ByteLen(wszValue));
   RegCloseKey(hKey);
}

//-------------------------------------------------------------------
// Automatic registration; for in-process server, do:
//    regsvr32.exe -u xxx.dll
//-------------------------------------------------------------------
void UnregisterServerFromSystem()
{
   long lRc = 0 ;

   lRc=RegDeleteKey(HKEY_CLASSES_ROOT, 
       TEXT("CLSID\\{DF22A6B2-A58A-11d1-ABCC-00207810D5FE}") 
     TEXT("\\InprocServer32"));
   DisplayStatus(TEXT("Unregistered CLSID's InprocServer"), lRc);

   lRc=RegDeleteKey(HKEY_CLASSES_ROOT, 
       TEXT("CLSID\\{DF22A6B2-A58A-11d1-ABCC-00207810D5FE}"));
   DisplayStatus(TEXT("Unregistered CLSID"), lRc);
}
 

//*******************************************************************
//*******************************************************************
//***  Implitcit unknown
//*******************************************************************
//*******************************************************************
template<class T> class CImplicitUnknown : public IUnknown {
public:
    // m_pbkptr points to the object implementing the
   // non-delegating IUnknown methods.
   void Bkptr(T *p) { m_pbkptr=p; }

    STDMETHODIMP QueryInterface(REFIID iid, void **ppv)
    { return m_pbkptr->InternalQueryInterface(iid, ppv); }
    STDMETHODIMP_(ULONG) AddRef(void)
    { return m_pbkptr->InternalAddRef(); }
    STDMETHODIMP_(ULONG) Release(void)
    { return m_pbkptr->InternalAddRef(); }
    
private:
    T *m_pbkptr;
};

//*******************************************************************
//*******************************************************************
//***  Class CoOcrEngine - A component object that implements two
//***  interfaces:  IOcr and ISpell
//***  This implementation that supports AGGREGATION.
//***  Containment works with every component object.
//***  Aggregation doesn't work across aparments, including 
//***  processes and machines.  This means that the object
//***  that wants to aggregate this object must run in the same
//***  apartment as this object.  This implies that no marshaling
//***  will be involved, and both the aggregator and the aggregatee
//***  (this object) will run in the same appartment, since we're
//***  in the same process, this also implies the same thread.
//*******************************************************************
//*******************************************************************
class CoOcrEngine : public IOcr, public ISpell {
public:
   //----------------------------------
   // Modified for aggregation support
   //----------------------------------
   CoOcrEngine(IUnknown *pOuterUnk) 
        : m_lRefCount(0), m_pOuterUnk(pOuterUnk), m_InnerUnk()
   { 
      ComponentAddRef(); 

      m_InnerUnk.Bkptr(this);
      if (pOuterUnk==NULL) { 
         // we're not aggregating; therefore, 
         // we've got no outer object
         // the outer object is ourselves
         m_pOuterUnk = &m_InnerUnk;
         DisplayStatus(TEXT("AGGRVersion: not being aggregated"), S_OK);
      } else {
         DisplayStatus(TEXT("AGGRVersion: being aggregated"), S_OK);
      }
   }

   ~CoOcrEngine()
   { ComponentRelease(); }

   //----------------------------------
   // Modified for aggregation support
   //----------------------------------
   static HRESULT CreateObject(LPUNKNOWN pOuterUnk, 
                             REFIID riid, void **ppv);

public: 
   //----------------------------------
   // Modified for aggregation support
   //----------------------------------
   // IUnknown Methods - possibly delegating to an out aggregator
   // these a method for this object's IUnknown's vtbl,
   // but the functionality depends on the m_pOuterUnk pointer.
   STDMETHODIMP QueryInterface(REFIID iid, void **ppv) 
   { return m_pOuterUnk->QueryInterface(iid, ppv); }
   STDMETHODIMP_(ULONG) AddRef(void)
   { return m_pOuterUnk->AddRef(); }
   STDMETHODIMP_(ULONG) Release(void)
   { return m_pOuterUnk->Release(); }

   // IOcr Methods
   STDMETHODIMP OcrImage(long lImageSize, 
                         byte *pbImage,                          
                         wchar_t **pwszOcrText);
   STDMETHODIMP OcrZone(long lImageSize, 
                        byte *pbImage,
                        Zone zone,
                        wchar_t **pwszOcrText);

   // ISpell Methods
   STDMETHODIMP Check(wchar_t *pwszWord,
                      PossibleWords *pWords);

private:
   //----------------------------------
   // Aggregation support
   //----------------------------------
   // non-delegating/inner/implicit unknown 
   // these are non-virtual private functions
   HRESULT InternalQueryInterface(REFIID iid, void **ppv);
   ULONG InternalAddRef(void)
   {
      wchar_t wszMsg[MAX_PATH];
      wsprintf(wszMsg, TEXT("CoOcrEngine::InternalAddRef (%ld)"), m_lRefCount); 
      return InterlockedIncrement(&m_lRefCount);
   }

   ULONG InternalRelease(void)
   {
      wchar_t wszMsg[MAX_PATH];
      wsprintf(wszMsg, TEXT("CoOcrEngine::InternalRelease (%ld)"), m_lRefCount-1); 
      long lCount = InterlockedDecrement(&m_lRefCount);
      if (lCount == 0) { delete this; } 
      return lCount; 
   }

private:
   LONG m_lRefCount;

   //----------------------------------
   // Aggregation support
   //----------------------------------
   IUnknown *m_pOuterUnk;
   friend CImplicitUnknown<CoOcrEngine>;
   CImplicitUnknown<CoOcrEngine> m_InnerUnk;
};

//-------------------------------------------------------------------
// CoOcrEngine::CreateObject - static function to create an
// CoOcrEngine object
//-------------------------------------------------------------------
HRESULT CoOcrEngine::CreateObject(LPUNKNOWN pOuterUnk, 
                                  REFIID riid, 
                                  void** ppv)
{
   // debug message to check this thread id 
   // and the client's thread id, uncomment to see the effect
   // Aggregation work only when the thread ids are equal
   // since we're only dealing in process.
   // wchar_t c[MAX_PATH];
   // wsprintf(c, TEXT("%u"), GetCurrentThreadId());
   // ::MessageBox(NULL, c, c, MB_OK);

   *ppv = NULL;

   // if aggregating, must initially request for IID_IUnknown
   if (pOuterUnk != NULL && riid != IID_IUnknown)
      return CLASS_E_NOAGGREGATION;

   // create the new object with a possibly valid outer unknown
   CoOcrEngine * pEngine = new CoOcrEngine(pOuterUnk);
   if (pEngine == NULL) { return E_OUTOFMEMORY; }

   //-----------------------------------------------
   // Modification to support aggregation
   //-----------------------------------------------
   // The first query must be to the implicit 
   // IUnknown (the inner IUnknown)
   HRESULT hr = pEngine->InternalQueryInterface(riid, ppv);

   if (FAILED(hr)) {  delete pEngine;  }

   return hr;
}

//-------------------------------------------------------------------
// CoOcrEngine::InternalQueryInterface - supporting aggregation
//-------------------------------------------------------------------
HRESULT
CoOcrEngine::InternalQueryInterface(REFIID riid, void** ppv)
{
   if (ppv==NULL) { return E_INVALIDARG; }

   if (riid==IID_IUnknown) {
      //----------------------------------------
      // Always return the implicit unknown
      //----------------------------------------
      *ppv= static_cast<IUnknown *>(&m_InnerUnk);
   } else if (riid==IID_IOcr) {
      *ppv= static_cast<IOcr *>(this);
   } else if (riid==IID_ISpell) {
      *ppv= static_cast<ISpell *>(this);
   } else {
      *ppv=NULL; return E_NOINTERFACE ;
   }

   reinterpret_cast<IUnknown *>(*ppv)->AddRef();

   return S_OK;
}

//-------------------------------------------------------------------
// CoOcrEngine::OcrImage
//-------------------------------------------------------------------
STDMETHODIMP 
CoOcrEngine::OcrImage(long lImageSize, 
                     byte *pbImage,                      
                     wchar_t **pwszOcrText)
{    
   // DisplayStatus(TEXT("CoOcrEngine::OcrImage"), S_OK);

   wchar_t wszPath[MAX_PATH];
   GetTempPath(MAX_PATH, wszPath);
   wchar_t wszFileName[MAX_PATH];
   GetTempFileName(wszPath, TEXT("TIF"), 0, wszFileName);
   DisplayStatus(wszFileName, S_OK);

   // write the received image to a file
   HANDLE hFile = CreateFile(wszFileName, 
       GENERIC_WRITE,
       FILE_SHARE_READ, 
       NULL, 
       CREATE_ALWAYS, 
       FILE_ATTRIBUTE_NORMAL, NULL);

   if (hFile==INVALID_HANDLE_VALUE) {
       Beep(1000,1000);
       wprintf(TEXT("hFile invalid\n"));
   } else {
       DWORD dwWritten = 0 ;
       WriteFile(hFile, 
           static_cast<LPCVOID>(pbImage), 
           lImageSize,
           &dwWritten, NULL);
       CloseHandle(hFile);
   }
   
   // allocate memory and return fake ocr data
   wchar_t wszOcrText[MAX_PATH];
   wcscpy(wszOcrText, TEXT("This is fake OCR text"));

   // remember that we have to take the len of string including null
   // multiply by the sizeof wchar_t
   long lAllocSize = (wcslen(wszOcrText)+1) * sizeof(wchar_t);
   wprintf(TEXT("lAllocSize = %u\n"), lAllocSize);

   *pwszOcrText = static_cast<wchar_t *>(CoTaskMemAlloc(lAllocSize));
   if (*pwszOcrText==NULL) { return E_OUTOFMEMORY; }

   wcscpy(*pwszOcrText, wszOcrText);

   wprintf(TEXT("Returning = %s\n"), *pwszOcrText);

   return S_OK ;
}

//-------------------------------------------------------------------
// CoOcrEngine::OcrZone
//-------------------------------------------------------------------
STDMETHODIMP 
CoOcrEngine::OcrZone(long lImageSize, byte *pbImage, 
                    Zone zone, wchar_t **pwszOcrText)
{
   return E_NOTIMPL ;
}

//-------------------------------------------------------------------
// CoOcrEngine::Check
//-------------------------------------------------------------------
STDMETHODIMP
CoOcrEngine::Check(wchar_t *pszWord, PossibleWords *pWords)
{
   wcscpy(reinterpret_cast<wchar_t *>(pWords->wszOne), TEXT("ChoiceOne"));
   wcscpy(reinterpret_cast<wchar_t *>(pWords->wszTwo), TEXT("ChoiceTwo"));
   return S_OK ;
}

//*******************************************************************
//*******************************************************************
//***  Class Factory: manufacturing CoOcrEngine objects
//***  No changes to factory to support Aggregation, since the static
//***  CreateObject function hides the changes
//*******************************************************************
//*******************************************************************
class CoOcrEngineFactory : public IClassFactory 
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
     // call CoOcrEngine's static function to create
     // a CoOcrEngine component object
     return CoOcrEngine::CreateObject(pUnkOuter, riid, ppv);
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
CoOcrEngineFactory g_OcrEngineClassFactory;

//-------------------------------------------------------------------
//  CoOcrEngineFactory::QueryInterface
//-------------------------------------------------------------------
STDMETHODIMP 
CoOcrEngineFactory::QueryInterface(REFIID riid, void** ppv)
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
//***  DLL Exports
//*******************************************************************
//*******************************************************************
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, 
                               DWORD dwReason, 
                               LPVOID /*lpReserved*/)
{
   if (dwReason == DLL_PROCESS_ATTACH) {
      // This message here to remind us that we are using
      // the aggregatable implementation of the CLSID_OCREngine
      // Either use this or the non-aggregatable version
      Beep(100,100);
      ::MessageBox(NULL, 
          TEXT("This is the Aggregatable Version of CLSID_OCREngine\n")
          TEXT("The Non-Aggregatable Version of CLSID_OCREngine\n")
          TEXT("lives in [basics\\inproc]\n"),
          TEXT("Aggregatable Version"), MB_OK);
      GetModuleFileName(hInstance, g_wszModuleName, MAX_PATH);
   } else if (dwReason == DLL_PROCESS_DETACH) {}

   return TRUE;    // ok
}

//*******************************************************************
//*******************************************************************
//** Used to determine whether the DLL can be unloaded by DCOM
//*******************************************************************
//*******************************************************************
STDAPI DllCanUnloadNow(void)
{ 
   if (g_lComponentRefCounts ==0) {
      return S_OK;
   } else {
      return S_FALSE;
   }
}

//*******************************************************************
//*******************************************************************
//** Returns a class factory to create an object of the requested type
//*******************************************************************
//*******************************************************************
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
   *ppv = NULL;

   // Does our component support this particular class ID?
   if (CLSID_OcrEngine!=rclsid) { return E_FAIL; }

   CoOcrEngineFactory * pCF = new CoOcrEngineFactory();
   if (pCF==NULL) { return E_OUTOFMEMORY; }

   HRESULT hr = pCF->QueryInterface(riid, ppv);
   if (FAILED(hr)) {
      delete pCF;
   } else {
      *ppv=pCF;
   }

   return hr;
}

//*******************************************************************
//*******************************************************************
//**  DllRegisterServer - Adds entries to the system registry
//*******************************************************************
//*******************************************************************
STDAPI DllRegisterServer(void)
{
   RegisterServerToSystem();
   return S_OK;
}

//*******************************************************************
//*******************************************************************
//** DllUnregisterServer - Removes entries from the system registry
//*******************************************************************
//*******************************************************************
STDAPI DllUnregisterServer(void)
{
   UnregisterServerFromSystem();
   return S_OK;
}

