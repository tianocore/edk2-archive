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
#WORKSPACE=$(pwd)
EDK_TOOLS_PATH=
Build_Flags=
SV_String=_
exitCode=0
Arch=IA32

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

## Setup EDK environment. Edksetup puts new copies of target.txt, tools_def.txt, build_rule.txt in WorkSpace\Conf
## Also run edksetup as soon as possible to avoid it from changing environment variables we're overriding
##. edksetup.sh > /dev/null
. edksetup.sh

## Get gcc version to determine which tool_def.template to use.
## If gcc version is 4.6 or before, use default. If not, use new one.
GCCVERSION=$(gcc --version | grep 'gcc' | grep '[0-9]' | cut -d ' ' -f 4 | cut -d '.' -f 2)
if (($GCCVERSION > 6)); then
  echo "GCC version is 4.7 or after"
  TOOL_CHAIN_TAG=GCC47
else
  echo "Type 'gcc --version' to check version"
  echo "Please update GCC version to 4.7 or later"
  ErrorExit
fi

#make -C BaseTools > /dev/null
make -C BaseTools 

## Define platform specific environment variables.
PLATFORM_NAME=BxtPlatformPkg
PLATFORM_PACKAGE=Platform/BxtPlatformPkg
EFI_SOURCE=$WORKSPACE
EDK_SOURCE=$WORKSPACE

##**********************************************************************
## Parse command line arguments
##**********************************************************************

