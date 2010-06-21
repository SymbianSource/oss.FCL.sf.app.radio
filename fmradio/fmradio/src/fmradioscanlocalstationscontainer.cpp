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
* Description:  Implementation of the class CFMRadioScanLocalStationsContainer
*
*/


// INCLUDE FILES
#include <e32math.h>
#include <AknsSkinInstance.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <AknsBasicBackgroundControlContext.h>
#include <akntoolbar.h>
#include <gulutil.h> 
#include <w32std.h>
#include <fmradio.rsg>
#include <StringLoader.h>
#include <eiklabel.h>
#include <AknWaitDialog.h>
#include <eikclbd.h> 
#include <AknIconArray.h>
#include <gulicon.h> 
#include <fmradiouids.h>
#include <aknconsts.h>
#include <avkon.mbg>

#if defined __SERIES60_HELP || defined FF_S60_HELPS_IN_USE
#include "radio.hlp.hrh"
#endif
#include "fmradioapp.h"
#include "fmradioappui.h"
#include "fmradioscanlocalstationscontainer.h"
#include "fmradioscanlocalstationsview.h"

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CFMRadioScanLocalStationsContainer::CFMRadioScanLocalStationsContainer
// Default constructor
// ----------------------------------------------------------------------------
//
CFMRadioScanLocalStationsContainer::CFMRadioScanLocalStationsContainer( CRadioEngine& aRadioEngine ):
	iRadioEngine( aRadioEngine )
	{	
	}

