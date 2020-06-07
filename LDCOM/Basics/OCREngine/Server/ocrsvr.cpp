//*******************************************************************
//*******************************************************************
//***  Implementation of the CoOcrEngine component object in an
//***  outofproc (EXE) component.
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
#include "..\share.h"
#include "..\idl\ocr.h"

//-------------------------------------------------------------------
// global event telling use wen to shutdown our application
//-------------------------------------------------------------------
HANDLE g_hExitEvent;

//-------------------------------------------------------------------
// Component Level Reference count for the lifetime managment of 
// the whole component.
// These specific implementations are used for out-of-process servers
// There are a pair of functions like these that are used for
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
        wprintf(TEXT("%s: OK\n"), pwszMsg);
        return;
    }

    if (HRESULT_FACILITY(hr) == FACILITY_WINDOWS) {
        hr = HRESULT_CODE(hr);
	}

    wchar_t *pwszStatus;
    FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
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
    return (long)(sizeof(wchar_t)*(wcslen(pwsz)+1));
}

//-------------------------------------------------------------------
// Automatic registration; for out-of-process server support the
// -RegServer option
//-------------------------------------------------------------------
WinError_t RegisterComponent()
{
    wchar_t wszKey[MAX_PATH];
    wchar_t wszValue[MAX_PATH];
    HKEY hKey = 0;
	WinError_t winerr = 0;

    // HKEY_CLASSES_ROOT\CLSID\{DF22A6B2-A58A-11d1-ABCC-00207810D5FE}
    //  @="OcrEngine"
    wcscpy(wszKey, TEXT("CLSID\\{DF22A6B2-A58A-11d1-ABCC-00207810D5FE}"));
    winerr = RegCreateKey(HKEY_CLASSES_ROOT, wszKey, &hKey);
	if(winerr) {
		return winerr;
	}
    wcscpy(wszValue, TEXT("OcrEngine"));
    winerr = RegSetValueEx(hKey, 0, 0, REG_SZ, (BYTE*)wszValue, ByteLen(wszValue));
	if(winerr) {
		return winerr;
	}

    //  "AppID"="{EF20ACA0-C12A-11d1-ABF6-00207810D5FE}"
    wcscpy(wszValue, TEXT("{EF20ACA0-C12A-11d1-ABF6-00207810D5FE}"));
    RegSetValueEx(hKey, TEXT("AppID"), 0, REG_SZ, 
                  (BYTE*)wszValue, ByteLen(wszValue));
    RegCloseKey(hKey);

    // HKEY_CLASSES_ROOT\CLSID\{DF22A6B2-A58A-11d1-ABCC-00207810D5FE}\LocalServer32
    //      @="...path...\ocrsvr.exe"
    wcscpy(wszKey, TEXT("CLSID\\{DF22A6B2-A58A-11d1-ABCC-00207810D5FE}\\")TEXT("LocalServer32"));
    RegCreateKey(HKEY_CLASSES_ROOT, wszKey, &hKey);
    GetModuleFileName(0, wszValue, MAX_PATH);
    RegSetValueEx(hKey, 0, 0, REG_SZ, (BYTE*)wszValue, ByteLen(wszValue));
    RegCloseKey(hKey);

    // HKEY_CLASSES_ROOT\AppID\ocrsvr.exe
    //      "AppID"="{EF20ACA0-C12A-11d1-ABF6-00207810D5FE}"
    wcscpy(wszKey, TEXT("AppID\\ocrsvr.exe"));
    RegCreateKey(HKEY_CLASSES_ROOT, wszKey, &hKey);
    wcscpy(wszValue, TEXT("{EF20ACA0-C12A-11d1-ABF6-00207810D5FE}"));
    RegSetValueEx(hKey, TEXT("AppID"), 0, REG_SZ, 
                  (BYTE*)wszValue, ByteLen(wszValue));
    RegCloseKey(hKey);

    // HKEY_CLASSES_ROOT\AppID\{EF20ACA0-C12A-11d1-ABF6-00207810D5FE}
    //      @="OcrEngine"
    wcscpy(wszKey, TEXT("AppID\\{EF20ACA0-C12A-11d1-ABF6-00207810D5FE}"));
    RegCreateKey(HKEY_CLASSES_ROOT, wszKey, &hKey);
    wcscpy(wszValue, TEXT("OcrEngine"));
    RegSetValueEx(hKey, 0, 0, REG_SZ, 
                  (BYTE*)wszValue, ByteLen(wszValue));
    RegCloseKey(hKey);

	return NOERROR;
}

//-------------------------------------------------------------------
// Automatic unregistration; for out-of-process server support the
// -unregserver option
//-------------------------------------------------------------------
WinError_t UnregisterComponent()
{   
    long lRc = 0 ;
    lRc = RegDeleteKey(HKEY_CLASSES_ROOT, TEXT("CLSID\\") 
             TEXT("{DF22A6B2-A58A-11d1-ABCC-00207810D5FE}\\") 
             TEXT("LocalServer32"));
    DisplayStatus(TEXT("Unregistered LocalServer32"), lRc);
	if(lRc!=NOERROR)
		return lRc;
    
	lRc = RegDeleteKey(HKEY_CLASSES_ROOT, TEXT("CLSID\\") 
             TEXT("{DF22A6B2-A58A-11d1-ABCC-00207810D5FE}"));
    DisplayStatus(TEXT("Unregistered CLSID"), lRc);
	if(lRc!=NOERROR)
		return lRc;
    
	lRc = RegDeleteKey(HKEY_CLASSES_ROOT, TEXT("AppID\\") 
             TEXT("{EF20ACA0-C12A-11d1-ABF6-00207810D5FE}"));
    DisplayStatus(TEXT("Unregistered AppID"), lRc);
	if(lRc!=NOERROR)
		return lRc;
    
	lRc = RegDeleteKey(HKEY_CLASSES_ROOT, TEXT("AppID\\ocrsvr.exe"));
    DisplayStatus(TEXT("Unregistered ocrsvr.exe"), lRc);
	if(lRc!=NOERROR)
		return lRc;

	return NOERROR;
}
 
