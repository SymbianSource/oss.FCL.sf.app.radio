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
* Description:  Implementation of the class CFMRadioChannelListContainer
*
*/


// INCLUDE FILES
#include <e32math.h>
#include <AknsSkinInstance.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <AknsBasicBackgroundControlContext.h>
#include <gulutil.h> 
#include <w32std.h>
#include <fmradio.rsg>
#if defined __SERIES60_HELP || defined FF_S60_HELPS_IN_USE
#include "radio.hlp.hrh"
#endif
#include <StringLoader.h>
#include <eiklabel.h>
#include <AknIconArray.h>
#include <gulicon.h>
#include <fmradiouids.h>
#include <aknconsts.h>
#include <avkon.mbg>


#include "fmradioappui.h"
#include "fmradiochannellistcontainer.h"
#include "fmradiochannellistview.h"

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================


// ----------------------------------------------------------------------------
// CFMRadioChannelListContainer::CFMRadioChannelListContainer
// Default constructor
// ----------------------------------------------------------------------------
//
CFMRadioChannelListContainer::CFMRadioChannelListContainer( 
	CRadioEngine& aRadioEngine,
	MChannelListHandler& aObserver ) :
	iRadioEngine( aRadioEngine ),
	iObserver( aObserver )
	
	{	
	}

