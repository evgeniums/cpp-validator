@ECHO OFF

SET CMAKE_PATH=C:\Program Files\CMake\bin
SET MSVC_ROOT=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community

SET FMT_ROOT=%cd%\deps\fmt
SET BOOST_ROOT=%cd%\deps\root-msvc-14.2-x86_64

SET MSVC_COMPILER=v142
SET COMPILER_VERSION=14.2

SET MSVC_BUILD_ARCH=x64
SET MSVC_ARCH=x64
SET MSVC_TOOLSET="%MSVC_COMPILER%,host=x64"

CALL "%MSVC_ROOT%\VC\Auxiliary\Build\vcvarsall.bat" %MSVC_ARCH% -vcvars_ver=%COMPILER_VERSION%

SET CMAKE_PATH=%PATH%;%CMAKE_PATH%

SET SRC_DIR=%cd%\cpp-validator
SET INSTALL_DIR=%cd%\build\cpp-validator\install
SET BUILD_DIR=%cd%\build\cpp-validator\build

IF EXIST %BUILD_DIR% rmdir /Q /S %BUILD_DIR% 

SET BASE_DIR=%cd%

mkdir %BUILD_DIR%
cd %BUILD_DIR%

cmake -A %MSVC_BUILD_ARCH% -T %MSVC_TOOLSET% ^
    -DCMAKE_INSTALL_PREFIX=%INSTALL_DIR% ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DVALIDATOR_WITH_TESTS=On ^
    %SRC_DIR%

cmake --build . --target install --config Release -- /m:1 /p:UseMultiToolTask=true /p:MultiProcMaxCount=8 /fileLogger

cd %BASE_DIR%

SET PATH=%PATH%;%BOOST_ROOT%\lib
%BUILD_DIR%\test\Release\dracoshavalidator-test --log_level=test_suite
