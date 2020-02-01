[2020-02-01]
Verified with VC6 SP6 + MsSDK Feb 2003.
Just type 

	nmake -f MAKE-ONE
	nmake -f MAKE-ONE OUTPROC=1

to build all output files and load them in VS2010 debugger to enjoy source-level debugging. 

You will get output files:
	Server.tlb
	Server.exe
	Server.dll
	AClient.exe
	DClient.exe

Note 1: On Win7+, when doing nmake, `regsvr32 server.dll` and `server.exe /RegServer` will not succeed due to access denied writing to HKCR\CLSID regkey.
That is expected behavior, not true building failure. So doing them manually in a Administrator CMD console solves the issue.

Note 2: On Win7+, You have to run AClient or DClient in Administrator CMD once, choosing "In-proc server", so that HKCR\TypeLib can be written with success.


