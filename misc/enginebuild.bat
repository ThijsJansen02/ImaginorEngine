@echo off
rem cl /LD /DIME_WINDOWS /DIME_DLL_BUILD /I..\..\..\Imaginor\src ..\..\..\Handmade\src\handmade\handmade.cpp /DLL /Fe:Handmade.dll 

set solutiondir=W:\ImaginorEngine

for /f "tokens=2 delims==" %%a in ('wmic OS Get localdatetime /value') do set "dt=%%a"
set "YY=%dt:~2,2%" & set "YYYY=%dt:~0,4%" & set "MM=%dt:~4,2%" & set "DD=%dt:~6,2%"
set "HH=%dt:~8,2%" & set "Min=%dt:~10,2%" & set "Sec=%dt:~12,2%"

set "fullstamp=%YYYY%-%MM%-%DD%_%HH%-%Min%-%Sec%"

set outputdir=%solutiondir%\bin\windows\debug-x64\ImaginorEngine
set prjdir=%solutiondir%\ImaginorEngine
set windowssdk=/I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\um" /I"C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\winrt" /I"C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\cppwinrt" /I"C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\shared" /I"C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\ucrt" /I"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30133\include" /I"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30133\ATLMFC\include"
set srcfiles=%prjdir%\src\ImaginorSandbox.cpp %prjdir%\src\rendering\Renderer2D.cpp %prjdir%\src\memory.cpp %prjdir%\src\userinterface\Context.cpp %prjdir%\src\userinterface\Window.cpp %prjdir%\src\textrenderering.cpp
set include=/I%solutiondir% /I%prjdir% /I%solutiondir%\ImaginorMath\src /I%prjdir%\dep\include /I%prjdir%\dep\Entt\src\
set defines=/DIME_DEBUG /DIME_DLL_BUILD
set libs=
set compilorflags=/std:c++17 /Zi /MD /Fe:ImaginorEngine.dll /EHsc
set linkerflags=/link /EXPORT:applicationUpdate /EXPORT:applicationInit /DLL /MAP /PDB:ImaginorEngine%fullstamp%.pdb  

pushd %outputdir%

cl %compilorflags% %defines% %include% %windowssdk% %srcfiles% %libs% %linkerflags%

copy ImaginorEngine.dll ..\platform

popd
