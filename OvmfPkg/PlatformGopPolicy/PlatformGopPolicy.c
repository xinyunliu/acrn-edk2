/*++

Copyright (c)  1999  - 2014, Intel Corporation. All rights reserved

  This program and the accompanying materials are licensed and made available under
  the terms and conditions of the BSD License that accompanies this distribution.
  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.


--*/

/** @file
**/

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/PlatformGopPolicy.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/PciLib.h>
#include "IgdOpRegion.h"

PLATFORM_GOP_POLICY_PROTOCOL  mPlatformGOPPolicy;

//
// Function implementations
//

/**
  The function will execute with as the platform policy, and gives
  the Platform Lid Status. IBV/OEM can customize this code for their specific
  policy action.

  @param CurrentLidStatus  Gives the current LID Status

  @retval EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
GetPlatformLidStatus (
   OUT LID_STATUS *CurrentLidStatus
)
{
  return EFI_UNSUPPORTED;
}

/**
  The function will execute and gives the Video Bios Table Size and Address.

  @param VbtAddress  Gives the Physical Address of Video BIOS Table

  @param VbtSize     Gives the Size of Video BIOS Table

  @retval EFI_STATUS.

**/

EFI_STATUS
EFIAPI
GetVbtData (
   OUT EFI_PHYSICAL_ADDRESS *VbtAddress,
   OUT UINT32 *VbtSize
)
{
  IGD_OPREGION_STRUCTURE   *OpRegion;

  DEBUG ((EFI_D_ERROR, "GetVbtData\n"));
  OpRegion    = (IGD_OPREGION_STRUCTURE *)(UINTN)PciRead32 (PCI_LIB_ADDRESS (0, 2, 0, 0xFC));
  if ((OpRegion != NULL) && \
      (CompareMem (OpRegion->Header.SIGN, IGD_OPREGION_HEADER_SIGN, sizeof(OpRegion->Header.SIGN)) == 0)) {

    *VbtAddress = (EFI_PHYSICAL_ADDRESS)OpRegion->MBox4.RVBT;
    *VbtSize    = sizeof(OpRegion->MBox4.RVBT);
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}

/**
  Entry point for the Platform GOP Policy Driver.

  @param ImageHandle       Image handle of this driver.
  @param SystemTable       Global system service table.

  @retval EFI_SUCCESS           Initialization complete.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.

**/

EFI_STATUS
EFIAPI
PlatformGOPPolicyEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )

{
  EFI_STATUS  Status = EFI_SUCCESS;

  gBS = SystemTable->BootServices;

  gBS->SetMem (
         &mPlatformGOPPolicy,
         sizeof (PLATFORM_GOP_POLICY_PROTOCOL),
         0
         );

  mPlatformGOPPolicy.Revision                = PLATFORM_GOP_POLICY_PROTOCOL_REVISION_01;
  mPlatformGOPPolicy.GetPlatformLidStatus    = GetPlatformLidStatus;
  mPlatformGOPPolicy.GetVbtData              = GetVbtData;

  //
  // Install protocol to allow access to this Policy.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gPlatformGOPPolicyGuid,
                  &mPlatformGOPPolicy,
                  NULL
                  );

  return Status;
}
