@echo off
rem cl /LD /DIME_WINDOWS /DIME_DLL_BUILD /I..\..\..\Imaginor\src ..\..\..\Handmade\src\handmade\handmade.cpp /DLL /Fe:Handmade.dll 
rem %prjdir%\dep\glew-2.1.0\lib\Release\x64\glew32s.lib

set solutiondir=W:\ImaginorEngine


set outputdir=%solutiondir%\bin\windows\debug-x64\platform
set prjdir=%solutiondir%\ImaginorPlatform
set windowssdk=/I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\um" /I"C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\winrt" /I"C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\cppwinrt" /I"C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\shared" /I"C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\ucrt" /I"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30133\include" /I"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30133\ATLMFC\include"
set srcfiles=%prjdir%\src\win32_platform.cpp %prjdir%\src\openGLimpl\win32_openGLimpl.cpp %prjdir%\src\win32_window.cpp %prjdir%\src\win32_debugfileIO.cpp %prjdir%\src\win32_window.cpp %prjdir%\src\win32_consolethread.cpp
set include=/I%solutiondir% /I%prjdir% /I%prjdir%\dep
set defines=/DIME_WINDOWS /DIME_DEBUG /DGLEW_STATIC
set libs=Gdi32.lib User32.lib Winmm.lib opengl32.lib
set compilerflags=/Zi /MTd /std:c++17

pushd %outputdir%

cl %compilerflags% %defines% %include% %windowssdk% %srcfiles% %libs%

popd

