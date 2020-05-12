@echo off
rem
rem Register.bat
rem Registration file for Chapter 10
rem
echo on
regsvr32 Server.dll
regsvr32 Proxy.dll
Server /RegServer