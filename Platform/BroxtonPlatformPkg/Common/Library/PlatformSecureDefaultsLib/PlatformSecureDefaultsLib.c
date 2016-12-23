/** @file
  IPC based PlatformFvbLib library instance.

  Copyright (c) 2009 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Library/PlatformSecureDefaultsLib.h"
#include <Guid/AuthenticatedVariableFormat.h>
#include <Guid/SetupVariable.h>

EFI_GUID mUefiImageSecurityDBGuid        = EFI_IMAGE_SECURITY_DATABASE_GUID;
EFI_GUID mUefiCertTypeRsa2048Guid        = EFI_CERT_RSA2048_GUID;

#define WIN_CERT_UEFI_RSA2048_SIZE        256
#define EFI_SECURE_BOOT_ENABLE_NAME       L"SecureBootEnable"

extern EFI_GUID mUefiCertTypeRsa2048Guid;
extern EFI_GUID gEfiSecureBootEnableDisableGuid;

EFI_GUID gOwnerSignatureGUID         = {0x77fa9abd, 0x0359, 0x4d32, {0xbd, 0x60, 0x28, 0xf4, 0xe7, 0x8f, 0x78, 0x4b}};
static EFI_GUID  gDbxUpdateImageGuid = {0xa3d48bb3, 0x350f, 0x4bcd, 0xa4, 0xad, 0x44, 0x5b, 0x93, 0x9f, 0x6d, 0x9c };

/**
  Create a time based data payload by concatenating the EFI_VARIABLE_AUTHENTICATION_2
  descriptor with the input data. NO authentication is required in this function.

  @param[in, out]   DataSize                 On input, the size of Data buffer in bytes.
                                             On output, the size of data returned in Data
                                             buffer in bytes.
  @param[in, out]   Data                     On input, Pointer to data buffer to be wrapped or
                                             pointer to NULL to wrap an empty payload.
                                             On output, Pointer to the new payload date buffer allocated from pool,
                                             it's caller's responsibility to free the memory when finish using it.

  @retval           EFI_SUCCESS              Create time based payload successfully.
  @retval           EFI_OUT_OF_RESOURCES     There are not enough memory resourses to create time based payload.
  @retval           EFI_INVALID_PARAMETER    The parameter is invalid.
  @retval           Others                   Unexpected error happens.

**/
EFI_STATUS
CreateTimeBasedPayload (
  IN OUT UINTN            *DataSize,
  IN OUT UINT8            **Data
  )
{
  EFI_STATUS                       Status;
  UINT8                            *NewData;
  UINT8                            *Payload;
  UINTN                            PayloadSize;
  EFI_VARIABLE_AUTHENTICATION_2    *DescriptorData;
  UINTN                            DescriptorSize;
  EFI_TIME                         Time;

  if (Data == NULL || DataSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // In Setup mode or Custom mode, the variable does not need to be signed but the
  // parameters to the SetVariable() call still need to be prepared as authenticated
  // variable. So we create EFI_VARIABLE_AUTHENTICATED_2 descriptor without certificate
  // data in it.
  //
  Payload     = *Data;
  PayloadSize = *DataSize;

  DescriptorSize    = OFFSET_OF (EFI_VARIABLE_AUTHENTICATION_2, AuthInfo) + OFFSET_OF (WIN_CERTIFICATE_UEFI_GUID, CertData);
  NewData = (UINT8 *) AllocateZeroPool (DescriptorSize + PayloadSize);
  if (NewData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if ((Payload != NULL) && (PayloadSize != 0)) {
    CopyMem (NewData + DescriptorSize, Payload, PayloadSize);
  }

  DescriptorData = (EFI_VARIABLE_AUTHENTICATION_2 *) (NewData);

  ZeroMem (&Time, sizeof (EFI_TIME));
  Status = gRT->GetTime (&Time, NULL);
  if (EFI_ERROR (Status)) {
    FreePool(NewData);
    return Status;
  }
  Time.Pad1       = 0;
  Time.Nanosecond = 0;
  Time.TimeZone   = 0;
  Time.Daylight   = 0;
  Time.Pad2       = 0;
  CopyMem (&DescriptorData->TimeStamp, &Time, sizeof (EFI_TIME));

  DescriptorData->AuthInfo.Hdr.dwLength         = OFFSET_OF (WIN_CERTIFICATE_UEFI_GUID, CertData);
  DescriptorData->AuthInfo.Hdr.wRevision        = 0x0200;
  DescriptorData->AuthInfo.Hdr.wCertificateType = WIN_CERT_TYPE_EFI_GUID;
  CopyGuid (&DescriptorData->AuthInfo.CertType, &gEfiCertPkcs7Guid);

  if (Payload != NULL) {
    FreePool(Payload);
  }

  *DataSize = DescriptorSize + PayloadSize;
  *Data     = NewData;
  return EFI_SUCCESS;
}


/**
  Generate the PK signature list from the X509 Certificate storing file (.cer)

  @param[in]   X509Data                   FileHandle of X509 Certificate storing file.
  @param[in]   X509DataSize               The size of fileHandle of X509 Certificate storing file.
  @param[out]  PkCert                     Point to the data buffer to store the signature list.

  @retval      EFI_UNSUPPORTED            Unsupported Key Length.
  @retval      EFI_OUT_OF_RESOURCES       There are not enough memory resourses to form the signature list.

**/
EFI_STATUS
CreatePkX509SignatureList (
  IN    UINT8                       *X509Data,
  IN    UINTN                       X509DataSize,
  OUT   EFI_SIGNATURE_LIST          **PkCert
  )
{
  EFI_STATUS              Status;
  EFI_SIGNATURE_DATA      *PkCertData;

  PkCertData = NULL;
  Status = EFI_SUCCESS;
  ASSERT (X509Data != NULL);

  //
  // Allocate space for PK certificate list and initialize it.
  // Create PK database entry with SignatureHeaderSize equals 0.
  //
  *PkCert = (EFI_SIGNATURE_LIST *) AllocateZeroPool (
              sizeof (EFI_SIGNATURE_LIST) + sizeof (EFI_SIGNATURE_DATA) - 1
              + X509DataSize
              );
  if (*PkCert == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  (*PkCert)->SignatureListSize   = (UINT32) (sizeof (EFI_SIGNATURE_LIST)
                                    + sizeof (EFI_SIGNATURE_DATA) - 1
                                    + X509DataSize);
  (*PkCert)->SignatureSize       = (UINT32) (sizeof (EFI_SIGNATURE_DATA) - 1 + X509DataSize);
  (*PkCert)->SignatureHeaderSize = 0;
  CopyGuid (&(*PkCert)->SignatureType, &gEfiCertX509Guid);
  PkCertData                     = (EFI_SIGNATURE_DATA *) ((UINTN) (*PkCert)
                                                          + sizeof (EFI_SIGNATURE_LIST)
                                                          + (*PkCert)->SignatureHeaderSize);
  CopyGuid (&PkCertData->SignatureOwner, &gEfiGlobalVariableGuid);
  //
  // Fill the PK database with PKpub data from X509 certificate file.
  //
  CopyMem (&(PkCertData->SignatureData[0]), X509Data, X509DataSize);

ON_EXIT:

  if (EFI_ERROR(Status) && *PkCert != NULL) {
    FreePool (*PkCert);
    *PkCert = NULL;
  }

  return Status;
}


EFI_STATUS
EnrollPlatformKey (
  IN  VOID             *Buf,
  IN  UINTN             BufSize
  )
{
  EFI_STATUS                      Status;
  UINT32                          Attr;
  UINTN                           DataSize;
  EFI_SIGNATURE_LIST              *PkCert;

  PkCert = NULL;

  //
  // Prase the selected PK file and generature PK certificate list.
  //
  Status = CreatePkX509SignatureList (
             Buf,
             BufSize,
             &PkCert
             );

  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }
  ASSERT (PkCert != NULL);

  //
  // Set Platform Key variable.
  //
  Attr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS
          | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;
  DataSize = PkCert->SignatureListSize;
  Status = CreateTimeBasedPayload (&DataSize, (UINT8 **) &PkCert);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Fail to create time-based data payload: %r", Status));
    goto ON_EXIT;
  }

  Status = gRT->SetVariable (
                  EFI_PLATFORM_KEY_NAME,
                  &gEfiGlobalVariableGuid,
                  Attr,
                  DataSize,
                  PkCert
                  );
  if (EFI_ERROR (Status)) {
    if (Status == EFI_OUT_OF_RESOURCES) {
      DEBUG ((EFI_D_ERROR, "Enroll PK failed with out of resource.\n"));
    }
    goto ON_EXIT;
  }

ON_EXIT:

  if (PkCert != NULL) {
    FreePool (PkCert);
  }

  return Status;
}


/**
  Enroll a new KEK item from X509 certificate file.

  @param[in] PrivateData            The module's private data.

  @retval    EFI_SUCCESS            New X509 is enrolled successfully.
  @retval    EFI_INVALID_PARAMETER  The parameter is invalid.
  @retval    EFI_UNSUPPORTED        Unsupported command.
  @retval    EFI_OUT_OF_RESOURCES   Could not allocate needed resources.

**/
EFI_STATUS
EnrollX509ToKek (
  VOID                *X509Data,
  UINTN                X509DataSize
  )
{
  EFI_STATUS                        Status;
  EFI_SIGNATURE_DATA                *KEKSigData;
  EFI_SIGNATURE_LIST                *KekSigList;
  UINTN                             DataSize;
  UINTN                             KekSigListSize;
  UINT32                            Attr;

  KekSigList     = NULL;
  KekSigListSize = 0;
  DataSize       = 0;
  KEKSigData     = NULL;

  ASSERT (X509Data != NULL);

  KekSigListSize = sizeof (EFI_SIGNATURE_LIST) + sizeof (EFI_SIGNATURE_DATA) - 1 + X509DataSize;
  KekSigList = (EFI_SIGNATURE_LIST *) AllocateZeroPool (KekSigListSize);
  if (KekSigList == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  //
  // Fill Certificate Database parameters.
  //
  KekSigList->SignatureListSize   = (UINT32) KekSigListSize;
  KekSigList->SignatureHeaderSize = 0;
  KekSigList->SignatureSize = (UINT32) (sizeof (EFI_SIGNATURE_DATA) - 1 + X509DataSize);
  CopyGuid (&KekSigList->SignatureType, &gEfiCertX509Guid);

  KEKSigData = (EFI_SIGNATURE_DATA *) ((UINT8 *) KekSigList + sizeof (EFI_SIGNATURE_LIST));
  CopyGuid (&KEKSigData->SignatureOwner, &gOwnerSignatureGUID);
  CopyMem (KEKSigData->SignatureData, X509Data, X509DataSize);

  //
  // Check if KEK been already existed.
  // If true, use EFI_VARIABLE_APPEND_WRITE attribute to append the
  // new kek to original variable
  //
  Attr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS
          | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;
  Status = CreateTimeBasedPayload (&KekSigListSize, (UINT8 **) &KekSigList);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Fail to create time-based data payload: %r", Status));
    goto ON_EXIT;
  }

  Status = gRT->GetVariable(
                  EFI_KEY_EXCHANGE_KEY_NAME,
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &DataSize,
                  NULL
                  );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Attr |= EFI_VARIABLE_APPEND_WRITE;
  } else if (Status != EFI_NOT_FOUND) {
    goto ON_EXIT;
  }

  Status = gRT->SetVariable(
                  EFI_KEY_EXCHANGE_KEY_NAME,
                  &gEfiGlobalVariableGuid,
                  Attr,
                  KekSigListSize,
                  KekSigList
                  );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

ON_EXIT:

  if (KekSigList != NULL) {
    FreePool (KekSigList);
  }

  return Status;
}


