@echo off
echo Creating Discord RPC Controller deployment package...

set BUILD_DIR=cmake-build-release
set DEPLOY_DIR=DiscordRPC-Release
set VENV_DIR=C:\Users\kerrigan\Documents\Projects\discord-rpc\.venv

REM Clean old deployment
if exist %DEPLOY_DIR% rmdir /s /q %DEPLOY_DIR%
mkdir %DEPLOY_DIR%

REM Copy executable
copy %BUILD_DIR%\DiscordRPC.exe %DEPLOY_DIR%\

REM Copy Python DLL
copy %BUILD_DIR%\python314.dll %DEPLOY_DIR%\

REM Copy module.py
copy module.py %DEPLOY_DIR%\

REM Copy Python libraries (pypresence, requests, etc.)
mkdir %DEPLOY_DIR%\Lib
mkdir %DEPLOY_DIR%\Lib\site-packages
xcopy /E /I /Y %VENV_DIR%\Lib\site-packages\pypresence %DEPLOY_DIR%\Lib\site-packages\pypresence
xcopy /E /I /Y %VENV_DIR%\Lib\site-packages\requests %DEPLOY_DIR%\Lib\site-packages\requests
xcopy /E /I /Y %VENV_DIR%\Lib\site-packages\urllib3 %DEPLOY_DIR%\Lib\site-packages\urllib3
xcopy /E /I /Y %VENV_DIR%\Lib\site-packages\certifi %DEPLOY_DIR%\Lib\site-packages\certifi
xcopy /E /I /Y %VENV_DIR%\Lib\site-packages\charset_normalizer %DEPLOY_DIR%\Lib\site-packages\charset_normalizer
xcopy /E /I /Y %VENV_DIR%\Lib\site-packages\idna %DEPLOY_DIR%\Lib\site-packages\idna

REM Run windeployqt for Qt DLLs
C:\Qt\6.10.1\mingw_64\bin\windeployqt.exe --release --no-translations %DEPLOY_DIR%\DiscordRPC.exe

echo.
echo Deployment package created in %DEPLOY_DIR%\
echo You can now distribute this folder!
pause
