/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Observer for system events
*
*/

#include "fmradiovariant.hrh" // include first for variation
#include <e32def.h> // Define before audiopolicy defs
#include <e32std.h> // Define before audiopolicy defs
#include <eikdef.h>
#include <sacls.h>
#include <voiceuidomainpskeys.h>
#include <ctsydomainpskeys.h>
#ifndef __WINS__

#ifdef __FMRADIO_ADVANCED_AUTO_RESUME

const TInt KFMRadioAudioCategoryArrayGranularity = 3;
#include <internal/audiopolicypubsubdata.h>
#include <internal/audiosw_pubsubkeys.h>

#else

#include <AudioClientsListPSData.h>
#include <AudioClientsListPSKeys.h>

#endif // __FMRADIO_ADVANCED_AUTO_RESUME

#endif //__WINS__

#include "debug.h"
#include "fmradiosystemeventdetector.h"
#include "fmradiosystemeventdetectorobserver.h"

const TInt KFMRadioAutoResumeDelay = 2000000; // micro seconds -> 2 seconds

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CFMRadioSystemEventDetector::CFMRadioSystemEventDetector
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CFMRadioSystemEventDetector::CFMRadioSystemEventDetector(MFMRadioSystemEventDetectorObserver&  aObserver) 
    :   iObserver( aObserver ),
        iIsNetworkCoverage( EFalse),
        iIsVoiceUiActive( EFalse )
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioSystemEventDetector::ConstructL
// EPOC default constructor can leave.
// ---------------------------------------------------------------------------
//
void CFMRadioSystemEventDetector::ConstructL()
    {
    CFMRadioPropertyObserver* networkObserver =
        CFMRadioPropertyObserver::NewLC( *this,
                                        KUidSystemCategory,
                                        KUidNetworkStatusValue,
                                        CFMRadioPropertyObserver::EFMRadioPropertyInt );
        
    iPropertyArray.AppendL( networkObserver );
    CleanupStack::Pop( networkObserver );
    
    CFMRadioPropertyObserver* voiceUiObserver =	
        CFMRadioPropertyObserver::NewLC( *this, 
                                        KPSUidVoiceUiAccMonitor, 
                                        KVoiceUiOpenKey, 
                                        CFMRadioPropertyObserver::EFMRadioPropertyInt );
                                                
    iPropertyArray.AppendL( voiceUiObserver );
    CleanupStack::Pop( voiceUiObserver );
    // Initialize call state observer.
    iCallStatusObserver = CFMRadioPropertyObserver::NewL( *this, 
                                                      	  KPSUidCtsyCallInformation, 
                                                      	  KCTsyCallState, 
                                                      	  CFMRadioPropertyObserver::EFMRadioPropertyInt);
										    
    iCallStatusObserver->ActivateL();
    iIsCallActive = iCallStatusObserver->ValueInt() != EPSCTsyCallStateNone;

    iAutoResumeTimer = CPeriodic::NewL( CActive::EPriorityStandard );

#ifndef __WINS__
#ifdef __FMRADIO_ADVANCED_AUTO_RESUME
    // Define audio types for not resuming.
    // Audio categories are currently defined by adaptation of each hw platform.
    // Nokia adaptation uses following definitions, which must be replaced by corresponding
    // adaptation specific definitions of licensee adaptation.
    // No general Symbian Foundation definitions for categories exists yet.
    iNoAutoResumeAudioCategories = RArray<TInt>( KFMRadioAudioCategoryArrayGranularity );
    iNoAutoResumeAudioCategories.AppendL( ECatMediaPlayer );
    iNoAutoResumeAudioCategories.AppendL( ECatMobileTv );
    iNoAutoResumeAudioCategories.AppendL( ECatUnknownPlayer );
    iNoAutoResumeAudioCategories.Compress();
#endif // __FMRADIO_ADVANCED_AUTO_RESUME
    // Start listening audio client events.
    CFMRadioPropertyObserver* audioPolicyObserver =
        CFMRadioPropertyObserver::NewLC( *this,
                                        KPSUidMMFAudioServer,
                                        KAudioPolicyAudioClients,
                                        CFMRadioPropertyObserver::EFMRadioPropertyByteArray );
                         
     iPropertyArray.AppendL( audioPolicyObserver );
     CleanupStack::Pop( audioPolicyObserver );
#endif	
    for ( TInt i = 0; i < iPropertyArray.Count(); i++ )
        {
        iPropertyArray[i]->ActivateL();	
        }
    
    TInt networkAvailability = iPropertyArray[EFMRadioNetworkCoverageProperty]->ValueInt();
            
    if (networkAvailability == ESANetworkAvailable)
        {
        iIsNetworkCoverage = ETrue;
        }
    else
        {
        iIsNetworkCoverage = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioSystemEventDetector::~CFMRadioSystemEventDetector
// Destructor
// ---------------------------------------------------------------------------
//
CFMRadioSystemEventDetector::~CFMRadioSystemEventDetector()
    {
    delete iCallStatusObserver;
    iPropertyArray.ResetAndDestroy();
    iPropertyArray.Close();
    iNoAutoResumeAudioCategories.Close();
    delete iAutoResumeTimer;  
    }
    
// ---------------------------------------------------------------------------
// CFMRadioSystemEventDetector::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFMRadioSystemEventDetector* CFMRadioSystemEventDetector::NewL(
                                    MFMRadioSystemEventDetectorObserver&  aObserver)
    {
    CFMRadioSystemEventDetector* self = new (ELeave) CFMRadioSystemEventDetector(aObserver);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
	
    return self;
    }
    
// ---------------------------------------------------------------------------
// CFMRadioSystemEventDetector::HandlePropertyChangeL
// Handling of the int property changes is done here. 
// Observer components are getting notifications in correspondence with what 
// has changed
// ---------------------------------------------------------------------------
//
void CFMRadioSystemEventDetector::HandlePropertyChangeL( const TUid& aCategory, 
                                                    const TUint aKey, 
                                                    const TInt aValue )
	{
    if (aCategory == KUidSystemCategory && aKey == KUidNetworkStatusValue)
    	{
    	switch (aValue)
    		{
    		case ESANetworkAvailable:
    			{
				if (!iIsNetworkCoverage)
				    {
				    FTRACE( FPrint( _L("CFMRadioSystemEventDetector - iObserver NetworkUpCallbackL") ) );
				    iIsNetworkCoverage = ETrue;
				    iObserver.NetworkUpCallbackL();
				    }
    			break;
    			}
    		case ESANetworkUnAvailable:
    			{
    			if (iIsNetworkCoverage)
    			    {
    			    FTRACE( FPrint( _L("CFMRadioSystemEventDetector - iObserver NetworkDownCallbackL") ) );
    			    iIsNetworkCoverage = EFalse;
    			    iObserver.NetworkDownCallbackL();
    			    }
    			break;
    			}
    		default:
    			{
    			break;   			
    			}
    		}
    	}
	else if ( aCategory == KPSUidCtsyCallInformation && aKey == KCTsyCallState )
    	{
		if ( ( !iIsCallActive ) && ( aValue > EPSCTsyCallStateNone ) ) 
			{
			FTRACE( FPrint( _L("CFMRadioSystemEventDetector - iObserver iIsCallActive = ETrue;") ) );
			iIsCallActive = ETrue;
			iObserver.CallActivatedCallbackL();
			}
		else if ( ( iIsCallActive ) && ( aValue <= EPSCTsyCallStateNone ) )
			{
			FTRACE( FPrint( _L("CFMRadioSystemEventDetector - iObserver iIsCallActive = EFalse;") ) );
			iIsCallActive = EFalse;
			iObserver.CallDeactivatedCallbackL();	
			}
		else
		    {
		    // No change
		    }
    	}
	else if ( aCategory == KPSUidVoiceUiAccMonitor && aKey == KVoiceUiOpenKey )
        {
        switch (aValue)
    		{
    		case KVoiceUiIsClose:
    			{
				if (iIsVoiceUiActive)
				    {
				    iIsVoiceUiActive = EFalse;
                    FTRACE( FPrint( _L("CFMRadioSystemEventDetector - iObserver KVoiceUiIsClose") ) );
                    if ( iNumberOfActiveAudioClients == 0 )
                        {
                        NotifyAudioResourcesAvailability();
                        }
				    }
    			break;
    			}
    		case KVoiceUiIsOpen:
    			{
    			FTRACE( FPrint( _L("CFMRadioSystemEventDetector - iObserver KVoiceUiIsOpen") ) );
    			if (!iIsVoiceUiActive)
    			    {
    			    iIsVoiceUiActive = ETrue;
    			    
    			    }
    			break;
    			}
    		default:
    			{
    			break;   			
    			}
    		}
        }
    else // NOP
    	{
    	}    	
	    	
	}

// ---------------------------------------------------------------------------
// CFMRadioSystemEventDetector::HandlePropertyChangeErrorL
// This is a callback function which is called when a P&S components returns 
// an error
// ---------------------------------------------------------------------------
//
void CFMRadioSystemEventDetector::HandlePropertyChangeErrorL( const TUid& /*aCategory*/, 
                                                         	  const TUint /*aKey*/, 
                                                         	  TInt /*aError*/ )
	{
	}

#ifndef __WINS__
// ---------------------------------------------------------------------------
// Handling of the byte array property changes is done here. 
// Observer components are getting notifications in correspondence with what 
// has changed
// ---------------------------------------------------------------------------
//
void CFMRadioSystemEventDetector::HandlePropertyChangeL( const TUid& aCategory, 
                                                    const TUint aKey, 
                                                    const TDesC8& aValue)
	{
    if ( aCategory == KPSUidMMFAudioServer )
        {        
        if ( aKey == KAudioPolicyAudioClients )
            {
            TAudioClientList audioClients;
            audioClients.Copy( aValue );
            
            iNumberOfActiveAudioClients = audioClients().iNumOfProcesses;                                                           
            FTRACE(FPrint(_L("CFMRadioSystemEventDetector::HandlePropertyChangeL() number of audio clients: = %d"), iNumberOfActiveAudioClients));
            
            TBool autoResumePossible = ETrue;
            
            if ( iNumberOfActiveAudioClients > 0 )
                {
                // cancel any previously started audio resume call
                iAutoResumeTimer->Cancel();
                }
            
            // Check all playing audios!            
            for ( TInt i = 0; i < iNumberOfActiveAudioClients; i++ )
                {
                TInt cat = audioClients().iClientCategoryList[i];                                                            
                FTRACE( FPrint( _L("CFMRadioSystemEventDetector - audio category %d"), cat ) );

                if ( iNoAutoResumeAudioCategories.Find(cat) != KErrNotFound )
                    {
                    FTRACE( FPrint( _L("CFMRadioSystemEventDetector - Radio audio will not be resumed") ) );
                    autoResumePossible = EFalse;
                    }
                else
                    {
                    FTRACE( FPrint( _L("CFMRadioSystemEventDetector - radio audio might be resumed") ) );                                      
                    }
                }
            
                // Decide audio resource availability from audio category info.
                if ( !autoResumePossible )
            		{
            		// Auto resume forbidden
            		FTRACE( FPrint( _L("CFMRadioSystemEventDetector::HandlePropertyChangeL - AudioAutoResumeForbiddenL ") ) );
            		iObserver.AudioAutoResumeForbiddenL();
            		}
            	else if ( iNumberOfActiveAudioClients == 0 )
            		{
            		if ( !iIsVoiceUiActive )
            		    {            		    
            		    FTRACE( FPrint( _L("CFMRadioSystemEventDetector::HandlePropertyChangeL - NotifyAudioResourcesAvailabilityL") ) );
            		    NotifyAudioResourcesAvailability();
            		    }
            		}
				else
					{
					// NOP. Some auto resume neutral audio is playing
					}
            }
        }
 	}


#endif //__WINS__

#ifdef __WINS__
// ---------------------------------------------------------------------------
// Dummy version for WINS in order to avoid compiler warnings. 
// The real implementation of function is above.
// ---------------------------------------------------------------------------
//
void CFMRadioSystemEventDetector::HandlePropertyChangeL( const TUid& /*aCategory*/, 
                                                    const TUint /*aKey*/, 
                                                    const TDesC8& /*aValue*/)
	{
 	}
#endif // __WINS__

// Handling of the text property changes is done here. 
// Observer components are getting notifications in correspondence with what 
// has changed
// ---------------------------------------------------------------------------
//
void CFMRadioSystemEventDetector::HandlePropertyChangeL( const TUid& /*aCategory*/, 
                                                    const TUint /*aKey*/, 
                                                    const TDesC& /*aValue*/)
	{
	}
	
// ---------------------------------------------------------------------------
// CFMRadioSystemEventDetector::IsNetworkCoverage
// ---------------------------------------------------------------------------
//
TBool CFMRadioSystemEventDetector::IsNetworkCoverage() const
	{
#ifdef __WINS__
    return ETrue;
#else
	return iIsNetworkCoverage;
#endif
	}

// ---------------------------------------------------------------------------
// CFMRadioSystemEventDetector::IsCallActive
// ---------------------------------------------------------------------------
//
TBool CFMRadioSystemEventDetector::IsCallActive() const
	{
	return iIsCallActive;
	}

// ---------------------------------------------------------------------------
// CFMRadioSystemEventDetector::NotifyAudioResourcesAvailability
// ---------------------------------------------------------------------------
//
void CFMRadioSystemEventDetector::NotifyAudioResourcesAvailability()
    {
    FTRACE( FPrint( _L("CFMRadioSystemEventDetector::NotifyAudioResourcesAvailability") ) );
    // call observer interface after a delay
    iAutoResumeTimer->Cancel();
    iAutoResumeTimer->Start( TTimeIntervalMicroSeconds32( KFMRadioAutoResumeDelay ),
                             TTimeIntervalMicroSeconds32( 0 ),
                             TCallBack( StaticAutoResumeTimerCallback, this ) );    
    }
    
// ---------------------------------------------------------------------------
// CFMRadioSystemEventDetector::StaticAutoResumeTimerCallback
// ---------------------------------------------------------------------------
//
TInt CFMRadioSystemEventDetector::StaticAutoResumeTimerCallback( TAny* aSelfPtr )
    {
    FTRACE( FPrint( _L("CFMRadioSystemEventDetector::StaticAutoResumeTimerCallback") ) );
    CFMRadioSystemEventDetector* self = static_cast<CFMRadioSystemEventDetector*>( aSelfPtr );
    if ( self )
        {
        self->iAutoResumeTimer->Cancel();
        TRAP_IGNORE( self->iObserver.AudioResourcesAvailableL() )
        }
    return KErrNone;
    }

// end of file
