// easyclient.cpp

// #import "component\debug\component.dll" no_namespace // old statement
// For Chj's VC2010 pattern, it should be modified to:
#if PlatformName==Win32
#import "Component\Win32\component.tlb" no_namespace
#elif PlatformName==x64
#import "Component\Win32\component.tlb" no_namespace
#else
#error Unknown PlatformName, I cannot continue.
#endif

#include <iostream>
using namespace std;

void main()
{
	CoInitialize(NULL);
	ISumPtr myRef(__uuidof(InsideDCOM));
	int result = myRef->Sum(5, 13);
	cout << "5 + 13 = " << result << endl;
	myRef = NULL;
	CoUninitialize();
}