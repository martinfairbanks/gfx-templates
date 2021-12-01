@echo off

if not exist build mkdir build
pushd build

set cFlagsDebug= -nologo -Od -Oi -MT -FC -Z7 -Wall -wd4100 -I..\ext\include\
set cFlagsRelease=-nologo -O2 -fp:fast -MT -FC -Wall -wd4100 -I..\ext\include\ 
set lFlags= -incremental:no -opt:ref -subsystem:windows -LIBPATH:"..\ext\lib"

:: 64-bit build with statically linked CRT
cl %cFlagsDebug% ..\code\win32_ogl4_template.cpp -link %lFlags% 
cl %cFlagsDebug% ..\code\win32_ogl4_glew_template.cpp -link %lFlags% 
del *.obj

popd