// ---------------------------------------------------------
// CFMRadioScanLocalStationsContainer::NewL
// Two-phase constructor of CFMRadioScanLocalStationsContainer
// ---------------------------------------------------------
//
CFMRadioScanLocalStationsContainer* CFMRadioScanLocalStationsContainer::NewL( const TRect& aRect, CRadioEngine& aRadioEngine )
    {
    CFMRadioScanLocalStationsContainer* self = new (ELeave) CFMRadioScanLocalStationsContainer( aRadioEngine );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------
// CFMRadioScanLocalStationsContainer::ConstructL
// EPOC two phased constructor
// ----------------------------------------------------
//
void CFMRadioScanLocalStationsContainer::ConstructL( const TRect& aRect )
    {
    // check the layout orientation
    CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( iCoeEnv->AppUi() );
    TBool isLandscape = appUi->IsLandscapeOrientation();
    CreateWindowL();

    iSkin = AknsUtils::SkinInstance();

    // Instantiate a listbox for the channel list
    iChannelList = new ( ELeave ) CAknSingleNumberStyleListBox();
    iChannelList->SetContainerWindowL( *this );
    iChannelList->SetListBoxObserver( this );
    iChannelList->ConstructL( this, CEikListBox::ELoopScrolling | EAknListBoxSelectionList ); // Looped list
    // Create scrollbars
    iChannelList->CreateScrollBarFrameL( ETrue );
    
    CAknIconArray* listIconArray = new ( ELeave ) CAknIconArray( 1 );
    CleanupStack::PushL( listIconArray );
    CreateListIconsL( *listIconArray );
    iChannelList->ItemDrawer()->ColumnData()->SetIconArray( listIconArray );
    CleanupStack::Pop( listIconArray );

    // Array for channels
    iChannelItemArray = new( ELeave ) CDesCArrayFlat( KMaxNumberOfChannelListItems );
    InitializeChannelListL();
    SetRect( aRect ); 

    CFMRadioScanLocalStationsView* searchStationsView = static_cast<CFMRadioScanLocalStationsView*> ( appUi->View( KFMRadioScanLocalStationsViewId ) );

    ActivateL();
    }

// ----------------------------------------------------
// CFMRadioScanLocalStationsContainer::CreateListIconsL
// ----------------------------------------------------
//
void CFMRadioScanLocalStationsContainer::CreateListIconsL( CArrayPtr<CGulIcon>& aArray )
    {
    if ( iBitMaps.Count() )
        {
        // release any previously created bitmaps
        iBitMaps.ResetAndDestroy();
        }
    
    TRgb defaultColor = iEikonEnv->Color( EColorControlText );

    // speaker icon
    CFbsBitmap* playingIconBitmap = NULL;
    CFbsBitmap* playingIconBitmapMask = NULL;
        
    AknsUtils::CreateColorIconLC( iSkin,
                KAknsIIDQgnIndiSpeaker,
                KAknsIIDQsnIconColors,
                EAknsCIQsnIconColorsCG13,
                playingIconBitmap,
                playingIconBitmapMask,
                KAvkonBitmapFile,
                EMbmAvkonQgn_indi_speaker,
                EMbmAvkonQgn_indi_speaker_mask,
                defaultColor
                );
    iBitMaps.AppendL( playingIconBitmap );
    iBitMaps.AppendL( playingIconBitmapMask );
    CleanupStack::Pop( 2 ); // playingIconBitmap, playingIconBitmapMask
         
    CGulIcon* playingIcon = CGulIcon::NewLC();
    playingIcon->SetBitmapsOwnedExternally( ETrue );
    playingIcon->SetBitmap( playingIconBitmap );
    playingIcon->SetMask( playingIconBitmap );
    aArray.AppendL( playingIcon );
    CleanupStack::Pop( playingIcon );
    }

// ----------------------------------------------------
// CFMRadioScanLocalStationsContainer::~CFMRadioScanLocalStationsContainer
// Class destructor
// ----------------------------------------------------
//
CFMRadioScanLocalStationsContainer::~CFMRadioScanLocalStationsContainer()
    {
    delete iChannelList;
    iBitMaps.ResetAndDestroy();
    iBitMaps.Close();
    delete iChannelItemArray;
    }

// ----------------------------------------------------
// CFMRadioScanLocalStationsContainer::InitializeChannelListL
// Create the channel list (initially all spots are set as empty)
// ----------------------------------------------------
//
void CFMRadioScanLocalStationsContainer::InitializeChannelListL()
	{
	iChannelList->Reset();
	iChannelItemArray->Reset();
	iChannelList->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    // Pass the array to the listbox model and set the ownership type
    iChannelList->Model()->SetItemTextArray( (MDesCArray*) iChannelItemArray );
    iChannelList->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    iChannelList->HandleItemAdditionL();
	DrawDeferred();
	}

// ----------------------------------------------------
// CFMRadioScanLocalStationsContainer::UpdateChannelListContentL
// Update the content of the channel at aIndex with the
// values specified
// ----------------------------------------------------
//
void CFMRadioScanLocalStationsContainer::UpdateChannelListContentL( TInt aIndex,
        const TDesC& aInfoText,
        TInt aChannelFrequency,
        TBool aNowPlaying,
        TBool aInfoTextPlacement )
    {
    TBuf<KLengthOfChannelItemString> textChannelItem;
    TBuf<KLengthOfChIndexStringChList> textChIndex;
    TBuf<KLengthOfChannelItemIconIndexString> textChIconIndex;
    
    textChIndex.SetLength( 0 );
    
    textChIconIndex.Format( KChIconIndexFormatChList, KNowPlayingIconIndexChList ) ;
    textChannelItem.Append( textChIndex );
    textChannelItem.Append( KColumnListSeparator );
    
    TReal frequency = static_cast<TReal>( aChannelFrequency / static_cast<TReal>( KHzConversionFactor ));
    // Gets locale decimal separator automatically
    TRealFormat format(KFrequencyMaxLength, iRadioEngine.DecimalCount() );
    TBuf<30> frequencyString;
    frequencyString.Num( frequency, format );

    HBufC* listItemString = NULL;
    if ( aInfoTextPlacement ) // frequency and PS name
        {
        const TInt granularity = 2;
        
        CDesCArray* stringsArray = new ( ELeave ) CDesCArrayFlat( granularity );
        CleanupStack::PushL( stringsArray );
        stringsArray->AppendL( frequencyString );
        stringsArray->AppendL( aInfoText );
        
        listItemString = StringLoader::LoadL( R_QTN_FMRADIO_SINGLE_FREQ_NAME, *stringsArray, iEikonEnv );
        CleanupStack::PopAndDestroy( stringsArray );
        CleanupStack::PushL( listItemString );
        }
    else // frequency
        {
        listItemString = StringLoader::LoadLC( R_QTN_FMRADIO_SINGLE_FREQ, frequencyString, iEikonEnv );
        }

    // Update for display of Hindi Devnagari Numbers
    TPtr listItemStringPtr = listItemString->Des();
    AknTextUtils::LanguageSpecificNumberConversion( listItemStringPtr );
        
    textChannelItem.Append( *listItemString );
    CleanupStack::PopAndDestroy( listItemString );
    
    // Set 'Now Playing' icon to the channel item
    textChannelItem.Append( KColumnListSeparator );
    
    if ( aNowPlaying )
        {
        textChannelItem.Append( textChIconIndex );	
        }
    
    if ( aIndex < iChannelItemArray->Count() && aIndex >= 0 )
        {
        iChannelItemArray->Delete( aIndex );
        iChannelItemArray->InsertL( aIndex, textChannelItem );
        }
    else
        {
        iChannelItemArray->AppendL( textChannelItem );
        iChannelList->HandleItemAdditionL(); // Update list
        iChannelList->UpdateScrollBarsL();
        }

    iChannelList->DrawDeferred();
    }

// --------------------------------------------------------------------------------
// CFMRadioScanLocalStationsContainer::InsertScannedChannelToListL
// --------------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsContainer::InsertScannedChannelToListL( TInt aIndex, TInt aChannelFrequency )
    {
    TBuf<KLengthOfChannelItemString> textChannelItem;
    TBuf<KLengthOfChIndexStringChList> textChIndex;
    
    textChannelItem.Append( textChIndex );
    textChannelItem.Append( KColumnListSeparator );
    
    TReal frequency = static_cast<TReal>( aChannelFrequency / static_cast<TReal>( KHzConversionFactor ));
    // Gets locale decimal separator automatically
    TRealFormat format(KFrequencyMaxLength, iRadioEngine.DecimalCount() );
    TBuf<30> frequencyString;
    frequencyString.Num( frequency, format );
    
    HBufC* listItemString = listItemString = StringLoader::LoadLC( R_QTN_FMRADIO_SINGLE_FREQ, frequencyString, iEikonEnv );
    // Update for display of Hindi Devnagari Numbers
    TPtr listItemStringPtr = listItemString->Des();
    AknTextUtils::LanguageSpecificNumberConversion( listItemStringPtr );
    textChannelItem.Append( *listItemString );
    CleanupStack::PopAndDestroy( listItemString );
    
    // Set 'Now Playing' icon to the channel item
    textChannelItem.Append( KColumnListSeparator );
    
    if ( aIndex <= ( iChannelItemArray->Count() - 1 ) )
        {
        iChannelItemArray->InsertL( aIndex, textChannelItem );
        }
    else
        {
        iChannelItemArray->AppendL( textChannelItem );
        }    
    
    iChannelList->HandleItemAdditionL(); // Update list
    iChannelList->UpdateScrollBarsL();
    iChannelList->DrawDeferred();
    }
	
// ----------------------------------------------------
// CFMRadioScanLocalStationsContainer::RemoveChannelListContentL
// 
// ----------------------------------------------------
//
void CFMRadioScanLocalStationsContainer::RemoveChannelListContentL( TInt aIndex )
    {
    if ( aIndex < iChannelItemArray->Count() )
        {
        iChannelItemArray->Delete( aIndex );
        iChannelList->HandleItemRemovalL();
        iChannelList->UpdateScrollBarsL();

        TInt channelCount = iChannelItemArray->Count();
        // check if the last channel is saved and update current item
        if ( aIndex == channelCount && channelCount > 0 )
            {
            iChannelList->SetCurrentItemIndex( channelCount - 1 );
            }
        iChannelList->DrawDeferred();
        }
    }

// ----------------------------------------------------
// CFMRadioScanLocalStationsContainer::UpdateNowPlayingIconL
// Displays 'Now Playing' icon in the current selected channel item.
// ----------------------------------------------------
//
void CFMRadioScanLocalStationsContainer::UpdateNowPlayingIconL( TInt aNewIndex, TInt aOldIndex )
    {
    HideNowPlayingIconL( aOldIndex, EFalse );

    if ( aNewIndex >= 0 && aNewIndex < iChannelItemArray->Count() )
        {
        TBuf<KLengthOfChannelItemIconIndexString> textChIconIndex;
        textChIconIndex.Format( KChIconIndexFormatChList, KNowPlayingIconIndexChList );
        HBufC* channelItem = HBufC::NewLC( KLengthOfChannelItemString );
        channelItem->Des().Copy( iChannelItemArray->MdcaPoint( aNewIndex ) );
        TPtr ptr( channelItem->Des() );
        ptr.Append( textChIconIndex ); // Add icon index

        iChannelItemArray->InsertL( aNewIndex, *channelItem );
        CleanupStack::PopAndDestroy( channelItem );
        iChannelList->HandleItemAdditionL(); // Update list
        iChannelItemArray->Delete( aNewIndex + 1 );
        iChannelList->HandleItemRemovalL();
        iChannelList->UpdateScrollBarsL();
        }
    // required for fast channel browsing
    iChannelList->DrawNow();
    }

// ----------------------------------------------------
// CFMRadioScanLocalStationsContainer::HideNowPlayingIconL
// Hides 'Now Playing' icon from the last selected channel.
// ----------------------------------------------------
//
void CFMRadioScanLocalStationsContainer::HideNowPlayingIconL( TInt aIndex, TBool aDraw )
    {
    if ( aIndex >= 0 && aIndex < iChannelItemArray->Count() )
        {
        HBufC* channelItem = HBufC::NewLC( KLengthOfChannelItemString );
        channelItem->Des().Copy( iChannelItemArray->MdcaPoint( aIndex ) );
        TPtr ptr( channelItem->Des() );	

        TBuf<KLengthOfChannelItemIconIndexString> textChIconIndex;
        TPtrC iconIndexPtr( textChIconIndex );
        TInt err = TextUtils::ColumnText( iconIndexPtr, 2, &ptr );

        if ( iconIndexPtr.Length() > 0 )
            {
            ptr.Delete( ptr.Length() - iconIndexPtr.Length(), iconIndexPtr.Length() ); // Remove icon index
            iChannelItemArray->InsertL( aIndex, *channelItem );
            iChannelList->HandleItemAdditionL(); // Update list
            iChannelItemArray->Delete( aIndex + 1 );
            iChannelList->HandleItemRemovalL();
            iChannelList->UpdateScrollBarsL();
            
            }
        CleanupStack::PopAndDestroy( channelItem );
        if ( aDraw )
            {
            iChannelList->DrawDeferred();
            }
        }    
    }

// ----------------------------------------------------
// CFMRadioScanLocalStationsContainer::SetFaded
// Fades the entire window and controls in the window owned
// by this container control.
// ----------------------------------------------------
//
void CFMRadioScanLocalStationsContainer::SetFaded( TBool aFaded )
	{
    iFadeStatus = aFaded;
    Window().SetFaded( aFaded, RWindowTreeNode::EFadeIncludeChildren );
	}

// ----------------------------------------------------
// CFMRadioScanLocalStationsContainer::UpdateLastListenedChannel
// Updates channel index - the highlighted channel in the list.
// ----------------------------------------------------
//
void CFMRadioScanLocalStationsContainer::UpdateLastListenedChannel( TInt aIndex )
	{
	if ( aIndex < iChannelItemArray->Count() &&
		 aIndex > -1 )
		{
	    iLastChIndex = aIndex;
		   
		iChannelList->SetCurrentItemIndex( iLastChIndex );
		iChannelList->ScrollToMakeItemVisible( iLastChIndex );
		DrawDeferred();			
		}

	}

// ----------------------------------------------------
// CFMRadioScanLocalStationsContainer::CurrentlySelectedChannel
// Returns the index of the selected channel item from the
// channel list.
// ----------------------------------------------------
//
TInt CFMRadioScanLocalStationsContainer::CurrentlySelectedChannel() const
	{
    return iChannelList->CurrentItemIndex(); // Index of selected channel
	}

// ----------------------------------------------------
// CFMRadioScanLocalStationsContainer::HandleListBoxEventL
// For handling listbox-, in this case, channel list events.
// The event equals to selecting a channel item from the list.
// ----------------------------------------------------
//
void CFMRadioScanLocalStationsContainer::HandleListBoxEventL( CEikListBox* /*aListBox*/,
                                                              TListBoxEvent aEventType )
    {
    // Check the event generated by keypress and report the event,
    // Also check for Pen Enabled touch screen event
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked: // new event for item activation
            {
            ReportEventL( MCoeControlObserver::EEventStateChanged );
            break;
            }
        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CFMRadioScanLocalStationsContainer::HandleResourceChange
// -----------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );
    if ( aType ==  KEikDynamicLayoutVariantSwitch  )
        {
        SizeChanged();
        }
    else if ( aType == KAknsMessageSkinChange )
        {
        CArrayPtr<CGulIcon>* iconArray = iChannelList->ItemDrawer()->ColumnData()->IconArray();
        // update icons with new skin
        iconArray->ResetAndDestroy();
        TRAP_IGNORE( CreateListIconsL( *iconArray ) )
        }
    }

// ---------------------------------------------------------
// CFMRadioScanLocalStationsContainer::SizeChanged
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CFMRadioScanLocalStationsContainer::SizeChanged()
    {
    iChannelList->SetRect( Rect() );
    }

// ---------------------------------------------------------
// CFMRadioScanLocalStationsContainer::CountComponentControls
// Return the number of controls in the window owned by this container
// ---------------------------------------------------------
//
TInt CFMRadioScanLocalStationsContainer::CountComponentControls() const
    {
    return 1;
    }

// ---------------------------------------------------------
// CFMRadioScanLocalStationsContainer::ComponentControl
// Return the control corresponding to the specified index
// ---------------------------------------------------------
//
CCoeControl* CFMRadioScanLocalStationsContainer::ComponentControl( TInt aIndex ) const
    {
    CCoeControl* control = NULL;
    
    switch ( aIndex )
        {
        case 0:
            {
            control = iChannelList;
            break;
            }
        default:
            {
            break;
            }
        }
    
    return control;
    }

// ---------------------------------------------------------
// CFMRadioScanLocalStationsContainer::OfferKeyEventL
// Allow the channel list to process key events
// ---------------------------------------------------------
//
TKeyResponse CFMRadioScanLocalStationsContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
														   		 TEventCode aType )
	{
	TKeyResponse response = EKeyWasNotConsumed;
    
    switch ( aKeyEvent.iCode )
        {
        case EKeyLeftArrow:
        case EKeyRightArrow:
            return EKeyWasNotConsumed;

        case EKeyUpArrow:
        case EKeyDownArrow:
            response = iChannelList->OfferKeyEventL( aKeyEvent, aType );
            if (response == EKeyWasConsumed)
                {
                ReportEventL( MCoeControlObserver::EEventRequestFocus );
                }
            return response;


        default:
			switch ( aKeyEvent.iScanCode ) //we need to use the scan code, because we need to process the event wrt the keyUp and keyDown action
				{
                case EKeyboardKey1: // Timed key
                case EKeyboardKey2: // Normal keys
                case EKeyboardKey3:
                case EKeyboardKey4:
                case EKeyboardKey5:
                case EKeyboardKey6:
                case EKeyboardKey7:
                case EKeyboardKey8:
                case EKeyboardKey9:
                case EKeyboardKey0:
                   return EKeyWasNotConsumed;
                default:
                    break;
                }
            break;
        }
    return iChannelList->OfferKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------------------------
// CFMRadioScanLocalStationsContainer::GetHelpContext
// Gets Help
// ---------------------------------------------------------------------------
//
void CFMRadioScanLocalStationsContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
#if defined __SERIES60_HELP || defined FF_S60_HELPS_IN_USE
    aContext.iMajor = TUid::Uid( KUidFMRadioApplication );
    aContext.iContext = KFMRADIO_HLP_SEARCH;
#endif
    }

