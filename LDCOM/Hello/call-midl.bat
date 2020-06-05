@echo off
REM ==== boilerplate code >>>
REM
REM set CMD var SolutionDir, ProjectDir, OutDir, TargetPath and strip trailing backslash
set SolutionDir=%1
set SolutionDir=%SolutionDir:~0,-1%
set ProjectDir=%2
set ProjectDir=%ProjectDir:~0,-1%
set OutDir=%3
set OutDir=%OutDir:~0,-1%
REM Set CMD var for PlatformName
set PlatformName=%4
REM Set CMD var for the input-file (xxx.idl)
set InputFilepath=%5
REM
REM This will set batfilenam=call-midl.bat
set batfilenam=%~n0%~x0
REM
call :Echos START for %ProjectDir%
REM
REM ==== boilerplate code <<<<

if "%PlatformName%" == "Win32" (
	set MIDL_CMD=midl /Oicf /win32 /out ..\idl\Win32 %InputFilepath%
) else (
	set MIDL_CMD=midl /Oicf /x64   /out ..\idl\x64   %InputFilepath%
)
echo on
%MIDL_CMD%
@echo off

goto :END

REM =============================
REM ==== Main Procedure Done ====
REM =============================

:Echos
  echo [%batfilenam%] %*
exit /b

:END
echo [%batfilenam%] END for %ProjectDir%