/**
  Enroll new KEK into the System without PK's authentication.
  The SignatureOwner GUID will be Private->SignatureGUID.

  @param[in] PrivateData            The module's private data.

  @retval    EFI_SUCCESS            New KEK enrolled successful.
  @retval    EFI_INVALID_PARAMETER  The parameter is invalid.
  @retval    others                 Fail to enroll KEK data.

**/
EFI_STATUS
EnrollKeyExchangeKey (
  IN VOID                  *DataBuf,
  IN UINTN                 BufSize
  )
{
  return EnrollX509ToKek (DataBuf, BufSize);
}


EFI_STATUS
EnrollX509toForbSigDB (
  IN CHAR16                         *VariableName,
  IN VOID                           *X509Data,
  IN UINTN                          X509DataSize
  )
{
  EFI_STATUS                        Status;
  VOID                              *Data;
  UINTN                             SigDBSize;
  UINT32                            Attr;
  UINTN                             DataSize;

  SigDBSize     = 0;
  DataSize      = 0;
  Data          = NULL;

  ASSERT (X509Data != NULL);

  SigDBSize = X509DataSize;

  Data = AllocateZeroPool (SigDBSize);
  if (Data == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  CopyMem ((UINT8 *) Data, X509Data, X509DataSize);

  Attr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS
          | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;

  //
  // Check if signature database entry has been already existed.
  // If true, use EFI_VARIABLE_APPEND_WRITE attribute to append the
  // new signature data to original variable
  //
  Status = gRT->GetVariable(
                  VariableName,
                  &gEfiImageSecurityDatabaseGuid,
                  NULL,
                  &DataSize,
                  NULL
                  );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Attr |= EFI_VARIABLE_APPEND_WRITE;
  } else if (Status != EFI_NOT_FOUND) {
    goto ON_EXIT;
  }

  Status = gRT->SetVariable(
                  VariableName,
                  &gEfiImageSecurityDatabaseGuid,
                  Attr,
                  SigDBSize,
                  Data
                  );

  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

ON_EXIT:

  if (Data != NULL) {
    FreePool (Data);
  }

  return Status;
}


