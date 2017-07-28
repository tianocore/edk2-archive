;; @file
;  Search for the Boot Firmware Volume (BFV) base address.
;
;  Copyright (c) 2008 - 2017, Intel Corporation. All rights reserved.<BR>
;
;  This program and the accompanying materials
;  are licensed and made available under the terms and conditions of the BSD License
;  which accompanies this distribution.  The full text of the license may be found at
;  http://opensource.org/licenses/bsd-license.php.
;
;  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
;  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED
;
;;

%define IA32_MTRR_CAP                 0x0FE
%define MTRR_PHYS_BASE_0              0x0200
%define MTRR_PHYS_MASK_0              0x0201
%define MTRR_PHYS_BASE_1              0x0202
%define MTRR_PHYS_MASK_1              0x0203
%define MTRR_PHYS_BASE_2              0x0204
%define MTRR_PHYS_MASK_2              0x0205
%define MTRR_PHYS_BASE_3              0x0206
%define MTRR_PHYS_MASK_3              0x0207
%define MTRR_PHYS_BASE_4              0x0208
%define MTRR_PHYS_MASK_4              0x0209
%define MTRR_PHYS_BASE_5              0x020A
%define MTRR_PHYS_MASK_5              0x020B
%define MTRR_PHYS_BASE_6              0x020C
%define MTRR_PHYS_MASK_6              0x020D
%define MTRR_PHYS_BASE_7              0x020E
%define MTRR_PHYS_MASK_7              0x020F
%define MTRR_PHYS_BASE_8              0x0210
%define MTRR_PHYS_MASK_8              0x0211
%define MTRR_PHYS_BASE_9              0x0212
%define MTRR_PHYS_MASK_9              0x0213
%define MTRR_FIX_64K_00000            0x0250
%define MTRR_FIX_16K_80000            0x0258
%define MTRR_FIX_16K_A0000            0x0259
%define MTRR_FIX_4K_C0000             0x0268
%define MTRR_FIX_4K_C8000             0x0269
%define MTRR_FIX_4K_D0000             0x026A
%define MTRR_FIX_4K_D8000             0x026B
%define MTRR_FIX_4K_E0000             0x026C
%define MTRR_FIX_4K_E8000             0x026D
%define MTRR_FIX_4K_F0000             0x026E
%define MTRR_FIX_4K_F8000             0x026F
%define MTRR_DEF_TYPE                 0x02FF

%define MTRR_MEMORY_TYPE_UC           0x00
%define MTRR_MEMORY_TYPE_WC           0x01
%define MTRR_MEMORY_TYPE_WT           0x04
%define MTRR_MEMORY_TYPE_WP           0x05
%define MTRR_MEMORY_TYPE_WB           0x06

%define MTRR_DEF_TYPE_E               0x0800
%define MTRR_DEF_TYPE_FE              0x0400
%define MTRR_PHYSMASK_VALID           0x0800
%define SRAMBase                      0xFFFE0000
%define SRAMSize                      0x20000

%define HOST2CSE                      0x70
%define CSE2HOST                      0x60

;
; Define the high 32 bits of MTRR masking
; This should be read from CPUID EAX = 080000008h, EAX bits [7:0]
; But for most platforms this will be a fixed supported size so it is
; fixed to save space.
;
%define MTRR_PHYS_MASK_VALID          0x0800
%define MTRR_PHYS_MASK_HIGH           0x00000000F      ; For 36 bit addressing
%define IA32_MISC_ENABLE              0x1A0
%define FAST_STRING_ENABLE_BIT        0x01
%define CR0_CACHE_DISABLE             0x040000000
%define CR0_NO_WRITE                  0x020000000
%define IA32_PLATFORM_ID              0x017
%define IA32_BIOS_UPDT_TRIG           0x079
%define IA32_BIOS_SIGN_ID             0x08B
%define PLATFORM_INFO                 0x0CE
%define NO_EVICT_MODE                 0x2E0
%define NO_EVICTION_ENABLE_BIT        0x01

