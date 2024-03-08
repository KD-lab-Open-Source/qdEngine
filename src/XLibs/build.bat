@ECHO OFF

CALL "set.bat"

if exist %XLibs% rd %XLibs% /q /s
md %XLibs% 
xcopy Heap %XLibs%\ /E



PUSHD Util\Serialization
call build.bat copyHeaders
POPD

PUSHD Util\XMath
call build.bat copyHeaders
POPD

@REM PUSHD Util\kdw
@REM call build.bat copyHeaders
@REM POPD

PUSHD Profiler
call build.bat copyHeaders
POPD



PUSHD STLport
call build.bat 
POPD

PUSHD XUtil
call build.bat 
POPD

PUSHD XZip
call build.bat 
POPD


PUSHD Util\Serialization
call build.bat 
POPD

PUSHD Util\XMath
call build.bat 
POPD

@REM PUSHD Util\kdw
@REM call build.bat
@REM POPD

PUSHD Profiler
call build.bat 
POPD


PUSHD OGG
call build.bat 
POPD



@REM PUSHD ProfUIS
@REM call build.bat 
@REM POPD



@REM PUSHD DemonWare
@REM call build.bat 
@REM POPD

@REM PUSHD Util\XmlRpc
@REM call build.bat
@REM POPD

@REM PUSHD MySQLpp
@REM call build.bat 
@REM POPD