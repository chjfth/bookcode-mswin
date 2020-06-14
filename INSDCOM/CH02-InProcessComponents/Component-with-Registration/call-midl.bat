@echo off
REM This will set batfilenam=call-midl.bat
set batfilenam=%~n0%~x0
REM
REM call-midl.bat $(SolutionDir) $(ProjectDir) $(TargetDir) $(TargetName) $(PlatformName) %(FullPath)
set SolutionDir=%1
set SolutionDir=%SolutionDir:~0,-1%
set ProjectDir=%2
set ProjectDir=%ProjectDir:~0,-1%
set TargetDir=%3
set TargetDir=%TargetDir:~0,-1%
REM Set CMD var for PlatformName (=Win32 or =x64)
set TargetName=%4
set PlatformName=%5
REM Set CMD var for the input-file (xxx.idl)
set InputFilepath=%6
REM
call :Echos START for %ProjectDir%
REM

if not exist ".\%PlatformName%" mkdir ".\%PlatformName%"

if "%PlatformName%" == "Win32" (
	set MIDL_CMD=midl /Oicf  /win32  /out .\%PlatformName% %InputFilepath%
) else (
	set MIDL_CMD=midl /Oicf  /x64    /out .\%PlatformName% %InputFilepath%
)

call :EchoExec %MIDL_CMD%
%MIDL_CMD%
if errorlevel 1 exit /b 4

set tlbFilepath=.\%PlatformName%\%TargetName%.tlb
set COPY_CMD=copy %tlbFilepath% %TargetDir%
if exist %tlbFilepath% (
	call :EchoExec %COPY_CMD%
	%COPY_CMD%
	if errorlevel 1 exit /b 4
)
goto :END

REM =============================
REM ==== Main Procedure Done ====
REM =============================

:Echos
  echo [%batfilenam%] %*
exit /b

:EchoExec
  echo [%batfilenam%] EXEC: %*
exit /b

:END
echo [%batfilenam%] END for %ProjectDir%
