//******************************************************************
//*******************************************************************
//*** Client that uses the CLSID_Dictionary object which reuses
//*** the ISpell interface of the CLSID_OcrEngine object by
//*** containment.
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
#include "..\idl\contain.h"  

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
   InitializeCriticalSection(&g_cs); 

   DisplayStatus(TEXT("Client: Started"), S_OK);

   HRESULT hr = S_OK;

   // Init COM
   hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
   assert(SUCCEEDED(hr));

   MULTI_QI mqi[] = {
      {&IID_IDictionary, NULL, E_FAIL}, 
      {&IID_ISpell, NULL, E_FAIL} 
   };

   hr = CoCreateInstanceEx(CLSID_Dictionary, 
       NULL, 
       CLSCTX_SERVER, 
       NULL, 
       sizeof(mqi)/sizeof(mqi[0]), 
       mqi);
   DisplayStatus(TEXT("CoCreateInstanceEx"), hr);

   if (SUCCEEDED(mqi[0].hr) && SUCCEEDED(mqi[1].hr)) {
      IDictionary * pDict = static_cast<IDictionary*>(mqi[0].pItf);
      ISpell * pSpell = static_cast<ISpell*>(mqi[1].pItf);

      hr = pDict->LookUp();
      DisplayStatus(TEXT("Called pDict->LookUp"), hr);

      // This call here supported by the CLSID_Dictionary object
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

