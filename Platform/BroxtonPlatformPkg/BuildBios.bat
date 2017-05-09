@echo off
::echo on
setlocal EnableDelayedExpansion EnableExtensions
echo.
echo %date%  %time%
echo.

::**********************************************************************
:: Initial Setup
::**********************************************************************
set WORKSPACE=%CD%
if %WORKSPACE:~-1%==\ (
  set WORKSPACE=%WORKSPACE:~0,-1%
)
set CORE_PATH=%WORKSPACE%\Core
set PLATFORM_PATH=Platform\BroxtonPlatformPkg
set SILICON_PATH=Silicon\BroxtonSoC
set AslPath=%WORKSPACE%\%PLATFORM_PATH%\Common\Tools\Iasl\iasl.exe
set PACKAGES_PATH=%CORE_PATH%;%WORKSPACE%\Silicon\;%WORKSPACE%\Platform;%WORKSPACE%\%PLATFORM_PATH%;%WORKSPACE%\%SILICON_PATH%;%WORKSPACE%\%PLATFORM_PATH%\Common ;
set EDK_TOOLS_BIN=%WORKSPACE%\BaseTools\Bin\Win32
set /a build_threads=1
set "Nasm_Flags=-D ARCH_IA32 -D DEBUG_PORT80"
set "Build_Flags= "
set exitCode=0
set Arch=X64
set Compiler=VS2013
set Stepping=B
if not defined BiosVersion set BiosVersion=DEV


:: Clean up previous build files.
echo Clean up old build files...
if exist %WORKSPACE%\edk2.log                  del %WORKSPACE%\edk2.log
if exist %WORKSPACE%\unitool.log               del %WORKSPACE%\unitool.log
if exist %WORKSPACE%\Conf\target.txt           del %WORKSPACE%\Conf\target.txt
if exist %WORKSPACE%\Conf\tools_def.txt        del %WORKSPACE%\Conf\tools_def.txt
if exist %WORKSPACE%\Conf\build_rule.txt       del %WORKSPACE%\Conf\build_rule.txt
if exist %WORKSPACE%\Conf\FrameworkDatabase.db del %WORKSPACE%\Conf\FrameworkDatabase.db
if exist %WORKSPACE%\Stitch\*.bin              del %WORKSPACE%\Stitch\*.bin
if exist conf\.cache rmdir /q/s conf\.cache


:: Override tools_def.txt
echo Creating Conf folder and build config files...
if not exist %WORKSPACE%\Conf md %WORKSPACE%\Conf
copy /y %CORE_PATH%\BaseTools\Conf\*.template %WORKSPACE%\Conf\*.txt
copy /y %WORKSPACE%\%PLATFORM_PATH%\DeviceCommonPkg\Override\BaseTools\Conf\tools_def.template %WORKSPACE%\Conf\tools_def.txt

:: Setup EDK environment. Edksetup puts new copies of target.txt, tools_def.txt, build_rule.txt in WorkSpace\Conf
:: Also run edksetup as soon as possible to avoid it from changing environment variables we're overriding
set "VCINSTALLDIR="
set EDK_TOOLS_PATH=%WORKSPACE%\BaseTools
call edksetup.bat
@echo off

set Minnow_RVP=MINN

:: Initialize all the build flags to FALSE
:: Then based on the cmd line input, some will be set to TRUE prior to building.
:: Note: These variable names match the actual Build Macro names in DSC for clarity.
set ENBDT_PF_BUILD=FALSE
set SrcDebug=FALSE
set APLK_SETUP_ENABLE_BUILD=FALSE
set FSP_BUILD=FALSE
set FSP_WRAPPER=FALSE

:: Set Defaults of platform specific environment variables.
set EFI_SOURCE=%CD%
set EDK_SOURCE=%CD%
set PLATFORM_NAME=BxtPlatformPkg
set PLATFORM_PACKAGE=%PLATFORM_PATH%\BxtPlatformPkg
set PLATFORM_RC_PACKAGE=Silicon\BroxtonSoC\BroxtonSiPkg
set COMMON_PLATFORM_PACKAGE=%PLATFORM_PATH%\BxtPlatformPkg
set FSP_BIN_PKG_NAME=BroxtonFspBinPkg
set STITCH_PATH=%WORKSPACE%\%PLATFORM_PATH%\Common\Tools\Stitch

