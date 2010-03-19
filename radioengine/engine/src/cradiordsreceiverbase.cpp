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

#include "cradiopubsub.h"
#include "cradiordsreceiverbase.h"
#include "cradioenginelogger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRdsReceiverBase::CRadioRdsReceiverBase( MRadioEngineSettings& aSettings )
    : iSettings( aSettings )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::BaseConstructL()
    {
    iPsName = HBufC::NewL( TRdsPSName().MaxLength() );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRdsReceiverBase::~CRadioRdsReceiverBase()
    {
    iObservers.Close();
    delete iPsName;
    iRadioText.Close();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::AddObserverL( MRadioRdsDataObserver* aObserver )
    {
    TInt index = iObservers.FindInAddressOrder( aObserver );
    if ( index == KErrNotFound )
        {
        iObservers.InsertInAddressOrderL( aObserver );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::RemoveObserver( MRadioRdsDataObserver* aObserver )
    {
    TInt index = iObservers.FindInAddressOrder( aObserver );

    if ( index >= 0 )
        {
        iObservers.Remove( index );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioRdsReceiverBase::AutomaticSwitchingEnabled() const
    {
    return iAfEnabled;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
const TDesC& CRadioRdsReceiverBase::ProgrammeService() const
    {
    return *iPsName;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
const TDesC& CRadioRdsReceiverBase::RadioText() const
    {
    return iRadioText;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioRdsReceiverBase::SignalAvailable() const
    {
    return iSignalAvailable;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::SetAutomaticSwitchingL( TBool aEnable )
    {
    iAfEnabled = aEnable;
    for ( TInt i = 0 ; i < iObservers.Count(); i++ )
        {
        iObservers[i]->RdsAfSearchStateChange( iAfEnabled );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::ClearRdsInformation()
    {
    iPsName->Des().Zero();
    iRadioText.Close();

    if ( iPubSub )
        {
        iPubSub->PublishRdsProgramService( *iPsName );
        }

    MrroRdsEventSignalChange( EFalse );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroStationSeekByPTYComplete( TInt DEBUGVAR( aError ),
                                                          TInt DEBUGVAR( aFrequency ) )
    {
    LOG_FORMAT( "CRadioRdsReceiverBase::MrroStationSeekByPTYComplete: err: %d, frequency: %d ", aError, aFrequency );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroStationSeekByTAComplete( TInt DEBUGVAR( aError ),
                                                         TInt DEBUGVAR( aFrequency ) )
    {
    LOG_FORMAT( "CRadioRdsReceiverBase::MrroStationSeekByTAComplete: err: %d, frequency: %d ", aError, aFrequency );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroStationSeekByTPComplete( TInt DEBUGVAR( aError ),
                                                         TInt DEBUGVAR( aFrequency ) )
    {
    LOG_FORMAT( "CRadioRdsReceiverBase::MrroStationSeekByTPComplete: err: %d, frequency: %d ", aError, aFrequency );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroGetFreqByPTYComplete( TInt DEBUGVAR( aError ),
                                                      RArray<TInt>& /*aFreqList*/ )
    {
    LOG_FORMAT( "CRadioRdsReceiverBase::MrroGetFreqByPTYComplete: err: %d", aError );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroGetFreqByTAComplete( TInt DEBUGVAR( aError ),
                                                     RArray<TInt>& /*aFreqList*/ )
    {
    LOG_FORMAT( "CRadioRdsReceiverBase::MrroGetFreqByTAComplete: err: %d", aError );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroGetPSByPTYComplete( TInt DEBUGVAR( aError ),
                                                    RArray<TRdsPSName>& /*aPsList*/ )
    {
    LOG_FORMAT( "CRadioRdsReceiverBase::MrroGetPSByPTYComplete: err: %d", aError );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroGetPSByTAComplete( TInt DEBUGVAR( aError ),
                                                   RArray<TRdsPSName>& /*aPsList*/ )
    {
    LOG_FORMAT( "CRadioRdsReceiverBase::MrroGetPSByTAComplete: err: %d", aError );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroRdsDataPI( TInt aPi )
    {
    LOG_FORMAT( "CRadioRdsReceiverBase::MrroRdsDataPI: PI: %d", aPi );
    const TInt observerCount = iObservers.Count();
    const TUint32 frequency = iSettings.TunedFrequency();
    for ( TInt i = 0; i < observerCount; ++i )
        {
        iObservers[i]->RdsDataPiCode( frequency, aPi );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroRdsDataPTY( TRdsProgrammeType aPty )
    {
    LOG_FORMAT( "CRadioRdsReceiverBase::MrroRdsDataPTY: PTY: %d", aPty );
    const TInt observerCount = iObservers.Count();
    const TUint32 frequency = iSettings.TunedFrequency();
    for ( TInt i = 0; i < observerCount; ++i )
        {
        iObservers[i]->RdsDataGenre( frequency, aPty );
        }
    }

// ---------------------------------------------------------------------------
// Notifies of the New Programme Service data
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroRdsDataPS( TRdsPSName& aPs )
    {
    LOG_FORMAT( "CRadioRdsReceiverBase::MrroRdsDataPS: PS: %S", &aPs );
    const TUint32 frequency = iSettings.TunedFrequency();
    if ( iPubSub )
        {
        iPubSub->PublishRdsProgramService( aPs );
        }

    iPsName->Des().Copy( aPs );
    const TInt observerCount = iObservers.Count();
    for ( TInt i = 0; i < observerCount; ++i )
        {
        iObservers[i]->RdsDataProgrammeService( frequency, *iPsName );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroRdsDataRT( TRdsRadioText& aRt )
    {
    LOG_FORMAT( "CRadioRdsReceiverBase::MrroRdsDataRT: RT: %S", &aRt );
    iRadioText.Close();
    iRadioText.Create( aRt );

    const TUint32 frequency = iSettings.TunedFrequency();
    if ( iPubSub )
        {
        iPubSub->PublishRdsRadioText( iRadioText );
        }

    const TInt observerCount = iObservers.Count();
    for ( TInt i = 0; i < observerCount; ++i )
        {
        iObservers[i]->RdsDataRadioText( frequency, iRadioText );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroRdsDataCT( TDateTime& /*aCt*/ )
    {
    LOG( "CRadioRdsReceiverBase::MrroRdsDataCT" );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroRdsDataTA( TBool DEBUGVAR( aTaOn ) )
    {
    LOG_FORMAT( "CRadioRdsReceiverBase::MrroRdsDataTA: aTaOn: %d", aTaOn );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroRdsDataRTplus( TRdsRTplusClass aRtPlusClass,
                                               TRdsRadioText& aRtPlusData )
    {
    LOG_FORMAT( "CRadioRdsReceiverBase::MrroRdsDataRTplus: Class: %d, data: %S", aRtPlusClass, &aRtPlusData );
    const TUint32 frequency = iSettings.TunedFrequency();
    const TInt observerCount = iObservers.Count();
    for ( TInt i = 0; i < observerCount; ++i )
        {
        iObservers[i]->RdsDataRadioTextPlus( frequency, aRtPlusClass, aRtPlusData );
        }
    }

// ---------------------------------------------------------------------------
// Notifies of the start of Alternate Frequency search
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroRdsSearchBeginAF()
    {
    LOG( "CRadioRdsReceiverBase::MrroRdsSearchBeginAF" );
    const TInt observerCount = iObservers.Count();
    for ( TInt i = 0; i < observerCount; ++i )
        {
        iObservers[i]->RdsAfSearchBegin();
        }
    }

// ---------------------------------------------------------------------------
// Notifies of the end of Alternate Frequency search
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroRdsSearchEndAF( TInt aError, TInt aFrequency )
    {
    LOG_FORMAT( "CRadioRdsReceiverBase::MrroRdsSearchEndAF: err: %d, Frequency: %d", aError, aFrequency );
    const TInt observerCount = iObservers.Count();
    for ( TInt i = 0; i < observerCount; ++i )
        {
        // Radio utility uses Hz, UI uses kHz. It's ok to divide with 1000, possible lost fractions are useless.
        iObservers[i]->RdsAfSearchEnd( aFrequency, aError );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroRdsStationChangeTA( TInt DEBUGVAR( aFrequency ) )
    {
    LOG_FORMAT( "CRadioRdsReceiverBase::MrroRdsStationChangeTA: Frequency: %d", aFrequency );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroRdsEventAutomaticSwitchingChange( TBool DEBUGVAR( aAuto ) )
    {
    LOG_FORMAT( "CRadioRdsReceiverBase::MrroRdsEventAutomaticSwitchingChange: aAuto: %d", aAuto );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroRdsEventAutomaticTrafficAnnouncement( TBool DEBUGVAR( aAuto ) )
    {
    LOG_FORMAT( "CRadioRdsReceiverBase::MrroRdsEventAutomaticTrafficAnnouncement: aAuto: %d", aAuto );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroRdsEventSignalChange( TBool aSignal )
    {
    LOG_FORMAT( "CRadioRdsReceiverBase::MrroRdsEventSignalChange: aSignal: %d", aSignal );
    iSignalAvailable = aSignal;
    const TUint32 frequency = iSettings.TunedFrequency();
    const TInt observerCount = iObservers.Count();
    for ( TInt i = 0; i < observerCount; ++i )
        {
        iObservers[i]->RdsAvailable( frequency, iSignalAvailable );
        }
    }
