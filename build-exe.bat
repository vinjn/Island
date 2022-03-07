set path=%~p0
set path=%path:~0,-1%
For %%A in ("%path%") do (Set CurrDirName=%%~nxA)

set RunUAT=C:\UE_4.27\Engine\Build\BatchFiles\RunUAT.bat
set PROJECT_FILE="%~dp0\%CurrDirName%.uproject"

echo "Start build" > build-exe.log
echo %DATE% %TIME% >> build-exe.log

%RunUAT% BuildCookRun -project=%PROJECT_FILE% -noP4 -ue4exe=UE4Editor-Cmd.exe -compressed -SkipCookingEditorContent^
-platform=Win64 ^
-clientconfig=Development -serverconfig=Development ^
-cook -iterate -build -stage -pak -utf8output ^
-logtimes ^
-archive -archivedirectory=%~dp0 >> build-exe.log

echo "End build" > build-exe.log
echo %DATE% %TIME% >> build-exe.log