PATH=%PATH%;%WORKSPACE%\%PLATFORM_PATH%\Common\Tools\GenBiosId;%WORKSPACE%\%PLATFORM_PATH%\Common\Tools\nasm\Win32
PATH=%PATH%;%WORKSPACE%\%PLATFORM_PATH%\Common\Tools\FCE;%WORKSPACE%\%PLATFORM_PATH%\Common\Tools\nasm\Win32

::**********************************************************************
:: Parse command line arguments
::**********************************************************************

:: Optional arguments
:OptLoop
if /i "%~1"=="" goto Usage
if /i "%~1"=="/?" goto Usage

if /i "%~1"=="/l" (
    set Build_Flags=%Build_Flags% -j EDK2.log
    shift
    goto OptLoop
)
if /i "%~1" == "/c" (
    echo Removing previous Build files...
    if exist build (
        del /f/s/q build > nul
        rmdir /s/q build
    )
    if exist Conf\.cache (
        del /f/s/q Conf\.cache > nul
        rmdir /s/q Conf\.cache
    )
    echo.
    shift
    goto OptLoop
)

if /i "%~1"=="/ia32" (
    set Arch=IA32
    shift
    goto OptLoop
)
if /i "%~1"=="/x64" (
    set Arch=X64
    shift
    goto OptLoop
)

if /i "%~1"=="/vs08" (
    set Compiler=VS2008
    echo.
    shift
    goto OptLoop
)
if /i "%~1"=="/vs10" (
    set Compiler=VS2010
    echo.
    shift
    goto OptLoop
)
if /i "%~1"=="/vs12" (
    set Compiler=VS2012
    echo.
    shift
    goto OptLoop
)
if /i "%~1"=="/vs13" (
    set Compiler=VS2013
    echo.
    shift
    goto OptLoop
)
if /i "%~1"=="/vs15" (
    set Compiler=VS2015
    echo.
    shift
    goto OptLoop
)
if /i "%~1"=="/FspW" (
    set FSP_WRAPPER=TRUE
    echo.
    shift
    goto OptLoop
)
if /i "%~1"=="/A" (
    set Stepping=A
    echo.
    shift
    goto OptLoop
)
if /i "%~1"=="/B" (
    set Stepping=B
    echo.
    shift
    goto OptLoop
)

:: Required argument(s)
if "%~2"=="" (
   echo. & echo -- ERROR: Not Enough Arguments Provided
   echo -- Please review the Help screen "/?" -- & echo.
   goto ExitFail
)

echo. & echo -- Setting compiler to %Compiler% -- & echo.

:: BOARD_ID needs to be exactly 7 characters (GenBiosId.exe limitation)
echo Setting  %1  platform configuration and BIOS ID...
if /i "%~1" == "%Minnow_RVP%" (
    set BOARD_ID=MINNOWV
    set ENBDT_PF_BUILD=TRUE
    set PLATFORM_NAME=BroxtonPlatformPkg
    set PLATFORM_PACKAGE=%PLATFORM_PATH%
    set APLK_SETUP_ENABLE_BUILD=TRUE
) else (
    echo. & echo Error - Unsupported PlatformType: %1
    echo Please review the Help screen "/?" & echo.
    goto ExitFail
)
set Platform_Type=%~1

::
:: Tack on BIOS Version
::

::**********************************************************************
:: Additional EDK Build Setup/Configuration
::**********************************************************************

::Setup DefineAtBuildMacros config file
set Build_Macros=.\%PLATFORM_PACKAGE%\DefineAtBuildMacros.dsc
copy /y nul %Build_Macros% >nul

::output platform specific build macros to DefineAtBuildMacros.dsc
echo DEFINE ENBDT_PF_BUILD          = %ENBDT_PF_BUILD%          >> %Build_Macros%
echo DEFINE SOURCE_DEBUG_ENABLE     = %SrcDebug%                >> %Build_Macros%
echo DEFINE APLK_SETUP_ENABLE_BUILD = %APLK_SETUP_ENABLE_BUILD% >> %Build_Macros%

