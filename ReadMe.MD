
This code base is designed for the MinnowBoard v3 platform using the Intel(R) Atom(tm) Processor E3900 Series.

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
   * `git clone -b devel-MinnowBoard3 https://github.com/tianocore/edk2-platforms.git`

   Once the download completes, all EDK II packages and files will be found
   under the "C:\MyWorkspace\edk2-platforms" directory.

3. Get the BaseTools binary and copy them to BaseTools binary folder.
   1. Enter folder BaseTools\Bin
   2. `git clone https://github.com/tianocore/edk2-BaseTools-win32.git Win32`
   3. Enter folder Win32
   4. `git checkout f0c97b7716ce92f1fc657f87f29bcc9170e5eb1f`

4. Get the Intel(R) Firmware Support Package(Intel FSP)
   1. Run `git clone -b ApolloLake https://github.com/IntelFsp/FSP.git` to download FSP.
   2. Copy the ApolloLakeFspBinPkg to the folder
      "C:\MyWorkspace\edk2-platforms\Silicon\BroxtonSoC\BroxtonFspPkg".

5. Install UEFI UNDI driver:
   1. Download PREBOOT.EXE Version 21.1 from the "Intel® Ethernet Connections
      Boot Utility, Preboot Images, and EFI Drivers" page.
      https://downloadcenter.intel.com/download/19186/Intel-Ethernet-Connections-Boot-Utility-Preboot-Images-and-EFI-Drivers
   2. Install PREBOOT.EXE into the default folder (C:\Intel21.1).
   3. Copy the UEFI x64 PCI-E gigabit driver (C:\Intel21.1\APPS\EFI\EFIx64\EnnnnX3.EFI,
      where "nnnn" is the driver version number) to the platform package directory below, create the folder if it does not exist:
      "C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Common\Binaries\UNDI\I210PcieUndiDxe"
   4. Open the file "C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\PlatformPkg.fdf"
      and change the UNDI driver file name reference with the one that was downloaded:
         SECTION PE32 = BroxtonPlatformPkg/Common/Binaries/UNDI/I210PcieUndiDxe/E7006X3.EFI

6. Install the NASM assembly language compiler:
   1. Download NASM 2.12.02 binaries:
      * http://www.nasm.us/pub/nasm/releasebuilds/2.12.02/win32/nasm-2.12.02-win32.zip
      * http://www.nasm.us/pub/nasm/releasebuilds/2.12.02/win64/nasm-2.12.02-win64.zip
   2. Unzip the 32-bit & 64-bit versions of nasm.exe to the directories listed below,
      respectively, and create the folders if they do not exist:
      * "C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Common\Tools\nasm\Win32"
      * "C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Common\Tools\nasm\Win64"

7. Install the ACPI Source Language (ASL) compiler:
   1. Install the IASL 2016-09-30 compiler from acpica.org:
      https://acpica.org/sites/acpica/files/iasl-win-20160930.zip
   2. Unzip the "iasl.exe" file into the directory below, and create the folder
      if it does not exist:
      * "C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Common\Tools\Iasl"

8. Follow the instructions found in the "Patch-HOWTO.txt" file located in the Workspace
   (e.g. "C:\MyWorkspace\edk2-platforms\Core\CryptoPkg\Library\OpensslLib\Patch-HOWTO.txt")
   to install the OpenSSL source code and enable the CryptoPkg module.

9. Download MinnowBoard v3 Binary Object Modules
   1. Download the "MinnowBoard_v3-0.60-Binary.Objects.zip" from the project page:
      * https://firmware.intel.com/projects/minnowboardv3
   2. Unzip and copy the two folders (A_Stepping, B_Stepping) into
      the directory below, and create the folder if it does nto exist:
      * "C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Common\Binaries\IFWI"

## 2. HOW TO BUILD (WINDOWS ENVIRONMENT)

Windows System Configuration:
  Microsoft Windows 7 Ultimate 64-bit*

1. Setup Build Environment

   1. Install Visual Studio 2013/2015 on the build machine.

   2. Install the latest Python 2 Release from: https://www.python.org/downloads/windows/
      * Make sure that a file with the extension of ".py" will be opened by Python.exe.

2. Build

  2a. Build Steps (32-bit)
    1. To build a 32-bit release version
      * Open a command prompt of Microsoft Visual Studio, such as "Developer Command Prompt for VS2013";
      * Type the command: `cd C:\MyWorkspace\edk2-platforms`
      * Type the command: `BuildBIOS.bat /VS13 /IA32 Broxton Release` to build a release version.
        (Please use "VS15" flag for Visual Studio 2015 build)

    2. To build a 32-bit debug version
      * Open a command prompt of Microsoft Visual Studio, such as "Developer Command Prompt for VS2013";
      * Type the command: `cd C:\MyWorkspace\edk2-platforms`
      * Type the command: `BuildBIOS.bat /VS13 /IA32 Broxton Debug` to build a debug version.
        (Please use "VS15" flag for Visual Studio 2015 build)

    3. After the build successfully completes, the 8MB firmware binary image will be located in the
       following location on your local hard drive:

      "C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Common\Tools\Stitch"

  2b. Build Steps (64-bit)
    1. To build a 64-bit release version
      * Open a command prompt of Microsoft Visual Studio, such as "Developer Command Prompt for VS2013";
      * Type the command: `cd C:\MyWorkspace\edk2-platforms`
      * Type the command: `BuildBIOS.bat /VS13 /x64 Broxton Release` to build a release version.
        (Please use "VS15" flag for Visual Studio 2015 build)

    2. To build a 64-bit debug version
      * Open a command prompt of Microsoft Visual Studio, such as "Developer Command Prompt for VS2013";
      * Type the command: `cd C:\MyWorkspace\edk2-platforms`
      * Type the command: `BuildBIOS.bat /VS13 /x64 Broxton Debug` to build a debug version.
        (Please use "VS15" flag for Visual Studio 2015 build)

    3. After the build successfully completes, the 8MB firmware binary image will be located in the
      following location on your local hard drive:

      "C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Common\Tools\Stitch"

## MAINTAINERS

* david.wei@intel.com
* mang.guo@intel.com
* vincent.zimmer@intel.com
* mike.wu@intel.com
* shifeix.a.lu@intel.com