;
; Cache init and test values
; These are inverted to flip each bit at least once
;
%define CACHE_INIT_VALUE              0xA5A5A5A5

;ECP porting

%define CACHE_TEST_VALUE              0x5aa55aa5

;
; Processor MSR definitions
;
%define MSR_BBL_CR_CTL3                          0x11E ; L2 cache configuration MSR
%define B_MSR_BBL_CR_CTL3_L2_NOT_PRESENT         23    ; L2 not present
%define B_MSR_BBL_CR_CTL3_L2_ENABLED             8    ; L2 enabled
%define B_MSR_BBL_CR_CTL3_L2_HARDWARE_ENABLED    0    ; L2 hardware enabled

;
; Fv Header
;
%define FVH_SIGINATURE_OFFSET       0x28
%define FVH_SIGINATURE_VALID_VALUE  0x4856465F    ; valid signature:_FVH
%define FVH_HEADER_LENGTH_OFFSET    0x30
%define FVH_EXTHEADER_OFFSET_OFFSET 0x34
%define FVH_EXTHEADER_SIZE_OFFSET   0x10

;
; Ffs Header
;
%define FSP_HEADER_GUID_DWORD1      0x912740BE
%define FSP_HEADER_GUID_DWORD2      0x47342284
%define FSP_HEADER_GUID_DWORD3      0xB08471B9
%define FSP_HEADER_GUID_DWORD4      0x0C3F3527
%define FFS_HEADER_SIZE_VALUE       0x18

;
; Section Header
;
%define SECTION_HEADER_TYPE_OFFSET     0x03
%define RAW_SECTION_HEADER_SIZE_VALUE  0x04

;
; Fsp Header
;
%define FSP_HEADER_IMAGEBASE_OFFSET    0x1C
%define FSP_HEADER_TEMPRAMINIT_OFFSET  0x30
%define FspReservedSizeOnStackTop      0x40
struc UpdateHeaderStruc
  .dHeaderVersion        RESD  1            ; Header version#
  .dUpdateRevision       RESD  1            ; Update revision#
  .dDate                 RESD  1            ; Date in binary (08/13/07 as 0x08132007)
  .dProcessorSignature   RESD  1            ; CPU type, family, model, stepping
  .dChecksum             RESD  1            ; Checksum
  .dLoaderRevision       RESD  1            ; Update loader version#
  .dProcessorFlags       RESD  1            ; Processor Flags
  .dDataSize             RESD  1            ; Size of encrypted data
  .dTotalSize            RESD  1            ; Total size of update in bytes
  .bReserved             RESD  12           ; 12 bytes reserved
endstruc

struc HobStruc
  .Sign                 RESD  1            ; Signiture#
  .CarBase              RESD  1            ; Cache As Ram Base Address
  .CarSize              RESD  1            ; Cache As Ram Size
  .IBBSource            RESD  1            ; IBBM Address in SRAM
  .IBBBase              RESD  1            ; IBBM Base in CAR.
  .IBBSize              RESD  1            ; IBBM Size
  .IBBLSource           RESD  1            ; IBBL Address in SRAM
  .IBBLBase             RESD  1            ; IBBL Base in CAR.
  .IBBLSize             RESD  1            ; IBBL Size
  .FITBase              RESD  1            ; FIT Base Address
  .StackHeapBase        RESD  1            ; STACK&HEAP Base .
  .StackHeapSize        RESD  1            ; STACK&HEAP Size
  .HostToCse            RESD  1
  .CseToHost            RESD  1
  .ChunkIndex           RESD  1
  .NumberOfChunks       RESD  1
  .IbbSizeLeft          RESD  1
  .Chunksize            RESD  1
  .IbblPerfRecord0      RESQ  1            ; The QWROD Performance record0 of IBBL
  .IbblPerfRecord1      RESQ  1            ; The QWROD Performance record1 of IBBL
  .IbblPerfRecord2      RESQ  1            ; The QWROD Performance record2 of IBBL
  .IbblPerfRecord3      RESQ  1            ; The QWROD Performance record3 of IBBL
  .IbblPerfRecord4      RESQ  1            ; The QWROD Performance record4 of IBBL
  .IbblPerfRecord5      RESQ  1            ; The QWROD Performance record5 of IBBL
