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

// User includes
#include "t_cradiordsreceiversimulator.h"
#include "mradioenginesettings.h"

const TInt KVRRdsEventSimulationDelay = 5000000;
const TUint KItemRelatedTagsCount = 2; //songTitles, artistNames
const TUint KRadioTextItemsCount = 3;
const TUint KRtToRtPlusEventTimeout = 400000; // 400 ms
const TUint KSubsequentRtPlusEventsTimeout = 200; // 0.02 ms
                       

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRdsReceiverSimulator::CRadioRdsReceiverSimulator( MRadioEngineSettings& aSettings )
    : CRadioRdsReceiverBase( aSettings )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverSimulator::ConstructL()
    {
    BaseConstructL();
    iEventSimulatorTimer = CPeriodic::NewL( CActive::EPriorityUserInput );
    iEventSimulatorTimer2 = CPeriodic::NewL( CActive::EPriorityUserInput );
    SetAutomaticSwitchingL( iSettings.RdsAfSearchEnabled() );

    StartReceiver();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRdsReceiverSimulator* CRadioRdsReceiverSimulator::NewL( MRadioEngineSettings& aSettings )
    {
    CRadioRdsReceiverSimulator* self = new (ELeave) CRadioRdsReceiverSimulator( aSettings );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRdsReceiverSimulator::~CRadioRdsReceiverSimulator()
    {
    if ( iEventSimulatorTimer )
        {
        iEventSimulatorTimer->Cancel();
	    }      
    if( iEventSimulatorTimer2 )
        {
        iEventSimulatorTimer2->Cancel();
        }
    delete iEventSimulatorTimer;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverSimulator::InitL( CRadioUtility& /*aRadioUtility*/, CRadioPubSub* aPubSub )
    {
    iPubSub = aPubSub;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverSimulator::StartReceiver()
    {
    if ( !iEventSimulatorTimer->IsActive() )
        {
        iEventSimulatorTimer->Start( TTimeIntervalMicroSeconds32( KVRRdsEventSimulationDelay ),
                                     TTimeIntervalMicroSeconds32( KVRRdsEventSimulationDelay ),
                                     TCallBack( StaticRdsSimulationCallback, this ));
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverSimulator::StopReceiver()
    {
    iEventSimulatorTimer->Cancel();
    iEventSimulatorTimer2->Cancel();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CRadioRdsReceiverSimulator::StaticRdsSimulationCallback( TAny* aSelfPtr )
    {
    TRadioRdsSimulationEvent events[] = { ERadioRdsEventSignalOn,
                                       //ERadioRdsEventAFBegin,
                                       ERadioRdsEventPS,
                                       ERadioRdsEventRadioText,
                                       //ERadioRdsEventRadioTextPlus,
                                       ERadioRdsEventPS,
                                       ERadioRdsEventRadioText,
                                       //ERadioRdsEventRadioTextPlus,
                                       ERadioRdsEventPS,
                                       ERadioRdsEventRadioText,
                                       //ERadioRdsEventRadioTextPlus,
                                       ERadioRdsEventPS,
                                       ERadioRdsEventRadioText ,
                                       //ERadioRdsEventPS,
                                       //ERadioRdsEventAFEnd,
                                       ERadioRdsEventSignalOff };

    TRdsPSName serviceNames[] = { _L("Tesla" ), _L("Tesla" ), _L("Tesla" ) };
    TRdsRadioText radioTexts[] = { _L( "Now playing Madonna Material Girl" ),
                                   _L( "Hox! Get Motorhead Ace of Spades from music store now for free!" ),
                                   _L( "Abba station-Waterloo Abba station-Waterloo  Abba station" ) };
    
    TInt frequencies[] = { 103100000, 95800000 }; // Frequencies in hz
    TInt numEvents = sizeof( events ) / sizeof( TRadioRdsSimulationEvent );
    TInt numServiceNames = sizeof( serviceNames ) / sizeof( TRdsPSName );
    TInt numFrequencies = sizeof( frequencies ) / sizeof( TInt );
    // RT ( radioTexts[]) and RT+ ( songTitles[] and artistNames[]) information according to PS names ( serviceNames[]).

    CRadioRdsReceiverSimulator* self = reinterpret_cast<CRadioRdsReceiverSimulator*>( aSelfPtr );
    if ( self )
        {
        switch ( events[self->iEventIterator] )
            {
            case ERadioRdsEventSignalOn:
                {
                self->MrroRdsEventSignalChange( ETrue );
                break;
                }

            case ERadioRdsEventSignalOff:
                {
                self->MrroRdsEventSignalChange( EFalse );
                break;
                }

            case ERadioRdsEventPS:
                {
                // Set RT/RT+ iterator value according to PS name iterator: If ERadioRdsEventRadioText or
                // ERadioRdsEventRadioTextPlus arrives, displays RT or RT+ according to radio station name.
                self->iRadioTextIterator = self->iPsIterator;
                self->MrroRdsDataPS( serviceNames[self->iPsIterator] );
                self->iPsIterator++;
                if ( self->iPsIterator >= numServiceNames )
                    {
                    self->iPsIterator = 0;
                    }
                break;
                }

            case ERadioRdsEventAFBegin:
                {
                if ( self->iSettings.RdsAfSearchEnabled() )
                    {
                    self->MrroRdsSearchBeginAF();
                    }
                break;
                }

            case ERadioRdsEventAFEnd:
                {
                if ( self->iSettings.RdsAfSearchEnabled() )
                    {
                    self->MrroRdsSearchEndAF( KErrNone, frequencies[self->iFrequencyIterator] );
                    }
                self->iFrequencyIterator++;
                if ( self->iFrequencyIterator >= numFrequencies )
                    {
                    self->iFrequencyIterator = 0;
                    }
                break;
                }
            case ERadioRdsEventRadioText:
                {
                // Display RT according to PS name.
                self->MrroRdsDataRT( radioTexts[self->iRadioTextIterator] );
                // how many times StaticRdsSimulationCallback2 entered
                if ( self->iRtPlusIterator >= KItemRelatedTagsCount )
                    {
                    self->iRtPlusIterator = 0;
                    }
                                
                // start timer for displaying the two consequent RT+ events
                self->startRTPlusTimer();
                
                // how many radio text tags received
                if( self->iRtIterator >= KRadioTextItemsCount )
                    {
                    self->iRtIterator = 1;                    
                    }
                else
                    {
                    self->iRtIterator++;
                    }
                break;
                }
            case ERadioRdsEventRadioTextPlus:
                {
                // Display RT+ information according to PS name.
				
                break;
                }
            default:
                break;
            }
        self->iEventIterator++;
        if ( self->iEventIterator >= numEvents )
            {
            self->iEventIterator = 0;
            }
        }
    return KErrNone;
    }

void CRadioRdsReceiverSimulator::startRTPlusTimer()
    {
    
    iEventSimulatorTimer2->Cancel();
    if ( !iEventSimulatorTimer2->IsActive() )
        {        
        iEventSimulatorTimer2->Start( TTimeIntervalMicroSeconds32( KRtToRtPlusEventTimeout ),
            TTimeIntervalMicroSeconds32( KSubsequentRtPlusEventsTimeout ),
            TCallBack( StaticRdsSimulationCallback2, this ));
        }
    }

TInt CRadioRdsReceiverSimulator::StaticRdsSimulationCallback2( TAny* aSelfPtr )
    {    
    TRdsRadioText songTitles[] = { _L( "Material Girl" ),
                                   _L( "Ace of Spades" ),
                                   _L( "Waterloo" ) };
    TRdsRadioText artistNames[] = { _L( "Madonna" ),
                                    _L( "Motorhead" ),
                                    _L( "Abba" ) };

    CRadioRdsReceiverSimulator* self = reinterpret_cast<CRadioRdsReceiverSimulator*>( aSelfPtr );
    
    if ( self )
        {
        if( self->iRtPlusIterator == 0 )
            {
            self->MrroRdsDataRTplus( ERTplusItemTitle, songTitles[self->iRtIterator-1] );
            }
        else if( self->iRtPlusIterator == 1 )
            {
            self->MrroRdsDataRTplus( ERTplusItemArtist, artistNames[self->iRtIterator-1] );
            }
        }
    self->iRtPlusIterator++;
        
    return KErrNone;
    }
