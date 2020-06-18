---------------------------------
Apartment Threading
---------------------------------
See Chapter 4 "Threading Models and Apartments"
for information on Threading



Minimum System Requirements
===========================
Windows 95 with DCOM installed or Windows NT 4.0
Visual C++ 5.0 Professional

NOTE: In order to compile and test the Global Interface Table
sample, you must have the following configuration:
 - Windows NT 4.0 with Service Pack 3 installed
 - Visual C++ 5.0 Professional
 - Microsoft Windows Platform SDK



<<1>> STA Model EXE sample (local)
============================
1.  Open "STA model EXE.dsw" in Visual C++
2.  Build Client.exe
    (this also builds Component.exe)
3.  Run Component.exe to register
4.  Run Client.exe to test

[2020-06-18] Chj: In order to use a debugger to debug Component.exe throughout all its life time,
we can use Visual Studio IDE to remote launch Component.exe with -Embedding parameter.

<<2>> STA Model EXE sample (remote)
=============================
Local computer can be Windows 95 with DCOM or Windows NT 4.0
Remote computer must be Windows NT 4.0
1.  Open "STA model EXE.dsw" in Visual C++
2.  Build Client.exe
    (this also builds Component.exe)
3.  Run Component.exe on local computer to register
4.  Run DCOM Configuration utility (dcomcnfg.exe) on local computer
    and specify that the "Inside DCOM Sample" should run on remote
    computer
5.  Copy Component.exe to remote computer
6.  Run Component.exe on remote computer to register
7.  Run Client.exe on local computer to test


<<3>> MTA Model EXE sample (local)
============================
1.  Open "MTA model EXE.dsw" in Visual C++
2.  Build Client.exe
    (this also builds Component.exe)
3.  Run Component.exe to register
4.  Run Client.exe to test

<<4>> MTA Model EXE sample (remote)
=============================
Local computer can be Windows 95 with DCOM or Windows NT 4.0
Remote computer must be Windows NT 4.0
1.  Open "MTA model EXE.dsw" in Visual C++
2.  Build Client.exe
    (this also builds Component.exe)
3.  Run Component.exe on local computer to register
4.  Run DCOM Configuration utility (dcomcnfg.exe) on local computer
    and specify that the "Inside DCOM Sample" should run on remote
    computer
5.  Copy Component.exe to remote computer
6.  Run Component.exe on remote computer to register
7.  Run Client.exe on local computer to test


NOTE: You can also test the component in the following samples with
different ThreadingModel values. For example, in the last parameter
of the RegisterServer function in the component code, you can set
the ThreadingModel value to Apartment, Free, or Both.


<<5>> Thread Safe In-Process sample
=============================
1.  Open "Thread Safe.dsw" in Visual C++
2.  Build Client.exe
    (this also builds Component.dll)
3.  Register Component.dll
4.  Run Client.exe to test


<<6>> Free Threaded Marshaler sample
==============================
Component with Free Threaded Marshaler
--------------------------------------
1.  Open FTM.dsw in Visual C++
2.  Build Client.exe
    (this also builds "Component with FTM.dll")
3.  Register "Component with FTM.dll"
4.  Run Client.exe to test


<<7>> Component without Free Threaded Marshaler
-----------------------------------------
1.  Open FTM.dsw in Visual C++
2.  Build Client.exe
3.  Build "Component without FTM.dll"
4.  Register "Component without FTM.dll"
5.  Run Client.exe to test


<<8>> Global Interface Table sample
=============================
Requires Windows NT 4.0 SP3 and Platform SDK
1.  Open GIT.dsw in Visual C++
2.  Build Client.exe
    (this also builds "Component with GIT.dll")
3.  Register "Component with GIT.dll"
4.  Build Comp.dll
5.  Register Comp.dll
6.  Run Client.exe to test with Global Interface Table
7.  Build "Component without GIT.dll"
8.  Register "Component without GIT.dll"
9.  Run Client.exe to test without Global Interface Table



--------------------------------------
NOTE: Threading Models in Visual Basic
--------------------------------------
In order to specify a Threading Model for components created in
Visual Basic 5.0 as shown in Chapter 4 "Threading Models and
Apartments" (Figure 4-5 on page 143), you must have at least
Service Pack 2 of Visual Basic installed.
See section "WHERE TO GET SOFTWARE FOR SAMPLES NOT INCLUDED ON CD"
in the ReadMe.txt file at the root of the CD for information on
how to obtain this Service Pack and other software.



<end>