// ---------------------------------------------------------
// CFMRadioScanLocalStationsContainer::FocusChanged(TDrawNow aDrawNow)
// ---------------------------------------------------------
//
void CFMRadioScanLocalStationsContainer::FocusChanged( TDrawNow aDrawNow )
    {
    CCoeControl::FocusChanged( aDrawNow );
    if ( iChannelList )
        {
        iChannelList->SetFocus( IsFocused(), aDrawNow );
        }
    }
	
// --------------------------------------------------------------------------------
// CFMRadioScanLocalStationsContainer::ResetChannelListL
// Sets toolbar visibility
// --------------------------------------------------------------------------------
//	
void CFMRadioScanLocalStationsContainer::ResetChannelListL()
	{
	iChannelItemArray->Reset();
	iChannelList->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff );
	iChannelList->HandleItemRemovalL();
	iChannelList->DrawDeferred();		
	}	

// ---------------------------------------------------------
// CFMRadioScanLocalStationsContainer::SetStationListEmptyTextL
// ---------------------------------------------------------
//
void CFMRadioScanLocalStationsContainer::SetStationListEmptyTextL( const TDesC& aText )
	{
   	iChannelList->View()->SetListEmptyTextL( aText );
	}
	
// ---------------------------------------------------------
// CFMRadioScanLocalStationsContainer::HandlePointerEventL
// ---------------------------------------------------------
//
void CFMRadioScanLocalStationsContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    // don't handle any pointer events if view is faded
    if ( !iFadeStatus )
        {
        CCoeControl::HandlePointerEventL( aPointerEvent );
        }
    }

// End of File
