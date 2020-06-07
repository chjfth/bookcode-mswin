
===============================================================================
NOTE:  Build these projects in the order shown below.
===============================================================================



===============================================================================
IDL
===============================================================================
   This directory contains the interface definition file for the interfaces
   that are used within the OCR examples throughout this book.

   You need to compile this IDL file using the midl.exe compiler, as follows:

      midl /Oicf ocr.idl

   to obtain the interface marshaling code for all interfaces defined within
   the ocr.idl file.  To wrap up all this code into a DLL, you need to use
   nmake.exe, as follows:

      nmake -f ocrps.mk

   Once you have OCRps.dll, you must register it as follows:

      regsrv32.exe ocrps.dll

   This is required for remote method invocation on custom interfaces.
   
   ----------------------------------------------------------------------------
   [2020-06-07] Chj:
   Using Visual Studio 2010 IDE, just open OCREngine.sln, F7 - Build the 
   "IDL" project, and you are done, convenient! MIDL.EXE is automatically 
   called and you can freely switch between 4 convigs(Debug/Release, Win32/x64
   combination) without stomping on each other's output files.
   ----------------------------------------------------------------------------
   

===============================================================================
Server  (out-of-proc server)
===============================================================================
   This directory contains the code for an out-of-process server that 
   supports an object that implements the interfaces defined within ocr.idl.

   To register this EXE component use the -RegServer option, as follows:

      ocrsvr.exe -RegServer

   Use -UnRegServer to unregister.
   
===============================================================================
Inproc  (in-proc server)
===============================================================================
   This directory contains the code for an in-process server that 
   supports an object that implements the interfaces defined within ocr.idl.

   Use regsvr32.exe to register this DLL component:

      regsvr32.exe inproc.dll

   To unregister, use the following:

      regsvr32.exe -u inproc.dll
   
===============================================================================
Client (client component to test the in- and out-of-proc server.
===============================================================================
   This directory contains the code for client component that used the IOcr
   interface specified in ocr.idl.  The ocrsvr.exe or inproc.exe components must
   be registered in order to successfully run this client component.  In 
   addition, the ocrps.dll must be registered.

   You need to put the "test.tif" file in the same directory as ocrcli.exe,
   because ocrcli.exe loads this file and send the binary TIFF image data
   to the server component.

===============================================================================
Reuse 
===============================================================================
   This directory contains the projects that illustrates aggregation and
   containment.  See the readme.txt in this subdirectory for more details.
   
