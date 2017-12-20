@echo off
SetLocal EnableDelayedExpansion EnableExtensions

:: Set script defaults
set exitCode=0

copy /y nul Stitching.log >nul

:: Set tool env
set WORKSPACE=%CD%
set CORE_TOOLS_PATH=%WORKSPACE%\..\..\..\..\..\BaseTools\Bin\Win32
if %WORKSPACE:~-1%==\ set WORKSPACE=%WORKSPACE:~0,-1%
PATH=%PATH%;%WORKSPACE%\Tools;%WORKSPACE%\Tools\VLV_Merge_Tools;%WORKSPACE%\;%CORE_TOOLS_PATH%

:: Set default Suffix as:  YYYY_MM_DD_HHMM
set hour=%time: =0%
reg copy "HKCU\Control Panel\International" "HKCU\Control Panel\International_Temp" /f >nul
reg add "HKCU\Control Panel\International" /v sShortDate /d "yyyy_MM_dd" /f >nul
for /f "tokens=1" %%i in ("%date%") do set today=%%i
reg copy "HKCU\Control Panel\International_Temp" "HKCU\Control Panel\International" /f >nul
reg delete "HKCU\Control Panel\International_Temp" /f >nul
set IFWI_Suffix=%today%_%hour:~0,2%%time:~3,2%

:: **********************************************************************
:: Process inputs
:: **********************************************************************
if "%~1"=="?"       goto Usage
if "%~1"=="/?"      goto Usage
if /i "%~1"=="Help" goto Usage

set FspWrapper=FALSE
set FabId=B
set BoardId=MN

if /i "%~2"=="B" (
    set FabId=B
)

if /i "%~2"=="A" (
    set FabId=A
)

if /i "%~3"=="MN" (
    set BoardId=MN
)

if /i "%~3"=="BG" (
    set BoardId=BG
)

if /i "%~3"=="MX" (
    set BoardId=MX
)

if /i "%~3"=="LH" (
    set BoardId=LH
)

:OptLoop1

if /i "%~1"=="/FspW" (
    echo FspW enabled..
    set FspWrapper=TRUE
    shift
    goto OptLoop1
)

::Require first parameter
if not exist %~1 echo BIOS [%~1] not found. & goto Usage
set BIOS_Names=%~1
shift

:: **********************************************************************
:: Stitching Init
:: **********************************************************************
echo %date%  %time% >>Stitching.log 2>&1
echo %date%  %time%
echo.
for %%i in (%BIOS_Names%) do (
    set BIOS_Name=%%~ni
)
set BIOS_Version=!BIOS_Name:~-7,7!

REM extract PlatformType from BIOS filename
set Platform_Type=!BIOS_Name:~0,4!

REM search BIOS_Name for Arch substring:  either IA32 or X64
if not "!BIOS_Name!"=="!BIOS_Name:_IA32_=!" (
    set Arch=IA32
) else if not "!BIOS_Name!"=="!BIOS_Name:_X64_=!" (
    set Arch=X64
) else (
    echo Error:  Could not determine Architecture for !BIOS_Name!
    goto Usage
)

set IFWI_Prefix=!Platform_Type!_IFWI_FSPW_!Arch!

REM search BIOS_Name for Build_Target substring: either R or D
if not "!BIOS_Name!"=="!BIOS_Name:_R_=!" (
    set Build_Target=Release
    set IFWI_Prefix=!IFWI_Prefix!_R
    set IFWI_Version=!IFWI_Prefix!_%IFWI_Suffix%
) else if not "!BIOS_Name!"=="!BIOS_Name:_D_=!" (
    set Build_Target=Debug
    set IFWI_Prefix=!IFWI_Prefix!_D
    set IFWI_Version=!IFWI_Prefix!_%IFWI_Suffix%
) else (
    echo Error:  Could not determine Build Target for !BIOS_Name!
    goto Usage
)

if /i "!Platform_Type!"=="MINN" (
    set Platform_Type=MINN
) else if /i "!Platform_Type!"=="BENS" (
    set Platform_Type=BENS
) else if /i "!Platform_Type!"=="M3MO" (
    set Platform_Type=M3MO
) else if /i "!Platform_Type!"=="LEAF" (
    set Platform_Type=LEAF
) else (
    echo Error - Unsupported PlatformType: !Platform_Type!
    goto Usage
)

:: rd /s /q %BIOS_Names%  >>Stitching.log

pushd %BIOS_Names%
copy /y /b FVIBBL.Fv IBBL.Fv
copy /y /b FVIBBM.fv+FSP_M.fv IBB.Fv
copy /y /b FSP_S.fv+FVIBBR.fv+FVOBB.Fv+FVOBBX.Fv+FVOBBY.Fv OBB.Fv
popd

set IFWI_Name=!IFWI_Prefix!_%IFWI_Suffix%

echo.
echo ------------------------------------------
echo.
echo   Generating SPI Image...
mkdir BIOS_COMPONENTS
copy /y /b %BIOS_Names%\IBBL.Fv .\BIOS_COMPONENTS
copy /y /b %BIOS_Names%\IBB.Fv .\BIOS_COMPONENTS
copy /y /b %BIOS_Names%\OBB.Fv .\BIOS_COMPONENTS
copy /y /b %BIOS_Names%\NvStorage.Fv .\BIOS_COMPONENTS

