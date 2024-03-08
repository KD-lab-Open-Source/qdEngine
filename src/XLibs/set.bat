@ECHO OFF

SET VsShell_2003_32="%VS71COMNTOOLS%\..\..\Vc7\bin\vcvars32.bat"
IF NOT DEFINED DevEnvDir (
  IF EXIST %VsShell_2003_32% (CALL %VsShell_2003_32%)
)

if "%XLibs%"=="" set XLibs=C:\XLibs

@REM NOTE(pabdulin): not used anywhere
@REM set devenv="%MSV%\Common7\IDE\devenv.com"
