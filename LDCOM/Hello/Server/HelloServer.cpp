//*******************************************************************
//* Required macros
//*******************************************************************
#ifndef UNICODE
#define UNICODE         // UNICODE
#endif

//*******************************************************************
//* Includes
//*******************************************************************
#include <assert.h>
#include <stdio.h>
#include <windows.h>
#include <initguid.h>
#include "..\idl\hello.h"

//*******************************************************************
//* [Chapter 3: Interfaces]  
//*  GUID for IHello interface.
//*******************************************************************
const IID IID_IHello = {
   0x11111111,0x1111,0x1111,{0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11}
};

//*******************************************************************
//* [Chapter 3: Objects]  
//*  CoHello component object implementation.
//*******************************************************************
class CoHello : public IHello {
public:
   // constructors/destructors
   CoHello() : m_lRefCount(0) 
   { 
      // [Chapter 3: Objects][Chapter 3: Servers]
   }

   ~CoHello() 
   { 
      // [Chapter 3: Objects][Chapter 3: Servers]
   }

public: 
   // IUnknown Methods
   STDMETHODIMP QueryInterface(REFIID riid, void **ppv)
   {
      // [Chapter 3: Objects]
      if (riid==IID_IUnknown||riid==IID_IHello) {
         *ppv= (IHello *)(this);
      } else {
         *ppv=NULL; return E_NOINTERFACE ;
      }

      AddRef();
      return S_OK;
   }

   STDMETHODIMP_(ULONG) AddRef(void)
   {
      // [Chapter 3: Objects]
      return ++m_lRefCount;
   }

   STDMETHODIMP_(ULONG) Release(void)
   {
      // [Chapter 3: Objects]
      long lCount = m_lRefCount-1;
      if (lCount == 0) {
         delete this; 
      } 
      return lCount; 
   }

   // IHello Methods
   STDMETHODIMP SayHello()
   {
      printf("Hello, Universe!\n");
      return S_OK;
   }

private:
    LONG m_lRefCount;
};



//*******************************************************************
//* [Chapter 3: Interfaces]  
//*  GUID for Hello class object. 
//*******************************************************************
const CLSID CLSID_Hello = {
   0xCCCCCCCC,0xCCCC,0xCCCC,{0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC}
};

//*******************************************************************
//* [Chapter 3: Classes]  
//*  Class Factory that manufactures CoHello objects.
//*******************************************************************
class CoHelloFactory : public IClassFactory 
{
public:
   // IUnknown Methods
   STDMETHODIMP QueryInterface (REFIID riid, void** ppv)
   {
      if (riid==IID_IUnknown||riid==IID_IClassFactory) {
         *ppv= static_cast<IClassFactory *>(this);
      } else {
         *ppv=NULL; return E_NOINTERFACE ;
      }

      return S_OK;
   }
       
   STDMETHODIMP_(ULONG) AddRef(void)
   { 
      return 1; 
   }

   STDMETHODIMP_(ULONG) Release(void) 
   { 
      return 1; 
   }

   // IClassFactory Methods
   STDMETHODIMP CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, void **ppv)
   {
      // [Chapter 3: Classes]
      CoHello * pHello = new CoHello;
      return pHello->QueryInterface(riid, ppv);
   }

   STDMETHODIMP LockServer(BOOL fLock)
   {
      // [Chapter 3: Classes][Chapter 4: Servers]
      return S_OK;        
   }   
};

const IID IID_IAnyvalue = {
	0x11111144,0x1111,0x1111,{0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11}
};

//*******************************************************************
//* Internally register the marshaler for the IID_IHello interface.
//*******************************************************************
void RegisterInterfaceMarshaler()
{
   // [Chapter 9][Chapter 10]
   DWORD dwCookie=0;
   IUnknown *pUnk=0;
   ::DllGetClassObject(IID_IHello, IID_IUnknown, (void**)&pUnk);
   Sleep(50);
   ::CoRegisterClassObject(IID_IAnyvalue, pUnk, CLSCTX_INPROC_SERVER,
                           REGCLS_MULTIPLEUSE, &dwCookie);
   Sleep(50);
   ::CoRegisterPSClsid(IID_IHello, IID_IAnyvalue); // COM background threads created inside
   Sleep(50);
}

//*******************************************************************
//* The factory that can create Hello objects is a globally defined.
//*******************************************************************
CoHelloFactory g_HelloClassFactory;

//*******************************************************************
//* The server component's main() routine.
//*******************************************************************
void main(int argc, char **argv)
{
   // [Chapter 4:  Initialization and Termination] Initialize COM
   CoInitializeEx(NULL, COINIT_MULTITHREADED);

   // [Chapter 9][Chapter 10] Register the marshaling code for IHello.
   RegisterInterfaceMarshaler();

   DWORD dwCookie=0;
   // [Chapter 4:  Dynamic Activation Support] 
   //              Make the Hello class factory public.
   CoRegisterClassObject(CLSID_Hello, 
                      &g_HelloClassFactory,
                      CLSCTX_SERVER, 
                      REGCLS_MULTIPLEUSE, 
                      &dwCookie);

   printf("Press CTRL-C to stop this server.\n");

   // For demonstration, this component will live forever.
   Sleep(INFINITE);
}



