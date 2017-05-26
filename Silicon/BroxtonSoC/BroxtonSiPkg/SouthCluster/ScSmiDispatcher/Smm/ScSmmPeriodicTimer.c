/** @file
  File to contain all the hardware specific stuff for the Periodical Timer dispatch protocol.

  Copyright (c) 2012 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "ScSmmHelpers.h"

//
// There is only one instance for PeriodicTimerCommBuffer.
// It's safe in SMM since there is no re-entry for the function.
//
EFI_SMM_PERIODIC_TIMER_CONTEXT          mPchPeriodicTimerCommBuffer;

typedef enum {
  PERIODIC_TIMER= 0,
  SWSMI_TIMER,
  NUM_TIMERS
} SUPPORTED_TIMER;

typedef struct _TIMER_INTERVAL {
  UINT64  Interval;
  UINT8   AssociatedTimer;
} TIMER_INTERVAL;

#define NUM_INTERVALS 8

///
/// Time constants, in 100 nano-second units
///
#define TIME_64s    640000000 /* 64   s  */
#define TIME_32s    320000000 /* 32   s  */
#define TIME_16s    160000000 /* 16   s  */
#define TIME_8s     80000000  /*  8   s  */
#define TIME_64ms   640000    /* 64   ms */
#define TIME_32ms   320000    /* 32   ms */
#define TIME_16ms   160000    /* 16   ms */
#define TIME_1_5ms  15000     /* 1.5  ms */

typedef enum {
  INDEX_TIME_64s  = 0,
  INDEX_TIME_32s,
  INDEX_TIME_16s,
  INDEX_TIME_8s,
  INDEX_TIME_64ms,
  INDEX_TIME_32ms,
  INDEX_TIME_16ms,
  INDEX_TIME_1_5ms,
  INDEX_TIME_MAX
} TIMER_INTERVAL_INDEX;

static TIMER_INTERVAL mSmmPeriodicTimerIntervals[NUM_INTERVALS] = {
  {
    TIME_64s,
    PERIODIC_TIMER
  },

  {
    TIME_32s,
    PERIODIC_TIMER
  },

  {
    TIME_16s,
    PERIODIC_TIMER
  },

  {
    TIME_8s,
    PERIODIC_TIMER
  },

  {
    TIME_64ms,
    SWSMI_TIMER
  },

  {
    TIME_32ms,
    SWSMI_TIMER
  },

  {
    TIME_16ms,
    SWSMI_TIMER
  },

  {
    TIME_1_5ms,
    SWSMI_TIMER
  },
};

typedef struct _TIMER_INFO {
  UINTN   NumChildren;        ///< number of children using this timer
  UINT64  MinReqInterval;     ///< minimum interval required by children
  UINTN   CurrentSetting;     ///< interval this timer is set at right now (index into interval table)
} TIMER_INFO;

TIMER_INFO          mTimers[NUM_TIMERS];

SC_SMM_SOURCE_DESC mTIMER_SOURCE_DESCS[NUM_TIMERS] = {
  {
    SC_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          {R_SMI_EN}
        },
        S_SMI_EN,
        N_SMI_EN_PERIODIC
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
        N_SMI_STS_PERIODIC
      }
    }
  },

  {
    SC_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          {R_SMI_EN}
        },
        S_SMI_EN,
        N_SMI_EN_SWSMI_TMR
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
        N_SMI_STS_SWSMI_TMR
      }
    }
  }
};


VOID
ScSmmPeriodicTimerProgramTimers (
  VOID
  );


/**
  Convert the dispatch context to the timer interval, this function will assert if then either:
  (1) The context contains an invalid interval
  (2) The timer interval table is corrupt

  @param[in] DispatchContext      The pointer to the Dispatch Context

  @retval    TIMER_INTERVAL       The timer interval of input dispatch context

**/
TIMER_INTERVAL *
ContextToTimerInterval (
  IN  SC_SMM_CONTEXT              *DispatchContext
  )
{
  UINTN  loopvar;

  //
  // Determine which timer this child is using
  //
  for (loopvar = 0; loopvar < NUM_INTERVALS; loopvar++) {
    if (((DispatchContext->PeriodicTimer.SmiTickInterval == 0) &&
         (DispatchContext->PeriodicTimer.Period >= mSmmPeriodicTimerIntervals[loopvar].Interval)) ||
        (DispatchContext->PeriodicTimer.SmiTickInterval == mSmmPeriodicTimerIntervals[loopvar].Interval)) {
      return &mSmmPeriodicTimerIntervals[loopvar];
    }
  }
  //
  // If this assertion fires, then either:
  //    (1) the context contains an invalid interval
  //    (2) the timer interval table is corrupt
  //
  ASSERT (FALSE);

  return NULL;
}


