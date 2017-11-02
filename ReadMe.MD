
This code base is designed for the MinnowBoard v3 platform using the Intel(R) Atom(tm) Processor E3900 Series.
This code base is based on UDK2017 core packages.

# INDEX

1.  HOW TO CREATE A FULL SOURCE TREE
2.  HOW TO BUILD (WINDOWS ENVIRONMENT)
3.  MAINTAINERS

## 1. HOW TO CREATE A FULL SOURCE TREE

1. Create a new folder (directory) on the root of your local development machine
   for use as your work space (this example uses "C:\MyWorkspace").

   NOTE: Some code paths are very long, and placing the working directory too
         deep below the ROOT directory may cause the path to be longer than the
         operating system or file system's maximum path length.

2. Get MinnowBoard v3 branch from GitHub with the following command.
   * `git clone -b devel-MinnowBoard3-UDK2017 https://github.com/tianocore/edk2-platforms.git`

   Once the download completes, all EDK II packages and files will be found
   under the "C:\MyWorkspace\edk2-platforms" directory.

3. Get the BaseTools binary and copy them to BaseTools binary folder.
   1. `git clone https://github.com/tianocore/edk2-BaseTools-win32.git Win32`
   2. Enter folder Win32
   3. `git checkout 0e088c19ab31fccd1d2f55d9e4fe0314b57c0097`
   4. Copy Win32 to the BaseTools Binary folder.
       (C:\MyWorkspace\edk2-platforms\BaseTools\Bin\)

4. Get the Intel(R) Firmware Support Package(Intel FSP).
   1. Run `git clone -b ApolloLake https://github.com/IntelFsp/FSP.git` to download FSP
   2. Enter folder FSP
   3. Run `git checkout de67689ef6261c1a0ecee01b7312bd635e3435e7`
   4. Copy the ApolloLakeFspBinPkg to the folder
      "C:\MyWorkspace\edk2-platforms\Silicon\BroxtonSoC\BroxtonFspPkg"

5. Install UEFI UNDI driver:
      Download PREBOOT.EXE Version 21.1 from the "Intel® Ethernet Connections
      Boot Utility, Preboot Images, and EFI Drivers" page.
      https://downloadcenter.intel.com/download/19186/Intel-Ethernet-Connections-Boot-Utility-Preboot-Images-and-EFI-Drivers
   Windows user:
   1. Install PREBOOT.EXE into the default folder (C:\Intel21.1).
   2. Copy the UEFI x64 PCI-E gigabit driver (C:\Intel21.1\APPS\EFI\EFIx64\E7320X3.EFI) 
      to the platform package directory below, create the folder if it does not exist:
      "C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Common\Binaries\UNDI\I210PcieUndiDxe"
   Linux user:
   1. Use command "unzip PREBOOT.EXE" to extract UEFI x64 PCI-E gigabit driver
      (preboot/APPS/EFI/EFIx64/E7320X3.EFI)
   2. Copy E7320X3.EFI to platform package directory below, create the folder
      if it does not exist:
    * "~/src/MyWorkspace/edk2-platforms/Platform/BroxtonPlatformPkg/Common/Binaries/UNDI/I210PcieUndiDxe"

6. Follow the instructions found in the "Patch-HOWTO.txt" file located in the Workspace
   (e.g. "C:\MyWorkspace\edk2-platforms\Core\CryptoPkg\Library\OpensslLib\Patch-HOWTO.txt")
   to install the OpenSSL source code and enable the CryptoPkg module.

7. Download MinnowBoard v3 Binary Object Modules
   1. Download the "MinnowBoard_v3-0.63-Binary.Objects.zip" from the project page:
      * https://firmware.intel.com/projects/minnowboardv3
   2. Unzip and copy the two folders (FAB_A, FAB_B) into
      the directory below, and create the folder if it does not exist:
      * "C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Common\Binaries\IFWI\MinnowBoard3"

## 2. HOW TO BUILD (WINDOWS ENVIRONMENT)

Windows System Configuration:
  Microsoft Windows 7 Ultimate 64-bit*

