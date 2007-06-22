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

@echo off

REM ##############################################################
REM # You should not have to modify anything below this line
REM #

if /I "%1"=="-h" goto usage
if /I "%1"=="-help" goto usage
if /I "%1"=="--help" goto usage
if /I "%1"=="/h" goto usage
if /I "%1"=="/?" goto usage
if /I "%1"=="/help" goto usage

REM
REM Check the required system environment variables
REM

echo ###########################################################
echo  Setup workspace environments
echo ###########################################################

:check_vc
if defined VCINSTALLDIR goto setup_workspace
if defined VS71COMNTOOLS (
  call "%VS71COMNTOOLS%\vsvars32.bat"
) else (
  echo.
  echo !!!WARNING!!! Cannot find Visual Studio !!!
  echo.
)

:setup_workspace
REM
REM Set the WORKSPACE to the current working directory
REM
set WORKSPACE=%CD%
set EDK_TOOLS_PATH=%WORKSPACE%\BaseTools

:tool_setup
echo.
echo ###########################################################
echo  Setup Framework Tools
echo ###########################################################
echo.

call %EDK_TOOLS_PATH%\toolsetup.bat

@echo off
REM
REM Done!!!
REM
goto end

:usage
echo.
echo  "Usage: %0 [/? | /h | /help | -h | -help | --help] [tools_path]"
echo.
echo                      tools_path       Tools' path. EDK_TOOLS_PATH will be set to this path.
echo.

:end
@echo on

