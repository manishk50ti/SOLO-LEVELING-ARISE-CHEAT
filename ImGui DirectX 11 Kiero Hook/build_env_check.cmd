@echo off
setlocal
for %%v in (2022 2019) do (
    for %%e in (Community Professional Enterprise) do (
        if exist "C:\Program Files\Microsoft Visual Studio\%%v\%%e\Common7\Tools\VsDevCmd.bat" (
            call "C:\Program Files\Microsoft Visual Studio\%%v\%%e\Common7\Tools\VsDevCmd.bat" -arch=amd64
            goto :found
        )
    )
)
echo ERROR: Could not find Visual Studio installation
exit /b 1
:found
 echo VCToolsVersion=%VCToolsVersion%
 echo VCTargetsPath=%VCTargetsPath%
 echo VSINSTALLDIR=%VSINSTALLDIR%
 where cl.exe
 where link.exe
 where msbuild.exe
