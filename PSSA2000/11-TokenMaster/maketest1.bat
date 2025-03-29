@echo off
@setlocal
set dirbase=D:\_testPSSA2000
echo Delete and recreate %dirbase% ...

REM Create a user ufoo1 if not exist yet, password 123.
net user ufoo1 >nul 2>&1 || net user ufoo1 123 /add

REM Create the grpFoo group if not exist yet.
net localgroup grpFoo >nul 2>&1 || net localgroup grpFoo /add

REM check whether ufoo1 has been in grpFoo, if not, add ufoo1 to grpFoo
net localgroup grpFoo | findstr /i "\<ufoo1\>" >nul 2>&1 && set in_group=yes || set in_group=

if not defined in_group (
    net localgroup grpFoo ufoo1 /add
)

rd /s /q %dirbase%

mkdir %dirbase%

REM Grant Users all access
icacls %dirbase% /grant Users:F

REM Remove inheritance for %dirbase%, to make NtfsDumpSD concise.
icacls %dirbase% /inheritance:r

echo fileA > %dirbase%\fileA.txt
icacls %dirbase%\fileA.txt /reset
icacls %dirbase%\fileA.txt /grant Users:F


echo fileB > %dirbase%\fileB.txt
icacls %dirbase%\fileB.txt /reset
icacls %dirbase%\fileB.txt /grant Users:F /deny grpFoo:F


echo fileC > %dirbase%\fileC.txt
icacls %dirbase%\fileC.txt /reset
icacls %dirbase%\fileC.txt /grant grpFoo:F
