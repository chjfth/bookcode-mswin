#include <stdio.h>
#include <Windows.h>

int main()
{
	CoInitialize(0);

	// Get the CLSID that maps to the program ID named Word.Basic.
	GUID clsid = {};
	CLSIDFromProgID(L"Word.Basic", &clsid);
	if(clsid==CLSID_NULL) {
		wprintf(L"ProgID \"Word.Basic\" not found. Microsoft Word not installed.\n");
		return 4;
	}

	// Activate the object and request for as IDispatch interface.
	IDispatch *pDisp = 0;
	HRESULT hr = CoCreateInstance(clsid, NULL, CLSCTX_LOCAL_SERVER, 
		IID_IDispatch, (void**)&pDisp);
	if(!SUCCEEDED(hr)) {
		wprintf(L"Get IID_IDispatch interface fail.\n");
		return 4;
	}

	// We need to know the name of the method we want to invoke.
	OLECHAR *method = L"AppShow";

	// The AppShow method expects no parameter.
	DISPPARAMS parms = {NULL, NULL, 0, 0};

	// Given a method name, get back the corresponding dispid.
	DISPID dispid;  
	pDisp->GetIDsOfNames(IID_NULL, &method, 1, LOCALE_USER_DEFAULT, &dispid);

	// Dynamically invoke the the AppShow method using a dispid.
	pDisp->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, 
		DISPATCH_METHOD, &parms, NULL, NULL, NULL);

	//  Do whatever....
	Sleep(5000);

	pDisp->Release();
	CoUninitialize();

	return 0;
}
