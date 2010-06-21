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
* Description:  Implementation of the class CFMRadioChannelListView
*
*/


// INCLUDE FILES
#include <featmgr.h>
#include <aknViewAppUi.h>
#include <akntitle.h>
#include <avkon.hrh>
#include <avkon.rsg>
#include <StringLoader.h>
#include <eikmenup.h>
#include <eikbtgpc.h>
#include <fmradio.rsg>
#include <fmradio.mbg>
#include <fmradiouids.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h>
#include <data_caging_path_literals.hrh>
#include <aknbutton.h>
#include <akntoolbar.h>

#include "fmradioengine.h"
#include "fmradio.hrh"
#include "fmradiochannellistview.h"
#include "fmradiochannellistcontainer.h"
#include "debug.h"
#include "fmradiordsreceiverbase.h"
#include "fmradiopubsub.h"
#include "fmradiofrequencyquerydialog.h"
#include "fmradioappui.h"

const TInt KMSKControlID(3);

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CFMRadioChannelListView::NewL
// Two-phase constructor of CFMRadioChannelListView
// ---------------------------------------------------------------------------
//
CFMRadioChannelListView* CFMRadioChannelListView::NewL(
    CRadioEngine* aRadioEngine, MChannelListHandler& aObserver )
    {
    CFMRadioChannelListView* self = new ( ELeave ) CFMRadioChannelListView( aObserver );
    CleanupStack::PushL(self);
    self->ConstructL(aRadioEngine);
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CFMRadioChannelListView::CFMRadioChannelListView
//
// ---------------------------------------------------------------------------
//
CFMRadioChannelListView::CFMRadioChannelListView( MChannelListHandler& aObserver ) :
    iNowPlayingIndex( KErrNotFound ),
	iObserver( aObserver )	
	{
	}
// ---------------------------------------------------------------------------
// CFMRadioChannelListView::ConstructL
// EPOC two-phased constructor
// ---------------------------------------------------------------------------
//
void CFMRadioChannelListView::ConstructL(
    CRadioEngine* aRadioEngine)
    {    
    iRadioEngine = aRadioEngine;
    BaseConstructL( R_FMRADIO_CH_LIST_VIEW );
    PrepareToolbar();
    
    if ( !AknLayoutUtils::PenEnabled() )
        {
        iToolbar->SetToolbarVisibility( EFalse );
        }
       
    //called here to ensure that empty channels are named in the way we want them to be.
    InitializeChannelsL();
    }

// ---------------------------------------------------------------------------
// CFMRadioChannelListView::~CFMRadioChannelListView
// Class destructor
// ---------------------------------------------------------------------------
//
CFMRadioChannelListView::~CFMRadioChannelListView()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }
    delete iContainer;
    }

// ---------------------------------------------------------------------------
// CFMRadioChannelListView::Id
// Return the unique identifier corresponding to this view
// ---------------------------------------------------------------------------
//
TUid CFMRadioChannelListView::Id() const
    {
    return KFMRadioChannelListViewId;
    }

// ---------------------------------------------------------------------------
// CFMRadioChannelListView::DoActivateL
// Activate the channel list view
// ---------------------------------------------------------------------------
//
void CFMRadioChannelListView::DoActivateL(
    const TVwsViewId& /*aPrevViewId*/,
    TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/ )
    {
    FTRACE( FPrint( _L("CFMRadioChannelListView::DoActivateL  Start") ) );

    iRadioEngine->RdsReceiver().AddObserver(this);
    
    iFirstMoveDone = ETrue;

    SetTitleL( R_FMRADIO_STATIONS_VIEW_TITLE );

    if ( !iContainer )
        {
        FTRACE( FPrint( _L("CFMRadioChannelListView::DoActivateL  inside second, not iContainer if") ) );
        iContainer = CFMRadioChannelListContainer::NewL( ClientRect(), *iRadioEngine, iObserver );
        iContainer->SetMopParent( this );
        iContainer->SetObserver( this );
        AppUi()->AddToStackL( *this, iContainer );
        }
    
    InitializeChannelsL();
    
    FadeAndShowExit( iFaded );
    
    AppUi()->HandleCommandL( EFMRadioCmdChannelListViewActive );
    
    UpdateMiddleSoftKeyL();
    }

