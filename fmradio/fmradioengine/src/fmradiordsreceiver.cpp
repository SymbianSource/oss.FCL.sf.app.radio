/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  The RDS receiver implementation for FM Radio
*
*/


#include "fmradiordsreceiver.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiver::CFMRadioRdsReceiver
// ---------------------------------------------------------------------------
//
CFMRadioRdsReceiver::CFMRadioRdsReceiver( TRadioSettings& aSettings ) : CFMRadioRdsReceiverBase(aSettings)
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiver::ConstructL
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiver::ConstructL()
    {
    BaseConstructL();
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiver::NewL
// ---------------------------------------------------------------------------
//
CFMRadioRdsReceiver* CFMRadioRdsReceiver::NewL( TRadioSettings& aSettings )
    {
    CFMRadioRdsReceiver* self = new ( ELeave ) CFMRadioRdsReceiver( aSettings );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiver::~CFMRadioRdsReceiver
// ---------------------------------------------------------------------------
//
CFMRadioRdsReceiver::~CFMRadioRdsReceiver()
    {
    if( iRdsUtility )
        {
        iRdsUtility->Close();
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiver::InitL
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiver::InitL( CRadioUtility& aRadioUtility, CFMRadioPubSub* aPubSub )
    {    
    iPubSub = aPubSub;
    
    iRdsUtility = &aRadioUtility.RadioRdsUtilityL( *this );

    SetAutomaticSwitchingL( iSettings.IsRdsAfSearchEnabled() );
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiver::SetAutomaticSwitchingL
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiver::SetAutomaticSwitchingL( TBool aEnable )
    {
    if( iRdsUtility )
        {
        User::LeaveIfError( iRdsUtility->SetAutomaticSwitching( aEnable ) );
        }
    
    CFMRadioRdsReceiverBase::SetAutomaticSwitchingL( aEnable );
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiver::StartReceiver
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiver::StartReceiver()
    {    
    TRdsData rdsData;
    rdsData.iRdsFunctions = ERdsProgrammeService | ERdsAlternateFrequency | ERdsRadioText | ERdsRadioTextPlus;
    rdsData.iAdditionalFunctions1 = 0;
    rdsData.iAdditionalFunctions2 = 0;
    
    if( iRdsUtility && !iStarted)
        {
        if( iRdsUtility->NotifyRdsDataChange( rdsData ) == KErrNone )
            {
            // Avoid further calls
            iStarted = ETrue;
            }
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiver::StopReceiver
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiver::StopReceiver()
    {
    if( iRdsUtility )
        {
        iRdsUtility->CancelNotifyRdsDataChange();
        }
    iStarted = EFalse;
    }
