/** @file
  File to contain all the hardware specific stuff for the Smm Ichn dispatch protocol.

  Copyright (c) 2012 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "ScSmmHelpers.h"
#include "PlatformBaseAddresses.h"

SC_SMM_SOURCE_DESC ICHN_SOURCE_DESCS[NUM_ICHN_TYPES] = {
  ///
  /// IchnMch
  ///
  {
    SC_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },

    {
      NULL_BIT_DESC_INITIALIZER
    }
  },

  ///
  /// IchnPme
  ///
  {
    SC_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },

    {
      NULL_BIT_DESC_INITIALIZER
    }
  },

  ///
  /// IchnRtcAlarm
  ///
  {
    SC_SMM_SCI_EN_DEPENDENT,
    {
      {
        {
          ACPI_ADDR_TYPE,
          {R_ACPI_PM1_EN}
        },
        S_ACPI_PM1_EN,
        N_ACPI_PM1_EN_RTC
      },
      NULL_BIT_DESC_INITIALIZER
    },

    {
      {
        {
          ACPI_ADDR_TYPE,
          {R_ACPI_PM1_STS}
        },
        S_ACPI_PM1_STS,
        N_ACPI_PM1_STS_RTC
      }
    }
  },

  ///
  /// IchnRingIndicate
  ///
  {
    SC_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },

    {
      NULL_BIT_DESC_INITIALIZER
    }
  },

  ///
  /// IchnAc97Wake
  /// Not supported,
  /// we just fill in invalid initializer and not use it.
  ///
  {
    SC_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },

    {
      NULL_BIT_DESC_INITIALIZER
    }
  },

  ///
  /// IchnSerialIrq
  ///
  {
    SC_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },

    {
      {
        {
          ACPI_ADDR_TYPE,
          {R_SMI_STS}
        },
        S_SMI_STS,
        N_SMI_STS_SERIRQ
      }
    }
  },

  ///
  /// IchnY2KRollover
  ///
  {
    SC_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },

    {
      NULL_BIT_DESC_INITIALIZER
    }
  },

  ///
  /// IchnTcoTimeout
  ///
  {
    SC_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          {R_SMI_EN}
        },
        S_SMI_EN,
        N_SMI_EN_TCO
      },
      NULL_BIT_DESC_INITIALIZER
    },

    {
      {
        {
          ACPI_ADDR_TYPE,
          {R_SMI_STS}
        },
        S_SMI_STS,
        N_SMI_STS_TCO
      }
    }
  },

  ///
  /// IchnOsTco
  ///
  {
    SC_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },

    {
      NULL_BIT_DESC_INITIALIZER
    }
  },

  ///
  /// IchnNmi
  ///
  {
    SC_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          {R_SMI_EN}
        },
        S_SMI_EN,
        N_SMI_EN_TCO
      },

      {
        {
          PCR_ADDR_TYPE,
          {SC_PCR_ADDRESS (0xD0, R_PCR_ITSS_NMICSTS)}
        },
        S_PCR_ITSS_NMICSTS,
        N_PCR_ITSS_NMI2SMIEN
      }
    },

    {
      {
        {
          PCR_ADDR_TYPE,
          {SC_PCR_ADDRESS (0xD0, R_PCR_ITSS_NMICSTS)}
        },
        S_PCR_ITSS_NMICSTS,
        N_PCR_ITSS_NMI2SMISTS
      }
    }
  },

  ///
  /// IchnIntruderDetect
  ///
  {
    SC_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },

    {
      NULL_BIT_DESC_INITIALIZER
    }
  },

  ///
  /// IchnBiosWp
  ///
#ifdef PCIESC_SUPPORT
  {
    SC_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          {R_SMI_EN}
        },
        S_SMI_EN,
        N_SMI_EN_SPI_SSMI
      },

      {
        {
          PCIE_ADDR_TYPE,
          {
            (DEFAULT_PCI_BUS_NUMBER_SC << 24) |
            (PCI_DEVICE_NUMBER_SPI << 16) |
            (PCI_FUNCTION_NUMBER_SPI << 8) |
            R_SPI_BCR
          }
        },
        S_SPI_BCR,
        N_SPI_BCR_BLE
      }
    },

    {
      {
        {
          ACPI_ADDR_TYPE,
          {R_SMI_STS}
        },
        S_SMI_STS,
        N_SMI_STS_SPI_SSMI
      }
    }
  },
#else
  {
    SC_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },

    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
#endif

  ///
  /// IchnMcSmi
  ///
  {
    SC_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },

    {
      NULL_BIT_DESC_INITIALIZER
    }
  },

  ///
  /// IchnPmeB0
  ///
  {
    SC_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },

    {
      NULL_BIT_DESC_INITIALIZER
    }
  },

  ///
  /// IchnThrmSts
  ///
  {
    SC_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },

    {
      NULL_BIT_DESC_INITIALIZER
    }
  },

  ///
  /// IchnIntelUsb2
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// IchnMonSmi7
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// IchnMonSmi6
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// IchnMonSmi5
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// IchnMonSmi4
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// IchnDevTrap13
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// IchnDevTrap12, KBC_ACT_STS
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// IchnDevTrap11
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// IchnDevTrap10
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// IchnDevTrap9, PIRQDH_ACT_STS
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// IchnDevTrap8, PIRQCG_ACT_STS
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// IchnDevTrap7, PIRQBF_ACT_STS
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// IchnDevTrap6, PIRQAE_ACT_STS
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// IchnDevTrap5
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// IchnDevTrap3
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// IchnDevTrap2
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// IchnDevTrap1
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// IchnDevTrap0, IDE_ACT_STS
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// SC I/O Trap register 3 monitor,
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// SC I/O Trap register 2 monitor
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// SC I/O Trap register 1 monitor
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// SC I/O Trap register 0 monitor
  ///
  NULL_SOURCE_DESC_INITIALIZER,
};


SC_SMM_SOURCE_DESC ICHN_EX_SOURCE_DESCS[IchnExTypeMAX - IchnExPciExpress] = {
  ///
  /// IchnExPciExpress
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// IchnExMonitor
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// IchnExSpi
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// IchnExQRT
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// IchnExGpioUnlock
  ///
  {
    SC_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },

    {
      NULL_BIT_DESC_INITIALIZER
    }
  },

  ///
  /// IchnExTmrOverflow
  ///
  {
    SC_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          {R_ACPI_PM1_EN}
        },
        S_ACPI_PM1_EN,
        N_ACPI_PM1_EN_TMROF
      },
      NULL_BIT_DESC_INITIALIZER
    },

    {
      {
        {
          ACPI_ADDR_TYPE,
          {R_ACPI_PM1_STS}
        },
        S_ACPI_PM1_STS,
        N_ACPI_PM1_STS_TMROF
      }
    }
  },
};


/**
  Clear the SMI status bit after the SMI handling is done

  @param[in] SrcDesc              Pointer to the SC SMI source description table

  @retval    None

**/
VOID
EFIAPI
ScSmmIchnClearSource (
  IN SC_SMM_SOURCE_DESC           *SrcDesc
  )
{
  ScSmmClearSource (SrcDesc);
}

