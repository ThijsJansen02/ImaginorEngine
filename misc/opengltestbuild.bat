@echo off 

set solutiondir=W:\ImaginorEngine


set outputdir=%solutiondir%\bin\windows\debug-x64\platform
set prjdir=%solutiondir%\ImaginorPlatform
set windowssdk=/I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\um" /I"C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\winrt" /I"C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\cppwinrt" /I"C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\shared" /I"C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\ucrt" /I"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30133\include" /I"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30133\ATLMFC\include"
set srcfiles=%prjdir%\src\openGLimpl\opengltest.cpp
set include=/I%solutiondir% /I%prjdir% /I%prjdir%\dep
set defines=/DIME_WINDOWS /DIME_DEBUG
set libs=Gdi32.lib User32.lib Winmm.lib opengl32.lib
set compilerflags=/Zi /MTd /std:c++17

pushd %outputdir%

cl %compilerflags% %defines% %include% %windowssdk% %srcfiles% %libs%
call opengltest.exe


popd

