@echo off
REM ==== boilerplate code >>>
REM
set batfilenam=%~n0%~x0
REM <Command>$(SolutionDir)PostBuildGeneric.bat $(SolutionDir) $(ProjectDir) $(Configuration) $(PlatformName) $(TargetDir) $(TargetFileName)</Command>
set SolutionDir=%1
set SolutionDir=%SolutionDir:~0,-1%
set ProjectDir=%2
set ProjectDir=%ProjectDir:~0,-1%
REM BuildConf : Debug | Release
set BuildConf=%3
REM PlatformName : Win32 | x64
set PlatformName=%4
REM TargetDir is the EXE/DLL output directory
set TargetDir=%5
set TargetDir=%TargetDir:~0,-1%
REM TargetFilenam is the EXE/DLL output name (varname chopping trailing 'e', means "no path prefix")
set TargetFilenam=%6
REM
rem call :Echos START for %ProjectDir%
REM
REM ==== boilerplate code <<<<


call :Echos called with params: 
call :Echos   SolutionDir = %SolutionDir%
call :EchoVar ProjectDir
call :EchoVar BuildConf
call :EchoVar PlatformName
call :EchoVar TargetDir
call :EchoVar TargetFilenam

REM Call PostBuildSyncOutput.bat only if that file exist. If you need it, just copy it from the .template aside.
if exist %SolutionDir%\PostBuildSyncOutput.bat (
	call %SolutionDir%\PostBuildSyncOutput.bat %BuildConf% %PlatformName% %TargetDir%
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

:EchoVar
  REM Env-var double expansion trick from: https://stackoverflow.com/a/1202562/151453
  set _Varname=%1
  for /F %%i in ('echo %%%_Varname%%%') do echo [%batfilenam%] %_Varname% = %%i
exit /b

:END
rem echo [%batfilenam%] END for %ProjectDir%
