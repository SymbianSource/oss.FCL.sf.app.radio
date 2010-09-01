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
* Description:  definition of the class CFMRadioScanLocalStationsContainer
*
*/


#ifndef FMRADIOSCANLOCALSTATIONSCONTAINER_H
#define FMRADIOSCANLOCALSTATIONSCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <aknlists.h>

#include "fmradiodefines.h"
#include "fmradio.hrh"
#include "fmradioengine.h"
// FORWARD DECLARATIONS
class CAknWaitDialog;

// CLASS DECLARATION

/**
* Creates and owns the UI components related to the Scan Local Stations View.
* @since 2.6
*/
class CFMRadioScanLocalStationsContainer : public CCoeControl,
                                     	   public MEikListBoxObserver
    {
    public: // Constructors and destructor
        /**
        * Two-phase constructor of CFMRadioScanLocalStationsContainer
        * @since 2.6
        * @param aRect Frame rectangle for container.
        * @param aRadioEngine Radio engine reference
        */
        static CFMRadioScanLocalStationsContainer* NewL( const TRect& aRect, CRadioEngine& aRadioEngine );
        /**
        * Destructor.
        */
        virtual ~CFMRadioScanLocalStationsContainer();
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
        * @param aInfoText Info text to be shown instead of the frequency value
        * @param aChannelFrequency the new frequency of the channel
        * @param aNowPlaying indicates if 'Now playing' icon needs to be shown
        * @param aInfoTextPlacement ETrue if info text is displayed after frequency string; EFalse otherwise.
        */
        void UpdateChannelListContentL( TInt aIndex, const TDesC& aInfoText, TInt aChannelFrequency, TBool aNowPlaying, TBool aInfoTextPlacement = EFalse );
        void RemoveChannelListContentL( TInt aIndex );
        /**
        * Update the content of the channel at aIndex with the values specified
        * @since 2.6
        * @param aIndex index of the last selected channel
        */
        void UpdateLastListenedChannel( TInt aIndex );
        /**
        * Displays 'Now Playing' icon for given index.
        * @param aNewIndex Index of channel to get the icon
        * @param aOldIndex Index of channel to loose the icon
        */
        void UpdateNowPlayingIconL( TInt aNewIndex, TInt aOldIndex = KErrNotFound );
        /**
        * Hide 'Now Playing' icon from given index
        * @param aIndex Index of channel to hide the icon
        * @param aDraw flag for updating list after icon removal
        */
        void HideNowPlayingIconL( TInt aIndex, TBool aDraw );
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
        * Resets channel list
        */
        void ResetChannelListL();
        /**
        * Create the channel list (initially all spots are set as empty)
        * @since 2.6
        */
        void InitializeChannelListL();
        
        void SetStationListEmptyTextL( const TDesC& aText );
        
        /*
         * Adds frequency to the scanned channels list. Used while
         * scanning is ongoing
         * @param aIndex channel index
         * @param aChannelFrequency frequency to add
         */
        void InsertScannedChannelToListL( TInt aIndex, TInt aChannelFrequency );

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
    	CFMRadioScanLocalStationsContainer( CRadioEngine& aRadioEngine );    
        /**
        * EPOC two phased constructor
        * @since 2.6
        * @param aRect Frame rectangle for container.
        */
        void ConstructL( const TRect& aRect);
        
        /*
         * Adds icons to the icon array.
         * @param aArray array to add icons 
         */
        void CreateListIconsL( CArrayPtr<CGulIcon>& aArray );

    private: //data
    
        // The channel list listbox
        CAknSingleNumberStyleListBox* iChannelList;
        // Array of channel list items
        CDesCArray* iChannelItemArray;
        // Index of the most recently listened channel to keep the listbox up-to-date.
        TInt iLastChIndex;
        TBool iFadeStatus;
        MAknsSkinInstance* iSkin;
        CRadioEngine& iRadioEngine; //not own
        // array for list icon bitmaps
        RPointerArray<CFbsBitmap> iBitMaps;

    };

#endif

// End of File
