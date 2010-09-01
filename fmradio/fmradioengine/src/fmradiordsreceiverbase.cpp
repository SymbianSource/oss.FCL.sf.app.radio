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


#include "fmradiordsreceiverbase.h"
#include "fmradiopubsub.h"
#include "fmradioengineradiosettings.h"

// The delay, in microseconds, after which RDS PS name is concidered static
const TInt KFMRadioStaticRdsPsNameDelay = 10000000; 

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::CFMRadioRdsReceiverBase
// ---------------------------------------------------------------------------
//
CFMRadioRdsReceiverBase::CFMRadioRdsReceiverBase( TRadioSettings& aSettings ) : iSettings(aSettings)
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::BaseConstructL
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverBase::BaseConstructL()
    {
    iPsName = HBufC::NewL( TRdsPSName().MaxLength() );
    iRadioText = HBufC::NewL( TRdsRadioText().MaxLength() );
    iPsNameTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::~CFMRadioRdsReceiverBase
// ---------------------------------------------------------------------------
//
CFMRadioRdsReceiverBase::~CFMRadioRdsReceiverBase()
    {
    iObservers.Close();
    delete iPsName;
    delete iRadioText;
    delete iPsNameTimer;
    
    iProgramWebUrl.Close();
    iArtistName.Close();
    iSongName.Close();
    iAlbumName.Close();
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::AddObserver
// ---------------------------------------------------------------------------
//
EXPORT_C void CFMRadioRdsReceiverBase::AddObserver( MFMRadioRdsObserver* aObserver )
    {
    TInt index = iObservers.FindInAddressOrder( aObserver );
    if( index == KErrNotFound )
        {
        iObservers.InsertInAddressOrder( aObserver );
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::RemoveObserver
// ---------------------------------------------------------------------------
//
EXPORT_C void CFMRadioRdsReceiverBase::RemoveObserver( MFMRadioRdsObserver* aObserver )
    {
    TInt index = iObservers.FindInAddressOrder( aObserver );

    if( index >= 0 )
        {
        iObservers.Remove( index );
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::AutomaticSwitchingEnabled
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CFMRadioRdsReceiverBase::AutomaticSwitchingEnabled() const
    {
    return iAfEnabled;
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::ProgrammeService
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CFMRadioRdsReceiverBase::ProgrammeService() const
    {
    return *iPsName;
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::ProgrammeServiceNameType
// ---------------------------------------------------------------------------
//
EXPORT_C CFMRadioRdsReceiverBase::TFMRadioProgrammeSeviceType CFMRadioRdsReceiverBase::ProgrammeServiceNameType() const
    {
    return iPsNameType;
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::SignalAvailable
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CFMRadioRdsReceiverBase::SignalAvailable() const
    {
    return iSignalAvailable;
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::RtPlusWebUrl
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CFMRadioRdsReceiverBase::RtPlusProgramUrl() const
    {
    return iProgramWebUrl;
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::RtPlusArtist
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CFMRadioRdsReceiverBase::RtPlusArtist() const
    {
    return iArtistName;
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::RtPlusAlbum
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CFMRadioRdsReceiverBase::RtPlusAlbum() const
    {
    return iAlbumName;
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::RtPlusSong
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CFMRadioRdsReceiverBase::RtPlusSong() const
    {
    return iSongName;
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::SetAutomaticSwitchingL
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverBase::SetAutomaticSwitchingL( TBool aEnable )
    {
    iAfEnabled = aEnable;
    for( TInt i = 0 ; i < iObservers.Count() ; i++ )
        {
        iObservers[i]->RdsAfSearchStateChange( iAfEnabled );
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::ClearRdsInformation
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverBase::ClearRdsInformation()
    {
    TRdsPSName name;
    TRdsRadioText radioText;
    iPsNameTimer->Cancel();
    iPsNameType = EFMRadioPSNameStaticAssumed;
    MrroRdsDataPS( name );
    MrroRdsDataRT( radioText );
    iProgramWebUrl.Close();
    iArtistName.Close();
    iSongName.Close();
    iAlbumName.Close();
    MrroRdsEventSignalChange( EFalse );
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::StaticPsNameTimerCallback
// ---------------------------------------------------------------------------
//
TInt CFMRadioRdsReceiverBase::StaticPsNameTimerCallback( TAny* aSelf )
    {
    CFMRadioRdsReceiverBase* self = static_cast<CFMRadioRdsReceiverBase*>( aSelf );
    if( self )
        {
        self->iPsNameTimer->Cancel();
        if ( self->iPsNameType == EFMRadioPSNameStaticAssumed )
            {
            self->iPsNameType = EFMRadioPSNameStatic;
            for( TInt i = 0 ; i < self->iObservers.Count() ; i++ )
                {
                self->iObservers[i]->RdsDataPsNameIsStatic( ETrue );
                }
            }
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::HandleRdsDataRTplusL
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverBase::HandleRdsDataRTplusL( TRdsRTplusClass aRtPlusClass, TRdsRadioText& aRtPlusData )
    {
    TBool dataChanged = EFalse;
    switch (aRtPlusClass)
        {
        case ERTplusItemTitle:
            if ( aRtPlusData != iSongName )
                {
                dataChanged = ETrue;
                iSongName.Close();
                iSongName.CreateL( aRtPlusData );
                }
            break;
        case ERTplusItemArtist:
        case ERTplusItemBand:
            if ( aRtPlusData != iArtistName )
                {
                dataChanged = ETrue;
                iArtistName.Close();
                iArtistName.CreateL( aRtPlusData );
                }
            break;
        case ERTplusItemAlbum:
            if ( aRtPlusData != iAlbumName )
                {
                dataChanged = ETrue;
                iAlbumName.Close();
                iAlbumName.CreateL( aRtPlusData );
                }
            break;
        case ERTplusProgramHomepage:
            {
            if ( aRtPlusData != iProgramWebUrl )
                {
                dataChanged = ETrue;
                iProgramWebUrl.Close();
                iProgramWebUrl.CreateL( aRtPlusData );
                }
            break;
            }
        default:
            // Let uncached values go through
            dataChanged = ETrue;
            break;
        }
    
    if ( dataChanged )
        {
        for( TInt i = 0 ; i < iObservers.Count() ; i++ )
            {
            iObservers[i]->RdsDataRadioTextPlus( aRtPlusClass, aRtPlusData );
            }
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::MrroStationSeekByPTYComplete
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverBase::MrroStationSeekByPTYComplete( TInt /*aError*/, TInt /*aFrequency*/ )
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::MrroStationSeekByTAComplete
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverBase::MrroStationSeekByTAComplete( TInt /*aError*/, TInt /*aFrequency*/ )
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::MrroStationSeekByTPComplete
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverBase::MrroStationSeekByTPComplete( TInt /*aError*/, TInt /*aFrequency*/ )
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::MrroGetFreqByPTYComplete
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverBase::MrroGetFreqByPTYComplete( TInt /*aError*/, RArray<TInt>& /*aFreqList*/ )
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::MrroGetFreqByTAComplete
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverBase::MrroGetFreqByTAComplete( TInt /*aError*/, RArray<TInt>& /*aFreqList*/ )
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::MrroGetPSByPTYComplete
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverBase::MrroGetPSByPTYComplete( TInt /*aError*/, RArray<TRdsPSName>& /*aPsList*/ )
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::MrroGetPSByTAComplete
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverBase::MrroGetPSByTAComplete( TInt /*aError*/, RArray<TRdsPSName>& /*aPsList*/ )
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::MrroRdsDataPI
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverBase::MrroRdsDataPI( TInt /*aPi*/ )
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::MrroRdsDataPTY
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverBase::MrroRdsDataPTY( TRdsProgrammeType /*aPty*/ )
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::MrroRdsDataPS
// Notifies of the New Programme Service data
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverBase::MrroRdsDataPS( TRdsPSName& aPs )
    {
    TBool reportStaticState = EFalse;
    if ( iPsName->Length() && aPs.Length() )
        {
        if ( *iPsName != aPs && iPsNameTimer->IsActive() )
            {
            // PS existed and it changed, cancel the timer as it is not static name anymore
            iPsNameTimer->Cancel();
            iPsNameType = EFMRadioPSNameDynamic;
            reportStaticState = ETrue;
            }
        else
            {
            if ( iPsNameType == EFMRadioPSNameStaticAssumed && !iPsNameTimer->IsActive() )
                {
                // The timer should be reactivated in order to complete the determination
                iPsNameTimer->Start( KFMRadioStaticRdsPsNameDelay, 
                                     0, 
                                     TCallBack( CFMRadioRdsReceiverBase::StaticPsNameTimerCallback, this ) );
                }
            }
        }
    else if ( !iPsName->Length() && aPs.Length() )
        {
        // PS name is set for the first time, start the timer to see if it is a static one
        iPsNameTimer->Cancel();
        iPsNameTimer->Start( KFMRadioStaticRdsPsNameDelay, 
                             0, 
                             TCallBack( CFMRadioRdsReceiverBase::StaticPsNameTimerCallback, this ) );
        }
    else
        {
        // PS name is reset
        }
    
    iPsName->Des().Copy( aPs );
    
    if ( iPubSub )
        {
        TRAP_IGNORE( iPubSub->PublishRDSProgramServiceL( *iPsName ));
        }
 
    for( TInt i = 0 ; i < iObservers.Count() ; i++ )
        {
        iObservers[i]->RdsDataProgrammeService( *iPsName );
        if( reportStaticState )
            {
            iObservers[i]->RdsDataPsNameIsStatic( iPsNameType == EFMRadioPSNameStatic );
            }
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::MrroRdsDataRT
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverBase::MrroRdsDataRT( TRdsRadioText& aRt )
    { 
    iRadioText->Des().Copy( aRt );
    for( TInt i = 0 ; i < iObservers.Count() ; i++ )
        {
        iObservers[i]->RdsDataRadioText( *iRadioText );
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::MrroRdsDataCT
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverBase::MrroRdsDataCT( TDateTime& /*aCt*/ )
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::MrroRdsDataTA
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverBase::MrroRdsDataTA( TBool /*aTaOn*/ )
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::MrroRdsDataRTplus
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverBase::MrroRdsDataRTplus( TRdsRTplusClass aRtPlusClass, TRdsRadioText& aRtPlusData )
    {
    TRAP_IGNORE( HandleRdsDataRTplusL( aRtPlusClass, aRtPlusData ) )
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::MrroRdsSearchBeginAF
// Notifies of the start of Alternate Frequency search
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverBase::MrroRdsSearchBeginAF()
    {
    for( TInt i = 0 ; i < iObservers.Count() ; i++ )
        {
        iObservers[i]->RdsAfSearchBegin();
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::MrroRdsSearchBeginAF
// Notifies of the end of Alternate Frequency search
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverBase::MrroRdsSearchEndAF( TInt aError, TInt aFrequency )
    {
    for( TInt i = 0 ; i < iObservers.Count() ; i++ )
        {
        // Radio utility uses Hz, UI uses kHz. It's ok to divide with 1000, possible lost fractions are useless.
        iObservers[i]->RdsAfSearchEnd( aFrequency / KFMRadioFreqMultiplier, aError );
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::MrroRdsStationChangeTA
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverBase::MrroRdsStationChangeTA( TInt /*aFrequency*/ )
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::MrroRdsEventAutomaticSwitchingChange
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverBase::MrroRdsEventAutomaticSwitchingChange( TBool /*aAuto*/ )
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::MrroRdsEventAutomaticTrafficAnnouncement
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverBase::MrroRdsEventAutomaticTrafficAnnouncement( TBool /*aAuto*/ )
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverBase::MrroRdsEventSignalChange
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverBase::MrroRdsEventSignalChange( TBool aSignal )
    {
    if ( aSignal )
        {
        if ( iPsName->Length() )
            {
            if ( iPsNameType == EFMRadioPSNameStaticAssumed && !iPsNameTimer->IsActive() )
                {
                iPsNameTimer->Cancel();
                iPsNameTimer->Start( KFMRadioStaticRdsPsNameDelay, 
                                     0, 
                                     TCallBack( CFMRadioRdsReceiverBase::StaticPsNameTimerCallback, this ) );
                }
            }
        }
    else
        {
        // PS name type determination must be canceled on bad signal
        iPsNameTimer->Cancel();
        }
    iSignalAvailable = aSignal;
    for( TInt i = 0 ; i < iObservers.Count() ; i++ )
        {
        iObservers[i]->RdsAvailable( iSignalAvailable );
        }
    }