// ---------------------------------------------------------
// CFMRadioChannelListContainer::NewL
// Two-phase constructor of CFMRadioChannelListContainer
// ---------------------------------------------------------
//
CFMRadioChannelListContainer* CFMRadioChannelListContainer::NewL( const TRect& aRect, 
		CRadioEngine& aRadioEngine, MChannelListHandler& aObserver  )
    {
    CFMRadioChannelListContainer* self = new (ELeave) CFMRadioChannelListContainer( aRadioEngine,
    		aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop( self );
    return self;
    }

//
// ----------------------------------------------------
// CFMRadioChannelListContainer::ConstructL
// EPOC two phased constructor
// ----------------------------------------------------
//
void CFMRadioChannelListContainer::ConstructL( const TRect& aRect )
    {
    CreateWindowL();

    // Instantiate a listbox for the channel list
    iChannelList = new ( ELeave ) CAknDoubleNumberStyleListBox();
    iControls.Append( iChannelList );
    iChannelList->SetContainerWindowL( *this );
    iChannelList->SetListBoxObserver( this );
    iChannelList->ConstructL( this, CEikListBox::ELoopScrolling | EAknListBoxSelectionList ); // Looped list
    iChannelList->SetRect( aRect ); // Set boundaries for listbox
    // Create scrollbars
    iChannelList->CreateScrollBarFrameL( ETrue );
    iChannelList->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    HBufC* emptyString = NULL;
    emptyString = StringLoader::LoadLC( R_QTN_FMRADIO_LIST_NO_STATIONS, iEikonEnv );
    
    iChannelList->View()->SetListEmptyTextL( *emptyString );
    CleanupStack::PopAndDestroy( emptyString );

    CAknIconArray* listIconArray = new ( ELeave ) CAknIconArray( 2 );
    CleanupStack::PushL( listIconArray );
    CreateListIconsL( *listIconArray );
    iChannelList->ItemDrawer()->FormattedCellData()->SetIconArrayL( listIconArray );
    CleanupStack::Pop( listIconArray );
    
    // Array for channels
    iChannelItemArray = new( ELeave ) CDesCArrayFlat( KMaxNumberOfChannelListItems );
    InitializeChannelListL();
    SetRect( aRect );             // Set its rectangle

    CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( iCoeEnv->AppUi() );
    iChannelView = static_cast<CFMRadioChannelListView*> ( appUi->View( KFMRadioChannelListViewId ) );
    ActivateL();
    }

// ----------------------------------------------------
// CFMRadioChannelListContainer::~CFMRadioChannelListContainer
// Class destructor
// ----------------------------------------------------
//
CFMRadioChannelListContainer::~CFMRadioChannelListContainer()
    {
    iControls.ResetAndDestroy();
    iControls.Close();
    iBitMaps.ResetAndDestroy();
    iBitMaps.Close();
    delete iChannelItemArray;
    }

// ----------------------------------------------------
// CFMRadioChannelListContainer::CreateListIconsL
// ----------------------------------------------------
//
void CFMRadioChannelListContainer::CreateListIconsL( CArrayPtr<CGulIcon>& aArray )
    {
    if ( iBitMaps.Count() )
        {
        // release any previously created bitmaps
        iBitMaps.ResetAndDestroy();
        }
    
    TRgb defaultColor = iEikonEnv->Color( EColorControlText );
    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();

    // speaker icon
    CFbsBitmap* playingIconBitmap = NULL;
    CFbsBitmap* playingIconBitmapMask = NULL;
        
    AknsUtils::CreateColorIconLC( skinInstance,
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
   
    // marked icon
    CFbsBitmap* markedIconBitmap = NULL;
    CFbsBitmap* markedIconBitmapMask = NULL;
    
    AknsUtils::CreateColorIconLC( skinInstance,
                KAknsIIDQgnIndiMarkedAdd,
                KAknsIIDQsnIconColors,
                EAknsCIQsnIconColorsCG13,
                markedIconBitmap,
                markedIconBitmapMask,
                KAvkonBitmapFile,
                EMbmAvkonQgn_indi_marked_add,
                EMbmAvkonQgn_indi_marked_add_mask,
                defaultColor
                );    
    iBitMaps.AppendL( markedIconBitmap );
    iBitMaps.AppendL( markedIconBitmapMask );
    CleanupStack::Pop( 2 ); // markedIconBitmap, markedIconBitmapMask
    
    CGulIcon* markedIcon = CGulIcon::NewLC();
    markedIcon->SetBitmapsOwnedExternally( ETrue );
    markedIcon->SetBitmap( markedIconBitmap );
    markedIcon->SetMask( markedIconBitmapMask );
    aArray.AppendL( markedIcon );
    CleanupStack::Pop( markedIcon );
    }

// ----------------------------------------------------
// CFMRadioChannelListContainer::InitializeChannelListL
// Create the channel list (initially all spots are set as empty)
// ----------------------------------------------------
//
void CFMRadioChannelListContainer::InitializeChannelListL()
	{
    TBuf<KLengthOfChannelItemString> textChannelItem;
    textChannelItem.SetLength( 0 );

    // Pass the array to the listbox model and set the ownership type
    iChannelList->Model()->SetItemTextArray( (MDesCArray*)iChannelItemArray );
    iChannelList->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    iChannelList->HandleItemAdditionL();
	DrawDeferred();
	}

// ----------------------------------------------------
// CFMRadioChannelListContainer::UpdateChannelListContentL
// Update the content of the channel at aIndex with the
// values specified
// ----------------------------------------------------
//
void CFMRadioChannelListContainer::UpdateChannelListContentL( TInt aIndex,
        const TDesC& aChannelName,
        TInt aChannelFrequency )
    {
    TBuf<KLengthOfChannelItemString> textChannelItem;
    TBuf<KLengthOfChIndexStringChList> textChIndex;
    TBuf<KLengthOfChannelItemIconIndexString> textChIconIndex;
    
    textChIndex.Format( KChIndexFormatChList, aIndex+1 ); // One over index
    
    textChIconIndex.Format( KChIconIndexFormatChList, KNowPlayingIconIndexChList );
    
    //Update to display Devnagari numbers for Text Index.
    AknTextUtils::LanguageSpecificNumberConversion( textChIndex );

    textChannelItem.Append( textChIndex );
    textChannelItem.Append( KColumnListSeparator );
    textChannelItem.Append( aChannelName );
    textChannelItem.Append( KColumnListSeparator );
    
    if ( aChannelFrequency )
        {
        TReal frequency = static_cast<TReal>( aChannelFrequency / static_cast<TReal>( KHzConversionFactor ));
        // Gets locale decimal separator automatically
        TRealFormat format(KFrequencyMaxLength, iRadioEngine.DecimalCount() );
        TBuf<30> frequencyString;
        frequencyString.Num( frequency, format );
    
        HBufC* stringHolder = StringLoader::LoadLC( R_QTN_FMRADIO_DOUBLE2_FREQ, frequencyString, iEikonEnv );
        //Update for display of Hindi Devnagari Numbers
        TPtr textItem2 = stringHolder->Des();
        AknTextUtils::LanguageSpecificNumberConversion(textItem2);
        textChannelItem.Append( textItem2 );
        CleanupStack::PopAndDestroy( stringHolder );
        }
        
    // Set 'Now Playing' icon to the channel item
    textChannelItem.Append( KColumnListSeparator );
                 
    if ( iRadioEngine.GetPresetIndex() == aIndex )
        {
        textChannelItem.Append( textChIconIndex );
        }
    
    if ( aIndex < iChannelItemArray->Count() && aIndex >= 0 )
        {
        iChannelItemArray->Delete( aIndex );
        iChannelItemArray->InsertL( aIndex, textChannelItem );
        iChannelList->DrawDeferred();
        }
    }

// ----------------------------------------------------
// CFMRadioChannelListContainer::RemoveChannelL
// Removes channel from container list
// ----------------------------------------------------
//
void CFMRadioChannelListContainer::RemoveChannelL( TInt aIndex )
    {
    if ( iChannelItemArray->Count() > 0 )
        {
        TInt presetIndex = iRadioEngine.GetPresetIndex();
        
        iChannelItemArray->Delete( aIndex );
        iChannelList->HandleItemRemovalL();
        iChannelList->UpdateScrollBarsL();

        if ( iChannelItemArray->Count() > 0 )
            {
            ReIndexAllL();
            
            if ( presetIndex > aIndex )			    
                {
                // update engine settings also
                iRadioEngine.SetCurrentPresetIndex( presetIndex - 1 );
                UpdateItemIconL( presetIndex - 1, KNowPlayingIconIndexChList );
                }
            else if ( presetIndex == aIndex  )
                {
                iRadioEngine.TunePresetL( 0 );	
                UpdateItemIconL( 0, KNowPlayingIconIndexChList );
                }
            else
                {
                // NOP
                }               
            iChannelList->SetCurrentItemIndex( 0 );
            
            }
        else
            {
            // The last item was deleted, tune to current frequency, out of preset mode
            iRadioEngine.Tune( iRadioEngine.GetTunedFrequency(), CRadioEngine::ERadioTunerMode );
            }
        iChannelList->DrawDeferred();
        }
    }

// ----------------------------------------------------
// CFMRadioChannelListContainer::AddChannel
// Adds channels to container list
// ----------------------------------------------------
//
void CFMRadioChannelListContainer::AddChannelL( 
			const TDesC& aChannelName, TInt aChannelFrequency, TBool aNowPlaying  )
	{
	TBuf<KLengthOfChannelItemString> textChannelItem;
	TBuf<KLengthOfChIndexStringChList> textChIndex;
	TBuf<KLengthOfChannelItemIconIndexString> textChIconIndex;
	
	textChIndex.Format( KChIndexFormatChList, iChannelItemArray->Count()+1 ); // One over index
 
   	if ( !AknLayoutUtils::PenEnabled() )
		{
    	textChIconIndex.Format( KChIconIndexFormatChList, KNowPlayingIconIndexChList ) ;
		}    
	
	//Update to display Devnagari numbers for Text Index.
	AknTextUtils::LanguageSpecificNumberConversion(textChIndex);
	//
	textChannelItem.Append( textChIndex );
	textChannelItem.Append( KColumnListSeparator );
	
    TReal frequency = static_cast<TReal>( aChannelFrequency / static_cast<TReal>( KHzConversionFactor ));
    // Gets locale decimal separator automatically
    TRealFormat format(KFrequencyMaxLength, iRadioEngine.DecimalCount() );
    TBuf<30> frequencyString;
    frequencyString.Num(frequency,format);	
	
	TBool channelHasName = EFalse;
	TInt resId = 0;
	// if name exists, place it on the first line
	if ( aChannelName.Length() > 0 )
	    {
	    textChannelItem.Append( aChannelName );
	    textChannelItem.Append( KColumnListSeparator );
	    channelHasName = ETrue;
	    resId = R_QTN_FMRADIO_DOUBLE2_FREQ;
	    }
	else // frequency goes to first line
	    {
	    resId = R_QTN_FMRADIO_DOUBLE1_FREQ;
	    }

	HBufC* stringHolder = StringLoader::LoadLC( resId, frequencyString, iEikonEnv );    
    //Update for display of Hindi Devnagari Numbers
    TPtr textItem2 = stringHolder->Des();
    AknTextUtils::LanguageSpecificNumberConversion(textItem2);
    textChannelItem.Append( textItem2 );
    CleanupStack::PopAndDestroy( stringHolder );
        
    if ( !channelHasName )
        {
        // add placeholder for second line
        textChannelItem.Append( KColumnListSeparator ); 
        }
        
	// Set 'Now Playing' icon to the channel item
    textChannelItem.Append( KColumnListSeparator );
	    		 
	if( !AknLayoutUtils::PenEnabled() && aNowPlaying )
	    {	    
		textChannelItem.Append( textChIconIndex );	
	    }
	
	iChannelItemArray->AppendL(  textChannelItem );
	
	iChannelList->HandleItemAdditionL(); // Update list
	iChannelList->UpdateScrollBarsL();
	iChannelList->DrawDeferred();
	
	}
// ----------------------------------------------------
// CFMRadioChannelListContainer::UpdateItemIconL
// Adds icon to a list item
// ----------------------------------------------------
//
void CFMRadioChannelListContainer::UpdateItemIconL( TInt aIndex, TInt aIconIndex )
    {
    HideIconsL();

    if ( aIndex >= 0 && aIndex < iChannelItemArray->Count() ) 
        {
        TBuf<KLengthOfChannelItemIconIndexString> textChIconIndex;
        textChIconIndex.Format( KChIconIndexFormatChList, aIconIndex );
        HBufC* channelItem = HBufC::NewLC( KLengthOfChannelItemString );
        channelItem->Des().Copy( iChannelItemArray->MdcaPoint( aIndex ) );
        TPtr ptr( channelItem->Des() );
        ptr.Append( textChIconIndex ); // Add icon index
        
        iChannelItemArray->Delete( aIndex );
        iChannelItemArray->InsertL( aIndex, *channelItem );
        CleanupStack::PopAndDestroy( channelItem );
        }
    // use draw now so that view is up to date during fast channel switching
    iChannelList->DrawNow();
    }

// ----------------------------------------------------
// CFMRadioChannelListContainer::HideIconsL
// hide all icons from the list
// ----------------------------------------------------
//
void CFMRadioChannelListContainer::HideIconsL()
    {
    for ( TInt index = 0 ; index < iChannelItemArray->Count(); index++ ) 
        {
        HBufC* channelItem = HBufC::NewLC( KLengthOfChannelItemString );
        channelItem->Des().Copy( iChannelItemArray->MdcaPoint( index ) );
        TPtr ptr( channelItem->Des() );	
        
        TBuf<KLengthOfChannelItemIconIndexString> textChIconIndex;
        TPtrC iconIndexPtr( textChIconIndex );
        TInt err = TextUtils::ColumnText( iconIndexPtr, 3, &ptr );
        
        if ( iconIndexPtr.Length() > 0 )
            {
            ptr.Delete( ptr.Length() - iconIndexPtr.Length(), iconIndexPtr.Length() ); // Remove icon index		
            iChannelItemArray->Delete( index );
            iChannelItemArray->InsertL( index, *channelItem );
            }
        CleanupStack::PopAndDestroy( channelItem );
        }
    }

// ----------------------------------------------------
// CFMRadioChannelListContainer::SetFaded
// Fades the entire window and controls in the window owned
// by this container control.
// ----------------------------------------------------
//
void CFMRadioChannelListContainer::SetFaded( TBool aFaded )
	{
    iFadeStatus = aFaded;
    Window().SetFaded( aFaded, RWindowTreeNode::EFadeIncludeChildren );
	}

// ----------------------------------------------------
// CFMRadioChannelListContainer::UpdateLastListenedChannel
// Updates channel index - the highlighted channel in the list.
// ----------------------------------------------------
//
void CFMRadioChannelListContainer::UpdateLastListenedChannel( TInt aIndex )
	{
    iLastChIndex = aIndex;
	iChannelList->SetCurrentItemIndex( iLastChIndex );
	iChannelList->ScrollToMakeItemVisible( iLastChIndex );
	iChannelList->DrawDeferred();
	}

// ----------------------------------------------------
// CFMRadioChannelListContainer::CurrentlySelectedChannel
// Returns the index of the selected channel item from the
// channel list.
// ----------------------------------------------------
//
TInt CFMRadioChannelListContainer::CurrentlySelectedChannel() const
	{
    return iChannelList->CurrentItemIndex(); // Index of selected channel
	}

// ----------------------------------------------------
// CFMRadioChannelListContainer::HandleListBoxEventL
// For handling listbox-, in this case, channel list events.
// The event equals to selecting a channel item from the list.
// ----------------------------------------------------
//
void CFMRadioChannelListContainer::HandleListBoxEventL( CEikListBox* /*aListBox*/,
                                                      TListBoxEvent aEventType )
    {
    // Check the event generated by keypress and report the event,
    // Also check for Pen Enabled touch screen event

    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
            {
            ReportEventL( MCoeControlObserver::EEventStateChanged );
            break;
            }
        case EEventItemSingleClicked:
            {
            if ( iMoveAction )
                {
                TouchMoveEventL( CurrentlySelectedChannel() ); 
                }
            else
                {
                ReportEventL( MCoeControlObserver::EEventStateChanged );
                }
            break;
            }
        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CFMRadioChannelListContainer::HandleResourceChange
// -----------------------------------------------------------------------------
//
void CFMRadioChannelListContainer::HandleResourceChange(TInt aType)
    {
    CCoeControl::HandleResourceChange(aType);
    if ( aType ==  KEikDynamicLayoutVariantSwitch  )
        {
        SizeChanged();
        }
    else if ( aType == KAknsMessageSkinChange )
        {
        CArrayPtr<CGulIcon>* iconArray = iChannelList->ItemDrawer()->FormattedCellData()->IconArray();
        // update icons with new skin
        iconArray->ResetAndDestroy();
        TRAP_IGNORE( CreateListIconsL( *iconArray ) )
        }
    }
// ---------------------------------------------------------
// CFMRadioChannelListContainer::SizeChanged
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CFMRadioChannelListContainer::SizeChanged()
    {
    // The channel list fills the whole view
    iChannelList->SetRect( Rect() );
    DrawDeferred();
    }

// ---------------------------------------------------------
// CFMRadioChannelListContainer::CountComponentControls
// Return the number of controls in the window owned by this container
// ---------------------------------------------------------
//
TInt CFMRadioChannelListContainer::CountComponentControls() const
    {
    return iControls.Count();
    }

// ---------------------------------------------------------
// CFMRadioChannelListContainer::ComponentControl
// Return the control corresponding to the specified index
// ---------------------------------------------------------
//
CCoeControl* CFMRadioChannelListContainer::ComponentControl( TInt aIndex ) const
    {
    return STATIC_CAST( CCoeControl *,iControls[aIndex] );
    }

// ---------------------------------------------------------
// CFMRadioChannelListContainer::OfferKeyEventL
// Allow the channel list to process key events
// ---------------------------------------------------------
//
TKeyResponse CFMRadioChannelListContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                           TEventCode aType )
    {
    TKeyResponse response = EKeyWasNotConsumed;

    switch ( aKeyEvent.iCode )
        {
        case EKeyBackspace:
            {
            if ( iChannelList->IsHighlightEnabled() && !iMoveAction )
                {
                iChannelView->HandleCommandL( EFMRadioCmdErase );
                return EKeyWasConsumed;
                }
            break;
            }
        case EKeyOK: // accept both
        case EKeyEnter:
            {
            // used to accept move action
            if ( iMoveAction )
                {
                iChannelView->MoveActionDoneL();
                return EKeyWasConsumed;
                }
            else if ( iChannelList->IsHighlightEnabled() ) // change to main view
                {
                if ( iChannelView->CurrentlyPlayingChannel() ==
                        CurrentlySelectedChannel() )
                    {
                    CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( iCoeEnv->AppUi() );
                    appUi->ActivateLocalViewL( KFMRadioMainViewId );
                    return EKeyWasConsumed;
                    }
                }
            break;
            }
        case EKeyLeftArrow:
        case EKeyRightArrow:
            {
            return EKeyWasNotConsumed;
            }
        case EKeyUpArrow:
            if ( iMoveAction )	
                {
                iKeyMoveActivated = ETrue;
                MoveUpL();
                response = EKeyWasConsumed;
                }
            else
                {
                response = iChannelList->OfferKeyEventL(aKeyEvent, aType);
                if (response == EKeyWasConsumed)
                    {
                    ReportEventL( MCoeControlObserver::EEventRequestFocus );
                    }
                }
            return response;
        case EKeyDownArrow:
            if ( iMoveAction )	
                {
                iKeyMoveActivated = ETrue;
                MoveDownL();
                response = EKeyWasConsumed;
                }
            else
                {
                response = iChannelList->OfferKeyEventL(aKeyEvent, aType);
                if (response == EKeyWasConsumed)
                    {
                    ReportEventL( MCoeControlObserver::EEventRequestFocus );
                    }
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
    return iChannelList->OfferKeyEventL(aKeyEvent, aType);
    }

// ---------------------------------------------------------------------------
// CFMRadioChannelListContainer::GetHelpContext
// Gets Help
//
// ---------------------------------------------------------------------------
//
void CFMRadioChannelListContainer::GetHelpContext(TCoeHelpContext& aContext) const
    {
#if defined __SERIES60_HELP || defined FF_S60_HELPS_IN_USE
    aContext.iMajor = TUid::Uid( KUidFMRadioApplication );
    aContext.iContext = KFMRADIO_HLP_LIST;
#endif
    }

// ---------------------------------------------------------
// CFMRadioChannelListContainer::FocusChanged(TDrawNow aDrawNow)
//
// ---------------------------------------------------------
//
void CFMRadioChannelListContainer::FocusChanged(TDrawNow aDrawNow)
    {
    CCoeControl::FocusChanged(aDrawNow);
    if ( iChannelList )
        {
        iChannelList->SetFocus(IsFocused(), aDrawNow);
        }
    }

// ---------------------------------------------------------
// CFMRadioChannelListContainer::HandlePointerEventL
// ---------------------------------------------------------
//
void CFMRadioChannelListContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    // don't handle any pointer events if view is faded
    if ( !iFadeStatus )
        {
        CCoeControl::HandlePointerEventL( aPointerEvent );
        }
    }

// ---------------------------------------------------------
// CFMRadioChannelListContainer::ActivateMove
// ---------------------------------------------------------
//
void CFMRadioChannelListContainer::ActivateMoveL()
    {
    iMoveAction = ETrue;
    iMoveIndex = iChannelList->CurrentItemIndex();
    iTouchMoveIndex = iMoveIndex;
    UpdateItemIconL( iMoveIndex, KMoveIconIndexChList );
    iKeyMoveActivated = EFalse;;
	}

// ---------------------------------------------------------
// CFMRadioChannelListContainer::TouchMoveEventL
// ---------------------------------------------------------
//
void CFMRadioChannelListContainer::TouchMoveEventL( TInt aIndex )
	{
	HBufC* channelItem = HBufC::NewLC( KLengthOfChannelItemString );
	channelItem->Des().Copy( iChannelItemArray->MdcaPoint( iTouchMoveIndex ) );
	TPtr ptr( channelItem->Des() );	
	
	TBuf<KLengthOfChannelItemIconIndexString> textChIconIndex;
	
	TPtrC iconIndexPtr( textChIconIndex );
	
  	TInt err = TextUtils::ColumnText( iconIndexPtr, 3, &ptr );
	
	iChannelItemArray->Delete( iTouchMoveIndex );
    iChannelList->HandleItemRemovalL();    
    
    iChannelItemArray->InsertL( aIndex, *channelItem );
    iChannelList->HandleItemAdditionL(); 
    iChannelList->UpdateScrollBarsL();
    iChannelList->SetCurrentItemIndex( aIndex ); 
	iChannelList->DrawDeferred();
	
	iTouchMoveIndex = aIndex;
	
	CleanupStack::PopAndDestroy( channelItem );
	
    ReIndexAllL();
    UpdateItemIconL( aIndex, KMoveIconIndexChList );
    iChannelView->MoveActionDoneL();
	}

// ---------------------------------------------------------
// CFMRadioChannelListContainer::MoveDownL
// ---------------------------------------------------------
//
void CFMRadioChannelListContainer::MoveDownL()
	{		
	TInt index = iChannelList->CurrentItemIndex();
	TInt originalindex = index;
	
	HBufC* channelItem = HBufC::NewLC( KLengthOfChannelItemString );
	channelItem->Des().Copy( iChannelItemArray->MdcaPoint( index ) );
	TPtr ptr( channelItem->Des() );
	
	iChannelItemArray->Delete( index );
    iChannelList->HandleItemRemovalL();
    
    if ( index < iChannelItemArray->Count() )
    	{
    	index = index+1;
    	   	
    	TBuf<KLengthOfChIndexStringChList> channelindex;
    		    
	    TPtrC indexPtr( channelindex );
	    
	  	TInt errori = TextUtils::ColumnText( indexPtr, 0, &ptr );
	  	
	  	ptr.Delete( 0, indexPtr.Length() );
	  		  	
	  	channelindex.Format( KChIndexFormatChList, index+1 );
	
	   	AknTextUtils::LanguageSpecificNumberConversion( channelindex );
	   	
	   	ptr.Insert( 0, channelindex ); 
	   	
	    iChannelItemArray->InsertL( index, *channelItem );
	    iChannelList->HandleItemAdditionL(); // Update list
	    iChannelList->UpdateScrollBarsL();
	    iChannelList->SetCurrentItemIndex( index );
		iChannelList->DrawDeferred();
		
		HBufC* origchannelItem = HBufC::NewLC( KLengthOfChannelItemString );
		origchannelItem->Des().Copy( iChannelItemArray->MdcaPoint( originalindex ) );
		TPtr origptr( origchannelItem->Des() );	
		
		iChannelItemArray->Delete( originalindex );
		iChannelList->HandleItemRemovalL();
		TBuf<KLengthOfChIndexStringChList> origchannelindex;
		    
		TPtrC origindexPtr( origchannelindex );
		    
		TextUtils::ColumnText( origindexPtr, 0, &origptr );	
		origptr.Delete( 0, origindexPtr.Length() );
		  		  	
		origchannelindex.Format( KChIndexFormatChList, originalindex+1 );
		
		AknTextUtils::LanguageSpecificNumberConversion( origchannelindex );
		origptr.Insert( 0, origchannelindex ); 
		iChannelItemArray->InsertL( originalindex, *origchannelItem );
		iChannelList->HandleItemAdditionL();
		iChannelList->UpdateScrollBarsL();
		iChannelList->DrawDeferred();
		
		CleanupStack::PopAndDestroy( origchannelItem );
    	}
    else
    	{
    	index = 0;
    	iChannelItemArray->InsertL( index, *channelItem );
	    iChannelList->HandleItemAdditionL(); // Update list
	    iChannelList->UpdateScrollBarsL();
	    iChannelList->SetCurrentItemIndex( index );
		iChannelList->DrawDeferred();
		
        ReIndexAllL();
        }
    
    iChannelList->ScrollToMakeItemVisible( index );
    
	CleanupStack::PopAndDestroy( channelItem );

	}
// ---------------------------------------------------------
// CFMRadioChannelListContainer::MoveUpL
// ---------------------------------------------------------
//
void CFMRadioChannelListContainer::MoveUpL()
	{
	TInt index = iChannelList->CurrentItemIndex();
	TInt originalindex = index;
	
	HBufC* channelItem = HBufC::NewLC( KLengthOfChannelItemString );
	channelItem->Des().Copy( iChannelItemArray->MdcaPoint( index ) );
	TPtr ptr( channelItem->Des() );	
	
	iChannelItemArray->Delete( index );
    iChannelList->HandleItemRemovalL();
    
    if ( 0 == index )
    	{
    	index = iChannelItemArray->Count();
    	
    	TBuf<KLengthOfChIndexStringChList> channelindex;
    	    
	    TPtrC indexPtr( channelindex );
	    
	    iChannelItemArray->InsertL( index, *channelItem );
	    iChannelList->HandleItemAdditionL();
	    iChannelList->UpdateScrollBarsL();

	    iChannelList->SetCurrentItemIndex( index );
	    
	    iChannelList->DrawItem( index ); 
	    
        ReIndexAllL();
    	}
    else
    	{
    	index = index-1; 
    
	    TBuf<KLengthOfChIndexStringChList> channelindex;
	    
	    TPtrC indexPtr( channelindex );
	    
	  	TInt errori = TextUtils::ColumnText( indexPtr, 0, &ptr );
	  	
	  	ptr.Delete( 0, indexPtr.Length() );
	  		  	
	  	channelindex.Format( KChIndexFormatChList, index+1 );
	
	   	AknTextUtils::LanguageSpecificNumberConversion( channelindex );
	   	ptr.Insert( 0, channelindex ); // Remove icon index
	
		iChannelItemArray->InsertL( index, *channelItem );
	    iChannelList->SetCurrentItemIndex( index );
	    iChannelList->HandleItemAdditionL();
	    iChannelList->UpdateScrollBarsL();
	    iChannelList->DrawItem( index ); 
		
		HBufC* origchannelItem = HBufC::NewLC( KLengthOfChannelItemString );
		origchannelItem->Des().Copy( iChannelItemArray->MdcaPoint( originalindex ) );
		TPtr origptr( origchannelItem->Des() );	
		
		iChannelItemArray->Delete( originalindex );
	    iChannelList->HandleItemRemovalL();
	    TBuf<KLengthOfChIndexStringChList> origchannelindex;
	        
	    TPtrC origindexPtr( origchannelindex );
	        
		TextUtils::ColumnText( origindexPtr, 0, &origptr );	
		
		origptr.Delete( 0, origindexPtr.Length() );
		  		  	
		origchannelindex.Format( KChIndexFormatChList, originalindex+1 );
		
		AknTextUtils::LanguageSpecificNumberConversion( origchannelindex );
		origptr.Insert( 0, origchannelindex ); 
		
		iChannelItemArray->InsertL( originalindex, *origchannelItem );
		iChannelList->HandleItemAdditionL();
		iChannelList->DrawDeferred();

		iChannelList->UpdateScrollBarsL();
		
		CleanupStack::PopAndDestroy( origchannelItem );
    	 }
    
    iChannelList->ScrollToMakeItemVisible( index );
    		
	CleanupStack::PopAndDestroy( channelItem );
	}

// ---------------------------------------------------------
// CFMRadioChannelListContainer::ReIndexAllL
// ---------------------------------------------------------
//
void CFMRadioChannelListContainer::ReIndexAllL()
	{
	HBufC* channelItem = HBufC::NewLC( KLengthOfChannelItemString );
			
	for ( TInt index = 0; index < iChannelItemArray->Count(); index++ )
		{
		channelItem->Des().Copy( iChannelItemArray->MdcaPoint( index ) );
		TPtr ptr( channelItem->Des() );	
		
		iChannelItemArray->Delete( index );
	    iChannelList->HandleItemRemovalL();
	    
	    TBuf<KLengthOfChIndexStringChList> channelindex;
	    
	    TPtrC indexPtr( channelindex );
	    
	  	TInt errori = TextUtils::ColumnText( indexPtr, 0, &ptr );
	  	
	  	ptr.Delete( 0, indexPtr.Length() );
	  		  	
	  	channelindex.Format( KChIndexFormatChList, index+1 );

	   	AknTextUtils::LanguageSpecificNumberConversion( channelindex );
	   	
	   	ptr.Insert( 0, channelindex ); // Remove icon index
			    
	    iChannelItemArray->InsertL( index, *channelItem );
	    
        iChannelList->HandleItemAdditionL();
        iChannelList->DrawDeferred();
        iChannelList->UpdateScrollBarsL();
		}
		
	CleanupStack::PopAndDestroy( channelItem );
	}

// ---------------------------------------------------------
// CFMRadioChannelListContainer::HandleDropL
// ---------------------------------------------------------
//
void CFMRadioChannelListContainer::HandleDropL()
    {
    iMoveAction = EFalse;
    
    if ( iMoveIndex != iChannelList->CurrentItemIndex() )
        {
        iObserver.UpdateChannelsL( EMoveChannels, 
                iMoveIndex, 
                iChannelList->CurrentItemIndex() );
        }
    HideIconsL();
    iChannelList->DrawDeferred();
    }
// ---------------------------------------------------------
// CFMRadioChannelListContainer::MoveDoneL
// ---------------------------------------------------------
//
void CFMRadioChannelListContainer::MoveDoneL()
	{
	if ( iMoveAction )
		{
		iMoveAction = EFalse;
		}
	
    iObserver.UpdateChannelsL( EStoreAllToRepository, 0, 0 );
	}

// ---------------------------------------------------------
// CFMRadioChannelListContainer::MoveCanceledL
// ---------------------------------------------------------
//
TBool CFMRadioChannelListContainer::MoveCanceledL()
    {
    TBool wasmove = EFalse;
    
    if ( iMoveAction )
        {
        HideIconsL();
        iMoveAction = EFalse;
        
        if ( iKeyMoveActivated )
            {
			TInt index = iChannelList->CurrentItemIndex();
            HBufC* channelItem = HBufC::NewLC( KLengthOfChannelItemString );
            channelItem->Des().Copy( iChannelItemArray->MdcaPoint( index ) );
            TPtr ptr( channelItem->Des() );	
            
            TBuf<KLengthOfChannelItemIconIndexString> textChIconIndex;
            TPtrC iconIndexPtr( textChIconIndex );
            TInt err = TextUtils::ColumnText( iconIndexPtr, 3, &ptr );
            iChannelItemArray->Delete( index );
            iChannelList->HandleItemRemovalL();
            
            iChannelItemArray->InsertL( iMoveIndex, *channelItem );
            iChannelList->HandleItemAdditionL();
            iChannelList->SetCurrentItemIndex( iMoveIndex );
            CleanupStack::PopAndDestroy( channelItem );
            iChannelList->ScrollToMakeItemVisible( index );
            }
        iChannelList->UpdateScrollBarsL();
        iChannelList->DrawDeferred();
        wasmove = ETrue;
		}
	
	return wasmove;
	}

// ---------------------------------------------------------
// CFMRadioChannelListContainer::MoveAction()
// ---------------------------------------------------------
//
TBool CFMRadioChannelListContainer::MoveAction()
	{
	return iMoveAction;
	
	}

// End of File
