@echo off

del Bin\autorun.exe
del Bin\qd_game.exe
del Bin\qd_game_final.exe
del Bin\qd_packager.exe
del Bin\qda_editor.exe
del Bin\QuestEditor.exe

cd Core
echo building qd_game.exe
BuildConsole qdEngine.sln /Prj="qdEngine" /cfg="Release" /NoLogo /ShowAgent %1
if not exist ..\Bin\qd_game.exe goto bad1

echo building qd_game_final.exe
BuildConsole qdEngine.sln /Prj="qdEngine" /cfg="Final" /NoLogo /ShowAgent %1
if not exist ..\Bin\qd_game_final.exe goto bad2

echo building qd_packager.exe
BuildConsole qdEngine.sln /Prj="qdPackager" /cfg="Release" /NoLogo /ShowAgent %1
if not exist ..\Bin\qd_packager.exe goto bad3

echo building qda_editor.exe
BuildConsole qdEngine.sln /Prj="qdaEditor" /cfg="Release" /NoLogo /ShowAgent
if not exist ..\Bin\qda_editor.exe goto bad4

echo building autorun.exe
BuildConsole qdEngine.sln /Prj="Autorun" /cfg="Release" /NoLogo /ShowAgent
if not exist ..\Bin\autorun.exe goto bad4


cd ..\QuestEditor

echo building QuestEditor.exe
BuildConsole QuestEditor7.1.sln /Prj="QuestEditor7.1" /cfg="Release" /NoLogo /ShowAgent %1
if not exist ..\Bin\QuestEditor.exe goto bad5

cd ..

goto good

:bad1
echo "qd_game.exe build failed"
pause
goto end
:bad2
echo "qd_game_final.exe build failed"
pause
goto end
:bad3
echo "qd_packager.exe build failed"
pause
goto end
:bad4
echo "qda_editor.exe build failed"
pause
goto end
:bad5
echo "QuestEditor.exe build failed"
pause
goto end


:good

net send localhost "qdEngine build done"

cd ..

:end
exit
