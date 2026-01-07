@echo off
echo Signing DiscordRPC.exe...

REM Update these paths
set SIGNTOOL="C:\Program Files (x86)\Windows Kits\10\bin\10.0.22621.0\x64\signtool.exe"
set PFX_FILE="C:\path\to\your\certificate.pfx"
set PFX_PASSWORD=your_password_here
set TIMESTAMP=http://timestamp.digicert.com

REM Sign the main executable
%SIGNTOOL% sign /f %PFX_FILE% /p %PFX_PASSWORD% /tr %TIMESTAMP% /td sha256 /fd sha256 ^
    "C:\Users\kerrigan\Documents\Projects\guiuis\DiscordRPC-Release\DiscordRPC.exe"

if %ERRORLEVEL% EQU 0 (
    echo Successfully signed DiscordRPC.exe
    
    REM Verify signature
    %SIGNTOOL% verify /pa "C:\Users\kerrigan\Documents\Projects\guiuis\DiscordRPC-Release\DiscordRPC.exe"
    
    if %ERRORLEVEL% EQU 0 (
        echo Signature verified successfully!
    ) else (
        echo WARNING: Signature verification failed!
    )
) else (
    echo ERROR: Failed to sign DiscordRPC.exe
)

pause
