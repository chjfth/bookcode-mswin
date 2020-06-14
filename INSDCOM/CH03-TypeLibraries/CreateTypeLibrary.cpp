// This program creates a type library that exactly matches that produced by
// the MIDL compiler when fed the IDL shown here. The comments found to the
// right of each line of IDL code will direct to the code that creates that
// element of the type library.

// Use the OLE/COM Object Viewer to view the contents of the TLB file

/*
import "unknwn.idl";

[ object, uuid(10000001-0000-0000-0000-000000000001),		// (6)
  oleautomation ]											// (7)
interface ISum												// (5)
	: IUnknown												// (12)
{
	HRESULT Sum(int x, int y, [out, retval] int* retval);	// (13)
}

[ uuid(10000003-0000-0000-0000-000000000001),				// (1)
  helpstring("Inside DCOM Component Type Library"),			// (4)
  version(1.0) ]											// (3)
library Component											// (2)
{
	importlib("stdole32.tlb");								// (12)

	interface ISum;

	[ uuid(10000002-0000-0000-0000-000000000001) ]			// (9)
	coclass InsideDCOM										// (8)
	{
		[default]											// (11)
			interface ISum;									// (10)
	}
};
*/

#define _WIN32_DCOM
#include <windows.h>
#include <iostream.h>

void main()
{
	CoInitialize(NULL);

	// Create the type library file
	ICreateTypeLib2* pCreateTypeLib2;
	CreateTypeLib2(SYS_WIN32, L"C:\\MYLIB.TLB", &pCreateTypeLib2);

	// (1) Set the library LIBID to {10000003-0000-0000-0000-000000000001}
	GUID LIBID_Component = {0x10000003,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01}};
	pCreateTypeLib2->SetGuid(LIBID_Component);

	// (2) Set the library name to Component
	pCreateTypeLib2->SetName(L"Component");

	// (3) Set the library version to 1.0
	pCreateTypeLib2->SetVersion(1, 0);

	// (4) Set the helpstring to Inside DCOM Component Type Library
	pCreateTypeLib2->SetDocString(L"Inside DCOM Component Type Library");

	// (5) Create the ISum interface
	ICreateTypeInfo* pCreateTypeInfoInterface;
	pCreateTypeLib2->CreateTypeInfo(L"ISum", TKIND_INTERFACE, &pCreateTypeInfoInterface);

	// (6) Set the ISum IID to {10000001-0000-0000-0000-000000000001}
	IID IID_ISum = {0x10000001,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01}};
	pCreateTypeInfoInterface->SetGuid(IID_ISum);

	// (7) Set the oleautomation flag
	pCreateTypeInfoInterface->SetTypeFlags(TYPEFLAG_FOLEAUTOMATION);

	// (8) Create the coclass InsideDCOM
	ICreateTypeInfo* pCreateTypeInfoCoClass;
	pCreateTypeLib2->CreateTypeInfo(L"InsideDCOM", TKIND_COCLASS, &pCreateTypeInfoCoClass);

	// (9) Set the InsideDCOM CLSID to {10000002-0000-0000-0000-000000000001}
	CLSID CLSID_InsideDCOM = {0x10000002,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01}};
	pCreateTypeInfoCoClass->SetGuid(CLSID_InsideDCOM);

	// Specify that this coclass can be instantiated
	pCreateTypeInfoCoClass->SetTypeFlags(TYPEFLAG_FCANCREATE);

	// Get a pointer to the ITypeLib interface
    ITypeLib* pTypeLib;
	pCreateTypeLib2->QueryInterface(IID_ITypeLib, (void**)&pTypeLib);

	// Get a pointer to the ITypeInfo interface for ISum
    ITypeInfo* pTypeInfo;
    pTypeLib->GetTypeInfoOfGuid(IID_ISum, &pTypeInfo);

	// Trade in the ITypeInfo pointer for an HREFTYPE
	HREFTYPE hRefTypeISum;
	pCreateTypeInfoCoClass->AddRefTypeInfo(pTypeInfo, &hRefTypeISum);

	// (10) Insert the ISum interface into the InsideDCOM coclass
    pCreateTypeInfoCoClass->AddImplType(0, hRefTypeISum);

	// (11) Set ISum to be the default interface in coclass InsideDCOM
	pCreateTypeInfoCoClass->SetImplTypeFlags(0, IMPLTYPEFLAG_FDEFAULT);

	// Get a pointer to the ITypeLib interface for StdOLE
    ITypeLib* pTypeLibStdOle;
	GUID GUID_STDOLE = {0x00020430,0x00,0x00,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46};
    LoadRegTypeLib(GUID_STDOLE, STDOLE_MAJORVERNUM, STDOLE_MINORVERNUM, STDOLE_LCID, &pTypeLibStdOle);

	// Get a pointer to the ITypeInfo interface for IUnknown
    ITypeInfo* pTypeInfoUnknown;
    pTypeLibStdOle->GetTypeInfoOfGuid(IID_IUnknown, &pTypeInfoUnknown);

	// (12) Declare that ISum is derived from IUnknown
    HREFTYPE hRefType;
	pCreateTypeInfoInterface->AddRefTypeInfo(pTypeInfoUnknown, &hRefType);
	pCreateTypeInfoInterface->AddImplType(0, hRefType);

	// Structures for the x, y, and retval parameters of the Sum method
	TYPEDESC tdescParams = { 0 };
    tdescParams.vt = VT_INT;
	ELEMDESC myParams[3] = { 0 };
	myParams[0].tdesc.vt = VT_INT;
	myParams[0].tdesc.lptdesc = &tdescParams;
	myParams[1].tdesc.vt = VT_INT;
	myParams[1].tdesc.lptdesc = &tdescParams;
	myParams[2].tdesc.vt = VT_PTR;
	myParams[2].tdesc.lptdesc = &tdescParams;
	myParams[2].paramdesc.wParamFlags = PARAMFLAG_FRETVAL|PARAMFLAG_FOUT;

	// Additional data describing the Sum method and its return value
	TYPEDESC tdescUser = { 0 };
    FUNCDESC FuncDesc = { 0 };
	FuncDesc.funckind = FUNC_PUREVIRTUAL;
    FuncDesc.invkind = INVOKE_FUNC;
    FuncDesc.callconv = CC_STDCALL;
    FuncDesc.elemdescFunc.tdesc.vt = VT_HRESULT;
    FuncDesc.elemdescFunc.tdesc.lptdesc = &tdescUser;
	FuncDesc.cParams = 3;
	FuncDesc.lprgelemdescParam = myParams;

	// (13) Add the Sum method to the ISum interface
	pCreateTypeInfoInterface->AddFuncDesc(0, &FuncDesc);

	// Set names for the Sum function and its parameters
	OLECHAR* Names[4] = { L"Sum", L"x", L"y", L"retval" };
	pCreateTypeInfoInterface->SetFuncAndParamNames(0, Names, 4);

	// Assign the v-table layout
	pCreateTypeInfoInterface->LayOut();

	// Save changes to disk
	pCreateTypeLib2->SaveAllChanges();

	// Release all references
	pTypeInfoUnknown->Release();
	pTypeLibStdOle->Release();
    pTypeInfo->Release();
	pTypeLib->Release();
	pCreateTypeLib2->Release();
	pCreateTypeInfoInterface->Release();
	pCreateTypeInfoCoClass->Release();

	CoUninitialize();
	cout << "Type library created: C:\\MYLIB.TLB" << endl;
}