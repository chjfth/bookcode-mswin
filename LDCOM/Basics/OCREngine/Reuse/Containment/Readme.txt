
===============================================================================
NOTE:  Build these projects in the order shown below.
===============================================================================

===============================================================================
IDL
===============================================================================
   This is the IDL file that supports the IDictionary interface, an interface
   that is implemented by an outer object.  The outer object (CoDictionary)
   reuses the inner object, which supports the ISpell interface, through
   containment.

   You need to build and register this proxy/stub DLL (containps.dll) prior to
   using the client.exe component within this directory.

   Given what we've just said, you have to do the following:

   regsvr32.exe ocrps.dll
   regsvr32.exe containps.dll  

===============================================================================
Contain
===============================================================================
   This directory contains the code for an out-of-process (EXE) component that
   reuses the CoOcrEngine object that is implemented within the inner.dll
   component through containment.  Build and register this component 
   using the -RegServer option:

   contain.exe -regserver

===============================================================================
Client
===============================================================================
   This directory contains the code for a client component that makes use 
   of the CoDictionary object implemented by the contain.exe component.


