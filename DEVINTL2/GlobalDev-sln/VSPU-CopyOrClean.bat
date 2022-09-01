@echo off
setlocal EnableDelayedExpansion
set batfilenam=%~n0%~x0
set batdir=%~dp0
set batdir=%batdir:~0,-1%
set _vspgINDENTS=%_vspgINDENTS%.
call :Echos START from %batdir%

REM Parameter: 1 or 0
REM If 1, do copy action.
REM If 0, do clean action.

REM == debugging purpose ==
REM call :EchoVar _BuildConf_
REM call :EchoVar BuildConf
REM call :EchoVar PlatformName
REM call :EchoVar _ExeDllDir_
REM call :EchoVar ExeDllDir
REM call :EchoVar TargetName

REM =========================================================================
REM This bat copies VSIDE project output(EXE/DLL etc) to other target directories,
REM or clean(=delete) them from those target directories. 
REM Let's call these directories barns.
REM 
REM Typical usage cases:
REM * Copy EXE/DLL to a remote machine shared folder, so to debug your program
REM   running on that remote machine (remote debugging).
REM * By copying EXE/DLL from different projects to a single barn, you collect those
REM   EXE/DLL to help create a complete set of software package.
REM 
REM Two variables control the copy operation implemented in this bat.
REM 
REM (1) AGILE_COPY_PATTERNS
REM
set AGILE_COPY_PATTERNS=*.exe *.dll global.txt
REM 
REM		This sets what file (a concrete filename) or file-pattern (*.exe etc) to copy.
REM	
REM		This can be absolute path or relative path. If relative path, it is relative
REM		to $(TargetDir) from VSIDE, also referred to as %ExeDllDir% in this bat.
REM	
REM		This can be a list of multiple file/file-patterns separated by spaces.
REM		It means you can have multiple barns, each barn gets one copy.
REM	
REM		NOTE: If a barn path has space char, you must wrap it in double-quotes.
REM	
REM	(2) AGILE_BARN_DIRS
REM 
set AGILE_BARN_DIRS=L:\barn
REM
REM		You can list multiple barn dirs, separated by spaces.
REM 
REM		Also, if a barn dir contains space char, you must wrap that dir-path in double-quotes.
REM	
REM Hint: You can set/modify these AGILE_xxx vars directly in this bat file, 
REM       or set them from parent environment.
REM =========================================================================

REM Check param1 validity, must be 1 or 0
if "%~1"=="1" (
	set vspg_COPYORCLEAN_DO_CLEAN=
) else if "%~1"=="0" (
	set vspg_COPYORCLEAN_DO_CLEAN=1
) else (
	call :Echos [ERROR] First parameter must be 1 or 0. 1=docopy, 0=doclean.
	exit /b 4
)


rem set AGILE_COPY_PATTERNS="%ExeDllDir%\%TargetFilenam%"
REM or
rem set AGILE_COPY_PATTERNS=*.exe *.dll
REM -- using wildcard is not recommended, bcz it is not "clean"-friendly.

if not defined AGILE_BARN_DIRS (
	REM This is not considered error, it just means user do not want to do copy/clean now.
	call :Echos AGILE_BARN_DIRS is empty, nothing to copy.
	exit /b 0
)

if not defined AGILE_COPY_PATTERNS (
	call :Echos [ERROR] AGILE_COPY_PATTERNS is empty.
	exit /b 4
)

REM For the two AGILE_xxx, escape double-quotes for wrapping into a single bat-parameter.
call :PackDoubleQuotes %AGILE_COPY_PATTERNS%
set __SourcePatterns__=%_vspg_dqpacked%
rem
call :PackDoubleQuotes %AGILE_BARN_DIRS%
set __DestDirs__=%_vspg_dqpacked%
set DestSubdir=%PlatformName%\__%BuildConf%

call "%bootsdir%\AgileCopyOrDelete.bat"  "%ExeDllDir%"  "%__SourcePatterns__%"  "%__DestDirs__%"  "%DestSubdir%"

exit /b %ERRORLEVEL%



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

:EchoVar
  setlocal & set Varname=%~1
  call echo %_vspgINDENTS%[%batfilenam%]%~2 %Varname% = %%%Varname%%%
exit /b 0

:SetErrorlevel
  REM Usage example:
  REM call :SetErrorlevel 4
exit /b %1

:PackDoubleQuotes
  REM Take whole %* as input, replace each " with "" and return the result string.
  REM The return value is put in global var _vspg_dqpacked .
  set allparams=%*
  set _vspg_dqpacked=%allparams:"=""%
exit /b 0

