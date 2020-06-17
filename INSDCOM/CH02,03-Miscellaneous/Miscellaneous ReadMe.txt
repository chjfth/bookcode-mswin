---------------------------------
Miscellaneous
---------------------------------
See the "Registration Using Context Menus" sidebar in Chapter 2
"IUnknown and IClassFactory", page 71, for more information on
"registration context menu.reg"

See the "RegTlb: A Utility for Registering Stand-Alone Type
Libaries" sidebar in Chapter 3, pages 86-87, for more information
on the RegTlb utility

See the "C++ Templates (A Quick Introduction)" section in Chapter
3, starting on page 89, for more information on C++ Templates



Minimum System Requirements
===========================
Windows 95 or Windows NT 4.0
Visual C++ 5.0 Professional



Registration Using Context Menus
================================
"registration context menu.reg" is a registry script that
provides context menus that let you register and unregister
components. To add these context menus, perform the following
steps:
1.  In Windows Explorer double-click the "registration context
    menu.reg" icon.
    Note: If you receive errors about being unable to import file,
    it may be due to spaces in the filename and possibly the path.
    In this case, remove the spaces in the filename "registration
    context menu.reg", be sure that the path to this file does not
    contain any spaces, and retest.
2.  Right-click on a DLL or EXE
    "Register Component" and "Unregister Component" should be
    displayed in the context menu.


RegTlb: A Utility for Registering Stand-Alone Type Libaries
===========================================================
1.  Open Misc.dsw in Visual C++
2.  Build regtlb.exe
3.  From a command prompt register a .tlb file using regtlb.exe.
    Sample usage:
        regtlb tlbfile.tlb
    Note: You can create a .tlb file by following the "Creating a
    Type Library" sample in the "Type Libraries" directory
4.  Use the Registry Editor (regedit.exe) to ensure that the type
    library was registered


C++ Templates
=============
1.  Open Misc.dsw in Visual C++
2.  Build Template.exe
3.  Run Template.exe to test



<end>