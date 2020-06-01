===============================================================================
GenerateGUID          
===============================================================================
   This directory contains the code to dynamically generate a GUID.

===============================================================================
Dispatch
===============================================================================
   This directory contains simple code to illustrate dynamic invocation.
   It simply shows MS Word, if this application is installed and correctly
   registered on your machine.

===============================================================================
ClassMoniker
===============================================================================
   This directory contains the code that shows an indirect way to 
   activate an object.  You need to know about the "lastest and greatest" 
   OCR factory by name, not by a ProgID or a CLSID.

   This project requires that the OCREngine class factory be registered.  In
   addition, you need to register OCRps.dll to marshal the required interfaces.

===============================================================================
Persistence
===============================================================================
   This directory contains the code that demonstrates creating and 
   intializing an object from a persistence file.  Any object that supports 
   the IPersistFile interface knows how to reload itself from a persistent 
   file.  The key to this program is the CoGetInstanceFromFile API function.

   If you have a word document and would like to see its contents, you can use
   the this program (persist.exe) program.  Word must be installed and registered
   on the local system.  This is the same for other persistent files; for an excel
   file, Excel must be installed and registered.

===============================================================================
OCREngine
===============================================================================
   This directory contains the code for the OCR examples that are used 
   throughout the book.  See a more detailed readme file under this directory
   for more details.
