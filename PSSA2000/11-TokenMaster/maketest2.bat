@echo off
setlocal
set batdir=%~dp0
set batdir=%batdir:~0,-1%
call "%batdir%\maketest1.bat"

echo ==============

set dirbase=D:\_testPSSA2000

REM Create grpBar group, it contains no members
net localgroup grpBar >nul 2>&1 || net localgroup grpBar /add

REM Deny grpBar to fileA.txt
icacls %dirbase%\fileA.txt /deny grpBar:F

REM Create file for grpBar only
echo only-grpBar > %dirbase%\only-grpBar.txt
icacls %dirbase%\only-grpBar.txt /reset
icacls %dirbase%\only-grpBar.txt /grant grpBar:F

REM Create file that allow Users and grpBar
echo Users-and-grpBar > %dirbase%\Users-and-grpBar.txt
icacls %dirbase%\Users-and-grpBar.txt /reset
icacls %dirbase%\Users-and-grpBar.txt /grant Users:F /grant grpBar:F

REM Create file that allow Users but deny grpBar
echo Users-deny-grpBar > %dirbase%\Users-deny-grpBar.txt
icacls %dirbase%\Users-deny-grpBar.txt /reset 
icacls %dirbase%\Users-deny-grpBar.txt /grant Users:F /deny grpBar:F
