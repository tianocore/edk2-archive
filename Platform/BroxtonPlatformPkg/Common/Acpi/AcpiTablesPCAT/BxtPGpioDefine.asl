/** @file
  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _BXTP_GPIO_DEFINE_ASL_
#define _BXTP_GPIO_DEFINE_ASL_

//
// GPIO pad and offset definition as GPIO HAS
// North community GPIO pad definition
//
#define N_GPIO_0              0x00C50500     //GPIO_0
#define N_GPIO_1              0x00C50508     //GPIO_1
#define N_GPIO_2              0x00C50510     //GPIO_2
#define N_GPIO_3              0x00C50518     //GPIO_3
#define N_GPIO_4              0x00C50520     //GPIO_4
#define N_GPIO_5              0x00C50528     //GPIO_5
#define N_GPIO_6              0x00C50530     //GPIO_6
#define N_GPIO_7              0x00C50538     //GPIO_7
#define N_GPIO_8              0x00C50540     //GPIO_8
#define N_GPIO_9              0x00C50548     //GPIO_9
#define N_GPIO_10             0x00C50550     //GPIO_10
#define N_GPIO_11             0x00C50558     //GPIO_11
#define N_GPIO_12             0x00C50560     //GPIO_12
#define N_GPIO_13             0x00C50568     //GPIO_13
#define N_GPIO_14             0x00C50570     //GPIO_14
#define N_GPIO_15             0x00C50578     //GPIO_15
#define N_GPIO_16             0x00C50580     //GPIO_16
#define N_GPIO_17             0x00C50588     //GPIO_17
#define N_GPIO_18             0x00C50590     //GPIO_18
#define N_GPIO_19             0x00C50598     //GPIO_19
#define N_GPIO_20             0x00C505A0     //GPIO_20
#define N_GPIO_21             0x00C505A8     //GPIO_21
#define N_GPIO_22             0x00C505B0     //GPIO_22
#define N_GPIO_23             0x00C505B8     //GPIO_23
#define N_GPIO_24             0x00C505C0     //GPIO_24
#define N_GPIO_25             0x00C505C8     //GPIO_25
#define N_GPIO_26             0x00C505D0     //GPIO_26
#define N_GPIO_27             0x00C505D8     //GPIO_27
#define N_GPIO_28             0x00C505E0     //GPIO_28
#define N_GPIO_29             0x00C505E8     //GPIO_29
#define N_GPIO_30             0x00C505F0     //GPIO_30
#define N_GPIO_31             0x00C505F8     //GPIO_31
#define N_GPIO_32             0x00C50600     //GPIO_32
#define N_GPIO_33             0x00C50608     //GPIO_33
#define N_GPIO_34             0x00C50610     //PWM0
#define N_GPIO_35             0x00C50618     //PWM1
#define N_GPIO_36             0x00C50620     //PWM2
#define N_GPIO_37             0x00C50628     //PWM3
#define N_GPIO_38             0x00C50630     //LPSS_UART0_RXD
#define N_GPIO_39             0x00C50638     //LPSS_UART0_TXD
#define N_GPIO_40             0x00C50640     //LPSS_UART0_RTS_B
#define N_GPIO_41             0x00C50648     //LPSS_UART0_CTS_B
#define N_GPIO_42             0x00C50650     //LPSS_UART1_RXD
#define N_GPIO_43             0x00C50658     //LPSS_UART1_TXD
#define N_GPIO_44             0x00C50660     //LPSS_UART1_RTS_B
#define N_GPIO_45             0x00C50668     //LPSS_UART1_CTS_B
#define N_GPIO_46             0x00C50670     //LPSS_UART2_RXD
#define N_GPIO_47             0x00C50678     //LPSS_UART2_TXD
#define N_GPIO_48             0x00C50680     //LPSS_UART2_RTS_B
#define N_GPIO_49             0x00C50688     //LPSS_UART2_CTS_B
#define N_GPIO_62             0x00C50690     //GP_CAMERASB00
#define N_GPIO_63             0x00C50698     //GP_CAMERASB01
#define N_GPIO_64             0x00C506A0     //GP_CAMERASB02
#define N_GPIO_65             0x00C506A8     //GP_CAMERASB03
#define N_GPIO_66             0x00C506B0     //GP_CAMERASB04
#define N_GPIO_67             0x00C506B8     //GP_CAMERASB05
#define N_GPIO_68             0x00C506C0     //GP_CAMERASB06
#define N_GPIO_69             0x00C506C8     //GP_CAMERASB07
#define N_GPIO_70             0x00C506D0     //GP_CAMERASB08
#define N_GPIO_71             0x00C506D8     //GP_CAMERASB09
#define N_GPIO_72             0x00C506E0     //GP_CAMERASB10
#define N_GPIO_73             0x00C506E8     //GP_CAMERASB11
#define N_TCK                 0x00C506F0     //TCK
#define N_TRST_B              0x00C506F8     //TRST_B
#define N_TMS                 0x00C50700     //TMS
#define N_TDI                 0x00C50708     //TDI
#define N_CX_PMODE            0x00C50710     //CX_PMODE
#define N_CX_PREQ_B           0x00C50718     //CX_PREQ_B
#define N_JTAGX               0x00C50720     //JTAGX
#define N_CX_PRDY_B           0x00C50728     //CXPRDY_B
#define N_TDO                 0x00C50730     //TDO
#define N_CNV_BRI_DT          0x00C50738     //CNV_BRI_DT
#define N_CNV_BRI_RSP         0x00C50740     //CNV_BRI_RSP
#define N_CNV_RGI_DT          0x00C50748     //CNV_RGI_DT
#define N_CNV_RGI_RSP         0x00C50750     //CNV_RGI_RSP
#define N_SVID0_ALERT_B       0x00C50758     //SVID0_ALERT_B
#define N_SVID0_DATA          0x00C50760     //SVID0_DATA
#define N_SVID0_CLK           0x00C50768     //SVID0_CLK

// Northwest community GPIO pad definition
#define NW_GPIO_187           0x00C40500    //HV_DDI0_DDC_SDA
#define NW_GPIO_188           0x00C40508    //HV_DDI0_DDC_SCL
#define NW_GPIO_189           0x00C40510    //HV_DDI1_DDC_SDA
#define NW_GPIO_190           0x00C40518    //HV_DDI1_DDC_SCL
#define NW_GPIO_191           0x00C40520    //DBI_SDA
#define NW_GPIO_192           0x00C40528    //DBI_SCL
#define NW_GPIO_193           0x00C40530    //PANEL0_VDDEN
#define NW_GPIO_194           0x00C40538    //PANEL0_BKLTEN
#define NW_GPIO_195           0x00C40540    //PANEL0_BKLTCTL
#define NW_GPIO_196           0x00C40548    //PANEL1_VDDEN
#define NW_GPIO_197           0x00C40550    //PANEL1_BKLTEN
#define NW_GPIO_198           0x00C40558    //PANEL1_BKLTCTL
#define NW_GPIO_199           0x00C40560    //DBI_CSX
#define NW_GPIO_200           0x00C40568    //DBI_RESX
#define NW_GPIO_201           0x00C40570    //GP_INTD_DSI_TE1
#define NW_GPIO_202           0x00C40578    //GP_INTD_DSI_TE2
#define NW_GPIO_203           0x00C40580    //USB_OC0_B
#define NW_GPIO_204           0x00C40588    //USB_OC1_B
#define NW_PMC_SPI_FS0        0x00C40590    //PMC_SPI_FS0
#define NW_PMC_SPI_FS1        0x00C40598    //PMC_SPI_FS1
#define NW_PMC_SPI_FS2        0x00C405A0    //PMC_SPI_FS2
#define NW_PMC_SPI_RXD        0x00C405A8    //PMC_SPI_RXD
#define NW_PMC_SPI_TXD        0x00C405B0    //PMC_SPI_TXD
#define NW_PMC_SPI_CLK        0x00C405B8    //PMC_SPI_CLK
#define NW_PMIC_PWRGOOD       0x00C405C0    //PMIC_PWRGOOD
#define NW_PMIC_RESET_B       0x00C405C8    //PMIC_RESET_B
#define NW_GPIO_213           0x00C405D0    //PMIC_SDWN_B
#define NW_GPIO_214           0x00C405D8    //PMIC_BCUDISW2
#define NW_GPIO_215           0x00C405E0    //PMIC_BCUDISCRIT
#define NW_PMIC_THERMTRIP_B   0x00C405E8    //PMIC_THERMTRIP_B
#define NW_PMIC_STDBY         0x00C405F0    //PMIC_STDBY
#define NW_PROCHOT_B          0x00C405F8    //PROCHOT_B
#define NW_PMIC_I2C_SCL       0x00C40600    //PMIC_I2C_SCL
#define NW_PMIC_I2C_SDA       0x00C40608    //PMIC_I2C_SDA
#define NW_GPIO_74            0x00C40610    //AVS_I2S1_MCLK
#define NW_GPIO_75            0x00C40618    //AVS_I2S1_BCLK
#define NW_GPIO_76            0x00C40620    //AVS_I2S1_WS_SYNC
#define NW_GPIO_77            0x00C40628    //AVS_I2S1_SDI
#define NW_GPIO_78            0x00C40630    //AVS_I2S1_SDO
#define NW_GPIO_79            0x00C40638    //AVS_M_CLK_A1
#define NW_GPIO_80            0x00C40640    //AVS_M_CLK_B1
#define NW_GPIO_81            0x00C40648    //AVS_M_DATA_1
#define NW_GPIO_82            0x00C40650    //AVS_M_CLK_AB2
#define NW_GPIO_83            0x00C40658    //AVS_M_DATA_2
#define NW_GPIO_84            0x00C40660    //AVS_I2S2_MCLK
#define NW_GPIO_85            0x00C40668    //AVS_I2S2_BCLK
#define NW_GPIO_86            0x00C40670    //AVS_I2S2_WS_SYNC
#define NW_GPIO_87            0x00C40678    //AVS_I2S2_SDI
#define NW_GPIO_88            0x00C40680    //AVS_I2S2_SDO
#define NW_GPIO_89            0x00C40688    //AVS_I2S3_BCLK
#define NW_GPIO_90            0x00C40690    //AVS_I2S3_WS_SYNC
#define NW_GPIO_91            0x00C40698    //AVS_I2S3_SDI
#define NW_GPIO_92            0x00C406A0    //AVS_I2S3_SDO
#define NW_GPIO_97            0x00C406A8    //FST_SPI_CS0_B
#define NW_GPIO_98            0x00C406B0    //FST_SPI_CS1_B
#define NW_GPIO_99            0x00C406B8    //FST_SPI_MOSI_IO0
#define NW_GPIO_100           0x00C406C0    //FST_SPI_MISO_IO1
#define NW_GPIO_101           0x00C406C8    //FST_SPI_IO2
#define NW_GPIO_102           0x00C406D0    //FST_SPI_IO3
#define NW_GPIO_103           0x00C406D8    //FST_SPI_CLK
#define NW_FST_SPI_CLK_FB     0x00C406E0    //FST_SPI_CLK_FB
#define NW_GPIO_104           0x00C406E8    //GP_SSP_0_CLK
#define NW_GPIO_105           0x00C406F0    //GP_SSP_0_FS0
#define NW_GPIO_106           0x00C406F8    //GP_SSP_0_FS1
#define NW_GPIO_109           0x00C40700    //GP_SSP_0_RXD
#define NW_GPIO_110           0x00C40708    //GP_SSP_0_TXD
#define NW_GPIO_111           0x00C40710    //GP_SSP_1_CLK
#define NW_GPIO_112           0x00C40718    //GP_SSP_1_FS0
#define NW_GPIO_113           0x00C40720    //GP_SSP_1_FS1
#define NW_GPIO_116           0x00C40728    //GP_SSP_1_RXD
#define NW_GPIO_117           0x00C40730    //GP_SSP_1_TXD
#define NW_GPIO_118           0x00C40738    //GP_SSP_2_CLK
#define NW_GPIO_119           0x00C40740    //GP_SSP_2_FS0
#define NW_GPIO_120           0x00C40748    //GP_SSP_2_FS1
#define NW_GPIO_121           0x00C40750    //GP_SSP_2_FS2
#define NW_GPIO_122           0x00C40758    //GP_SSP_2_RXD
#define NW_GPIO_123           0x00C40760    //GP_SSP_2_TXD

// West community GPIO pad definition
#define W_GPIO_124            0x00C70500     //LPSS_I2S0_SDA
#define W_GPIO_125            0x00C70508     //LPSS_I2S0_SCL
#define W_GPIO_126            0x00C70510     //LPSS_I2S1_SDA
#define W_GPIO_127            0x00C70518     //LPSS_I2S1_SCL
#define W_GPIO_128            0x00C70520     //LPSS_I2S2_SDA
#define W_GPIO_129            0x00C70528     //LPSS_I2S2_SCL
#define W_GPIO_130            0x00C70530     //LPSS_I2S3_SDA
#define W_GPIO_131            0x00C70538     //LPSS_I2S3_SCL
#define W_GPIO_132            0x00C70540     //LPSS_I2S4_SDA
#define W_GPIO_133            0x00C70548     //LPSS_I2S4_SCL
#define W_GPIO_134            0x00C70550     //LPSS_I2S5_SDA
#define W_GPIO_135            0x00C70558     //LPSS_I2S5_SCL
#define W_GPIO_136            0x00C70560     //LPSS_I2S6_SDA
#define W_GPIO_137            0x00C70568     //LPSS_I2S6_SCL
#define W_GPIO_138            0x00C70570     //LPSS_I2S7_SDA
#define W_GPIO_139            0x00C70578     //LPSS_I2S7_SCL
#define W_GPIO_146            0x00C70580     //ISH_GPIO_0
#define W_GPIO_147            0x00C70588     //ISH_GPIO_1
#define W_GPIO_148            0x00C70590     //ISH_GPIO_2
#define W_GPIO_149            0x00C70598     //ISH_GPIO_3
#define W_GPIO_150            0x00C705a0     //ISH_GPIO_4
#define W_GPIO_151            0x00C705a8     //ISH_GPIO_5
#define W_GPIO_152            0x00C705b0     //ISH_GPIO_6
#define W_GPIO_153            0x00C705b8     //ISH_GPIO_7
#define W_GPIO_154            0x00C705c0     //ISH_GPIO_8
#define W_GPIO_155            0x00C705c8     //ISH_GPIO_9
#define W_GPIO_209            0x00C705d0     //PCIE_CLKREQ0_B
#define W_GPIO_210            0x00C705d8     //PCIE_CLKREQ1_B
#define W_GPIO_211            0x00C705e0     //PCIE_CLKREQ2_B
#define W_GPIO_212            0x00C705e8     //PCIE_CLKREQ3_B
#define W_OSC_CLK_OUT_0       0x00C705f0     //OSC_CLK_OUT0
#define W_OSC_CLK_OUT_1       0x00C705f8     //OSC_CLK_OUT1
#define W_OSC_CLK_OUT_2       0x00C70600     //OSC_CLK_OUT2
#define W_OSC_CLK_OUT_3       0x00C70608     //OSC_CLK_OUT3
#define W_OSC_CLK_OUT_4       0x00C70610     //OSC_CLK_OUT4
#define W_PMU_AC_PRESENT      0x00C70618     //PMU_AC_PRESENT
#define W_PMU_BATLOW_B        0x00C70620     //PMU_BATLOW_B
#define W_PMU_PLTRST_B        0x00C70628     //PMU_PLTRST_B
#define W_PMU_PWRBTN_B        0x00C70630     //PMU_PWRBTN_B
#define W_PMU_RESETBUTTON_B   0x00C70638     //PMU_RESETBUTTON_B
#define W_PMU_SLP_S0_B        0x00C70640     //PMU_SLP_S0_B
#define W_PMU_SLP_S3_B        0x00C70648     //PMU_SLP_S3_B
#define W_PMU_SLP_S4_B        0x00C70650     //PMU_SLP_S4_B
#define W_PMU_SUSCLK          0x00C70658     //PMU_SUSCLK
#define W_PMU_WAKE_B          0x00C70660     //PMU_WAKE_B
#define W_SUS_STAT_B          0x00C70668     //SUS_STAT_B
#define W_SUSPWRDNACK         0x00C70670     //SUSPWRDNACK

// Southwest community GPIO pad definition
#define SW_GPIO_205           0x00C00500    //PCIE_WAKE0_B
#define SW_GPIO_206           0x00C00508    //PCIE_WAKE1_B
#define SW_GPIO_207           0x00C00510    //PCIE_WAKE2_B
#define SW_GPIO_208           0x00C00518    //PCIE_WAKE3_B
#define SW_GPIO_156           0x00C00520    //EMMC0_CLK
#define SW_GPIO_157           0x00C00528    //EMMC0_D0
#define SW_GPIO_158           0x00C00530    //EMMC0_D1
#define SW_GPIO_159           0x00C00538    //EMMC0_D2
#define SW_GPIO_160           0x00C00540    //EMMC0_D3
#define SW_GPIO_161           0x00C00548    //EMMC0_D4
#define SW_GPIO_162           0x00C00550    //EMMC0_D5
#define SW_GPIO_163           0x00C00558    //EMMC0_D6
#define SW_GPIO_164           0x00C00560    //EMMC0_D7
#define SW_GPIO_165           0x00C00568    //EMMC0_CMD0
#define SW_GPIO_166           0x00C00570    //SDIO_CLK
#define SW_GPIO_167           0x00C00578    //SDIO_D0
#define SW_GPIO_168           0x00C00580    //SDIO_D1
#define SW_GPIO_169           0x00C00588    //SDIO_D2
#define SW_GPIO_170           0x00C00590    //SDIO_D3
#define SW_GPIO_171           0x00C00598    //SDIO_CMD
#define SW_GPIO_172           0x00C005A0    //SDCARD_CLK
#define SW_GPIO_179           0x00C005A8    //SDCARD_CLK_FB
#define SW_GPIO_173           0x00C005B0    //SDCARD_D0
#define SW_GPIO_174           0x00C005B8    //SDCARD_D1
#define SW_GPIO_175           0x00C005C0    //SDCARD_D2
#define SW_GPIO_176           0x00C005C8    //SDCARD_D3
#define SW_GPIO_177           0x00C005D0    //SDCARD_CD_B
#define SW_GPIO_178           0x00C005D8    //SDCARD_CMD
#define SW_GPIO_186           0x00C005E0    //SDCARD_LVL_WP
#define SW_GPIO_182           0x00C005E8    //EMMC0_STROBE
#define SW_GPIO_183           0x00C005F0    //SDIO_PWR_DOWN_B
#define SW_SMB_ALERTB         0x00C005F8    //SMB_ALERTB
#define SW_SMB_CLK            0x00C00600    //SMB_CLK
#define SW_SMB_DATA           0x00C00608    //SMB_DATA
#define SW_LPC_ILB_SERIRQ     0x00C00610    //LPC_ILB_SERIRQ
#define SW_LPC_CLKOUT0        0x00C00618    //LPC_CLKOUT0
#define SW_LPC_CLKOUT1        0x00C00620    //LPC_CLKOUT1
#define SW_LPC_AD0            0x00C00628    //LPC_AD0
#define SW_LPC_AD1            0x00C00630    //LPC_AD1
#define SW_LPC_AD2            0x00C00638    //LPC_AD2
#define SW_LPC_AD3            0x00C00640    //LPC_AD3
#define SW_LPC_CLKRUN         0x00C00648    //LPC_CLKRUN
#define SW_LPC_FRAMEB         0x00C00650    //LPC_FRAMEB

#endif

