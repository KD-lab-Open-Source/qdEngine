@echo off

del Bin\qd_game.exe
del Bin\qd_game_final.exe
del Bin\qd_packager.exe
del Bin\qda_editor.exe
del Bin\QuestEditor.exe

cd Core
echo building qd_game.exe
devenv.exe qdEngine.sln /build Release /out qdEngine_build.log %1
if not exist ..\Bin\qd_game.exe goto bad1
if not exist ..\Bin\qd_packager.exe goto bad3
if not exist ..\Bin\qda_editor.exe goto bad4
if not exist ..\Bin\autorun.exe goto bad4

echo building qd_game_final.exe
devenv.exe qdEngine.sln /build Final /out qdEngine_build_final.log %1
if not exist ..\Bin\qd_game_final.exe goto bad2

cd ..\QuestEditor

echo building QuestEditor.exe
devenv.exe QuestEditor7.1.sln /build Release /out QuestEditor_build.log
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

exit

:end