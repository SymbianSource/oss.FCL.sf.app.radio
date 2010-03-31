/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  definition of the class CFMRadioScanLocalStationsView
*
*/


#ifndef FMRADIOSCANLOCALSTATIONSVIEW_H
#define FMRADIOSCANLOCALSTATIONSVIEW_H

// INCLUDES
#include <aknview.h>
#include <AknProgressDialog.h>
#include <akntoolbarobserver.h>
 
#include "fmradioscanlocalstationscontainer.h"
#include "mfmradiolayoutchangeobserver.h"
#include "fmradiordsobserver.h"
#include "mchannellisthandler.h"
#include "fmradiosvkevents.h"

// FORWARD DECLARATIONS
class CFMRadioScanLocalStationsContainer;
class CRadioEngine;
class CAknWaitDialog;
class CFMRadioPreset;


// CLASS DECLARATION  

/**
* CFMRadioScanLocalStationsView view class.
* @since 2.6
*/
class CFMRadioScanLocalStationsView : public CAknView,
                                	  public MCoeControlObserver,
                                	  public MFMRadioLayoutChangeObserver,
                                	  public MProgressDialogCallback,
                                      public MFMRadioRdsObserver,
                                      public MAknToolbarObserver
    {
    public: // Constructors and destructor
        /**
        * Two-phase constructor of CFMRadioScanLocalStationsView
        * @since 2.6
        * @param aRadioEngine pointer to a radio engine object
        * @param aObserver channel list observer
        */
        static CFMRadioScanLocalStationsView* NewL( CRadioEngine& aRadioEngine,
        		MChannelListHandler& aObserver );
        /**
        * Destructor.
        * @since 2.6
        */
        virtual ~CFMRadioScanLocalStationsView();
    public:  // New functions
        /**
        * Return the unique identifier corresponding to this view
        * @since 2.6
        * @return the unique identifier corresponding to this view
        */
        TUid Id() const;
        /**
        * Keep track of the last selected channel
        * @since 2.6
        * @param aIndex the index of the last listened to channel
        */
        void SetLastListenedChannel( TInt aIndex );
        /**
        * Keep track of the current selected channel
        * @param aChannelIndex the index of the current channel
        */
        void SetNowPlayingChannelL ( TInt aChannelIndex );
         /**
        * Returns the index of the playing channel item from the 
        * channel list. 
        * @return the currently playing channel
        */
        TInt CurrentlyPlayingChannel();
        /**
        * Returns the index of the selected channel item from the 
        * channel list. If the container is active get the info from 
        * it, because it may have more up-to-date info.
        * @since 2.6
        * @return the currently selected channel
        */
        TInt CurrentlySelectedChannel();
        /**
        * Make a request to the channel container to update the 
        * content of the channel at aIndex with the values specified
        * @since 2.6
        * @param aIndex the index of the channel to update
        * @param aChannelFrequency the new frequency
        */
        void UpdateChannelListContentL( TInt aIndex, TInt aChannelFrequency );
        /**
        * Return true/false to indicate whether the channel specified is in use
        * @since 2.6
        * @param aIndex the index of the channel to check
        * @return true/false to indicate whether the channel is currently being used
        */
        TBool ChannelInUse( const TInt aIndex ) const;
        /**
        * Return true/false to indicate whether the channel specified is in play
        * @param aChannelIndex the index of the channel to check
        * @return true/false to indicate whether the channel is currently being played
        */
        TBool ChannelInPlay( TInt aChannelIndex ) const; 
        /**
        * Retrieve the index of the next channel that has been set.
        * @since 2.6
        * @return the index of the next "in use" channel
        */
        TInt NextChannel();
        /**
        * Retrieve the index of the previous channel that has been set.
        * @since 2.6
        * @return the index of the previous "in use" channel
        */
        TInt PreviousChannel();
        /**
        * Fade the view and all controls associated with it. Change
        * the soft keys so that "Exit" is displayed
        * @since 2.6
        * @param aFaded true/false to indicate whether the view should fade/unfade.
        */
        void FadeAndShowExit( TBool aFaded );
        /**
        * Update display when focus regained
        * @since 3.0
        */
        void UpdateDisplayForFocusGained();
        /**
        * Display a wait note while the channel fill is occuring.
        * @since 2.6        *  
        */
        void DisplayScanningInProgressNoteL();
        /**
        * Remove and destroy the scanning note.
        * @since 2.6
        */
        void RemoveScanningInProgressNoteL();
        /**
        * from MFMRadioLayoutChangeObserver
		* This method gets called by the AppUI when a dynamic layout change
		* event occurs.
		*/
		void LayoutChangedL( TInt aType );

		/**
		 * Invoked by AppUi when a new radio station is found.
		 */
		void StopSeekL();
		/**
         * Restores the radio after scanning
         */
        void RestoreRadio( TBool aTuneInitialFrequency = ETrue, TBool aUnmute = ETrue );
		/**
		 * Called when a new scan is activated.
		 */
		void ResetListAndStartScanL();
		/**
		 * Saves all the scanned channels into Radio Engine's presets.
		 */
		void SaveAllChannelsL();
		/**
		* Sets toolbar visibility
		* @param aVisible 
		* @return none
		*/
		void ShowToolbar( TBool aVisible );
		
		 // from base class MFMRadioRdsObserver
	    void RdsDataProgrammeService( const TDesC& aProgramService );
	    void RdsDataPsNameIsStatic( TBool aStatic );
	    void RdsDataRadioText( const TDesC& aRadioText );
        void RdsDataRadioTextPlus( const TInt /*aRTPlusClass*/, const TDesC& /*aRadioText*/ ){}; 	    
	    void RdsAfSearchBegin();
	    void RdsAfSearchEnd( TUint32 aFrequency, TInt aError );
	    void RdsAfSearchStateChange( TBool aEnabled );
	    void RdsAvailable( TBool aAvailable );
	    
	    //for canceling the wizard
	    void SetScanCanceled( TFMRadioCancelScanType aType );
	    TFMRadioCancelScanType CancelType();
        /**
         * Dim or undim save and saveall buttons.
         */ 
        void UpdateToolbar();
        
        /*
         * Handle view specific remote control events
         * @param aEvent accessory event
         */
        void HandleScanListRemConEventL( TAccessoryEvent aEvent );

    private: // Functions from base classes
        /**
        * Activate the channel list view
        * @since 2.6
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,TUid aCustomMessageId, const TDesC8& aCustomMessage );
        /**
        * Deactivate the channel list view
        * @since 2.6
        */
        void DoDeactivate();
        /**
        * Interprets view's menu,softkey and other commands and acts 
        * accordingly by calling the appropriate command handler 
        * function for further action.
        * @since 2.6
        * @param aCommand the command to process
        */
        void HandleCommandL( TInt aCommand );
        /**
        * From MCoeControlObserver, control event observing. In this case,
        * the user selection from the list is reported by container class
        * and observed here.
        * @since 2.6
        * @param aControl the control that caused the event
        * @param aEventType the type of event that was triggered
        */
        void HandleControlEventL( CCoeControl* aControl,TCoeEvent aEventType );
        /**
        * Sets the state of menu items dynamically according to the 
        * state of application data.
        * @since 2.6
        * @param aResourceId Resource ID identifying the menu pane to initialise.
        * @param aMenuPane The in-memory representation of the menu pane.
        */
        void DynInitMenuPaneL( TInt aResourceId,CEikMenuPane* aMenuPane );
        
        /**
        * SetMiddleSoftKeyLabelL, sets the MSK label
        * from resource
        */
        void SetMiddleSoftKeyLabelL( const TInt aResourceId, const TInt aCommandId );        
        /**
        * SetMiddleSoftKeyIconL, sets the MSK icon
        * 
        */
        void SetMiddleSoftKeyIconL();        
        /**
        * Called when user cancels waiting note or process finishes
        */       
        void DialogDismissedL( TInt  aButtonId );
        /**
        * from MAknToolbarObserver
        * Handle commands from toolbar.
        * @param aCommandId The command id
        */        
        void OfferToolbarEventL( TInt aCommandId );
        /**
        * SetContextMenu, to set MSK context
        */
	    void SetContextMenu( TInt aMenuTitleResourceId );
    private: // new functions

		/** modes for channel saving */
		enum TSavingMode
			{
			EAppend = 0x1,
			EReplace
			};	
	    /**
		* C++ default constructor
		*/
	 	CFMRadioScanLocalStationsView( CRadioEngine& aRadioEngine, MChannelListHandler& aObserver );
	    /**
	    * EPOC default constructor.
	    * @since 2.6
	    */
	    void ConstructL();
		/**
		* Save scanned channels to presets
		*/        
		void SaveFoundChannelsToPresetsL( TSavingMode aSavingMode );         
		/**
		* Add found channel to the frequency list
		*/           
		void AddTunedFrequencyToListL();
		/**
		* Tune to the selected channel
		*/        
		void PlayCurrentlySelectedChannelL();
		
		/**
		 * Show aResQuery confimation query with aResText.
		 * aResText and aResQuery must be real resource ids.  
		 * Returns users response, 0 if user denied.
		 */
		TInt ShowConfirmationQueryL(TInt aResText, TInt aResQuery);		
		/**
        * Prepare toolbar for FMRadio command buttons
        */
		void PrepareToolbar();
		        /**
        * Set the current dimmed state of the toolbar 
        * 
        * @param aState the new toolbar dimmed state
        */
        void SetToolbarDimmedState( const TBool aState );        
        /**
         * check if the frequency is already found
         * @param aFrequency frequency to check
		 * @return list index or KErrNotFound if the frequency is not in the list
         */
        TInt FrequencyIndex( TInt aFrequency );
        /**
         * Handle single channel save
         */
        void HandleOneChannelSaveL();
        /*
         * Activates main view. Doesn't bring radio to
         * foreground if the app is in the background
         */
        void ActivateMainViewL();
        
        /*
         * Resolves index number for the given frequency.
         * Frequencies are sorted from smallest to largest.
         * @param aFrequency frequency to add
         */
        TInt SortedFrequencyListIndex( TInt aFrequency );
    	
    private: // Data
    	CFMRadioScanLocalStationsContainer* iContainer;
        TInt iChIndex;
        TInt iNowPlayingIndex;
        TBool iScanAndSaveActivated;
        TInt iCurrentMSKCommandId; //owned
		TBool iFaded;
		TBool iScanCancelled;
        CRadioEngine& iRadioEngine; 
		TInt iLowerFrequencyBound;
		TInt iUpperFrequencyBound;
		
		TBool iScanningNoteIsVisible;
		// array for the scanned channels
		RPointerArray<CFMRadioPreset> iScannedChannels; 
		
		TInt iInitialTunedFrequency;
		// A wait used when channel fill in progress
        CAknWaitDialog* iScanningNote;
        TBool iTuneRequested;
       	/**
		* Toolbar for controlling the application.	
		*/
		CAknToolbar* iToolbar;
        /**
        * channel list handler observer
        */
        MChannelListHandler& iObserver; 
        TFMRadioCancelScanType iCancelType;
    };

#endif

// End of File
