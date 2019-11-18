@echo off
@pushd build

cl /Zi /EHsc /MD ^
	..\src\main.cpp ^
	..\libraries\glad\src\glad.c ^
	D:\static\vcpkg\installed\x64-windows\lib\glfw3dll.lib ^
	D:\static\vcpkg\installed\x64-windows\lib\imgui.lib ^
	..\libraries\imgui\examples\example_glfw_opengl3\Debug\imgui_impl_glfw.obj ^
	..\libraries\imgui\examples\example_glfw_opengl3\Debug\imgui_impl_opengl3.obj ^
	kernel32.lib ^
	opengl32.lib ^
	gdi32.lib ^
	shell32.lib ^
	/I ..\libraries\glad\include ^
	/I ..\libraries\imgui\examples ^
	/I D:\static\vcpkg\installed\x64-windows\include
@popd
