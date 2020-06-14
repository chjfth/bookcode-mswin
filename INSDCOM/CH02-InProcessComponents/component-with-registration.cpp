// component.cpp
#include <iostream>
using namespace std;

#include "Component-with-Registration\component.h" // Generated by MIDL
#include "registry.h" // Add This!!!

// {10000002-0000-0000-0000-000000000001}
const CLSID CLSID_InsideDCOM = {0x10000002,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01}};

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

	CInsideDCOM() : m_cRef(1) { g_cComponents++; }
	~CInsideDCOM() { cout << "Component: CInsideDCOM::~CInsideDCOM()" << endl, g_cComponents--; }

private:
	ULONG m_cRef;
};

ULONG CInsideDCOM::AddRef()
{
	cout << "Component: CInsideDCOM::AddRef() m_cRef = " << m_cRef + 1 << endl;
	return ++m_cRef;
}

ULONG CInsideDCOM::Release()
{
	cout << "Component: CInsideDCOM::Release() m_cRef = " << m_cRef - 1 << endl;
	if(--m_cRef != 0)
		return m_cRef;
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
	cout << "Component: CInsideDCOM::Sum() " << x << " + " << y << " = " << x + y << endl;
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
	ULONG m_cRef;
};

ULONG CFactory::AddRef()
{
	cout << "Component: CFactory::AddRef() m_cRef = " << m_cRef + 1 << endl;
	return ++m_cRef;
}

ULONG CFactory::Release()
{
	cout << "Component: CFactory::Release() m_cRef = " << m_cRef - 1 << endl;
	if(--m_cRef != 0)
		return m_cRef;
	delete this;
	return 0;
}

HRESULT CFactory::QueryInterface(REFIID riid, void** ppv)
{
	if(riid == IID_IUnknown || riid == IID_IClassFactory)
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

	// QueryInterface probably for IID_IUnknown
	HRESULT hr = pInsideDCOM->QueryInterface(riid, ppv);
	pInsideDCOM->Release();
	return hr;
}

HRESULT CFactory::LockServer(BOOL bLock)
{
	if(bLock)
		g_cServerLocks++;
	else
		g_cServerLocks--;
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
	
	if(clsid != CLSID_InsideDCOM)
		return CLASS_E_CLASSNOTAVAILABLE;

	CFactory* pFactory = new CFactory;
	if(pFactory == NULL)
		return E_OUTOFMEMORY;

	// QueryInterface probably for IClassFactory
	HRESULT hr = pFactory->QueryInterface(riid, ppv);
	pFactory->Release();
	return hr;
}

HRESULT __stdcall DllRegisterServer()
{
	return RegisterServer("Component-with-Registration.dll", CLSID_InsideDCOM, "Inside DCOM Sample", "Component.InsideDCOM", "Component.InsideDCOM.1", NULL);
}

HRESULT __stdcall DllUnregisterServer()
{
	return UnregisterServer(CLSID_InsideDCOM, "Component.InsideDCOM", "Component.InsideDCOM.1");
}