if "%Arch%"=="IA32" (
    echo DEFINE X64_CONFIG              = FALSE                     >> %Build_Macros%
) else if "%Arch%"=="X64" (
    echo DEFINE X64_CONFIG              = TRUE                      >> %Build_Macros%
)

::Stage of copy of BiosId.env in Conf/ with Platform_Type and Build_Target values removed

if "%Arch%"=="X64" (
    findstr /b /v "BOARD_ID BUILD_TYPE" %PLATFORM_PACKAGE%\BiosId.env > Conf\BiosId.env
) else if "%Arch%"=="IA32" (
    findstr /b /v "BOARD_ID BUILD_TYPE OEM_ID" %PLATFORM_PACKAGE%\BiosId.env > Conf\BiosId.env
    echo OEM_ID = I32 >> Conf\BiosId.env
)

if /i "%~2" == "RELEASE" (
    set target=RELEASE
    echo BUILD_TYPE = R >> Conf\BiosId.env
) else (
    set target=DEBUG
    echo BUILD_TYPE = D >> Conf\BiosId.env
)

:: Set the Build_Type, Version_Major, and Version_Minor environment variables
find /v "#" Conf\BiosId.env > ver_strings
for /f "tokens=1,3" %%i in (ver_strings) do set %%i=%%j
del /f/q ver_strings >nul
:: Do this after parsing "ver_strings"
echo BOARD_ID = %BOARD_ID% >> Conf\BiosId.env

echo.
echo Setting the Build environment for Visual Studio...
if "%Compiler%"=="VS2008" (
  if defined VS90COMNTOOLS (
     call "%VS90COMNTOOLS%\vsvars32.bat" >nul
     if /I "%VS90COMNTOOLS%" == "C:\Program Files\Microsoft Visual Studio 9.0\Common7\Tools\" (
      set TOOL_CHAIN_TAG=VS2008
     ) else  set TOOL_CHAIN_TAG=VS2008x86
  ) else (
    echo  --ERROR: VS2008 not installed correctly. VS90COMNTOOLS not defined ^^!
    goto BldFail
  )
) else if "%Compiler%"=="VS2010" (
  if defined VS100COMNTOOLS (
     call "%VS100COMNTOOLS%\vsvars32.bat" >nul
     if /I "%VS100COMNTOOLS%" == "C:\Program Files\Microsoft Visual Studio 10.0\Common7\Tools\" (
        set TOOL_CHAIN_TAG=VS2010
     ) else  set TOOL_CHAIN_TAG=VS2010x86
  ) else (
     echo  --ERROR: VS2010 not installed correctly. VS100COMNTOOLS not defined ^^!
     goto BldFail
  )
) else if "%Compiler%"=="VS2012" (
  if defined VS110COMNTOOLS (
     call "%VS110COMNTOOLS%\vsvars32.bat" >nul
     if /I "%VS110COMNTOOLS%" == "C:\Program Files\Microsoft Visual Studio 11.0\Common7\Tools\" (
        set TOOL_CHAIN_TAG=VS2012
     ) else  set TOOL_CHAIN_TAG=VS2012x86
  ) else (
     echo  --ERROR: VS2012 not installed correctly. VS110COMNTOOLS not defined ^^!
     goto BldFail
  )
) else if "%Compiler%"=="VS2013" (
  if defined VS120COMNTOOLS (
     call "%VS120COMNTOOLS%\vsvars32.bat" >nul
     if /I "%VS120COMNTOOLS%" == "C:\Program Files\Microsoft Visual Studio 12.0\Common7\Tools\" (
        set TOOL_CHAIN_TAG=VS2013
     ) else  set TOOL_CHAIN_TAG=VS2013x86
  ) else (
     echo  --ERROR: VS2013 not installed correctly. VS120COMNTOOLS not defined ^^!
     echo           See help ^(/?^) for how specify a different VS version.
     goto BldFail
  )
) else if "%Compiler%"=="VS2015" (
  if defined VS140COMNTOOLS (
     call "%VS140COMNTOOLS%\vsvars32.bat" >nul
     if /I "%VS140COMNTOOLS%" == "C:\Program Files\Microsoft Visual Studio 14.0\Common7\Tools\" (
        set TOOL_CHAIN_TAG=VS2015
     ) else  set TOOL_CHAIN_TAG=VS2015x86

  ) else (
     echo  --ERROR: VS2015 not installed correctly. VS140COMNTOOLS not defined ^^!
     echo           See help ^(/?^) for how specify a different VS version.
     goto BldFail
  )
)

