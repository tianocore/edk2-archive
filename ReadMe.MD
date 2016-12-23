
This code base is designed for the MinnowBoard v3 platform using the Intel(R) Atom(tm) Processor E3900 Series.

================================================================================
                                    INDEX 
================================================================================
1.  HOW TO CREATE A FULL SOURCE TREE
2.  HOW TO BUILD (WINDOWS ENVIRONMENT)
3.  MAINTAINERS
      
================================================================================
1.                           HOW TO CREATE A FULL SOURCE TREE 
================================================================================
1) Create a new folder (directory) on the root of your local development machine
   for use as your work space (this example uses "C:\MyWorkspace").
   
   NOTE: Some code paths are very long, and placing the working directory too
         deep below the ROOT directory may cause the path to be longer than the 
         operating system or file system's maximum path length.

2) Get MinnowBoard v3 branch from GitHub with the following command.
   a. git clone https://github.com/tianocore/edk2-platforms.git -b devel-MinnowBoard3
  
   Once the download completes, all EDK II packages and files will be found
   under the "C:\MyWorkspace\edk2-platforms" directory.

3) Get the BaseTools binary and copy them to BaseTools binary folder.
   a. git clone https://github.com/tianocore/edk2-BaseTools-win32.git
   b. Enter folder edk2-BaseTools-win32
   b. git checkout f0c97b7716ce92f1fc657f87f29bcc9170e5eb1f
   c. Rename folder name from "edk2-BaseTools-win32/" to "win32/" and copy
      win32/ to the BaseTools Binary folder.
      (C:\MyWorkspace\edk2-platforms\BaseTools\Bin\)

4) Get the Intel(R) Firmware Support Package(Intel FSP)
   a. Run "git clone -b ApolloLake https://github.com/IntelFsp/FSP.git" to download FSP.
   b. Copy the ApolloLakeFspBinPkg to the folder
      "C:\MyWorkspace\edk2-platforms\Silicon\BroxtonSoC\BroxtonFspPkg".
   
5) Install UEFI UNDI driver:
   a. Download PREBOOT.EXE Version 21.1 from the "Intel® Ethernet Connections 
      Boot Utility, Preboot Images, and EFI Drivers" page.
      https://downloadcenter.intel.com/download/19186/Intel-Ethernet-Connections-Boot-Utility-Preboot-Images-and-EFI-Drivers
   b. Install PREBOOT.EXE into the default folder (C:\Intel21.1).
   c. Copy the UEFI x64 PCI-E gigabit driver (C:\Intel21.1\APPS\EFI\EFIx64\EnnnnX3.EFI,
      where "nnnn" is the driver version number) to the platform package directory below, create the folder if it does not exist:
      "C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Common\Binaries\UNDI\I210PcieUndiDxe"
   d. Open the file "C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\PlatformPkg.fdf"
      and change the UNDI driver file name reference with the one that was downloaded:
         SECTION PE32 = BroxtonPlatformPkg/Common/Binaries/UNDI/I210PcieUndiDxe/E7006X3.EFI
   
6) Install the NASM assembly language compiler:                                             
   a. Download NASM 2.12.02 binaries: 
      http://www.nasm.us/pub/nasm/releasebuilds/2.12.02/win32/nasm-2.12.02-win32.zip
      http://www.nasm.us/pub/nasm/releasebuilds/2.12.02/win64/nasm-2.12.02-win64.zip
   b. Unzip the 32-bit & 64-bit versions of nasm.exe to the directories listed below,
      respectively, and create the folders if they do not exist:
      "C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Common\Tools\nasm\Win32"
      "C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Common\Tools\nasm\Win64"

7) Install the ACPI Source Language (ASL) compiler:
   a. Install the IASL 2016-09-30 compiler from acpica.org:
      https://acpica.org/sites/acpica/files/iasl-win-20160930.zip
   b. Unzip the "iasl.exe" file into the directory below, and create the folder
      if it does not exist:
      "C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Common\Tools\Iasl"
      
8) Follow the instructions found in the "Patch-HOWTO.txt" file located in the Workspace
   (e.g. "C:\MyWorkspace\edk2-platforms\Core\CryptoPkg\Library\OpensslLib\Patch-HOWTO.txt")
   to install the OpenSSL source code and enable the CryptoPkg module.   

9) Download MinnowBoard v3 Binary Object Modules
   a. Download the "MinnowBoard_v3-0.60-Binary.Objects.zip" from the project page:
      https://firmware.intel.com/projects/minnowboardv3
   b. Unzip and copy the two folders (A_Stepping, B_Stepping) into
      the directory below, and create the folder if it does nto exist:
      "C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Common\Binaries\IFWI"

================================================================================
2.                           HOW TO BUILD (WINDOWS ENVIRONMENT)
================================================================================
Windows System Configuration:
  Microsoft Windows 7 Ultimate 64-bit*
  
1) Setup Build Environment

   1) Install C compiler (Visual Studio .NET VS2013/VS2015) in the 
      build machine.
      
   2) Install version Python 2.7.10 from: https://www.python.org/downloads/release/python-2710/
      Make sure that a file with the extension of ".py" will be opened by Python.exe.
     
2) Build

2a) Build Steps (32-bit) 
   1) To build a 32-bit release version
      Open a command prompt of Microsoft Visual Studio, such as "Developer Command Prompt for VS2013";
      Type the command: "cd C:\MyWorkspace\edk2-platforms"; 
      Type the command: "BuildBIOS.bat /VS13 /IA32 Broxton Release" to build a release version.
      (Please use "VS15" flag for Visual Studio 2015 build)
      
   2) To build a 32-bit debug version
      Open a command prompt of Microsoft Visual Studio, such as "Developer Command Prompt for VS2013";
      Type the command: "cd C:\MyWorkspace\edk2-platforms"; 
      Type the command: "BuildBIOS.bat /VS13 /IA32 Broxton Debug" to build a debug version.
      (Please use "VS15" flag for Visual Studio 2015 build)
               
   3) After the build successfully completes, the 8MB firmware binary image will be located in the
      following location on your local hard drive: 
      
      "C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Common\Tools\Stitch"

2b) Build Steps (64-bit) 
   1) To build a 64-bit release version
      Open a command prompt of Microsoft Visual Studio, such as "Developer Command Prompt for VS2013";
      Type the command: "cd C:\MyWorkspace\edk2-platforms"; 
      Type the command: "BuildBIOS.bat /VS13 /x64 Broxton Release" to build a release version.
      (Please use "VS15" flag for Visual Studio 2015 build)
      
   2) To build a 64-bit debug version
      Open a command prompt of Microsoft Visual Studio, such as "Developer Command Prompt for VS2013";
      Type the command: "cd C:\MyWorkspace\edk2-platforms"; 
      Type the command: "BuildBIOS.bat /VS13 /x64 Broxton Debug" to build a debug version.
      (Please use "VS15" flag for Visual Studio 2015 build)
               
   3) After the build successfully completes, the 8MB firmware binary image will be located in the
      following location on your local hard drive: 
      
      "C:\MyWorkspace\edk2-platforms\Platform\BroxtonPlatformPkg\Common\Tools\Stitch"

================================================================================
                                  MAINTAINERS
================================================================================
david.wei@intel.com
mang.guo@intel.com
vincent.zimmer@intel.com
mike.wu@intel.com 
shifeix.a.lu@intel.com