if %BoardId%==BG (
  if %FabId%==B (
    copy /y /b ..\..\..\Board\BensonGlacier\IFWI\FAB_B\SpiChunk1.bin .
    copy /y /b ..\..\..\Board\BensonGlacier\IFWI\FAB_B\SpiChunk2.bin .
    copy /y /b ..\..\..\Board\BensonGlacier\IFWI\FAB_B\SpiChunk3.bin .
  ) else (
    copy /y /b ..\..\..\Board\BensonGlacier\IFWI\FAB_A\SpiChunk1.bin .
    copy /y /b ..\..\..\Board\BensonGlacier\IFWI\FAB_A\SpiChunk2.bin .
    copy /y /b ..\..\..\Board\BensonGlacier\IFWI\FAB_A\SpiChunk3.bin .  
  )  
  copy /y /b SpiChunk1.bin+.\BIOS_COMPONENTS\IBBL.Fv+.\BIOS_COMPONENTS\IBB.Fv+SpiChunk2.bin+.\BIOS_COMPONENTS\OBB.Fv+.\BIOS_COMPONENTS\NvStorage.Fv+SpiChunk3.bin spi_out.bin
) else if %BoardId%==MN (
         if %FabId%==B (
           copy /y /b ..\..\..\Board\MinnowBoard3\IFWI\FAB_B\SpiChunk1.bin .
           copy /y /b ..\..\..\Board\MinnowBoard3\IFWI\FAB_B\SpiChunk2.bin .
           copy /y /b ..\..\..\Board\MinnowBoard3\IFWI\FAB_B\SpiChunk3.bin .
           copy /y /b SpiChunk1.bin+.\BIOS_COMPONENTS\IBBL.Fv+.\BIOS_COMPONENTS\IBB.Fv+SpiChunk2.bin+.\BIOS_COMPONENTS\OBB.Fv+.\BIOS_COMPONENTS\NvStorage.Fv+SpiChunk3.bin spi_out.bin
         ) else (
           copy /y /b ..\..\..\Board\MinnowBoard3\IFWI\FAB_A\SpiChunk1.bin .
           copy /y /b ..\..\..\Board\MinnowBoard3\IFWI\FAB_A\SpiChunk2.bin .
           copy /y /b ..\..\..\Board\MinnowBoard3\IFWI\FAB_A\SpiChunk3.bin .
           copy /y /b SpiChunk1.bin+.\BIOS_COMPONENTS\IBBL.Fv+.\BIOS_COMPONENTS\IBB.Fv+SpiChunk2.bin+.\BIOS_COMPONENTS\OBB.Fv+.\BIOS_COMPONENTS\NvStorage.Fv+SpiChunk3.bin spi_out.bin
         )
) else if %BoardId%==MX (
           copy /y /b ..\..\..\Board\MinnowBoard3Next\IFWI\FAB_A\SpiChunk1.bin .
           copy /y /b ..\..\..\Board\MinnowBoard3Next\IFWI\FAB_A\SpiChunk2.bin .
           copy /y /b ..\..\..\Board\MinnowBoard3Next\IFWI\FAB_A\SpiChunk3.bin .
           copy /y /b SpiChunk1.bin+.\BIOS_COMPONENTS\IBBL.Fv+.\BIOS_COMPONENTS\IBB.Fv+SpiChunk2.bin+.\BIOS_COMPONENTS\OBB.Fv+.\BIOS_COMPONENTS\NvStorage.Fv+SpiChunk3.bin spi_out.bin
) else if %BoardId%==LH (
           copy /y /b ..\..\..\Board\LeafHill\IFWI\FAB_D\SpiChunk1.bin .
           copy /y /b ..\..\..\Board\LeafHill\IFWI\FAB_D\SpiChunk2.bin .
           copy /y /b ..\..\..\Board\LeafHill\IFWI\FAB_D\SpiChunk3.bin .
           copy /y /b SpiChunk1.bin+.\BIOS_COMPONENTS\IBBL.Fv+.\BIOS_COMPONENTS\IBB.Fv+SpiChunk2.bin+.\BIOS_COMPONENTS\OBB.Fv+.\BIOS_COMPONENTS\NvStorage.Fv+SpiChunk3.bin spi_out.bin
)
move /y spi_out.bin %BIOS_ID%.bin  >> Stitching.log

if !ERRORLEVEL! NEQ 0 (
  echo. & echo  -- Error during stitching ^^!  >>Stitching.log
  goto ScriptFail
)

echo Stitched successfully      >>Stitching.log
echo  ***  Stitched successfully  ***
echo.
echo  ----------  IFWI_Stitch Complete  ----------
goto Exit


:Usage
echo.
echo **************************************************************************************************
echo.
echo Usage: IFWIStitch_Simple.bat <BIOS Name>
echo.
echo ****************************************************************************************************
exit /b 1

:ScriptFail
set exitCode=1
copy Stitching.log ..\Stitching.log
echo.

:Exit
echo.
echo  --- See  'Stitching.log'  for more info. ---
echo.
echo  %date%  %time%
echo.
exit /b %exitCode%
EndLocal
