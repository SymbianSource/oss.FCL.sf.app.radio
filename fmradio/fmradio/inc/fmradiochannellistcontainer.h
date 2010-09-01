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
* Description:  definition of the class CFMRadioRecordingContainer
*
*/


#ifndef FMRADIOCHANNELLISTCONTAINER_H
#define FMRADIOCHANNELLISTCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <aknlists.h>

#include "fmradiodefines.h"
#include "fmradioengine.h"
#include "mchannellisthandler.h"

// FORWARD DECLARATIONS
class CAknWaitDialog;
class CFMRadioChannelListView;
// CLASS DECLARATION

/**
* Creates and owns the UI components related to the Channel List view.
* @since 2.6
*/
class CFMRadioChannelListContainer : public CCoeControl, 
                                     public MEikListBoxObserver
    {
    public: // Constructors and destructor
        /**
        * Two-phase constructor of CFMRadioChannelListContainer
        * @since 2.6
        * @param aRect Frame rectangle for container
        * @param aRadioEngine Radio engine
        * @param aObserver Handler for channel list
        */
        static CFMRadioChannelListContainer* NewL( const TRect& aRect, 
        		CRadioEngine& aRadioEngine,
        		MChannelListHandler& aObserver);
        /**
        * Destructor.
        */
        virtual ~CFMRadioChannelListContainer();
    public: // New functions
        /**
        * Returns the index of the selected channel item from the channel list.
        * @since 2.6
        * @return the index of the currently selected channel
        */
        TInt CurrentlySelectedChannel() const;
        /**
        * Update the content of the channel at aIndex with the values specified
        * @since 2.6
        * @param aIndex index of the channel to update
        * @param aChannelName the new name of the channel
        * @param aChannelFrequency the new frequency of the channel
        */
        void UpdateChannelListContentL( TInt aIndex, const TDesC& aChannelName, TInt aChannelFrequency );
        /**
	    * RemoveChannel from channel list 
	    * @since 2.6
	    * @param aIndex the index of the channel to update
	    */
	    void RemoveChannelL( TInt aIndex );
	    /**
	    * AddChannel from channel list 
	    * @since 2.6
	    * @param aChannelName the new name of the channel
        * @param aChannelFrequency the new frequency of the channel
        * @param aNowPlaying ETrue if channel is tuned
        */
	    void AddChannelL( const TDesC& aChannelName, TInt aChannelFrequency, TBool aNowPlaying ); 
        /**
        * Update the content of the channel at aIndex with the values specified
        * @since 2.6
        * @param aIndex index of the last selected channel
        */
        void UpdateLastListenedChannel( TInt aIndex );
        /**
        * Adds icon to a list item
        * @param aIndex new icon index
        * @param aIconIndex icon index in list icon array
        */
        void UpdateItemIconL( TInt aIndex, TInt aIconIndex );
        /**
        * Hide all icons from the list
        */
        void HideIconsL();
        /**
        * Fades the entire window and controls in the window owned by this container control.
        * @since 2.6
        * @param aFaded flag to indicate whether we should fade or unfade
        */
        void SetFaded( TBool aFaded );
        /**
        * From CCoeControl
        */
        void HandleResourceChange(TInt aType);
        /**
	    * MoveMode handled -> All Done
	    */
        void MoveDoneL();        
        /**
	    * ReIndexAll items in constainer list 
	    */
        void ReIndexAllL();        
        /**
         * Handle movable item drop
         */
        void HandleDropL();        
        /**
         * Move selected item in list up
         */
        void MoveUpL();        
        /**
         * Move selected item in list down
         */
        void MoveDownL();        
        /**
         * Activate move action -> when grab is selected
         */
        void ActivateMoveL();
        /**
         * Move action canceled, return list to previous state
         */ 
        TBool MoveCanceledL();    
        /**
         * Current Move action state.
         */
        TBool MoveAction();
        /**
	     * Touch pad move event handling
	     */ 
        void TouchMoveEventL( TInt aIndex );
        
    private: // Functions from base classes
        /**
        * Called by framework when the view size is changed
        * @since 2.6
        */
        void SizeChanged();
        /**
        * Return the number of controls in the window owned by this container
        * @since 2.6
        * @return number of controls
        */
        TInt CountComponentControls() const;
        /**
        * Return the control corresponding to the specified index
        * @since 2.6
        * @param aIndex the index of the control to retrieve
        * @return the control corresponding to the specified index
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;
        /**
        * From MEikListBoxObserver, for listbox event handling.
        * @since 2.6
        * @param aListBox The originating list box.
        * @param aEventType A code for the event.
        */
        void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );
        /**
        * Allow the channel list to process key events
        * @since 2.6
        * @param aKeyEvent The key event.
        * @param aType The type of key event.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
		/**
        * Required for help.
		* @since 2.7
		* @param aContext the help context to be used.
        */
        void GetHelpContext(TCoeHelpContext& aContext) const;
        
        void FocusChanged(TDrawNow aDrawNow);
        /*
        * from CCoeControl
        */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );
        
    private: // new functions
    	 // Default constructor
    	CFMRadioChannelListContainer( CRadioEngine& aRadioEngine, MChannelListHandler& aObserver ); 
        /**
        * EPOC default constructor.
        * @since 2.6
        * @param aRect Frame rectangle for container.
        */
        void ConstructL( const TRect& aRect);

        /*
         * Adds icons to the icon array.
         * @param aArray array to add icons 
         */
        void CreateListIconsL( CArrayPtr<CGulIcon>& aArray );

        /**
        * Create the channel list (initially all spots are set as empty)
        * @since 2.6
        */
        void InitializeChannelListL();
    private: //data
        // The channel list listbox
        CAknDoubleNumberStyleListBox* iChannelList;
        // Array of channel list items
        CDesCArray* iChannelItemArray;
        // Index of the most recently listened channel to keep the listbox up-to-date.
        TInt iLastChIndex;
        // array for list icon bitmaps
        RPointerArray<CFbsBitmap> iBitMaps;
        TBool iFadeStatus;
        CRadioEngine& iRadioEngine;
        TBool iMoveAction;
        TInt iMoveIndex;
        TInt iTouchMoveIndex;
        MChannelListHandler& iObserver;
        // not owned
        CFMRadioChannelListView* iChannelView;
        // flag for rocker key move activation
        TBool iKeyMoveActivated;
    };

#endif

// End of File
