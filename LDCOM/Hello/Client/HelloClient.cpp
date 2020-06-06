//*******************************************************************
//* Client component.
//*******************************************************************

#ifndef UNICODE
#define UNICODE         // UNICODE
#endif

#include <tchar.h>
#include <assert.h>
#include <stdio.h>
#include <windows.h>
#include <initguid.h>
#include "..\idl\hello.h"

//*******************************************************************
//* GUID for Hello class object. 
//*******************************************************************
const CLSID CLSID_Hello = {
   0xCCCCCCCC,0xCCCC,0xCCCC,{0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC}
};

//*******************************************************************
//* GUID for IHello interface.
//*******************************************************************
const IID IID_IHello = {
   0x11111111,0x1111,0x1111,{0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11}
};
const IID IID_IAnything = {
	0x11111100,0x1111,0x1111,{0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11}
};

//*******************************************************************
//* Internally register the marshaler for the IID_IHello interface.
//*******************************************************************
void RegisterInterfaceMarshaler()
{
   // [Chapter 9][Chapter 10]
   DWORD dwCookie=0;
   IUnknown *pUnk=0;
   ::DllGetClassObject(IID_IHello, IID_IUnknown, (void**)&pUnk); // call into dlldata.c
   ::CoRegisterClassObject(IID_IAnything, pUnk, CLSCTX_INPROC_SERVER,
                           REGCLS_MULTIPLEUSE, &dwCookie);
   ::CoRegisterPSClsid(IID_IHello, IID_IAnything);
}

//*******************************************************************
//*  Client component's main() routine.
//*******************************************************************
void _tmain(int argc, WCHAR **argv)
{
	WCHAR *server = L"localhost"; // Chj
	if(argc>1)
		server = argv[1];

   // [Chapter 4:  Initialization and Termination]
   CoInitializeEx(NULL, COINIT_MULTITHREADED);

   // [Chapter 9][Chapter 10] Register the marshaling code for IHello.
   RegisterInterfaceMarshaler();

   // [Chapter 4:  Creating an Object]
   // Request for the IHello interface.
   MULTI_QI mqi[] = { {&IID_IHello, NULL, S_OK} };
   
   COSERVERINFO csi = {0, server, NULL, 0};
   // Create an instance of the Hello distributed component object.
   CoCreateInstanceEx(CLSID_Hello, 
                      NULL, 
                      CLSCTX_SERVER, 
                      &csi, 
                      sizeof(mqi)/sizeof(mqi[0]), 
                      mqi);

   // [Chapter 4:  Using an Object]
   IHello *pHello = (IHello *)(mqi[0].pItf);
   pHello->SayHello();
   pHello->Release();

   // [Chapter 4:  Initialization and Termination]
   CoUninitialize();
}