/**
  Figure out which timer the child is requesting and
  send back the source description

  @param[in]  DispatchContext      The pointer to the Dispatch Context instances
  @param[out] SrcDesc              The pointer to the source description

  @retval     None

**/
VOID
MapPeriodicTimerToSrcDesc (
  IN  SC_SMM_CONTEXT             *DispatchContext,
  OUT SC_SMM_SOURCE_DESC         *SrcDesc
  )
{
  TIMER_INTERVAL                 *TimerInterval;

  //
  // Figure out which timer the child is requesting and
  // send back the source description
  //
  TimerInterval = ContextToTimerInterval (DispatchContext);
  if (TimerInterval == NULL) {
    return;
  }

  CopyMem (
    (VOID *) SrcDesc,
    (VOID *) (&mTIMER_SOURCE_DESCS[TimerInterval->AssociatedTimer]),
    sizeof (SC_SMM_SOURCE_DESC)
    );

  //
  // Program the value of the interval into hardware
  //
  ScSmmPeriodicTimerProgramTimers ();
}


/**
  Update the elapsed time from the Interval data of DATABASE_RECORD

  @param[in]  Record              The pointer to the DATABASE_RECORD.
  @param[out] HwContext           The Context to be updated.

  @retval     None

**/
VOID
EFIAPI
PeriodicTimerGetContext (
  IN  DATABASE_RECORD             *Record,
  OUT SC_SMM_CONTEXT              *HwContext
  )
{
  TIMER_INTERVAL  *TimerInterval;

  ASSERT (Record->ProtocolType == PeriodicTimerType);

  TimerInterval = ContextToTimerInterval (&Record->ChildContext);
  if (TimerInterval == NULL) {
    return;
  }

  //
  // Ignore the hardware context. It's not required for this protocol.
  // Instead, just increment the child's context.
  // Update the elapsed time w/ the data from our tables
  //
  Record->MiscData.ElapsedTime += (UINTN) TimerInterval->Interval;
  *HwContext = Record->ChildContext;
}


/**
  Check whether Periodic Timer of two contexts match

  @param[in] Context1                Context 1 that includes Periodic Timer  1
  @param[in] Context2                Context 2 that includes Periodic Timer  2

  @retval    FALSE                   Periodic Timer match
  @retval    TRUE                    Periodic Timer don't match

**/
BOOLEAN
EFIAPI
PeriodicTimerCmpContext (
  IN SC_SMM_CONTEXT                  *HwContext,
  IN SC_SMM_CONTEXT                  *ChildContext
  )
{
  DATABASE_RECORD                    *Record;

  Record = DATABASE_RECORD_FROM_CHILDCONTEXT (ChildContext);
  if (Record->MiscData.ElapsedTime >= ChildContext->PeriodicTimer.Period) {
    //
    // This child should be dispatched
    // Need reset ElapsedTime, or SMI handler will be invoked during SmiTickInterval instead of Period.
    //
    //
    // For EDKII, the ElapsedTime is reset when PeriodicTimerGetCommBuffer
    //
    return TRUE;
  } else {
    return FALSE;
  }
}


/**
  Gather the CommBuffer information of SmmPeriodicTimerDispatch2.

  @param[in]  Record              No use
  @param[out] CommBuffer          Point to the CommBuffer structure
  @param[out] CommBufferSize      Point to the Size of CommBuffer structure

**/
VOID
EFIAPI
PeriodicTimerGetCommBuffer (
  IN  DATABASE_RECORD             *Record,
  OUT VOID                        **CommBuffer,
  OUT UINTN                       *CommBufferSize
  )
{
  ASSERT (Record->ProtocolType == PeriodicTimerType);

  mPchPeriodicTimerCommBuffer.ElapsedTime = Record->MiscData.ElapsedTime;

  //
  // For EDKII, the ElapsedTime is reset here
  //
  Record->MiscData.ElapsedTime = 0;

  //
  // Return the CommBuffer
  //
  *CommBuffer = (VOID *) &mPchPeriodicTimerCommBuffer;
  *CommBufferSize = sizeof (EFI_SMM_PERIODIC_TIMER_CONTEXT);
}


