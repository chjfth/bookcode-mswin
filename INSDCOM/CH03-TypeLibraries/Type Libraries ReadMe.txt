---------------------------------
Type Libraries
---------------------------------
See Chapter 3 "Type Libraries and Language Integration"
for information on type libraries and building clients
and components in C++, Visual Basic, and Java



Minimum System Requirements
===========================
Windows 95 with DCOM installed or Windows NT 4.0
Visual C++ 5.0 Professional
Visual Basic 5.0 Professional (Service Pack 2 or higher)
Visual J++ 1.1



Creating a Type Library sample
==============================
1.  Open "Type Libraries.dsw" in Visual C++
2.  Review comments in "Create Type Library.cpp"
3.  Build CreateTypeLib.exe
4.  Run CreateTypeLib.exe to create C:\MYLIB.TLB
5.  Use the OLE/COM Object Viewer utility (oleview.exe) to view 
    the contents of C:\MYLIB.TLB


C++ Clients That Utilize the Type Library sample
================================================
C++ Component, Easy C++ Client
1.  Open "Type Libraries.dsw" in Visual C++
2.  Build Client.exe
    (this also builds Component.dll)
3.  Register Component.dll
4.  Run Client.exe to test

C++ Component, Smart Pointer Client
1.  Open "Type Libraries.dsw" in Visual C++
2.  Build SmartPointerClient.exe
    (this also builds Component.dll)
3.  Register Component.dll
4.  Run SmartPointerClient.exe to test


C++ Component, Visual Basic Client sample
=========================================
1.  If you haven't already, build and register the Component.dll in
    "C++ Clients That Utilize the Type Library" above.
2.  Open vbclient.vbp in Visual Basic
3.  Choose Project/References and ensure that a proper reference is
    set for "Inside DCOM Component Type Libary".  If not, add the
    reference by browsing for 
    "Type Libraries\Component\component.tlb".
4.  Run program and press Command1 button to test


Visual Basic Component, C++ Client sample
=========================================
--------------------------------------
NOTE: Threading Models in Visual Basic
--------------------------------------
In order to specify a Threading Model for components created in
Visual Basic 5.0 as shown in section "Building a Component in Visual
Basic" (page 111) of Chapter 3, you must have at least Service Pack 2
of Visual Basic installed.
See section "WHERE TO GET SOFTWARE FOR SAMPLES NOT INCLUDED ON CD"
in the ReadMe.txt file at root of the CD for information on how to
obtain this Service Pack and other software.

1.  Open VBComponent.vbp in Visual Basic
    (If a dialog box is displayed about being unable to set the
    version compatible component, just click OK)
2.  Choose Project/References and ensure that a proper reference is
    set for "Inside DCOM Component Type Libary".  If not, add the
    reference by browsing for the type library, 
    "Type Libraries\Component\component.tlb", which gets created when
    Component.dll is built for "C++ Client That Utilize the Type
    Library" above.
3.  Make VBComponent.dll and save in "Type Libraries" directory
4.  Open VBnJava.dsw in Visual C++
5.  Build VBClient.exe
6.  Run VBClient.exe to test


C++ Component, Java Client sample
=================================
1.  If you haven't already, build and register the Component.dll
    in "C++ Clients That Utilize the Type Library" above.
2.  Open VBnJava.dsw in Visual Studio (Visual C++/Visual J++)
3.  Set TestCOM as the Active Project
4.  Choose Project/Settings, and select the Debug tab
5.  In the Class For Debugging/Executing text box, type "TestCOM"
    without quotes
6.  In the Debug/Execute Project Under section, select the 
    Stand-Alone Interpreter (Applications Only) option and click OK.
7.  Choose Tools/Java Type Library Wizard
8.  Check the Inside DCOM Component Type Library check box, and
    click OK.
9.  Build TestCOM
10. Run TestCOM to test


Java Component, C++ Client sample
=================================
1.  Open VBnJava.dsw in Visual Studio (Visual C++/Visual J++)
2.  Set JavaSum as the Active Project
3.  In SumClass.java, comment out the import and implements
    statements as shown here:
//import sumclasslib.*;
//
//
// SumClass
//
//
public class SumClass //implements sumclasslib.ISumClass
{
    private static final String CLSID = 
        "5a870200-7281-11d1-a6fc-0000c0cc7be1";

    public int Sum(int x, int y)
    {
        return x + y;
    }
}
4.  Build JavaSum
5.  Choose Tools/ActiveX Wizard For Java
6.  In the What Java Class File Do You Want To Convert Into A COM-
    Callable ActiveX Component text box enter the location for
    SumClass, which should be in "Type Libraries\JavaSum\SumClass.class"
7.  Click Next
8.  Click Next
9.  In the What Type Of Interface Would You Like To Create option,
    select Dual Interface
10. Click Finish
    NOTE: The following J0182 error may be displayed:
        "command line error MIDL1004 : cannot execute C preprocessor
           C:\PROGRA~1\DEVSTU~1\VJ\BIN\CL_VJ.EXE
        error J0182: Cannot create TLB file C:\InsideDCOM\
          Type Libraries\JavaSum\SumClasslib.tlb
        Tool returned code: 1"
    If this error is displayed, try copying
    C:\Program Files\DevStudio\SharedIDE\bin\Mspdb41.dll into the
    C:\Program Files\DevStudio\VJ\bin\ directory and retesting the
    ActiveX Wizard for Java.
11. Click OK
12. Choose Project/Settings, and select the General tab
13. In the left pane, select the JavaSum project
14. In the Output Directory text box, type "C:\Windows\Java\Lib"
    without quotes. If your Windows directory has a different name,
    be sure to use that name.
15. Click OK to close the Project Settings dialog box
16. In SumClass.java, uncomment the import and implements statements
    as shown here:
import sumclasslib.*;
//
//
// SumClass
//
//
public class SumClass implements sumclasslib.ISumClass
{
    private static final String CLSID = 
        "5a870200-7281-11d1-a6fc-0000c0cc7be1";

    public int Sum(int x, int y)
    {
        return x + y;
    }
}
17. Choose Build/Rebuild All
18. Set JavaClient as Active Project
19. Build JavaClient.exe
20. Run JavaClient.exe to test



<end>