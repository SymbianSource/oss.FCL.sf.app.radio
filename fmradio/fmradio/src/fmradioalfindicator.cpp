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
* Description:  Implementation of the class CFMRadioAlfIndicator
*
*/


// INCLUDE FILES

#include <alf/alfenv.h>
#include <alf/alftextvisual.h>
#include <alf/alfevent.h>
#include <alf/alftransformation.h>
#include <alf/alftextstyle.h>

#include "fmradioalfindicator.h"

// CONSTANTS

const TInt  KIndicatorFadeInDefaultDurationTime = 500;
const TInt  KIndicatorFadeOutDefaultDurationTime = 500;
const TReal KDefaultOpacityInVisibleState = 1.0f;
const TReal KDefaultOpacityInHiddenState = 0.0f;

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CFMRadioAlfIndicator::NewL
// Two-phase constructor of CFMRadioAlfIndicator
// ---------------------------------------------------------------------------
//
CFMRadioAlfIndicator* CFMRadioAlfIndicator::NewL( CAlfEnv& aEnv )
	{
	CFMRadioAlfIndicator* self = new (ELeave) CFMRadioAlfIndicator( );
	CleanupStack::PushL(self);
	self->ConstructL( aEnv );
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfIndicator::CFMRadioAlfIndicator
// Default constructor
// ----------------------------------------------------------------------------
//
CFMRadioAlfIndicator::CFMRadioAlfIndicator( ) 
:  iTextStyleId( KErrNotFound ),
   iOpacityInVisibleState( KDefaultOpacityInVisibleState ),
   iOpacityInHiddenState( KDefaultOpacityInHiddenState )
	{
	//No implementation needed
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfIndicator::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CFMRadioAlfIndicator::ConstructL( CAlfEnv& aEnv )
	{	    
	CAlfControl::ConstructL( aEnv ); 
    AddIndicatorLayerL();   
    Hide( EFalse );
  	}

// ---------------------------------------------------------------------------
// ~CFMRadioAlfIndicator::~CFMRadioAlfIndicator
// Destructor
// ---------------------------------------------------------------------------
//
CFMRadioAlfIndicator::~CFMRadioAlfIndicator()
	{
	if( iTextStyleId != KErrNotFound )
		{
		CAlfTextStyleManager& manager = Env().TextStyleManager();
		manager.DeleteTextStyle( iTextStyleId );
		}
	} 

// ---------------------------------------------------------------------------
// CFMRadioAlfIndicator::OfferEventL
// From CAlfControl, takes care of alfred event handling. 
// ---------------------------------------------------------------------------
//
TBool CFMRadioAlfIndicator::OfferEventL( const TAlfEvent& /*aEvent*/ )
	{
	return EFalse;
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfIndicator::SetRect
// Sets the indicator rectangle. 
// ---------------------------------------------------------------------------
//
void CFMRadioAlfIndicator::SetRect( const TRect& aRect )
	{
	iRect = aRect;
	TRAP_IGNORE( SetTextStyleL() );
	if( iIndicatorTextAnchor )
		{
		SetAbsoluteCornerAnchors( iIndicatorTextAnchor, 0, iRect.iTl, iRect.iBr );
		iIndicatorTextAnchor->UpdateChildrenLayout();
		}
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfIndicator::AddIndicatorLayerL
// Creates the needed drawing layers and visual objects
// for the indicator. 
// ---------------------------------------------------------------------------
//
void CFMRadioAlfIndicator::AddIndicatorLayerL()
	{   
	// Create an anchor for the indicator layout 
	iIndicatorTextAnchor = CAlfAnchorLayout::AddNewL( *this );	
	
	// Create visual object for the indicator
	iText = CAlfTextVisual::AddNewL( *this, iIndicatorTextAnchor );
	iText->SetColor( iTextColor );
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfIndicator::SetTextStyleL
// Creates the text style for indicator
// ---------------------------------------------------------------------------
//
void CFMRadioAlfIndicator::SetTextStyleL()
	{
    CAlfTextStyleManager& manager = Env().TextStyleManager();   
    if ( iTextStyleId == KErrNotFound )
        {
        iTextStyleId = manager.CreatePlatformTextStyleL( 
                EAknLogicalFontSecondaryFont,
                EAlfTextStyleNormal );
        }
    iText->SetTextStyle( iTextStyleId );
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfIndicator::SetTextColor
// Changes indicator text color.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfIndicator::SetTextColor( const TRgb& aTextColor )
	{
	iTextColor = aTextColor;
	if( iText )
		{
		iText->SetColor( aTextColor );	
		}
	}

// ---------------------------------------------------------
// CFMRadioAlfIndicator::SetTextL
// Sets the indicator text.
// ---------------------------------------------------------
//
void CFMRadioAlfIndicator::SetTextL( const TDesC& aText ) 
	{	 
	iText->SetTextL( aText );
	}

// ---------------------------------------------------------
// CFMRadioAlfIndicator::Show
// Shows the indicator with the defined opacity value.
// ---------------------------------------------------------
//
void CFMRadioAlfIndicator::Show( TBool aShowWithFading )
	{
	if( aShowWithFading )
		{
		Fade( iText, KIndicatorFadeInDefaultDurationTime, iOpacityInVisibleState );
		}
	else
		{
		Fade( iText, 0, iOpacityInVisibleState );
		}	
	}

// ---------------------------------------------------------
// CFMRadioAlfIndicator::Hide
// Hides the indicator with the defined opacity value.
// ---------------------------------------------------------
//
void CFMRadioAlfIndicator::Hide( TBool aHideWithFading )
	{
	if( aHideWithFading )
		{
		Fade( iText, KIndicatorFadeOutDefaultDurationTime, iOpacityInHiddenState );		
		}
	else
		{
		Fade( iText, 0, iOpacityInHiddenState );	
		}
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfIndicator::SetOpacityInVisibleState
// Sets the indicator opacity in visible state.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfIndicator::SetOpacityInVisibleState( const TReal aOpacity )
	{
	iOpacityInVisibleState = aOpacity;
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfIndicator::SetOpacityInHiddenState
// Sets the indicator opacity in hidden state.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfIndicator::SetOpacityInHiddenState( const TReal aOpacity )
	{
	iOpacityInHiddenState = aOpacity;
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfIndicator::SetAbsoluteCornerAnchors
// Sets absolute rect of the anchor by top left and bottom right points.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfIndicator::SetAbsoluteCornerAnchors( CAlfAnchorLayout* aAnchor,
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

// ---------------------------------------------------------------------------
// CFMRadioAlfIndicator::SetAbsoluteCornerAnchors
// Sets absolute rect of the anchor by top left point and size of the rect
// ---------------------------------------------------------------------------
//	
void CFMRadioAlfIndicator::SetAbsoluteCornerAnchors( CAlfAnchorLayout* aAnchor,
												   	 TInt aOrdinal,
													 const TPoint& aTopLeftPosition,
													 const TSize& aSize )
	{
	if( aAnchor )
		{
		// set top left corner position
		aAnchor->SetAnchor( EAlfAnchorTopLeft, aOrdinal,
		EAlfAnchorOriginLeft, 
		EAlfAnchorOriginTop,
		EAlfAnchorMetricAbsolute, 
		EAlfAnchorMetricAbsolute, 
		TAlfTimedPoint( aTopLeftPosition.iX, aTopLeftPosition.iY ) );
		// .. and set the bottom right corner also to fix the size
		aAnchor->SetAnchor( EAlfAnchorBottomRight, aOrdinal,
		EAlfAnchorOriginLeft, 
		EAlfAnchorOriginTop,
		EAlfAnchorMetricAbsolute, 
		EAlfAnchorMetricAbsolute,
		TAlfTimedPoint( aTopLeftPosition.iX + aSize.iWidth, aTopLeftPosition.iY + aSize.iHeight ) );			
		}
	}
	
// ---------------------------------------------------------------------------
// CFMRadioAlfIndicator::SetRelativeCornerAnchors
// Sets relative rect of the anchor by top left and bottom right points.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfIndicator::SetRelativeCornerAnchors( CAlfAnchorLayout* aAnchor,
													 TInt aOrdinal,
													 const TAlfRealPoint& aTopLeftPosition,
													 const TAlfRealPoint& aBottomRightPosition )
	{
	if( aAnchor )
		{
		aAnchor->SetRelativeAnchorRect( aOrdinal,
    	EAlfAnchorOriginLeft, EAlfAnchorOriginTop, aTopLeftPosition,
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop, aBottomRightPosition );		
		}
	}
	
// ---------------------------------------------------------------------------
// CFMRadioAlfIndicator::SetRelativeCornerAnchors
// Sets relative rect of the anchor by top left point and size of the rect
// ---------------------------------------------------------------------------
//	
void CFMRadioAlfIndicator::SetRelativeCornerAnchors( CAlfAnchorLayout* aAnchor,
													 TInt aOrdinal,
													 const TAlfRealPoint& aTopLeftPosition,
													 const TAlfRealSize& aSize )
	{
	if( aAnchor )
		{
		aAnchor->SetRelativeAnchorRect( aOrdinal,
    	EAlfAnchorOriginLeft, EAlfAnchorOriginTop, aTopLeftPosition,
        EAlfAnchorOriginLeft, EAlfAnchorOriginTop, aTopLeftPosition + TAlfRealPoint( aSize.iWidth, aSize.iHeight ) );		
		}
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfIndicator::Fade
// Sets the fading animation to the CAlfVisual object.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfIndicator::Fade( CAlfVisual* aVisual, TInt aFadingTime, TReal aOpacity ) const
	{
	TAlfTimedValue opacity;
	opacity.SetTarget( aOpacity, aFadingTime ); // and smooth target
	aVisual->SetOpacity( opacity );
	}

//  End of File  

