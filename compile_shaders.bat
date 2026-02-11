@echo off
setlocal

cd /d "%~dp0"

if "%VULKAN_SDK%"=="" (
    echo [Error] Vulkan SDK environment variable not found. 
    echo Please make sure Vulkan SDK is installed correctly.
    pause
    exit /b
)

echo Starting to compile shaders...

"%VULKAN_SDK%\Bin\glslc.exe" shader.vert -o vert.spv
if %ERRORLEVEL% NEQ 0 (echo [Error] Vertex shader compilation failed! & pause & exit /b)

"%VULKAN_SDK%\Bin\glslc.exe" shader.frag -o frag.spv
if %ERRORLEVEL% NEQ 0 (echo [Error] Fragment shader compilation failed! & pause & exit /b)

echo [Success] Shaders compiled successfully to .spv files.
