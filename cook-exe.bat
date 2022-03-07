set path=%~p0
set path=%path:~0,-1%
For %%A in ("%path%") do (Set CurrDirName=%%~nxA)

set RunUAT=C:\UE_4.27\Engine\Build\BatchFiles\RunUAT.bat
set PROJECT_FILE="%~dp0\%CurrDirName%.uproject"

echo "Start build" > cook-exe.log
echo %DATE% %TIME% >> cook-exe.log

call %RunUAT% BuildCookRun -project=%PROJECT_FILE% -noP4 -ue4exe=UE4Editor-Cmd.exe -compressed -SkipCookingEditorContent^
-platform=Win64 ^
-clientconfig=Development -serverconfig=Development ^
-cook -iterate -utf8output ^
-logtimes >> cook-exe.log

echo "End build" >> cook-exe.log
echo %DATE% %TIME% >> cook-exe.log
