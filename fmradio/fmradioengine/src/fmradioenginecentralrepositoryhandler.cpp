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
* Description:  Contains implementation of the CCentralRepositoryHandler class.
*
*/


//  INCLUDE FILES
#include <centralrepository.h>
#include <s32mem.h>
#include <coemain.h> 
#include <barsread.h>
#include <ProfileEngineSDKCRKeys.h>
#include <fmradioengine.rsg>

#include "fmradioenginecentralrepositoryhandler.h"
#include "fmradioengineradiosettings.h"
#include "fmradioenginecrkeys.h"
#include "debug.h"


// CONSTANTS
const TInt KRadioMaxStartupCountToTrack = 5;
// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------
// CCentralRepositoryHandler::CCentralRepositoryHandler
// Default class constructor.
// ----------------------------------------------------
//
CCentralRepositoryHandler::CCentralRepositoryHandler(
    TRadioSettings& aRadioSettings)  // a reference to aRadioSettings
    :iRadioSettings(&aRadioSettings)
    {
    FTRACE(FPrint(_L("calling CCentralRepositoryHandler::NewL()")));
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::ConstructL
// Second phase class constructor.
// ----------------------------------------------------
//
void CCentralRepositoryHandler::ConstructL()
    {
    iCentralRepository = CRepository::NewL( KCRUidFMRadioEngine );
    
    RetrieveInitialSettings();                
    InitializeRegionsL();
        
    #ifdef __WINS__
    SetInitialSettings();
    #endif //__WINS__    
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::NewL
// Two-phased class constructor.
// ----------------------------------------------------
//
CCentralRepositoryHandler* CCentralRepositoryHandler::NewL(
    TRadioSettings& aRadioSettings)  // a reference to aRadioSettings
    {
    CCentralRepositoryHandler *self = new(ELeave) CCentralRepositoryHandler(aRadioSettings);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::~CCentralRepositoryHandler
// Destructor of CCentralRepositoryHandler class.
// ----------------------------------------------------
//
CCentralRepositoryHandler::~CCentralRepositoryHandler()
    {
    iRadioSettings = NULL;
    if ( iCentralRepository )
         {
         delete iCentralRepository;
         }
	iRegions.ResetAndDestroy();
	iRegions.Close();                
    }


// ----------------------------------------------------
// CCentralRepositoryHandler::RetrieveInitialSettings
// Retrieves persistent settings from shared data.
// Returns: None
// ----------------------------------------------------
//
void CCentralRepositoryHandler::RetrieveInitialSettings()
    {
    FTRACE(FPrint(_L("calling CCentralRepositoryHandler::RetrieveInitialSettings() Start")));
    TInt tempData;
    
    if ( iCentralRepository->Get( KRadioHeadsetVolume, tempData ) == KErrNone )
          {
          iRadioSettings->SetHeadsetVolume( tempData );
          }
          
    if ( iCentralRepository->Get( KRadioSpeakerVolume, tempData ) == KErrNone ) 
            {
            iRadioSettings->SetSpeakerVolume( tempData );
            }
            
    if ( iCentralRepository->Get( KFmRadioFrequency, tempData ) == KErrNone )
        {
        iRadioSettings->SetFrequency( tempData );
        }
        
    if ( iCentralRepository->Get( KFmRadioChannel, tempData ) == KErrNone )
        {
        iRadioSettings->SetCurrentPreset( tempData );
        }
        
    if ( iCentralRepository->Get( KFmRadioMode, tempData ) == KErrNone)
        {
        iRadioSettings->SetRadioMode( static_cast<CRadioEngine::TRadioMode>( tempData ) );        
        }
        	
	if ( iCentralRepository->Get( KFMCRCurrentRegion, tempData ) != KErrNone )	        
		{
		iCentralRepository->Set( KFMCRCurrentRegion, KErrNotFound );			
		}
			
	if ( iCentralRepository->Get( KRadioCRStartupCount, tempData ) == KErrNone )
		{
		iRadioSettings->SetStartupCount( tempData );
		}
					
	if ( iCentralRepository->Get( KFMCRRdsAfSearchEnabled, tempData ) == KErrNone )
        {
        if ( static_cast<TBool> ( tempData ) )
        	{
        	iRadioSettings->SetRdsAfSearchEnabled();
        	}
        else
        	{
            iRadioSettings->SetRdsAfSearchDisabled();
        	}
        }
	
	if ( iCentralRepository->Get( KFMCRJapanRegionAllowed, tempData ) == KErrNone )
		{				
		iRadioSettings->SetJapanRegionAllowed( static_cast<TBool>( tempData ) );
		}			       	        
    }

#ifdef __WINS__
// ----------------------------------------------------
// CCentralRepositoryHandler::SetInitialSettings
// Set network info for emulator testing purposes
// ----------------------------------------------------
//
void CCentralRepositoryHandler::SetInitialSettings()
	{
	_LIT(KNetworkInfoForEmulator, "358123");
	iCentralRepository->Set( KFMRadioCREmulatorNetworkInfo, KNetworkInfoForEmulator );			
	}
#endif //__WINS__	

// ----------------------------------------------------
// CCentralRepositoryHandler::SaveEngineSettings
// Save all engine related persistent info back to shared data
// Returns: None
// ----------------------------------------------------
//
void CCentralRepositoryHandler::SaveEngineSettings()
    {
    if ( IsFrequencyValid( iRadioSettings->Frequency() / KFMRadioFreqMultiplier ) )
    	{
    	iCentralRepository->Set( KFmRadioFrequency, iRadioSettings->Frequency() );
    	}
    else
    	{
    	TInt freq = MinFrequency() * KFMRadioFreqMultiplier;
    	iCentralRepository->Set( KFmRadioFrequency, freq  );
    	}
            
    iCentralRepository->Set( KFmRadioChannel, iRadioSettings->CurrentPreset() );
    iCentralRepository->Set( KFmRadioMode, iRadioSettings->RadioMode() );
    iCentralRepository->Set( KFMCRRdsAfSearchEnabled, iRadioSettings->IsRdsAfSearchEnabled() );
    }

// ---------------------------------------------------------------------------
// CCentralRepositoryHandler::CountRegions
// Return count of fmradio regions in regions array
// ---------------------------------------------------------------------------
//
TInt CCentralRepositoryHandler::CountRegions() const
    {
    return iRegions.Count();
    }

// ---------------------------------------------------------------------------
// CCentralRepositoryHandler::Region
// Returns fmradio region at given index
// ---------------------------------------------------------------------------
//
CFMRadioRegion& CCentralRepositoryHandler::Region( TInt aIndex ) const
    {    
    if( aIndex == KErrNotFound )
        {
        aIndex = RegionIndexForId( static_cast<TInt>( EFMRadioRegionDefault ) );        
        }

    return *iRegions[aIndex];
    }

// ---------------------------------------------------------------------------
// CCentralRepositoryHandler::RegionId
// Return current region Id.
// ---------------------------------------------------------------------------
//
TInt CCentralRepositoryHandler::RegionId() const
    {
    TInt regionId;
    iCentralRepository->Get( KFMCRCurrentRegion, regionId );
    return regionId;
    }

// ---------------------------------------------------------------------------
// CCentralRepositoryHandler::SetRegionIdL
// Change the the current region Id.
// ---------------------------------------------------------------------------
//
void CCentralRepositoryHandler::SetRegionIdL( TInt aRegion )
    {
    iCentralRepository->Set( KFMCRCurrentRegion, aRegion );
    UpdateCurrentRegionIdx( aRegion );    
    
    iRadioSettings->SetFrequency( Region( iCurrentRegionIdx ).MinFrequency() * KFMRadioFreqMultiplier ); 
	iRadioSettings->SetMinFrequency( MinFrequency() );
	iRadioSettings->SetMaxFrequency( MaxFrequency() );
	iRadioSettings->SetFrequencyStepSize( FrequencyStepSize() );
	iRadioSettings->SetDecimalCount( DecimalCount() );
    }

// ---------------------------------------------------------------------------
// CCentralRepositoryHandler::FrequencyStepSize
// Returns frequency step size used for current region
// ---------------------------------------------------------------------------
//
TUint32 CCentralRepositoryHandler::FrequencyStepSize() const
    {
    return Region( iCurrentRegionIdx ).StepSize();
    }

// ---------------------------------------------------------------------------
// CCentralRepositoryHandler::MaxFrequency
// Returns highest allowed frequency for current region
// ---------------------------------------------------------------------------
//
TUint32 CCentralRepositoryHandler::MaxFrequency() const
    {
    return Region( iCurrentRegionIdx ).MaxFrequency();
    }

// ---------------------------------------------------------------------------
// CCentralRepositoryHandler::MinFrequency
// Returns lowest allowed frequency for current region
// ---------------------------------------------------------------------------
//
TUint32 CCentralRepositoryHandler::MinFrequency() const
    {
    return Region( iCurrentRegionIdx ).MinFrequency();
    }
// ---------------------------------------------------------------------------
// CCentralRepositoryHandler::DecimalCount
// return decimal count for current region Mhz information
// ---------------------------------------------------------------------------
//
TInt CCentralRepositoryHandler::DecimalCount() const
    {
    return Region( iCurrentRegionIdx ).DecimalCount();
    }


// ---------------------------------------------------------------------------
// CCentralRepositoryHandler::InitializeRegionsL
// Initializes the regions based on resources.
// ---------------------------------------------------------------------------
//
void CCentralRepositoryHandler::InitializeRegionsL()
    {
	iRegions.ResetAndDestroy();

	TResourceReader reader;
	
	CCoeEnv* coeEnv = CCoeEnv::Static();
	
	if ( iRadioSettings->IsJapanRegionAllowed() )
		{
		coeEnv->CreateResourceReaderLC( reader, R_RADIO_REGIONS_ALL );	
		}
	else // Japan region is not allowed so read region resources without Japan
		{
		coeEnv->CreateResourceReaderLC( reader, R_RADIO_REGIONS_WITHOUT_JAPAN );					
		}
			
	TInt regionCount = reader.ReadInt16();

	for( TInt i = 0 ; i < regionCount; i++ )
		{
		TInt resId = reader.ReadInt32(); // The next resource ID to read.
        TResourceReader regionReader;
        coeEnv->CreateResourceReaderLC( regionReader, resId );
		User::LeaveIfError( iRegions.Append( CFMRadioRegion::NewL( regionReader ) ) );
		CleanupStack::PopAndDestroy();
		}

	CleanupStack::PopAndDestroy();

    UpdateCurrentRegionIdx( RegionId() );
        	
	iRadioSettings->SetMinFrequency( MinFrequency() * KFMRadioFreqMultiplier );
	iRadioSettings->SetMaxFrequency( MaxFrequency() * KFMRadioFreqMultiplier );
	iRadioSettings->SetFrequencyStepSize( FrequencyStepSize() );
	iRadioSettings->SetDecimalCount( DecimalCount() );
    }

// ---------------------------------------------------------------------------
// CCentralRepositoryHandler::RegionIndexForId
// Converts Region ID to index
// ---------------------------------------------------------------------------
//
TInt CCentralRepositoryHandler::RegionIndexForId( TInt aRegionId ) const
    {
    TInt idx = KErrNotFound;
    for( TInt i = 0 ; i < CountRegions(); i++ )
        {
        if( Region( i ).Id() == static_cast<TFMRadioRegionSetting>( aRegionId ) )
            {
            idx = i;
            /*lint -save -e960 (Note -- Violates MISRA Required Rule 58, non-switch break used)*/
            break;
            /*lint -restore*/
            }
        }
    return idx;
    }

// ---------------------------------------------------------------------------
// CCentralRepositoryHandler::UpdateCurrentRegionIdx
// Updates the current region
// ---------------------------------------------------------------------------
//
void CCentralRepositoryHandler::UpdateCurrentRegionIdx( TInt aRegionId )
    {
    iCurrentRegionIdx = RegionIndexForId( aRegionId );
    }

// ---------------------------------------------------------------------------
// CCentralRepositoryHandler::IsOfflineProfileActiveL
// Checks if offine profile is current active profile
// Returns: true/false 
// ---------------------------------------------------------------------------
//    
TBool CCentralRepositoryHandler::IsOfflineProfileActiveL()
	{
	CRepository* repository = CRepository::NewLC( KCRUidProfileEngine );
	
	TInt profileId(0);
	
	// Get ID of current profile
	User::LeaveIfError( repository->Get( KProEngActiveProfile, profileId ) );
	
	CleanupStack::PopAndDestroy( repository );
	
	// Check value to determine the active profile
	// 5 = Offline profile
	if ( profileId == 5 )
		{
		// Current profile is the offline profile
		return ETrue;
		}
	
	return EFalse;	
	}

// ---------------------------------------------------------------------------
// CCentralRepositoryHandler::IsFrequencyValid
// Checks if frequency is in the current region range
// ---------------------------------------------------------------------------
//
TBool CCentralRepositoryHandler::IsFrequencyValid( TUint32 aFrequency ) const
    {
    TBool ret( EFalse );
    if( !aFrequency )
        {
        aFrequency = iRadioSettings->Frequency();
        }
    if ( aFrequency >= MinFrequency() && aFrequency <= MaxFrequency() )
        {
        ret = ETrue;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CCentralRepositoryHandler::IncreaseStartupCount
// Increases startup time by one and sets the value to the repository
// ---------------------------------------------------------------------------
//    
void CCentralRepositoryHandler::IncreaseStartupCount() const
	{				
	TInt startCount = iRadioSettings->StartupCount();
	// Dont update if maximum tracking count is reached
	if ( startCount < KRadioMaxStartupCountToTrack )
		{
		startCount++;
		iRadioSettings->SetStartupCount( startCount ); 
		iCentralRepository->Set( KRadioCRStartupCount, startCount );	
		}	
	}

// ---------------------------------------------------------------------------
// CCentralRepositoryHandler::SaveHeadsetVolumeSetting
// Save headset volume level to the repository
// ---------------------------------------------------------------------------
//	
void CCentralRepositoryHandler::SaveHeadsetVolumeSetting() const
	{
	FTRACE( FPrint(_L("CCentralRepositoryHandler::SaveHeadsetVolumeSetting()") ) );
	iCentralRepository->Set( KRadioHeadsetVolume, iRadioSettings->HeadsetVolume() );	
	}
	
// ---------------------------------------------------------------------------
// CCentralRepositoryHandler::SaveSpeakerVolumeSetting
// Save speaker volume level to the repository
// ---------------------------------------------------------------------------
//	
void CCentralRepositoryHandler::SaveSpeakerVolumeSetting() const
	{
	FTRACE( FPrint(_L("CCentralRepositoryHandler::SaveSpeakerVolumeSetting()") ) );
	iCentralRepository->Set( KRadioSpeakerVolume, iRadioSettings->SpeakerVolume() );		
	}
	
// ---------------------------------------------------------------------------
// CCentralRepositoryHandler::MusicStoreNoteDisplayed
// ---------------------------------------------------------------------------
//
TBool CCentralRepositoryHandler::MusicStoreNoteDisplayed()
    {
    TBool result;
    TInt err = iCentralRepository->Get( KRadioMusicStoreNoteDisplayed, result );
    if ( err != KErrNone )
    	{
    	result = EFalse;
    	}    
    return result;
    }

// ---------------------------------------------------------------------------
// CCentralRepositoryHandler::SetMusicStoreNoteDisplayed
// ---------------------------------------------------------------------------
//
void CCentralRepositoryHandler::SetMusicStoreNoteDisplayed()
    {
    iCentralRepository->Set( KRadioMusicStoreNoteDisplayed, 1 );
    }

// ---------------------------------------------------------------------------
// CCentralRepositoryHandler::GetRTPlusSupportLevel
// ---------------------------------------------------------------------------
//
TInt CCentralRepositoryHandler::GetRTPlusSupportLevel()
    {
    //See fmradioengine.hrh:TFMRadioRTPlusLevel for correct values
    TInt result;
    TInt err = iCentralRepository->Get( KRadioRTPlusSupportLevel, result );
    if ( err != KErrNone )
    	{
    	result = KErrNotFound;
    	}
    
    return result;
    }

// end of file

