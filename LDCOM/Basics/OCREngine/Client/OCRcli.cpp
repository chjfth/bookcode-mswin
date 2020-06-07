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
#include "..\idl\ocr.h"

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

   if (hr == S_OK) {
       wprintf(TEXT("%s\n"), pwszMsg);
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

void DisplayStatus(HRESULT hr)
{
   if (hr == S_OK) return ;

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
      NULL 
   );

   wprintf(TEXT("%s (%lx)\n"), pwszStatus, hr);

   LocalFree(pwszStatus);
}


//*******************************************************************
//*******************************************************************
//*** Thread -- All thread must enter an apartment by calling
//*** CoInitializeEx....  This one enters the MTA.
//*******************************************************************
//*******************************************************************
DWORD WINAPI UseSpellFunc(LPVOID lpSpell)
{
   CoInitializeEx(NULL, COINIT_MULTITHREADED);

   HRESULT hr=0;

   ISpell * pSpell = static_cast<ISpell*>(lpSpell);

   pSpell->AddRef();

   do {
      PossibleWords pw;
      hr = pSpell->Check(TEXT("AWord"), &pw);
      if (FAILED(hr))
      {
         DisplayStatus(TEXT("ISpell::Check failed"), hr);
         break;
      }
      EnterCriticalSection(&g_cs);
      wprintf(TEXT("ISpell::Check returned:  (%s, %s)\n"), pw.wszOne, pw.wszTwo);
      LeaveCriticalSection(&g_cs);
   } while(false);

   pSpell->Release();

   CoUninitialize();

   return S_OK;
}
 

//*******************************************************************
//*******************************************************************
//*** DemoMultiQI
//*** This function will go the a local or remote server!!!
//*** These are the one time when marshaling occurs and therefore
//*** proxies being create on the client side.  IMultiQI is only
//*** supported by proxies!!!
//*******************************************************************
//*******************************************************************
void DemoMultiQI()
{
   MULTI_QI mqi[] = { {&IID_IOcr, NULL, S_OK} };

    // we need to specify CLSCTX_LOCAL_SERVER|CLSCTX_REMOTE_SERVER
    // in order to successfully query for IMultiQI.
    // In other words, we need marshaling to happen before we
    // can qi for this interface.  In order for marshaling to
    // happen, we need to be in separate apartments.
    // Using CLSCTX_INPROC_SERVER will fail in our program
    // since both the client and the object live in the same
    // apartment...
    HRESULT hr = CoCreateInstanceEx(CLSID_OcrEngine, 
        NULL, 
        CLSCTX_LOCAL_SERVER|CLSCTX_REMOTE_SERVER,        
        NULL, 
        sizeof(mqi)/sizeof(mqi[0]), 
        mqi);

   if (SUCCEEDED(mqi[0].hr)) {
      IOcr *pUnk = static_cast<IOcr *>(mqi[0].pItf);
      IMultiQI *pMulti = 0;
      hr = pUnk->QueryInterface(IID_IMultiQI, (void**)&pMulti);
      assert(SUCCEEDED(hr));

      DisplayStatus(TEXT("IMultiQI..."), hr);
      MULTI_QI mqi[] = { {&IID_IOcr, NULL, S_OK}, {&IID_ISpell, NULL, S_OK} };
      hr = pMulti->QueryMultipleInterfaces( sizeof(mqi)/sizeof(mqi[0]), mqi);
      assert(SUCCEEDED(hr));
      pMulti->Release();
      assert(SUCCEEDED(mqi[0].hr)&&SUCCEEDED(mqi[1].hr));
      DisplayStatus(TEXT("QueryMultipleInterfaces..."), hr);

      // use the correct interface to release
      static_cast<IOcr *>(mqi[0].pItf)->Release();
      // use the correct interface to release
      static_cast<ISpell *>(mqi[1].pItf)->Release();
   } else {
      DisplayStatus(TEXT("IMultiQI..."), hr);
   }

   DisplayStatus(TEXT("Exit DemoMultiQI..."), S_OK);
}


//*******************************************************************
//*******************************************************************
//*** DemoUsage
//***
//*** QI for the interface pointer before use.
//*** Release the correct pointer.
//*******************************************************************
//*******************************************************************
void DemoUsage()
{
   MULTI_QI mqi[] = { 
      {&IID_IOcr, NULL, S_OK},
      {&IID_IPersistFile, NULL, S_OK}
   };

   HRESULT hr = CoCreateInstanceEx(CLSID_OcrEngine, 
                   NULL, 
                   CLSCTX_SERVER, 
                   NULL, 
                   sizeof(mqi)/sizeof(mqi[0]), 
                   mqi);

   if (SUCCEEDED(hr)) {

      if (SUCCEEDED(mqi[0].hr)) {

         IOcr *pOcr = static_cast<IOcr *>(mqi[0].pItf);
         ISpell *pSpell = 0;
         hr = pOcr->QueryInterface(IID_ISpell, 
                    reinterpret_cast<void**>(&pSpell));

         if (SUCCEEDED(hr)) {
            PossibleWords pw;
            pSpell->Check(TEXT("HaHa"), &pw);
            wprintf(TEXT("%s----%s\n"), pw.wszOne, pw.wszTwo);
            pSpell->Release();
         }

         pOcr->Release();

      } else {

         DisplayStatus(TEXT("IOcr failed..."), hr);
      }

      if (SUCCEEDED(mqi[1].hr)) {

         IPersistFile *pPersist = static_cast<IPersistFile *>(mqi[1].pItf);
         pPersist->Release();
      } else {

         DisplayStatus(TEXT("IPersistFile failed..."), hr);
      }

   }

   DisplayStatus(TEXT("Exit DemoUsage..."), S_OK);
}

