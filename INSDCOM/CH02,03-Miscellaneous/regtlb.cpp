// regtlb.cpp
#include <windows.h>
#include <iostream.h>

void main(int argc, char** argv)
{
	if(argc < 2)
	{
		cout << "Usage: regtlb tlbfile.tlb" << endl;
		return;
	}

	CoInitialize(NULL);

	OLECHAR psz[255];
	MultiByteToWideChar(CP_ACP, 0, argv[1], strlen(argv[1]), psz, 255);

	ITypeLib* pTypeLib;
	HRESULT hr = LoadTypeLibEx(psz, REGKIND_REGISTER, &pTypeLib);
	if(FAILED(hr))
	{
		cout << "LoadTypeLibEx failed" << endl;
		return;
	}
	else
		cout << "Type library registered" << endl;

	pTypeLib->Release();

	CoUninitialize();
}