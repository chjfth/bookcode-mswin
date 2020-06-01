//******************************************************************
//*******************************************************************
//*** Client
//*******************************************************************
//*******************************************************************

//-------------------------------------------------------------------
//  Required definitions
//-------------------------------------------------------------------
#define UNICODE
#define _WIN32_DCOM

//-------------------------------------------------------------------
//  includes
//-------------------------------------------------------------------
#include <assert.h>
#include <stdio.h>
#include <windows.h>
#include <initguid.h>
#include "..\OCREngine\idl\ocr.h"

//*******************************************************************
//*******************************************************************
//***  UTILITY FUNCTIONS
//*******************************************************************
//*******************************************************************
void DisplayStatus(wchar_t *pwszMsg, HRESULT hr)
{

    if (hr == S_OK)
    {
        wprintf(TEXT("%s\n"), pwszMsg);
        return;
    }

    if (HRESULT_FACILITY(hr) == FACILITY_WINDOWS)
        hr = HRESULT_CODE(hr);

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

wchar_t *GetBestOcrFactory()
{
    static wchar_t *pwszChangeableFactory = 
        TEXT("CLSID:DF22A6B2-A58A-11d1-ABCC-00207810D5FE:");
    return pwszChangeableFactory;
}

//*******************************************************************
//*******************************************************************
//*** Main
//*******************************************************************
//*******************************************************************
void main(int argc, char **argv)
{
    DisplayStatus(TEXT("Client: Started"), S_OK);

    HRESULT hr = S_OK;

    // Init COM
    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr)) { return ; }

    // Dynamically get the best OCR Factory
    // This extra level of indirection allows 
    // you to build cool distributed software.
    wchar_t *pwszDispName = GetBestOcrFactory();

    // Create a binding context
    IBindCtx *pBind = 0;
    hr = CreateBindCtx(0, &pBind);
    assert(SUCCEEDED(hr));

    // Create a class moniker based on the textual name
    // A class moniker represents a factory
    ULONG ulEaten = 0;
    IMoniker *pMon = 0;
    hr = MkParseDisplayName(pBind, pwszDispName, &ulEaten, &pMon);
    assert(SUCCEEDED(hr));

    // Bind to the factory and get the IID_IClassFactory interface
    IClassFactory *pClassFactory = 0;
    hr = pMon->BindToObject(pBind, 0, IID_IClassFactory, (void**)&pClassFactory);
    assert(SUCCEEDED(hr));
    pMon->Release();
    pBind->Release();

    // Tell the factory to manufacture an object
    // and get back a pointer to its IOcr interface
    IOcr *pOcr =0;
    hr = pClassFactory->CreateInstance(0, IID_IOcr, (void**)&pOcr);
    assert(SUCCEEDED(hr));
    pClassFactory->Release();

    // Call pOcr methods.
    DisplayStatus(TEXT("Make good use of the object.\n")
		          TEXT("Call pOcr methods...\n")
		          TEXT("This sample creates an object using a Class Moniker.\n")
		          TEXT("Check out the code, and see how simple it is!"), 
				  S_OK);

    pOcr->Release();

    CoUninitialize();
    DisplayStatus(TEXT("Client shutting down..."), S_OK);
}

