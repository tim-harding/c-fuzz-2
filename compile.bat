@echo off
@pushd D:\19\11\c-fuzz-2\build

cl /Zi /EHsc /MT ^
	..\src\*.cpp ^
	..\libraries\glad\src\glad.obj ^
	D:\static\vcpkg\installed\x64-windows\lib\glfw3dll.lib ^
	D:\static\vcpkg\installed\x64-windows\lib\freetype.lib ^
	/I ..\libraries\glad\include ^
	/I D:\static\vcpkg\installed\x64-windows\include ^
	/I D:\static\c_libs\gb ^
	/Fe:.\fuzz.exe
@popd
