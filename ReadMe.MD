# ﻿MinnowBoard 3 and Leaf Hill CRB Platform

This codebase is designed for the MinnowBoard 3 platform and Leaf Hill Customer Reference Board (CRB), using the [Intel Atom(R) Processor E3900 Series](https://www.intel.com/content/www/us/en/embedded/products/apollo-lake/overview.html) (formerly Apollo Lake).

## Codebase Overview

This codebase is based on UDK2017 core packages, [Intel(R) Firmware Support Package](https://github.com/IntelFsp/FSP.git)
(Intel(R) FSP), [UEFI UNDI driver](https://downloadcenter.intel.com/download/19186/Intel-Ethernet-Connections-Boot-Utility-Preboot-Images-and-EFI-Drivers)
for Intel Ethernet Controllers, and pre-compiled binary objects required for the final Integrated Firmware Image (IFWI).

Additional information on this project is available at [firmware.intel.com](https://firmware.intel.com/projects/minnowboard3).

## Build Instructions

Instructions for downloading a source tree and compiling firmware are available in the project release notes at [firmware.intel.com](https://firmware.intel.com/projects/minnowboard3):
* https://firmware.intel.com/sites/default/files/MinnowBoard3-Rel_0_69-ReleaseNotes.txt
* https://firmware.intel.com/sites/default/files/LeafHill-Rel_0_69-ReleaseNotes.txt

*Note that Release 0.69 added a separate [release notes file for the Leaf Hill CRB](https://firmware.intel.com/sites/default/files/LeafHill-Rel_0_69-ReleaseNotes.txt). Instructions for downloading  the Leaf Hill CRB source tree and compiling an IFWI image differ slightly from MinnowBoard 3.*

This project's build instructions have been validated in the following environments:
* Microsoft Windows 7 Ultimate 64-bit & Microsoft Windows 10 Enterprise 64-bit with Microsoft Visual Studio 2013/2015
* Ubuntu Linux 16.04.3.LTS with GCC 5

## Pre-Compiled Firmware Images

Pre-compiled binary images are available at https://firmware.intel.com/projects/minnowboard3

This [project](https://firmware.intel.com/projects/minnowboard3) also includes IA32 & x64 'flash update' tools for the UEFI Shell (FirmwareUpdateUtilities).
These utilities require the 'BIOS Lock' feature to be disabled. Please refer to the release notes for programming instructions.

## Reporting Issues

To report issues on this codebase, please use [TianoCore Bugzilla](https://bugzilla.tianocore.org/). For more information, please refer to the [Reporting Issues](https://github.com/tianocore/tianocore.github.io/wiki/Reporting-Issues) entry on the [TianoCore wiki](https://github.com/tianocore/tianocore.github.io/wiki).

## MAINTAINERS

* david.wei@intel.com
* mang.guo@intel.com
* vincent.zimmer@intel.com
* mike.wu@intel.com
* shifeix.a.lu@intel.com
