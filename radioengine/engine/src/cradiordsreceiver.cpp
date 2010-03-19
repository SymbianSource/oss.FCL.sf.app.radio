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
* Description:
*
*/

#include "cradioenginelogger.h"

#include "cradiordsreceiver.h"
#include "mradioenginesettings.h"
#include "mradiordsdataobserver.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRdsReceiver::CRadioRdsReceiver( MRadioEngineSettings& aSettings )
    : CRadioRdsReceiverBase( aSettings )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiver::ConstructL()
    {
    BaseConstructL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRdsReceiver* CRadioRdsReceiver::NewL( MRadioEngineSettings& aSettings )
    {
    CRadioRdsReceiver* self = new ( ELeave ) CRadioRdsReceiver( aSettings );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRdsReceiver::~CRadioRdsReceiver()
    {
    if ( iRdsUtility )
        {
        iRdsUtility->Close();
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiver::InitL( CRadioUtility& aRadioUtility, CRadioPubSub* aPubSub )
    {
    LOG_METHOD_AUTO;
    iPubSub = aPubSub;

    iRdsUtility = &aRadioUtility.RadioRdsUtilityL( *this );

    LogReceiverCapabilities();

    SetAutomaticSwitchingL( iSettings.RdsAfSearchEnabled() );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiver::SetAutomaticSwitchingL( TBool aEnable )
    {
    if ( iRdsUtility )
        {
        User::LeaveIfError( iRdsUtility->SetAutomaticSwitching( aEnable ) );
        }

    // Notifies the observers
    CRadioRdsReceiverBase::SetAutomaticSwitchingL( aEnable );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiver::StartReceiver()
    {
    // Request to be notified of almost all RDS data.
    // Unwanted RDS values:
    //    ERdsClockTime
    //    ERdsTrafficProgramme
    //    ERdsTrafficAnnouncement
    //    ERdsAlternateFrequency
    TRdsData rdsData;
    rdsData.iRdsFunctions = ERdsProgrammeIdentification |
                            ERdsProgrammeType |
                            ERdsProgrammeService |
                            ERdsRadioText |
                            ERdsRadioTextPlus;

    rdsData.iAdditionalFunctions1 = 0;
    rdsData.iAdditionalFunctions2 = 0;

    LOG_ASSERT( iRdsUtility, LOG( "CRadioRdsReceiver::StartReceiverL. Error: RDS utility not created!" ) );
    if ( iRdsUtility && !iStarted )
        {
        TInt err = iRdsUtility->NotifyRdsDataChange( rdsData );
        if ( !err )
            {
            // Avoid further calls
            iStarted = ETrue;
            LOG( "CRadioRdsReceiver::StartReceiverL: Requested RDS notifications from receiver." );
            }
        else
            {
            LOG_FORMAT( "CRadioRdsReceiver::StartReceiverL. Failed to request RDS data with err %d", err );
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiver::StopReceiver()
    {
    LOG( "CRadioRdsReceiver::StopReceiver" );

    if ( iRdsUtility )
        {
        iRdsUtility->CancelNotifyRdsDataChange();
        }
    iStarted = EFalse;
    }

// ---------------------------------------------------------------------------
// Logs the RDS receiver capabilities if logging is enabled
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiver::LogReceiverCapabilities()
    {
#ifdef LOGGING_ENABLED
    // Log the RDS utility capabilities
    if ( iRdsUtility )
        {
        TRdsCapabilities caps;
        caps.iRdsFunctions = 0;
        caps.iAdditionalFunctions1 = 0;
        caps.iAdditionalFunctions2 = 0;
        TInt err = iRdsUtility->GetCapabilities( caps );
        if ( !err )
            {
            TBuf<100> capsBuf;
            if ( caps.iRdsFunctions & ERdsProgrammeIdentification == ERdsProgrammeIdentification )
                {
                capsBuf.Append( _L("PI ") );
                }
            if ( caps.iRdsFunctions & ERdsProgrammeType == ERdsProgrammeType )
                {
                capsBuf.Append( _L("Genre ") );
                }
            if ( caps.iRdsFunctions & ERdsProgrammeService == ERdsProgrammeService )
                {
                capsBuf.Append( _L("PS ") );
                }
            if ( caps.iRdsFunctions & ERdsRadioText == ERdsRadioText )
                {
                capsBuf.Append( _L("RT ") );
                }
            if ( caps.iRdsFunctions & ERdsRadioTextPlus == ERdsRadioTextPlus )
                {
                capsBuf.Append( _L("RT+ ") );
                }
            if ( caps.iRdsFunctions & ERdsAlternateFrequency == ERdsAlternateFrequency )
                {
                capsBuf.Append( _L("AF ") );
                }
            if ( caps.iRdsFunctions & ERdsClockTime == ERdsClockTime )
                {
                capsBuf.Append( _L("Time ") );
                }
            if ( caps.iRdsFunctions & ERdsTrafficProgramme == ERdsTrafficProgramme )
                {
                capsBuf.Append( _L("TrafficProgramme ") );
                }
            if ( caps.iRdsFunctions & ERdsTrafficAnnouncement == ERdsTrafficAnnouncement )
                {
                capsBuf.Append( _L("TrafficAnnouncement ") );
                }
            LOG_FORMAT( "CRadioRdsReceiver::LogReceiverCapabilities. RDS receiver capabilities: %S", &capsBuf );
            LOG_FORMAT( "CRadioRdsReceiver::LogReceiverCapabilities. Functions: %d, AdditionalFunctions1: %d, iAdditionalFunctions2: %d",
                    caps.iRdsFunctions, caps.iAdditionalFunctions1, caps.iAdditionalFunctions2 );
            }
        else
            {
            LOG_FORMAT( "CRadioRdsReceiver::LogReceiverCapabilities: Failed to get RDS utility capabilities: err %d", err );
            }
        }
#endif // LOGGING_ENABLED
    }
