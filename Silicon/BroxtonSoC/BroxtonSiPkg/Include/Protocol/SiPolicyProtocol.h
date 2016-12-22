/** @file
  Protocol used for specifying platform related Silicon information and policy setting.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SI_POLICY_PROTOCOL_H_
#define _SI_POLICY_PROTOCOL_H_

#include <IndustryStandard/Hsti.h>

//
// DXE_SI_POLICY_PROTOCOL revisions
//
#define DXE_SI_POLICY_PROTOCOL_REVISION_1 1

extern EFI_GUID gDxeSiPolicyProtocolGuid;


/**
  The protocol allows the platform code to publish a set of configuration information that the
  Silicon drivers will use to configure the processor in the DXE phase.
  This Policy Protocol needs to be initialized for Silicon configuration.
  @note The Protocol has to be published before processor DXE drivers are dispatched.
**/
typedef struct {
  /**
  This member specifies the revision of the Si Policy protocol. This field is used to indicate backward
  compatible changes to the protocol. Any such changes to this protocol will result in an update in the revision number.

  <b>Revision 1</b>:
   - Initial version
  **/

  UINT8                          Revision;
/**
    This member describes a pointer to Hsti results from previous boot. In order to mitigate the large performance cost
    of performing all of the platform security tests on each boot, we can save the results across boots and retrieve
    and point this policy to them prior to the launch of HstiSiliconDxe. Logic should be implemented to not populate this
    upon major platform changes (i.e changes to setup option or platform hw)to ensure that results accurately reflect the
    configuration of the platform.
**/
  ADAPTER_INFO_PLATFORM_SECURITY *Hsti;    ///< This is a pointer to Hsti results from previous boot
  UINTN                          HstiSize; ///< Size of results, if setting Hsti policy to point to previous results
} DXE_SI_POLICY_PROTOCOL;

#endif