/**
  Enroll X509 certificate into Forbidden Database (DBX) without
  KEK's authentication.

  @param[in] VariableName           Variable name of signature database, must be
  @param[in] *DataBuf               Pointer to Data Buffer
  @param[in] BufSize                Data Buffer size

  @retval    EFI_SUCCESS            New X509 is enrolled successfully.
  @retval    EFI_OUT_OF_RESOURCES   Could not allocate needed resources.

**/
EFI_STATUS
EnrollKeyForbiddenSignatureDatabase (
  IN CHAR16                             *VariableName,
  IN VOID                               *DataBuf,
  IN UINTN                              BufSize
  )
{
  return EnrollX509toForbSigDB (VariableName, DataBuf, BufSize);
}


/**
  Enroll a new X509 certificate into Signature Database (DB or DBX) without
  KEK's authentication.

  @param[in] PrivateData            The module's private data.
  @param[in] VariableName           Variable name of signature database, must be
                                    EFI_IMAGE_SECURITY_DATABASE or EFI_IMAGE_SECURITY_DATABASE1.

  @retval    EFI_SUCCESS            New X509 is enrolled successfully.
  @retval    EFI_OUT_OF_RESOURCES   Could not allocate needed resources.

**/
EFI_STATUS
EnrollX509toSigDB (
  IN CHAR16                         *VariableName,
  IN VOID                           *X509Data,
  IN UINTN                          X509DataSize
  )
{
  EFI_STATUS                        Status;
  EFI_SIGNATURE_LIST                *SigDBCert;
  EFI_SIGNATURE_DATA                *SigDBCertData;
  VOID                              *Data;
  UINTN                             DataSize;
  UINTN                             SigDBSize;
  UINT32                            Attr;

  SigDBSize     = 0;
  DataSize      = 0;
  SigDBCert     = NULL;
  SigDBCertData = NULL;
  Data          = NULL;

  ASSERT (X509Data != NULL);

  SigDBSize = sizeof (EFI_SIGNATURE_LIST) + sizeof (EFI_SIGNATURE_DATA) - 1 + X509DataSize;

  Data = AllocateZeroPool (SigDBSize);
  if (Data == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  //
  // Fill Certificate Database parameters.
  //
  SigDBCert = (EFI_SIGNATURE_LIST *) Data;
  SigDBCert->SignatureListSize   = (UINT32) SigDBSize;
  SigDBCert->SignatureHeaderSize = 0;
  SigDBCert->SignatureSize = (UINT32) (sizeof (EFI_SIGNATURE_DATA) - 1 + X509DataSize);
  CopyGuid (&SigDBCert->SignatureType, &gEfiCertX509Guid);

  SigDBCertData = (EFI_SIGNATURE_DATA *) ((UINT8 *) SigDBCert + sizeof (EFI_SIGNATURE_LIST));
  CopyGuid (&SigDBCertData->SignatureOwner, &gOwnerSignatureGUID);
  CopyMem ((UINT8 *) (SigDBCertData->SignatureData), X509Data, X509DataSize);

  //
  // Check if signature database entry has been already existed.
  // If true, use EFI_VARIABLE_APPEND_WRITE attribute to append the
  // new signature data to original variable
  //
  Attr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS
          | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;
  Status = CreateTimeBasedPayload (&SigDBSize, (UINT8 **) &Data);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Fail to create time-based data payload: %r", Status));
    goto ON_EXIT;
  }

  Status = gRT->GetVariable(
                  VariableName,
                  &gEfiImageSecurityDatabaseGuid,
                  NULL,
                  &DataSize,
                  NULL
                  );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Attr |= EFI_VARIABLE_APPEND_WRITE;
  } else if (Status != EFI_NOT_FOUND) {
    goto ON_EXIT;
  }

  Status = gRT->SetVariable(
                  VariableName,
                  &gEfiImageSecurityDatabaseGuid,
                  Attr,
                  SigDBSize,
                  Data
                  );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

