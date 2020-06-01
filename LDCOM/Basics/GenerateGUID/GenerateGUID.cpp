#include <windows.h>
#include <objbase.h>
#include <stdio.h>

void main()
{
	CoInitialize(0);
	GUID guid;
	CoCreateGuid(&guid);

	wchar_t strGuid[40];
	StringFromGUID2(guid, strGuid, 40);

	wprintf(L"%s\r\n", strGuid);

	CoUninitialize();
}