::**********************************************************************
:: Build BIOS
::**********************************************************************

echo Setup build directory for GenBiosId...
set BUILD_PATH=Build\%PLATFORM_NAME%\%TARGET%_%TOOL_CHAIN_TAG%
if not exist %BUILD_PATH%\IA32  mkdir %BUILD_PATH%\IA32

echo Creating BiosId...
GenBiosId.exe -i Conf\BiosId.env -o %BUILD_PATH%\IA32\BiosId.bin -ob Conf\BiosId.bat
if "%Arch%"=="X64" (
   if not exist %BUILD_PATH%\X64  mkdir %BUILD_PATH%\X64
   GenBiosId.exe -i Conf\BiosId.env -o %BUILD_PATH%\X64\BiosId.bin -ob Conf\BiosId.bat
) else if "%Arch%"=="IA32" (
   if not exist %BUILD_PATH%\IA32  mkdir %BUILD_PATH%\IA32
   GenBiosId.exe -i Conf\BiosId.env -o %BUILD_PATH%\IA32\BiosId.bin -ob Conf\BiosId.bat
)

if ErrorLevel 1 goto BldFail

echo Building ResetVector...

set ResetVectorPath=%WORKSPACE%\%PLATFORM_RC_PACKAGE%\Cpu\ResetVector

pushd %ResetVectorPath%\Vtf0
  nasm.exe %Nasm_Flags% -o Bin\ResetVector.ia32.port80.raw ResetVectorCode.asm
  python %CORE_PATH%\UefiCpuPkg\ResetVector\Vtf0\Tools\FixupForRawSection.py Bin\ResetVector.ia32.port80.raw
popd

pushd %ResetVectorPath%\Vtf1
  nasm.exe %Nasm_Flags% -o Bin\ResetVector.ia32.port80.raw ResetVectorCode.asm
  python %CORE_PATH%\UefiCpuPkg\ResetVector\Vtf0\Tools\FixupForRawSection.py Bin\ResetVector.ia32.port80.raw
popd

:: SaveWorkSpace w/a is needed when using subst for Workspace builds (eg. R:/)
set SaveWorkSpace=%WORKSPACE%
set WORKSPACE=%CD%
if "%Arch%"=="X64" (
set ActivePlatform=%PLATFORM_PACKAGE%\PlatformPkgX64.dsc
) else if "%Arch%"=="IA32" (
set ActivePlatform=%PLATFORM_PACKAGE%\PlatformPkgIA32.dsc
)

echo Modifying Conf files for this build...
:: Remove lines with these tags from target.txt
findstr /V "TARGET  TARGET_ARCH  TOOL_CHAIN_TAG  ACTIVE_PLATFORM  MAX_CONCURRENT_THREAD_NUMBER" Conf\target.txt > Conf\target.txt.tmp

if "%Arch%"=="IA32" (
    echo TARGET_ARCH = IA32                                  >> Conf\target.txt.tmp
) else if "%Arch%"=="X64" (
    echo TARGET_ARCH = IA32 X64                              >> Conf\target.txt.tmp
)
echo TARGET          = %TARGET%                              >> Conf\target.txt.tmp
echo TOOL_CHAIN_TAG  = %TOOL_CHAIN_TAG%                      >> Conf\target.txt.tmp
echo ACTIVE_PLATFORM = %ActivePlatform%                      >> Conf\target.txt.tmp
echo MAX_CONCURRENT_THREAD_NUMBER = %build_threads%          >> Conf\target.txt.tmp

