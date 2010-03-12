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
* Description:  Definition of class FMRadioAppUi. Provides command
*                      and callback interfaces to control the engine and the
*                     application views.
*
*/


#ifndef FMRADIOAPPUI_H
#define FMRADIOAPPUI_H

// INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknViewAppUi.h>
#include <coecntrl.h>
#include <aknnotedialog.h>
#include <aknnotewrappers.h>
#include <iaupdateobserver.h>

#include "fmradioenginestatechangecallback.h"
#include "fmradioengine.h"
#include "fmradiodefines.h"
#include "fmradiosvkevents.h"
#include "fmradioglobalconfirmationquery.h"
#include "mchannellisthandler.h"


// FORWARD DECLARATIONS
class CAknQueryDialog;
class CFMRadioBackSteppingServiceWrapper;
class CFMRadioDocument;
class CFMRadioMainContainer;
class CFMRadioScanLocalStationsView;
class CFMRadioChannelListView;
class CFMRadioMainView;
class CEikMenuPane;
class MFMRadioLayoutChangeObserver;
class CAlfEnv;
class CAknVolumePopup;
class CAknInformationNote;
class CAknGlobalNote;
class CIAUpdate;
class CIAUpdateParameters;
class CFMRadioControlEventObserverImpl;
class CRepository;
// CLASS DECLARATION

/**
* Interface to tell when dialog was terminated
*
*    @since 2.6
*/
class MInformationNoteInterface
	{
	public:
		
		/**
        * tells when dialog was terminated
        */
		virtual void DialogTerminated() = 0;
	
	};
	
	
/**
* Information note for headset key events. Enabling functionality to disable new dialog creation 
* when dialog is active
*
*    @since 2.6
*/
class CFMInformationNote: public CAknInformationNote
	{
	public:
	
		/**
        * default constructor.
        */
		CFMInformationNote( MInformationNoteInterface& aObserver);
		
		/**
        * Destructor.
        */
		virtual ~CFMInformationNote();
		
	private:
		
		// Dialog event observer
		MInformationNoteInterface& iDialogObserver;
	
	};

 

/**
* Instantiates the application views. It also acts as the default command handler for the
*  application. Part of the standard application framework.
*
*    @since 2.6
*/