ON_EXIT:

  if (Data != NULL) {
    FreePool (Data);
  }

  return Status;
}


EFI_STATUS
EnrollSignatureDatabase (
  IN CHAR16                             *VariableName,
  IN VOID                               *DataBuf,
  IN UINTN                              BufSize
  )
{
  return EnrollX509toSigDB (VariableName, DataBuf, BufSize);
}


/**
  Function to Load Secure Keys given the binary GUID

  @param[in]      VendorGuid       GUID of the Variable.
  @param[in]      VariableName     Name of the Variable.
  @param[in]      VendorGuid       GUID of the Variable.

  @retval         EFI_SUCCESS      Set the variable successfully.
  @retval         Others           Set variable failed.

**/
EFI_STATUS
SetSecureVariabeKeys (
  IN  EFI_GUID                  *ImageGuid,
  IN  CHAR16                    *VariableName,
  IN  EFI_GUID                  *VendorGuid
  )
{
  EFI_STATUS                     Status;
  EFI_FIRMWARE_VOLUME2_PROTOCOL  *Fv;
  UINTN                          FvProtocolCount;
  EFI_HANDLE                     *FvHandles;
  UINTN                          Index1;
  UINT32                         AuthenticationStatus;
  UINT8                          *Buffer=NULL;
  UINTN                          BufferSize=0;
  UINT32                         Attr;

  Attr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS;

  FvHandles = NULL;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &FvProtocolCount,
                  &FvHandles
                  );

  if (!EFI_ERROR (Status)) {
    for (Index1 = 0; Index1 < FvProtocolCount; Index1++) {
      Status = gBS->HandleProtocol (
                      FvHandles[Index1],
                      &gEfiFirmwareVolume2ProtocolGuid,
                      (VOID **) &Fv
                      );
      BufferSize= 0;

      Status = Fv->ReadSection (
                     Fv,
                     ImageGuid,
                     EFI_SECTION_RAW,
                     0,
                     (VOID **) &Buffer,
                     &BufferSize,
                     &AuthenticationStatus
                     );

      if (!EFI_ERROR (Status)) {
        Status = EFI_SUCCESS;
        break;
      }
    }
  }

  if (Buffer == NULL)
    return EFI_UNSUPPORTED;
  if (StrCmp (VariableName, L"PK") == 0){
    Status = EnrollPlatformKey (Buffer, BufferSize);
  } else if (StrCmp (VariableName, L"KEK") == 0) {
    Status = EnrollKeyExchangeKey (Buffer, BufferSize);
  } else if (CompareGuid (ImageGuid, &gDbxUpdateImageGuid)) {
    Status = EnrollKeyForbiddenSignatureDatabase (VariableName,Buffer, BufferSize);
  } else {
    Status = EnrollSignatureDatabase (VariableName, Buffer, BufferSize);
  }
  return Status;
}


