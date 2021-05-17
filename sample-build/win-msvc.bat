@ECHO OFF

SET CMAKE_PATH=C:\Program Files\CMake\bin
SET MSVC_ROOT=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community

SET FMT_ROOT=%cd%\deps\fmt
SET BOOST_ROOT=%cd%\deps\root-msvc-14.2-x86_64

SET BUILD_TYPE=Release

SET MSVC_COMPILER=v142
SET COMPILER_VERSION=14.2

SET MSVC_BUILD_ARCH=x64
SET MSVC_ARCH=x64
SET MSVC_TOOLSET="%MSVC_COMPILER%,host=x64"

CALL "%MSVC_ROOT%\VC\Auxiliary\Build\vcvarsall.bat" %MSVC_ARCH% -vcvars_ver=%COMPILER_VERSION%

SET PATH=%PATH%;%CMAKE_PATH%

SET SRC_DIR=%cd%\cpp-validator
SET INSTALL_DIR=%cd%\build\cpp-validator\install
SET BUILD_DIR=%cd%\build\cpp-validator\build

IF EXIST %BUILD_DIR% rmdir /Q /S %BUILD_DIR%

SET BASE_DIR=%cd%

mkdir %BUILD_DIR%
cd %BUILD_DIR%

cmake -A %MSVC_BUILD_ARCH% -T %MSVC_TOOLSET% ^
    -DCMAKE_INSTALL_PREFIX=%INSTALL_DIR% ^
    -DVALIDATOR_WITH_TESTS=On ^
    -DVALIDATOR_WITH_EXAMPLES=On ^
    %SRC_DIR%

cmake --build . --config %BUILD_TYPE% -- /m:1 /p:UseMultiToolTask=true /p:MultiProcMaxCount=8 /fileLogger

SET PATH=%PATH%;%BOOST_ROOT%\lib
ctest -VV -C %BUILD_TYPE%

cd %BASE_DIR%
