//******************************************************************
//*******************************************************************
//*** Persist
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
#include <stdio.h>
#include <windows.h>
#include <initguid.h>

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
	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
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
	DisplayStatus(TEXT("Client: Started\n")
		TEXT("Simple example demonstrating creating\n")
		TEXT("and initializing and object from a \n")
		TEXT("persistent file.\n\n")
		TEXT("\nEnter any office file and check it out.\n"),
		S_OK);

	HRESULT hr = S_OK;

	// Init COM
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr)) { return ; }

	if (argc<2) {
		wprintf(TEXT("Enter full path and file name for any office file;\n")
			TEXT("i.e., word, excel, powerpoint.\n\n"));
		printf("Example:\n\t%s c:\\msoffice\\mydocs\\sample.doc\n", argv[0]);
		CoUninitialize(); 
		return;
	}

	wchar_t pwszPersist[MAX_PATH];
	mbstowcs(pwszPersist, argv[1], strlen(argv[1])+1);

	MULTI_QI mqi[] = { {&IID_IDataObject, NULL, S_OK} };
	hr = CoGetInstanceFromFile(0,0,0, CLSCTX_SERVER, 
		STGM_READ, pwszPersist, 
		sizeof(mqi)/sizeof(mqi[0]), 
		mqi);

	IDataObject *pData = 0;

	// You can also use the short hand version...
	// hr = CoGetObject(pwszPersist, 0, IID_IDataObject, (void**) &pData);

	if (SUCCEEDED(hr)) {
		FORMATETC formatEtc = 
		{CF_TEXT, 0, DVASPECT_CONTENT,-1, TYMED_HGLOBAL };

		STGMEDIUM stgMedium;
		pData = reinterpret_cast<IDataObject*>(mqi[0].pItf);
		hr = pData->GetData(&formatEtc,	&stgMedium);

		if (SUCCEEDED(hr)) {
			// extracting the data
			char *lpString = (char *)GlobalLock(stgMedium.hGlobal);
			if (lpString) {
				printf("Here's the data:\n\n\n%s\n", lpString);
			}
			GlobalUnlock(stgMedium.hGlobal);
		}
		ReleaseStgMedium(&stgMedium);
		pData->Release();
	} else {
		DisplayStatus(TEXT("HINT: Full path required!"), hr);
	}

	CoUninitialize();
	DisplayStatus(TEXT("Client shutting down..."), S_OK);
}
