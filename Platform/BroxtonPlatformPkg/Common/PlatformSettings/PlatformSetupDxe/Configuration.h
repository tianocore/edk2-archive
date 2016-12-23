/** @file
  Driver configuration include file.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

//
// System Setup Page. Do not have to be sequential but have to be unique
//
#define ROOT_FORM_ID                        1
#define ROOT_MAIN_FORM_ID                   2
#define CPU_CONFIGURATION_FORM_ID           3
#define CPU_PWR_CONFIGURATION_FORM_ID       4
#define BOOT_CONFIGURATION_FORM_ID          5
#define IGD_FORM_ID                         6
#define SECURITY_CONFIGURATION_FORM_ID      7
#define SOUTH_CLUSTER_FORM_ID               8
#define DPTF_FORM_ID                        9
#define PLATFORM_INFORMATION_FORM_ID        10
#define SATA_CONFIGURATION_ID               11
#define SENSOR_CONFIGURATION_ID             12
#define LPSS_OPTIONS_FORM_ID                13
#define UNCORE_FORM_ID                      14
#define TPM_FORM_ID                         15
#define THERMAL_FORM_ID                     16
#define PASSWORD_SETTING_ID                 17
#define LAN_OPTIONS_FORM_ID                 18
#define HDAUDIO_OPTIONS_FORM_ID             19
#define MISC_OPTIONS_FORM_ID                20
#define USB_OPTIONS_FORM_ID                 21
#define PCIE_DEVICE_OPTIONS_FORM_ID         22
#define SYSTEM_COMPONENT_FORM_ID            23
#define DEBUG_CONFIGURATION_FORM_ID         24
#define SCC_OPTIONS_FORM_ID                 25
#define PCIERP1_OPTIONS_FORM_ID             26
#define PCIERP2_OPTIONS_FORM_ID             27
#define PCIERP3_OPTIONS_FORM_ID             28
#define PCIERP4_OPTIONS_FORM_ID             29
#define PCIERP5_OPTIONS_FORM_ID             30
#define PCIERP6_OPTIONS_FORM_ID             31
#define GMM_OPTIONS_FORM_ID                 32
#define ISH_OPTIONS_FORM_ID                 33
#define ACPI_SETUP_FORM_ID                  34
#define PEP_FORM_ID                         35
#define RTD3_FORM_ID                        36
#define DPTF_POLICY_FORM_ID                 46

#define DEBUG_NPK_FORM_ID                   42

//
// Other defines
//
#define OFFSET_0                            0
#define OFFSET_1                            1
#define OFFSET_2                            2
#define OFFSET_3                            3
#define OFFSET_4                            4
#define OFFSET_5                            5
#define OFFSET_6                            6
#define OFFSET_7                            7
#endif // #ifndef _CONFIGURATION_H

