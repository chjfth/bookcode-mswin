// jclient.cpp
// Before running, copy SumClass.class to C:\Windows\Java\Lib
#import "JavaSum\SumClasslib.tlb" no_namespace
#include <iostream.h>

void main()
{
	CoInitialize(NULL);
	ISumClassPtr myRef(__uuidof(CSumClass));
	int result = myRef->Sum(5, 13);
	cout << "5 + 13 = " << result << endl;
	myRef = NULL;
	CoUninitialize();
}