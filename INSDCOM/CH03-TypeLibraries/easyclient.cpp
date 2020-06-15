// easyclient.cpp

#if _MSC_VER <= 1200
// for original VC6
#import "component\debug\component.dll" no_namespace // old statement
#elif PlatformName==Win32
// for VC2010+
#import "Component\Win32\component.tlb" no_namespace
#elif PlatformName==x64
// for VC2010+
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