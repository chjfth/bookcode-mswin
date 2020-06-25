// local.cpp
#define _WIN32_DCOM
#include <assert.h>
#include <iostream>
using namespace std;
#include "Component\component.h"
#include "registry.h"

HANDLE g_hEvent;

class CInsideDCOM : public ISum
{
public:
	// IUnknown
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall QueryInterface(REFIID riid, void** ppv);

	// ISum
	HRESULT __stdcall Sum(int x, int y, int *retval);

	CInsideDCOM();
	~CInsideDCOM();

private:
	long m_cRef;
};

void InitiateComponentShutdown()
{
	cout << "InitiateComponentShutdown()" << endl;
	SetEvent(g_hEvent);
}

CInsideDCOM::CInsideDCOM() : m_cRef(1)
{
	CoAddRefServerProcess();
}

CInsideDCOM::~CInsideDCOM()
{
	cout << "Component: CInsideDCOM::~CInsideDCOM()" << endl;
	if(CoReleaseServerProcess() == 0)
		InitiateComponentShutdown();
}

ULONG CInsideDCOM::AddRef()
{
	cout << "Component: CInsideDCOM::AddRef() m_cRef = " << m_cRef + 1 << endl;
	return InterlockedIncrement(&m_cRef);
}

ULONG CInsideDCOM::Release()
{
	cout << "Component: CInsideDCOM::Release() m_cRef = " << m_cRef - 1 << endl;
	unsigned cRef = InterlockedDecrement(&m_cRef);
	if(cRef != 0)
		return cRef;
	delete this;
	return 0;
}

HRESULT CInsideDCOM::QueryInterface(REFIID riid, void **ppv)
{
	if(riid == IID_IUnknown)
	{
		cout << "Component: CInsideDCOM::QueryInterface() for IUnknown" << endl;
		*ppv = (IUnknown*)this;
	}
	else if(riid == IID_ISum)
	{
		cout << "Component: CInsideDCOM::QueryInterface() for ISum" << endl;
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

HRESULT CInsideDCOM::Sum(int x, int y, int *retval)
{
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
	~CFactory() { cout << "Component: CFactory::~CFactory()" << endl; }

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
	unsigned cRef = InterlockedDecrement(&m_cRef);
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

HRESULT CFactory::CreateInstance(IUnknown *pUnknownOuter, REFIID riid, void **ppv)
{
	if(pUnknownOuter != NULL)
		return CLASS_E_NOAGGREGATION;

	CInsideDCOM *pInsideDCOM = new CInsideDCOM;
	cout << "Component: CFactory::CreateInstance() " << pInsideDCOM << endl;

	if(pInsideDCOM == NULL)
		return E_OUTOFMEMORY;

	HRESULT hr = pInsideDCOM->QueryInterface(riid, ppv);
	pInsideDCOM->Release();
	return hr;
}

HRESULT CFactory::LockServer(BOOL bLock)
{
	if(bLock)
		CoAddRefServerProcess();
	else
		if(CoReleaseServerProcess() == 0)
			InitiateComponentShutdown();
	return S_OK;
}

void RegisterComponent()
{
	ITypeLib* pTypeLib;
	HRESULT hr = LoadTypeLibEx(L"component.exe", REGKIND_DEFAULT, &pTypeLib);
	pTypeLib->Release();

	RegisterServer("component.exe", CLSID_InsideDCOM, 
		"Inside DCOM Sample", 
		"Component.InsideDCOM", 
		"Component.InsideDCOM.1", 
		NULL);
}

void CommandLineParameters(int argc, char** argv)
{
	RegisterComponent();
	if(argc < 2)
	{
		cout << "No parameter, but registered anyway..." << endl;
		exit(false);
	}
	char* szToken = strtok(argv[1], "-/"); 
	if(_stricmp(szToken, "RegServer") == 0)
	{
		RegisterComponent();
		cout << "RegServer" << endl;
		exit(true);
	}
	if(_stricmp(szToken, "UnregServer") == 0)
	{
		UnRegisterTypeLib(LIBID_Component, 1, 0, LANG_NEUTRAL, SYS_WIN32);
		UnregisterServer(CLSID_InsideDCOM, "Component.InsideDCOM", "Component.InsideDCOM.1");
		cout << "UnregServer" << endl;
		exit(true);
	}
	if(_stricmp(szToken, "Embedding") != 0)
	{
		cout << "Invalid parameter" << endl;
		exit(false);
	}
}

void main(int argc, char** argv)
{
	CommandLineParameters(argc, argv);

	CoInitializeEx(NULL, COINIT_MULTITHREADED); // make this thread MTA
	g_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	DWORD dwRegister;
	IClassFactory *pIFactory = new CFactory();
	CoRegisterClassObject(CLSID_InsideDCOM, pIFactory, CLSCTX_LOCAL_SERVER, REGCLS_SUSPENDED|REGCLS_MULTIPLEUSE, &dwRegister);
	CoResumeClassObjects();

	WaitForSingleObject(g_hEvent, INFINITE);
	CloseHandle(g_hEvent);

	CoRevokeClassObject(dwRegister);
	pIFactory->Release();
	CoUninitialize();
}