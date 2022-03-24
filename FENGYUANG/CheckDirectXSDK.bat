@echo off
setlocal EnableDelayedExpansion
set batfilenam=%~n0%~x0
set batdir=%~dp0
set batdir=%batdir:~0,-1%
set _vspgINDENTS=%_vspgINDENTS%.
call :Echos START from %batdir%

REM If Visual Studio version is less than VS2015(=v140), we would require user to have extra
REM DirectX SDK installed somewhere on his disk, indicated by DIR_DXSDK env-var.

if "%PlatformToolsetVersion%" LSS "140" (
	if "%DIR_DXSDK%"=="" (
		call :Echos [ERROR] For Visual Studio prior to VS2015, you need to specify DirectX SDK location in env-var DIR_DXSDK.
		call :Echos For example:
		call :Echos .   DIR_DXSDK=d:\DXSDK-Jun-2007
		call :Echos where d:\DXSDK-Jun-2007\Include\ddraw.h exists.
		exit /b 4
	)
	
	if not exist "%DIR_DXSDK%\Include\ddraw.h" (
		call :Echos [ERROR] You have DIR_DXSDK=%DIR_DXSDK%
		call :Echos .       But "%DIR_DXSDK%\Include\ddraw.h" does not exist.
		exit /b 4
	)
	
	call :Echos Detected: DIR_DXSDK=%DIR_DXSDK%
)


REM =============================
REM ====== Functions Below ======
REM =============================

:Echos
  REM This function preserves %ERRORLEVEL% for the caller,
  REM and, LastError does NOT pollute the caller.
  setlocal & set LastError=%ERRORLEVEL%
  echo %_vspgINDENTS%[%batfilenam%] %*
exit /b %LastError%

:EchoAndExec
  echo %_vspgINDENTS%[%batfilenam%] EXEC: %*
  call %*
exit /b %ERRORLEVEL%
