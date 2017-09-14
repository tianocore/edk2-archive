@echo off
echo %date%  %time%
echo.

set exitCode=0
set WORKSPACE=%CD%
set CORE_PATH=%CD%
set BuildFlags=
set PlatformName=
set BuildTarget=Debug
set Compiler=/vs13
set Arch=/x64
set FabId=/B
set BoardId=/MN

:: Optional arguments
:OptLoop

if /i "%~1"=="" goto Usage
if /i "%~1"=="/?" goto Usage

if /i "%~1"=="/IA32" (
    set Arch=/IA32
    shift
    goto OptLoop
)

if /i "%~1"=="/x64" (
    set Arch=/x64
    shift
    goto OptLoop
)

if /i "%~1"=="/vs08" (
    set Compiler=/vs08
    echo.
    shift
    goto OptLoop
)
if /i "%~1"=="/vs10" (
    set Compiler=/vs10
    echo.
    shift
    goto OptLoop
)
if /i "%~1"=="/vs12" (
    set Compiler=/vs12
    echo.
    shift
    goto OptLoop
)
if /i "%~1"=="/vs13" (
    set Compiler=/vs13
    echo.
    shift
    goto OptLoop
)
if /i "%~1"=="/vs15" (
    set Compiler=/vs15
    echo.
    shift
    goto OptLoop
)
if /i "%~1"=="/A" (
    set FabId=/A
    echo.
    shift
    goto OptLoop
)
if /i "%~1"=="/B" (
    set FabId=/B
    echo.
    shift
    goto OptLoop
)
if /i "%~1"=="/MN" (
    set BoardId=/MN
    echo.
    shift
    goto OptLoop
)
if /i "%~1"=="/BG" (
    set BoardId=/BG
    echo.
    shift
    goto OptLoop
)

:: Required argument(s)
:: Require 2 input parameters
if "%~2"=="" goto Usage

:: Assign required arguments
set PlatformName=%~1
set BuildTarget=%~2

:OptLoopEnd
echo ---- Call Build Script of Broxton ----
echo calling : Platform\%PlatformName%PlatformPkg\BuildIFWI.bat  %Compiler% %Arch% %BoardId% %FabId% /fspw %BuildFlags% MINN %BuildTarget% 
call Platform\%PlatformName%PlatformPkg\BuildIFWI.bat  %Compiler% %Arch% %BoardId% %FabId% /fspw %BuildFlags% MINN %BuildTarget% 

goto Exit

:Usage
echo Usage: BuildBIOS.bat [options] ^<PlatformName^> ^<BuildTarget^>
echo.
echo Options:
echo.       /?    Display this help text
echo.
echo        /vs13  Set Compiler to vs2013 build (default: vs2013)
echo        /x64   Set Arch to X64  (default: X64)
echo        /IA32  Set Arch to IA32 (default: X64)
echo        /A     Set FabId to A (default:  FAB_B)
echo        /B     Set FabId to B (default:  FAB_B)
echo        /MN    Minnow3 Board (default: MN)
echo        /BG    Benson Glacier Board
echo        PlatformName:  Broxton
echo        BuildTargets:  Release, Debug

echo Examples:
echo    BuildBIOS.bat /vs13 /B /x64 Broxton Debug

set exitCode=1

:Exit
exit /b %exitCode%