endstruc

%macro ADDR_OF_CAR 2
mov %1, [ADDR_OF(HOBStructure) + HobStruc.IBBLBase]
add %1, [ADDR_OF(HOBStructure) + HobStruc.IBBLSize]
sub %1, fourGigabytes - %2
%endmacro

ALIGN   16
HOBStructure:
istruc HobStruc
    dd "$SIG"       ; .Sign
    dd 0xFEF00000   ; .CarBase
    dd 0x00100000   ; .CarSize
    dd 0xFFF00000   ; .IBBSource  = Not used
    dd 0xFEF45000   ; .IBBBase    = .CarBase
    dd 0x0008E000   ; .IBBSize    = size of (FVIBBM.fv+FSP_M.fv) = BLD_IBBM_SIZE + FSP_IBBM_SIZE = 0x8D000
    dd 0xFFFFF000   ; .IBBLSource = 0x100000000 - .IBBLSize = PcdFlashFvIBBLBase
    dd 0xFEF40000   ; .IBBLBase   = .IBBBase + .IBBSize
    dd 0x00001000   ; .IBBLSize   = PcdFlashFvIBBLSize = FLASH_REGION_FV_IBBL_SIZE in .fdf
    dd 0xFEF41000   ; .FITBase
    dd 0xFEF00000   ; .Stack&Heap Base
    dd 0x16000      ; .Stack&Heap Size
    dd 0            ; .HostToCse
    dd 0            ; .CseToHost
    dd 0            ; .ChunkIndex
    dd 0            ; .NumberOfChunks
    dd 0            ; .IbbSizeLeft
    dd 0            ; .Chunksize
    dq 0            ; .IbblPerfRecord0
    dq 0            ; .IbblPerfRecord1
    dq 0            ; .IbblPerfRecord2
    dq 0            ; .IbblPerfRecord3
    dq 0            ; .IbblPerfRecord4
    dq 0            ; .IbblPerfRecord5
iend
;%macro  ShowPostCode 1
;    out 0x80, ax
;    mov ecx, %1 / 400
;    loop $
;%endmacro

BITS    32


RuninCAR:
    rdtsc
    movd mm6, eax
    movd mm7, edx

  ;
  ; Optionally Test the Region...
  ;

  ;
  ; Test area by writing and reading
  ;
    cld
    mov     edi, [ADDR_OF(HOBStructure) + HobStruc.StackHeapBase]
    mov     ecx, [ADDR_OF(HOBStructure) + HobStruc.StackHeapSize]
    shr     ecx, 2
    mov     eax, CACHE_TEST_VALUE
  TestDataStackArea:
    stosd
    cmp     eax, DWORD [edi-4]
    jnz     DataStackTestFail
    loop    TestDataStackArea
    jmp     DataStackTestPass

  ;
  ; Cache test failed
  ;
  DataStackTestFail:
    debugShowPostCode 0xD0
    jmp     $

  ;
  ; Configuration test failed
  ;
  ConfigurationTestFailed:
    debugShowPostCode 0xD1
    jmp     $

  DataStackTestPass:

    ;
    ; relocate GDT to CAR to let CSE use SRAM for IBBM
    ;
    ADDR_OF_CAR ebx, gdtr
    ADDR_OF_CAR esi, GDT_BASE
    mov dword [ebx+2],esi
    lgdt [ebx]
    ADDR_OF_CAR ebp,  HOBStructure
    mov dword[esi-4], ebp ; save HobStructure pointer near Gdt base for easy locating later.

    mov eax, 0x80007800;
    mov dx,  0CF8h
    out dx,  eax
    add dx,  4
    in  eax, dx
    cmp eax, 0xFFFFFFFF
    jz SKIPCSE ; if HECI PCI device does not exist, which mean we are not run on PSS2.0
    ;    jmp SKIPCSE
