// client.cpp
#define _WIN32_DCOM
#include <iostream>
#include <conio.h>
using namespace std;

#include "Component\component.h"

void main()
{
	cout << "Client: Calling CoInitialize()" << endl;
	CoInitialize(NULL);
		
	IUnknown* pUnknown;
	ISum* pSum;

	cout << "Client: Calling CoCreateInstance() " << endl;
	HRESULT hr = CoCreateInstance(CLSID_InsideDCOM, NULL, CLSCTX_LOCAL_SERVER, IID_IUnknown, (void**)&pUnknown);
	if(FAILED(hr)) {
		cout << "CoCreateInstance() failed with HRESULT=0x" << hex << hr << endl;
		return;
	}

	cout << "Client: Calling QueryInterface() for ISum on " << pUnknown << endl;
	hr = pUnknown->QueryInterface(IID_ISum, (void**)&pSum);

	if(FAILED(hr))
		cout << "QueryInterface FAILED" << endl;

	cout << "Client: Calling Release() for pUnknown" << endl;
	hr = pUnknown->Release();

	cout << "Client: pSum = " << pSum << endl;

	int sum;
	hr = pSum->Sum(4, 9, &sum);
	if(SUCCEEDED(hr))
		cout << "Client: Calling Sum() return value is " << sum << endl;
	else
		cout << "Client: Calling Sum() failed. HRESULT=0x" << std::hex << hr << endl;

	cout << "Chj: == Press any key to go on doing final Release(). ==" << endl;
	cout << "     == You can take the chance to inspect this process with a debugger. ==" << endl;
	_getch();

	cout << "Client: Calling Release() for pSum" << endl;
	hr = pSum->Release();

	cout << "Client: Calling CoUninitialize()" << endl;
	CoUninitialize();
}