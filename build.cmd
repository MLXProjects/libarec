@echo off
setlocal enableextensions enabledelayedexpansion

set AREC_TOOLCHAIN=C:\Users\MLX\projects\armhf-6.3\bin\arm-linux-gnueabihf-
set AREC_GCC=%AREC_TOOLCHAIN%gcc
set AREC_GPP=%AREC_TOOLCHAIN%g++
set AREC_BASE=%~dp0
set AREC_CFLAGS=-O3 -pthread -s -w
rem default lib sources
set AREC_TARGET_SRC=%AREC_BASE%\src\arec.c ^
					%AREC_BASE%\src\tools.c
set AREC_MKSERVER=1

if "%1"=="noserver" ( 
	set AREC_CFLAGS=!AREC_CFLAGS! -DAREC_NOSERVER 
	set AREC_MKSERVER=0
) else if "%2"=="noserver" (
	set AREC_CFLAGS=!AREC_CFLAGS! -DAREC_NOSERVER
	set AREC_MKSERVER=0
) else if not "%1"=="test" set AREC_TARGET_SRC=!AREC_TARGET_SRC! %AREC_BASE%\src\arec_sv_utils.c

:mklib
pushd %AREC_BASE%\out\obj

echo Building libarec
%AREC_GCC% -c ^
	!AREC_CFLAGS! ^
	!AREC_TARGET_SRC! ^
	-I%AREC_BASE%\include\ 
rem find object files and fill the variable with their paths
set "AREC_TARGET_SRC= "
for /f "tokens=*" %%F in ('dir /b /a:-d "%~dp0\out\obj\*.o"') do call set AREC_TARGET_SRC=%%AREC_TARGET_SRC%% %~dp0\out\obj\%%F
cd ..

if "%1"=="test" (
	if !AREC_MKSERVER!==1 (
		goto mkclient 
	) else goto mktest
) else if !AREC_MKSERVER!==1 (
	goto mkserver
) else goto :eof

:mkserver
echo Building libarec server
%AREC_GCC% -static ^
	!AREC_CFLAGS! ^
	!AREC_TARGET_SRC! ^
	%AREC_BASE%\src\arec_server.c ^
	-I%AREC_BASE%\include\ ^
	-o arec-server
goto end

:mkclient
echo Building libarec test client
%AREC_GCC% -static ^
	!AREC_CFLAGS! ^
	!AREC_TARGET_SRC! ^
	%AREC_BASE%\tests\test.c ^
	-I%AREC_BASE%\include\ ^
	-o arec-client
goto end

:mktest
echo Building libarec test app
%AREC_GCC% -static ^
	!AREC_CFLAGS! ^
	!AREC_TARGET_SRC! ^
	%AREC_BASE%\tests\test.c ^
	-I%AREC_BASE%\include\ ^
	-o arec-test
goto end

:end
popd
endlocal