/**
  Program Smm Periodic Timer

**/
VOID
ScSmmPeriodicTimerProgramTimers (
  VOID
  )
{
  UINT8           GenPmCon1;
  UINT8           GenPmCon2;
  SUPPORTED_TIMER Timer;
  DATABASE_RECORD *RecordInDb;
  LIST_ENTRY      *LinkInDb;
  UINT32          PmcBase;
  TIMER_INTERVAL  *TimerInterval;

  PmcBase = PMC_BASE_ADDRESS;
  //
  // Find the minimum required interval for each timer
  //
  for (Timer = 0; Timer < NUM_TIMERS; Timer++) {
    mTimers[Timer].MinReqInterval = ~(UINT64) 0x0;
    mTimers[Timer].NumChildren    = 0;
  }

  LinkInDb = GetFirstNode (&mPrivateData.CallbackDataBase);
  while (!IsNull (&mPrivateData.CallbackDataBase, LinkInDb)) {
    RecordInDb = DATABASE_RECORD_FROM_LINK (LinkInDb);
    if (RecordInDb->ProtocolType == PeriodicTimerType) {
      //
      // This child is registerd with the PeriodicTimer protocol
      //
      TimerInterval = ContextToTimerInterval (&RecordInDb->ChildContext);
      if (TimerInterval == NULL) {
        return;
      }

      Timer = TimerInterval->AssociatedTimer;
      if (Timer < 0 || Timer >= NUM_TIMERS) {
        ASSERT (FALSE);
        CpuDeadLoop ();
        return;
      }

      if (mTimers[Timer].MinReqInterval > RecordInDb->ChildContext.PeriodicTimer.SmiTickInterval) {
        mTimers[Timer].MinReqInterval = RecordInDb->ChildContext.PeriodicTimer.SmiTickInterval;
      }

      mTimers[Timer].NumChildren++;
    }

    LinkInDb = GetNextNode (&mPrivateData.CallbackDataBase, &RecordInDb->Link);
  }
  //
  // Program the hardware
  //
  if (mTimers[PERIODIC_TIMER].NumChildren > 0) {
    GenPmCon2 = MmioRead8 (PmcBase + R_PMC_GEN_PMCON_2);

    GenPmCon2 &= ~B_PMC_GEN_PMCON_PER_SMI_SEL;
    switch (mTimers[PERIODIC_TIMER].MinReqInterval) {
      case TIME_64s:
        GenPmCon2 |= V_PMC_GEN_PMCON_PER_SMI_64S;
        mTimers[PERIODIC_TIMER].CurrentSetting = INDEX_TIME_64s;
        break;

      case TIME_32s:
        GenPmCon2 |= V_PMC_GEN_PMCON_PER_SMI_32S;
        mTimers[PERIODIC_TIMER].CurrentSetting = INDEX_TIME_32s;
        break;

      case TIME_16s:
        GenPmCon2 |= V_PMC_GEN_PMCON_PER_SMI_16S;
        mTimers[PERIODIC_TIMER].CurrentSetting = INDEX_TIME_16s;
        break;

      case TIME_8s:
        GenPmCon2 |= V_PMC_GEN_PMCON_PER_SMI_8S;
        mTimers[PERIODIC_TIMER].CurrentSetting = INDEX_TIME_8s;
        break;

      default:
        ASSERT (FALSE);
        break;
    }

    MmioWrite8 (PmcBase + R_PMC_GEN_PMCON_2, GenPmCon2);

    //
    // Restart the timer here, just need to clear the SMI
    //
    ScSmmClearSource (&mTIMER_SOURCE_DESCS[PERIODIC_TIMER]);
  } else {
    ScSmmDisableSource (&mTIMER_SOURCE_DESCS[PERIODIC_TIMER]);
  }

  if (mTimers[SWSMI_TIMER].NumChildren > 0) {
    //
    // ICH9, ICH10 and SC share the same bit positions for SW SMI Rate settings
    //
    GenPmCon1 = MmioRead8 (PmcBase + R_PMC_GEN_PMCON_1);
    GenPmCon1 &= ~B_PMC_GEN_PMCON_SWSMI_RTSL;
    switch (mTimers[SWSMI_TIMER].MinReqInterval) {
      case TIME_64ms:
        GenPmCon1 |= V_PMC_GEN_PMCON_SWSMI_RTSL_64MS;
        mTimers[SWSMI_TIMER].CurrentSetting = INDEX_TIME_64ms;
        break;

      case TIME_32ms:
        GenPmCon1 |= V_PMC_GEN_PMCON_SWSMI_RTSL_32MS;
        mTimers[SWSMI_TIMER].CurrentSetting = INDEX_TIME_32ms;
        break;

      case TIME_16ms:
        GenPmCon1 |= V_PMC_GEN_PMCON_SWSMI_RTSL_16MS;
        mTimers[SWSMI_TIMER].CurrentSetting = INDEX_TIME_16ms;
        break;

      case TIME_1_5ms:
        GenPmCon1 |= V_PMC_GEN_PMCON_SWSMI_RTSL_1_5MS;
        mTimers[SWSMI_TIMER].CurrentSetting = INDEX_TIME_1_5ms;
        break;

      default:
        ASSERT (FALSE);
        break;
    }

    MmioWrite8 (PmcBase + R_PMC_GEN_PMCON_1, GenPmCon1);

    //
    // Restart the timer here, need to disable, clear, then enable to restart this timer
    //
    ScSmmDisableSource (&mTIMER_SOURCE_DESCS[SWSMI_TIMER]);
    ScSmmClearSource (&mTIMER_SOURCE_DESCS[SWSMI_TIMER]);
    ScSmmEnableSource (&mTIMER_SOURCE_DESCS[SWSMI_TIMER]);
  } else {
    ScSmmDisableSource (&mTIMER_SOURCE_DESCS[SWSMI_TIMER]);
  }
}