move /Y Conf\target.txt.tmp Conf\target.txt >nul
echo *_VS2008x86_*_ASL_PATH = %AslPath% >> Conf\tools_def.txt
echo *_VS2010x86_*_ASL_PATH = %AslPath% >> Conf\tools_def.txt
echo *_VS2012x86_*_ASL_PATH = %AslPath% >> Conf\tools_def.txt
echo *_VS2013x86_*_ASL_PATH = %AslPath% >> Conf\tools_def.txt
echo *_VS2015x86_*_ASL_PATH = %AslPath% >> Conf\tools_def.txt

echo.
echo Invoking normal EDK2 build...
build %Build_Flags%
if ErrorLevel 1 goto BldFail

set WORKSPACE=%SaveWorkSpace%

::**********************************************************************
:: Post Build processing and cleanup
::**********************************************************************
echo Create FlashMap.h...

set PLAT_INIT_PRE_MEM=PlatformPreMemPei

set AutoGenPath=%BUILD_PATH%\IA32\BroxtonPlatformPkg\Common\PlatformSettings\PlatformPreMemPei\%PLAT_INIT_PRE_MEM%\DEBUG\AutoGen.h

if not exist "%AutoGenPath%" (
  echo ERROR: Couldn't find %AutoGenPath% so we can generate FlashMap.h
  goto BldFail
)
findstr /L "_PCD_VALUE_" %AutoGenPath% > %STITCH_PATH%\FlashMap.h

echo Running FCE...
:: Extract Hii data from build and store a copy in HiiDefaultData.txt
fce.exe read -i %BUILD_PATH%\FV\Soc.fd > %BUILD_PATH%\FV\HiiDefaultData.txt 2>>EDK2.log
:: Generate the Setup variable and save changes to BxtXXX.fd
fce.exe update -i %BUILD_PATH%\FV\Soc.fd -s %BUILD_PATH%\FV\HiiDefaultData.txt -o %BUILD_PATH%\FV\Bxt%Arch%.fd  1>>EDK2.log 2>&1

if ErrorLevel 1 goto BldFail

@echo off

:: Translate Release Build Type
if "%BUILD_TYPE%"=="R" set BUILD_TYPE=R

echo Copy BIOS...
set BIOS_Name=%BOARD_ID%_%Arch%_%BUILD_TYPE%_%VERSION_MAJOR%_%VERSION_MINOR%
copy /y/b %BUILD_PATH%\FV\Bxt%Arch%.fd  %STITCH_PATH%\%BIOS_Name%.ROM >nul
copy /y   %STITCH_PATH%\FlashMap.h        %STITCH_PATH%\%BIOS_Name%.map >nul

set Storage_Folder=%STITCH_PATH%\%BIOS_Name%
if not exist %Storage_Folder%  mkdir %Storage_Folder%

copy /y/b %BUILD_PATH%\FV\FvIBBL.fv  %Storage_Folder% >nul
copy /y/b %BUILD_PATH%\FV\FvIBBM.fv  %Storage_Folder% >nul
copy /y/b %BUILD_PATH%\FV\FvIBBR.fv  %Storage_Folder% >nul
copy /y/b %BUILD_PATH%\FV\FvOBB.fv   %Storage_Folder% >nul
copy /y/b %BUILD_PATH%\FV\FvOBBX.fv  %Storage_Folder% >nul
copy /y/b %BUILD_PATH%\FV\FvOBBY.fv  %Storage_Folder% >nul

