@echo off

REM You can add more bat search location via vspg_USER_BAT_SEARCH_DIRS .
REM set vspg_USER_BAT_SEARCH_DIRS="mydir1" "mydir2"

setlocal EnableDelayedExpansion

set batfilenam=%~n0%~x0
set batdir=%~dp0
set batdir=%batdir:~0,-1%
set _vspgINDENTS=%_vspgINDENTS%.
call :Echos START from %batdir% (Loading Env-vars affecting parent env)

call :GetAbsPath parentdir "%batdir%\.."

REM We have to endlocal here, bcz the following :SetEnvVar need to change parent bat's env.
endlocal & (
	REM Every batch var definition from now on will affect the parent env.
	REM So be conservative, define as little env-var as possible.
	REM I suggest to prefix them with _tmp_ if you don't use them after this bat exits.
	set _tmp_=%parentdir%
)


REM If you have special environment variable(env-var) to set for you own .bat files,
REM you can set it here.



REM ################ KEY CONTENT HERE ################ 

call :GetAbsPath dirThisBookRoot "%ProjectDir%\..\.."
call :SetEnvVar vspg_USER_BAT_SEARCH_DIRS="%dirThisBookRoot%"

exit /b 0



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

:Echos0
  REM This function preserves %ERRORLEVEL% for the caller,
  REM and, LastError does NOT pollute the caller.
  setlocal & set LastError=%ERRORLEVEL%
  echo %_vspgINDENTS%.[%~n0%~x0] %*
exit /b %LastError%

:SetEnvVar
  call :Echos0 set %*
  set %*
exit /b 0

:GetAbsPath
  REM Get absolute-path from a relative-path(relative to %CD%). 
  REM If already absolute, return as is.
  REM If your input dir is relative to current .bat, use RelaPathToAbs instead.
  REM Param1: Var name to receive output.
  REM Param2: The input path.
  REM
  REM Feature 1: this function removes redundant . and .. from input path.
  REM I
  REM For example:
  REM     call :GetAbsPath outvar C:\abc\.\def\..\123
  REM Returns outvar:
  REM     C:\abc\123
  REM
  REM Feature 2: It's pure string manipulation, so the input path doesn't have to 
  REM actually exist on the file-system.
  REM
  if "%~1"=="" exit /b 4
  if "%~2"=="" exit /b 4
  for %%a in ("%~2") do set "%~1=%%~fa"
exit /b 0