/**
  This services returns the next SMI tick period that is supported by the chipset.
  The order returned is from longest to shortest interval period.

  @param[in]      This                      Pointer to the EFI_SMM_PERIODIC_TIMER_DISPATCH_PROTOCOL instance.
  @param[in, out] SmiTickInterval           Pointer to pointer of the next shorter SMI interval period that is supported by the child.

  @retval         EFI_SUCCESS               The service returned successfully.
  @retval         EFI_INVALID_PARAMETER     The parameter SmiTickInterval is invalid.

**/
EFI_STATUS
EFIAPI
ScSmmPeriodicTimerDispatchGetNextShorterInterval (
  IN CONST EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL  *This,
  IN OUT UINT64                                       **SmiTickInterval
  )
{
  TIMER_INTERVAL  *IntervalPointer;

  if (SmiTickInterval == NULL) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  IntervalPointer = (TIMER_INTERVAL *) *SmiTickInterval;
  if (IntervalPointer == NULL) {
    //
    // The first time child requesting an interval
    //
    IntervalPointer = &mSmmPeriodicTimerIntervals[0];
  } else if (IntervalPointer == &mSmmPeriodicTimerIntervals[NUM_INTERVALS - 1]) {
    //
    // At end of the list
    //
    IntervalPointer = NULL;
  } else {
    if ((IntervalPointer >= &mSmmPeriodicTimerIntervals[0]) &&
        (IntervalPointer < &mSmmPeriodicTimerIntervals[NUM_INTERVALS - 1])
        ) {
      //
      // Get the next interval in the list
      //
      IntervalPointer++;
    } else {
      //
      // Input is out of range
      //
      return EFI_INVALID_PARAMETER;
    }
  }

  if (IntervalPointer != NULL) {
    *SmiTickInterval = &IntervalPointer->Interval;
  } else {
    *SmiTickInterval = NULL;
  }

  return EFI_SUCCESS;
}


/**
  This function is responsible for calculating and enabling any timers that are required
  to dispatch messages to children. The SrcDesc argument isn't acutally used.

  @param[in] SrcDesc              Pointer to the SC_SMM_SOURCE_DESC instance.

  @retval    None

**/
VOID
EFIAPI
ScSmmPeriodicTimerClearSource (
  IN SC_SMM_SOURCE_DESC     *SrcDesc
  )
{
  ScSmmPeriodicTimerProgramTimers ();
}