if /i "%FSP_WRAPPER%" == "TRUE" (
  if %Stepping%==B (
::  0xFEF7A000 = gIntelFsp2WrapperTokenSpaceGuid.PcdFlashFvFspBase = $(CAR_BASE_ADDRESS) + $(BLD_RAM_DATA_SIZE) + $(FSP_RAM_DATA_SIZE) + $(FSP_EMP_DATA_SIZE) + $(BLD_IBBM_SIZE)
    pushd  %WORKSPACE%\Silicon\BroxtonSoC\BroxtonFspPkg\ApolloLakeFspBinPkg\FspBin
    python %WORKSPACE%\Core\IntelFsp2Pkg\Tools\SplitFspBin.py rebase -f ApolloLakeFsp.fd -c m -b 0xFEF7A000 -o .\ -n FSP.fd
    python %WORKSPACE%\Core\IntelFsp2Pkg\Tools\SplitFspBin.py split -f FSP.fd -o .\ -n FSP.Fv
    popd
    copy /y/b %WORKSPACE%\Silicon\BroxtonSoC\BroxtonFspPkg\ApolloLakeFspBinPkg\FspBin\FSP_T.Fv %Storage_Folder%\FSP_T.Fv
    copy /y/b %WORKSPACE%\Silicon\BroxtonSoC\BroxtonFspPkg\ApolloLakeFspBinPkg\FspBin\FSP_M.Fv %Storage_Folder%\FSP_M.Fv
    copy /y/b %WORKSPACE%\Silicon\BroxtonSoC\BroxtonFspPkg\ApolloLakeFspBinPkg\FspBin\FSP_S.Fv %Storage_Folder%\FSP_S.Fv
  ) else (
    pushd  %WORKSPACE%\Silicon\BroxtonSoC\BroxtonFspPkg\ApolloLakeFspGold\FSP
    python %WORKSPACE%\Core\IntelFsp2Pkg\Tools\SplitFspBin.py split -f FSP.fd -o .\ -n FSP.Fv
    popd
    copy /y/b %WORKSPACE%\Silicon\BroxtonSoC\BroxtonFspPkg\ApolloLakeFspGold\FSP\FSP_T.Fv %Storage_Folder%\FSP_T.Fv
    copy /y/b %WORKSPACE%\Silicon\BroxtonSoC\BroxtonFspPkg\ApolloLakeFspGold\FSP\FSP_M.Fv %Storage_Folder%\FSP_M.Fv
    copy /y/b %WORKSPACE%\Silicon\BroxtonSoC\BroxtonFspPkg\ApolloLakeFspGold\FSP\FSP_S.Fv %Storage_Folder%\FSP_S.Fv
  )
)

echo Get NvStorage Base and Size...
if not exist "%STITCH_PATH%\FlashMap.h" (
  echo ERROR: Couldn't find %STITCH_PATH%\FlashMap.h
  goto BldFail
)

::generate NvStorage.Fv
::      %STITCH_PATH%\FlashMap.h has the information for the magic numbers below. Parse it.
::        You'll need to account for both hex (0x000000) and int (1234567U) value types
::        FLASH_REGION_VPD_OFFSET
::          _PCD_VALUE_PcdFlashNvStorageBase - _PCD_VALUE_PcdFlashBaseAddress
::        FLASH_REGION_VPD_SIZE + FLASH_REGION_NVSTORAGE_SUBREGION_NV_FTW_WORKING_SIZE + FLASH_REGION_NVSTORAGE_SUBREGION_NV_FTW_SPARE_SIZE
::          _PCD_VALUE_PcdFlashNvStorageSize
::
::                     PCD to find                      Variable      File to search
::===========================================================================================
  call :FindPcdInFile _PCD_VALUE_PcdFlashNvStorageBase NvStorageBase "%STITCH_PATH%\FlashMap.h"
  call :FindPcdInFile _PCD_VALUE_PcdFlashBaseAddress   BaseAddress   "%STITCH_PATH%\FlashMap.h"
  call :FindPcdInFile _PCD_VALUE_PcdFlashNvStorageSize NvStorageSize "%STITCH_PATH%\FlashMap.h"
:: Find image offset as opposed to memory offset
  PowerShell ($env:NvStorageBase - $env:BaseAddress) > temp.pcd
  set /p VpdOffset=<temp.pcd
  del /f /q temp.pcd
:: Force the variables we care about into DEC string format
  set VpdSize=%NvStorageSize%
  call :ForceToDec VpdOffset
  call :ForceToDec VpdSize
:: Dump what we found
  echo - NvStorageBase = %NvStorageBase%
  echo - BaseAddress   = %BaseAddress%
  echo - NvStorageSize = %NvStorageSize%
  echo - VpdOffset     = %VpdOffset%
  echo - VpdSize       = %VpdSize%

