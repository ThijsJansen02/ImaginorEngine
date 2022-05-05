@echo off
rem cl /LD /DIME_WINDOWS /DIME_DLL_BUILD /I..\..\..\Imaginor\src ..\..\..\Handmade\src\handmade\handmade.cpp /DLL /Fe:Handmade.dll 

set solutiondir=W:\ImaginorEngine

for /f "tokens=2 delims==" %%a in ('wmic OS Get localdatetime /value') do set "dt=%%a"
set "YY=%dt:~2,2%" & set "YYYY=%dt:~0,4%" & set "MM=%dt:~4,2%" & set "DD=%dt:~6,2%"
set "HH=%dt:~8,2%" & set "Min=%dt:~10,2%" & set "Sec=%dt:~12,2%"

set "fullstamp=%YYYY%-%MM%-%DD%_%HH%-%Min%-%Sec%"

set outputdir=%solutiondir%\bin\windows\debug-x64\ImaginorDebugConsole
set prjdir=%solutiondir%\ImaginorDebug
set windowssdk=/I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\um" /I"C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\winrt" /I"C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\cppwinrt" /I"C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\shared" /I"C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\ucrt" /I"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30133\include" /I"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30133\ATLMFC\include"
set srcfiles=%prjdir%\src\DebugConsole.cpp %solutiondir%\ImaginorEngine\src\rendering\Renderer2D.cpp %solutiondir%\ImaginorEngine\src\memory.cpp
set include=/I%solutiondir% /I%prjdir% /I%solutiondir%\ImaginorMath\src 
set defines=/DIME_DEBUG /DIME_DLL_BUILD
set libs=ImaginorEngine.lib
set compilorflags=/MDd /std:c++17 /Zi /Fe:ImaginorDebugConsole.dll
set linkerflags=/link /EXPORT:applicationUpdate /EXPORT:applicationInit /DLL /MAP /PDB:ImaginorEngine%fullstamp%.pdb

pushd %outputdir%

copy ..\ImaginorEngine\ImaginorEngine.lib 
cl %compilorflags% %defines% %include% %windowssdk% %srcfiles% %libs% %linkerflags%

copy ImaginorDebugConsole.dll ..\platform

popd
