___________________________________________________________________

                      Companion CD README
                               for

                     Inside Distributed COM


                  by Guy Eddon and Henry Eddon

          Copyright (c) 1998 by Guy Eddon and Henry Eddon
                       All Rights Reserved
___________________________________________________________________



SHORT SHORT VERSION OF README
=============================
HOW TO USE THIS CD
 - Open INDCOM.HTM
WHAT'S ON THIS CD?
 - Code Samples (SAMPLES)
 - Setup Program (SETUP)
     Run SETUP.EXE to install
     Use Add/Remove Programs to uninstall
 - DCOM for Windows 95 version 1.1 (DCOM95)
 - DCOM Configuration Utility version 1.1 (DCOM95)
 - Microsoft SDK for Java version 2.01 (JVASDK20)
 - Microsoft Internet Explorer 4.01 (IE401)
 - The COM Specification (COMSPEC)
 - Distributed COM Protocol -- DCOM/1.0 (DCOMSPEC)
 - DCOM Technical Overview (DCOMTEC)
 - Chapter 5 Addendum (INDCOM.HTM)
TESTING THE CODE SAMPLES
 - Read the README.TXT in each SAMPLES subdirectory
 - Some samples have specific software requirements, but most
   samples can be tested with the following minimum configuration:
     Windows 95 with DCOM installed or Windows NT 4.0
     Microsoft Visual C++ 5.0
 - To build the C++ and Java samples, open the .DSW file in Visual
   Studio and go for it
TROUBLESHOOTING CODE SAMPLES
 - See troubleshooting steps below
WHERE TO GET SOFTWARE FOR SAMPLES NOT INCLUDED ON CD
 - Microsoft Platform SDK
     http://www.microsoft.com/msdn/sdk/
 - Visual Basic Service Pack
     http://www.microsoft.com/vstudio/sp/
 - Microsoft Transaction Server 2.0
     http://www.microsoft.com/transaction/default.asp
TROUBLES VIEWING LONG FILE NAMES ON CD
 - Look in SETUP directory and run SETUP.EXE
SUPPORT INFORMATION
 - http://mspress.microsoft.com/support/support.htm
 - MSPINPUT@MICROSOFT.COM



HOW TO USE THIS CD
==================
To use this CD, insert the disc in your CD-ROM drive and the CD
home page should automatically be displayed in your web browser.
If the page does not display, open the file INDCOM.HTM in your
browser. If you do not have a browser, the IE401 directory on this
CD contains an installer for Microsoft Internet Explorer 4.01.



WHAT'S ON THIS CD?
==================
Code Samples 
------------
The SAMPLES directory on this CD contains C++, Visual Basic, and
Java samples that demonstrate all of the topics discussed in the
book.  The samples are arranged by topic rather than by chapter.
You can just copy individual samples from the SAMPLES directory to
your hard disk to test, but be aware that you will have to clear
the Read-Only flag for many of the files in order to compile and
test. To avoid having to clear the Read-Only flag, run the
setup program (SETUP.EXE) in the SETUP directory.
 
Setup Program
-------------
Installing Files
The SETUP directory on this CD contains a short file name (8.3)
version of the files in the SAMPLES, COMPSPEC, and DCOMSPEC
directories. It also contains a setup program (SETUP.EXE) that can
copy these files to your hard disk, rename the files using the long
file names, and clear the Read-Only flag on the files. The files
get copied to your hard disk in the directory X:\InsideDCOM, where
X is the drive you specify in the installation.

Uninstalling Files
To Uninstall the files from your hard disk follow these steps:
1.  Click Start/Settings/Control Panel
2.  Open Add/Remove Programs
3.  Click the item you want to uninstall, which could be:
    "Inside DCOM Code Samples"
    "COM Specification"
    "DCOM Specification"
4.  Click the Add/Remove button

DCOM for Windows 95 version 1.1
DCOM Configuration Utility version 1.1
--------------------------------------
The DCOM95 directory on this CD contains an installer for DCOM for
Windows 95 (DCOM95.EXE) and an installer for the DCOM Configuration
utility (DCM95DFG.EXE). For up to date information on DCOM, consult
the COM web site:
    http://www.microsoft.com/com/

Microsoft SDK for Java version 2.01
-----------------------------------
The JVASDK20 directory on this CD contains the US English Version of
Microsoft SDK for Java 2.01 for Windows 95 & NT 4.0.
Specifically, it contains the following components:
    Microsoft SDK for Java version 2.01 (SDK-JAVA.EXE)
    Microsoft virtual machine for Java (MSJAVX86.EXE)
    SDK for Java Documentation (SDKJDOC.EXE)
    Java Base Package Documentation (JAVADOCS.EXE)
    DirectX(tm) 3a Runtime (DIRECTX.EXE)
    DirectAnimation Runtime (DXMEDEX.EXE)
    DirectAnimation Classes (DAJAVCLS.EXE)
    DirectAnimation Tutorial and Samples (DXMA.EXE)
    Microsoft virtual machine for Java for Dec Alpha (MSJAVAXP.EXE)