1. Setup Build Environment

   1. Install Visual Studio 2013/2015 on the build machine.

   2. Install version Python 2.7.10 from: https://www.python.org/downloads/release/python-2710/
      * Make sure that a file with the extension of ".py" will be opened by Python.exe.

   3. Install the NASM assembly language compiler:
      a. Download NASM 2.12.02 binaries:
        * http://www.nasm.us/pub/nasm/releasebuilds/2.12.02/win32/nasm-2.12.02-win32.zip
        * http://www.nasm.us/pub/nasm/releasebuilds/2.12.02/win64/nasm-2.12.02-win64.zip
      b. Unzip the 32-bit & 64-bit versions of nasm.exe to the directories listed below,
         respectively, and create the folders if they do not exist:
        * "C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Common\Tools\nasm\Win32"
        * "C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Common\Tools\nasm\Win64"

   4. Install the ACPI Source Language (ASL) compiler:
      a. Install the IASL 2016-09-30 compiler from acpica.org:
         https://acpica.org/sites/acpica/files/iasl-win-20160930.zip
      b. Unzip the "iasl.exe" file into the directory below, and create the folder
         if it does not exist:
         * "C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Common\Tools\Iasl"

2. Build

  2a. Build Steps (32-bit)
    1. To build a 32-bit release version
      * Open a command prompt of Microsoft Visual Studio, such as "Developer Command Prompt for VS2013";
      * Type the command: `cd C:\MyWorkspace\edk2-platforms`
      * Type the command: `BuildBIOS.bat /B /VS13 /IA32 Broxton Release` to build a release version.
        (Please use "/VS15" flag for Visual Studio 2015 build; Replace "/B" with "/A" flag for MinnowBoard 3 FAB A board)

    2. To build a 32-bit debug version
      * Open a command prompt of Microsoft Visual Studio, such as "Developer Command Prompt for VS2013";
      * Type the command: `cd C:\MyWorkspace\edk2-platforms`
      * Type the command: `BuildBIOS.bat /B /VS13 /IA32 Broxton Debug` to build a debug version.
        (Please use "/VS15" flag for Visual Studio 2015 build; Replace "/B" with "/A" flag for MinnowBoard 3 FAB A board)

    3. After the build successfully completes, the 8MB firmware binary image will be located in the
       following location on your local hard drive:

      "C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Common\Tools\Stitch"

  2b. Build Steps (64-bit)
    1. To build a 64-bit release version
      * Open a command prompt of Microsoft Visual Studio, such as "Developer Command Prompt for VS2013";
      * Type the command: `cd C:\MyWorkspace\edk2-platforms`
      * Type the command: `BuildBIOS.bat /B /VS13 /x64 Broxton Release` to build a release version.
        (Please use "/VS15" flag for Visual Studio 2015 build; Replace "/B" with "/A" flag for MinnowBoard 3 FAB A board)

    2. To build a 64-bit debug version
      * Open a command prompt of Microsoft Visual Studio, such as "Developer Command Prompt for VS2013";
      * Type the command: `cd C:\MyWorkspace\edk2-platforms`
      * Type the command: `BuildBIOS.bat /B /VS13 /x64 Broxton Debug` to build a debug version.
        (Please use "/VS15" flag for Visual Studio 2015 build; Replace "/B" with "/A" flag for MinnowBoard 3 FAB A board)

    3. After the build successfully completes, the 8MB firmware binary image will be located in the
      following location on your local hard drive:

      "C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Common\Tools\Stitch"

## 3. HOW TO BUILD (LINUX ENVIRONMENT)

Linux System Configuration:
  Ubuntu 16.04 64-bit
  bash shell
  gcc version 5 or above

1. Setup Build Environment
   Use below command to install required packages:
      # sudo apt-get update
      # sudo apt-get install nasm
      # sudo apt-get install iasl
      # sudo apt-get install build-essential
      # sudo apt-get install uuid-dev

2. Build Steps (64-bit)
    1. To build a 64-bit release version
      * Open a command prompt, type command: "cd ~/src/MyWorkspace/edk2-platforms";
      * For MinnowBoard 3 FAB B, type the command:"./BuildBIOS.sh /B Release" to build a release version.
        (Replace "/B" with "/A" for MinnowBoard 3 FAB A board)
    2. To build a 64-bit debug version 
      * Open a command prompt, type command: "cd ~/src/MyWorkspace/edk2-platforms";
      * For MinnowBoard 3 FAB B, type the command:"./BuildBIOS.sh /B Debug" to build a debug version.
        (Replace "/B" with "/A" for MinnowBoard 3 FAB A board)
    3. After the build successfully completes, the 8MB firmware binary image will be located in the
       following location on your local hard drive:

      "~/src/MyWorkspace/edk2-platforms/Platform/BroxtonPlatformPkg/Common/Tools/Stitch/"

## MAINTAINERS

* david.wei@intel.com
* mang.guo@intel.com
* vincent.zimmer@intel.com
* mike.wu@intel.com
* shifeix.a.lu@intel.com

