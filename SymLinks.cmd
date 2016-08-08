@echo off
setlocal enabledelayedexpansion

net file 1>nul 2>nul
if errorlevel 1 goto :runadmin

set i=0
set a=x32\
set b=..

pushd %~p0

for %%d in (Debug, Release, x64\Debug, x64\Release) do (
if not exist %%d md %%d

if !i!==2 (
	set a=
	set b=..\..
)

for %%f in (TCPCS.dll, zlib1.dll) do (
	if exist %%d\%%f del %%d\%%f
	mklink %%d\%%f "!b!\Common\CNLIB\!a!%%d\%%f"
)

set /a i+=1
)

popd

endlocal
pause
goto :eof

:runadmin
wscript RunAdmin.vbs %*