;    mov ax, 1234h
;    ShowPostCode 0x1000000
    xor ebx, ebx
    mov ecx, 0x8b ; Microcode Info MSR
    rdmsr
    cmp edx, 0
    je uCodeNotLoad
    mov ebx, 0x40000000
uCodeNotLoad:
    or  ebx, 0x80000000

    mov eax, dword [ebp + HobStruc.IBBSize]
    shl eax, 4
    and eax, 0x3FFFFFFF
    add eax, ebx
    mov dword [ebp + HobStruc.HostToCse], eax ; value of HOST_TO_SEC
    mov dword [ebp + HobStruc.ChunkIndex], 0         ; current chunk index for copying

    mov eax, 0x80007800 + HOST2CSE              ; PCI HECI HOST_TO_CSE register
    mov dx,  0CF8h
    out dx,  eax
    mov eax, [ebp + HobStruc.HostToCse]
    add dx,  4
    out dx,  eax
;    out 80h, eax

polling:
    mov eax, 0x80007800 + CSE2HOST ;
    mov dx,  0CF8h
    out dx,  eax
    add dx,  4
    in  eax, dx
    mov [ebp + HobStruc.CseToHost], eax ; value of CSE_TO_HOST
    ; eax: 29:28, number of chunks; 27:14, sizes in 1Ks.
;    out 80h, eax

    mov ebx, eax
    bswap ebx
    shr ebx, 4
    and ebx, 3

    cmp ebx, 0
    jnz success
    mov ax, 2300h
    mov al, bl
;    ShowPostCode 0x4000000
    jmp polling
success:
    cmp ebx, 3
    jnz next
    inc ebx
next:
    mov [ebp + HobStruc.NumberOfChunks],ebx ; [ebp+12] number of chunks
    mov ebx, eax;
    and ebx, 0fffc000h
    shr ebx, 4 ;  bit 27:14, sizes in 1Ks.
    mov [ebp + HobStruc.IbbSizeLeft], ebx ; size of IBB

    ; calculate Chunk Size
    mov eax, SRAMSize
    xor edx, edx
    div dword [ebp + HobStruc.NumberOfChunks]
    mov [ebp + HobStruc.Chunksize], eax

;    mov ax, 123Ah
;    ShowPostCode 0x4000000

RecheckCSEReady:
    mov  al, byte [ebp + HobStruc.CseToHost]
    mov  ah, byte [ebp + HobStruc.HostToCse] ; check whether there is ring buffer ready to copy
    and  ax, 0x0F0F
    xor al, ah
;   out 80h, al
;   mov ecx, 0x4000000
;   loop $
;    ShowPostCode 0x4000000
    cmp  al, 0
    jnz startcopy

;    mov ax, 2345h
;    ShowPostCode 0x4000000

    mov eax, 0x80007800 + CSE2HOST
    mov dx,  0CF8h
    out dx,  eax
    add dx,  4
    in  eax, dx
    mov [ebp + HobStruc.CseToHost], eax ; value of SEC_TO_HOST

    out 80h, eax
    jmp  RecheckCSEReady

startcopy:
    mov bl, al;
    mov eax, [ebp + HobStruc.ChunkIndex]