For up to date information on the Microsoft SDK for Java, consult
Microsoft's Java web site:
    http://www.microsoft.com/java/

Microsoft Internet Explorer 4.01
--------------------------------
The IE401 directory on this CD contains the US English Version of
Microsoft Internet Explorer 4.01 for Windows 95 & NT 4.0
(IE4SETUP.EXE). For up to date information on Internet Explorer,
consult Microsoft's Internet Explorer web site:
    http://www.microsoft.com/ie/

The COM Specification
---------------------
The COMSPEC directory on this CD contains the COM Specification in
Microsoft Word format. For up to date information on COM, consult
the COM web site:
    http://www.microsoft.com/com/

Distributed COM Protocol -- DCOM/1.0
------------------------------------
The DCOMSPEC directory on this CD contains a text file of the
Distributed Component Object Model Protocol -- DCOM/1.0 (January
1998) (draft-brown-dcom-v1-spec-02.txt). For up to date information
on COM and DCOM, consult the COM web site:
    http://www.microsoft.com/com/

DCOM Technical Overview 
-----------------------
The DCOMTEC directory on this CD contains a DCOM Technical Overview
by Microsoft Corporation in Microsoft Word format (DCOMTEC.DOC).

Chapter 5 Addendum
------------------
INDCOM.HTM contains a "Chapter 5 Addendum" link that contains
material to supplement Chapter 5. This material includes
information on the BSTR Automation type and a table of COM helper
functions for working with the VARIANT type.



TESTING THE CODE SAMPLES
========================
The SAMPLES directory on this CD contains C++, Visual Basic, and
Java samples that demonstrate all of the topics discussed in the
book.  The samples are arranged by topic rather than by chapter.
Within the main topic directory is a README.TXT file that contains
brief instructions on how to build and run the samples.

Configuration
-------------
Various code samples will have different requirements as specified
in the main topic README file. Many of the code samples only
require the following configuration:
    Microsoft Windows 95 with DCOM installed or
        Microsoft Windows NT 4.0
    Microsoft Visual C++ 5.0
The configuration that the code samples were primarily tested on
is as follows:
    Microsoft Windows 95 with DCOM installed or
        Microsoft Windows NT 4.0 Service Pack 3 (SP3)
    Microsoft Visual C++ 5.0 Professional SP3 (typical install)
    Microsoft Visual Basic 5.0 Professional SP3 (typical install)
    Microsoft Visual J++ 1.1 (typical install)
    Microsoft Windows Platform SDK (January 1998)
Some code samples also require the following:
    Microsoft Transaction Server 1.0/2.0
    Microsoft Internet Explorer 4.0 (available on this CD)
    Windows NT 5.0 beta 1 or later
    Microsoft SDK for Java version 2.01 (available on this CD)
    Microsoft virtual machine for Java (available on this CD)
    Microsoft Word

Building the C++ and Java Samples
---------------------------------
All of the C++ projects and Java projects have been set up as
workspaces to be used with Visual Studio.  You typically open the
.DSW file and build the appropriate projects.  When you build the
Component projects, the MIDL compiler is automatically called, so
it is not necessary to run the MIDL compiler separately.  Also,
many of the projects contain dependencies.  For example, if you
build the Client project, which is dependent upon files generated
by the Component build, the Component project is automatically
built first and then the Client project is built.



TROUBLESHOOTING CODE SAMPLES
============================
If you have troubles getting the samples to build and run, try some
of the following troubleshooting steps:
1.  Consult the README file in each sample directory to review
    steps on how to build and run the sample.
2.  Consult the README file in each sample to verify if you have
    the proper software installed (i.e. DCOM for Windows 95,
    Service Packs, Platform SDK, etc.).
    See "WHERE TO GET SOFTWARE FOR SAMPLES NOT INCLUDED ON CD"
    below for more information.
3.  If a sample requires the Platform SDK, be sure that the path to
    the SDK files is listed before the other compiler paths in
    Visual C++ 5.0. To check this out, in Visual C++, choose 
    Tools/Options and click the Directories tab. For a default
    install of the Platform SDK, the following paths should be
    listed first in Executable Files, Include Files, and Library
    Files:
        C:\MSSDK\bin
        C:\MSSDK\include
        C:\MSSDK\lib
4.  Make sure you have the proper EXEs and DLLs registered.
    You typically register an EXE by running the EXE.
    You typically register a DLL by using the REGSVR32.EXE utility
    and using the following syntax:
        REGSVR32 Component.dll
    You can also register a DLL within Visual C++ 5.0 by making it
    the active project and choosing Tools/Register Control.
