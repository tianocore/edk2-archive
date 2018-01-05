@echo off
echo %date%  %time%
echo.
setlocal EnableDelayedExpansion EnableExtensions

set exitCode=0
set BuildFlags=

set thisscript=%0

:: Optional arguments
:OptLoop

if /i "%~1"=="" goto Usage
if /i "%~1"=="/?" goto Usage

set BuildOption=%~1
if "!BuildOption:~0,1!"=="/" (
    set BuildFlags=%BuildFlags% %BuildOption%
    shift
    goto OptLoop
)

:: Required argument(s)
:: Require 2 input parameters , first parameter without a "/" is Platform Name
if "%~2"=="" goto Usage

:: Assign required arguments
set PlatformName=%~1
set BuildTarget=%~2

:OptLoopEnd
echo ---- Call Build Script of Broxton ----

if not exist Platform\%PlatformName%PlatformPkg\BuildIFWI.bat (
  echo Platform %PlatformName%PlatformPkg does not exist
  echo. & echo Error - Unsupported Platform name: %1 
  echo.
  goto Usage
)

echo calling : Platform\%PlatformName%PlatformPkg\BuildIFWI.bat  %BuildFlags%  /fspw MINN %BuildTarget% 
call Platform\%PlatformName%PlatformPkg\BuildIFWI.bat  %BuildFlags%  /fspw MINN %BuildTarget% 

goto Exit

:Usage
echo Usage: %thisscript% [options] ^<PlatformName^> ^<BuildTarget^>
echo.
echo Options:
echo.       /?    Display this help text
echo.
echo        /m     Set the build thread count to number of processors
echo        /vs13  Set Compiler to vs2013 build (default: vs2013)
echo        /x64   Set Arch to X64  (default: X64)
echo        /IA32  Set Arch to IA32 (default: X64)
echo        /A     Set FabId to A
echo        /B     Set FabId to B (default:  FAB_B)
echo        /D     Set FabId to D
echo        /MN    MinnowBoard 3(default: MN)
echo        /MX    MinnowBoard 3 Module
echo        /BG    Benson Glacier Board
echo        /AG    Aurora Glacier Board
echo        /LH    LeafHill CRB Board
echo        PlatformName:  Broxton
echo        BuildTargets:  Release, Debug

echo Examples:
echo    %thisscript% /vs13 /B /x64 Broxton Debug

set exitCode=1

:Exit
exit /b %exitCode%