;    ShowPostCode 0x4000000
    xor edx, edx
    div dword [ebp + HobStruc.NumberOfChunks]; edx: ring buffer index

    ; ecx is the index of ring buffer executing
    mov ecx,edx;

    ; calculated the source address in ring buffer
    mov esi, SRAMBase
    mov eax, [ebp + HobStruc.Chunksize]
    mul edx
    add esi, eax

    ; calculate the destination address in Cache
    mov edi, [ebp + HobStruc.IBBBase]
    mov eax, [ebp + HobStruc.Chunksize]
    mul dword [ebp + HobStruc.ChunkIndex]
    add edi, eax

    mov dl, 01h;
    shl dl, cl ; edx, the value of current ring buffer

    and bl, dl ; ebx will be used to flip HOST_TO_CSE chunk values.
    xchg bl, dl
    cmp dl, 0
    jz RecheckCSEReady

    ; calculate the size
    mov ecx, [ebp + HobStruc.Chunksize] ; [ebp + 20], chunk size, [ebp + 16], size not copied.
    cmp [ebp + HobStruc.IbbSizeLeft], ecx
    ja bigger
    mov ecx, [ebp + HobStruc.IbbSizeLeft]
bigger:
    sub [ebp + HobStruc.IbbSizeLeft], ecx
    mov eax, esi
    out 80h, eax
    mov eax, edi
    out 80h, eax
    mov eax, ecx
    out 80h, eax

    shr ecx, 6
copy2:
    MOVNTDQA xmm0, [esi]
    MOVNTDQA xmm1, [esi+16]
    MOVNTDQA xmm2, [esi+32]
    MOVNTDQA xmm3, [esi+48]
    MOVDQA [edi], xmm0
    MOVDQA [edi+16], xmm1
    MOVDQA [edi+32], xmm2
    MOVDQA [edi+48], xmm3
    add esi,64
    add edi, 64
    loop copy2

    mov al, byte [ebp + HobStruc.CseToHost]
    and al, bl

    mov cl, byte [ebp + HobStruc.HostToCse]
    not bl
    and cl,bl
    or al, cl
    mov byte [ebp + HobStruc.HostToCse],al

    mov eax, 0x80007800 + HOST2CSE
    mov dx,  0CF8h
    out dx,  eax

    mov eax, [ebp + HobStruc.HostToCse]
    add dx,  4
    out dx,  eax
    out 80h, eax

;    ShowPostCode 0x4000000

    inc dword [ebp + HobStruc.ChunkIndex]
    cmp dword [ebp + HobStruc.IbbSizeLeft], 0

    jnz RecheckCSEReady

;    mov ax, 3456h
;    ShowPostCode 0x4000000
    jmp noskipcse
;
SKIPCSE:
    mov esi, [ebp + HobStruc.IBBSource]
    mov edi, [ebp + HobStruc.IBBBase]
    mov ecx, [ebp + HobStruc.IBBSize]
    shr ecx, 2
    rep movsd

noskipcse:
    mov ecx, 4800h
    xchg esi, edi
    rep lodsd

    ;
    ; For every copy of code from SRAM to NEM this has to be set to avoid dirty iL1
    ; if (for some reason)Code-B is cached in iL1, and then Code-A modifes data next to Code-B,
    ; the Core will eject both the data and code-B from iL1. So wrmsr 120 before jmping to Code-A.
    ; GLM HSD ES 4942265
    ;
    mov     ecx, 0120h  ;Power_misc
    rdmsr
    or      eax, 0100h  ;BIT 8
    wrmsr

    mov esi, [ebp + HobStruc.IBBBase]
    mov eax, [esi+020h] ; get FvLength
