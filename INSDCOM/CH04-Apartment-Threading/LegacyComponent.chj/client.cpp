//
// [2020-06-19] Chj applies some improvements to the sample program:
// 1. printf from different threads are serialized.
// 2. User can choose between STA or MTA for the main thread and worker thread.
// 3. If the parameter is a positive number(e.g. 3000), Sum() will executes
//	  for 3000 milliseconds(delay inside). 
// 4. User can try using unmarshaled(=bare) pointer in the worker thread, .
//	  Pass a *negative number* as parameter to activate this.
//
// Memo: For STA, you can see concurrent object calling behavior with this delay behavior.
//
// client.cpp
#define _WIN32_DCOM
#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <conio.h>

#include <utils.h>

#include "..\registry.h"
#include "Component\component.h" // Generated by MIDL

void PrintSum(ISum *pSum, int a1, int a2)
{
	pl("Client: Calling Sum(%d, %d)...", a1, a2);

	int sum = 0;
	HRESULT hr = pSum->Sum(a1, a2, &sum);

	if(SUCCEEDED(hr)) {
		pl("Client: Sum(%d, %d) returned %d", a1, a2, sum);
	}
	else {
		pl("Client: Sum(%d, %d) failed. HRESULT=0x%X", a1, a2, hr);
	}
}

bool MyCoInit(const char *prefix, bool isSTA)
{
	HRESULT hr = 0;
	const char *szCoInit = isSTA?"COINIT_APARTMENTTHREADED":"COINIT_MULTITHREADED";
	hr = CoInitializeEx(NULL, isSTA?COINIT_APARTMENTTHREADED:COINIT_MULTITHREADED);
	if(FAILED(hr)) {
		pl("%s CoInitialize(%s) failed, hr=0x%X", prefix, szCoInit, hr);
		return false;
	}
	else {
		pl("%s ----------- CoInitialize(%s) success.", prefix, szCoInit);
		return true;
	}
}

