#
# Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
# This program and the accompanying materials
# are licensed and made available under the terms and conditions of the BSD License
# which accompanies this distribution.  The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
# 
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#


##**********************************************************************
## Function define
##**********************************************************************
function Usage() {
  echo
  echo "Script to build BIOS firmware and stitch the entire IFWI."
  echo
  echo "Usage: BuildIFWI.bat [options] ^<PlatformType^> ^<BuildTarget^> [IFWI Suffix]"
  echo
  echo "       /?    Display this help text                                                       "
  echo "       /q    Quiet mode. Only display Fatal Errors (slightly faster)                      "
  echo "       /l    Log a copy of the build output to EDK2.log                                   "
  echo "       /c    CleanAll before building                                                     "
  echo "       /w    Enabled WinDbg module for build                                              "
  echo "       /sv   Build with SVBIOS flags enabled                                              "
  echo "       /ppv  Build with PPV flags enabled                                                 "
  echo "       /rvv  Build with RVV BXTM flags enabled                                            "
  echo "       /rvvp Build with RVVP BXTP flags enabled                                           "
  echo "       /x64  Set Arch to X64  (default: IA32)                                             "
  echo "       /sata Enable SATA Build                                                            "
  echo "       /pcie Enable PCIe Build                                                            "
  echo "       /nG   Not update GOP driver per StitchConfig (override src version)                "
  echo "       /uM   Update Microcode per StitchConfig  (override src version)                    "
  echo
  echo "      Set ONLY ONE of the following:                                                      "
  echo "       /vp   Set special build flag for ALL Pre-Si                                        "
  echo "       /csle Set special build flag for C-SLE                                             "
  echo
  echo "     Platform Types:   $eNB_RVP $Phblt_RVP $Phblt_Die1_RVP $Embd_RVP                      "
  echo "                      APLK - Netbook/Desktop                                              "
  echo "                      BXTM - Phablet build for BXT-A                                      "
  echo "                      BXTM1 - Phablet build for BXT Die1(E0)                              "
  echo "                      APLI - Embedded/IVI build (IOTG)                                     "
  echo
  echo "       Build Targets:    Release, Debug                                                   "
  echo "       IFWI Suffix:      Suffix to append to end of IFWI filename (default: MM_DD_YYYY)   "
  echo
  echo "       See  Stitch/Stitch_Config.txt  for additional stitching settings.                  "
  echo
  echo "Press any key......"
  read
  exit 0
}


## Assign initial values
exitCode=0
Build_Flags=
Stitch_Flags=
Arch=IA32

## Description of each Platform/Board_ID
##   APLK - Netbook/Desktop (PCCG)
##   BXTM - Phablet build (MCG)
##   BXTM1 - Phablet build for BXT Die1(E0) (MCG)
##   APLI - Embedded/IVI build (IOTG)
eNB_RVP=APLK
Phblt_RVP=BXTM
Phblt_Die1_RVP=BXTM1
Embd_RVP=APLI

## Parse Optional arguments
if [ "$1" == "/?" ]; then
  Usage
fi

## Build Flags
for (( i=1; i<=$#; ))
  do
    if [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/Q" ]; then
      Build_Flags="$Build_Flags /q"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/L" ]; then
      Build_Flags="$Build_Flags /l"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/C" ]; then
      Build_Flags="$Build_Flags /c"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/W" ]; then
      Build_Flags="$Build_Flags /w"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/SV" ]; then
      Build_Flags="$Build_Flags /sv"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/PPV" ]; then
      Build_Flags="$Build_Flags /ppv"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/RVVP" ]; then
      Build_Flags="$Build_Flags /rvvp"
      shift
	elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/RVV" ]; then
      Build_Flags="$Build_Flags /rvv"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/X64" ]; then
      Arch=X64
      Build_Flags="$Build_Flags /x64"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/VP" ]; then
      Build_Flags="$Build_Flags /vp"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/CSLE" ]; then
      Build_Flags="$Build_Flags /csle"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/SATA" ]; then
      Build_Flags="$Build_Flags /sata"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/PCIE" ]; then
      Build_Flags="$Build_Flags /pcie"
      shift
    else
      break
    fi
  done

## Stitch Flags
for (( i=1; i<=$#; ))
  do
    if [ "$1" == "/nG" ]; then
      Stitch_Flags="$Stitch_Flags /nG"
      shift
    elif [ "$1" == "/uM" ]; then
      Stitch_Flags="$Stitch_Flags /uM"
      shift
    elif [ "$1" == "/nB" ]; then
      Stitch_Flags="$Stitch_Flags /nB"
      shift
    else
      break
    fi
  done

## Require 2 input parameters
if [ "$2" == "" ]; then
  echo "Not Enough Arguments Provided"
  echo "Please review the Help screen "/?""
fi

## Assign required arguments
Platform_Type=$1
Build_Target=$2
if [ "$3" == "" ]; then
  IFWI_Suffix=
else
  IFWI_Suffix="/S $3"
fi


## Build BIOS
echo "======================================================================"
echo "Build_IFWI:  Calling BIOS build Script..."
sh Platform/BroxtonPlatformPkg/BuildBios.sh $Build_Flags $Platform_Type $Build_Target

echo
echo Finished Building Process.
echo


