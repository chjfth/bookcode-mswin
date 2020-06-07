@echo off
set batdir=%~dp0
set batdir=%batdir:~0,-1%
set _batfilenam=%~n0%~x0
set batfilenam=%_batfilenam%
REM ==== boilerplate code >>>
REM Called with PostBuildCmdsME.bat $(Configuration) $(PlatformName) $(TargetDir) $(TargetFileName)
REM Example:    PostBuildCmdsME.bat Debug Win32 D:\...\OCREngine\__Debug\Win32\ Server.exe
REM Example:    PostBuildCmdsME.bat Release x64 D:\...\OCREngine\__Release\x64\ Server.exe
set Conf=%1
set PlatformName=%2
set TargetDir=%3
set TargetDir=%TargetDir:~0,-1%
set TargetFilenam=%4
set TargetFilepath=%TargetDir%\%TargetFilenam%
REM
rem call :Echos START for %TargetFilepath%
REM
REM ==== boilerplate code <<<<

call %batdir%\..\PostBuildCmds.bat %1 %2 %3 %4
REM --- PostBuildCmds.bat will set DIR_SYNC_TO_FINAL for us. OK.
REM Re-assign batfilenam here, bcz it has been overwritten by PostBuildCmds.bat
set batfilenam=%_batfilenam%

rem call :Echos Got DIR_SYNC_TO_FINAL=%DIR_SYNC_TO_FINAL%


set COPY_CMD=copy %TargetDir%\ocr.tlb %DIR_SYNC_TO_FINAL%
REM
if not "%DIR_SYNC_TO_FINAL%" == "" (
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
rem echo [%batfilenam%] END for %TargetFilepath%
