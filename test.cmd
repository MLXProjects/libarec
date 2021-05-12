@echo off

if "%1" == "r" (
	echo Going to skip build
	if "%1"=="noserver" goto test
	if "%2"=="noserver" goto test 
	goto client
)
if "%2" == "r" (
	echo Going to skip build
	if "%1"=="noserver" goto test
	if "%2"=="noserver" goto test 
	goto client
)

if "%1"=="noserver" (
	call build.cmd test noserver
	goto test
) else if "%2"=="noserver" (
	call build.cmd test noserver
	goto test	
) else call build.cmd test 

:client
echo Running test client
if not exist %~dp0\out\arec-client goto enoent
adb push %~dp0\out\arec-client /tmp/arec-client
adb shell chmod 0755 /tmp/arec-test
adb shell /tmp/arec-client
goto :eof

:test
echo Running test app
if not exist %~dp0\out\arec-test goto enoent
adb push %~dp0\out\arec-test /tmp/arec-test
adb shell chmod 0755 /tmp/arec-test
adb shell /tmp/arec-test
goto :eof

:enoent
echo File doesn't exist, did it build correctly?