/**
  Internal function to Update User Mode to Setup Mode given its name and GUID, no authentication
  required.

  @param[in]      VariableName            Name of the Variable.
  @param[in]      VendorGuid              GUID of the Variable.

  @retval         EFI_SUCCESS             Updated to Setup Mode successfully.
  @retval         Others                  The driver failed to start the device.

**/
EFI_STATUS
UpdateSetupModetoUserMode (
  IN  CHAR16                    *VariableName,
  IN  EFI_GUID                  *VendorGuid
  )
{
  EFI_STATUS     Status;
  VOID*          Variable;
  UINT8          SetupMode;
  UINT8          SecureBootEnable;

  SetupMode = 0;
  SecureBootEnable = 1;

  GetVariable2 (VariableName, VendorGuid, &Variable, NULL);
  if (Variable == NULL) {
    return EFI_SUCCESS;
  }

  Status = gRT->SetVariable (
                  VariableName,
                  VendorGuid,
                  EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  1,
                  &SetupMode
                  );

  if (!EFI_ERROR (Status)) {
    Status = gRT->SetVariable (
                    EFI_SECURE_BOOT_ENABLE_NAME,
                    &gEfiSecureBootEnableDisableGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    sizeof (UINT8),
                    &SecureBootEnable
                    );
  }
  return Status;
}


