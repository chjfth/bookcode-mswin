// component.cpp
// This in-process component can be installed with the ThreadingModel value
// set to Apartment, Free, or Both. The can be adjusted in the last parameter
// of the RegisterServer function.

#define _WIN32_DCOM
#include <iostream>  // For cout
using namespace std;

#include "comp/comp.h"      // Generated by MIDL
#include "../registry.h"  // For registry functions

HINSTANCE g_hInstance;
long g_cComponents = 0;
long g_cServerLocks = 0;

class CInsideDCOM : public ISum
{
public:
	// IUnknown
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall QueryInterface(REFIID riid, void** ppv);

	// ISum
	HRESULT __stdcall Sum(int x, int y, int* retval);

	CInsideDCOM();
	~CInsideDCOM();

private:
	long m_cRef;
};

CInsideDCOM::CInsideDCOM() : m_cRef(1)
{
	InterlockedIncrement(&g_cComponents);
}

CInsideDCOM::~CInsideDCOM()
{
	InterlockedDecrement(&g_cComponents);
}

ULONG CInsideDCOM::AddRef()
{
	cout << "Component: CInsideDCOM::AddRef() m_cRef = " << m_cRef + 1 << endl;
	return InterlockedIncrement(&m_cRef);
}

ULONG CInsideDCOM::Release()
{
	cout << "Component: CInsideDCOM::Release() m_cRef = " << m_cRef - 1 << endl;
	ULONG cRef = InterlockedDecrement(&m_cRef);
	if(cRef != 0)
		return cRef;
	delete this;
	return 0;
}

HRESULT CInsideDCOM::QueryInterface(REFIID riid, void** ppv)
{
	if(riid == IID_IUnknown)
	{
		cout << "Component: CInsideDCOM::QueryInterface() for IUnknown returning " << this << endl;
		*ppv = (IUnknown*)this;
	}
	else if(riid == IID_ISum)
	{
		cout << "Component: CInsideDCOM::QueryInterface() for ISum returning " << this << endl;
		*ppv = (ISum*)this;
	}
	else 
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}
	AddRef();
	return S_OK;
}

HRESULT CInsideDCOM::Sum(int x, int y, int* retval)
{
//	cout << "Component: CInsideDCOM::Sum() " << x << " + " << y << " = " << x + y << endl;
	*retval = x + y;
	return S_OK;
}

class CFactory : public IClassFactory
{
public:
	// IUnknown
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall QueryInterface(REFIID riid, void** ppv);

	// IClassFactory
	HRESULT __stdcall CreateInstance(IUnknown *pUnknownOuter, REFIID riid, void** ppv);
	HRESULT __stdcall LockServer(BOOL bLock);

	CFactory() : m_cRef(1) { }
	~CFactory() { }

private:
	long m_cRef;
};

ULONG CFactory::AddRef()
{
	cout << "Component: CFactory::AddRef() m_cRef = " << m_cRef + 1 << endl;
	return InterlockedIncrement(&m_cRef);
}

ULONG CFactory::Release()
{
	cout << "Component: CFactory::Release() m_cRef = " << m_cRef - 1 << endl;
	ULONG cRef = InterlockedDecrement(&m_cRef);
	if(cRef != 0)
		return cRef;
	delete this;
	return 0;
}

HRESULT CFactory::QueryInterface(REFIID riid, void** ppv)
{
	if((riid == IID_IUnknown) || (riid == IID_IClassFactory))
	{
		cout << "Component: CFactory::QueryInteface() for IUnknown or IClassFactory " << this << endl;
		*ppv = (IClassFactory *)this;
	}
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}
	AddRef();
	return S_OK;
}

HRESULT CFactory::CreateInstance(IUnknown *pUnknownOuter, REFIID riid, void** ppv)
{
	if(pUnknownOuter != NULL)
		return CLASS_E_NOAGGREGATION;

	CInsideDCOM *pInsideDCOM = new CInsideDCOM;
	cout << "Component: CFactory::CreateInstance() " << pInsideDCOM << endl;

	if(pInsideDCOM == NULL)
		return E_OUTOFMEMORY;

	// QueryInterface probably for IID_IUNKNOWN
	HRESULT hr = pInsideDCOM->QueryInterface(riid, ppv);
	pInsideDCOM->Release();
	return hr;
}

HRESULT CFactory::LockServer(BOOL bLock)
{
	if(bLock)
		InterlockedIncrement(&g_cServerLocks);
	else
		InterlockedDecrement(&g_cServerLocks);
	return S_OK;
}

HRESULT __stdcall DllCanUnloadNow()
{
	cout << "Component: DllCanUnloadNow() " << (g_cServerLocks == 0 && g_cComponents == 0 ? "Yes" : "No") << endl;
	if(g_cServerLocks == 0 && g_cComponents == 0)
		return S_OK;
	else
		return S_FALSE;
}

HRESULT __stdcall DllGetClassObject(REFCLSID clsid, REFIID riid, void** ppv)
{
	cout << "Component: DllGetClassObject" << endl;
	
	if(clsid != CLSID_InsideDCOM2)
		return CLASS_E_CLASSNOTAVAILABLE;

	CFactory* pFactory = new CFactory;
	if(pFactory == NULL)
		return E_OUTOFMEMORY;

	// QueryInterface probably for IClassFactory
	HRESULT hr = pFactory->QueryInterface(riid, ppv);
	pFactory->Release();
	return hr;
}

char g_wszDllFilepath[MAX_PATH];

HRESULT __stdcall DllRegisterServer()
{
	char DllPath[256];
	OLECHAR wDllPath[256];
	GetModuleFileName(g_hInstance, DllPath, 256);
	mbstowcs(wDllPath, DllPath, 256);
	ITypeLib* pTypeLib;
	HRESULT hr = LoadTypeLibEx(wDllPath, REGKIND_REGISTER, &pTypeLib);
	if(FAILED(hr))
		return hr;
	pTypeLib->Release();

	// Adjust the threading model here
	return RegisterServer(g_wszDllFilepath, 
		CLSID_InsideDCOM2, // note: this internal object uses CLSID_InsideDCOM2, not the outer CLSID_InsideDCOM.
		"Inside DCOM Sample", "Component.InsideDCOM2", "Component.InsideDCOM2.1", "Apartment");
}

HRESULT __stdcall DllUnregisterServer()
{
	HRESULT hr = UnRegisterTypeLib(LIBID_Comp, 1, 0, LANG_NEUTRAL, SYS_WIN32);
	if(FAILED(hr))
		return hr;
	return UnregisterServer(CLSID_InsideDCOM2, "Component.InsideDCOM2", "Component.InsideDCOM2.1");
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, void* pv)
{
	g_hInstance = hInstance;
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		GetModuleFileName(hInstance, g_wszDllFilepath, MAX_PATH);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
	}
	return TRUE;
}
