@REM Chj: Build with VC6 SP6 + MsSDK Feb 2003, uuid2.lib and uuid3.lib are not required.
cl -MTd -Zi -Od -D_DEBUG Category.cpp ole32.lib advapi32.lib uuid.lib 
