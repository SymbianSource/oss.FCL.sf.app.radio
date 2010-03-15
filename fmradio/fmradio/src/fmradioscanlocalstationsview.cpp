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
* Description:  Implementation of the class CFMRadioScanLocalStationsView
*
*/


// INCLUDE FILES
#include <AknWaitDialog.h> 
#include <featmgr.h>
#include <aknViewAppUi.h>
#include <aknnotedialog.h>
#include <akntitle.h>
#include <aknnotewrappers.h> // for basic note
#include <aknbutton.h>
#include <avkon.hrh>
#include <avkon.rsg>
#include <StringLoader.h>
#include <eikmenup.h>
#include <eikbtgpc.h>
#include <fmradio.rsg>
#include <fmradio.mbg>
#include <fmradiouids.h>
#include <data_caging_path_literals.hrh>
#include <akntoolbar.h>

#include "fmradiobacksteppingservicewrapper.h"
#include "fmradioengine.h"
#include "fmradiordsreceiverbase.h"
#include "fmradio.hrh"
#include "fmradioscanlocalstationsview.h"
#include "fmradiopreset.h"
#include "fmradioscanlocalstationscontainer.h"
#include "debug.h"
#include "fmradioappui.h"


const TInt KMSKControlID(3);

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::NewL
// Two-phase constructor of CFMRadioScanLocalStationsView
// ---------------------------------------------------------------------------
//
CFMRadioScanLocalStationsView* CFMRadioScanLocalStationsView::NewL( CRadioEngine& aRadioEngine,
		MChannelListHandler& aObserver )
    {
    CFMRadioScanLocalStationsView* self = new (ELeave) CFMRadioScanLocalStationsView( aRadioEngine, 
    		aObserver );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop( self );
    return self; 
    }

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::CFMRadioScanLocalStationsView
// Parameter constructor ( default constructor )
// ---------------------------------------------------------------------------
//
CFMRadioScanLocalStationsView::CFMRadioScanLocalStationsView( CRadioEngine& aRadioEngine,
    MChannelListHandler& aObserver ):
    iNowPlayingIndex( KErrNotFound ),
    iScanAndSaveActivated( EFalse ),
    iScanCancelled( EFalse ),
    iRadioEngine( aRadioEngine ),
    iTuneRequested( EFalse ),
    iObserver( aObserver ),
    iCancelType( EFMRadioCancelScanDefault )
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::ConstructL
// EPOC two-phased constructor
// ---------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::ConstructL()
    {
    BaseConstructL( R_FMRADIO_SEARCH_STATIONS_VIEW );
    PrepareToolbar();
    }

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::~CFMRadioScanLocalStationsView
// Class destructor
// ---------------------------------------------------------------------------
//
CFMRadioScanLocalStationsView::~CFMRadioScanLocalStationsView()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
    	iContainer = NULL;
        }
    
    if ( iScanningNote )
    	{
    	TRAP_IGNORE( iScanningNote->ProcessFinishedL() );
    	iScanningNote = NULL;
    	}
    iScannedChannels.ResetAndDestroy();
    iScannedChannels.Close();
    }

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::Id
// Return the unique identifier corresponding to this view
// ---------------------------------------------------------------------------
//
TUid CFMRadioScanLocalStationsView::Id() const
    {
    return KFMRadioScanLocalStationsViewId;
    }

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::DoActivateL
// Activate the channel list view
// ---------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
												 TUid /*aCustomMessageId*/,
												 const TDesC8& /*aCustomMessage*/ )
    {
    FTRACE( FPrint( _L("CFMRadioScanLocalStationsView::DoActivateL  Start") ) );
    Cba()->SetCommandSetL( R_AVKON_SOFTKEYS_EMPTY ); 
    Cba()->DrawDeferred(); // Redraw softkeys
    

    HBufC* title = StringLoader::LoadLC( R_FMRADIO_SEARCH_STATIONS_VIEW_TITLE, iCoeEnv );
    static_cast<CAknTitlePane*>( StatusPane()->ControlL( 
        TUid::Uid( EEikStatusPaneUidTitle ) ) )->SetTextL( *title );
    CleanupStack::PopAndDestroy( title );

    if ( !iContainer )
        {
        FTRACE( FPrint( _L("CFMRadioScanLocalStationsView::DoActivateL  inside second, not iContainer if") ) );
        iContainer = CFMRadioScanLocalStationsContainer::NewL( ClientRect(), iRadioEngine );
        iContainer->SetMopParent( this );
        iContainer->SetObserver( this );
        AppUi()->AddToStackL( *this, iContainer );
        }
    AppUi()->HandleCommandL( EFMRadioCmdScanLocalStationsViewActive );

    ResetListAndStartScanL();
    iRadioEngine.RdsReceiver().AddObserver( this );
    }

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::HandleCommandL
// Interprets view's menu,softkey and other commands and acts
// accordingly by calling the appropriate command handler
// function for further action.
// ---------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::HandleCommandL( TInt aCommand )
    {
    CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( AppUi() );
    switch ( aCommand )
        {
        case EAknSoftkeyBack: // go back to the main view
            {
            appUi->ActivateLocalViewL( KFMRadioChannelListViewId );
            break;
            }
        case EEikBidCancel:
            {
            if ( iFaded )
                {
                appUi->HandleCommandL( aCommand );
                }
            else
                {
                appUi->ActivateLocalViewL( KFMRadioChannelListViewId );
                }
            break;
            }
        case EFMRadioCmdSaveChannel:
            {
            HandleOneChannelSaveL();
            break;      	        
        	}        	
        case EFMRadioCmdSaveAllChannels:
            SaveAllChannelsL();
            break;
        case EFMRadioCmdScanLocalStationsScan: // start station scanning
            {
            Cba()->SetCommandSetL( R_AVKON_SOFTKEYS_EMPTY ); 
            Cba()->DrawDeferred(); // Redraw softkeys
            //reset all counters, make station list empty
            ResetListAndStartScanL();
        	break;
        	}
        	
        case EFMRadioCmdListenCh:
        	{
        	PlayCurrentlySelectedChannelL();
        	SetContextMenu( R_FMRADIO_SCAN_STATIONS_CTX_MENUBAR_SAVE );
        	break;
        	}
        default: // Send all other commands to AppUi
            appUi->HandleCommandL( aCommand );
            break;
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::SetLastListenedChannel
// Keep track of the last selected channel
// ---------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::SetLastListenedChannel( TInt aIndex )
    {
    iChIndex = aIndex;
    if ( iContainer )
        {
        iContainer->UpdateLastListenedChannel( aIndex ); // Update index
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::SetNowPlayingChannelL
// Keep track of the current selected channel
// ---------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::SetNowPlayingChannelL( TInt aChannelIndex )
    {
    TInt oldIndex = iNowPlayingIndex;
    iNowPlayingIndex = aChannelIndex;
    
    if ( iContainer && ( aChannelIndex != oldIndex ) )
        {
        if ( ChannelInUse( oldIndex ) &&
                !iScannedChannels[ oldIndex ]->PresetNameValid() )
            {
            // Remove dynamic ps name
            iContainer->UpdateChannelListContentL( oldIndex,
                    KNullDesC,
                    iScannedChannels[ oldIndex ]->PresetFrequency(),
                    EFalse,
                    EFalse );
            }
        iContainer->UpdateNowPlayingIconL( iNowPlayingIndex, oldIndex );
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::CurrentlyPlayingChannel
// Returns the index of the playing channel item from the channel list.
// ---------------------------------------------------------------------------
//
TInt CFMRadioScanLocalStationsView::CurrentlyPlayingChannel()
	{
	return iNowPlayingIndex;
	}

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::ChannelInPlay
// Return true/false to indicate whether the channel specified
// is in play
// ---------------------------------------------------------------------------
//
TBool CFMRadioScanLocalStationsView::ChannelInPlay( TInt aChannelIndex ) const
    {
    if( aChannelIndex == iNowPlayingIndex )
    	{
    	return ETrue;
    	}
    else 
    	{
    	return EFalse;
    	}
    }

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::ChannelInUse
// Return true/false to indicate whether the channel specified
// is in use
// ---------------------------------------------------------------------------
//
TBool CFMRadioScanLocalStationsView::ChannelInUse( const TInt aChannelIndex ) const
    {
    TBool ret = EFalse;
    FTRACE( FPrint( _L("CFMRadioScanLocalStationsView::ChannelInUse  Start") ) );

    if ( aChannelIndex >= KMinNumberOfChannelListItems &&
            aChannelIndex < iScannedChannels.Count() )
        {
        ret = ETrue;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::FadeAndShowExit
// Fade the view and all controls associated with it. Change
// the soft keys so that "Exit" is displayed
// ---------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::FadeAndShowExit( TBool aFaded )
    {
    TInt err = KErrNone;
    iFaded = aFaded;
    if( iContainer )
        {
        if ( aFaded ) // Fade
            {
            // In case options menu is open when headset is disconnected
            StopDisplayingMenuBar();
            SetToolbarDimmedState( ETrue ); 
            iContainer->SetFaded( aFaded );
            TRAP( err,Cba()->SetCommandSetL( R_FMRADIO_SOFTKEYS_EXIT ) );
            }
        else
            {
            SetToolbarDimmedState( EFalse ); 
            iContainer->SetFaded(aFaded);            
            TRAP( err,Cba()->SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_BACK ) );
            // Update index to be sure it is up-to-date (in case if list was scrolled in idle state)
            iContainer->UpdateLastListenedChannel( iChIndex );
       	    
            if( iScannedChannels.Count() > 0 )
            	{
                TRAP_IGNORE( SetMiddleSoftKeyIconL(); )
            	
				if ( ( CurrentlyPlayingChannel() == CurrentlySelectedChannel() ) )
					{
					SetContextMenu( R_FMRADIO_SCAN_STATIONS_CTX_MENUBAR_SAVE );
					}
				else
					{
					SetContextMenu( R_FMRADIO_SCAN_STATIONS_CTX_MENUBAR );
					}
            	}
            }
        Cba()->DrawDeferred();
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::UpdateDisplayForFocusGained
// Update the view after a focus gained event.
// 1. Fade/Unfade view
// 2. Update MSK label
// ---------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::UpdateDisplayForFocusGained()
    {
    FadeAndShowExit( iFaded );
    }

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::UpdateChannelListContentL
// Make a request to the channel container to update the
// content of the channel at aIndex with the values specified
// ---------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::UpdateChannelListContentL( TInt aIndex,
															   TInt aChannelFrequency )
    {
    TBool nowPlaying = EFalse;	
    if( ChannelInPlay( aIndex ) )
	    {
	    nowPlaying = ETrue;	
	    }
	else 
		{
		 nowPlaying = EFalse;	
		}
    if ( iContainer )
        {
        iContainer->UpdateChannelListContentL( aIndex, KNullDesC, aChannelFrequency, nowPlaying );
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::CurrentlySelectedChannel
// Returns the index of the selected channel item from the
// channel list. If the container is active get the info from
// it, because it may have more up-to-date info.
// ---------------------------------------------------------------------------
//
TInt CFMRadioScanLocalStationsView::CurrentlySelectedChannel()
    {
    if ( iContainer )
        {
        iChIndex = iContainer->CurrentlySelectedChannel();
        }
    return iChIndex;
    }

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::NextChannel
// Retrieve the index of the next channel that has been set.
// ---------------------------------------------------------------------------
//
TInt CFMRadioScanLocalStationsView::NextChannel()
    {
    if ( iContainer )
        {
        iChIndex = iContainer->CurrentlySelectedChannel();
        }
    else
        {
        iChIndex = iRadioEngine.GetPresetIndex();
        }

    TInt nextChannel = iChIndex;
    for ( TInt i = iChIndex + 1; i < KMaxNumberOfChannelListItems; i++ )
        {
        if ( ChannelInUse( i ) )
            {
            nextChannel = i;
            break;
            }
        }
    if ( nextChannel == iChIndex )
        {
        for ( TInt i = 0; i < iChIndex; i++ )
            {
            if ( ChannelInUse( i ) )
                {
                nextChannel = i;
                break;
                }
            }
        }
    return nextChannel;
    }

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::PreviousChannel
// Retrieve the index of the previous channel that has been set.
// ---------------------------------------------------------------------------
//
TInt CFMRadioScanLocalStationsView::PreviousChannel()
    {
    if ( iContainer )
        {
        iChIndex = iContainer->CurrentlySelectedChannel();
        }
    else
        {
        iChIndex = iRadioEngine.GetPresetIndex();
        }

    TInt previousChannel = iChIndex;
    
    for ( TInt i = iChIndex - 1; i >= 0; i-- )
        {
        if ( ChannelInUse( i ) )
            {
            previousChannel = i;
            break;
            }
        }
    if ( previousChannel == iChIndex )
        {
        for ( TInt i = KMaxNumberOfChannelListItems - 1; i > iChIndex; i-- )
            {
            if ( ChannelInUse( i ) )
                {
                previousChannel = i;
                break;
                }
            }
        }
    return previousChannel;
    }

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::HandleControlEventL
// Handles control event observing. In this case the interest
// is only on the channel selection from the channel list.
// Other events are not reported by the container,
// ---------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::HandleControlEventL( CCoeControl* aControl,
                                                         TCoeEvent aEventType )
    {
    if ( aControl == iContainer )
        {
        if ( aEventType == MCoeControlObserver::EEventRequestFocus )
            {
            // Don't display MSK when display is faded.
            if ( !iFaded && !AknLayoutUtils::PenEnabled() )
                {
                // event sent if up or down arrow keys are pressed
           	    if ( ( CurrentlyPlayingChannel() == CurrentlySelectedChannel() ) )
                    {
                    SetContextMenu( R_FMRADIO_SCAN_STATIONS_CTX_MENUBAR_SAVE );
                    }
                else
                    {
                	SetContextMenu( R_FMRADIO_SCAN_STATIONS_CTX_MENUBAR );
                    }
                }
            else if ( ChannelInUse( CurrentlySelectedChannel() ) )
                {
                PlayCurrentlySelectedChannelL();
                }
            }
        else if ( aEventType == MCoeControlObserver::EEventStateChanged
                && ChannelInUse( CurrentlySelectedChannel() ) )
            {
            PlayCurrentlySelectedChannelL();
            }
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::DoDeactivate
// Deactivate the channel list view
// ---------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::DoDeactivate()
    {
    FTRACE( FPrint( _L("CFMRadioScanLocalStationsView::DoDeactivate") ) );
    CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( AppUi() );
    
    if ( iToolbar )
        {
        // hide toolbar so it is not visible when activating view again
        iToolbar->SetToolbarVisibility( EFalse );
        }

    if ( appUi->RadioEngine() )
    	{
        iRadioEngine.RdsReceiver().RemoveObserver( this );
    	}
    if ( iContainer )
        {
        appUi->RemoveFromViewStack( *this, iContainer );
        }

    iScanAndSaveActivated = EFalse;
    
    // If exiting, iRadioEngine is no longer valid object
    if( appUi->RadioEngine() )
        {
        appUi->RadioEngine()->CancelScanLocalStationsScan();
        // Stop seek doesn't get called automatically as this is not active view anymore
        TRAP_IGNORE( StopSeekL() )
        }
    
    delete iContainer;
    iContainer = NULL;
	// release allocated memory for scanned stations
    iScannedChannels.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::DynInitMenuPaneL
// Sets the state of menu items dynamically according to the
// state of application data.
// ---------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::DynInitMenuPaneL( TInt aResourceId,
                                                      CEikMenuPane* aMenuPane )
    {
    if ( aResourceId == R_FMRADIO_SEARCH_STATIONS_OPTIONS_MENU ) // Main menu
        {
        if ( iRadioEngine.IsAudioRoutingPossible() )
            {
            if ( iRadioEngine.GetAudioOutput() == CRadioEngine::EFMRadioOutputIHF )
                {
                // Delete existing menu option and add a new one
                aMenuPane->DeleteMenuItem( EFMRadioCmdActivateIhf );
                if ( iRadioEngine.IsHeadsetConnected() )
                    {
                    aMenuPane->SetItemTextL( EFMRadioCmdDeactivateIhf, R_QTN_FMRADIO_OPTIONS_DEACTIVATE );
                    }
                }
            else
                {
                // Delete existing menu option and add a new one
                aMenuPane->DeleteMenuItem( EFMRadioCmdDeactivateIhf );
                aMenuPane->SetItemTextL( EFMRadioCmdActivateIhf, R_QTN_FMRADIO_OPTIONS_ACTIVATE );
                }
            }
        else
            {
            aMenuPane->SetItemDimmed( EFMRadioCmdDeactivateIhf, ETrue );
            aMenuPane->SetItemDimmed( EFMRadioCmdActivateIhf, ETrue );
            }

        if ( iScannedChannels.Count() < 2 )
            {
            aMenuPane->DeleteMenuItem( EFMRadioCmdSaveAllChannels );
            }

        // if help is not enabled, disable help option
        if ( !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
            {
            aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
            }
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::LayoutChangedL
// ---------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::LayoutChangedL( TInt aType )
    {
    if( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        iContainer->HandleResourceChange( aType );
        }
    }

// ---------------------------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::SetMiddleSoftKeyLabelL
// Sets middle softkey label.
// ---------------------------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::SetMiddleSoftKeyLabelL( const TInt aResourceId,
                                                      const TInt aCommandId )
    {
    if ( !AknLayoutUtils::PenEnabled() )
        {
        CEikButtonGroupContainer* cbaGroup = Cba();
        if ( cbaGroup )
            {
            cbaGroup->RemoveCommandFromStack( KMSKControlID, iCurrentMSKCommandId );
            HBufC* middleSKText = StringLoader::LoadLC( aResourceId, iCoeEnv );
            TPtr mskPtr = middleSKText->Des();
            cbaGroup->AddCommandToStackL(
                KMSKControlID,
                aCommandId,
                mskPtr );
            CleanupStack::PopAndDestroy( middleSKText );
            iCurrentMSKCommandId = aCommandId;
            }
        }
    }

// ---------------------------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::SetMiddleSoftKeyIconL
// Sets middle softkey icon.
// ---------------------------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::SetMiddleSoftKeyIconL()
    {
    if ( !AknLayoutUtils::PenEnabled() )
        {
        CEikButtonGroupContainer* cbaGroup = Cba();
        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* mask = NULL;
        if ( cbaGroup )
            {
            TFileName mbmFileName;
            TFindFile finder( iCoeEnv->FsSession() );
            TInt err = finder.FindByDir( KFMRadioBmpFile, KDC_APP_BITMAP_DIR );
            if ( err == KErrNone )
                {
                mbmFileName.Copy( finder.File() );
                }

            AknsUtils::CreateColorIconL(
                AknsUtils::SkinInstance(),
                KAknsIIDQgnPropMskMenu,
                KAknsIIDQsnComponentColors,
                EAknsCIQsnComponentColorsCG13,
                bitmap,
                mask,
                mbmFileName,
                EMbmFmradioQgn_prop_msk_menu,
                EMbmFmradioQgn_prop_msk_menu_mask,
                KRgbBlack );

            CleanupStack::PushL( bitmap );
            CleanupStack::PushL( mask );

            cbaGroup->RemoveCommandFromStack( KMSKControlID, iCurrentMSKCommandId );
            cbaGroup->AddCommandToStackL(
                KMSKControlID,
                EAknSoftkeyContextOptions,
                *bitmap,
                *mask );

            iCurrentMSKCommandId = EAknSoftkeyContextOptions;

            CleanupStack::PopAndDestroy( 2, bitmap );
            }
        }
    }

// -----------------------------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::SaveAllChannelsL
// Saves all the scanned channels into Radio Engine's presets.
// -----------------------------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::SaveAllChannelsL()
    {
    CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( iCoeEnv->AppUi() );
    
    if ( appUi->NumberOfChannelsStored() > 0 )
        {
        //We have some tuned channels,
        //so we have to check does the user want to
        //add new channes or replace the old ones.
        TInt numberOfEmptySlots = KMaxNumberOfChannelListItems - appUi->NumberOfChannelsStored();
        if( numberOfEmptySlots >= iScannedChannels.Count() )
            {
            //more vacant than new channels
            //we can add or replace
            TInt index(0);
            CAknListQueryDialog* query = new (ELeave) CAknListQueryDialog( &index );
            query->PrepareLC( R_FMRADIO_LISTQUERY_SAVE_ALL_REPLACE_APPEND );
            if( query->RunLD() )
                {
                const TInt KIndexOfSaveAllReplaceExistingCmd = 0;
                const TInt KIndexOfSaveAllAppendCmd = 1;
                if( index == KIndexOfSaveAllReplaceExistingCmd )
                    {
                    SaveFoundChannelsToPresetsL( EReplace );
                    }
                else if( index == KIndexOfSaveAllAppendCmd )
                    {
                    SaveFoundChannelsToPresetsL( EAppend );
                    }
                }
            else
                {
                ShowToolbar( ETrue );
                }
            }
        else
            {
            //no room for all the channels
            if( numberOfEmptySlots == 0 )
                {
                CAknQueryDialog* query = CAknQueryDialog::NewL();
                if ( query->ExecuteLD( R_FMRADIO_REPLACE_EXISTING_CHANNELS_QUERY ) )
                    {
                    SaveFoundChannelsToPresetsL( EReplace );
                    }
                else
                    {
                    ShowToolbar( ETrue );
                    }
                }
            else
                {
                TInt index(0);
                CAknListQueryDialog* query = new (ELeave) CAknListQueryDialog( &index );
                query->PrepareLC( R_FMRADIO_LISTQUERY_SAVE_ALL_REPLACE_APPEND_LIST_ALMOST_FULL );
                if( query->RunLD() )
                    {
                    const TInt KIndexOfSaveAllAppendCmd = 0;
                    const TInt KIndexOfSaveAllReplaceExistingCmd = 1;
                    if( index == KIndexOfSaveAllReplaceExistingCmd )
                        {
                        SaveFoundChannelsToPresetsL( EReplace );
                        }
                    else if( index == KIndexOfSaveAllAppendCmd )
                        {
                        SaveFoundChannelsToPresetsL( EAppend );
                        }
                    }
                else
                    {
                    ShowToolbar( ETrue );
                    }   
                }
            }
        }
    else
        {
        //No previous channels
        SaveFoundChannelsToPresetsL( EReplace );
        }
    }

// -----------------------------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::ShowConfirmationQueryL
// Get confirmation from user.
// -----------------------------------------------------------------------------------------------
//
TInt CFMRadioScanLocalStationsView::ShowConfirmationQueryL(TInt aResText, TInt aResQuery) 
	{
	TInt accepted = EFalse;
    HBufC* prompt = StringLoader::LoadLC( aResText, iEikonEnv );
    CAknQueryDialog* queryDlg = CAknQueryDialog::NewL();
    accepted = queryDlg->ExecuteLD( aResQuery, *prompt );
    CleanupStack::PopAndDestroy( prompt );
    return accepted;
    }

// -----------------------------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::StopSeekL
// Invoked by AppUi when a seek is completed or aborted.
// -----------------------------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::StopSeekL()
    {
    FTRACE( FPrint( _L("CFMRadioScanLocalStationsView::StopSeekL") ) );

    if ( iTuneRequested )
        {
        iTuneRequested = EFalse;

        // if tune request comes from scan all (= ResetListAndStartScanL )
        // then start the whole scan
        if ( iScanAndSaveActivated )
            {
            AppUi()->HandleCommandL( EFMRadioCmdScanLocalStationsScan );
            }
        }
    else if ( iScanAndSaveActivated )
        {
        // No channels found or user has canceled search before any channels are found
        if ( iRadioEngine.GetTunedFrequency() == iLowerFrequencyBound &&
                iScannedChannels.Count() == 0 )
            {
            CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( iCoeEnv->AppUi() );

            if ( appUi->IsStartupWizardRunning() )
                {                
                //no stations are found in Wizard
                if (iCancelType == EFMRadioCancelScanDefault)
                    {
                    HBufC* string = NULL; 
                    string = StringLoader::LoadLC( R_QTN_FMRADIO_NO_STATIONS_FOUND, iEikonEnv );
                    CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue ); // waiting
                    note->ExecuteLD( *string );
                    CleanupStack::PopAndDestroy( string );
                    }

                iScanAndSaveActivated = EFalse;

                RemoveScanningInProgressNoteL();
                RestoreRadio( EFalse );
                appUi->AutoTuneInMainView(ETrue);
                
                ActivateMainViewL();
                }
            else
                {
                if( iContainer )
                    {
                    HBufC* listEmptyText = NULL; 
                    listEmptyText = StringLoader::LoadLC( R_QTN_FMRADIO_LIST_NO_STATIONS_FOUND, iEikonEnv );
                    iContainer->SetStationListEmptyTextL( *listEmptyText );
                    CleanupStack::PopAndDestroy( listEmptyText );
                    
                    iContainer->ResetChannelListL();
                    }

                iScanAndSaveActivated = EFalse;
                Cba()->SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_BACK ); 
                Cba()->DrawDeferred(); // Redraw softkeys
                RemoveScanningInProgressNoteL();

                UpdateToolbar();
                ShowToolbar( ETrue );
                RestoreRadio();
                }
            }
        else
            {
            AddTunedFrequencyToListL();
            }
        }
    else
        {
        RemoveScanningInProgressNoteL();
        TBool restoreFrequency = EFalse;
        TInt tunedFrequency = iRadioEngine.GetTunedFrequency();
        
        if ( tunedFrequency == iLowerFrequencyBound &&
                iScannedChannels.Count() == 0 )
            {
            restoreFrequency = ETrue;
            }
        UpdateToolbar();
        RestoreRadio( restoreFrequency );
        
        // update now playing index if the tuning is triggered from accessory 
        TInt frequencyIndex = FrequencyIndex( tunedFrequency );
        
        if ( frequencyIndex != KErrNotFound )
            {
            SetNowPlayingChannelL( frequencyIndex );
            SetLastListenedChannel( frequencyIndex );
            }
        else // frequency is not in the list, remove now playing channel status
            {
            SetNowPlayingChannelL( KErrNotFound );
            }
        }
    }

// -----------------------------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::RestoreRadio
// Restores the radio after scanning
// -----------------------------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::RestoreRadio( TBool aTuneInitialFrequency )
    {
    FTRACE( FPrint( _L("CFMRadioScanLocalStationsView::RestoreRadio") ) );
    CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( iCoeEnv->AppUi() );
    // if iInitialTunedFrequency is 0, radio has already been restored
    if ( iInitialTunedFrequency )
        {
        if ( aTuneInitialFrequency )
            {
            iTuneRequested = ETrue;
            iRadioEngine.Tune( iInitialTunedFrequency );
            }
        //unmute radio
        iRadioEngine.SetMuteOn( EFalse );
        iInitialTunedFrequency = 0;
        }
    appUi->SetStartupWizardRunning( EFalse );
    }

// -----------------------------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::AddTunedFrequencyToListL
// Updates found channels to the list.
// -----------------------------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::AddTunedFrequencyToListL()
    {
    FTRACE( FPrint( _L("CFMRadioScanLocalStationsView::AddTunedFrequencyToListL") ) );
    TInt tunedFrequency = iRadioEngine.GetTunedFrequency();
    
    if ( KErrNotFound == FrequencyIndex( tunedFrequency ) )
        {
        // the frequency is new and may be added
        CFMRadioPreset* channel = CFMRadioPreset::NewL();
        CleanupStack::PushL( channel );
        channel->SetPresetFrequency( tunedFrequency );
        // Make the name invalid by default so that possibly dynamic PS name won't get saved
        channel->SetPresetNameValid( EFalse );
        
        TInt freqIndex = SortedFrequencyListIndex( tunedFrequency );
        
        if ( freqIndex < ( iScannedChannels.Count() - 1 ) )
            {
            iScannedChannels.Insert( channel, freqIndex );
            }
        else
            {
            iScannedChannels.AppendL( channel );
            }
        CleanupStack::Pop( channel );
        
        // update a radiostation slot in the list
        // check if there is place
        if ( iContainer )
            {
            iContainer->InsertScannedChannelToListL( freqIndex, tunedFrequency );
            iContainer->DrawDeferred();
            }
        // scan for another frequency
        AppUi()->HandleCommandL( EFMRadioCmdScanLocalStationsScan );
        }
    else
        {
        //stop scanning
        iScanAndSaveActivated = EFalse;
        Cba()->SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_BACK ); 
        Cba()->DrawDeferred(); // Redraw softkeys

        RemoveScanningInProgressNoteL();
        SetMiddleSoftKeyIconL();
        SetContextMenu( R_FMRADIO_SCAN_STATIONS_CTX_MENUBAR_SAVE );

        if ( iContainer )
            {
            iContainer->UpdateLastListenedChannel( 0 ); // select the first channel
            PlayCurrentlySelectedChannelL();
            iContainer->DrawDeferred();
            }

        //unmute radio
        iRadioEngine.SetMuteOn( EFalse );

        CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( iCoeEnv->AppUi() );

        if ( appUi->IsStartupWizardRunning() )
            {
            appUi->SetStartupWizardRunning( EFalse );

            if ( iScanCancelled )
                {
                iScanCancelled = EFalse;
                ActivateMainViewL();
                }
            else
                {
                SaveAllChannelsL();
                }
            }
        else
            {
            UpdateToolbar();
            ShowToolbar( ETrue );
            }
        }
    }

// -----------------------------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::ResetCountersL
// Called when a new scan is activated.
// -----------------------------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::ResetListAndStartScanL()
    {
    User::LeaveIfNull( iContainer );
    iNowPlayingIndex = KErrNotFound;
    ShowToolbar( EFalse );
    iContainer->SetStationListEmptyTextL( KNullDesC );
    iScanAndSaveActivated = ETrue;	
    DisplayScanningInProgressNoteL();

    iInitialTunedFrequency = 0;
    iScanCancelled = EFalse;
    iCancelType = EFMRadioCancelScanDefault;
    iScannedChannels.ResetAndDestroy();
    iContainer->InitializeChannelListL();
    
    //mute radio
    if ( !iRadioEngine.IsMuteOn() )
        {
        iRadioEngine.SetMuteOn( ETrue );
        }

    //update upper and lower frequency bands
    iRadioEngine.GetFrequencyBandRange( iLowerFrequencyBound, iUpperFrequencyBound );

    iInitialTunedFrequency = iRadioEngine.GetTunedFrequency();

    if ( iRadioEngine.GetTunedFrequency() != iLowerFrequencyBound )
        {
        iTuneRequested = ETrue;
        iRadioEngine.Tune( iLowerFrequencyBound );
        }
    else
        {
        AppUi()->HandleCommandL( EFMRadioCmdScanLocalStationsScan );
        }
    }

// ---------------------------------------------------------
// CFMRadioScanLocalStationsContainer::DisplayScanningInProgressNoteL
// Display a wait note while the channel fill is occuring.
// ---------------------------------------------------------
//
void CFMRadioScanLocalStationsView::DisplayScanningInProgressNoteL()
	{
	iScanningNote = new (ELeave) CAknWaitDialog(REINTERPRET_CAST(CEikDialog**, &iScanningNote));
	iScanningNote->SetCallback( this );
	
	CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( iCoeEnv->AppUi() );
	
	if ( appUi->IsStartupWizardRunning() )
		{
		iScanningNote->PrepareLC( R_FMRADIO_SEARCH_STATIONS_WAIT_CANCEL_NOTE );
		}
	else
		{
		iScanningNote->PrepareLC( R_FMRADIO_SEARCH_STATIONS_WAIT_NOTE );
		}
	
    iScanningNote->RunLD();
	}	
	
// ---------------------------------------------------------
// CFMRadioScanLocalStationsContainer::RemoveScanningInProgressNoteL
// Remove and destroy the scanning fill note.
// ---------------------------------------------------------
//
void CFMRadioScanLocalStationsView::RemoveScanningInProgressNoteL()
    {
    FTRACE(FPrint(_L("CFMRadioScanLocalStationsView::RemoveScanningInProgressNoteL")));
    if ( iScanningNote )
        {
        iScanningNote->ProcessFinishedL(); // Inform seek note to stop and hide it        
        iScanningNote = NULL;
        }
    }

// ---------------------------------------------------------
// CFMRadioScanLocalStationsContainer::DialogDismissedL
// Called when user cancels waiting note or process finishes
// ---------------------------------------------------------
//	
void CFMRadioScanLocalStationsView::DialogDismissedL( TInt  aButtonId ) 	
	{
	FTRACE(FPrint(_L("CFMRadioScanLocalStationsView::DialogDismissedL")));	
	iScanningNote = NULL;
	if ( aButtonId == EAknSoftkeyCancel )
   		{
      	// cancel any process in here    	
   		/*Dialog get this button ID when dismissed, if headset is pulled off
   		we don't want to change the cancel type*/
   		if( iCancelType != EFMRadioCancelScanByHeadsetDisconnect)
   			{
   	   		SetScanCanceled(EFMRadioCancelScanBySoftkey);
   			}
   		iRadioEngine.CancelScanLocalStationsScan(); 
  		}
	}

void CFMRadioScanLocalStationsView::SetScanCanceled( TFMRadioCancelScanType aType )
	{
	CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( iCoeEnv->AppUi() );
	
	if ( appUi->IsStartupWizardRunning() )
	    {
	    FTRACE(FPrint(_L("CFMRadioScanLocalStationsView::SetScanCanceled")));	
	    iScanCancelled = ETrue;
	    iCancelType = aType;
	    }
	else
		{
		iScanCancelled = EFalse;
		iCancelType = EFMRadioCancelScanDefault;
		}
	}

TFMRadioCancelScanType CFMRadioScanLocalStationsView::CancelType()
	{
	return iCancelType;
	}

// ---------------------------------------------------------
// CFMRadioScanLocalStationsContainer::SaveFoundChannelsToPresets
// Save scanned channels to presets 
// ---------------------------------------------------------
//	
void CFMRadioScanLocalStationsView::SaveFoundChannelsToPresetsL( TSavingMode aSavingMode )
	{
	FTRACE(FPrint(_L("CFMRadioScanLocalStationsView::SaveFoundChannelsToPresetsL Mode %d"), aSavingMode ) );
	CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( iCoeEnv->AppUi() );
	
	//store as much as the list is keeping
	TInt limit;	
	TInt chCount = iScannedChannels.Count();
	
	if ( chCount >= KMaxNumberOfChannelListItems )
		{
		limit = KMaxNumberOfChannelListItems;
		}
	else
		{
		limit = chCount;
		}
	
	TInt savedChannelCount = 0;	
	
    if ( aSavingMode == EAppend ) 
        {
        /** Append found channels to preset list */		
        for ( TInt freqIx = 0; KMaxNumberOfChannelListItems > iObserver.Channels().Count() && 
            chCount > freqIx; freqIx++ )
            {
            const TDesC& name = iScannedChannels[ freqIx ]->PresetNameValid() ? 
                    iScannedChannels[ freqIx ]->PresetName() : KNullDesC;

		    iObserver.AddChannelToListL( name,
                                         iScannedChannels[ freqIx ]->PresetFrequency() );
						
			savedChannelCount++;

			}
		
		iObserver.UpdateChannelsL( EStoreAllToRepository, 0, 0 );

		}
	else // Replace
		{
		iObserver.UpdateChannelsL( ERemoveAllFromRepository, 0, 0  );
		
		for ( TInt index = 0; index < limit; index++ )
			{
            const TDesC& name = iScannedChannels[ index ]->PresetNameValid() ? 
                    iScannedChannels[ index ]->PresetName() : KNullDesC;
			iObserver.AddChannelToListL( name,
                                         iScannedChannels[ index ]->PresetFrequency() );
			savedChannelCount++;
			}
		iObserver.UpdateChannelsL( EStoreAllToRepository, 0, 0 );
		}
	
    // display note about how many many channels were saved to the preset list
    HBufC* string = NULL;                    
    string = StringLoader::LoadLC( R_QTN_FMRADIO_STATIONS_SAVED, savedChannelCount, iEikonEnv );        
    CAknConfirmationNote* note = new ( ELeave ) CAknConfirmationNote( ETrue ); // waiting
    note->ExecuteLD( *string );
    CleanupStack::PopAndDestroy( string );
    
    ActivateMainViewL();
    }

// ---------------------------------------------------------
// CFMRadioScanLocalStationsView::PlayCurrentlySelectedChannel
// Play currently selected channel from scan local stations list
// ---------------------------------------------------------
//
void CFMRadioScanLocalStationsView::PlayCurrentlySelectedChannelL()
    {
    FTRACE(FPrint(_L("CFMRadioScanLocalStationsView::PlayCurrentlySelectedChannelL")));
    
    TInt selectedChannel = CurrentlySelectedChannel();    
    if ( ChannelInUse( selectedChannel ) )
        {
        TInt selectedFrequency = iScannedChannels[ selectedChannel ]->PresetFrequency();
        
        if ( iRadioEngine.GetTunedFrequency() != selectedFrequency )
            {
            CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( iCoeEnv->AppUi() );
            iTuneRequested = ETrue;
            SetNowPlayingChannelL( selectedChannel );
            SetLastListenedChannel( selectedChannel );
            appUi->TuneL( selectedFrequency );
            }
        else
            {
            SetNowPlayingChannelL( selectedChannel );
            SetLastListenedChannel( selectedChannel );
            }
        }
    }

// ---------------------------------------------------------
// CFMRadioScanLocalStationsView::ShowToolbar
// Sets toolbar visibility.
// ---------------------------------------------------------
//
void CFMRadioScanLocalStationsView::ShowToolbar( TBool aVisible )
	{
	if ( iToolbar && AknLayoutUtils::PenEnabled() )
		{						
		iToolbar->SetToolbarVisibility( aVisible );
		iToolbar->UpdateBackground();
		iToolbar->DrawDeferred();			
		// just to make sure container has the right rect when toolbar is displayed
		if ( aVisible && iContainer )
			{
			iContainer->SetRect( ClientRect() );	
			}		
		}
	}
		
// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::RdsDataProgrammeService
// ---------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::RdsDataProgrammeService( const TDesC& aProgramService )
    {
    FTRACE(FPrint(_L("CFMRadioScanLocalStationsView::RdsDataProgrammeService")));
    if ( iContainer && !iTuneRequested && aProgramService.Length() > 0 )
        {
        TInt tunedChannel = CurrentlyPlayingChannel();
        
        TRAP_IGNORE
            (
            if ( ChannelInUse( tunedChannel ) )
                {
                iContainer->UpdateChannelListContentL( tunedChannel,
                        aProgramService,
                        iScannedChannels[ tunedChannel ]->PresetFrequency(),
                        ETrue,
                        ETrue );

                // update PS name also to the list containing the scanned preset values
                iScannedChannels[ tunedChannel ]->SetPresetNameL( aProgramService );
                iContainer->DrawDeferred();
                }
            )
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::RdsDataPsNameIsStatic
// ---------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::RdsDataPsNameIsStatic( TBool aStatic )
    {
    TInt tunedChannel = CurrentlyPlayingChannel();
    if( ChannelInUse( tunedChannel ) )
        {
        iScannedChannels[ tunedChannel ]->SetPresetNameValid( aStatic );
        }
    }

// ---------------------------------------------------------
// CFMRadioScanLocalStationsView::RdsDataRadioText
// ---------------------------------------------------------
//
void CFMRadioScanLocalStationsView::RdsDataRadioText( const TDesC&/* aRadioText */)
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::RdsAfSearchBegin
// ---------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::RdsAfSearchBegin()
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::RdsAfSearchEnd
// ---------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::RdsAfSearchEnd( TUint32 /*aFrequency*/, TInt /*aError*/ )
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::RdsAfSearchStateChange
// ---------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::RdsAfSearchStateChange( TBool /*aEnabled*/ )
    { 
    }

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::RdsAvailable
// ---------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::RdsAvailable( TBool /*aAvailable*/ )
    {
    }

// --------------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::PrepareToolbar
// Prepare toolbar for displaying basic radio command buttons
// --------------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::PrepareToolbar()
	{
    if ( AknLayoutUtils::PenEnabled() )
        {
        iToolbar = Toolbar();   	
        iToolbar->SetToolbarVisibility( EFalse );
        // Set observer
        iToolbar->SetToolbarObserver( this );
        }
    }
	
// --------------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::OfferToolbarEventL
// Handle commands from toolbar.
// --------------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::OfferToolbarEventL( TInt aCommandId )
    {
    switch ( aCommandId )
        {
        case EFMRadioToolbarButtonCmdSaveAll:
            {
            HandleCommandL( EFMRadioCmdSaveAllChannels );
            break;
            }
        case EFMRadioToolbarButtonCmdScan:
            {
            HandleCommandL( EFMRadioCmdScanLocalStationsScan );
            break;
            }
        case EFMRadioToolbarButtonCmdToggleIhf:
            {
            if ( iRadioEngine.GetAudioOutput() == CRadioEngine::EFMRadioOutputHeadset )
                {
                AppUi()->HandleCommandL( EFMRadioCmdActivateIhf );
                }
            else
                {
                AppUi()->HandleCommandL( EFMRadioCmdDeactivateIhf );
                }
            break;
            }
        default:
            {
            break;
            }
        }
    }

// --------------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::SetToolbarDimmedState
// set toolbar buttons dimmed state
// --------------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::SetToolbarDimmedState( const TBool aState )
	{
    if ( iToolbar && AknLayoutUtils::PenEnabled() )	
        {
        iToolbar->SetDimmed( aState );
        if ( !aState )
            {
            UpdateToolbar();
            }
        iToolbar->DrawDeferred();
        }
    }

// --------------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::UpdateToolbar
// Set save buttons dimming state
// --------------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::UpdateToolbar()
    {
    if ( iContainer && iToolbar && AknLayoutUtils::PenEnabled() )
        {
        if ( iRadioEngine.IsAudioRoutingPossible() )
            {
                if ( iRadioEngine.IsHeadsetConnected() )
                    {
                    CAknButton* centerButton = NULL;    
                    centerButton = static_cast<CAknButton*>( iToolbar->ControlOrNull( EFMRadioToolbarButtonCmdToggleIhf ) );    

                    iToolbar->SetItemDimmed( EFMRadioToolbarButtonCmdToggleIhf, EFalse, ETrue );
                    
                    if ( iRadioEngine.GetAudioOutput() == CRadioEngine::EFMRadioOutputIHF )
                        {
                        if ( centerButton )
                            {
                            centerButton->SetCurrentState( 1, ETrue );
                            }
                        }
                    else
                        {
                        if ( centerButton )
                            {
                            centerButton->SetCurrentState( 0, ETrue );
                            }
                        }
                    }
                else
                    {
                    iToolbar->SetItemDimmed( EFMRadioToolbarButtonCmdToggleIhf, ETrue, ETrue );
                    }
                }
            else
                {
                iToolbar->SetItemDimmed( EFMRadioToolbarButtonCmdToggleIhf, ETrue, ETrue );
                }

        TInt chCount = iScannedChannels.Count();
        if ( chCount == 0 )
            { //dim save buttons
            iToolbar->SetItemDimmed( EFMRadioToolbarButtonCmdSaveAll, ETrue, ETrue );
            }
        else if ( chCount == 1 )
            { //dim 'save all' button.
            iToolbar->SetItemDimmed( EFMRadioToolbarButtonCmdSaveAll, ETrue, ETrue );
            }
            else
            { //undim both
            iToolbar->SetItemDimmed( EFMRadioToolbarButtonCmdSaveAll, EFalse, ETrue );
            }
        }
    }

// --------------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::IsFrequencyScanned
// check if the frequency is already found
// --------------------------------------------------------------------------------
//
TInt CFMRadioScanLocalStationsView::FrequencyIndex( TInt aFrequency )
    {
    TInt frequencyIndex = KErrNotFound;
    TInt chCount = iScannedChannels.Count();
    
    for ( TInt i = 0; i < chCount; i++ )
        {
        if ( aFrequency == iScannedChannels[ i ]->PresetFrequency() )
            {
            frequencyIndex = i;
            break;
            }
        }
    return frequencyIndex;
    }

// --------------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::HandleOneChannelSaveL
// Handle single channel save
// --------------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::HandleOneChannelSaveL()
    {    
    TInt currentPresetCount = iObserver.Channels().Count();
    TInt chIndex = iContainer->CurrentlySelectedChannel();
    TBool continueWithSave = ETrue;
    
    if ( !ChannelInUse( chIndex ) )
        {
        User::Leave( KErrNotFound );
        }
    // check if the channel list is full and show replace query if necessary
    if ( currentPresetCount == KMaxNumberOfChannelListItems )
        {
        CAknQueryDialog* query = CAknQueryDialog::NewL();
        if ( query->ExecuteLD( R_FMRADIO_REPLACE_EXISTING_CHANNELS_QUERY ) )
            {
            iObserver.UpdateChannelsL( ERemoveAllFromRepository, 0, 0  );
            }
        else
            {
            continueWithSave = EFalse;
            }
        }
    if ( continueWithSave )
        {
        const TDesC& name = iScannedChannels[ chIndex ]->PresetNameValid() ? 
                iScannedChannels[ chIndex ]->PresetName() : KNullDesC;
        TInt presetFrequency = iScannedChannels[ chIndex ]->PresetFrequency();
        iObserver.AddChannelToListL( name, 
                                     presetFrequency );
        iObserver.UpdateChannelsL( EStoreAllToRepository, 0, 0 );

        TBool updateNowPlayingIcon = EFalse;
        TInt tunedIndex = CurrentlyPlayingChannel();
        // update now playing icon and list index
        if ( chIndex == tunedIndex )
            {
            // channel is going be deleted so now playing index is not in the list
            iNowPlayingIndex = KErrNotFound;
            }
        else if ( chIndex < tunedIndex )
            {
            iNowPlayingIndex = tunedIndex - 1;
            iContainer->HideNowPlayingIconL( tunedIndex, ETrue );
            updateNowPlayingIcon = ETrue;
            }
         else
            {
            // NOP
            }
        
        // remove also from the UI list
        iContainer->RemoveChannelListContentL( chIndex );
        
        if ( updateNowPlayingIcon )
            {
            iContainer->UpdateNowPlayingIconL( tunedIndex - 1 );
            }
        // received PS name is shown                                
        if ( name.Length() > 0 )
            {
            HBufC* nameSavedString = StringLoader::LoadLC( R_QTN_FMRADIO_SAVING_DONE_ONE, name, iEikonEnv );
            CAknConfirmationNote* saveInfoNote = new ( ELeave ) CAknConfirmationNote( ETrue ); // waiting
            saveInfoNote->ExecuteLD( *nameSavedString );
            CleanupStack::PopAndDestroy( nameSavedString );  
            }
        else // show frequency
            {
            TReal frequency = static_cast<TReal>( presetFrequency / static_cast<TReal>( KHzConversionFactor ) );
            TRealFormat format( KFrequencyMaxLength, iRadioEngine.DecimalCount() );
            TBuf<KFrequencyMaxLength> frequencyString;
            frequencyString.Num( frequency, format );
            AknTextUtils::LanguageSpecificNumberConversion( frequencyString );

            HBufC* freqSavedString = StringLoader::LoadLC( R_QTN_FMRADIO_SAVING_DONE_ONE_NO_NAME, frequencyString, iEikonEnv );
            CAknConfirmationNote* saveInfoNote = new ( ELeave ) CAknConfirmationNote( ETrue ); // waiting
            saveInfoNote->ExecuteLD( *freqSavedString );
            CleanupStack::PopAndDestroy( freqSavedString );
            }
        // channel info is removed after save
        delete iScannedChannels[ chIndex ];
        iScannedChannels.Remove( chIndex );
        iScannedChannels.Compress();
        }
    UpdateToolbar();
    }

// --------------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::ActivateMainViewL
// Activates main view
// --------------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::ActivateMainViewL()
    {
    CAknViewAppUi* appUi = AppUi();
    
    if ( !appUi->IsForeground() )
        {
        // don't bring radio automatically to the foreground
        // if the view is about to change
        // and we are still in the background
        appUi->SetCustomControl( 1 );
        appUi->ActivateLocalViewL( KFMRadioMainViewId );
        appUi->SetCustomControl( 0 );
        }
    else
        {
        appUi->ActivateLocalViewL( KFMRadioMainViewId );
        }
    }

// --------------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::SetContextMenu
// --------------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::SetContextMenu( TInt aMenuTitleResourceId )
    {
    if ( !AknLayoutUtils::PenEnabled() )
        {
        CEikMenuBar* menubar = MenuBar();
        if ( menubar )
            {
            menubar->SetContextMenuTitleResourceId( aMenuTitleResourceId );
            menubar->SetMenuType( CEikMenuBar::EMenuContext );
            }
        }
    }

// --------------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::HandleScanListRemConEventL
// --------------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsView::HandleScanListRemConEventL( TAccessoryEvent aEvent )
    {
    TInt scannedChannelCount = iScannedChannels.Count();

    if ( scannedChannelCount >= 1 )
        {
        TInt selectedChannelIndex = CurrentlySelectedChannel();
        TInt nextChannellIndex = 0;
        
        if ( aEvent == EForward )
            {
            nextChannellIndex = selectedChannelIndex + 1;
            
            if ( nextChannellIndex >= scannedChannelCount )
                {
                // go around the the list to the first channnel
                nextChannellIndex = 0;
                }
            }
        else if ( aEvent == ERewind )
            {
            nextChannellIndex = selectedChannelIndex - 1;
        
            if ( nextChannellIndex < KMinNumberOfChannelListItems )
                {
                // jump to the last item
                nextChannellIndex = scannedChannelCount - 1;
                }
            }
        // check that index is really a valid one
        if ( ChannelInUse( nextChannellIndex ) )
            {
            TInt nextChannelFrequency = iScannedChannels[ nextChannellIndex ]->PresetFrequency();
            // tune if necessary
            if ( iRadioEngine.GetTunedFrequency() != nextChannelFrequency )
                {
                CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( iCoeEnv->AppUi() );
                appUi->TuneL( nextChannelFrequency );
                }
            }
        }
    }

// --------------------------------------------------------------------------------
// CFMRadioScanLocalStationsView::SortedFrequencyListIndex
// --------------------------------------------------------------------------------
//
TInt CFMRadioScanLocalStationsView::SortedFrequencyListIndex( TInt aFrequency )
    {
    TInt channelCount = iScannedChannels.Count();
    TInt sortedIndex = 0;
    
    for ( TInt i = 0; i < channelCount; i++ )
        {
        TInt presetFreq = iScannedChannels[i]->PresetFrequency();
        
        if ( aFrequency > presetFreq )
            {
            sortedIndex = i + 1;
            }
        else if ( aFrequency < presetFreq )
            {
            sortedIndex = i;
            break;
            }        
        }
    return sortedIndex;
    }

// End of File
