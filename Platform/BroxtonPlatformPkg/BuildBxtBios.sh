#
# Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>
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
function ErrorExit() {
  echo
  echo "Press any key......"
  read
  exit 0
}

echo -e $(date)
##**********************************************************************
## Initial Setup
##**********************************************************************

Build_Flags=
SV_String=_
exitCode=0
Arch=X64
FabId=B
BoardId=MN

## Initialize all the build flags to FALSE
## depending on the cmd line input, some will be set to TRUE prior to building
ENBDT_PF_BUILD=FALSE
TABLET_PF_BUILD=FALSE
BYTI_PF_BUILD=FALSE
CSLE_ENABLE=FALSE
VP_BIOS_ENABLE=FALSE
SV_BIOS_ENABLE=FALSE
PPV_BIOS_ENABLE=FALSE
RVVP_BIOS_ENABLE=FALSE
RVV_BIOS_ENABLE=FALSE
SrcDebug=FALSE

## Clean up previous build files.
if [ -e $(pwd)/EDK2.log ]; then
  rm $(pwd)/EDK2.log
fi

if [ -e $(pwd)/Unitool.log ]; then
  rm $(pwd)/Unitool.log
fi

if [ -e $(pwd)/Conf/target.txt ]; then
  rm $(pwd)/Conf/target.txt
fi

if [ -e $(pwd)/Conf/BiosId.env ]; then
  rm $(pwd)/Conf/BiosId.env
fi

if [ -e $(pwd)/Conf/build_rule.txt ]; then
  rm $(pwd)/Conf/build_rule.txt
fi

if [ -e $(pwd)/FlashMap.h ]; then
  rm $(pwd)/FlashMap.h
fi

## Override tools_def.txt
#cp $WORKSPACE\BaseTools\Conf\tools_def.template $WORKSPACE\Conf\tools_def.txt
#:: Override tools_def.txt
echo Creating Conf folder and build config files...
cp $WORKSPACE/BaseTools/Conf/target.template $WORKSPACE/Conf/target.txt
cp $WORKSPACE/BaseTools/Conf/tools_def.template $WORKSPACE/Conf/tools_def.txt
cp $WORKSPACE/BaseTools/Conf/build_rule.template $WORKSPACE/Conf/build_rule.txt




TOOL_CHAIN_TAG=GCC5

## Define platform specific environment variables.
PLATFORM_NAME=BroxtonPlatformPkg
PLATFORM_PACKAGE=Platform/BroxtonPlatformPkg
EFI_SOURCE=$WORKSPACE
EDK_SOURCE=$WORKSPACE

##**********************************************************************
## Parse command line arguments
##**********************************************************************

