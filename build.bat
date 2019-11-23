@echo off
@pushd build

cl /Zi /EHsc /MD ^
	..\src\main.cpp ^
	..\libraries\glad\src\glad.c ^
	D:\static\vcpkg\installed\x64-windows\lib\glfw3dll.lib ^
	D:\static\vcpkg\installed\x64-windows\lib\freetype.lib ^
	/I ..\libraries\glad\include ^
	/I D:\static\vcpkg\installed\x64-windows\include
@popd