/**
  Enrolls PK, KEK, Db and Dbx.

  Note: Setup variable uses UEFI Runtime Services.
        Do not call this function from PEI.

**/
VOID
EnrollKeys (
  VOID
  )
{
  EFI_STATUS              Status;
  UINT8                   SecureBootCstMde;
  UINTN                   DataSize;
  SYSTEM_CONFIGURATION    SystemConfiguration;
  UINTN                   VarSize;

  EFI_GUID  KekImageGuid  = { 0x5d354a1f, 0x98d7, 0x4938, 0x8f, 0x18, 0xf8, 0x4e, 0x1c, 0x89, 0xb2, 0xed };
  EFI_GUID  Db1ImageGuid  = { 0x4de09060, 0x5864, 0x471a, 0xb3, 0x52, 0xd4, 0x50, 0x6e, 0xd7, 0xbb, 0xb0 };
  EFI_GUID  DbxImageGuid  = { 0x96b44e98, 0x6c49, 0x4c03, 0xa8, 0xa4, 0x77, 0x93, 0xef, 0x41, 0x68, 0x5a };
  EFI_GUID  PkImageGuid   = { 0xc43024ad, 0x8cb8, 0x4393, 0x8a, 0xe1, 0xf3, 0x5c, 0xbf, 0xc7, 0xcd, 0x56 };
  EFI_GUID  Db2ImageGuid  = { 0x0f97c7a2, 0xba0c, 0x4e8a, 0x90, 0xf9, 0xb1, 0xcc, 0x40, 0x57, 0x01, 0xf8 };
  EFI_GUID  Db3ImageGuid  = { 0x774491b2, 0x85ff, 0x47b0, 0x89, 0xa4, 0xcc, 0xd8, 0xb3, 0x99, 0xaa, 0xd4 };
  EFI_GUID  Kek2ImageGuid = { 0xE989363D, 0x449F, 0x4b32, 0x96, 0xB0, 0xB2, 0x71, 0x73, 0x44, 0xD0, 0xEE };
  EFI_GUID  Db4ImageGuid  = { 0xB69B054C, 0x7EA4, 0x4f13, 0xB7, 0xFF, 0x72, 0xC6, 0x32, 0x3B, 0xC8, 0x5A };
  EFI_GUID  Db5ImageGuid  = { 0xB8FA2839, 0xE0C1, 0x4368, 0xA5, 0x1B, 0x5F, 0x4A, 0x21, 0x74, 0x61, 0x29 };
  EFI_GUID  Db6ImageGuid  = { 0x758FBB84, 0xEF4C, 0x4acf, 0xB1, 0xA6, 0xE8, 0x44, 0xD5, 0xFF, 0x6B, 0xA6 };

  VarSize = sizeof (SYSTEM_CONFIGURATION);
  Status = gRT->GetVariable (
                  L"Setup",
                  &gEfiSetupVariableGuid,
                  NULL,
                  &VarSize,
                  &SystemConfiguration
                  );

  ASSERT_EFI_ERROR (Status);

  //
  // Enroll Key Exchange Key
  //
  SetSecureVariabeKeys (&KekImageGuid, EFI_KEY_EXCHANGE_KEY_NAME, &gEfiGlobalVariableGuid);
  if (!(SystemConfiguration.UseProductKey)) {
    SetSecureVariabeKeys (&Kek2ImageGuid, EFI_KEY_EXCHANGE_KEY_NAME, &gEfiGlobalVariableGuid);
    //
    // Enroll Authenticated database.
    //
    SetSecureVariabeKeys (&Db1ImageGuid, EFI_IMAGE_SECURITY_DATABASE, &gEfiImageSecurityDatabaseGuid);
    SetSecureVariabeKeys (&Db4ImageGuid, EFI_IMAGE_SECURITY_DATABASE, &gEfiImageSecurityDatabaseGuid);
    SetSecureVariabeKeys (&Db5ImageGuid, EFI_IMAGE_SECURITY_DATABASE, &gEfiImageSecurityDatabaseGuid);
    SetSecureVariabeKeys (&Db6ImageGuid, EFI_IMAGE_SECURITY_DATABASE, &gEfiImageSecurityDatabaseGuid);
    //
    // Enroll Platform Key - 219_Microsoft_UEFI_Logo_Test_KEK.cer for WOS and common_PK.x509.cer for AOS
    //
    SetSecureVariabeKeys (&PkImageGuid, EFI_PLATFORM_KEY_NAME, &gEfiGlobalVariableGuid);
  } else {
    //
    // Enroll Platform Key - KEK_MSFTproductionKekCA.cer
    //
    SetSecureVariabeKeys (&KekImageGuid, EFI_PLATFORM_KEY_NAME, &gEfiGlobalVariableGuid);
  }
  SetSecureVariabeKeys (&Db2ImageGuid, EFI_IMAGE_SECURITY_DATABASE, &gEfiImageSecurityDatabaseGuid);
  SetSecureVariabeKeys (&Db3ImageGuid, EFI_IMAGE_SECURITY_DATABASE, &gEfiImageSecurityDatabaseGuid);
  //
  //Enroll Forbidden  Database
  //
  SetSecureVariabeKeys (&DbxImageGuid, EFI_IMAGE_SECURITY_DATABASE1, &gEfiImageSecurityDatabaseGuid);
  SetSecureVariabeKeys (&gDbxUpdateImageGuid, EFI_IMAGE_SECURITY_DATABASE1, &gEfiImageSecurityDatabaseGuid);

  //
  // If secure boot mode in custom mode, change to standard mode.
  //
  Status = gRT->GetVariable (
                  EFI_CUSTOM_MODE_NAME,
                  &gEfiCustomModeEnableGuid,
                  NULL,
                  &DataSize,
                  &SecureBootCstMde
                  );

  if (SecureBootCstMde) {
    SecureBootCstMde = !SecureBootCstMde;
    Status = gRT->SetVariable (
                    EFI_CUSTOM_MODE_NAME,
                    &gEfiCustomModeEnableGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    sizeof (UINT8),
                    &SecureBootCstMde
                    );
  }
}