## Optional arguments
for (( i=1; i<=$#; ))
  do
    if [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/Q" ]; then
      Build_Flags="$Build_Flags --quiet"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/L" ]; then
      Build_Flags="$Build_Flags -j EDK2.log"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/C" ]; then
      echo "Removing previous Build files..."
      if [ -d "Build" ]; then
        rm -r Build
      fi
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/W" ]; then
      SrcDebug=TRUE
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/SV" ]; then
      SV_BIOS_ENABLE=TRUE
      SV_String=_SV_
      Arch=X64
      echo "-- Forcing to 64-bit for SV build --"
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/PPV" ]; then
      PPV_BIOS_ENABLE=TRUE
      SV_String=_PPV_
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/RVVP" ]; then
      RVVP_BIOS_ENABLE=TRUE
      SV_String=_RVVP_
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/RVV" ]; then
      RVV_BIOS_ENABLE=TRUE
      SV_String=_RVV_
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/X64" ]; then
      Arch=X64
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/VP" ]; then
      VP_BIOS_ENABLE=TRUE
      shift
    elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "/CSLE" ]; then
      CSLE_ENABLE=TRUE
      shift
    else
      break
    fi
  done

## Required argument(s)
if [ "$2" == "" ]; then
  echo 
  echo "Not Enough Arguments Provided"
  echo "Please review the Help screen "/?""
  ErrorExit
fi

## Remove the values for Platform_Type and Build_Target from BiosId.env and stage in Conf/
cp $PLATFORM_PACKAGE/BiosId.env       Conf/BiosId.env
sed -i '/^BOARD_ID/d' Conf/BiosId.env
sed -i '/^BUILD_TYPE/d' Conf/BiosId.env

## BOARD_ID needs to be exactly 7 characters (GenBiosId.exe limitation)
echo "Setting  $1  platform configuration and BIOS ID..."
if [ "$(echo $1 | tr 'a-z' 'A-Z')" == "$eNB_RVP" ]; then ## Lower case to upper case
  BOARD_ID="$eNB_RVP"RVP
  echo BOARD_ID = "$eNB_RVP"RVP >> Conf/BiosId.env
  ENBDT_PF_BUILD=TRUE
  PLATFORM_NAME=AplPlatSamplePkg
  PLATFORM_PACKAGE=Platform/AplPlatSamplePkg

elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "$Phblt_RVP" ]; then
  BOARD_ID="$Phblt_RVP"RVP
  echo BOARD_ID = "$Phblt_RVP"RVP >> Conf/BiosId.env
  TABLET_PF_BUILD=TRUE

elif [ "$(echo $1 | tr 'a-z' 'A-Z')" == "$Embd_RVP" ]; then
  BOARD_ID="$Embd_RVP"RVP
  echo BOARD_ID = "$Embd_RVP"RVP >> Conf/BiosId.env
  BYTI_PF_BUILD=TRUE
  PLATFORM_PACKAGE=Platform/IsgPlatPkg

else
  echo "Error - Unsupported PlatformType: $1"
  echo "Please review the Help screen "/?""
  ErrorExit
fi
  
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
echo DEFINE SOURCE_DEBUG_ENABLE = $SrcDebug         >> $Build_Macros

if [ $Arch == "IA32" ]; then
  echo DEFINE X64_CONFIG = FALSE      >> $Build_Macros
else
  echo DEFINE X64_CONFIG = TRUE       >> $Build_Macros
fi

if [ "$(echo $2 | tr 'a-z' 'A-Z')" == "RELEASE" ]; then
  TARGET=RELEASE
  BUILD_TYPE=R
  echo BUILD_TYPE = R >> Conf/BiosId.env
else
  TARGET=DEBUG
  BUILD_TYPE=D
  echo BUILD_TYPE = D >> Conf/BiosId.env
fi


##**********************************************************************
## Additional EDK Build Setup/Configuration
##**********************************************************************
echo "Ensuring correct build directory is present for GenBiosId..."
ACTIVE_PLATFORM=$PLATFORM_PACKAGE/PlatformPkg.dsc
#TOOL_CHAIN_TAG=GCC47
MAX_CONCURRENT_THREAD_NUMBER=1
sed -i '/^ACTIVE_PLATFORM/d' Conf/target.txt
sed -i '/^TARGET /d' Conf/target.txt
sed -i '/^TARGET_ARCH/d' Conf/target.txt
sed -i '/^TOOL_CHAIN_TAG/d' Conf/target.txt
sed -i '/^MAX_CONCURRENT_THREAD_NUMBER/d' Conf/target.txt
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
  echo  "*_GCC46_*_ASL_PATH = \"wine $WORKSPACE/PlatformTools/ASL/iasl.exe\"" >> Conf/tools_def.txt
  echo  "*_GCC47_*_ASL_PATH = \"wine $WORKSPACE/PlatformTools/ASL/iasl.exe\"" >> Conf/tools_def.txt
## Fix error due to '\' in OBJCOPY_ADDDEBUGFLAG GCC Common rule
  echo DEBUG_*_*_OBJCOPY_ADDDEBUGFLAG     = --add-gnu-debuglink=\$\(DEBUG_DIR\)/\$\(MODULE_NAME\).debug >> Conf/tools_def.txt
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
wine PlatformTools/GenBiosId/GenBiosId.exe -i Conf/BiosId.env -o $BUILD_PATH/$Arch/BiosId.bin

echo "Running UniTool..."
## Scan the main CRB tree, excluding R8VlvDeviceSvRestrictedPkg folders
wine PlatformTools/UniTool/UniTool.exe -b -u -x Platform/BxtPlatformPkg  PlatformSetupDxe/UqiList.uni 1>>Unitool.log 2>&1

echo
echo "**** Replace DebugLib.h to save space.... ****"
cp PlatformTools/GCC/DebugLib.h Core/MdePkg/Include/Library/

echo
echo "**** Copy ResetVector to original folder ****"
echo "**** Due to nasm can't execute in Ubuntu ****"
cp PlatformTools/GCC/Vtf0/ResetVector.ia32.port80.raw $PLATFORM_PACKAGE/Override/UefiCpuPkg/ResetVector/Vtf0/Bin
cp PlatformTools/GCC/Vtf1/ResetVector.ia32.port80.raw $PLATFORM_PACKAGE/Override/UefiCpuPkg/ResetVector/Vtf1/Bin

echo "Invoking EDK2 build..."
build $Build_Flags


##**********************************************************************
## Post Build processing and cleanup
##**********************************************************************
grep "_PCD_VALUE_" $BUILD_PATH/IA32/$PLATFORM_PACKAGE/PlatformPei/PlatformPei/DEBUG/AutoGen.h > FlashMap.h

#echo "Running fce..."
## Extract Hii data from build and store in HiiDefaultData.txt
#wine PlatformTools/FCE/FCE.exe read -i $BUILD_PATH/FV/SOC.fd > $BUILD_PATH/FV/HiiDefaultData.txt 1>>EDK2.log 2>&1

## copy the Setup variable to the SetupDefault variable and save changes to BxtXXX.fd
#wine PlatformTools/FCE/FCE.exe mirror -i $BUILD_PATH/FV/SOC.fd -o $BUILD_PATH/FV/Bxt"$Arch".fd Setup SetupDefault 1>>EDK2.log 2>&1
echo "Skip FCE tool..."
cp $BUILD_PATH/FV/SOC.fd $BUILD_PATH/FV/Bxt"$Arch".fd

##echo Running KeyEnroll...
## RestrictedBegin
##if /i not "$Platform_Type" == "$eNB_RVP" (
##   call $PLATFORM_PACKAGE/Restricted/Internal/Tools/KeyEnroll/KeyEnroll.bat  $BUILD_PATH  $BUILD_PATH/FV/Vlv"$Arch".fd 1>>EDK2.log 2>&1
##) else if /i "$Platform_Type" == "$eNB_RVP" (
##   call $PLATFORM_PACKAGE/Restricted/Internal/Tools/KeyEnroll/BBAY-KeyEnroll.bat  $BUILD_PATH  $BUILD_PATH/FV/Vlv"$Arch".fd 1>>EDK2.log 2>&1
##)
##   if %ERRORLEVEL% NEQ 0 goto BldFail
## RestrictedEnd
echo Skip "KeyEnroll tool..."

## Set the Board_Id, Build_Type, Version_Major, and Version_Minor environment variables
##find /v "#" Conf\BiosId.env > ver_strings
##for /f "tokens=1,3" %%i in (ver_strings) do set %%i=%%j
##del /f/q ver_strings >nul
##
VERSION_MAJOR=$(grep '^VERSION_MAJOR' Conf/BiosId.env | cut -d ' ' -f 3 | cut -c 1-4)
VERSION_MINOR=$(grep '^VERSION_MINOR' Conf/BiosId.env | cut -d ' ' -f 3 | cut -c 1-2)
BIOS_Name="$BOARD_ID""$SV_String""$Arch"_"$BUILD_TYPE"_"$VERSION_MAJOR"_"$VERSION_MINOR"
cp -f $BUILD_PATH/FV/Bxt"$Arch".fd  $WORKSPACE/$BIOS_Name.ROM
cp -f $BUILD_PATH/FV/FVOBB.Fv  $WORKSPACE
cp -f $BUILD_PATH/FV/FVOBBX.Fv $WORKSPACE
cp -f $BUILD_PATH/FV/FVIBBR.Fv $WORKSPACE
cp -f $BUILD_PATH/FV/FVIBBM.Fv $WORKSPACE
cp -f $BUILD_PATH/FV/FVIBBL.Fv $WORKSPACE
cp FlashMap.h $WORKSPACE/$BIOS_Name.map

echo
echo "Skip Running BIOS_Signing.bat ..."
#echo "Running BIOS_Signing.bat ..."
##pushd Stitch/BIOS_Signing
##set SEC_Ver=1.0.0.1054
##call BIOS_Signing.bat ../../$BIOS_Name  $target  $Arch  BLAK  $SEC_Ver  >>../../EDK2.log 2>&1
## use temp signing until Signing Script can be updated for BXT
#cd Stitch/BIOS_Signing_Temp
#./SBT_sign_no_KM_3M.sh  ../../$BIOS_Name  >>../../EDK2.log 2>&1
#echo "BIOS signing complete"

echo
echo Build location:     $BUILD_PATH
echo BIOS ROM Created:   $BIOS_Name
echo
echo -------------------- The EDKII BIOS build has successfully completed. --------------------
echo
