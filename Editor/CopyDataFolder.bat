@echo off
set "sourceFolder=./data"
set "debugFolder=../x64/Debug/data"
set "releaseFolder=../x64/Release/data"

xcopy "%sourceFolder%" "%debugFolder%" /e /i /y
xcopy "%sourceFolder%" "%releaseFolder%" /e /i /y