/**
  Internal function to delete a Variable given its name and GUID, no authentication
  required.

  @param[in]      VariableName             Name of the Variable.
  @param[in]      VendorGuid               GUID of the Variable.

  @retval         EFI_SUCCESS              Variable deleted successfully.
  @retval         Others                   The driver failed to start the device.

**/
EFI_STATUS
DeleteVariable (
  IN  CHAR16                    *VariableName,
  IN  EFI_GUID                  *VendorGuid
  )
{
  EFI_STATUS              Status;
  VOID*                   Variable;
  UINT8                   *Data;
  UINTN                   DataSize;
  UINT32                  Attr;

  GetVariable2 (VariableName, VendorGuid, &Variable, NULL);
  if (Variable == NULL) {
    return EFI_SUCCESS;
  }

  Data     = NULL;
  DataSize = 0;
  Attr     = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS
             | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;

  Status = CreateTimeBasedPayload (&DataSize, &Data);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Fail to create time-based data payload: %r", Status));
    return Status;
  }

  Status = gRT->SetVariable (
                  VariableName,
                  VendorGuid,
                  Attr,
                  DataSize,
                  Data
                  );
  if (Data != NULL) {
    FreePool (Data);
  }
  return Status;
}


/**
  Internal function to Update User Mode to Setup Mode given its name and GUID, no authentication
  required.

  @param[in]      VariableName             Name of the Variable.
  @param[in]      VendorGuid               GUID of the Variable.

  @retval         EFI_SUCCESS              Updated to Setup Mode successfully.
  @retval         Others                   The driver failed to start the device.

**/
EFI_STATUS
UpdateUserModetoSetupMode (
  IN  CHAR16                    *VariableName,
  IN  EFI_GUID                  *VendorGuid
  )
{
  EFI_STATUS              Status;
  VOID*                   Variable;
  UINT8                   SetupMode;
  UINT8                   SecureBootDisable;

  SetupMode = 1;
  SecureBootDisable = 0;

  GetVariable2 (VariableName, VendorGuid, &Variable, NULL);
  if (Variable == NULL) {
    return EFI_SUCCESS;
  }

  Status = gRT->SetVariable (
                  VariableName,
                  VendorGuid,
                  EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  1,
                  &SetupMode
                  );

  if (!EFI_ERROR (Status)) {
    GetVariable2 (EFI_SECURE_BOOT_ENABLE_NAME, &gEfiSecureBootEnableDisableGuid, &Variable, NULL);
    Status = gRT->SetVariable (
                    EFI_SECURE_BOOT_ENABLE_NAME,
                    &gEfiSecureBootEnableDisableGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    sizeof (UINT8),
                    &SecureBootDisable
                    );
  }
  return Status;
}