5.  Registry entries from the configuration of previous samples may
    conflict with the current sample being tested. If you suspect
    that this is the case you can delete these entries from the
    registry using the Registry Editor (REGEDIT.EXE).
    Remember, it is always a good idea to back up your Registery
    before you make changes to it.    
    The Inside DCOM samples typically have a subkey with the
    following format:
        {1?000???-0000-0000-0000-0000000000??}
    Some sample keys entered into the registry by the Inside
    DCOM samples are shown here:
        HKEY_CLASSES_ROOT\AppID\{10000002-0000-0000-0000-000000000001}
        HKEY_CLASSES_ROOT\CLSID\{10000002-0000-0000-0000-000000000001}
        HKEY_CLASSES_ROOT\Component.InsideDCOM
        HKEY_CLASSES_ROOT\Component.Prime
        HKEY_CLASSES_ROOT\Interface\{10000001-0000-0000-0000-000000000001}
        HKEY_CLASSES_ROOT\TypeLib\{10000003-0000-0000-0000-000000000001}
6.  If you have an HRESULT error value, use the HRESULT Error
    Utility or WINERROR.H to determine what the error value means.
    The HRESULT Error Utility source files are located in the
    HRESULT directory on this CD.  WINERROR.H is installed with
    Visual C++ and by default is located in the directory
    C:\Program Files\DevStudio\VC\include
7.  If you are testing a sample that runs remotely, be sure that it
    will run properly locally first.
8.  If you are testing a sample that runs remotely, you may need to
    adjust Location, Security, Identity settings with the DCOM
    Configuration utility (DCOMCNFG.EXE).
9.  If you are testing a sample that runs remotely, be aware that
    Windows 95/98 does not support remote instantiation, so the
    remote computer may need to be running Windows NT 4.0 or higher.
10. When the README instructions say to use the DCOM Configuration
    utility to specify that the sample should run on a remote
    computer, you should follow these steps:
        a.  Run DCOMCNFG.EXE
        b.  Double click the registered executable component
            (for most samples, it is "Inside DCOM Sample")
        c.  Click the Location tab
        d.  Make sure the Run Application On This Computer checkbox
            is unchecked
        e.  Check Run Application On The Following Computer
        f.  Type the computer name in the text box and click OK
11. If you suspect security is an issue when testing a remote
    sample, use the DCOM Configuration utility to specify full
    access for Everyone for the component. Once you get the
    component running, then you can specify proper access
    permissions.
12. When testing a remote sample, use the Windows NT Task Manager
    to terminate possible component processes still running from a
    previous sample.
13. If possible, try testing using Windows NT or on another
    computer.



WHERE TO GET SOFTWARE FOR SAMPLES NOT INCLUDED ON CD
====================================================
Microsoft Platform Software Development Kit (SDK)
-------------------------------------------------
The Platform SDK is the successor to the Win32 SDK and includes
components that have been distributed separately in the Win32 SDK,
the BackOffice SDK, the Internet Client SDK, and the DirectX SDK.
For information on how to download or obtain the Platform SDK,
check out the following web site:
    http://www.microsoft.com/msdn/sdk/

Visual Basic Service Pack
-------------------------
Visual Basic Service Pack 3, which is a superset of Service Packs 1
and 2, can be downloaded from the following web site:
    http://www.microsoft.com/vstudio/sp/
This site also has information on how to order a Visual Studio 97
Service Pack 3 CD, which includes the Visual Basic Service Pack 3.

Microsoft Transaction Server 2.0
--------------------------------
MTS 1.0 was not available for Windows 95, but with MTS 2.0 it is
available for Windows 95 by installing the "Windows NT Option Pack
for Windows 95". MTS 2.0 is available for Windows NT 4.0 by
installing the "Windows NT Option Pack for Windows NT 4.0".
For information on how to download or obtain this Option Pack for
Windows 95 or Windows NT 4.0, check out the following web site:
    http://www.microsoft.com/transaction/default.asp
NOTE: If you try to install the "Windows NT Option Pack for Windows
95", the following error may be displayed:
    "WinSock2 is required to run this setup utility"
For information on WinSock2 and how to download, check out the
following web site:
    http://www.microsoft.com/win32dev/netwrk/winsock2/ws295sdk.html



TROUBLES VIEWING LONG FILE NAMES ON CD
======================================
If you're unable to view the files in the COMSPEC, DCOMSPEC, and
SAMPLES directories, your CD driver software may not support long
file names.  To see and use these files, run the setup program
(SETUP.EXE) in the SETUP directory.  This setup program copies the
files to your hard disk, renames the files using the long file
names, and clears the Read-Only flag on the files.



SUPPORT INFORMATION
===================
Microsoft Press support information
-----------------------------------
Every effort has been made to ensure the accuracy of the book and
the contents of this companion CD. Microsoft Press provides
corrections for books at the following web site:
    http://mspress.microsoft.com/support/support.htm
If you have comments, questions, or ideas regarding the book or
this companion CD, please send them to Microsoft Press via 
e-mail to:
    MSPINPUT@MICROSOFT.COM
or via postal mail to:
    Microsoft Press
    Attn:  Inside Distributed COM Editor
    One Microsoft Way
    Redmond, WA  98052-6399
Please note that product support is not offered through the above
addresses. 



<end>