// ---------------------------------------------------------------------------
// CFMRadioChannelListView::HandleCommandL
// Interprets view's menu,softkey and other commands and acts
// accordingly by calling the appropriate command handler
// function for further action.
// ---------------------------------------------------------------------------
//
void CFMRadioChannelListView::HandleCommandL( TInt aCommand )
    {
    CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( AppUi() );
    switch ( aCommand )
        {
    	case EFMRadioCmdMove:
        	{
            MoveCurrentItemIndexL();
        	break;
        	}
    	case EFMRadioCmdDone:
    		{
    		iMoveMode = EFalse;
    		iFirstMoveDone = ETrue;
    		Cba()->SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_BACK );
    		
    		UpdateMiddleSoftKeyL();

    		iContainer->MoveDoneL();
            TInt currentIndex = 0;
            currentIndex = appUi->MatchingChannelL( iRadioEngine->GetTunedFrequency() );
            if ( currentIndex != KErrNotFound )
                {
                iRadioEngine->SetCurrentPresetIndex( currentIndex );
                SetNowPlayingChannel( currentIndex );
                }
            ShowToolbar( ETrue );
    		break;
    		}
    	case EFMRadioCmdDrop:
    		{
    		iFirstMoveDone = EFalse;
    		Cba()->SetCommandSetL( R_FMRADIO_SOFTKEYS_GRAP_DONE );    		    		
    		iContainer->HandleDropL();
    		break;
    		}
    	case EFMRadioCmdGrap:
    		{
        	if ( iFirstMoveDone )
        		{
                Cba()->SetCommandSetL( R_FMRADIO_SOFTKEYS_DROP_CANCEL );
        		}
        	else
        		{
                Cba()->SetCommandSetL( R_FMRADIO_SOFTKEYS_DROP_DONE );
        		}
        	iContainer->ActivateMoveL();
        	break;
    		}
        case EAknSoftkeyBack:   // Go back to the main view
            {
            appUi->ActivateLocalViewL( KFMRadioMainViewId );
            break;
            }
        case EEikBidCancel:
            {
            CancelMoveL();
            break;
            }
        case EAknSoftkeySelect: // Save a channel to the highlighted slot
            {
            break;
            }
        case EFMRadioCmdTuneManually:
        	{
        	// If manual tuning is canceled, return to previously tuned frequency. 	
			TUint32 prevFreq = iRadioEngine->GetTunedFrequency() / KFMRadioFreqMultiplier;
			TUint32 retFreq = prevFreq;
			
			iManualQueryDialog = CFMRadioFrequencyQueryDialog::NewL( retFreq );            
            iManualQueryDialog->SetObserver( this );
			
			TInt res = iManualQueryDialog->ExecuteLD( R_FMRADIO_MANUAL_TUNING_QUERY );

			if ( res )
				{				
                if ( iObserver.Channels().Count() < KMaxNumberOfChannelListItems )
					{
					TInt usefreq = static_cast<TInt>( retFreq * KFMRadioFreqMultiplier );
					iRadioEngine->Tune( usefreq );
					TBuf<KPresetNameLength> stationName;
					stationName.SetLength( 0 );
				
					iObserver.AddChannelToListL( stationName, usefreq );
					
					iObserver.UpdateChannelsL( EStoreAllToRepository, 0, 0 );
					
					iContainer->AddChannelL( stationName, usefreq, ETrue );	
					
                    iRadioEngine->TunePresetL( iObserver.Channels().Count() - 1 );
					
					TInt index = iRadioEngine->GetPresetIndex();
					
					iContainer->UpdateItemIconL( index, KNowPlayingIconIndexChList );
					
                    iContainer->UpdateLastListenedChannel( index ); // Update index
					}
				else
					{
					// Show icon, or overwrite last item, or selected item?
					CAknInformationNote* infonote = new( ELeave ) CAknInformationNote();
			    	
				    HBufC* noteText = StringLoader::LoadLC( 
				    		R_QTN_FMRADIO_LIST_QUERY_LIST_FULL, iCoeEnv );
				    
				    
				    infonote->ExecuteLD( *noteText );
				    	   
				    CleanupStack::PopAndDestroy( noteText );					
					}					
				}
			else //cancel pressed, return to previous frequency
				{
				iRadioEngine->Tune( static_cast<TInt>( prevFreq * KFMRadioFreqMultiplier ) );				
				}
        	break;
        	}
        case EFMRadioCmdListenCh:
            {
            if ( !AknLayoutUtils::PenEnabled() )
                {
                appUi->HandleCommandL( aCommand );
                // update msk here
                UpdateMiddleSoftKeyL();
                }
            if ( iMoveMode )
                {
                MoveActionDoneL();
                }
            break;
            }
        case EFMRadioToolbarButtonCmdToggleIhf:
            {
            if ( iRadioEngine->GetAudioOutput() == CRadioEngine::EFMRadioOutputHeadset )
                {
                appUi->HandleCommandL( EFMRadioCmdActivateIhf );
                }
            else
                {
                appUi->HandleCommandL( EFMRadioCmdDeactivateIhf );
                }
            break;
            }
        case EFMRadioCmdErase:
            {
            if ( iObserver.Channels().Count() > 0 )
                {
                TInt selectedIndex = CurrentlySelectedChannel();
                
                if ( appUi->EraseChannelL( selectedIndex ) )
                    {
                    if ( iContainer )
                        {
                        iContainer->RemoveChannelL( selectedIndex );
                        }
                    // Dim the rename and delete buttons in case the list is now empty
                    UpdateToolbar();
                    }
                }
            break;
            }
        default: // Send all other commands to AppUi
        	{        		        	
        	appUi->HandleCommandL( aCommand );
            break;
        	}
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioChannelListView::SetLastListenedChannel
// Keep track of the last selected channel
// ---------------------------------------------------------------------------
//
void CFMRadioChannelListView::SetLastListenedChannel( TInt aIndex )
    {
    iChIndex = aIndex;
    if ( iContainer && !iMoveMode )
        {
        iContainer->UpdateLastListenedChannel( aIndex ); // Update index
        }
    }
    
// ---------------------------------------------------------------------------
// CFMRadioChannelListView::SetNowPlayingChannel
// Keep track of the current selected channel
// ---------------------------------------------------------------------------
//
void CFMRadioChannelListView::SetNowPlayingChannel( TInt aChannelIndex ) 
    {
    iNowPlayingIndex = aChannelIndex;
    
    if ( iContainer && !iMoveMode )
        {
        TRAP_IGNORE( iContainer->UpdateItemIconL( aChannelIndex, KNowPlayingIconIndexChList ) )
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioChannelListView::CurrentlyPlayingChannel
// Returns the index of the playing channel item from the channel list.
// ---------------------------------------------------------------------------
//
TInt CFMRadioChannelListView::CurrentlyPlayingChannel()
	{
	return iNowPlayingIndex;
	}

// ---------------------------------------------------------------------------
// CFMRadioChannelListView::InitializeChannelsL
// Initialize the channels with the values currently saved in
// the radio engine
// ---------------------------------------------------------------------------
//
void CFMRadioChannelListView::InitializeChannelsL()
    {
    FTRACE( FPrint( _L("CFMRadioChannelListView::InitializeChannelsL  Start") ) );
    
    if ( iContainer )
        {
        TBool nowPlaying = EFalse;
        
        RPointerArray<CFMRadioPreset>& channels = iObserver.Channels();        
        TInt channelCount = channels.Count();
        
        for ( TInt channelIndex = 0; channelCount > channelIndex; channelIndex++ )
            {
            FTRACE( FPrint( _L("CFMRadioChannelListView::InitializeChannelsL  inside Channel in use") ) );

            iContainer->AddChannelL( channels[ channelIndex ]->PresetName(),
                   channels[ channelIndex ]->PresetFrequency(),
                   nowPlaying );
            
            UpdateMiddleSoftKeyL();
            FTRACE( FPrint( _L("CFMRadioChannelListView::InitializeChannelsL  inside Channel in use Channel = %d"), iRadioEngine->GetPresetFrequencyL( channelIndex )) );
            }
        
        CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( AppUi() );
        
        TInt currentPresetIndex = iRadioEngine->GetPresetIndex();
        TInt chIndexForTunedFreq = appUi->MatchingChannelL( iRadioEngine->GetTunedFrequency() );
        
        if ( iRadioEngine->GetRadioMode() == CRadioEngine::ERadioPresetMode &&
                currentPresetIndex < channelCount &&
                currentPresetIndex >= 0 )
            {
            SetLastListenedChannel( currentPresetIndex );
            SetNowPlayingChannel( currentPresetIndex );
            }
        else if ( chIndexForTunedFreq != KErrNotFound )
            {
            SetLastListenedChannel( chIndexForTunedFreq );
            SetNowPlayingChannel( chIndexForTunedFreq );
            }
        else
            {
            SetLastListenedChannel( 0 );
            }
        }
    FTRACE(FPrint(_L("InitializeChannelsL complete")));
    }

// ---------------------------------------------------------------------------
// CFMRadioChannelListView::ChannelInPlay
// Return true/false to indicate whether the channel specified
// is in play
// ---------------------------------------------------------------------------
//
TBool CFMRadioChannelListView::ChannelInPlay(
    TInt aChannelIndex ) const
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
// CFMRadioChannelListView::FadeAndShowExit
// Fade the view and all controls associated with it. Change
// the soft keys so that "Exit" is displayed
// ---------------------------------------------------------------------------
//
void CFMRadioChannelListView::FadeAndShowExit( TBool aFaded )
    {
    iFaded = aFaded;
    if( iContainer && iToolbar )
        {
        if ( aFaded ) // Fade
            {
            // In case options menu is open when headset is disconnected
            StopDisplayingMenuBar();
            iContainer->SetFaded( aFaded );
            iToolbar->SetDimmed( ETrue );
            TRAP_IGNORE( Cba()->SetCommandSetL( R_FMRADIO_SOFTKEYS_EXIT ) );
            }
        else
            {
            iContainer->SetFaded(aFaded);
            iToolbar->SetDimmed( EFalse );
            UpdateToolbar();
            
            if ( iMoveMode )
                {
                TRAP_IGNORE
                    (
                    Cba()->SetCommandSetL( R_FMRADIO_SOFTKEYS_EMPTY_CANCEL );
                    UpdateMiddleSoftKeyL();
                    )
                }
            else
                {
                TRAP_IGNORE( Cba()->SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_BACK ) );
                }
            }
        Cba()->DrawDeferred();
        iToolbar->DrawDeferred();
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioChannelListView::UpdateDisplayForFocusGained
// Update the view after a focus gained event.
// 1. Fade/Unfade view
// 2. Update MSK label
// ---------------------------------------------------------------------------
//
void CFMRadioChannelListView::UpdateDisplayForFocusGained()
    {
    FadeAndShowExit( iFaded );
    
    // Don't display MSK if display is faded.
    if ( !iFaded )
    	{
    	TRAP_IGNORE( UpdateMiddleSoftKeyL() );
    	}
    }

// ---------------------------------------------------------------------------
// CFMRadioChannelListView::UpdateChannelListContentL
// Make a request to the channel container to update the
// content of the channel at aIndex with the values specified
// ---------------------------------------------------------------------------
//
void CFMRadioChannelListView::UpdateChannelListContentL(
    TInt aIndex,
    const TDesC& aChannelName,
    TInt aChannelFrequency )
    {
    if ( iContainer )
        {
        iContainer->UpdateChannelListContentL( aIndex, aChannelName, aChannelFrequency );
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioChannelListView::CurrentlySelectedChannel
// Returns the index of the selected channel item from the
// channel list. If the container is active get the info from
// it, because it may have more up-to-date info.
// ---------------------------------------------------------------------------
//
TInt CFMRadioChannelListView::CurrentlySelectedChannel()
    {
    if ( iContainer )
        {
        iChIndex = iContainer->CurrentlySelectedChannel();
        }
    return iChIndex;
    }

// ---------------------------------------------------------------------------
// CFMRadioChannelListView::NextChannel
// Retrieve the index of the next channel that has been set.
// ---------------------------------------------------------------------------
//
TInt CFMRadioChannelListView::NextChannel()
    {
    if ( iContainer )
        {
        iChIndex = iContainer->CurrentlySelectedChannel();
        }
    else
        {
        iChIndex = iRadioEngine->GetPresetIndex();
        }

    iChIndex = iChIndex + 1;
    
    if ( iChIndex >= iObserver.Channels().Count() )
    	{
    	iChIndex = 0;
    	}
    
    return iChIndex;
    }

// ---------------------------------------------------------------------------
// CFMRadioChannelListView::PreviousChannel
// Retrieve the index of the previous channel that has been set.
// ---------------------------------------------------------------------------
//
TInt CFMRadioChannelListView::PreviousChannel()
    {
    if ( iContainer )
        {
        iChIndex = iContainer->CurrentlySelectedChannel();
        }
    else
        {
        iChIndex = iRadioEngine->GetPresetIndex();
        }
    
        
    if ( iChIndex <= 0 )
    	{
    	iChIndex = iObserver.Channels().Count() - 1;
    	}
    else
    	{
    	iChIndex = iChIndex - 1;
    	}
    
    return iChIndex;
    }

// ---------------------------------------------------------------------------
// CFMRadioChannelListView::HandleControlEventL
// Handles control event observing. In this case the interest
// is only on the channel selection from the channel list.
// Other events are not reported by the container,
// ---------------------------------------------------------------------------
//
void CFMRadioChannelListView::HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType )
    {
    if ( aControl == iContainer )
        {
        // don't change channels in move mode
        if ( !iMoveMode )
            {
            if ( aEventType == MCoeControlObserver::EEventRequestFocus )
                {
                // Don't display MSK when display is faded.
                if ( !iFaded )
                    {
                    if ( !AknLayoutUtils::PenEnabled() )
                        {
                        UpdateMiddleSoftKeyL();
                        }
                    else
                        {
                        AppUi()->HandleCommandL( EFMRadioCmdListenCh );
                        }
                     }
                }
        
            else if ( aEventType == MCoeControlObserver::EEventStateChanged )
                    {
                    // Send command to Ui
                    AppUi()->HandleCommandL( EFMRadioCmdListenCh );
                    }
            }
        }
    
    else if ( iManualQueryDialog && aControl == iManualQueryDialog && aEventType == MCoeControlObserver::EEventStateChanged )
        {
        TUint32 newFreq = static_cast<CFMRadioFrequencyQueryDialog*>( iManualQueryDialog )->Frequency();
        iRadioEngine->Tune( static_cast<TInt>( newFreq * KFMRadioFreqMultiplier ) );
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioChannelListView::DoDeactivate
// Deactivate the channel list view
// ---------------------------------------------------------------------------
//
void CFMRadioChannelListView::DoDeactivate()
    {
    CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>(AppUi());
    if ( appUi->RadioEngine() )
        {
        iRadioEngine->RdsReceiver().RemoveObserver( this );
        }
    
    if ( iContainer )
        {
        appUi->RemoveFromViewStack( *this, iContainer );
        }
    delete iContainer;
    iContainer = NULL;
    }

// ---------------------------------------------------------------------------
// CFMRadioChannelListView::DynInitMenuPaneL
// Sets the state of menu items dynamically according to the
// state of application data.
// ---------------------------------------------------------------------------
//
void CFMRadioChannelListView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    if ( aResourceId == R_FMRADIO_CH_LIST_OPTIONS_MENU ) // Main menu
        {
        if ( iRadioEngine->IsAudioRoutingPossible() )
            {
            if ( iRadioEngine->GetAudioOutput() == CRadioEngine::EFMRadioOutputIHF )
                {
                // Delete existing menu option and add a new one
                aMenuPane->DeleteMenuItem( EFMRadioCmdActivateIhf );
                if ( iRadioEngine->IsHeadsetConnected() )
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
        
        // if help is not enabled, disable help option
        if ( !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
            {
            aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
            }
        
        TInt channelCount = iObserver.Channels().Count();
        
        // don't show stylus pop-up menu during move operation
        if ( channelCount > 1 && !iMoveMode )
            {
            aMenuPane->SetItemDimmed( EFMRadioCmdMove, EFalse );
            }
        else
            {
            aMenuPane->SetItemDimmed( EFMRadioCmdMove, ETrue );
            }
        
        if ( iMoveMode || channelCount < 1 )
            {
            // hide rename and erase 
            aMenuPane->SetItemDimmed( EFMRadioCmdRename, ETrue );
            aMenuPane->SetItemDimmed( EFMRadioCmdErase, ETrue );
            }
        else
            {
            aMenuPane->SetItemDimmed( EFMRadioCmdRename, EFalse );
            aMenuPane->SetItemDimmed( EFMRadioCmdErase, EFalse );
            }
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioChannelListView::LayoutChangedL
// ---------------------------------------------------------------------------
//
void CFMRadioChannelListView::LayoutChangedL( TInt aType )
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        iContainer->HandleResourceChange( aType );
        }
    }

// ---------------------------------------------------------------------------------------------
// Sets middle softkey label.
// ---------------------------------------------------------------------------------------------
//
void CFMRadioChannelListView::SetMiddleSoftKeyLabelL(
    TInt aResourceId,
    TInt aCommandId )
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

// ---------------------------------------------------------------------------------------------
// Updates the middle softkey functionality.
// ---------------------------------------------------------------------------------------------
//
void CFMRadioChannelListView::UpdateMiddleSoftKeyL()
    {
    
    if ( ( iContainer && 
    		( iContainer->CurrentlySelectedChannel() == iRadioEngine->GetPresetIndex() 
    		|| iObserver.Channels().Count() == 0 ) ) || 
    		iMoveMode )
    	{
    	SetMiddleSoftKeyLabelL(R_QTN_FMRADIO_NONE, EFMRadioCmdListenCh);
    	}
    else
    	{
    	SetMiddleSoftKeyLabelL(R_QTN_FMRADIO_MSK_LISTEN, EFMRadioCmdListenCh);
    	}
    }

// -----------------------------------------------------------------------------------------------
// CFMRadioChannelListView::SetEmptyChannelListItem
// Set empty channel list item when preset is deleted from fmradiopresetutility
// -----------------------------------------------------------------------------------------------
//
void CFMRadioChannelListView::SetEmptyChannelListItemL( const TInt aIndex ) const
	{
	FTRACE( FPrint( _L("CFMRadioChannelListView::SetEmptyChannelListItem index = %d"), aIndex ) );
	
	if ( iContainer )
		{					
	    iContainer->RemoveChannelL( aIndex );	    	    
	    }

	// Dim the rename and delete buttons in case the list is now empty
	UpdateToolbar();
	}

// -----------------------------------------------------------------------------------------------
// CFMRadioChannelListView::RdsDataProgrammeService
// 
// -----------------------------------------------------------------------------------------------
//
void CFMRadioChannelListView::RdsDataProgrammeService( const TDesC& /*aProgramService*/ )
	{
	}

// --------------------------------------------------------------------------------
// CFMRadioChannelListView::RdsDataPsNameIsStatic
// --------------------------------------------------------------------------------
//
void CFMRadioChannelListView::RdsDataPsNameIsStatic( TBool aStatic )
    {
    const TDesC& programmeService = iRadioEngine->RdsReceiver().ProgrammeService();
    if ( iContainer && 
            aStatic && 
            programmeService.Length() &&
            !iMoveMode )
        {
        TInt currentPresetIx = iRadioEngine->GetPresetIndex();
        // When we haven't preset name, ps name will save as preset name
        if ( iRadioEngine->GetRadioMode() == CRadioEngine::ERadioPresetMode &&
                currentPresetIx != KErrNotFound )
            {
            RPointerArray<CFMRadioPreset>& channels = iObserver.Channels();
            
            // this checks that index is not out of bound and cause -21 error during update
            if ( currentPresetIx < channels.Count() )
                {
                const TDesC& stationName = channels[ currentPresetIx ]->PresetName();
                TInt presetFreq = channels[ currentPresetIx ]->PresetFrequency();
                                
                if ( stationName.Length() == 0 ) //Set and save PSname only if no name set before
                    {
                    TRAP_IGNORE(
                        {
                        iContainer->UpdateChannelListContentL( currentPresetIx,
                                programmeService,
                                presetFreq );

                        iRadioEngine->SetPresetNameFrequencyL( currentPresetIx, programmeService, presetFreq );
                        channels[ currentPresetIx ]->SetPresetNameL( programmeService );
                        } )
                    }
                }
            }
        }
    }

// --------------------------------------------------------------------------------
// CFMRadioMainView::PrepareToolbar
// Prepare toolbar for displaying basic radio command buttons
// --------------------------------------------------------------------------------
//
void CFMRadioChannelListView::PrepareToolbar()
    {
    // get toolbar from view
    iToolbar = Toolbar();
    // Set observer
    iToolbar->SetToolbarObserver( this );
    }

// --------------------------------------------------------------------------------
// CFMRadioMainView::UpdateToolbar
// Set the dimmed status of buttons according to list state
// --------------------------------------------------------------------------------
//
void CFMRadioChannelListView::UpdateToolbar() const
    {
    if ( iContainer && iToolbar && AknLayoutUtils::PenEnabled() )
        {
        if ( iRadioEngine->IsAudioRoutingPossible() )
            {
            if ( iRadioEngine->IsHeadsetConnected() )
                {
                CAknButton* centerButton = NULL;    
                centerButton = static_cast<CAknButton*>( iToolbar->ControlOrNull( EFMRadioToolbarButtonCmdToggleIhf ) );

                iToolbar->SetItemDimmed( EFMRadioToolbarButtonCmdToggleIhf, EFalse, ETrue );
                
                if ( iRadioEngine->GetAudioOutput() == CRadioEngine::EFMRadioOutputIHF )
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
        }
    }

// --------------------------------------------------------------------------------
// CFMRadioChannelListView::OfferToolbarEventL
// Handle commands from toolbar.
// --------------------------------------------------------------------------------
//
void CFMRadioChannelListView::OfferToolbarEventL( TInt aCommandId )
    {
    HandleCommandL( aCommandId );
    }

// ---------------------------------------------------------
// CFMRadioChannelListView::ShowToolbar
// Sets toolbar visibility.
// ---------------------------------------------------------
//
void CFMRadioChannelListView::ShowToolbar( TBool aVisible )
    {
    if ( AknLayoutUtils::PenEnabled() &&
         iToolbar &&
         iContainer )
        {
        iToolbar->SetToolbarVisibility( aVisible );
        iToolbar->UpdateBackground();
        iToolbar->DrawDeferred();
        // set correct container rect with or without toolbar
        iContainer->SetRect( ClientRect() );
        }
    }


// -----------------------------------------------------------------------------------------------
// CFMRadioChannelListView::RdsDataRadioText
// From MFMRadioRdsObserver, implementation not needed.
// -----------------------------------------------------------------------------------------------
//
void CFMRadioChannelListView::RdsDataRadioText( const TDesC& /*aRadioText*/ ){}

void CFMRadioChannelListView::RdsDataRadioTextPlus( const TInt aRTPlusClass, const TDesC& aRadioText )
    {
    if ( aRTPlusClass == ERTplusProgramHomepage &&
            iRadioEngine->GetRadioMode() == CRadioEngine::ERadioPresetMode )
        {
        TInt currentPresetIx = iRadioEngine->GetPresetIndex();
        if ( currentPresetIx < iObserver.Channels().Count() )
            {
            TRAP_IGNORE
                (
                // save permanently
                iRadioEngine->SaveUrlToPresetL( currentPresetIx, aRadioText );
                // save to ui list
                iObserver.Channels()[ currentPresetIx ]->SetPresetUrlL( aRadioText );
                )
            }
        }
    }

// -----------------------------------------------------------------------------------------------
// CFMRadioChannelListView::RdsAfSearchBegin
// From MFMRadioRdsObserver, implementation not needed.
// -----------------------------------------------------------------------------------------------
//
void CFMRadioChannelListView::RdsAfSearchBegin(){}

// -----------------------------------------------------------------------------------------------
// CFMRadioChannelListView::RdsAfSearchEnd
// From MFMRadioRdsObserver, implementation not needed.
// -----------------------------------------------------------------------------------------------
//
void CFMRadioChannelListView::RdsAfSearchEnd( TUint32 /* aFrequency*/, TInt /*aError*/ ){}

// -----------------------------------------------------------------------------------------------
// CFMRadioChannelListView::RdsAfSearchStateChange
// From MFMRadioRdsObserver, implementation not needed.
// -----------------------------------------------------------------------------------------------
//
void CFMRadioChannelListView::RdsAfSearchStateChange( TBool /* aEnabled*/ ){}

// -----------------------------------------------------------------------------------------------
// CFMRadioChannelListView::RdsAvailable
// From MFMRadioRdsObserver, implementation not needed.
// -----------------------------------------------------------------------------------------------
//
void CFMRadioChannelListView::RdsAvailable( TBool /*aAvailable*/ ){}

// ---------------------------------------------------------
// CFMRadioChannelListView::SetTitleL
// ---------------------------------------------------------
//
void CFMRadioChannelListView::SetTitleL( TInt aResourceId )
    {
    HBufC* title = StringLoader::LoadLC( aResourceId, iCoeEnv );
    static_cast<CAknTitlePane*>( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) )->SetTextL( *title, ETrue );
    CleanupStack::PopAndDestroy( title );
    }


// ---------------------------------------------------------
// CFMRadioChannelListView::MoveCurrentItemIndexL
// ---------------------------------------------------------
//
void CFMRadioChannelListView::MoveCurrentItemIndexL()
    {
    iMoveMode = ETrue;
    
    Cba()->SetCommandSetL( R_FMRADIO_SOFTKEYS_EMPTY_CANCEL );
    UpdateMiddleSoftKeyL();
    ShowToolbar( EFalse );
    
    iContainer->HideIconsL();

    iContainer->ActivateMoveL();
    }

// ---------------------------------------------------------
// CFMRadioChannelListView::MoveActionDoneL
// ---------------------------------------------------------
//
void CFMRadioChannelListView::MoveActionDoneL()
    {
    iMoveMode = EFalse;
    iFirstMoveDone = ETrue;
    Cba()->SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_BACK );
    
    UpdateMiddleSoftKeyL();
    iContainer->HandleDropL();
    iContainer->MoveDoneL();
    
    TInt currentIndex = 0;    
    CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( AppUi() );
    currentIndex = appUi->MatchingChannelL( iRadioEngine->GetTunedFrequency() );
    
    if ( currentIndex != KErrNotFound )
        {
        iRadioEngine->SetCurrentPresetIndex( currentIndex );
        SetNowPlayingChannel( currentIndex );
        }
    ShowToolbar( ETrue );
    }

// ---------------------------------------------------------
// CFMRadioChannelListView::CancelMoveL
// ---------------------------------------------------------
//
void CFMRadioChannelListView::CancelMoveL()
    {
    CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( AppUi() );
    TBool viewActive = appUi->ActiveView() == KFMRadioChannelListViewId && iContainer;
    if ( viewActive && ( iContainer->MoveCanceledL() || iMoveMode ) )
        {
        iMoveMode = EFalse;
        iFirstMoveDone = ETrue;
        iContainer->ReIndexAllL();
        Cba()->SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_BACK );
        UpdateMiddleSoftKeyL();
        TInt currentIndex = 0;
        currentIndex = appUi->MatchingChannelL( iRadioEngine->GetTunedFrequency() );
        if ( currentIndex != KErrNotFound )
            {
            SetNowPlayingChannel( currentIndex );
            }
        ShowToolbar( ETrue );
        }
    }

// ---------------------------------------------------------
// CFMRadioChannelListView::StopSeekL
// ---------------------------------------------------------
//
void CFMRadioChannelListView::StopSeekL()
    {
    // update icon if radio is in tuner mode i.e.
    // "accessory seek up/down" is launched
    if ( iContainer &&
            iRadioEngine->GetRadioMode() == CRadioEngine::ERadioTunerMode )
        {
        CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( AppUi() );
        TInt tunedFrequency = iRadioEngine->GetTunedFrequency();
        const TInt index = appUi->MatchingChannelL( tunedFrequency );

        if ( index == KErrNotFound )
            {
            // frequency is not saved, hide now playing icon
            iContainer->HideIconsL();
            }
        else
            {
            SetNowPlayingChannel( index );
            SetLastListenedChannel( index );
            }
        }
    }

// End of File