/**
  Deletes PK, KEK, Db and Dbx.

**/
VOID
DeleteKeys (
  )
{
  //
  // 1. Clear PK.
  //
  DeleteVariable (EFI_PLATFORM_KEY_NAME, &gEfiGlobalVariableGuid);

  //
  // 2. Update "SetupMode" variable to SETUP_MODE.
  //
  UpdateUserModetoSetupMode (EFI_SETUP_MODE_NAME, &gEfiGlobalVariableGuid);

  //
  // 3. Clear KEK, DB and DBX.
  //
  DeleteVariable (EFI_KEY_EXCHANGE_KEY_NAME, &gEfiGlobalVariableGuid);
  DeleteVariable (EFI_IMAGE_SECURITY_DATABASE, &gEfiImageSecurityDatabaseGuid);
  DeleteVariable (EFI_IMAGE_SECURITY_DATABASE1, &gEfiImageSecurityDatabaseGuid);
}


/**
  Enable Custom Mode.

**/
 VOID
 EnableCustomMode (
  )
{
  UINT8          CustomMode;
  EFI_STATUS     Status;

  CustomMode = 1;

  Status = gRT->SetVariable (
                  EFI_CUSTOM_MODE_NAME,
                  &gEfiCustomModeEnableGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  sizeof (UINT8),
                  &CustomMode
                  );

}

