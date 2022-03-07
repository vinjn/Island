set path=%~p0
set path=%path:~0,-1%
For %%A in ("%path%") do (Set CurrDirName=%%~nxA)

set RunUAT=C:\UE_4.27\Engine\Build\BatchFiles\RunUAT.bat
set PROJECT_FILE="%~dp0\%CurrDirName%.uproject"

echo "Start build" > build-apk.log
echo %DATE% %TIME% >> build-apk.log

COPY /Y source\GameActivity.java c:\UE_4.27\Engine\Build\Android\Java\src\com\epicgames\ue4\GameActivity.java.template
MOVE /Y c:\UE_4.27\Engine\Source\ThirdParty\heapprofd\arm64-v8a\libheapprofd_standalone_client.so c:\UE_4.27\Engine\Source\ThirdParty\heapprofd\arm64-v8a\libheapprofd_standalone_client.so_

call %RunUAT% BuildCookRun -project=%PROJECT_FILE% -noP4 -ue4exe=UE4Editor-Cmd.exe -compressed -SkipCookingEditorContent^
-platform=android -targetplatform=android -cookflavor=ASTC ^
-clientconfig=Development -serverconfig=Development ^
-cook -iterate -build -stage -pak -utf8output ^
-logtimes ^
-archive -archivedirectory=%~dp0 >> build-apk.log

echo "End build" >> build-apk.log
echo %DATE% %TIME% >> build-apk.log
