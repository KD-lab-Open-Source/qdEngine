call ..\set.bat

if "%1"=="copyHeaders" goto copyHeaders

cd Profiler
call ..\..\buildVcProj Profiler
cd ..

:copyHeaders

copy /b Profiler\Profiler.h %XLibs%\
