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
* Description:  Defines APIs to communicate with Central Repository
*
*/


#ifndef FMRADIOENGINECENTRALREPOSITORYHANDLER_H
#define FMRADIOENGINECENTRALREPOSITORYHANDLER_H

//  INCLUDES
#include "fmradioregion.h"

// CONSTANTS
const TInt KBufferLength = 20;
const TInt KStreamBufferSize = 100;

// Forward declare implementation class
class TRadioSettings;
class CRepository;

// CLASS DECLARATION

/**
*  it is a wrapper of Central Repository, responsible for the operations that need to access 
*  the central respositorhy.
*
*  @lib FMRadioEngine.lib
*  @since Series 60 3.0_version
*/
class CCentralRepositoryHandler : public CBase
    {
    public:
      	/**
      	* Two-phased class constructor.
      	* @param aRadioSettings  pointer to the CRadioSettings class
      	*/
        static CCentralRepositoryHandler* NewL(TRadioSettings& aRadioSettings);
      	/**
      	* Destructor of CCentralRepositoryHandler class.
      	*/
        virtual ~CCentralRepositoryHandler();
    public:
      	/**
      	* Save all engine related persistent info back to Central Repository
        * @since Series 60 3.0
      	* @return none
      	*/
        void SaveEngineSettings();
        
	    TInt CountRegions() const;
	    CFMRadioRegion& Region( TInt aIndex ) const;
	    TInt RegionId() const;
	    TUint32 FrequencyStepSize() const;
	    TUint32 MaxFrequency() const;
	    TUint32 MinFrequency() const;
	    TInt DecimalCount() const;
	    
	    void SetRegionIdL( TInt aRegion );
	    
	    /**
	     * Determines if RT+ interaction tutorial dialog has been shown.
	     * See CFMRadioEngine::MusicStoreNoteDisplayed().
	     */
	    TBool MusicStoreNoteDisplayed();
	    /**
	     * Sets RT+ interaction tutorial dialog as displayed.
	     * See CFMRadioEngine::SetMusicStoreNoteDisplayed().
	     */
	    void SetMusicStoreNoteDisplayed();
	    /**
	     * Gets supported RT+ interactions. See CFMradioEngine::GetRTPlusSupportLevel().
	     */
	    TInt GetRTPlusSupportLevel();
	    
	    /**
		* Checks if offine profile is current active profile
		* @return true/false 
		*/
		TBool IsOfflineProfileActiveL();
		/**
		* Increases startup time by one and returns amount of program startups.
		* Also updates count to the repository.
		*/
		void IncreaseStartupCount() const;
		/*
		* Save headset volume level to the repository
		*/
		void SaveHeadsetVolumeSetting() const;
		/*
		* Save speaker volume level to the repository
		*/
		void SaveSpeakerVolumeSetting() const;
		    
    private:
      	/**
      	* Default class constructor.
      	* @param aRadioSettings - pointer to the CRadioSettings class
      	*/
        CCentralRepositoryHandler(TRadioSettings& aRadioSettings);
       	/**
     	* Second phase class constructor.
     	*/
        void ConstructL();
     	/**
     	* Retrieves persistent settings from Central Repository.
     	*/
        void RetrieveInitialSettings();
	    /**
	    * Initializes the regions as defined in the resource file.
	    */
	    void InitializeRegionsL();
	    
	    /**
        * Converts Region ID to index
	    *
        * @param aRegionId The region ID that is wanted to convert
	    * @return The region index, KErrNotFound if no match found
	    */
	    TInt RegionIndexForId( TInt aRegionId ) const;

	    /**
	    * Updates the current region index
	    *
	    * @param aRegionId The new region ID
	    */
        void UpdateCurrentRegionIdx( TInt aRegionId );
        /*
        * Set emulator networkinfo
        *
        */
        #ifdef __WINS__
        void SetInitialSettings();
       	#endif //__WINS__
       	/**
	    * Checks if frequency is in the current region range
	    *
	    * @param aFrequency Frequency to check
	    */
       	TBool IsFrequencyValid( TUint32 aFrequency ) const;
       			       	
	private: // data
       	
        // Handle to Central Repository server
        CRepository* iCentralRepository; 
        // Reference to RadioSetting object     
        TRadioSettings* iRadioSettings;
        /** The current region. */
    	TInt iCurrentRegionIdx;    	    	
    	/** The available regions. */     
    	RPointerArray<CFMRadioRegion> iRegions;
    	
    	
        
    };

#endif  // FMRADIOENGINECENTRALREPOSITORYHANDLER_H

// end of file