## Optional arguments
for (( i=1; i<=$#; ))
  do
    if [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/X64" ]; then
      Arch=X64
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/MN" ]; then
      BoardId=MN
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/BG" ]; then
      BoardId=BG
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/MX" ]; then
      BoardId=MX
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/LH" ]; then
      BoardId=LH
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/D" ]; then
      FabId=D
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/B" ]; then
      FabId=B
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/A" ]; then
      FabId=A
      shift
    else
      break
    fi
  done

## Required argument(s)
if [ "$2" == "" ]; then
  echo 
  echo "Not Enough Arguments Provided"
  echo "Please review the Help screen"
  ErrorExit
fi

## Remove the values for Platform_Type and Build_Target from BiosId.env and stage in Conf/
cp $WORKSPACE/$PLATFORM_PACKAGE/BiosId.env       $WORKSPACE/Conf/BiosId.env
sed -i '/^BOARD_ID/d' $WORKSPACE/Conf/BiosId.env
sed -i '/^BOARD_REV/d' $WORKSPACE/Conf/BiosId.env
sed -i '/^BUILD_TYPE/d' $WORKSPACE/Conf/BiosId.env

if [ $BoardId == "MN" ]; then
  BOARD_ID=MNW3
  echo BOARD_ID = MINNOW3 >> $WORKSPACE/Conf/BiosId.env
elif [ $BoardId == "MX" ]; then
  BOARD_ID=M3MO
  echo BOARD_ID = M3MODUL >> $WORKSPACE/Conf/BiosId.env
elif [ $BoardId == "BG" ]; then
  BOARD_ID=BEN1
  echo BOARD_ID = BENSONV >> $WORKSPACE/Conf/BiosId.env
elif [ $BoardId == "LH" ]; then
  BOARD_ID=LEAF
  echo BOARD_ID = LEAFHIL >> $WORKSPACE/Conf/BiosId.env
else
  break
fi

ENBDT_PF_BUILD=TRUE
PLATFORM_NAME=BroxtonPlatformPkg
PLATFORM_PACKAGE=Platform/BroxtonPlatformPkg  
Platform_Type=$1

## Create new DefineAtBuildMacros.dsc file
Build_Macros=$WORKSPACE/$PLATFORM_PACKAGE/DefineAtBuildMacros.dsc
if [ -f "$Build_Macros" ]; then
  rm $Build_Macros
fi
touch $Build_Macros

## Output platform specific build macros to DefineAtBuildMacros.dsc
echo DEFINE ENBDT_PF_BUILD      = $ENBDT_PF_BUILD   >> $Build_Macros
echo DEFINE TABLET_PF_BUILD     = $TABLET_PF_BUILD  >> $Build_Macros
echo DEFINE BYTI_PF_BUILD       = $BYTI_PF_BUILD    >> $Build_Macros
echo DEFINE CSLE_ENABLE         = $CSLE_ENABLE      >> $Build_Macros
echo DEFINE VP_BIOS_ENABLE      = $VP_BIOS_ENABLE   >> $Build_Macros
echo DEFINE SV_BIOS_ENABLE      = $SV_BIOS_ENABLE   >> $Build_Macros
echo DEFINE PPV_BIOS_ENABLE     = $PPV_BIOS_ENABLE  >> $Build_Macros
echo DEFINE RVVP_BIOS_ENABLE    = $RVVP_BIOS_ENABLE >> $Build_Macros
echo DEFINE RVV_BIOS_ENABLE     = $RVV_BIOS_ENABLE  >> $Build_Macros

if [ $Arch == "IA32" ]; then
  echo DEFINE X64_CONFIG = FALSE      >> $Build_Macros
else
  echo DEFINE X64_CONFIG = TRUE       >> $Build_Macros
fi

if [ "$(echo $2 | tr 'a-z' 'A-Z')" == "RELEASE" ]; then
  TARGET=RELEASE
  BUILD_TYPE=R
  echo BUILD_TYPE = R >> $WORKSPACE/Conf/BiosId.env
else
  TARGET=DEBUG
  BUILD_TYPE=D
  echo BUILD_TYPE = D >> $WORKSPACE/Conf/BiosId.env
fi

if [ $BoardId == "BG" ]; then
  if [ $FabId == "B" ]; then
    BOARD_REV=B
    echo BOARD_REV = B >> $WORKSPACE/Conf/BiosId.env
  else
    BOARD_REV=A
    echo BOARD_REV = A >> $WORKSPACE/Conf/BiosId.env
  fi
fi

if [ $BoardId == "MN" ]; then
  if [ $FabId == "B" ]; then
    BOARD_REV=B
    echo BOARD_REV = B >> $WORKSPACE/Conf/BiosId.env
  else
    BOARD_REV=A
    echo BOARD_REV = A >> $WORKSPACE/Conf/BiosId.env
  fi
fi

if [ $BoardId == "MX" ]; then
  if [ $FabId == "B" ]; then
    BOARD_REV=B
    echo BOARD_REV = B >> $WORKSPACE/Conf/BiosId.env
  else
    BOARD_REV=A
    echo BOARD_REV = A >> $WORKSPACE/Conf/BiosId.env
  fi
fi

if [ $BoardId == "LH" ]; then
  if [ $FabId == "D" ]; then
    BOARD_REV=D
    echo BOARD_REV = D >> $WORKSPACE/Conf/BiosId.env
  fi
fi

##**********************************************************************
## Additional EDK Build Setup/Configuration
##**********************************************************************
echo "Ensuring correct build directory is present for GenBiosId..."
ACTIVE_PLATFORM=$PLATFORM_PACKAGE/PlatformPkgX64Gcc.dsc
#TOOL_CHAIN_TAG=GCC47
MAX_CONCURRENT_THREAD_NUMBER=1
sed -i '/^ACTIVE_PLATFORM/d' $WORKSPACE/Conf/target.txt
sed -i '/^TARGET /d' $WORKSPACE/Conf/target.txt
sed -i '/^TARGET_ARCH/d' $WORKSPACE/Conf/target.txt
sed -i '/^TOOL_CHAIN_TAG/d' $WORKSPACE/Conf/target.txt
sed -i '/^MAX_CONCURRENT_THREAD_NUMBER/d' $WORKSPACE/Conf/target.txt
echo ACTIVE_PLATFORM = $ACTIVE_PLATFORM                           >> Conf/target.txt
echo TARGET          = $TARGET                                    >> Conf/target.txt
echo TOOL_CHAIN_TAG  = $TOOL_CHAIN_TAG                            >> Conf/target.txt
echo MAX_CONCURRENT_THREAD_NUMBER = $MAX_CONCURRENT_THREAD_NUMBER >> Conf/target.txt
if [ $Arch == "IA32" ]; then
  echo TARGET_ARCH   = IA32                                       >> Conf/target.txt
else
  echo TARGET_ARCH   = IA32 X64                                   >> Conf/target.txt
fi

###
### Update ASL path for GCC47 & 46
###

## Fix error due to '\' in OBJCOPY_ADDDEBUGFLAG GCC Common rule
  echo DEBUG_*_*_OBJCOPY_ADDDEBUGFLAG     = --add-gnu-debuglink=\$\(DEBUG_DIR\)/\$\(MODULE_NAME\).debug >> $WORKSPACE/Conf/tools_def.txt
### 

##**********************************************************************
## Build BIOS
##**********************************************************************
echo "Creating BiosId..."
BUILD_PATH=Build/$PLATFORM_NAME/"$TARGET"_"$TOOL_CHAIN_TAG"
if [ ! -d "$BUILD_PATH/$Arch" ]; then
  mkdir -p $BUILD_PATH/$Arch
fi
if [ -e "$BUILD_PATH/$Arch/BiosId.bin" ]; then
  rm -f $BUILD_PATH/$Arch/BiosId.bin
fi
./Platform/BroxtonPlatformPkg/Common/Tools/GenBiosId/GenBiosId -i Conf/BiosId.env -o $BUILD_PATH/$Arch/BiosId.bin


echo
echo "**** Copy ResetVector to original folder ****"
echo "**** Due to nasm can't execute in Ubuntu ****"


echo "Invoking EDK2 build..."

build $Build_Flags

echo "check if Build was successful"

##**********************************************************************
## Post Build processing and cleanup
##**********************************************************************
grep "_PCD_VALUE_" $BUILD_PATH/IA32/BroxtonPlatformPkg/Common/PlatformSettings/PlatformPreMemPei/PlatformPreMemPei/DEBUG/AutoGen.h > FlashMap.h

#
# FSP Rebase and Split
#
#   0xFEF7A000 = gIntelFsp2WrapperTokenSpaceGuid.PcdFlashFvFspBase = $(CAR_BASE_ADDRESS) + $(BLD_RAM_DATA_SIZE) + $(FSP_RAM_DATA_SIZE) + $(FSP_EMP_DATA_SIZE) + $(BLD_IBBM_SIZE)
pushd  $WORKSPACE/Silicon/BroxtonSoC/BroxtonFspPkg/ApolloLakeFspBinPkg/FspBin
python $WORKSPACE/Core/IntelFsp2Pkg/Tools/SplitFspBin.py rebase -f Fsp.fd -c m -b 0xFEF7A000 -o ./ -n ApolloLakeFsp.fd
python $WORKSPACE/Core/IntelFsp2Pkg/Tools/SplitFspBin.py split -f ApolloLakeFsp.fd -o ./ -n FSP.Fv
popd
cp -f $WORKSPACE/Silicon/BroxtonSoC/BroxtonFspPkg/ApolloLakeFspBinPkg/FspBin/FSP_T.Fv $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
cp -f $WORKSPACE/Silicon/BroxtonSoC/BroxtonFspPkg/ApolloLakeFspBinPkg/FspBin/FSP_M.Fv $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
cp -f $WORKSPACE/Silicon/BroxtonSoC/BroxtonFspPkg/ApolloLakeFspBinPkg/FspBin/FSP_S.Fv $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch

echo "Running fce..."
cat $BUILD_PATH/FV/FVIBBM.Fv $BUILD_PATH/FV/SOC.fd > $BUILD_PATH/FV/Temp.fd
# Extract Hii data from build and store a copy in HiiDefaultData.txt
# UQI 0006 005C 0078 0030 0031 0030 0031 is for question prompt(STR_IPU_ENABLED)
# First 0006 is the length of string; Next six byte values are mapped to STR_IPU_ENABLED string value defined in Platform/BroxtonPlatformPkg/Common/PlatformSettings/PlatformSetupDxe/VfrStrings.uni.
./$PLATFORM_PACKAGE/Common/Tools/FCE/FCE read -i $BUILD_PATH/FV/Temp.fd 0006 005C 0078 0030 0031 0030 0031 > $BUILD_PATH/FV/HiiDefaultData.txt

## copy the Setup variable to the SetupDefault variable and save changes to BxtXXX.fd
./$PLATFORM_PACKAGE/Common/Tools/FCE/FCE update -i $BUILD_PATH/FV/Temp.fd -s $BUILD_PATH/FV/HiiDefaultData.txt -o $BUILD_PATH/FV/Bxt"$Arch".fd -g B73FE497-B92E-416e-8326-45AD0D270091 -a 1>>EDK2.log 2>&1
#echo "Skip FCE tool..."
Split -f $BUILD_PATH/FV/Bxt"$Arch".fd -s 0x35000 -o $BUILD_PATH/FV/FVIBBM.Fv
#cp $BUILD_PATH/FV/SOC.fd $BUILD_PATH/FV/Bxt"$Arch".fd

## Set the Board_Id, Build_Type, Version_Major, and Version_Minor environment variables
##find /v "#" Conf\BiosId.env > ver_strings
##for /f "tokens=1,3" %%i in (ver_strings) do set %%i=%%j
##del /f/q ver_strings >nul
##
VERSION_MAJOR=$(grep '^VERSION_MAJOR' Conf/BiosId.env | cut -d ' ' -f 3 | cut -c 1-4)
VERSION_MINOR=$(grep '^VERSION_MINOR' Conf/BiosId.env | cut -d ' ' -f 3 | cut -c 1-2)
BIOS_Name="$BOARD_ID""$BOARD_REV""$SV_String""$Arch"_"$BUILD_TYPE"_"$VERSION_MAJOR"_"$VERSION_MINOR"
cp $BUILD_PATH/FV/SOC.fd $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch/$BIOS_Name.ROM

echo Get NvStorage Base and Size...
if [ -e $(pwd)/FlashMap.h ]; then
  NvStorageBase=$(printf "0x%x" $(grep _PCD_VALUE_PcdFlashNvStorageBase FlashMap.h | awk '{print $3}' | awk -FU '{print $1}'))
  BaseAddress=$(printf "0x%x" $(grep _PCD_VALUE_PcdFlashBaseAddress FlashMap.h | awk '{print $3}' | awk -FU '{print $1}'))
  NvStorageSize=$(printf "0x%x" $(grep _PCD_VALUE_PcdFlashNvStorageSize FlashMap.h | awk '{print $3}' | awk -FU '{print $1}'))
  VpdOffset=$(($NvStorageBase - $BaseAddress))
  VpdSize=$(printf "%d" $NvStorageSize)

  #Dump what we found
  echo - NvStorageBase = $NvStorageBase
  echo - BaseAddress = $BaseAddress
  echo - NvStorageSize = $NvStorageSize
  echo - VpdOffset = $VpdOffset
  echo - VpdSize= $VpdSize

  #Create NvStorage.fv
  echo Create NvStorage.fv...
  pushd $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
  Split -f $BIOS_Name.ROM -s $VpdOffset -o temp1.bin -t temp2.bin
  Split -f temp2.bin -s $VpdSize -o NvStorage.Fv -t temp3.bin
  rm temp1.bin temp2.bin temp3.bin
  popd
else
  echo "ERROR: Couldn't find FlashMap.h"
  ErrorExit
fi


cp -f $BUILD_PATH/FV/FVOBB.Fv  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
cp -f $BUILD_PATH/FV/FVOBBX.Fv $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
cp -f $BUILD_PATH/FV/FVIBBR.Fv $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
cp -f $BUILD_PATH/FV/FVIBBM.Fv $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
cp -f $BUILD_PATH/FV/FVIBBL.Fv $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch

if [ $BoardId == "BG" ]; then
  if [ $FabId == "B" ]; then
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Board/BensonGlacier/IFWI/FAB_B/SpiChunk1.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Board/BensonGlacier/IFWI/FAB_B/SpiChunk2.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Board/BensonGlacier/IFWI/FAB_B/SpiChunk3.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
  else 
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Board/BensonGlacier/IFWI/FAB_A/SpiChunk1.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Board/BensonGlacier/IFWI/FAB_A/SpiChunk2.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Board/BensonGlacier/IFWI/FAB_A/SpiChunk3.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
  fi
fi

if [ $BoardId == "MN" ]; then
  if [ $FabId == "B" ]; then
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Board/MinnowBoard3/IFWI/FAB_B/SpiChunk1.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Board/MinnowBoard3/IFWI/FAB_B/SpiChunk2.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Board/MinnowBoard3/IFWI/FAB_B/SpiChunk3.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
  else
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Board/MinnowBoard3/IFWI/FAB_A/SpiChunk1.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Board/MinnowBoard3/IFWI/FAB_A/SpiChunk2.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Board/MinnowBoard3/IFWI/FAB_A/SpiChunk3.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
  fi
fi

if [ $BoardId == "MX" ]; then
  if [ $FabId == "B" ]; then
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Board/MinnowBoard3Next/IFWI/FAB_B/SpiChunk1.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Board/MinnowBoard3Next/IFWI/FAB_B/SpiChunk2.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Board/MinnowBoard3Next/IFWI/FAB_B/SpiChunk3.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
  else
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Board/MinnowBoard3Next/IFWI/FAB_A/SpiChunk1.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Board/MinnowBoard3Next/IFWI/FAB_A/SpiChunk2.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Board/MinnowBoard3Next/IFWI/FAB_A/SpiChunk3.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
  fi
fi

if [ $BoardId == "LH" ]; then
  if [ $FabId == "D" ]; then
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Board/LeafHill/IFWI/FAB_D/SpiChunk1.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Board/LeafHill/IFWI/FAB_D/SpiChunk2.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Board/LeafHill/IFWI/FAB_D/SpiChunk3.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
  fi
fi

#
# Assmeble components
#
pushd $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch

cat FVIBBL.Fv > IBBL.Fv

cat FVIBBM.Fv FSP_M.Fv > IBB.Fv

cat FSP_S.Fv FVIBBR.Fv FVOBB.Fv FVOBBX.Fv > OBB.Fv

cat SpiChunk1.bin IBBL.Fv IBB.Fv SpiChunk2.bin OBB.Fv NvStorage.Fv SpiChunk3.bin > $BIOS_Name"_GCC".bin

popd

echo
echo Check if SPI IFWI image is generated at below location:
echo $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch/$BIOS_Name"_GCC".bin
echo
echo