;    mov eax, [ebp + HobStruc.IBBSize]
    add esi, eax
    sub esi, 0x10
    mov eax, esi
    out 80h, eax
    mov eax, [esi]
    out 80h, eax

    ;
    ; Save the Start Time of Reset Vector to IbblPerfRecord0
    ;
    movd eax, mm2
    movd edx, mm3
    mov [ebp + HobStruc.IbblPerfRecord0], eax
    mov [ebp + HobStruc.IbblPerfRecord0 + 4], edx
    ;
    ; Save the End Time of InitNEM to IbblPerfRecord1
    ;
    movd eax, mm4
    movd edx, mm5
    mov [ebp + HobStruc.IbblPerfRecord1], eax
    mov [ebp + HobStruc.IbblPerfRecord1 + 4], edx
    ;
    ; Save the End Time of IBBLSdw to IbblPerfRecord2
    ;
    movd eax, mm6
    movd edx, mm7
    mov [ebp + HobStruc.IbblPerfRecord2], eax
    mov [ebp + HobStruc.IbblPerfRecord2 + 4], edx
    ;
    ; Save the End Time of IBBM Load to IbblPerfRecord3
    ;
    rdtsc
    mov [ebp + HobStruc.IbblPerfRecord3], eax
    mov [ebp + HobStruc.IbblPerfRecord3 + 4], edx

    ;
    ; Jump to Virtual Reset Vector in IBBM
    ;

 WaitVerify:
    mov eax, 0x80007800 + CSE2HOST ;
    mov dx,  0CF8h
    out dx,  eax
    add dx,  4
    in  eax, dx
    and eax, 0600h
    cmp eax, 0600h
    jne WaitVerify

    ;
    ; Save the End Time of IBBM Verify to IbblPerfRecord4
    ;
    rdtsc
    mov [ebp + HobStruc.IbblPerfRecord4], eax
    mov [ebp + HobStruc.IbblPerfRecord4 + 4], edx

    ;
    ; Restore initial EAX value into the EAX register
    ;
    mov eax, esp

    jmp esi    ;jump to \Vtf1\Main.asm #40

InitNEM:
  ;  Enable cache for use as stack and for caching code
  ;  The algorithm is specified in the processor BIOS writer's guide

  ;  Ensure that the system is in flat 32 bit protected mode.
  ;
  ;  Platform Specific - configured earlier
  ;
  ;  Ensure that only one logical processor in the system is the BSP.
  ;  (Required step for clustered systems).
  ;
  ;  Platform Specific - configured earlier

  ;  Ensure all APs are in the Wait for SIPI state.
  ;  This includes all other logical processors in the same physical processor
  ;  as the BSP and all logical processors in other physical processors.
  ;  If any APs are awake, the BIOS must put them back into the Wait for
  ;  SIPI state by issuing a broadcast INIT IPI to all excluding self.
  ;
  mov     edi, 0xFEE00300               ; 0FEE00300h - Send INIT IPI to all excluding self
  mov     eax, 0x000C4500; ORAllButSelf + ORSelfINIT ; 0000C4500h
  mov     [edi], eax

  loop1:
    mov     eax, [edi]
    bt      eax, 12                       ; Check if send is in progress
    jc      loop1                            ; Loop until idle

  ;
  ;   Load microcode update into BSP.
  ;
  ;   Ensure that all variable-range MTRR valid flags are clear and
  ;   IA32_MTRR_DEF_TYPE MSR E flag is clear.  Note: This is the default state
  ;   after hardware reset.
  ;
  ;   Platform Specific - MTRR are usually in default state.
  ;

  ;
  ; Initialize all fixed-range and variable-range MTRR register fields to 0.
  ;
   mov    ecx, IA32_MTRR_CAP            ; get variable MTRR support
   rdmsr
   movzx  ebx, al                       ; EBX = number of variable MTRR pairs
   shl    ebx, 2                        ; *4 for Base/Mask pair and WORD size
   add    ebx, 24                       ; EBX = size of  Fixed and Variable MTRRs

   xor    eax, eax                      ; Clear the low dword to write
   xor    edx, edx                      ; Clear the high dword to write

  InitMtrrLoop:
     add    ebx, -2
     mov    esi, ADDR_OF(MtrrInitTable)
     movzx  ecx, word [cs:esi+ebx]
     ; ecx <- address of mtrr to zero
     wrmsr
     jnz    InitMtrrLoop                  ; loop through the whole table

   ;
   ; Configure the default memory type to un-cacheable (UC) in the
   ; IA32_MTRR_DEF_TYPE MSR.
   ;
   mov     ecx, MTRR_DEF_TYPE            ; Load the MTRR default type index
   rdmsr
   and     eax, 0xFFFFF300; NOT (0x0000CFF)          ; Clear the enable bits and def type UC.
   wrmsr

   ;
   ; Configure MTRR_PHYS_MASK_HIGH for proper addressing above 4GB
   ; based on the physical address size supported for this processor
   ; This is based on read from CPUID EAX = 080000008h, EAX bits [7:0]
   ;
   ; Examples:
   ;  MTRR_PHYS_MASK_HIGH = 00000000Fh  For 36 bit addressing
   ;  MTRR_PHYS_MASK_HIGH = 0000000FFh  For 40 bit addressing
   ;
   mov     eax, 0x80000008                ; Address sizes leaf
   cpuid
   sub     al,  32
   movzx   eax, al
   xor     esi, esi
   bts     esi, eax
   dec     esi                           ; esi <- MTRR_PHYS_MASK_HIGH

   ;
   ; Configure the DataStack region as write-back (WB) cacheable memory type
   ; using the variable range MTRRs.
   ;

   ;
   ; Set the base address of the DataStack cache range
   ;
   mov     eax, [ADDR_OF(HOBStructure) + HobStruc.CarBase]
   or      eax, MTRR_MEMORY_TYPE_WB
                                         ; Load the write-back cache value
   xor     edx, edx                      ; clear upper dword
   mov     ecx, MTRR_PHYS_BASE_0         ; Load the MTRR index
   wrmsr                                 ; the value in MTRR_PHYS_BASE_0

   ;
   ; Set the mask for the DataStack cache range
   ; Compute MTRR mask value:  Mask = NOT (Size - 1)
   ;

   mov     eax, [ADDR_OF(HOBStructure) + HobStruc.CarSize]
   bsr     ecx, eax                     ; Get the least significant set bit of 1 for length
   bsf     edx, eax                     ; Get the reversed most significant set bit of 1 for length
   cmp     ecx, edx
   jz      noadjust
   mov     eax, 2
   shl     eax, cl
