// client.cpp
#define _WIN32_DCOM
#include <iostream.h>
#include "Component\component.h"

void main()
{
	cout << "Client: Calling CoInitialize()" << endl;
	CoInitialize(NULL);
		
	IUnknown* pUnknown;
	ISum* pSum;

	cout << "Client: Calling CoCreateInstance() " << endl;
	CoCreateInstance(CLSID_InsideDCOM, NULL, CLSCTX_LOCAL_SERVER, IID_IUnknown, (void**)&pUnknown);

	cout << "Client: Calling QueryInterface() for ISum on " << pUnknown << endl;
	HRESULT hr = pUnknown->QueryInterface(IID_ISum, (void**)&pSum);

	if(FAILED(hr))
		cout << "QueryInterface FAILED" << endl;

	cout << "Client: Calling Release() for pUnknown" << endl;
	hr = pUnknown->Release();

	cout << "Client: pSum = " << pSum << endl;

	int sum;
	hr = pSum->Sum(4, 9, &sum);
	if(SUCCEEDED(hr))
		cout << "Client: Calling Sum() return value is " << sum << endl;

	cout << "Client: Calling Release() for pSum" << endl;
	hr = pSum->Release();

	cout << "Client: Calling CoUninitialize()" << endl;
	CoUninitialize();
}