//*******************************************************************
//*******************************************************************
//***  Class CoOcrEngine - A component object that implements two
//***  interfaces:  IOcr and ISpell
//***  Simple implementation that doesn't support aggregation.
//***  This object cannot be aggregated by another object,
//***  Containment works with every component object!
//*******************************************************************
//*******************************************************************
class CoOcrEngine : public IOcr, public ISpell {
public:
    // constructors/destructors
    CoOcrEngine() : m_lRefCount(0) { 
		ComponentAddRef(); 
	}
    ~CoOcrEngine() { 
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
    {
		//------debug messages
        // wchar_t wszMsg[MAX_PATH];
        // wsprintf(wszMsg, TEXT("CoOcrEngine::AddRef (%ld)"), m_lRefCount); 
        // DisplayStatus(wszMsg, S_OK);
        return InterlockedIncrement(&m_lRefCount);
    }
    STDMETHODIMP_(ULONG) Release(void)
    {
		//------debug messages
        // wchar_t wszMsg[MAX_PATH];
        // wsprintf(wszMsg, TEXT("CoOcrEngine::Release (%ld)"), m_lRefCount-1); 
        // DisplayStatus(wszMsg, S_OK);

        long lCount = InterlockedDecrement(&m_lRefCount);
        if (lCount == 0) {
            // DisplayStatus(TEXT("CoOcrEngine::Release - delete object"), S_OK);
            delete this; 
        } 
        return lCount; 
    }

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
    LONG m_lRefCount;
};

//-------------------------------------------------------------------
// CoOcrEngine::CreateObject - static function to create an
// CoOcrEngine object
//-------------------------------------------------------------------
HRESULT CoOcrEngine::CreateObject(LPUNKNOWN pUnkOuter, 
								  REFIID riid, 
                                  void** ppv)
{
    *ppv = NULL;

	// CoOcrEngine doesn't support aggregation
    if (pUnkOuter != NULL) { 
		return CLASS_E_NOAGGREGATION; 
	}

    CoOcrEngine * pEngine = new CoOcrEngine;
    if (pEngine == NULL) { 
		return E_OUTOFMEMORY; 
	}

    HRESULT hr = pEngine->QueryInterface(riid, ppv);
	if (FAILED(hr)) { delete pEngine; }

    return hr;
}

//-------------------------------------------------------------------
// CoOcrEngine::QueryInterface
//-------------------------------------------------------------------
STDMETHODIMP
CoOcrEngine::QueryInterface(REFIID riid, void** ppv)
{
    //DisplayStatus(TEXT("CoOcrEngine::QueryInterface"), S_OK);

    if (ppv==NULL) { return E_INVALIDARG; }

	wchar_t *pIID=0;
	StringFromIID(riid, &pIID);
    wprintf(TEXT("CoOcrEngine::QueryInterface[%s]\n"), pIID);
	CoTaskMemFree(pIID);

    if (riid==IID_IUnknown) {
        *ppv= static_cast<IOcr *>(this);
    } else if (riid==IID_IOcr) {
        *ppv= static_cast<IOcr *>(this);
    }  else if (riid==IID_ISpell) {
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
    long lAllocSize = (long)( (wcslen(wszOcrText)+1) * sizeof(wchar_t) );
    wprintf(TEXT("lAllocSize = %u\n"), lAllocSize);

    *pwszOcrText = static_cast<wchar_t *>(CoTaskMemAlloc(lAllocSize));
    if (*pwszOcrText==NULL) { return E_OUTOFMEMORY; }

    //reinterpret_cast<wchar_t*>
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

	wchar_t *pIID=0;
	StringFromIID(riid, &pIID);
    wprintf(TEXT("CoOcrEngineFactory::QueryInterface[%s]\n"), pIID);
	CoTaskMemFree(pIID);

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
int main(int argc, char **argv)
{
	WinError_t winerr = 0;
    DisplayStatus(TEXT("Server: Started"), S_OK);

    g_hExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(g_hExitEvent);

    // registration if required
    if (argc > 1) {
        if (_stricmp(argv[1], "-RegServer")==0){
            winerr = RegisterComponent();
            DisplayStatus(TEXT("RegServer..."), winerr);
            return winerr;
        }
        if (_stricmp(argv[1], "-UnRegServer")==0){
            winerr = UnregisterComponent();
            DisplayStatus(TEXT("UnRegServer... "), winerr);
            return winerr;
        }
    }

    // Init COM
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));

    // Register OcrEngine Class Factory with COM
    DWORD dwRegister;
    hr = CoRegisterClassObject(CLSID_OcrEngine, 
                               &g_OcrEngineClassFactory,
                               CLSCTX_SERVER, 
                               REGCLS_MULTIPLEUSE, 
                               &dwRegister);
	assert(SUCCEEDED(hr));
	// we don't have to do this, but it's a good habit
    g_OcrEngineClassFactory.Release();  

    // sit and wait until CoOcrEngine object is used
    WaitForSingleObject(g_hExitEvent, INFINITE);

	// revoke the factory from public view
    CoRevokeClassObject(dwRegister);

	// uninitialize
    CoUninitialize();

    DisplayStatus(TEXT("Server shutting down in 5 seconds..."), S_OK);

    Sleep(5000);
	return 0;
}
