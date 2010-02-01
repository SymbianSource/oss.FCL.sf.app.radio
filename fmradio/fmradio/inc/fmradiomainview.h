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
* Description:  Definition of the class CFMRadioMainView.
*
*/


#ifndef FMRADIOMAINVIEW_H
#define FMRADIOMAINVIEW_H

// INCLUDES
#include <aknview.h>
#include <alf/alfenv.h>
#include <akntoolbarobserver.h> 

#include "mfmradiolayoutchangeobserver.h"
#include "fmradiordsobserver.h"
#include "fmradio.hrh"
#include "mchannellisthandler.h"
#include "fmradioalfrdsviewobserver.h"
#include "fmradiologoobserver.h"

// FORWARD DECLARATIONS
class CFMRadioMainContainer;
class CRadioEngine;
class CFMRadioMusicStoreHandler;

// CLASS DECLARATION

/**
*  CFMRadioMainView view class.
* 
*    @since 2.6
*/
class CFMRadioMainView : public CAknView, 
                         public MFMRadioLayoutChangeObserver,
                         public MFMRadioRdsObserver,
                         public MAknToolbarObserver,
                         public MFMRadioAlfRdsViewObserver,
                         public MFMRadioLogoObserver
    {
    public: // Constructors and destructor
        /**
        * Two-phase constructor of CFMRadioMainView
        * @since 2.6
        * @param aRadioEngine pointer to the radio engine object
        * @param aAlfEnv alfred environment reference
        * @param aObserver channel list handler 
        */
        static CFMRadioMainView* NewL( 
        			CRadioEngine* aRadioEngine, 
        			CAlfEnv& aAlfEnv,
        			MChannelListHandler& aObserver );
        /**
        * Destructor.
        * @since 2.6
        */
        virtual ~CFMRadioMainView();
    public: // New functions
        /**
        * Return the unique identifier corresponding to this view
        * @since 2.6
        * @return unique view identifier
        */
        TUid Id() const;
        /**
        * Fade the view and all controls associated with it. Change
        * the soft keys so that "Exit" is displayed
        * @since 2.6
        * @param aFaded flag whether to fade or unfade
        */
        void FadeAndShowExit( TBool aFaded );
        /**
        * Update display when focus is regained
        * @since 3.0
        */
        void UpdateDisplayForFocusGained();
        /**
        * Tells the window owning container to display seek note
        * and updates softkeys
        * @since 2.6
        */
        void SeekL();
        /**
        * Seek operation has completed. Update the view to reflect
        * this fact.
        * @since 2.6
        */
        void StopSeekL();
        /**
        * Inform the window owning container that it should display
        * the frequency and name of the channel specified
        * @since 2.6
        * @param aChannelIndex The index of the channel to display
        */
        void DisplayChannelL( TInt aChannelIndex );
        /**
        * from MFMRadioLayoutChangeObserver
		* This method gets called by the AppUI when a dynamic layout change
		* event occurs.
		*/
		void LayoutChangedL( TInt aType );
	
		/**
		 * Sets the type for forthcoming station change
		 * @param aChangeType The change type
		 */
		void SetStationChangeType( TFMRadioStationChangeType aChangeType );
		
		/*
		 * called before channel is about to change
		 * */
		void PrepareViewForChannelChange();

		/**
		* Sets toolbar visibility
		* @param aVisible 
		* @return none
		*/
		void ShowToolbar( TBool aVisible );
		 
	    /**
	    *  From base class MFMRadioRdsObserver
	    */
	    void RdsDataProgrammeService( const TDesC& aProgramService );
	    void RdsDataPsNameIsStatic( TBool aStatic );
	    void RdsDataRadioText( const TDesC& aRadioText );
        void RdsDataRadioTextPlus( const TInt aRadioTextPlusClass, const TDesC& aRadioText );
	    void RdsAfSearchBegin();
	    void RdsAfSearchEnd( TUint32 aFrequency, TInt aError );
	    void RdsAfSearchStateChange( TBool aEnabled );
	    void RdsAvailable( TBool aAvailable );

	    /*
	     * Requests the right idle component to be started  
	     */
        void DetermineActiveMediaIdleComponent();
        
        /*
         * Shows default radio logo
         */
        void ShowDefaultLogo();

    private:  // New functions
        /**
        * EPOC default constructor.
        * @since 2.6
        */
        void ConstructL();
        /**
        * constructor.
        * @since 2.6
        * @param aRadioEngine pointer to the radio engine object
        * @param aAlfEnv alf environment 
        * @param aObserver
        */
        CFMRadioMainView( CRadioEngine* aRadioEngine, CAlfEnv& aAlfEnv, MChannelListHandler& aObserver  );
        /**
         * Shows a tutorial dialog when user sees RT+ interaction indicator
         * for the very first time.
         */
        void ShowRTPlusFirstTimeDialogL();
        /**
         * Determines if RT+ interaction indicator should be shown, based on if
         * data is available in iSongName, iArtistName and iWebUrl.
         * 
         * @return ETrue if there is valid data for RT+ interactions 
         */
        TBool IsRTPlusInterActionIndicatorNeeded();
        /**
         * StaticRdsSignalTimerCallback Callback for iRdsSignalTimer
         */
        static TInt StaticRdsSignalTimerCallback( TAny* aSelfPtr );
       	/**
        * Prepare toolbar for FMRadio command buttons
        */
		void PrepareToolbar();
		/**
        * Set the current dimmed state of the toolbar 
        * @param aState the new toolbar dimmed state
        */
        void SetToolbarDimmedState( TBool aState );

        /**
         * Checks if the rds feed has a web link 
         */
        TBool IsWebLinkAvailable();
        
        /**
         * Updates speaker button state in the toolbar
         * extension list
         */
        void UpdateToolbarSpeakerButtonStatus();
        
    private: // Functions from base classes
        /**
        * All commands are handled by the AppUi
        * @since 2.6
        * @param aCommand key code of the command to handle
        */
        void HandleCommandL( TInt aCommand );
        /**
        * From base class CAknView 
        */
        void HandleForegroundEventL( TBool aForeground );
        /**
        * Activate the main view
        * @since 2.6
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,TUid aCustomMessageId, const TDesC8& aCustomMessage );
        /**
        * Deactivate the main view
        * @since 2.6
        */
        void DoDeactivate();
        /**
        * From MEikMenuObserver, for changing menu items dynamically.
        *
        * @param aResourceId Resource ID identifying the menu pane to initialise.
        * @param aMenuPane The in-memory representation of the menu pane.
        */
        void DynInitMenuPaneL( TInt aResourceId,CEikMenuPane* aMenuPane );
        /**
        * from MAknToolbarObserver
        */
        void OfferToolbarEventL( TInt aCommandId );
        
        /**
         * from MAknToolbarObserver
         */         
        void DynInitToolbarL( TInt aResourceId, CAknToolbar* aToolbar );

        /**
        * Launch WebBrowser or Music shop based on given command Id.
        * @param aCommandId The command id
        */
        void LaunchBrowserL(TInt aCommandId);
        /**
        * Handle RDS programme service change.
        * @param aProgramService New programme service
        */
        void RdsDataProgrammeServiceL( const TDesC& aProgramService );
        /**
         * Handles the notification about type of PS name
         * @param aStatic
         */
        void HandleStaticProgrammeServiceL( TBool aStatic );
        /**
        * PrepareViewForChannelChangeL.
        */
        void PrepareViewForChannelChangeL();
        /**
        * Show save query if the current channel is not saved
        */
        void HandleSaveChannelQueryL();
        /**
        * from MEikMenuObserver
        */
        void SetEmphasis( CCoeControl* aMenuControl, TBool aEmphasis );
        
        // from MFMRadioLogoObserver
        void LogoDisplayTimeCompleted();
        
        // from MFMRadioAlfRdsViewObserver
        void NotifyRdsTextTimerCompletion();
        
    private: // Data
        // pointer to the container of the main view
        CFMRadioMainContainer* iContainer;
        // A pointer to the radio engine object
        CRadioEngine*   iRadioEngine;
        // flag to keep track of whether the radio is currently seeking
        TBool iSeeking;
        
        /**
         * Station change direction used for station information animation
         */
        TFMRadioStationChangeType iChangeDirection;
        
        /** 
	    * Is the start up of application ongoing. 
	    */    
	    TBool iStartUp;
		TBool iFaded;
		/** 
	    * Timer for hiding the RDS signal icon after a delay
	    */    
	    CPeriodic* iRdsSignalTimer;	    
	
        /**
        * Toolbar for controlling the application.	
        */
        CAknToolbar* iToolbar;
        
        /**
        * observer for list event from main list	
        */
        MChannelListHandler& iObserver;
        /**
         * Takes care of Music store integration.
         */
        CFMRadioMusicStoreHandler* iMusicStoreHandler;
        // flag for options menu status
        TBool iOptionsMenuOpen;
    };

#endif

// End of File
