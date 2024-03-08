call ..\set.bat

cd PlayOgg

call ..\..\buildVcProj.bat PlayOgg

copy /b PlayOgg.h %XLibs%\


cd ..