class CFMRadioAppUi : public CAknViewAppUi,
                      public MFMRadioSvkEventsObserver,
                      public MCoeControlObserver,
                      private MRadioEngineStateChangeCallback,
                      private MFMRadioGlobalConfirmationQueryObserver,
                      private MInformationNoteInterface,
                      public MChannelListHandler,
                      public MIAUpdateObserver
                      
    {
    public:
        // Volume, seek, tune and channel changing directions
        enum TFMRadioDirections
            {
            EDirectionUp,
            EDirectionNone,
            EDirectionDown
            };

    public: // Constructors and destructor
        /**
        * EPOC default constructor.
        */
        void ConstructL();
        /**
        * Destructor.
        */
        virtual ~CFMRadioAppUi();

        /**
         * Utility method to select the right layout resource according to 
         * mirroring and orientation
         *
         * @param aNormalPortraitRes Normal portrait resource id
         * @param aMirroredPortraitRes Mirrored portrait resource id
         * @param aNormalLandscapeRes Normal landscape resource id
         * @param aMirroredLandscapeRes Mirrored landscape resource id
         * @return one of the resource id's passed to this method
         */
        TInt ChooseLayoutResource( const TInt aNormalPortraitRes, 
                const TInt aMirroredPortraitRes,
                const TInt aNormalLandscapeRes, 
                const TInt aMirroredLandscapeRes ) const;
    public: // Functions from base classes
        
        /**
        * From CEikAppUi, takes care of command handling.
        * @since 2.6
        * @param aCommand command to be handled
        */
        void HandleCommandL( TInt aCommand );
        
        /**
        * Notification interface from RadioEngine. Used to inform
        * us when requests have completed.
        * @since 2.6
        * @param aEventCode code corresponding to the event being propogated
        * @param aErrorCode error code of the event being propogated
        */
        void HandleRadioEngineCallBack( MRadioEngineStateChangeCallback::TFMRadioNotifyEvent aEventCode, TInt aErrorCode );
    public: // new functions 
    	/**
		* Returns a pointer to the radio engine
	    *
	    * @return Radio engine
		*/
    	CRadioEngine* RadioEngine();
    	
        /**
         * Returns a reference to Back Stepping wrapper
         * 
         * @return Reference to Back Stepping wrapper
         */
        CFMRadioBackSteppingServiceWrapper& BackSteppingWrapper() const;
    	
    	/**
	     * Returns a pointer to FMRadio's document, cannot be null.
	     *
	     * @return A pointer to a CFMRadioDocument object.
	     */
    	CFMRadioDocument* Document() const;
        /**
		* Checks if current screen orientation is landscape
	    *
	    * @return ETrue if orientation is landscape, otherwise EFalse
		*/
		TBool IsLandscapeOrientation() const;
		
    	/**
     	* Handles offline mode at startup.
     	*/
     	void HandleOfflineModeAtStartUpL();
     	
        /**
        * Handles start up foreground event.
        */		
        void HandleStartupForegroundEventL();
        /**
        * Nunber of channels stored in presets.
        * @return number of channels in presets
        */ 
        TInt NumberOfChannelsStored() const;           
        /**
    	 * Returns the UID of the currently active local view. KNullUid if none 
    	 * active (construction/destruction).
    	 * @return UID of the currently active local view, KNullUid if none 
    	 * active.
    	 */
    	TUid ActiveView() const;
    	/**
    	* Return startup scanning wizard status.
    	* @return running status
    	*/
    	TBool IsStartupWizardRunning() const;
    	/**
    	* Set startup scanning wizard status.
    	* @param aRunningState running state
    	*/
    	void SetStartupWizardRunning( const TBool aRunningState );
    	/**
        * Tune to the specified frequency
        * @since 2.6
        * @param aFrequency the frequency to be tuned
        */
        void TuneL(TInt aFrequency);
        /**        
        * @return mirrored layout state
        */
        TBool IsLayoutMirrored() const;
		/**
		* Show currently active volume popup control
		*/
		void ShowVolumePopupL();

		/**
		 * From MChannelListHandler 
		 * Updates channels array in appui 
		 * @param aOperation tells which operation is made to array
		 * @param aIndex tells affecting index
		 * @param aMovedToNewIndex optional during move operation
		 */	
		void UpdateChannelsL( TMoveoperations aOperation, 
				TInt aIndex, 
				TInt aMovedToNewIndex  );
		
		/**
		 * From MChannelListHandler
		 * Add a new channel to list of channels
		 * @param aChannelName Name of the channel to be saved
		 * @param aChannelFreq Channel frequency
		 */	
		void AddChannelToListL( const TDesC& aChannelName, TInt aChannelFreq );
		
		/**
		 * From MChannelListHandler
		 * Accessory method to channels array
		 */
		CArrayFixFlat<TChannelInformation>* Channels();
		
		void AutoTuneInMainView ( TBool aTune );
        /**
         * status of the startup wizard state
         * @return wizard status
         */
        TBool IsStartupWizardHandled() const;
        /**
        * Return the index of the first channel that matches the 
        * frequency specified.
        * @since 2.6
        * @param aFrequency The frequency to match against
        * @return the index of the channel whose frequency matched.
        *         -1 if no frequency matched.
        */
        TInt MatchingChannelL( TInt aFrequency );
        
        /**
        * Erase a channel
        * @param aIndex index to delete
        * @return ETrue if user accepted delete, otherwise EFalse
        */
        TBool EraseChannelL( TInt aIndex );

        /*
         * Returns a pointer to the currently active volume popup control
         * @return active volume popup
         */
        CAknVolumePopup* ActiveVolumePopup() const;

    protected:
        /**
        * From CEikAppUi HandleWsEventL
        */
        void HandleWsEventL(const TWsEvent& aEvent,CCoeControl* aDestination);
        
        /**
        * From base class CCoeAppUi    
    	*/
    	void HandleForegroundEventL( TBool aForeground );
    	
    private: // Functions from base classes

        /**
        * Handles a change to the application's resources
        * @param aType The type of changed resource
        */
        void HandleResourceChangeL( TInt aType );
        /**
		* Updates the landscape data. This is done separately 
		* and not in IsLandscapeOrientation method as
		* IsLandscapeOrientation MUST NOT do a window server flush
		*/
		void UpdateLandscapeInformation();
     private: // new functions

        /**
        * All leaving function calls are made here, so that nothing is left on the stack if the ConstructL leaves
        * The reason for this behaviour is that the UI framework doesn't handle leaves from the AppUi correctly.
        */
        void SecondaryConstructL();
        /**
        * Perform object cleanup. This would normally be performed in the destructor, but since it can also be
        * called from SecondaryConstructL it was moved into a seperate function.
        */
        void Cleanup();
        /**
        * Handles callback from engine
        * @since 3.0
        */
        void HandleInitializedCallbackL();
        /**
        * Updates radio volume level by one step at time.
        * @since 2.6
        * @param aDirection Parameter is interpreted as
        *        following: 1 = increase volume, 0 = no change,
        *        -1 = decrease volume.
        */
        void UpdateVolume( CFMRadioAppUi::TFMRadioDirections aDirection );
        /**
        * Perform automatic tuning to the next available audible frequency
        * @since 2.6
        */
        void ScanUpL();
        /**
        * Perform automatic tuning to the last available audible frequency
        * @since 2.6
        */
        void ScanDownL();
        /**
        * Save currently tuned frequency to the currently selected channel
        * @since 2.6
        * @param aIndex the index of the slot to save the channel to.
        */
        void SaveChannelL( TInt aIndex );
        /**
        * Tune the radio hardware to the frequency saved at the specified channel
        * @since 2.6
        * @param aIndex index of channel to tune to
        */
        void PlayChannel( TInt aIndex );
        /**
        * Set the audio output of the radio, and update the UI
        * @since 2.6
        * @param aAudioOutput the audio output (headset or IHF)
        */
        void SetAudioOutput( CRadioEngine::TFMRadioAudioOutput aAudioOutput );
        /**
        * Cancel outstanding seek request
        * @since 2.6
        */
        void CancelSeek();
        /**
        * Shutdown the application.
        * @since 2.6
        */
        void ExitApplication();
        /**
        * Rename the currently selected channel
        * @since 2.6
        */
        void RenameCurrentChannelL();
        /**
        * Display confirmation dialog for channel deletion
        * @param aIndex the index of the channel to delete
        */
        TBool ConfirmChannelListDeleteL( TInt aIndex );
        /**
        * Saves channel to be last item in channels list
        * @since 2.6
        */
        void SaveChannelToLastIntoListL();
        /**
        * Displays an error note with the text contained in the passed in reference
        * @since 2.6
        * @param aErrorNote id of the text to display in the error note
        */
        void DisplayErrorNoteL( TInt aErrorNote );
        
        /**
        * Displays an information note with the text contained in the passed in reference
        * @since 2.6
        * @param aInfoNote id of the text to display in the error note
        */
        void DisplayInformationNoteL( TInt aInfoNote );
        
        /**
        * Return the numeric key value corresponding to the key code
        * @since 2.6
        * @param aCode the Symbian key code
        * @return the numeric key value
        */
        TInt NumericKeyValue( TUint aCode );
        /**
        * Turn the fm radio hardware on
        * @since 2.6
        */
        void TurnRadioOn();
        /**
        * Processes "seek operation completed" callback
        * @since 2.6
        */
        void HandleStopSeekCallback();
        /**
        * Processes "set mute state" callback
        * @since 2.6
        */
        void HandleSetMuteStateCallback();
        /**
        * Processes "volume changed" callback
        * @since 2.6
        */
        void HandleVolumeChangedCallback();
        /**
        * Processes "set audio output completed" callback
        * @since 2.6
        */
        void HandleAudioOutputSetCallback();
        /**
        * Processes "headset reconnected" callback
        * @since 2.6
        */
        void HandleHeadsetReconnectedCallback();
        /**
        * Processes "headset disconnected" callback
        * @since 2.6
        */
        void HandleHeadsetDisconnectedCallback();
        /**
        * Processes "flightmode disabled" callback
        * @since 2.6
        */
        void HandleFlightmodeDisabledCallback();
        /**
        * Processes "volume update failed" callback
        * @since 2.6
        */
        void HandleVolumeUpdateFailedCallback();                
        /**
        * Handles mute keypress from main view
        * 
        */
        void HandleMuteCommand();
        /**
        * Processes "manual tune failed" callback
        * @since 2.6
        */
        void HandleManualTuneFailedCallback();
        /**
        * Processes audio resource not available
        * @since 3.0
        */
        void HandleAudioResourceNotAvailableL(TInt aErrorCode);

        /**
        * Processes audio resource available
        * @since 3.0
        */
        void HandleAudioResourceAvailableL();

        /**
        * Processes FM Transmitter on.
        * @since 3.2
        */
        void HandleFMTransmitterOnCallbackL();

        /**
        * Processes Frequency range changed.
        * @since 3.2
        */
        void HandleFreqRangeChangedCallback();

        /**
        * Processes Frequency range changed.
        * @since 3.2
        */
        void HandleFlightModeEnabledCallbackL();

        /**
        * Handle flight mode disabled.
        * @since 3.2
        */
        void HandleFlightModeDisabledCallback();

        /**
        * Processes Frequency range changed.
        * @since 3.2
        */
        void HandleTunerReadyCallback();

        /**
 		* Get region automatically from network or manually by user choice if network
        * is not available
        */
        TFMRadioRegionSetting HandleRegionsAtStartUpL();
        
        /**
        * Request tuner control from engine
        */        
        void RequestTunerControl() const;
        
        /**
        * Is offline profile activated when radio audio was disabled.
        * @return ETrue, if offline profile activated when radio 
        *         audio was disabled. EFalse otherwise.
        */
        TBool IsOfflineProfileActivatedWhenRadioAudioDisabled() const;        

        /**
        * Sets the toolbar visibility.
        * @param aVisible 
        */ 
        void ShowToolbar( TBool aVisible );

        /**
        * Determine if scan all local stations view should be started
        * in wizard mode
        */
        void HandleStartupWizardL();

        /**
        * Start local stations scan
        */        
        void StartLocalStationsSeekL();

        /**
        * Tries to start radio again if the situation allows it
        */        
        void TryToResumeAudioL();
        /**
        * Fade and show exit on all views
        */        
        void FadeViewsAndShowExit( TBool aState );

        /*
        * Show connect headset query with only exit softkey
        */
        void ShowConnectHeadsetDialogL();

        /*
         * Get help context based on active view
         * @return array containing help context for active view
        */
        CArrayFix<TCoeHelpContext>* GetCurrentHelpContextL() const;

        /**
        * A call back function for processing start up foreground event. 
        *
        * @param aSelfPtr  Pointer to self.
        * @return Always 0.
        */        
        static TInt StaticStartupForegroundCallback( TAny* aSelfPtr );     
        
    private:

        /**
        * Called when volume level should be changed.
        * Set volume control visible.
        * @param aVolumeChange: +1 change volume up
        *                       -1 change volume down
        */
        void FMRadioSvkChangeVolumeL( TInt aVolumeChange );

        /**
        * Called when volume was changed last time 2 seconds ago or
        * timer was started 2 seconds ago. The remove control timer
        * is started when ChangeVolumeL() is called
        * or when started through CMmsSvkEvents::StartRemoveTimerL()
        */
        void FMRadioSvkRemoveVolumeL( );

        void FMRadioHeadsetEvent(TAccessoryEvent aEvent);
        
        /**
         * From MFMRadioGlobalConfirmationQueryObserver
         * 
         * @see MFMRadioGlobalConfirmationQueryObserver::GlobalConfirmationQueryDismissedL(TInt aSoftKey)
         */
        void GlobalConfirmationQueryDismissedL( TInt aSoftKey );
        
        /**
        *From MCoeControlObserver. To handle the volume events.
        */
        void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );
        
        /**
        * From MInformationNoteInterface. To handle dialog terminated events.
        */
        void DialogTerminated();
        
        /**
        * Gets the Channels array
        */
        void GetChannelsArrayL();
		/**
		* Check available IAD updates
		*/
		void HandleIADUpdateCheckL();
		// from base class MIAUpdateObserver
    	void CheckUpdatesComplete( TInt aErrorCode, TInt aAvailableUpdates );
    	void UpdateComplete( TInt aErrorCode, CIAUpdateResult* aResultDetails );
    	void UpdateQueryComplete( TInt aErrorCode, TBool aUpdateNow );
        
    	/**
    	 * Processes the tail of the command
    	 * @param aTail The tail to be processed
    	 */
    	void ProcessCommandTailL( const TDesC8& aTail );
    	
        // from base class CEikAppUi	
        MCoeMessageObserver::TMessageResponse HandleMessageL(
            TUint32 aClientHandleOfTargetWindowGroup, 
            TUid aMessageUid, 
            const TDesC8& aMessageParameters );

        TBool ProcessCommandParametersL( TApaCommand aCommand, 
                                         TFileName& aDocumentName, 
                                         const TDesC8& aTail );
	    
        /**
        * Check if Active Idle app is on foreground.
        *
        * @return ETrue if idle app is in the foreground, EFalse otherwise.
        */	
        TBool IsIdleAppForeground();
	
        /**
        * Check if Active Idle is enabled.
        *
        * @return ETrue if active idle is in the enabled, EFalse otherwise.
        */
        TBool IsActiveIdleEnabled();

        /**
         * Handles the possibly pending view activation 
         */
        void HandlePendingViewActivationL();
        
    private:

        enum TInterfaceState
            {
            EFMRadioStateOff = 0, 			// Radio off
            EFMRadioStateOffForPhoneCall,	// Radio off because a phone call
            EFMRadioStateOffBeforePhoneCall,// Radio was off + phone call going on -> no resume
            EFMRadioStateOn,
            EFMRadioStateRecording,
            EFMRadioStateBusyRadioOn,
            EFMRadioStateBusyRadioOff,
            EFMRadioStateBusyScanUp,
            EFMRadioStateBusyScanDown,
            EFMRadioStateBusySeek,
            EFMRadioStateBusyMute,
            EFMRadioStateBusyVolume,
            EFMRadioStateBusyManualTune,
            EFMRadioStateBusyScanLocalStations,
            EFMRadioStateExiting
            };
            
        /**
         * State to indicate the mute status
         */
        enum TFMMuteStatus
            {
            EFMUninitialized = 0,       // Uninitialized value
            EFMUnmuted,                 // Unmute state
            EFMMuted                    // Mute state
            };
        
        // System's volume indicator
        CAknVolumePopup* iIhfVolumePopupControl;
        CAknVolumePopup* iHeadsetVolumePopupControl;
        CAknVolumePopup* iActiveVolumePopupControl;
                        
        CRadioEngine*   iRadioEngine;
        TInt iCurrentRadioState;
        CFMRadioScanLocalStationsView* iScanLocalStationsView;
        CFMRadioChannelListView* iChannelListView;
        CFMRadioMainView* iMainView;
        //all the possible radio UI states
        TInt iFMRadioVariationFlags;
        CFMRadioSvkEvents*   iSvkEvents;
        TBool iAlreadyClean;
        MFMRadioLayoutChangeObserver* iLayoutChangeObserver;
        CFMRadioControlEventObserverImpl* iControlEventObserver;
        /**
        * Is audio resources available
        */
        TBool iAudioResourceAvailable;
        
        /**
        * Was audio lost "permanently" due to another music playing application?
        */
        TBool iAudioLost;
        
        /**
        * Global offline query. Owned
        */
        CFMRadioGlobalConfirmationQuery* iGlobalOfflineQuery;
        
        /**
        * Local activate offline query. Owned
        */  
        CAknQueryDialog* iLocalActivateOfflineQuery;
        
        /**
        * Local continue offline query. Owned
        */
        CAknQueryDialog* iLocalContinueOfflineQuery;
        
        /**
        * Is offline profile activated when radio audio was disabled.
        */
        TBool iOfflineProfileActivatedWhenRadioAudioDisabled;
        
        /**
        * Is the local "continue in offline" query visible.
        */
        TBool iShowingLocalOfflineContinueQuery;
        
        /**
        * Is the local or global offline query activated.
    	*/
        TBool iOfflineQueryDialogActivated;
        
    	/** 
    	* Is the start up of application ongoing. 
    	*/
    	TBool iStartUp;
        
        /** 
		* Is the screen orientation landscape 
		*/
		TBool iLandscape;
        CAlfEnv* iAlfEnv; // Owned
        // flag to indicate wizard handling
        TBool iStartupWizardHandled;
        // flag for startup wizard status
        TBool iStartupWizardRunning;
        // connect headset query
        CAknQueryDialog* iConnectHeadsetQuery;
        // flag to indicate if automatic tune is activated from startup wizard
        TBool iTuneFromWizardActivated;    
       	// akn information note ptr to check that dialog has dismissed from display
       	TBool iInfoNoteOn;
       	// global note for headset status when radio is background
       	CAknGlobalNote* iConnectHeadsetGlobalNote;
       	// flag for previous mute status 
        TFMMuteStatus iMuteStatusBeforeRadioInit;
       	// Channels list
       	CArrayFixFlat<TChannelInformation>* iChannels; 	  
       	// IAD client object. Owned.
       	CIAUpdate* iUpdate; 	  
       	// Parameters for IAD update. Owned.
       	CIAUpdateParameters* iParameters;       	
       	TBool iAutoTune;
       	TBool iAutoTuneUnmute;
        // The backstepping wrapper. Owned.
        CFMRadioBackSteppingServiceWrapper* iBsWrapper;       	
        // Is feature manager initialized, used for uninitializing feature manager
        TBool iFeatureManagerInitialized;
        // own, for active idle setting
        CRepository* iSettingsRepository;
        // Callback for processing start up foreground event. 
        CAsyncCallBack* iStartupForegroundCallback;
        // The view id of the view that is to be activated when possible
        TUid iPendingViewId;
        // Boolean for region change, force rescanning
        TBool iRegionChanged;
    };

#endif

// End of File
