@echo off
set batfilenam=%~n0%~x0
REM ==== boilerplate code >>>
REM Called with PostBuildCmds.bat $(Configuration) $(PlatformName) $(TargetDir) $(TargetFileName)
REM Example:    PostBuildCmds.bat Debug Win32 D:\...\OCREngine\__Debug\Win32\ Server.exe
REM Example:    PostBuildCmds.bat Release x64 D:\...\OCREngine\__Release\x64\ Server.exe
set Conf=%1
set PlatformName=%2
set TargetDir=%3
set TargetDir=%TargetDir:~0,-1%
set TargetFilenam=%4
set TargetFilepath=%TargetDir%\%TargetFilenam%
REM
call :Echos START for %TargetFilepath%
REM
REM ==== boilerplate code <<<<

set DIR_SYNC_TO_FINAL=%DIR_SYNC_TO%\__%Conf%\%PlatformName%

REM --------------------------------------------------------------------------
REM If DIR_SYNC_TO env-var is defined, we sync %TargetFilepath% to that dir.
REM --------------------------------------------------------------------------

set COPY_CMD=copy %TargetFilepath% %DIR_SYNC_TO_FINAL%
REM
if "%DIR_SYNC_TO%" == "" (
	call :Echos Env-var DIR_SYNC_TO is null, sync nothing.
) else (
	call :Echos See Env-var DIR_SYNC_TO=%DIR_SYNC_TO%
	mkdir %DIR_SYNC_TO_FINAL%
	
	call :EchoExec %COPY_CMD%
	%COPY_CMD%
    if errorlevel 1 (
      exit /b 4
    )
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
echo [%batfilenam%] END for %TargetFilepath%
