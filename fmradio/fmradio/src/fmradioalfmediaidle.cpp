/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the class CFMRadioAlfMediaIdle
*
*/


// INCLUDE FILES

#include <aknlayoutscalable_apps.cdl.h>
#include <alf/alfenv.h>
#include <alf/alftextvisual.h>
#include <alf/alfevent.h>
#include <alf/alftransformation.h>
#include <alf/alfgradientbrush.h>
#include <alf/alfbrusharray.h>
#include <alf/alfborderbrush.h>
#include <alf/alfimagebrush.h>
#include <alf/alftexturemanager.h>
#include <alf/alfviewportlayout.h>
#include <alf/alfdecklayout.h>
#include <alf/alfflowlayout.h> 
#include <alf/alfutil.h>
#include <alf/alftextstyle.h>
#include <AknsItemID.h>
#include <data_caging_path_literals.hrh>
#include <e32math.h>
#include <eikenv.h>
#include <StringLoader.h>

#include "fmradioalfmediaidle.h"
#include "fmradioappui.h"
#include "debug.h"

// CONSTANTS


_LIT8( KMediaIdleAnchorTag, "mediaIdleAnchor" );
_LIT8( KMediaIdleViewportTag, "mediaIdleViewport" );
_LIT8( KMediaIdleFlowLayoutTag, "mediaIdleFlowLayout" );
_LIT8( KMediaIdleBackgroundTextTag, "mediaIdleBackgroundText" );
_LIT8( KMediaIdleItemTag, "mediaIdleItem" );

const TInt KNumberOfTextStyles = 4;
const TReal KRelativeFontSizeArray[KNumberOfTextStyles] = {0.12f, 0.17f, 0.25f, 0.33f};
const TReal KRelativeSizeOfMediaIdleBackgroundFont = 0.7f;

const TInt KMediaIdleFadeOutDurationTime = 500;
const TInt KMediaIdleFadeInDurationTime = 500;

const TInt  KMediaIdleBackgroundTextScrollInterval = 12000;
const TReal KMediaIdleBackgroundTextOpacity = 0.2f;
const TInt  KMediaIdleBackgroundTextSideToSideTransitionDurationTime = 25000;
const TInt  KMediaIdleBackgroundTextYTransition = 10;

//const TInt KMediaIdleModeChangeInterval = 20000;

const TInt KMaxNumberOfMediaIdleItems = 5;
const TInt KMaxMediaIdleItemScrollDurationTime = 35000;
const TInt KMinMediaIdleItemScrollDurationTime = 25000;
const TInt KMinMediaIdleItemHideDurationTime = 5000;
const TInt KModeChangingFactor = 3;

const TReal KMaxOpacity = 0.3f;
const TReal KMinOpacity = 0.1f;

const TRgb KMediaIdleItemDefaultColor(0, 0, 0);  
const TRgb KMediaIdleItemBackgroundColor(255, 255, 255); 
const TReal KMediaIdleItemBackgroundOpacity = 0.4f;

const TInt KMediaIdleItemArrayGranularity = 4;

const TInt KLAFVarietyRadioIdlePortrait = 0;
const TInt KLAFVarietyRadioIdleLandscape = 1;


// ============================ MEMBER FUNCTIONS ===============================


// ============================ LOCAL FUNCTIONS ===============================

