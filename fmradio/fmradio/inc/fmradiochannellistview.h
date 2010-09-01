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
* Description:  definition of the class CFMRadioChannelListView
*
*/


#ifndef FMRADIOCHANNELLISTVIEW_H
#define FMRADIOCHANNELLISTVIEW_H

// INCLUDES
#include <aknview.h>
#include <akntoolbarobserver.h> 

#include "fmradiochannellistcontainer.h"
#include "mfmradiolayoutchangeobserver.h"
#include "fmradiordsobserver.h"
#include "mchannellisthandler.h"


// FORWARD DECLARATIONS
class CFMRadioChannelListViewContainer;
class CRadioEngine;

// CLASS DECLARATION  

/**
* CFMRadioChannelListView view class.
* @since 2.6
*/
class CFMRadioChannelListView : public CAknView,
                                public MCoeControlObserver,
                                public MFMRadioLayoutChangeObserver,
                                public MFMRadioRdsObserver,
                                public MAknToolbarObserver
    {
    public: // Constructors and destructor
        /**
        * Two-phase constructor of CFMRadioChannelListView
        * @since 2.6
        * @param aRadioEngine pointer to a radio engine object
        * @param aObserver Handler for channel list
        */
        static CFMRadioChannelListView* NewL( CRadioEngine* aRadioEngine, 
        		MChannelListHandler& aObserver );
        
        /**
        * CFMRadioChannelListView.
        * @since 2.6
        */
        CFMRadioChannelListView::CFMRadioChannelListView( MChannelListHandler& aObserver ) ;
        
        /**
        * Destructor.
        * @since 2.6
        */
        virtual ~CFMRadioChannelListView();
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
        * @param  aChannelIndex the index of the current channel
        */
        void SetNowPlayingChannel ( TInt aChannelIndex );
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
        * @param aChannelName the new name of the channel
        * @param aChannelFrequency the new frequency
        */
        void UpdateChannelListContentL( TInt aIndex, const TDesC& aChannelName, TInt aChannelFrequency );
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
        * Initialize the channels with the values currently saved in 
        * the radio engine
        * @since 2.6
        */
        void InitializeChannelsL();
        /**
        * Fade the view and all controls associated with it. Change
        * the soft keys so that "Exit" is displayed
        * @since 2.6
        * @param aFaded indicates whether the view should fade/unfade.
        */
        void FadeAndShowExit( TBool aFaded );
        /**
        * Update display when focus regained
        * @since 3.0
        */
        void UpdateDisplayForFocusGained();
        /**
        * from MFMRadioLayoutChangeObserver
		* This method gets called by the AppUI when a dynamic layout change
		* event occurs.
        * @param aType layout change type
		*/
		void LayoutChangedL( TInt aType );
		/**
		* Set empty channel list item 
		* when preset is deleted from fmradiopresetutility
		* @param aIndex list index to delete
		*/
		void SetEmptyChannelListItemL( const TInt aIndex ) const;
		
        /**
        * From MAknToolbarObserver
        * Handle commands from toolbar.
        * @param aCommandId The command id
        */        
        void OfferToolbarEventL( TInt aCommandId );

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
        void RdsDataRadioTextPlus( const TInt aRTPlusClass, const TDesC& aRadioText );
        void RdsAfSearchBegin();
        void RdsAfSearchEnd( TUint32 aFrequency, TInt aError );
        void RdsAfSearchStateChange( TBool aEnabled );
        void RdsAvailable( TBool aAvailable );
        /**
        * Procedures for finishing a move of channel on the list
        * @param none 
        * @return none
        */
        void MoveActionDoneL();
        
        /**
         * Cancel the moving
         */
        void CancelMoveL();
        
        /**
        * Dim relevant buttons when the list is empty
        */
        void UpdateToolbar() const;
        
        /*
         * Updates view when seek event is ended 
         */
        void StopSeekL();
        
        /**
        * Interprets view's menu,softkey and other commands and acts 
        * accordingly by calling the appropriate command handler 
        * function for further action.
        * @since 2.6
        * @param aCommand the command to process
        */
        void HandleCommandL( TInt aCommand );
        
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
        * 
        */
        void SetMiddleSoftKeyLabelL (TInt aResourceId, TInt aCommandId);
       
        /**
        * UpdateMiddleSoftKeyL, updates the MSK
        * 
        */
        void UpdateMiddleSoftKeyL();
    private: // new functions
        /**
        * EPOC default constructor.
        * @since 2.6
        * @param aRadioEngine pointer to a radio engine object
        */
        void ConstructL( CRadioEngine* aRadioEngine );
        
        /**
        * Prepare toolbar for FMRadio command buttons
        */
        void PrepareToolbar();
        /*
         * Set view title text
         * @param aResourceId resource Id containing the title text
         */
        void SetTitleL( TInt aResourceId );
        /**
        * Procedures for beginning a move of channel on the list
        * @param none 
        * @return none
        */
        void MoveCurrentItemIndexL();
        
    private: // Data
        CFMRadioChannelListContainer* iContainer;
        CRadioEngine*   iRadioEngine;
        TInt iChIndex; 
        TInt iNowPlayingIndex;
        TInt iCurrentMSKCommandId; //owned
		TBool iFaded;
	    MChannelListHandler& iObserver;
	    
	    CAknQueryDialog* iManualQueryDialog;
	    
	    TBool iMoveMode;
	    
	   /**
	    * Boolean for knowing if any moves have been made
	    */
	    TBool iFirstMoveDone;
	    
       /**
        * Toolbar for controlling the application.  
        */
        CAknToolbar* iToolbar;
	};

#endif

// End of File
