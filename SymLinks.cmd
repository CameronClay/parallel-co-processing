@echo off
setlocal enabledelayedexpansion
set i=0
set a=x32
set b=..

for %%d in (Debug, Release, x64\Debug, x64\Release) do (
if not exist %%d md %%d

if !i!==2 (
	set a=x64
	set b=..\..
)
if not exist "%%d\TCPCS.dll" mklink "%%d\TCPCS.dll" "!b!\Common\CNLIB\!a!\TCPCS.dll"
if not exist "%%d\zlib1.dll" mklink "%%d\zlib1.dll" "!b!\Common\CNLIB\!a!\zlib1.dll"

set /a i+=1
)
endlocal