noadjust:
   dec     eax
   not     eax
   or      eax, MTRR_PHYS_MASK_VALID
                                         ; turn on the Valid flag
   mov     edx, esi                      ; edx <- MTRR_PHYS_MASK_HIGH
   mov     ecx, MTRR_PHYS_MASK_0         ; For proper addressing above 4GB
   wrmsr                                 ; the value in MTRR_PHYS_BASE_0

  mov     ecx, MTRR_DEF_TYPE            ; Load the MTRR default type index
  rdmsr
  or      eax, MTRR_DEF_TYPE_E          ; Enable variable range MTRRs
  wrmsr

  ;
  ;   Enable the logical processor's (BSP) cache: execute INVD and set
  ;   CR0.CD = 0, CR0.NW = 0.
  ;
  mov     eax, cr0
  and     eax,  0x9FFFFFFF; NOT (CR0_CACHE_DISABLE + CR0_NO_WRITE)
  invd
  mov     cr0, eax
  ;
  ;   Enable No-Eviction Mode Setup State by setting
  ;   NO_EVICT_MODE  MSR 2E0h bit [0] = '1'.
  ;
  mov     ecx, NO_EVICT_MODE
  rdmsr
  or      eax, 1
  wrmsr

  ;
  ;   One location in each 64-byte cache line of the DataStack region
  ;   must be written to set all cache values to the modified state.
  ;
  mov     edi, [ADDR_OF(HOBStructure) + HobStruc.CarBase]
  mov     ecx, [ADDR_OF(HOBStructure) + HobStruc.CarSize]
  shr     ecx, 6
  mov     eax, CACHE_INIT_VALUE
write:
  mov  [edi], eax
  sfence
  add  edi, 64
  loop  write

  ;
  ;   Enable No-Eviction Mode Run State by setting
  ;   NO_EVICT_MODE MSR 2E0h bit [1] = '1'.
  ;
  mov     ecx, NO_EVICT_MODE
  rdmsr
  or      eax, 2
  wrmsr

  ;
  ; Finished with cache configuration
  ;
