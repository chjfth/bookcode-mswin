// easyclient.cpp
#import "component\debug\component.dll" no_namespace
#include <iostream.h>

void main()
{
	CoInitialize(NULL);
	ISumPtr myRef(__uuidof(InsideDCOM));
	int result = myRef->Sum(5, 13);
	cout << "5 + 13 = " << result << endl;
	myRef = NULL;
	CoUninitialize();
}