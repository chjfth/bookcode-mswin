@echo off
rem
rem Register.bat
rem Registration file for Chapter 10
rem
rem Chj: Don't use /s on regsvr32, so that regsvr32 reports failure loudly.
echo on

REM For in-proc server to run:
regsvr32 Server.dll

REM For out-proc server to run:
regsvr32 Proxy.dll
Server /RegServer