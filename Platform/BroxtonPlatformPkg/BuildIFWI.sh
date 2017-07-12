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
function Usage () {
  echo
  echo "Script to build BIOS firmware and stitch the entire IFWI."
  echo
  echo "Usage: BuildIFWI.sh Build_Flags Platform_Type Build_Target"
  echo
  echo "       Build_Flags:                 /A     Set FabId to A (default:  FAB_B)"
  echo "       Build_Flags:                 /B     Set FabId to B (default:  FAB_B)"
  echo "       Platform_Type [optional]:    Broxton  "                   
  echo "       Build_Target:                Release, Debug       "
  echo
  echo "Press any key......"
  read
  exit 0
}


## Assign initial values
exitCode=0
Build_Flags=
Arch=IA32
FabId=B


## Parse Optional arguments
if [ "$1" == "/?" ]; then
  Usage
fi

if [ "$1" == "" ]; then
  echo "Not Enough Arguments Provided"
  echo "Please review the Help screen"
  Usage
fi

## Build Flags
for (( i=1; i<=$#; ))
  do
    if [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/X64" ]; then
      Arch=X64
      Build_Flags="$Build_Flags /x64"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/B" ]; then
      FabId=B
      Build_Flags="$Build_Flags /B"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/A" ]; then
      FabId=A
      Build_Flags="$Build_Flags /A"
      shift
    else
      break
    fi
  done


## Require 2 input parameters
if [ "$2" == "" ]; then
  echo "Not Enough Arguments Provided"
  echo "Please review the Help screen"
  Usage
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
bash Platform/BroxtonPlatformPkg/BuildBios.sh $Build_Flags $Platform_Type $Build_Target

echo
echo Finished Building Process.
echo


