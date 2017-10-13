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
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/BG" ]; then
      BoardId=BG
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
else
  BOARD_ID=BEN1
  echo BOARD_ID = BENSONV >> $WORKSPACE/Conf/BiosId.env
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
  BOARD_REV=A
  echo BOARD_REV = A >> $WORKSPACE/Conf/BiosId.env
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
echo "**** Replace DebugLib.h to save space.... ****"
cp PlatformTools/GCC/DebugLib.h Core/MdePkg/Include/Library/

echo
echo "**** Copy ResetVector to original folder ****"
echo "**** Due to nasm can't execute in Ubuntu ****"


echo "Invoking EDK2 build..."

build $Build_Flags

##**********************************************************************
## Post Build processing and cleanup
##**********************************************************************

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

#echo "Running fce..."
## Extract Hii data from build and store in HiiDefaultData.txt
#wine PlatformTools/FCE/FCE.exe read -i $BUILD_PATH/FV/SOC.fd > $BUILD_PATH/FV/HiiDefaultData.txt 1>>EDK2.log 2>&1

## copy the Setup variable to the SetupDefault variable and save changes to BxtXXX.fd
#wine PlatformTools/FCE/FCE.exe mirror -i $BUILD_PATH/FV/SOC.fd -o $BUILD_PATH/FV/Bxt"$Arch".fd Setup SetupDefault 1>>EDK2.log 2>&1
#echo "Skip FCE tool..."
cp $BUILD_PATH/FV/SOC.fd $BUILD_PATH/FV/Bxt"$Arch".fd

## Set the Board_Id, Build_Type, Version_Major, and Version_Minor environment variables
##find /v "#" Conf\BiosId.env > ver_strings
##for /f "tokens=1,3" %%i in (ver_strings) do set %%i=%%j
##del /f/q ver_strings >nul
##
VERSION_MAJOR=$(grep '^VERSION_MAJOR' Conf/BiosId.env | cut -d ' ' -f 3 | cut -c 1-4)
VERSION_MINOR=$(grep '^VERSION_MINOR' Conf/BiosId.env | cut -d ' ' -f 3 | cut -c 1-2)
BIOS_Name="$BOARD_ID""$BOARD_REV""$SV_String""$Arch"_"$BUILD_TYPE"_"$VERSION_MAJOR"_"$VERSION_MINOR"

cp -f $BUILD_PATH/FV/FVOBB.Fv  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
cp -f $BUILD_PATH/FV/FVOBBX.Fv $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
cp -f $BUILD_PATH/FV/FVIBBR.Fv $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
cp -f $BUILD_PATH/FV/FVIBBM.Fv $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
cp -f $BUILD_PATH/FV/FVIBBL.Fv $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch

if [ $BoardId == "BG" ]; then
  cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Binaries/IFWI/BensonGlacier/FAB_A/SpiChunk1.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
  cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Binaries/IFWI/BensonGlacier/FAB_A/SpiChunk2.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
  cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Binaries/IFWI/BensonGlacier/FAB_A/SpiChunk3.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
  cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Binaries/IFWI/BensonGlacier/FAB_A/GCC/NvStorage.Fv $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
fi

if [ $BoardId == "MN" ]; then
  if [ $FabId == "B" ]; then
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Binaries/IFWI/MinnowBoard3/FAB_B/SpiChunk1.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Binaries/IFWI/MinnowBoard3/FAB_B/SpiChunk2.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Binaries/IFWI/MinnowBoard3/FAB_B/SpiChunk3.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Binaries/IFWI/MinnowBoard3/FAB_B/GCC/NvStorage.Fv $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
  else
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Binaries/IFWI/MinnowBoard3/FAB_A/SpiChunk1.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Binaries/IFWI/MinnowBoard3/FAB_A/SpiChunk2.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Binaries/IFWI/MinnowBoard3/FAB_A/SpiChunk3.bin  $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
    cp -f $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Binaries/IFWI/MinnowBoard3/FAB_A/GCC/NvStorage.Fv $WORKSPACE/Platform/BroxtonPlatformPkg/Common/Tools/Stitch
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