//*******************************************************************
//*******************************************************************
//*** Main
//*******************************************************************
//*******************************************************************
void main(int argc, char **argv)
{
	DisplayStatus(TEXT("Client: Started"), S_OK);

	InitializeCriticalSection(&g_cs);
	HRESULT hr = S_OK;

	// Init COM
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));

	//*******************************************************************
	//*******************************************************************
	//*** QI interface before use.
	//*** Release the correct interface.
	//*** AddRef when copying an interface.
	//*******************************************************************
	//*******************************************************************
	DemoUsage();

	//*******************************************************************
	//*******************************************************************
	//*** MultiQI exists only when there is a proxy object.
	//*******************************************************************
	//*******************************************************************
	DemoMultiQI();

	//*******************************************************************
	//*******************************************************************
	//*** Use ISpell and IOcr
	//*******************************************************************
	//*******************************************************************
	COSERVERINFO csi, *pcsi=NULL;
	wchar_t wsz [MAX_PATH];
	DWORD dwClsCtx = CLSCTX_INPROC_SERVER;

	if (argc > 1) {
		// machine name
		mbstowcs(wsz, argv[1], strlen(argv[1])+1);
		memset(&csi, 0, sizeof(COSERVERINFO));
		csi.pwszName = wsz;
		pcsi = &csi;
		dwClsCtx = CLSCTX_LOCAL_SERVER|CLSCTX_REMOTE_SERVER;
	}

	// create an object on the remote machine
	DisplayStatus(TEXT("Creating instance..."), S_OK);

	MULTI_QI mqi[] = { {&IID_IOcr, NULL, S_OK}, {&IID_ISpell, NULL, S_OK} };

	hr = CoCreateInstanceEx(CLSID_OcrEngine, 
							NULL, 
							dwClsCtx, 
							pcsi, 
							sizeof(mqi)/sizeof(mqi[0]), 
							mqi);

	if (FAILED(hr)) {

		DisplayStatus(TEXT("CoCreateInstanceEx failed"), hr);

	} else {

		IOcr * pOcr = NULL ;
		ISpell * pSpell = NULL ;

		pOcr = static_cast<IOcr*>(mqi[0].pItf);

		do {
			// write the binary data to disk
			char szFile[MAX_PATH] = {"test.tif"} ;
			if (argc>2)
			{
				strcpy(szFile, argv[2]);
			}

			wchar_t wcFile[MAX_PATH];
			mbstowcs(wcFile, szFile, strlen(szFile)+1);

			HANDLE hFile = CreateFile(wcFile,
				GENERIC_READ,
				FILE_SHARE_READ, 
				NULL, 
				OPEN_EXISTING, 
				FILE_ATTRIBUTE_NORMAL, NULL);

			if (hFile==INVALID_HANDLE_VALUE)
			{
				wprintf(TEXT("hFile invalid\n"));
				break;
			}

			DWORD dwFileSize = GetFileSize(hFile, &dwFileSize);
			BYTE *pbImage = new BYTE[dwFileSize];
			DWORD dwRead = 0;
			BOOL bRc = ReadFile(hFile, (void *)pbImage,
								dwFileSize, &dwRead, NULL);
			CloseHandle(hFile);

			wchar_t *pOut=0;
			hr = pOcr->OcrImage(dwFileSize, (byte*)pbImage, &pOut);

			delete [] pbImage ;

			if (FAILED(hr))
			{
				DisplayStatus(TEXT("IOcr::OcrImage failed"), hr);
				break ;
			}
			wprintf(TEXT("IOcr::OcrImage returned: (%s)\n"), pOut);
			CoTaskMemFree(pOut);

			#define arraysz 10
			HANDLE threads[arraysz];

			for (short s=0; s<arraysz; s++)
			{  // first method
				DWORD dwTid=0;
				threads[s] = CreateThread(NULL, 0, UseSpellFunc, 
											(void*) mqi[1].pItf, 
											0, &dwTid);
			}

			WaitForMultipleObjects(arraysz, threads, TRUE, INFINITE);
		} while(false);

		if (pSpell!=NULL) { pSpell->Release(); }
		if (pOcr!=NULL) { pOcr->Release(); }
	}

	CoUninitialize();
	DisplayStatus(TEXT("Client shutting down..."), S_OK);

	DeleteCriticalSection(&g_cs);
}

