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
#include <stdio.h>
#include <assert.h>
#include <conio.h>

#include <utils.h>
#include "Component\component.h" // Generated by MIDL

struct SThreadParam
{ 
	IStream *pStream;
	ISum *pSum_main;
	bool is_sta;
};

int MyThread(void *param)
{
	DWORD mytid = GetCurrentThreadId();
	pl("Work thread starts running, WorkThread-tid=%d", mytid);

	SThreadParam *ptp = (SThreadParam*)param;
	IStream* pStream = ptp->pStream;

	DWORD coinit = ptp->is_sta ? COINIT_APARTMENTTHREADED : COINIT_MULTITHREADED;
	HRESULT hr = CoInitializeEx(NULL, coinit);
	if(FAILED(hr)) {
		pl("Work thread CoInitializeEx failed");
		return 4;
	}
	else {
		pl("Work thread CoInitializeEx(%s) success.",
			coinit==COINIT_APARTMENTTHREADED?"COINIT_APARTMENTTHREADED":"COINIT_MULTITHREADED");
	}

	ISum* pSum = NULL;
	if(ptp->pSum_main==NULL) {
		// The correct way to use cross-thread pointer.
		CoGetInterfaceAndReleaseStream(pStream, IID_ISum, (void**)&pSum);
	}
	else {
		pSum = ptp->pSum_main;
	}

	for(int count = 0; count < 5; count++)
	{
		int sum;
		pl("Client: Calling Sum(%d, %d)...", count, count);
		hr = pSum->Sum(count, count, &sum);
		if(SUCCEEDED(hr)) {
			pl("Client: Sum(%d, %d) returned %d", count, count, sum);
		}
		else {
			pl("Client: Sum(%d, %d) failed! HRESULT=0x%08X", count, count, hr);
		}

	}

	pSum->Release();
	CoUninitialize();
	return 0;
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
	bool is_try_bareptr = false;
	int obj_delay_millisec = 0;
	if(argc==2) {
		obj_delay_millisec = strtoul(argv[1], NULL, 0);
		
		if(obj_delay_millisec<0) {
			is_try_bareptr = true;
			obj_delay_millisec = -obj_delay_millisec;
		}
	}

	bool is_main_STA = Choose_STA_MTA("Main");
	bool is_work_STA = Choose_STA_MTA("Work");

	pl_need_prefix("EXE", true, true, true);

	DWORD mytid = GetCurrentThreadId();
	pl("Client: Calling CoInitialize(). MainThread-tid=%d", mytid);

	DWORD coinit = is_main_STA ? COINIT_APARTMENTTHREADED : COINIT_MULTITHREADED;
	HRESULT hr = CoInitializeEx(NULL, coinit);
	if(FAILED(hr)) {
		pl("Main thread CoInitializeEx failed, hr=0x%X", hr);
		return 4;
	}
	else {
		pl("Main thread CoInitializeEx(%s) success.",
			coinit==COINIT_APARTMENTTHREADED?"COINIT_APARTMENTTHREADED":"COINIT_MULTITHREADED");
	}

	pl("Client: Calling CoCreateInstance()");
	ISum* pSum;
	hr = CoCreateInstance(CLSID_InsideDCOM, NULL, CLSCTX_INPROC_SERVER, IID_ISum, (void**)&pSum);
	if(FAILED(hr)) {
		pl("CoCreateInstance() failed, hr=0x%X", hr);
		return 4;
	}
	else {
		pl("CoCreateInstance() success, the interface-ptr returned is **0x%p**", pSum);
	}

	if(obj_delay_millisec>0) {
		int old_delay = 0;
//		pSum->SetDelay(2, &old_delay); // just a test
		pSum->SetDelay(obj_delay_millisec, &old_delay); // cannot use NULL as 2nd param
//		assert(old_delay==2);
	}

	IStream* pStream;
	CoMarshalInterThreadInterfaceInStream(IID_ISum, pSum, &pStream);

	SThreadParam tp = {};
	tp.pStream = pStream;
	tp.is_sta = is_work_STA;
	if(is_try_bareptr) {
		tp.pSum_main = pSum;
		pSum->AddRef(); 
	}
	
	HANDLE thread_handle = winCreateThread(MyThread, (void*)&tp);

	int sum;
	hr = pSum->Sum(2, 3, &sum);
	if(SUCCEEDED(hr)) {
		// cout << "Client: Calling Sum(2, 3) = " << sum << endl;
		pl("Client: Sum(2, 3) returned %d", sum);
	}
	else {
		pl("Client: Sum(2, 3) failed. HRESULT=0x%X", hr);
	}

	DWORD refcount = pSum->Release();
	pl("Client: Calling pSum->Release(). reference count drops to %d", refcount);

	// Now the message loop, pump message and wait for thread-done simultaneously.
	BOOL fQuit = FALSE;
	while (!fQuit) 
	{
		DWORD dwResult = MsgWaitForMultipleObjects(1, &thread_handle, 
			FALSE, // wait either thread-done or MSG appears
			INFINITE, 
			QS_ALLEVENTS);

		switch (dwResult) 
		{{
		case WAIT_OBJECT_0: // The event became signaled.
			fQuit = TRUE;
			break; // break out of `case`
		case WAIT_OBJECT_0 + 1: // A message is in our queue.
			// Dispatch all of the messages.
			MSG msg;
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

	pl("Client: Calling CoUninitialize()");
	CoUninitialize();
	return 0;
}