bool PumpMsg_and_WaitForSingleObject(HANDLE hobjWait)
{
	while (true) 
	{
		DWORD dwResult = MsgWaitForMultipleObjects(1, &hobjWait,
			FALSE, // wait either thread-done or MSG appears
			INFINITE, 
			QS_ALLINPUT);

		switch (dwResult) 
		{{
		case WAIT_OBJECT_0: // The event became signaled.
			return true;

		case WAIT_OBJECT_0 + 1: // A message is in our queue.
			// Dispatch all of the messages.
			MSG msg = {};
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
			{
				if (msg.message == WM_QUIT) {
					// A WM_QUIT message, exit the loop
					return false;
				} else {
					// Translate and dispatch the message.
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			} // Our queue is empty.
			break; // break out of `case`
		}}
	} // End of while loop
}

struct SThreadParam
{
	bool is_work_STA;
	HANDLE hEventObjReady; // Work thread sets this event, telling main thread COM object ready. 
	HANDLE hEventQuit;  // Main thread sets this event, telling work thread to quit.
	bool is_quit_early;
	IStream *pStream;   // set by worker, read by main
};

int MyThread(void *param)
{
	HRESULT hr = 0;
	DWORD mytid = GetCurrentThreadId();
	pl("Work thread starts running, WorkThread-tid=%d", mytid);

	SThreadParam *ptp = (SThreadParam*)param;
	IStream* pStream = ptp->pStream;

	if(!MyCoInit("Work thread", ptp->is_work_STA))
		return 4;

	//
	// Create COM object in worker thread. 
	//
	pl("Client: Work thread calling CoCreateInstance()...");
	ISum* pSum = NULL;
	hr = CoCreateInstance(CLSID_InsideDCOM, NULL, CLSCTX_INPROC_SERVER, IID_ISum, (void**)&pSum);
	if(FAILED(hr)) {
		pl("CoCreateInstance() failed, hr=0x%X", hr);
		return 4;
	}
	else {
		pl("CoCreateInstance() success, the interface-ptr returned is **0x%p**", pSum);
	}

	PrintSum(pSum, 1, 2);

	pl("Work thread: Calling CoMarshalInterThreadInterfaceInStream()...");
	CoMarshalInterThreadInterfaceInStream(IID_ISum, pSum, &(ptp->pStream));
	pl("Work thread:  Called CoMarshalInterThreadInterfaceInStream().");

	pl("Work thread: Calling pSum->Release()...");
	DWORD refcount = pSum->Release();
	pl("Work thread: Called pSum->Release(). reference count drops to %d", refcount);

	SetEvent(ptp->hEventObjReady);

	if(!ptp->is_quit_early)
	{
		pl("Work thread: Now wait for quit event...");
		bool is_quit = PumpMsg_and_WaitForSingleObject(ptp->hEventQuit);
		assert(is_quit);
		pl("Work thread: Got quit event.");
	}
	else
	{
		pl("Work thread: NOT waiting for quit event. Quit early!");
//		Sleep(500);
	}

	pl("Work thread: Calling CoUninitialize() and quit self.");
	CoUninitialize();
	return 0;
}

void MainThreadCallObject(bool is_coinit, bool isSTA, IStream *pStream, char quit_early)
{
	if(quit_early=='X')
	{
		pl("quit_early='%c', main-thread sleep 1 second.", quit_early);
		Sleep(1000);
	}

	HRESULT hr = 0;
	if(is_coinit)
	{
		if(!MyCoInit("Main thread", isSTA))
			return;
	}

	if(quit_early=='x')
	{
		pl("quit_early='%c', main-thread sleep 1 second.", quit_early);
		Sleep(1000);
	}

	ISum* pSum = NULL;
	pl("Main thread: Calling CoGetInterfaceAndReleaseStream()...");
	//
	hr = CoGetInterfaceAndReleaseStream(pStream, IID_ISum, (void**)&pSum);
	//
	if(SUCCEEDED(hr)) {
		pl("Main thread:  Called CoGetInterfaceAndReleaseStream(). Got marsptr: **0x%p**", pSum);
	}
	else {
		pl("Main thread CoGetInterfaceAndReleaseStream() fail! HRESULT=0x%X", hr);
		assert(pSum==NULL);
		return;
	}

	PrintSum(pSum, 20, 30);

	pl("Main thread: Calling pSum->Release()...");
	DWORD refcount = pSum->Release();
	pl("Main thread: Called pSum->Release(). reference count drops to %d", refcount);
}

bool AskForLateCoinit()
{
	printf("For the main thread, do you want to call CoInitialize() *after* the work thread has created COM object? (y/N) ");
	int key = _getch();
	if(key=='y' || key=='Y') {
		printf("y=AFTER\n");
		return true;
	}
	else {
		printf("N=BEFORE\n");
		return false;
	}
}

bool Choose_STA_MTA(const char *prefix)
{
	printf("%s-thread as STA or MTA? (1/0) ", prefix);
	int key = _getch();
	if(!(key=='0')) {
		printf("STA\n");
		return true;
	}
	else {
		printf("MTA\n");
		return false;
	}
}

int main(int argc, char *argv[])
{
	char quit_early = 0; // if true, let worker thread quit early
	if(argc==2)
	{
		char c = argv[1][0]; 
		if(c=='x' || c=='X')
			quit_early = c;
		else 
			quit_early = 0;
	}

	HRESULT hr = 0;
	bool is_late_coinit = AskForLateCoinit();

	bool is_main_STA = Choose_STA_MTA("Main");
	bool is_work_STA = Choose_STA_MTA("Work");

	pl_need_prefix("EXE", true, true, true);

	DWORD mytid = GetCurrentThreadId();
	pl("Client: MainThread-tid=%d", mytid);

	if(!is_late_coinit)
	{
		if(!MyCoInit("Main thread", is_main_STA))
			return 4;
	}

	//
	// Print COM objects apartment registry setting. 
	//
	char szregkey[1024] = {0}, szregvalue[1024] = {0}, szClsid[64] = {0};
	CLSIDtochar(CLSID_InsideDCOM, szClsid, sizeof(szClsid));
	_sntprintf_s(szregkey, sizeof(szregkey), _TRUNCATE, "CLSID\\%s\\InprocServer32", szClsid);
	pl("Reading from regkey [%s]", szregkey);
	bool succ = HKCR_GetValueSZ(szregkey, "ThreadingModel", szregvalue, sizeof(szregvalue));
	if(!succ || szregvalue[0]=='\0')
		pl("COM object registry-setting ThreadingModel Not-Exist (=Legacy Component)");
	else
		pl("COM object registry-setting ThreadingModel=%s", szregvalue);

	SThreadParam tp = {};
	tp.is_work_STA = is_work_STA;
	tp.hEventObjReady = CreateEvent(NULL, TRUE, FALSE, NULL);
	tp.hEventQuit = CreateEvent(NULL, TRUE, FALSE, NULL);
	tp.is_quit_early = quit_early ? true : false;
	HANDLE thread_handle = winCreateThread(MyThread, (void*)&tp);

	// Now the message loop, pump message and wait for thread-done simultaneously.
	BOOL fQuit = FALSE;
	bool isObjReady = false;
	while (!fQuit) 
	{
		DWORD dwResult = MsgWaitForMultipleObjects(1, 
			isObjReady ? &thread_handle : &tp.hEventObjReady, 
			FALSE, // wait either thread-done or MSG appears
			INFINITE, 
			QS_ALLINPUT);

		switch (dwResult) 
		{{
		case WAIT_OBJECT_0: // The event became signaled.

			if(!isObjReady)
			{
				// We know that tp.EventObjReady has just been ready.
				isObjReady = true;
				
				MainThreadCallObject(is_late_coinit, is_main_STA, tp.pStream, quit_early);
				SetEvent(tp.hEventQuit);
			}
			else
			{	// worker thread has ended
				fQuit = TRUE;
				break; // break out of `case`
			}

		case WAIT_OBJECT_0 + 1: // A message is in our queue.
			// Dispatch all of the messages.
			MSG msg = {};
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
			{
				if (msg.message == WM_QUIT) {
					// A WM_QUIT message, exit the loop
					fQuit = TRUE;
				} else {
					// Translate and dispatch the message.
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			} // Our queue is empty.
			break; // break out of `case`
		}}
	} // End of while loop

	hr = WaitForSingleObject(thread_handle, INFINITE);

	CloseHandle(thread_handle);
	CloseHandle(tp.hEventObjReady);
	CloseHandle(tp.hEventQuit);


	pl("Main thread: Calling CoUninitialize()");
	CoUninitialize();
	return 0;
}
