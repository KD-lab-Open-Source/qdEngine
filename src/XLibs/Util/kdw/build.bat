call ..\..\set.bat

if "%1"=="copyHeaders" goto copyHeaders

call ..\..\buildVcProj.bat kdWidgets

:copyHeaders
rd %XLibs%\kdw\ /q /s
xcopy *.h %XLibs%\kdw\ /S
rd %XLibs%\kdw\Test /q /s
del %XLibs%\kdw\stdafx.h /q 