:: Create NvStorage.fv
  echo Create NvStorage.fv...
  pushd %STITCH_PATH%
    REM This -s offset must equal the decimal value of  FLASH_REGION_VPD_OFFSET  in the Platform.fdf
    Split.exe -f %STITCH_PATH%\%BIOS_Name%.ROM -s %VpdOffset% -o temp1.bin -t temp2.bin
    REM This -s offset must equal the decimal value of
    REM  FLASH_REGION_VPD_SIZE + FLASH_REGION_NVSTORAGE_SUBREGION_NV_FTW_WORKING_SIZE + FLASH_REGION_NVSTORAGE_SUBREGION_NV_FTW_SPARE_SIZE
    Split.exe -f temp2.bin -s %VpdSize% -o %Storage_Folder%\NvStorage.Fv -t temp3.bin
    del /F/Q temp1.bin temp2.bin temp3.bin
  popd

echo.
echo Build location:     %BUILD_PATH%
echo BIOS ROM Created:   %BIOS_Name%
echo.
echo --------------- The EDKII BIOS build has successfully completed. ---------------
echo.
goto Exit

::
:: Find PCD value (%~1) in a file (%~3) and return it in a variable (%~2) as a HEX string
::
:: USAGE: call :FindPcdInFile PCD Variable File
:FindPcdInFile
  call :GetPcd "%~1" "%~2" "%~3"
  call :ForceToHex "%~2"
goto :EOF

::
:: Find PCD value (%~1) in a file (%~3) and return it in a variable (%~2)
::
:: USAGE: call :GetPcd PCD Variable File
:GetPcd
  set _TargetFile=%~3
  if not exist "%_TargetFile%" (
    echo ERROR: Couldn't find %_TargetFile%^!
    set ExitCode=1
  ) else (
    for /f "tokens=3" %%a in ('findstr /C:"%~1" "%_TargetFile%"') do (
      set _TestValue=%%~a
      set _PcdValue=!_TestValue:~0,-1!
    )
    set %~2=!_PcdValue!
	echo %2 %_PcdValue%
  )
  set _TargetFile=
  set _TestValue=
  set _PcdValue=
goto :EOF

::
:: Force value of %1 into hex format 0x1234
::
:: USAGE: call :ForceToHex Variable
:ForceToHex
  call set _Temp=%%%~1%%
  if "%_Temp:~0,2%" == "0x" goto :ForceToHexExit
  PowerShell ('0x' + [convert]::tostring($env:_Temp, 16).ToUpper()) > temp.pcd
  set /p %~1=<temp.pcd
  del /f /q temp.pcd
:ForceToHexExit
  set _Temp=
goto :EOF

::
:: Force value of %1 into decimal format
::
:: USAGE: call :ForceToDec Variable
:ForceToDec
  call set _Temp=%%%~1%%
  if not "%_Temp:~0,2%" == "0x" goto :ForceToDecExit
  PowerShell ([convert]::toInt64((Get-Item env:_Temp).value, 16)) > temp.pcd
  set /p %~1=<temp.pcd
  del /f /q temp.pcd
:ForceToDecExit
  set _Temp=
goto :EOF

:Usage
echo.
echo ***************************************************************************
echo Build BIOS Rom for BXT platforms.
echo.
echo Usage: BuildBios.bat [options] ^<PlatformType^> ^<BuildTarget^>
echo.
echo.   /?       Display this help text
echo    /l       Log a copy of the build output to EDK2.log
echo    /c       CleanAll before building
echo    /x64     Set Arch to X64  (default)
echo    /ia32    Set Arch to IA32
echo    /vs08    Set compiler to VisualStudio 2008
echo    /vs10    Set compiler to VisualStudio 2010
echo    /vs12    Set compiler to VisualStudio 2012
echo    /vs13    Set compiler to VisualStudio 2013  (default)
echo    /vs15    Set compiler to VisualStudio 2015
echo    Platform Types:  %Minnow_RVP%
echo    Build Targets:   Debug, Release
echo.
echo ***************************************************************************
:ExitFail
echo.
set exitCode=1
goto Exit

:BldFail
set exitCode=1
echo  -- Error:  EDKII BIOS Build has failed!
echo See EDK2.log for more details

:Exit
echo %date%  %time%
(
  EndLocal
  set BiosVersion=%BiosVersion%
  exit /b %ExitCode%
)
