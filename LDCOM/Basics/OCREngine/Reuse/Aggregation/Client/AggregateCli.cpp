//******************************************************************
//*******************************************************************
//*** Client that uses the CLSID_Thesaurus object which reuses
//*** the ISpell and IOcr interfaces of the CLSID_OcrEngine 
//*** object by aggregation
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
#include "..\..\..\idl\ocr.h"   
#include "..\idl\aggregate.h"   

//-------------------------------------------------------------------
//  includes
//-------------------------------------------------------------------
CRITICAL_SECTION g_cs;

//*******************************************************************
//*******************************************************************
//***  UTILITY FUNCTIONS
//*******************************************************************
//*******************************************************************
void DisplayStatus(wchar_t *pwszMsg, HRESULT hr)
{

   if (hr == S_OK) { wprintf(TEXT("%s\n"), pwszMsg); return; }

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


//*******************************************************************
//*******************************************************************
//*** Main
//*******************************************************************
//*******************************************************************
void main(int argc, char **argv)
{
   // In order to run this program, you must you the following:
   // (1) register the inner.dll, since it's used by aggregate.exe
   // (2) register the ocrps.dll proxy/stub
   // (3) register the aggregateps.dll proxy/stub
   // (4) register aggregate.exe for the SCM to find it.
   //     i.e., if you don't register, you must run 
   //     aggregate.exe so that it registers the its CLSID
   //     with the SCM via CoRegisterClassObject!
   //  2&3 are need because this client runs in separate
   //  process from aggregate.exe, which is this client's server.


   InitializeCriticalSection(&g_cs); 

   DisplayStatus(TEXT("Client: Started"), S_OK);

   HRESULT hr = S_OK;

   // Init COM
   hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
   assert(SUCCEEDED(hr));

  MULTI_QI mqi[] = {
       {&IID_IThesaurus, NULL, E_FAIL}, 
       {&IID_ISpell, NULL, E_FAIL} 
   };

   // client testing aggregation : CLSID_Thesaurus is
   // object aggregating CLSID_OcrEngine
   hr = CoCreateInstanceEx(CLSID_Thesaurus, 
       NULL, 
       CLSCTX_SERVER, 
       NULL, 
       sizeof(mqi)/sizeof(mqi[0]), 
       mqi);
   DisplayStatus(TEXT("CoCreateInstanceEx"), hr);

   if (SUCCEEDED(mqi[0].hr) && SUCCEEDED(mqi[1].hr)) {
       IThesaurus * pDict = static_cast<IThesaurus*>(mqi[0].pItf);
       ISpell * pSpell = static_cast<ISpell*>(mqi[1].pItf);

       hr = pDict->LookUp();
       DisplayStatus(TEXT("Called pDict->LookUp"), hr);

       // This call here supported by the CLSID_Thesaurus object
       // but it forwards the call to the CLSID_OcrEngine object
       // because it reuses the CLSID_OcrEngine object by
       // containment.
       PossibleWords pw;
       hr = pSpell->Check(TEXT("AWord"), &pw);
       DisplayStatus(TEXT("Called pSpell->Check"), hr);
       wprintf(TEXT("ISpell::Check returned:  (%s, %s)\n"), pw.wszOne, pw.wszTwo);

       pSpell->Release();
       pDict->Release();
   }

   CoUninitialize();
   DisplayStatus(TEXT("Client shutting down..."), S_OK);

   DeleteCriticalSection(&g_cs);
}

