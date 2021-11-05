@echo off
rem cl /LD /DIME_WINDOWS /DIME_DLL_BUILD /I..\..\..\Imaginor\src ..\..\..\Handmade\src\handmade\handmade.cpp /DLL /Fe:Handmade.dll 

set solutiondir=W:\ImaginorEngine


set outputdir=%solutiondir%\bin\windows\debug-x64\ImaginorGL
set prjdir=%solutiondir%\ImaginorGL
set windowssdk=/I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\um" /I"C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\winrt" /I"C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\cppwinrt" /I"C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\shared" /I"C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\ucrt" /I"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30133\include" /I"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30133\ATLMFC\include"
set srcfiles=%prjdir%\src\ImaginorGL.cpp
set include=/I%solutiondir% /I%prjdir% /I%solutiondir%\ImaginorMath\src
set defines=/DIME_DEBUG /DIME_DLL_BUILD
set libs=
set compilorflags=/O2 /std:c++17 /Fe:ImaginorGL.dll 
set exports=/EXPORT:ime_glapi_create_vbo /EXPORT:ime_glapi_init /EXPORT:ime_glapi_bind_vbo /EXPORT:ime_glapi_draw_array
set linkerflags=/link /DLL /MAP

pushd %outputdir%

cl %compilorflags% %defines% %include% %windowssdk% %srcfiles% %libs% %linkerflags% %exports%

copy ImaginorGL.dll ..\platform

popd
