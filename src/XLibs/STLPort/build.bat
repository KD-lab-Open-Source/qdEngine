call ..\set.bat

cd build\lib


::call configure.bat -c msvc71 --not-thread-safe

:: static mt
call configure.bat -c msvc71
nmake /fmsvc.mak


:: mt dll
::call configure.bat -c msvc71 --rtl-dynamic
::nmake /fmsvc.mak

move obj\vc71\static\*.lib %XLIBS%\Vc7.1
move obj\vc71\static-g\*.lib %XLIBS%\Vc7.1
move obj\vc71\static\*.pdb %XLIBS%\Vc7.1
move obj\vc71\static-g\*.pdb %XLIBS%\Vc7.1

move obj\vc71\shared\*.lib %XLIBS%\Vc7.1
move obj\vc71\shared-g\*.lib %XLIBS%\Vc7.1

move obj\vc71\shared\*.dll %XLIBS%
move obj\vc71\shared-g\*.dll %XLIBS%


::del obj\*.* /s /q
::rd obj /s /q

cd ..\..

del %XLibs%\stl /q /s
rd %XLibs%\stl /q /s
xcopy stlport %XLibs%\stl\ /E

