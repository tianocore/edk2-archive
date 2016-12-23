/** @file
  This file include all platform action which can be customized by IBV/OEM.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/DevicePathLib.h>
#include "BdsPlatform.h"

#pragma pack(1)

typedef unsigned char  __u8;
typedef unsigned short __u16;
typedef unsigned int   __u32;
typedef unsigned long long __u64;

#define EDD_MBR_SIG_MAX 16
#define E820MAX 128
#define EDDMAXNR 6

struct screen_info {
  __u8  orig_x;
  __u8  orig_y;
  __u16 ext_mem_k;
  __u16 orig_video_page;
  __u8  orig_video_mode;
  __u8  orig_video_cols;
  __u16 unused2;
  __u16 orig_video_ega_bx;
  __u16 unused3;
  __u8  orig_video_lines;
  __u8  orig_video_isVGA;
  __u16 orig_video_points;

  //
  // VESA graphic mode -- linear frame buffer
  //
  __u16 lfb_width;
  __u16 lfb_height;
  __u16 lfb_depth;
  __u32 lfb_base;
  __u32 lfb_size;
  __u16 cl_magic, cl_offset;
  __u16 lfb_linelength;
  __u8  red_size;
  __u8  red_pos;
  __u8  green_size;
  __u8  green_pos;
  __u8  blue_size;
  __u8  blue_pos;
  __u8  rsvd_size;
  __u8  rsvd_pos;
  __u16 vesapm_seg;
  __u16 vesapm_off;
  __u16 pages;
  __u16 vesa_attributes;
  __u32 capabilities;
  __u8  _reserved[6];
};


struct apm_bios_info {
  __u16   version;
  __u16   cseg;
  __u32   offset;
  __u16   cseg_16;
  __u16   dseg;
  __u16   flags;
  __u16   cseg_len;
  __u16   cseg_16_len;
  __u16   dseg_len;
};

struct ist_info {
  __u32 signature;
  __u32 command;
  __u32 event;
  __u32 perf_level;
};

struct sys_desc_table {
  __u16 length;
  __u8  table[14];
};

struct efi_info {
  __u32 efi_loader_signature;
  __u32 efi_systab;
  __u32 efi_memdesc_size;
  __u32 efi_memdesc_version;
  __u32 efi_memmap;
  __u32 efi_memmap_size;
  __u32 efi_systab_hi;
  __u32 efi_memmap_hi;
};

struct setup_header {
  __u8   setup_sects;
  __u16  root_flags;
  __u32  syssize;
  __u16  ram_size;
  __u16  vid_mode;
  __u16  root_dev;
  __u16  boot_flag;
  __u16  jump;
  __u32  header;
  __u16  version;
  __u32  realmode_swtch;
  __u16  start_sys;
  __u16  kernel_version;
  __u8   type_of_loader;
  __u8   loadflags;
  __u16  setup_move_size;
  __u32  code32_start;
  __u32  ramdisk_image;
  __u32  ramdisk_size;
  __u32  bootsect_kludge;
  __u16  heap_end_ptr;
  __u16  _pad1;
  __u32  cmd_line_ptr;
  __u32  initrd_addr_max;
  __u32  kernel_alignment;
  __u8   relocatable_kernel;
  __u8   _pad2[3];
  __u32  cmdline_size;
  __u32  hardware_subarch;
  __u64  hardware_subarch_data;
  __u32  payload_offset;
  __u32  payload_length;
  __u64  setup_data;
};

struct edid_info {
  unsigned char dummy[128];
};

struct e820entry {
  __u64 addr;     ///< start of memory segment
  __u64 size;     ///< size of memory segment
  __u32 type;     ///< type of memory segment
};

struct edd_device_params {
  __u16 length;
  __u16 info_flags;
  __u32 num_default_cylinders;
  __u32 num_default_heads;
  __u32 sectors_per_track;
  __u64 number_of_sectors;
  __u16 bytes_per_sector;
  __u32 dpte_ptr;                 ///< 0xFFFFFFFF for our purposes
  __u16 key;                      ///< = 0xBEDD
  __u8 device_path_info_length;   ///< = 44
  __u8 reserved2;
  __u16 reserved3;
  __u8 host_bus_type[4];
  __u8 interface_type[8];
  union {
    struct {
      __u16 base_address;
      __u16 reserved1;
      __u32 reserved2;
    }  isa;
    struct {
      __u8 bus;
      __u8 slot;
      __u8 function;
      __u8 channel;
      __u32 reserved;
    }  pci;

    //
    // pcix is same as pci
    //
  struct {
      __u64 reserved;
    }  ibnd;
    struct {
      __u64 reserved;
    }  xprs;
    struct {
      __u64 reserved;
    }  htpt;
    struct {
      __u64 reserved;
    }  unknown;
  } interface_path;
  union {
    struct {
      __u8 device;
      __u8 reserved1;
      __u16 reserved2;
      __u32 reserved3;
      __u64 reserved4;
    }  ata;
    struct {
      __u8 device;
      __u8 lun;
      __u8 reserved1;
      __u8 reserved2;
      __u32 reserved3;
      __u64 reserved4;
    }  atapi;
    struct {
      __u16 id;
      __u64 lun;
      __u16 reserved1;
      __u32 reserved2;
    }  scsi;
    struct {
      __u64 serial_number;
      __u64 reserved;
    }  usb;
    struct {
      __u64 eui;
      __u64 reserved;
    }  i1394;
    struct {
      __u64 wwid;
      __u64 lun;
    }  fibre;
    struct {
      __u64 identity_tag;
      __u64 reserved;
    }  i2o;
         struct {
      __u32 array_number;
      __u32 reserved1;
      __u64 reserved2;
    }  raid;
    struct {
      __u8 device;
      __u8 reserved1;
      __u16 reserved2;
      __u32 reserved3;
      __u64 reserved4;
    }  sata;
    struct {
      __u64 reserved1;
      __u64 reserved2;
    }  unknown;
  } device_path;
  __u8 reserved4;
  __u8 checksum;
};

struct edd_info {
  __u8 device;
  __u8 version;
  __u16 interface_support;
  __u16 legacy_max_cylinder;
  __u8 legacy_max_head;
  __u8 legacy_sectors_per_track;
  struct edd_device_params params;
};

//
// The so-called "zeropage"
//
struct boot_params {
  struct screen_info screen_info;
  struct apm_bios_info apm_bios_info;
  __u8  _pad2[12];
  struct ist_info ist_info;
  __u8  _pad3[16];
  __u8  hd0_info[16];
  __u8  hd1_info[16];
  struct sys_desc_table sys_desc_table;
  __u8  _pad4[144];
  struct edid_info edid_info;
  struct efi_info efi_info;
  __u32 alt_mem_k;
  __u32 scratch;
  __u8  e820_entries;
  __u8  eddbuf_entries;
  __u8  edd_mbr_sig_buf_entries;
  __u8  _pad6[6];
  struct setup_header hdr;
  __u8  _pad7[0x290 - 0x1f1 - sizeof (struct setup_header)];
  __u32 edd_mbr_sig_buffer[EDD_MBR_SIG_MAX];
  struct e820entry e820_map[E820MAX];
  __u8  _pad8[48];
  struct edd_info eddbuf[EDDMAXNR];
  __u8  _pad9[276];
} ;

#define X86_SUBARCH_PC         0
#define X86_SUBARCH_LGUEST     1
#define X86_SUBARCH_XEN        2
#define X86_SUBARCH_MRST       3

typedef struct _GDT_ENTRY {
  UINT16 Limit15_0;
  UINT16 Base15_0;
  UINT8  Base23_16;
  UINT8  Type;
  UINT8  Limit19_16_and_flags;
  UINT8  Base31_24;
} GDT_ENTRY;

typedef struct _GDT_ENTRIES {
  GDT_ENTRY Null;
  GDT_ENTRY Null2;
  GDT_ENTRY SysCode;
  GDT_ENTRY SysData;
} GDT_ENTRIES;

#pragma pack()

#define CMDLINE_OFFSET    0x1100000
#define BZIMAGE_SIZE_OFFSET  (CMDLINE_OFFSET + 0x400)
#define INITRD_SIZE_OFFSET  (BZIMAGE_SIZE_OFFSET + 4)
#define STACK_OFFSET    0x1101000
#define BZIMAGE_OFFSET    0x1102000

#define SETUP_HEADER_OFFSET (BZIMAGE_OFFSET + 0x1F1)
#define SETUP_HEADER_SIZE (0x0202 + *(unsigned char*)(0x0201+BZIMAGE_OFFSET))
#define BOOT_PARAMS_OFFSET 0x8000

#define SETUP_SIGNATURE 0x5a5aaa55

