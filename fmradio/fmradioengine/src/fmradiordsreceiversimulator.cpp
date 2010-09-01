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


#include "fmradiordsreceiversimulator.h"

const TInt KFMRadioRdsEventSimulationDelay = 3000000;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverSimulator::CFMRadioRdsReceiverSimulator
// ---------------------------------------------------------------------------
//
CFMRadioRdsReceiverSimulator::CFMRadioRdsReceiverSimulator( TRadioSettings& aSettings ) : CFMRadioRdsReceiverBase(aSettings)
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverSimulator::ConstructL
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverSimulator::ConstructL()
    {    
    BaseConstructL();
    iEventSimulatorTimer = CPeriodic::NewL(CActive::EPriorityUserInput);
    SetAutomaticSwitchingL( iSettings.IsRdsAfSearchEnabled() );
    StartReceiver();
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverSimulator::NewL
// ---------------------------------------------------------------------------
//
CFMRadioRdsReceiverSimulator* CFMRadioRdsReceiverSimulator::NewL( TRadioSettings& aSettings )
    {
    CFMRadioRdsReceiverSimulator* self = new ( ELeave ) CFMRadioRdsReceiverSimulator( aSettings );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverSimulator::~CFMRadioRdsReceiverSimulator
// ---------------------------------------------------------------------------
//
CFMRadioRdsReceiverSimulator::~CFMRadioRdsReceiverSimulator()
    {    
    if( iEventSimulatorTimer && iEventSimulatorTimer->IsActive() )
        {
        iEventSimulatorTimer->Cancel();
        }
        
    delete iEventSimulatorTimer;
	iEventSimulatorTimer = NULL;
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverSimulator::InitL
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverSimulator::InitL( CRadioUtility& /*aRadioUtility*/, CFMRadioPubSub* aPubSub )
    {
    iPubSub = aPubSub;
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverSimulator::StartReceiver
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverSimulator::StartReceiver()
    {
    if( !iEventSimulatorTimer->IsActive() )
        {
    	iEventSimulatorTimer->Start( TTimeIntervalMicroSeconds32(KFMRadioRdsEventSimulationDelay),
    					             TTimeIntervalMicroSeconds32(KFMRadioRdsEventSimulationDelay), 
    					             TCallBack(StaticRdsSimulationCallback, this));
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverSimulator::StopReceiver
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverSimulator::StopReceiver()
    {
    iEventSimulatorTimer->Cancel();
    }

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverSimulator::SetRadioEngineForRadioFmTunerSimulation
// ---------------------------------------------------------------------------
//
void CFMRadioRdsReceiverSimulator::SetRadioEngineForRadioFmTunerSimulation( CRadioEngine* aEngine )
	{ 
	iEngine = aEngine;
	}

// ---------------------------------------------------------------------------
// CFMRadioRdsReceiverSimulator::StaticRdsSimulationCallback
// ---------------------------------------------------------------------------
//
TInt CFMRadioRdsReceiverSimulator::StaticRdsSimulationCallback( TAny* aSelfPtr )
	{
	TFMRadioRdsSimulationEvent events[] = { EFMRadioRdsEventRadioTextPlus,
	                                        EFMRadioRdsEventRadioTextPlus,
	                                        EFMRadioRdsEventRadioTextPlus,
	                                        EFMRadioRdsEventRadioTextPlus,
	                                        EFMRadioRdsEventSignalOn,
									//   EFMRadioRdsEventAFBegin,
							        //   EFMRadioRdsEventAFEnd,
									   EFMRadioRdsEventPS,
									   EFMRadioRdsEventRadioText,
                                       //EFMRadioRdsEventSignalOff,
                                       //EFMRadioRdsEventSignalOn,
                                       EFMRadioRdsEventPS,
                                       EFMRadioRdsEventPS,
                                 //      EFMRadioRdsEventRadioText,
                                 //      EFMRadioRdsEventRadioText
                                       EFMRadioRdsEventAFBegin,
                                       EFMRadioRdsEventPS,
                                       EFMRadioRdsEventAFEnd,
                                       EFMRadioRdsEventSignalOff,
                                       EFMRadioRdsEventRadioTextPlus};

    TRdsPSName serviceNames[] = 
        { 
        _L("RadioXYZ"), 
        _L("Some"), 
        _L("program"), 
        _L("service"), 
        _L("text") 
        };
    
    TRdsRadioText radioTexts[] = 
        { 
        _L("radio text"),
		_L("radio text radio text radio text"),
		_L("full radio text full radio text full radio text full radio textt") 
		};
    
    TRdsRadioText radioTextsPlus[] = 
        { 
        _L("The rock song"),
        _L("Album of the year"),
        _L("Rock 'n' Rollers"),
        _L("http://www.symbianfoundation.org/"),
        _L("Disco Dancers"),
        _L("The disco song"),
        _L("The Greatest"),
        _L("http://developer.symbian.org/"),
        };
    
    TRdsRTplusClass radioTextsPlusClasses[] = 
        { 
        ERTplusItemTitle,
        ERTplusItemAlbum,
        ERTplusItemArtist,
        ERTplusProgramHomepage,
        ERTplusItemArtist,
        ERTplusItemTitle,
        ERTplusItemAlbum,
        ERTplusProgramHomepage,
        };
    
    TInt frequencies[] = { 103100000, 95800000 }; // Frequencies in hz
    
    TInt numEvents = sizeof( events ) / sizeof( TFMRadioRdsSimulationEvent );
    TInt numServiceNames = sizeof( serviceNames ) / sizeof( TRdsPSName );
    TInt numRadioTexts = sizeof( radioTexts ) / sizeof( TRdsRadioText );
    TInt numRadioTextsPlus = sizeof( radioTextsPlus ) / sizeof( TRdsRadioText );
    TInt numFrequencies = sizeof( frequencies ) / sizeof( TInt );
    
	CFMRadioRdsReceiverSimulator* self = static_cast<CFMRadioRdsReceiverSimulator*>(aSelfPtr);
	if( self )
		{
        switch( events[self->iEventIterator] )
            {
            case EFMRadioRdsEventSignalOn:
                {
                self->MrroRdsEventSignalChange( ETrue );
                break;
                }
                
            case EFMRadioRdsEventSignalOff:
                {
                self->MrroRdsEventSignalChange( EFalse );
                break;
                }
                
            case EFMRadioRdsEventPS:
                {
                self->MrroRdsDataPS( serviceNames[self->iPsIterator] );
                self->iPsIterator++;
                if( self->iPsIterator >= numServiceNames )
                    {
                    self->iPsIterator = 0;
                    }
                break;
                }
            case EFMRadioRdsEventRadioText:
                {
                self->MrroRdsDataRT( radioTexts[self->iRadioTextIterator] );
                self->iRadioTextIterator++;
                if( self->iRadioTextIterator >= numRadioTexts )
                    {
                    self->iRadioTextIterator = 0;
                    }
                break;
                }   
            case EFMRadioRdsEventAFBegin:
                {
                if ( self->AutomaticSwitchingEnabled() )
                	{
                	 self->MrroRdsSearchBeginAF();
                	}
                break;
                }
                
            case EFMRadioRdsEventAFEnd:
                {
                if ( self->AutomaticSwitchingEnabled() )
                	{ 
                     // Simulate MrftoFrequencyChange method from MRadioFmTunerObserver
                     if( self->iEngine )
                     	{
                     	self->iEngine->MrftoFrequencyChange( frequencies[self->iFrequencyIterator] );
                     	}
                     
                     self->MrroRdsSearchEndAF( KErrNone, frequencies[self->iFrequencyIterator] );
                     
                     self->iFrequencyIterator++;
                     if( self->iFrequencyIterator >= numFrequencies )
                         {
                         self->iFrequencyIterator = 0;
                         }
                	}
                break;
                }
            case EFMRadioRdsEventRadioTextPlus:
                {
                self->MrroRdsDataRTplus( radioTextsPlusClasses[self->iRadioTextPlusIterator], 
                                         radioTextsPlus[self->iRadioTextPlusIterator]);
                self->iRadioTextPlusIterator++;
                if( self->iRadioTextPlusIterator >= numRadioTextsPlus )
                    {
                    self->iRadioTextPlusIterator = 0;
                    }
                break;
                }                 
            default:
                break;
            }
        self->iEventIterator++;
        if( self->iEventIterator >= numEvents )
            {
            self->iEventIterator = 0;
            }
		}
	return KErrNone;
	}