namespace LocalFunctions {

// ---------------------------------------------------------------------------
// fullyVerticallyVisibleRect
// Check does the given rect fit vertically into a CAlfLayout area
// ---------------------------------------------------------------------------
//
static TBool fullyVerticallyVisibleRect( const CAlfLayout& aLayout, const TAlfRealRect & aRect )
	{	
	TBool ret = ETrue;
	TInt layoutHeight = aLayout.Size().IntTarget().iY;
	TInt bottomY = aRect.BottomLeft().iY;
	TInt topY = aRect.TopRight().iY;
	
	if ( bottomY >= layoutHeight || topY < 0 ) ret = EFalse;
	
	return ret;
	}
// ---------------------------------------------------------------------------
// updateItemVisibility
// Hide (or unhide) an visual depending is it going to be not fully visible or not
// ---------------------------------------------------------------------------
//
static void updateItemVisibility( const CAlfLayout& aLayout, TInt index )
	{
	CAlfVisual& visual = aLayout.Visual( index );
	if ( !LocalFunctions::fullyVerticallyVisibleRect( aLayout, visual.DisplayRectTarget() ) )
		{
		visual.SetOpacity( TAlfTimedValue( 0.0f, KMinMediaIdleItemHideDurationTime ) );
		}
	}
}

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::NewL
// Two-phase constructor of CFMRadioAlfMediaIdle
// ---------------------------------------------------------------------------
//
CFMRadioAlfMediaIdle* CFMRadioAlfMediaIdle::NewL( CAlfEnv& aEnv )
	{
	CFMRadioAlfMediaIdle* self = new (ELeave) CFMRadioAlfMediaIdle( );
	CleanupStack::PushL(self);
	self->ConstructL( aEnv );
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::CFMRadioAlfMediaIdle
// Default constructor
// ----------------------------------------------------------------------------
//
CFMRadioAlfMediaIdle::CFMRadioAlfMediaIdle() 
:   iFontsOrientation( EOrientationNone ),
    iOrientation( EOrientationNone ),
	iMediaIdleItemBackgroundTextStyleId( KErrNotFound ), 
	iMediaIdleState( EStopped ),
	iMediaIdleMode( EBackgroundTextAndMediaIdleItems ),
	iMediaIdleRequestedMode( EBackgroundTextAndMediaIdleItems ),	
	iMediaIdlePrimaryColor( KRgbBlue ),
	iMediaIdleSecondaryColor( KRgbBlue ),
	iPsName( NULL )
	{
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CFMRadioAlfMediaIdle::ConstructL( CAlfEnv& aEnv )
    {
    CAlfControl::ConstructL( aEnv );
    UpdateLayout();
    iIsConstructed = ETrue;
    }

// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::CreateFontsL
// Create fonts that are based on a list of relative sizes
// ----------------------------------------------------------------------------
//
void CFMRadioAlfMediaIdle::CreateFontsL()
	{
	TInt layoutHeight = 20;
	
	if ( iMediaIdleLayout.Rect().Height() != 0 )
		{
		layoutHeight = iMediaIdleLayout.Rect().Height();
		}
	
	if( iFontsOrientation != iOrientation ||  
		iFontReferenceHeight != layoutHeight )
		{
		CAlfTextStyleManager& manager = Env().TextStyleManager();
		for( TInt i=0; i < iTextStyleIdArray.Count(); i++ )
			{
			manager.DeleteTextStyle( iTextStyleIdArray[i] );
			iTextStyleIdArray.Reset();
			}
		if( iMediaIdleItemBackgroundTextStyleId != KErrNotFound )
			{
			manager.DeleteTextStyle( iMediaIdleItemBackgroundTextStyleId );
			iMediaIdleItemBackgroundTextStyleId = KErrNotFound;
			}

		// Create largest font for the background text of media idle
		iMediaIdleItemBackgroundTextStyleId = manager.CreatePlatformTextStyleL();	
		CAlfTextStyle* mediaIdleItemStyle = manager.TextStyle( iMediaIdleItemBackgroundTextStyleId );					
		mediaIdleItemStyle->SetTextSizeInPixels( KRelativeSizeOfMediaIdleBackgroundFont * layoutHeight, ETrue );
		// Create text styles for other media idle items
		for( TInt i = 0; i < KNumberOfTextStyles; i++ )
			{		
			TInt mediaIdleItemTextStyleId = manager.CreatePlatformTextStyleL();	
			CAlfTextStyle* mediaIdleItemStyle = manager.TextStyle( mediaIdleItemTextStyleId );					
			mediaIdleItemStyle->SetTextSizeInPixels( KRelativeFontSizeArray[i] * layoutHeight, ETrue ); 
			iTextStyleIdArray.Append( mediaIdleItemTextStyleId );
			}
		iFontsOrientation = iOrientation;
		iFontReferenceHeight = layoutHeight;
		}
	}

// ---------------------------------------------------------------------------
// ~CFMRadioAlfMediaIdle::~CFMRadioAlfMediaIdle
// Destructor
// ---------------------------------------------------------------------------
//
CFMRadioAlfMediaIdle::~CFMRadioAlfMediaIdle()
	{
	Env().CancelCustomCommands( this );
	if( iMediaIdleItemArray )
		{
		if( iMediaIdleItemArray->Count() > 0 )
			{
			iMediaIdleItemArray->Reset();	
			}
		delete iMediaIdleItemArray;
		}
	CAlfTextStyleManager& manager = Env().TextStyleManager();
	for( TInt i=0; i < iTextStyleIdArray.Count(); i++ )
		{
		manager.DeleteTextStyle( iTextStyleIdArray[i] );
		}
	iTextStyleIdArray.Close();
    if ( iMediaIdleItemBackgroundTextStyleId != KErrNotFound )
        {
        manager.DeleteTextStyle( iMediaIdleItemBackgroundTextStyleId );
        }
    delete iPsName;
    }
 
// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::AddMediaIdleL
// Creates the media idle with passed number of media idle items
// ----------------------------------------------------------------------------
//
void CFMRadioAlfMediaIdle::AddMediaIdleL( const TInt aNumberOfMediaIdleItems ) 
	{	
	FTRACE( FPrint( _L("CFMRadioAlfMediaIdle::AddMediaIdleL(%d)"), aNumberOfMediaIdleItems ) );
	
	CAlfAnchorLayout* mediaIdleAnchor = static_cast<CAlfAnchorLayout*> ( FindTag( KMediaIdleAnchorTag ) );
	if ( !mediaIdleAnchor )
		{
		mediaIdleAnchor = CAlfAnchorLayout::AddNewL( *this );	
		}
	mediaIdleAnchor->SetTagL( KMediaIdleAnchorTag );
	
	CAlfViewportLayout* mediaIdleViewport = static_cast<CAlfViewportLayout*> ( FindTag( KMediaIdleViewportTag ) );
	if( !mediaIdleViewport )
		{
		mediaIdleViewport = CAlfViewportLayout::AddNewL( *this, mediaIdleAnchor );
		mediaIdleViewport->SetTagL( KMediaIdleViewportTag );
		mediaIdleViewport->SetViewportPos( TAlfRealPoint( .0f, .0f ), 0 );
		mediaIdleViewport->SetViewportSize( TAlfRealSize( 1.0f, 1.0f ), 0 );
		}
	MediaIdleFadeOut( ETrue ); 

	// Create the flow layout for the other media idle items
	CAlfFlowLayout* mediaIdleFlowLayout = static_cast<CAlfFlowLayout*> ( FindTag( KMediaIdleFlowLayoutTag ) );
	if( !mediaIdleFlowLayout )
		{
		mediaIdleFlowLayout = CAlfFlowLayout::AddNewL( *this, mediaIdleViewport );
		mediaIdleFlowLayout->SetTagL( KMediaIdleFlowLayoutTag );
		mediaIdleFlowLayout->SetFlowDirection( CAlfFlowLayout::EFlowVertical );
		mediaIdleFlowLayout->SetMode( CAlfFlowLayout::EModeCenterPerpendicular );
		}
	
	InitializeMediaIdleItemsL( aNumberOfMediaIdleItems );
	UpdateLayout();	
	AnimateMediaIdleItems();
	MediaIdleFadeIn();
	}
	
// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::CreateBackgroundTextL
// Creates backgrounf text for media idle content
// ----------------------------------------------------------------------------
//
void CFMRadioAlfMediaIdle::CreateBackgroundTextL()
	{
	FTRACE( FPrint( _L("CFMRadioAlfMediaIdle::CreateBackgroundTextL()") ) );

	CAlfViewportLayout* mediaIdleViewport = static_cast<CAlfViewportLayout*> ( FindTag( KMediaIdleViewportTag ) );
	// Create and animate the background text of the media idle
	HBufC* itemText = GetRandomMediaIdleContentL();
	CleanupStack::PushL( itemText ); 
	CAlfTextVisual* backgroundText = static_cast<CAlfTextVisual*> ( mediaIdleViewport->FindTag( KMediaIdleBackgroundTextTag ) );
	if( !backgroundText && itemText )
		{	
		backgroundText = AddMediaIdleItemL( mediaIdleViewport, *itemText, EFalse, KMediaIdleItemBackgroundColor, KMediaIdleItemBackgroundOpacity );			
		backgroundText->SetTagL( KMediaIdleBackgroundTextTag );
		SetMediaIdleItemSettingsL( backgroundText, *itemText, iMediaIdleItemBackgroundTextStyleId );
		backgroundText->SetOpacity( TAlfTimedValue( KMediaIdleBackgroundTextOpacity ) ); 
		backgroundText->EnableBrushesL();
		backgroundText->EnableShadow( EFalse );
        backgroundText->SetFlag( EAlfVisualFlagManualSize );
		SetScrollingToBackgroundText( backgroundText,
						   KMediaIdleBackgroundTextSideToSideTransitionDurationTime,
						   EMediaIdleBackgroundTextScrollEnd );	
		}
	else if( itemText )
		{
		SetMediaIdleItemSettingsL( backgroundText, *itemText, iMediaIdleItemBackgroundTextStyleId );
		backgroundText->SetOpacity( TAlfTimedValue( KMediaIdleBackgroundTextOpacity ) ); 
		SetScrollingToBackgroundText( backgroundText, KMediaIdleBackgroundTextSideToSideTransitionDurationTime, EMediaIdleBackgroundTextScrollEnd );
		}
	CleanupStack::PopAndDestroy( itemText ); 
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::InitializeMediaIdleItemsL
// Initializes the media idle items
// ----------------------------------------------------------------------------
//	
void CFMRadioAlfMediaIdle::InitializeMediaIdleItemsL( const TInt aNumberOfMediaIdleItems )
	{
	FTRACE( FPrint( _L("CFMRadioAlfMediaIdle::InitializeMediaIdleItemsL(%d)"), aNumberOfMediaIdleItems ) );

	CAlfFlowLayout* mediaIdleFlowLayout = static_cast<CAlfFlowLayout*> ( FindTag( KMediaIdleFlowLayoutTag ) );
	if( mediaIdleFlowLayout )
		{
		CreateFontsL();
		CreateBackgroundTextL();
		// Create the media idle items 	  	
		for( TInt i = 0, fontStyleIdIx = 0; i < aNumberOfMediaIdleItems; i++, fontStyleIdIx++ )
			{
			HBufC* itemText = GetRandomMediaIdleContentL();
			const TPtrC& text = itemText ? itemText->Des() : KNullDesC();

			CleanupStack::PushL( itemText ); 
            TInt randFontStyleIx = AlfUtil::RandomInt( 0,  iTextStyleIdArray.Count() - 1 );
            CAlfTextVisual* mediaIdleItem = NULL;
            if( mediaIdleFlowLayout->Count() < aNumberOfMediaIdleItems )	
                {
                mediaIdleItem  = AddMediaIdleItemL( mediaIdleFlowLayout, text, EFalse, KMediaIdleItemBackgroundColor, KMediaIdleItemBackgroundOpacity );					
                }
            else
                {
                mediaIdleItem = static_cast<CAlfTextVisual*> ( &mediaIdleFlowLayout->Visual( i ) );
                mediaIdleItem->SetOpacity( TAlfTimedValue( .0f ) );
                mediaIdleItem->EnableTransformationL( ETrue );
                mediaIdleItem->Transformation().LoadIdentity();
                }	
            SetMediaIdleItemSettingsL( mediaIdleItem, text, iTextStyleIdArray[ randFontStyleIx ] );		
            CleanupStack::PopAndDestroy( itemText );
			} 
		}
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::AnimateMediaIdleItems
// Sets defined animation to the created media idle items
// ----------------------------------------------------------------------------
//	
void CFMRadioAlfMediaIdle::AnimateMediaIdleItems()
	{
	FTRACE( FPrint( _L("CFMRadioAlfMediaIdle::AnimateMediaIdleItems()") ) );

	CAlfFlowLayout* mediaIdleFlowLayout = static_cast<CAlfFlowLayout*> ( FindTag( KMediaIdleFlowLayoutTag ) );
	if( mediaIdleFlowLayout )
		{		  	
		for( TInt i = 0; i < mediaIdleFlowLayout->Count(); i++ )
			{	
		    CAlfTextVisual* mediaIdleItem = static_cast<CAlfTextVisual*> ( &mediaIdleFlowLayout->Visual( i ) );
		    if( mediaIdleItem )	
				{
			 	SetMediaIdleItemAnimation( mediaIdleItem );	
				}	
			} 
		}
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::AddMediaIdleItemL
// Creates the media idle item to the passed media idle view port.
// ----------------------------------------------------------------------------
//	
CAlfTextVisual* CFMRadioAlfMediaIdle::AddMediaIdleItemL( CAlfLayout* aMediaIdle, 
                                                         const TDesC& aMediaIdleItemText, 
                                                         TBool aMediaIdleItemHasBackground, 
                                                         const TRgb& aBackgroundColor, 
                                                         const TReal aBackgroundOpacity )
	{
	FTRACE( FPrint( _L("CFMRadioAlfMediaIdle::AddMediaIdleItemL('%S')"), &aMediaIdleItemText ) );
	
	CAlfTextVisual* mediaIdleItem = CAlfTextVisual::AddNewL( *this, aMediaIdle );
    mediaIdleItem->EnableBrushesL();
    mediaIdleItem->SetTextL( aMediaIdleItemText );	
    mediaIdleItem->SetTagL( KMediaIdleItemTag );
    mediaIdleItem->EnableShadow( EFalse );
    mediaIdleItem->SetAlign( EAlfAlignHCenter, EAlfAlignVCenter );
   	// mediaIdleItem->SetFlag( EAlfVisualFlagManualLayout );
    mediaIdleItem->SetSize( mediaIdleItem->TextExtents() );
        
    if ( aMediaIdleItemHasBackground )
	    {
		CAlfGradientBrush* gradientBrush = CAlfGradientBrush::NewLC( Env() );
		gradientBrush->SetColor( aBackgroundColor, aBackgroundOpacity );
		mediaIdleItem->Brushes()->AppendL( gradientBrush, EAlfHasOwnership );
		CleanupStack::Pop( gradientBrush );		
	    }
	return mediaIdleItem;
	}
	
// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::SetMediaIdleItemSettings
// Sets color, opacity and scaling settings to the media idle item
// ----------------------------------------------------------------------------
//	
void CFMRadioAlfMediaIdle::SetMediaIdleItemSettingsL( CAlfTextVisual* aMediaIdleItem, const TDesC& aMediaIdleItemText, const TInt aTextStyleId )
	{
	// Set text of the media idle item
	aMediaIdleItem->SetTextL( aMediaIdleItemText );	
	aMediaIdleItem->SetOpacity( TAlfTimedValue( .0f ) );
	aMediaIdleItem->EnableTransformationL( ETrue );
	CAlfTransformation* mediaIdleItemTransformation = &( aMediaIdleItem->Transformation() );
	mediaIdleItemTransformation->LoadIdentity();	
	
	// Select text color of the media idle item	
	TInt randomColorSelector = AlfUtil::RandomInt( 0, 1 );
	if ( randomColorSelector )
		{
		// Set text color of the media idle item 	
		aMediaIdleItem->SetColor( iMediaIdlePrimaryColor );	
		}
	else
		{
		// Set text color of the media idle item 	
		aMediaIdleItem->SetColor( iMediaIdleSecondaryColor );
		}
				    	
	// Set text style of the media idle	item   
	aMediaIdleItem->SetTextStyle( aTextStyleId );
		
	TSize itemSize( 0, 0 );
	if ( aMediaIdleItem->TextExtents() != itemSize )
		{
		aMediaIdleItem->SetSize( aMediaIdleItem->TextExtents() );	
		}		
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::SetMediaIdleItemAnimation
// Sets translating animations to the media idle item
// ----------------------------------------------------------------------------
//
void CFMRadioAlfMediaIdle::SetMediaIdleItemAnimation( CAlfTextVisual* aMediaIdleItem )
	{	
	FTRACE( FPrint( _L("CFMRadioAlfMediaIdle::SetMediaIdleItemAnimation()") ) );

	CAlfFlowLayout* mediaIdleFlowLayout = static_cast<CAlfFlowLayout*> ( aMediaIdleItem->Layout() );
	TInt mediaIdleItemCount = mediaIdleFlowLayout->Count();
	TInt mediaIdleItemIndex = mediaIdleFlowLayout->FindVisual( aMediaIdleItem );
	
	TInt mediaIdleItemWidth = aMediaIdleItem->DisplayRect().Width();
	TInt mediaIdleWidth = mediaIdleFlowLayout->Size().IntTarget().iX;
	
	TInt randomTranslateDuration = AlfUtil::RandomInt( KMinMediaIdleItemScrollDurationTime, KMaxMediaIdleItemScrollDurationTime );
	TInt scrollDirection = AlfUtil::RandomInt( 0,  1 );
	TAlfTimedValue targetPositionX;
	if( scrollDirection )
		{
		targetPositionX.SetValueNow( -mediaIdleWidth/2 - mediaIdleItemWidth/2 ); 
		targetPositionX.SetTarget( mediaIdleWidth/2 + mediaIdleItemWidth/2, randomTranslateDuration ); 
		}
	else
		{
		targetPositionX.SetValueNow( mediaIdleWidth/2 + mediaIdleItemWidth/2  );
		targetPositionX.SetTarget( -mediaIdleWidth/2 - mediaIdleItemWidth/2, randomTranslateDuration ); 
		}

	Translate( aMediaIdleItem, targetPositionX, TAlfTimedValue(0)); // targetPositionY );

	if( mediaIdleItemIndex >= 0 )
		{
		Env().Send( TAlfCustomEventCommand( EMediaIdleItemTransformationEnd,
											this,
											mediaIdleItemIndex ),
											randomTranslateDuration );
		}
	// Set text opacity of the media idle item
	if( !LocalFunctions::fullyVerticallyVisibleRect( *mediaIdleFlowLayout, aMediaIdleItem->DisplayRect() ) )
		{
		aMediaIdleItem->SetOpacity( TAlfTimedValue( 0.0f ) );
		}
	else
		{
		TReal randomOpacity = AlfUtil::RandomReal( KMinOpacity, KMaxOpacity );
		aMediaIdleItem->SetOpacity( TAlfTimedValue( randomOpacity ) );
		}
	
	mediaIdleFlowLayout->UpdateChildrenLayout( KMinMediaIdleItemScrollDurationTime );
	
	// Checking all items, should they fade or not; must be done after UpdateChildrenLayout
	for (TInt i=0; i<mediaIdleItemCount; i++ ) 
		{
		LocalFunctions::updateItemVisibility( *mediaIdleFlowLayout, i );
		}
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::MediaIdleFadeIn
// Fades in media idle
// ---------------------------------------------------------------------------
//		
void CFMRadioAlfMediaIdle::MediaIdleFadeIn( TBool aNow )
	{
	FTRACE( FPrint( _L("CFMRadioAlfMediaIdle::MediaIdleFadeIn(%d)"), aNow ) );

	CAlfViewportLayout* mediaIdle = static_cast<CAlfViewportLayout*> ( FindTag( KMediaIdleViewportTag ) );
	if ( mediaIdle )
		{	
		if( aNow )
			{
			FadeIn( mediaIdle, 0 );
			}
		else
			{
			FadeIn( mediaIdle, KMediaIdleFadeInDurationTime );	
			}
		}
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::MediaIdleFadeOut
// Fades out media idle
// ---------------------------------------------------------------------------
//	
void CFMRadioAlfMediaIdle::MediaIdleFadeOut( TBool aNow )
	{
	FTRACE( FPrint( _L("CFMRadioAlfMediaIdle::MediaIdleFadeOut(%d)"), aNow ) );

	CAlfViewportLayout* mediaIdle = static_cast<CAlfViewportLayout*> ( FindTag( KMediaIdleViewportTag ) );
	if ( mediaIdle )
		{		
		if( aNow )
			{
			FadeOut( mediaIdle, 0 );
			}
		else
			{
			FadeOut( mediaIdle, KMediaIdleFadeOutDurationTime );	
			}
		}
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::StartMediaIdleL
// Launches media idle.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfMediaIdle::StartMediaIdleL() 
    {
    FTRACE( FPrint( _L("CFMRadioAlfMediaIdle::StartMediaIdleL()") ) );

    if ( iMediaIdleState == EStopped )
        {
        Env().CancelCustomCommands( this );
        iMediaIdleState = ERunning;
        AddMediaIdleL( KMaxNumberOfMediaIdleItems );
        }
    }

// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::AddMediaIdleContentL
// Adds media idle item text to be used in the media idle
// ----------------------------------------------------------------------------
//		
void CFMRadioAlfMediaIdle::AddMediaIdleContentL( const TDesC& aMediaIdleItemText )
	{
	FTRACE( FPrint( _L("CFMRadioAlfMediaIdle::AddMediaIdleContentL('%S')"), &aMediaIdleItemText ) );

	if( !iMediaIdleItemArray )
		{
		// Array for media idle items
    	iMediaIdleItemArray = new( ELeave ) CDesCArrayFlat( KMediaIdleItemArrayGranularity );
    	iMediaIdleItemArray->AppendL( aMediaIdleItemText );
		}
	else
	    {
	    TInt searchResultIndex = 0;
	    if ( iMediaIdleItemArray->Find( aMediaIdleItemText, searchResultIndex ) )
	        {
	        // text wasn't added earlier
	        iMediaIdleItemArray->AppendL( aMediaIdleItemText );
	        }
	    }
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::GetRandomMediaIdleContentL
// Retrieves text from randomly chousen media idle item
// ----------------------------------------------------------------------------
//	
HBufC* CFMRadioAlfMediaIdle::GetRandomMediaIdleContentL()	
	{
	if( !iMediaIdleItemArray )
		{	
		return NULL;		
		}
	TInt count = iMediaIdleItemArray->MdcaCount();
	if( count > 0 )
    	{  
    	TInt mediaIdleItemIndex;
    	mediaIdleItemIndex = AlfUtil::RandomInt( 0, count - 1 );
    	TPtrC itemContent( iMediaIdleItemArray->MdcaPoint( mediaIdleItemIndex ) );
    	return itemContent.AllocL();
    	}
    else
    	{
    	return NULL;	
    	}
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::ResetMediaIdleContent
// Removes all media idle item texts
// ----------------------------------------------------------------------------
//
void  CFMRadioAlfMediaIdle::ResetMediaIdleContent()
	{
	FTRACE( FPrint( _L("CFMRadioAlfMediaIdle::ResetMediaIdleContent()") ) );

	delete iPsName;
	iPsName = NULL;
	if( iMediaIdleItemArray )
		{
		if( iMediaIdleItemArray->Count() > 0 )
			{
			iMediaIdleItemArray->Reset();	
			iMediaIdleItemArray->Compress();
			}	
		}	
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::MediaIdleContentCount
// Returns number of media idle content items  
// ----------------------------------------------------------------------------
//
TInt CFMRadioAlfMediaIdle::MediaIdleContentCount() const
	{
	if( iMediaIdleItemArray )
		{
		return iMediaIdleItemArray->Count();
		}
	else
		{
		return 0;
		}
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::SetPrimaryColor
// Sets primary color for the media idle
// ----------------------------------------------------------------------------
//
void CFMRadioAlfMediaIdle::SetPrimaryColor( const TRgb& aColor )		
	{
	iMediaIdlePrimaryColor = aColor;	
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::SetSecondaryColor
// Sets secondary color for the media idle
// ----------------------------------------------------------------------------
//
void CFMRadioAlfMediaIdle::SetSecondaryColor( const TRgb& aColor )		
	{
	iMediaIdleSecondaryColor = aColor;	
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::Translate
// Translates the visual object with passed arguments
// ----------------------------------------------------------------------------
//	
void CFMRadioAlfMediaIdle::Translate( CAlfTextVisual* aTextVisual, const TAlfTimedValue& aX, const TAlfTimedValue& aY )
	{
	if( aTextVisual )
		{
		TRAPD( err, aTextVisual->EnableTransformationL( ETrue ) );
		if( err == KErrNone )
			{
			CAlfTransformation* visualTransformation = &( aTextVisual->Transformation() );
			TAlfTimedValue x(aX);
			x.SetStyle( EAlfTimedValueStyleLinear );
			
			TAlfTimedValue y(aY);
			y.SetStyle( EAlfTimedValueStyleLinear );
			
			visualTransformation->Translate( x, y );
			}			
		}
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::LoadTextVisualIdentity
// Removes all transformations of the visual object such as the scaling and translating.
// ----------------------------------------------------------------------------
//
void CFMRadioAlfMediaIdle::LoadTextVisualIdentity( CAlfTextVisual* aTextVisual )
	{
	if( aTextVisual )
		{
		TRAPD( err, aTextVisual->EnableTransformationL( ETrue ) );
		if( err == KErrNone )
			{
			CAlfTransformation* visualTransformation = &( aTextVisual->Transformation() );
			visualTransformation->LoadIdentity();		
			}
		}
	}
	
// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::UpdateLayout
// Calculates display layout
// ----------------------------------------------------------------------------
//
void CFMRadioAlfMediaIdle::UpdateLayout()
	{
	FTRACE( FPrint( _L("CFMRadioAlfMediaIdle::UpdateLayout()") ) );
	
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );   
    TRect displayRect( mainPaneRect.Size() );   	

    TInt lafVarietyMediaIdle = 0;

    // Adjust positions of the visual elements 
    if ( iOrientation == EPortrait || iOrientation == EOrientationNone )
        {
        lafVarietyMediaIdle = KLAFVarietyRadioIdlePortrait;		
        }
    else if ( iOrientation == ELandscape )
        {
        lafVarietyMediaIdle = KLAFVarietyRadioIdleLandscape;
        }

    iMediaIdleLayout.LayoutRect( displayRect,
            AknLayoutScalable_Apps::area_fmrd2_visual_pane( lafVarietyMediaIdle ).LayoutLine() );

	if ( iIsConstructed )
		{
		CAlfVisual* thirdVisual = FindTag( KMediaIdleAnchorTag );
		if ( thirdVisual )
			{
			CAlfAnchorLayout* mediaIdleAnchor = static_cast<CAlfAnchorLayout*>( thirdVisual );
			SetAbsoluteCornerAnchors( mediaIdleAnchor, 0, iMediaIdleLayout.Rect().iTl, iMediaIdleLayout.Rect().iBr  );
			mediaIdleAnchor->UpdateChildrenLayout();
			}
		}
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::SetModeChangeRequest
// Sets request for mode change
// ----------------------------------------------------------------------------
//
void CFMRadioAlfMediaIdle::SetModeChangeRequestL( TMediaIdleMode aRequestedMode )
	{
	FTRACE( FPrint( _L("CFMRadioAlfMediaIdle::SetModeChangeRequestL(%d)"), aRequestedMode ) );

	iMediaIdleRequestedMode = aRequestedMode;
	if( iMediaIdleRequestedMode == EBackgroundTextAndMediaIdleItems && 
		Mode() == EOnlyBackgroundText )
		{
		InitializeMediaIdleItemsL( KMaxNumberOfMediaIdleItems );
		AnimateMediaIdleItems();
		SetMode( EBackgroundTextAndMediaIdleItems );
		}
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::SetMode
// Sets the media idle mode
// ----------------------------------------------------------------------------
//
void CFMRadioAlfMediaIdle::SetMode( TMediaIdleMode aMode )
	{
	FTRACE( FPrint( _L("CFMRadioAlfMediaIdle::SetMode(%d)"), aMode ) );

	iMediaIdleMode = aMode;
	}
	
// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::Mode
// Retrieves the media idle mode
// ----------------------------------------------------------------------------
//
CFMRadioAlfMediaIdle::TMediaIdleMode CFMRadioAlfMediaIdle::Mode()
	{
	return iMediaIdleMode;
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::Orientation
// Return orientation of the display
// ----------------------------------------------------------------------------
//
CFMRadioAlfMediaIdle::TOrientation CFMRadioAlfMediaIdle::Orientation()
	{
	return iOrientation;
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::Orientation
// Set orientation of the display
// ----------------------------------------------------------------------------
//		
void CFMRadioAlfMediaIdle::SetOrientation( TOrientation aOrientation )
	{
	iOrientation = aOrientation;
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::OfferEventL
// From CAlfControl, takes care of alfred event handling.
// ----------------------------------------------------------------------------
//
TBool CFMRadioAlfMediaIdle::OfferEventL( const TAlfEvent& aEvent )
	{
	TBool isHandled = EFalse;
	if( aEvent.IsCustomEvent() )
		{
		FTRACE( FPrint( _L("CFMRadioAlfMediaIdle::OfferEventL(aEvent.CustomParameter()=%d)"), aEvent.CustomParameter() ) );

		switch( aEvent.CustomParameter() )
			{
			case EMediaIdleItemTransformationEnd:
				{
				HandleMediaIdleItemTransformationEndEventL( aEvent );	
				isHandled = ETrue;
				}
				break;
			case EMediaIdleBackgroundTextScrollEnd:
				{	
				HandleBackgroundTextScrollEndEventL( aEvent );		
				isHandled = ETrue;
				}
				break;
			case EMediaIdleChangeMode:
				{	
				if( Mode() == EBackgroundTextAndMediaIdleItems )
					{
					SetModeChangeRequestL( EOnlyBackgroundText );
					}
				else
					{
					SetModeChangeRequestL( EBackgroundTextAndMediaIdleItems );
					}
				isHandled = ETrue;
				}
				break;
			default:
			break;
			}
		}
	return isHandled;
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::HandleMediaIdleItemTransformationEndEventL
// ----------------------------------------------------------------------------
//
void CFMRadioAlfMediaIdle::HandleMediaIdleItemTransformationEndEventL( const TAlfEvent& aEvent )
	{
	FTRACE( FPrint( _L("CFMRadioAlfMediaIdle::HandleMediaIdleItemTransformationEndEventL()") ) );
	
	iItemsTransformationEndEventCounter++;
	if ( iMediaIdleRequestedMode == EOnlyBackgroundText || Mode() == EOnlyBackgroundText )
		{			
		CAlfFlowLayout* mediaIdleFlowLayout = static_cast<CAlfFlowLayout*> ( FindTag( KMediaIdleFlowLayoutTag ) );			
		if ( mediaIdleFlowLayout )
			{					
			TInt mediaIdleItemIndex = aEvent.CustomEventData(); 
			CAlfTextVisual* mediaIdleItem = static_cast<CAlfTextVisual*> ( &mediaIdleFlowLayout->Visual( mediaIdleItemIndex ) );
			if ( mediaIdleItem )
				{
				mediaIdleItem->SetOpacity( TAlfTimedValue( .0f ) ); 
				}
			
			if( iItemsTransformationEndEventCounter == mediaIdleFlowLayout->Count() )
				{
				SetMode( EOnlyBackgroundText ); 
				iItemsTransformationEndEventCounter = 0;
				Env().Send( TAlfCustomEventCommand( EMediaIdleBackgroundTextScrollEnd, this ), 0 );
				}
			}
		}
	else 
		{ 
		CAlfFlowLayout* mediaIdleFlowLayout = static_cast<CAlfFlowLayout*> ( FindTag( KMediaIdleFlowLayoutTag ) );			
		if ( mediaIdleFlowLayout )
			{
			HBufC* itemText = GetRandomMediaIdleContentL();
			if( itemText )
				{
				CleanupStack::PushL( itemText ); 
				TInt mediaIdleItemIndex = aEvent.CustomEventData(); 
				CAlfTextVisual* mediaIdleItem = static_cast<CAlfTextVisual*> ( &mediaIdleFlowLayout->Visual( mediaIdleItemIndex ) );
				if ( mediaIdleItem && iTextStyleIdArray.Count() > 0 )
					{
			    	// First hide the media idle item	
					mediaIdleItem->SetOpacity( TAlfTimedValue( .0f ) );
					mediaIdleItem->EnableTransformationL( ETrue );
					mediaIdleItem->Transformation().LoadIdentity();	
										
					TInt randFontStyleIx = AlfUtil::RandomInt( 0,  iTextStyleIdArray.Count() - 1 );
					SetMediaIdleItemSettingsL( mediaIdleItem, *itemText,  iTextStyleIdArray[ randFontStyleIx ] );				
					SetMediaIdleItemAnimation( mediaIdleItem );	
					
					if ( iMediaIdleRequestedMode != EOnlyBackgroundText )
						{
						if( iItemsTransformationEndEventCounter == mediaIdleFlowLayout->Count() )
							{
							Env().Send( TAlfCustomEventCommand( EMediaIdleChangeMode, this ),
									KMaxMediaIdleItemScrollDurationTime * KModeChangingFactor );
							iItemsTransformationEndEventCounter = 0;
							}
						}
					}
				CleanupStack::PopAndDestroy( itemText ); 
				}
			}
		}
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::HandleBackgroundTextScrollEndEventL
// ----------------------------------------------------------------------------
//
void CFMRadioAlfMediaIdle::HandleBackgroundTextScrollEndEventL( const TAlfEvent& /*aEvent*/ )
	{
    FTRACE( FPrint( _L("CFMRadioAlfMediaIdle::HandleBackgroundTextScrollEndEventL()") ) );
	   
	CAlfViewportLayout* mediaIdleViewport = static_cast<CAlfViewportLayout*> ( FindTag( KMediaIdleViewportTag ) );
	if( mediaIdleViewport )
		{
		CAlfTextVisual* backgroundText = static_cast<CAlfTextVisual*> ( mediaIdleViewport->FindTag( KMediaIdleBackgroundTextTag ) );
		if( backgroundText )
			{				
			backgroundText->SetOpacity( TAlfTimedValue( .0f ) );

			if ( iMediaIdleRequestedMode == EOnlyBackgroundText && Mode() ==  EBackgroundTextAndMediaIdleItems )
				{
				}
			else
				{
				if( iMediaIdleRequestedMode == EOnlyBackgroundText && 
						Mode() == EOnlyBackgroundText )
					{
					Env().CancelCustomCommands( this );
					Env().Send( TAlfCustomEventCommand( EMediaIdleChangeMode, this ),
							KMediaIdleBackgroundTextSideToSideTransitionDurationTime );
					}
				backgroundText->EnableTransformationL( ETrue );
				backgroundText->Transformation().LoadIdentity();
				HBufC* itemText = GetRandomMediaIdleContentL();
				if( itemText )
					{
					CleanupStack::PushL( itemText ); 
					SetMediaIdleItemSettingsL( backgroundText, *itemText, iMediaIdleItemBackgroundTextStyleId );
					CleanupStack::PopAndDestroy( itemText );
					}
				backgroundText->SetOpacity( TAlfTimedValue( KMediaIdleBackgroundTextOpacity ) );
				SetScrollingToBackgroundText( backgroundText,
								   KMediaIdleBackgroundTextSideToSideTransitionDurationTime,
								   EMediaIdleBackgroundTextScrollEnd );		
				}	
			}
		}
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::SetAbsoluteCornerAnchors
// Sets absolute rect of the anchor by top left and bottom right points.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfMediaIdle::SetAbsoluteCornerAnchors( CAlfAnchorLayout* aAnchor,
                                                     TInt aOrdinal,
                                                     const TPoint& aTopLeftPosition,
                                                     const TPoint& aBottomRightPosition )
    {
    if ( aAnchor )
        {
        // Set top/left anchor.
        aAnchor->Attach( aOrdinal, 
                         EAlfAnchorTypeTopLeft,
                         TAlfXYMetric( TAlfMetric( aTopLeftPosition.iX ), TAlfMetric( aTopLeftPosition.iY ) ),
                         EAlfAnchorAttachmentOriginTopLeft );
        
        // Set bottom/right anchor.
        aAnchor->Attach( aOrdinal, 
                         EAlfAnchorTypeBottomRight, 
                         TAlfXYMetric( TAlfMetric( aBottomRightPosition.iX ), TAlfMetric( aBottomRightPosition.iY ) ),
                         EAlfAnchorAttachmentOriginTopLeft );
        }
    }

// ----------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::SetScrollingToBackgroundText
// ----------------------------------------------------------------------------
//
void CFMRadioAlfMediaIdle::SetScrollingToBackgroundText( CAlfTextVisual* aVisual, TInt aTransitionTime, TMediaIdleCustomEvent aEventAfterScrollingEnd )
    {
    FTRACE( FPrint( _L("CFMRadioAlfMediaIdle::SetScrollingToBackgroundText(aTransitionTime=%d)"), aTransitionTime ) );
    
    CAlfViewportLayout* mediaIdle = static_cast<CAlfViewportLayout*> ( aVisual->Layout() );
    TInt mediaIdleItemWidth = aVisual->DisplayRect().Width();
    TInt mediaIdleWidth = mediaIdle->Size().IntTarget().iX;
    
    TAlfTimedValue targetPositionX;
    if ( iBackgroundTextScrollDirection == EScrollRightToLeft )
        {
        targetPositionX.SetValueNow(  -mediaIdleItemWidth );
        targetPositionX.SetTarget( mediaIdleWidth, aTransitionTime );
        iBackgroundTextScrollDirection = EScrollLeftToRight;
        }
    else
        { 	
        targetPositionX.SetValueNow( mediaIdleWidth );
        targetPositionX.SetTarget( -mediaIdleItemWidth, aTransitionTime ); 
        iBackgroundTextScrollDirection = EScrollRightToLeft;
        }
    Translate( aVisual, targetPositionX, TAlfTimedValue( KMediaIdleBackgroundTextYTransition ) );
    Env().Send( TAlfCustomEventCommand( aEventAfterScrollingEnd, this ), 
                aTransitionTime + KMediaIdleBackgroundTextScrollInterval );
    }

// ---------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::FadeIn
// Sets fade-in animation to the CAlfVisual.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfMediaIdle::FadeIn( CAlfVisual* aVisual, TInt aTime, TReal aOpacity ) const
	{
	FTRACE( FPrint( _L("CFMRadioAlfMediaIdle::FadeIn(aVisual=0x%x, aTime=%d)"), aVisual, aTime ) );
	
	TAlfTimedValue opacity;
	opacity.SetValueNow( 0.0f ); // immediate change
	opacity.SetTarget( aOpacity, aTime ); // and smooth target
	aVisual->SetOpacity( opacity );
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::FadeOut
// Sets fade-out animation to the CAlfVisual.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfMediaIdle::FadeOut( CAlfVisual* aVisual, TInt aTime, TReal aOpacity ) const
	{
	FTRACE( FPrint( _L("CFMRadioAlfMediaIdle::FadeOut(aVisual=0x%x, aTime=%d)"), aVisual, aTime ) );
	TAlfTimedValue opacity;
	opacity.SetTarget( aOpacity, aTime ); 
	aVisual->SetOpacity( opacity );
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::AddPsNameToMediaIdleL
// Adds PS name to the media idle item array
// ---------------------------------------------------------------------------
//
void CFMRadioAlfMediaIdle::AddPsNameToMediaIdleL( const TDesC& aPsName )
	{
	FTRACE( FPrint( _L("CFMRadioAlfMediaIdle::AddPsNameToMediaIdleL('%S')"), &aPsName ) );
	
	if ( !iMediaIdleItemArray )
		{
		// Array for media idle items
    	iMediaIdleItemArray = new ( ELeave ) CDesCArrayFlat( KMediaIdleItemArrayGranularity );
    	iPsName = aPsName.AllocL();
    	iMediaIdleItemArray->AppendL( *iPsName );
    	return;
		}
	else if ( !iPsName ) 	
		{	
		// PS name is added for the first time or media idle item array is reseted			
		if ( iMediaIdleItemArray->Count() < KFMRadioMaxNumberOfMediaIdleContentItem )
			{
			iPsName = aPsName.AllocL();							
			iMediaIdleItemArray->AppendL( *iPsName );
			}
		return;
		}
		
	TInt returnValue;
	TInt searchResultIndex;	
	
	returnValue = iMediaIdleItemArray->Find( *iPsName, searchResultIndex );
		
	if ( returnValue == 0 ) 
		{
		// if old PS name is found, replace it with new name
        iMediaIdleItemArray->InsertL( searchResultIndex, aPsName );
        iMediaIdleItemArray->Delete( searchResultIndex + 1 );
		}
						
	delete iPsName;
	iPsName = NULL;
	iPsName = aPsName.AllocL();			
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::StopAndFadeOutMediaIdle
// stops media idle and clears out animation and background image
// ---------------------------------------------------------------------------
//
void CFMRadioAlfMediaIdle::StopAndFadeOutMediaIdle()
    {
    FTRACE( FPrint( _L("CFMRadioAlfMediaIdle::StopAndFadeOutMediaIdle() - iMediaIdleState was %d"), iMediaIdleState ) );

    if ( iMediaIdleState != EStopped )
        {
        Env().CancelCustomCommands( this );
        iMediaIdleState = EStopped;
        MediaIdleFadeOut();
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::Show
// from MFMRadioIdleControlInterface
// ---------------------------------------------------------------------------
//
void CFMRadioAlfMediaIdle::Show()
    {
    TRAP_IGNORE( StartMediaIdleL() )
    }

// ---------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::Hide
// from MFMRadioIdleControlInterface
// ---------------------------------------------------------------------------
//
void CFMRadioAlfMediaIdle::Hide()
    {
    StopAndFadeOutMediaIdle();
    }

// ---------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::Deactivate
// from MFMRadioIdleControlInterface
// ---------------------------------------------------------------------------
//
void CFMRadioAlfMediaIdle::Deactivate()
    {
    StopAndFadeOutMediaIdle();
    }

//  End of File