OneTimeCallRet InitNEM


SwtichToCAR:
    rdtsc
    movd mm4, eax
    movd mm5, edx
    mov esi, [ADDR_OF(HOBStructure) + HobStruc.IBBLSource]
    mov ecx, [ADDR_OF(HOBStructure) + HobStruc.IBBLSize]
    mov edi, [ADDR_OF(HOBStructure) + HobStruc.IBBLBase]
    shr ecx, 2
    rep movsd

    mov ecx, 4800h
    mov esi, [ADDR_OF(HOBStructure) + HobStruc.CarBase]
    rep lodsd

    ; Copy FIT structure to CAR for use in PEI
    mov edi, 0xffffffc0  ; load the FIT pointer
    mov esi, [edi]       ; load location of FIT

    add esi, 0x8         ; get the address of the FIT table size
    mov eax, [esi]       ; get the value that contians the FIT table size
    and eax, 0x00ffffff  ; mask off the FIT table Size which is only 24bit
    shl eax, 2           ; since size is in 16 bytes chunks, multiply by 4 to get #of Dword to copy
    mov ecx, eax         ; move the size value to ecx to prepare for the move
    sub esi, 0x8         ; restore the esi by the FIT table entry address

    ; calculate the destination address in CAR for FIT copy - just after IBBL
    mov edi, [ADDR_OF(HOBStructure) + HobStruc.FITBase]
    rep movsd           ; copy the FIT structure into CAR.

    ;
    ; For every copy of code from SRAM to NEM this has to be set to avoid dirty iL1
    ; GLM HSD ES 4942265
    ;
    mov     ecx, 0120h  ;Power_misc
    rdmsr
    or      eax, 0100h  ;BIT 8
    wrmsr

    ;
    ; calculate the address in CAR for IBBL and jmp to, and so CSE can put IBBM into SRAM.
    ;
    ADDR_OF_CAR esi, RuninCAR
    jmp esi
    nop
    HLT

    ; Add enough nop's to ensure that Size(InitNem) + Size(SwitchToCar) > 12*64 (768 bytes)
    ; to ensure that end code in "RuninCAR" is far enough from .data of IBBL.fv
    ALIGN  64
    HLT
    ALIGN  64   ;1 cache line (64bytes)
    HLT
    ALIGN  64
    HLT


MtrrInitTable:
    DW  MTRR_DEF_TYPE
    DW  MTRR_FIX_64K_00000
    DW  MTRR_FIX_16K_80000
    DW  MTRR_FIX_16K_A0000
    DW  MTRR_FIX_4K_C0000
    DW  MTRR_FIX_4K_C8000
    DW  MTRR_FIX_4K_D0000
    DW  MTRR_FIX_4K_D8000
    DW  MTRR_FIX_4K_E0000
    DW  MTRR_FIX_4K_E8000
    DW  MTRR_FIX_4K_F0000
    DW  MTRR_FIX_4K_F8000
    DW  MTRR_PHYS_BASE_0
    DW  MTRR_PHYS_MASK_0
    DW  MTRR_PHYS_BASE_1
    DW  MTRR_PHYS_MASK_1
    DW  MTRR_PHYS_BASE_2
    DW  MTRR_PHYS_MASK_2
    DW  MTRR_PHYS_BASE_3
    DW  MTRR_PHYS_MASK_3
    DW  MTRR_PHYS_BASE_4
    DW  MTRR_PHYS_MASK_4
    DW  MTRR_PHYS_BASE_5
    DW  MTRR_PHYS_MASK_5
    DW  MTRR_PHYS_BASE_6
    DW  MTRR_PHYS_MASK_6
    DW  MTRR_PHYS_BASE_7
    DW  MTRR_PHYS_MASK_7
    DW  MTRR_PHYS_BASE_8
    DW  MTRR_PHYS_MASK_8
    DW  MTRR_PHYS_BASE_9
    DW  MTRR_PHYS_MASK_9

