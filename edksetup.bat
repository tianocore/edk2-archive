@REM
@REM Copyright (c) 2006, Intel Corporation
@REM All rights reserved. This program and the accompanying materials
@REM are licensed and made available under the terms and conditions of the BSD License
@REM which accompanies this distribution.  The full text of the license may be found at
@REM http://opensource.org/licenses/bsd-license.php
@REM
@REM THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
@REM WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
@REM

@REM set following environment in this file or in command shell

@REM usage: edksetup.bat [Rebuild] [ForceRebuild] [Reconfig]
@REM if the argument, skip is present, only the paths and the
@REM test and set of environment settings are performed. 

@REM ##############################################################
@REM # You should not have to modify anything below this line
@REM #

@echo off

@REM
@REM Check the required system environment variables
@REM

:check_vc
if defined VCINSTALLDIR goto check_cygwin
if defined VS71COMNTOOLS (
  call "%VS71COMNTOOLS%\vsvars32.bat"
) else (
  echo.
  echo !!! WARNING !!!! Cannot find Visual Studio !!!
  echo.
)

:check_cygwin
if defined CYGWIN_HOME goto setup_workspace
if exist c:\cygwin (
  set CYGWIN_HOME=c:\cygwin
) else (
  echo.
  echo !!! WARNING !!!! Not set CYGWIN_HOME, gcc build may not be used !!!
  echo.
)

:setup_workspace
@REM
@REM Set the WORKSPACE to the current working directory
@REM
set WORKSPACE=%CD%

@REM
@REM check the EDK_TOOLS_PATH
@REM
if not defined EDK_TOOLS_PATH goto no_tools_path
if not exist %EDK_TOOLS_PATH% (
  echo.
  echo !!! WARNING !!!! %EDK_TOOLS_PATH% doesn't exist. Please check EDK_TOOLS_PATH !!!
  echo.
  goto end
)

:no_tools_path
if exist %WORKSPACE%\Tools (
  set EDK_TOOLS_PATH=%WORKSPACE%\Tools
) else (
  echo.
  echo !!! WARNING !!!! No tools path found. Please set EDK_TOOLS_PATH !!!
  echo.
  goto end
)

if defined WORKSPACE_TOOLS_PATH goto check_path
set PATH=%EDK_TOOLS_PATH%\Bin
echo Setting the PATH variable to include the EDK_TOOLS_PATH for this WORKSPACE
goto path_ok

:check_path
if "%EDK_TOOLS_PATH%"=="%WORKSPACE_TOOLS_PATH%" goto path_ok
set PATH=%EDK_TOOLS_PATH%;%PATH%
set WORKSPACE_TOOLS_PATH=%EDK_TOOLS_PATH%
echo Resetting the PATH variable to include the EDK_TOOLS_PATH for this WORKSPACE

:path_ok

@if /I "%1"=="-h" goto Usage
@if /I "%1"=="-help" goto Usage
@if /I "%1"=="--help" goto Usage
@if /I "%1"=="/h" goto Usage
@if /I "%1"=="/?" goto Usage
@if /I "%1"=="/help" goto Usage
@if /I "%1"=="ForceRebuild" goto ForceBuild
@if /I "%1"=="Reconfig" goto Reconfig
@if /I "%1"=="Rebuild" goto NormalBuild
@if NOT "%1"=="" goto Usage

:NormalBuild
@REM
@REM Start to build the Framework Tools
@REM

echo.
echo Building the Framework Tools
echo.

call %EDK_TOOLS_PATH%\setup.bat
@REM
@REM Done!!!
@REM
goto end


:ForceBuild 
call %EDK_TOOLS_PATH%\setup.bat cleanall

:Reconfig
@REM
@REM Reinstall all config files
@REM
call %EDK_TOOLS_PATH%\setup.bat Reconfig
goto end

:Usage
echo.
echo  Usage: %0 [Rebuild] [ForceRebuild] [Reconfig]
echo         Rebuild:       Incremental build, only build those updated tools; 
echo         ForceRebuild:  Rebuild all tools neither updated or not; 
echo         Reconfig:      Reinstall target.txt, tools_def.txt, FrameworkDatabase.db. 
echo.
echo  Note that target.template, tools_def.template, FrameworkDatabase.template will be
echo  only copied to target.txt, tools_def.txt, FrameworkDatabase.db respectively if they
echo  are not existed. Using option [Reconfig] to do the force copy. 
echo.

:end
@echo on

