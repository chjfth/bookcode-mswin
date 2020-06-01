
===============================================================================
NOTE:  Build these projects in the order shown below.
===============================================================================

===============================================================================
Inner (A component that implements an inner, reusable object.)
===============================================================================
   This component uses the [..\..\IDL\ocr.idl] file, so you need to build and
   register the OCRps.dll in that directory in order to successfully test
   out containment and aggregation.

   The component object implemented in this component can be aggregated.  
   All objects can be reused via containment, but only object that 
   support aggregation can be aggregated.

   Build this component and use regsvr32.exe to register it, prior to building
   the projects under the Aggregate and Contain subdirectories.

   The registry entries for this component will overwrite the onces for
   the inproc.dll component.  These two components are essentially the same;
   the only difference is that this component (inner.dll) 
   supports aggregation.

===============================================================================
Containment
===============================================================================
   This directory contains the code that shows reuse using the CONTAINMENT
   technique.  See the readme.txt file under this directory for more 
   information.

===============================================================================
Aggregation
===============================================================================
   This directory contains the code that shows reuse using the AGGREGATION
   technique.  See the readme.txt file under this directory for more 
   information.
