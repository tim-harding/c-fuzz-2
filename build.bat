@echo off
@pushd build
cl /Zi /EHsc ^
	..\src\main.cpp ^
	..\libraries\glad\src\glad.c ^
	D:\static\vcpkg\installed\x64-windows\lib\glfw3dll.lib ^
	/I ..\libraries\glad\include ^
	/I D:\static\vcpkg\installed\x64-windows\include
@popd
