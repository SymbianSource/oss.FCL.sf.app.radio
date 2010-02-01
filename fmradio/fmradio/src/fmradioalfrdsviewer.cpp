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

#include <e32cmn.h>
#include <alf/alfenv.h>
#include <alf/alftextvisual.h>
#include <alf/alfevent.h>
#include <alf/alftransformation.h>
#include <alf/alftextstyle.h>

// brushes
#include <alf/alfgradientbrush.h>
#include <alf/alfbrusharray.h>
#include <alf/alfborderbrush.h>
#include <alf/alfimagebrush.h>

#include "debug.h"
#include "fmradioalfrdsviewer.h"
#include "fmradioalfrdsviewobserver.h"
// CONSTANTS

const TInt  KIndicatorFadeInDefaultDurationTime = 500;
const TInt  KIndicatorFadeOutDefaultDurationTime = 500;
const TInt  KRDSDisplayPeriod = 60000; //milliseconds = 1 min.
const TReal KDefaultOpacityInVisibleState = 1.0f;
const TReal KDefaultOpacityInHiddenState = 0.0f;
const TReal KRelativeFontSize = 0.10;

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CFMRadioAlfIndicator::NewL
// Two-phase constructor of CFMRadioAlfIndicator
// ---------------------------------------------------------------------------
//
CFMRadioAlfRDSViewer* CFMRadioAlfRDSViewer::NewL( CAlfEnv& aEnv )
	{
	CFMRadioAlfRDSViewer* self = new (ELeave) CFMRadioAlfRDSViewer( );
	CleanupStack::PushL(self);
	self->ConstructL( aEnv );
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfRDSViewer::CFMRadioAlfRDSViewer
// Default constructor
// ----------------------------------------------------------------------------
//
CFMRadioAlfRDSViewer::CFMRadioAlfRDSViewer()
:  iTextStyleId( KErrNotFound ),
   iOpacityInVisibleState( KDefaultOpacityInVisibleState ),
   iOpacityInHiddenState( KDefaultOpacityInHiddenState ),
   iIsRdsTextVisible( EFalse ),
   iRdsTextVisibilityTimerCompleted( ETrue )
    {
    //No implementation needed
    }


// ----------------------------------------------------------------------------
// CFMRadioAlfRDSViewer::SetObserver
// Sets observer
// ----------------------------------------------------------------------------
//
void CFMRadioAlfRDSViewer::SetObserver( MFMRadioAlfRdsViewObserver* aObserver )
    {
	FTRACE( FPrint( _L( "CFMRadioAlfRDSViewer::SetObserver" ) ) );
    TInt index = iObservers.FindInAddressOrder( aObserver );
    if ( index == KErrNotFound )
        {
        iObservers.InsertInAddressOrder( aObserver );
        }
    }

// ----------------------------------------------------------------------------
// CFMRadioAlfRDSViewer::RemoveObserver
// Removes observer
// ----------------------------------------------------------------------------
//
void CFMRadioAlfRDSViewer::RemoveObserver( MFMRadioAlfRdsViewObserver* aObserver )
    {
	FTRACE( FPrint( _L( "CFMRadioAlfRDSViewer::RemoveObserver" ) ) );
    TInt index = iObservers.FindInAddressOrder( aObserver );

    if ( index >= 0 )
        {
        iObservers.Remove( index );
        }
    }

// ----------------------------------------------------------------------------
// CFMRadioAlfRDSViewer::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CFMRadioAlfRDSViewer::ConstructL( CAlfEnv& aEnv )
    {
    FTRACE( FPrint( _L( "CFMRadioAlfRDSViewer::ConstructL" ) ) );
    CAlfControl::ConstructL( aEnv );
    AddIndicatorLayerL();
    Hide();
    }

// ---------------------------------------------------------------------------
// ~CFMRadioAlfRDSViewer::~CFMRadioAlfRDSViewer
// Destructor
// ---------------------------------------------------------------------------
//
CFMRadioAlfRDSViewer::~CFMRadioAlfRDSViewer()
    {
    FTRACE( FPrint( _L( "CFMRadioAlfRDSViewer::Destructor" ) ) );
    Env().CancelCustomCommands( this );
    if( iTextStyleId != KErrNotFound )
        {
        CAlfTextStyleManager& manager = Env().TextStyleManager();
        manager.DeleteTextStyle( iTextStyleId );
        }
    iRadioText.Close();
    iObservers.Close();
    } 

// ---------------------------------------------------------------------------
// CFMRadioAlfRDSViewer::OfferEventL
// From CAlfControl, takes care of alfred event handling. 
// ---------------------------------------------------------------------------
//
TBool CFMRadioAlfRDSViewer::OfferEventL( const TAlfEvent& aEvent )
    {
    FTRACE( FPrint( _L( "CFMRadioAlfRDSViewer::OfferEventL" ) ) );
    TBool eventHandled = EFalse;

    if ( aEvent.IsCustomEvent() )
        {
        switch( aEvent.CustomParameter() )
            {
        	case EFadeInCompleted:
                {
                eventHandled = ETrue;
                Env().Send( TAlfCustomEventCommand( ERDSDisplayTimerCompleted, this ), KRDSDisplayPeriod );
                break;
                }
            case EOldTextFadeEffectCompleted:
                {
                eventHandled = ETrue;
                iText->SetTextL( iRadioText );
                // fade in the new radio text
                Fade( iText, KIndicatorFadeInDefaultDurationTime, iOpacityInVisibleState );
                Env().Send( TAlfCustomEventCommand( EFadeInCompleted, this ), KIndicatorFadeOutDefaultDurationTime );				
                break;
                }
            case ERDSDisplayTimerCompleted:
                {
                eventHandled = ETrue;
                iRdsTextVisibilityTimerCompleted = ETrue;
                iIsRdsTextVisible = EFalse;
                Fade( iText, KIndicatorFadeOutDefaultDurationTime, iOpacityInHiddenState );
                for ( TInt i = 0 ; i < iObservers.Count() ; i++ )
                    {
                    iObservers[i]->NotifyRdsTextTimerCompletion();
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        }
    return eventHandled;
    }

// ---------------------------------------------------------------------------
// CFMRadioAlfRDSViewer::SetRect
// Sets the indicator rectangle. 
// ---------------------------------------------------------------------------
//
void CFMRadioAlfRDSViewer::SetRect( const TRect& aRect )
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
// CFMRadioAlfRDSViewer::AddIndicatorLayerL
// Creates the needed drawing layers and visual objects
// for the indicator. 
// ---------------------------------------------------------------------------
//
void CFMRadioAlfRDSViewer::AddIndicatorLayerL()
	{   
	// Create an anchor for the indicator layout 
	iIndicatorTextAnchor = CAlfAnchorLayout::AddNewL( *this );	
	
	// Create visual object for the indicator
	iText = CAlfTextVisual::AddNewL( *this, iIndicatorTextAnchor );
	iText->SetColor( iTextColor );
	iText->SetAlign( EAlfAlignHCenter, EAlfAlignVCenter );	
	iText->SetWrapping( CAlfTextVisual::ELineWrapBreak );
	iText->SetClipping( ETrue );
	
	// shadow enablers
	//iText->EnableDropShadowL( ETrue );
	//iText->EnableShadow( ETrue );
	
	iText->EnableBrushesL();
	
	/*CAlfGradientBrush* gradientBrush = CAlfGradientBrush::NewLC( Env() );
	gradientBrush->SetColor( KRgbBlack, 0.7f );
	iText->Brushes()->AppendL( gradientBrush, EAlfHasOwnership );
	CleanupStack::Pop( gradientBrush );	
	*/
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfRDSViewer::SetTextStyleL
// Creates the text style for indicator
// ---------------------------------------------------------------------------
//
void CFMRadioAlfRDSViewer::SetTextStyleL()
	{
	CAlfTextStyleManager& manager = Env().TextStyleManager();
	CAlfTextStyle* textStyle = NULL;
	if( iTextStyleId == KErrNotFound )
		{
		iTextStyleId = manager.CreatePlatformTextStyleL( EAlfTextStyleNormal );
		}
	
	textStyle = manager.TextStyle( iTextStyleId );
	// 5 lines 
	textStyle->SetTextSizeInPixels( ( iRect.Height()*KRelativeFontSize ), ETrue );
	//textStyle->SetTextPaneHeightInPixels(iRect.Height(), ETrue ); 
	iText->SetTextStyle( iTextStyleId );
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfRDSViewer::SetTextColor
// Changes indicator text color.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfRDSViewer::SetTextColor( const TRgb& aTextColor )
	{
	iTextColor = aTextColor;
	if( iText )
		{
		iText->SetColor( aTextColor );	
		}
	}

// ---------------------------------------------------------
// CFMRadioAlfRDSViewer::SetTextL
// Sets the indicator text.
// ---------------------------------------------------------
//
TBool CFMRadioAlfRDSViewer::SetTextL( const TDesC& aText )
    {
    TBool result = EFalse;
    if ( aText.Length() > 0 )
        {
        if ( iRadioText.Compare( aText ) != 0 )
            {
            iRdsTextVisibilityTimerCompleted = EFalse;
            Env().CancelCustomCommands( this, ERDSDisplayTimerCompleted );

            iRadioText.Close();
            iRadioText.Create( aText );

            if ( !iIsRdsTextVisible )
                {
                iText->SetTextL( aText );
                }
            else
                {
                // fade out the old radio text
                Fade( iText, KIndicatorFadeOutDefaultDurationTime, iOpacityInHiddenState );
                Env().Send( TAlfCustomEventCommand( EOldTextFadeEffectCompleted, this ), KIndicatorFadeOutDefaultDurationTime );
                }
            result = ETrue;
            }
        }
    return result;
    }

// ---------------------------------------------------------
// CFMRadioAlfRDSViewer::Show
// Shows the indicator with the defined opacity value.
// ---------------------------------------------------------
//
void CFMRadioAlfRDSViewer::Show()
    {    
    if ( !iIsRdsTextVisible )
        {
        iIsRdsTextVisible = ETrue;
        Fade( iText, KIndicatorFadeInDefaultDurationTime, iOpacityInVisibleState );
        Env().Send( TAlfCustomEventCommand( EFadeInCompleted, this ), KIndicatorFadeOutDefaultDurationTime );
        }
    }

// ---------------------------------------------------------
// CFMRadioAlfRDSViewer::Hide
// Hides the indicator with the defined opacity value.
// ---------------------------------------------------------
//
void CFMRadioAlfRDSViewer::Hide()
    {
    if ( iIsRdsTextVisible )
        {
        // cancel timer event
        Env().CancelCustomCommands( this, ERDSDisplayTimerCompleted );
        iIsRdsTextVisible = EFalse;
        Fade( iText, KIndicatorFadeOutDefaultDurationTime, iOpacityInHiddenState );
        }
    }

// ---------------------------------------------------------
// CFMRadioAlfRDSViewer::Reset
// Cancel events and flush radio text cache
// ---------------------------------------------------------
//
void CFMRadioAlfRDSViewer::Reset()
    {
    FTRACE( FPrint( _L( "CFMRadioAlfRDSViewer::Reset" ) ) );
    iRdsTextVisibilityTimerCompleted = ETrue;
    iRadioText.Close();
    Env().CancelCustomCommands( this );
    }
// ---------------------------------------------------------------------------
// CFMRadioAlfRDSViewer::SetOpacityInVisibleState
// Sets the indicator opacity in visible state.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfRDSViewer::SetOpacityInVisibleState( const TReal aOpacity )
	{
	iOpacityInVisibleState = aOpacity;
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfRDSViewer::SetOpacityInHiddenState
// Sets the indicator opacity in hidden state.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfRDSViewer::SetOpacityInHiddenState( const TReal aOpacity )
	{
	iOpacityInHiddenState = aOpacity;
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfRDSViewer::SetAbsoluteCornerAnchors
// Sets absolute rect of the anchor by top left and bottom right points.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfRDSViewer::SetAbsoluteCornerAnchors( CAlfAnchorLayout* aAnchor,
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
// CFMRadioAlfRDSViewer::SetAbsoluteCornerAnchors
// Sets absolute rect of the anchor by top left point and size of the rect
// ---------------------------------------------------------------------------
//	
void CFMRadioAlfRDSViewer::SetAbsoluteCornerAnchors( CAlfAnchorLayout* aAnchor,
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
// CFMRadioAlfRDSViewer::SetRelativeCornerAnchors
// Sets relative rect of the anchor by top left and bottom right points.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfRDSViewer::SetRelativeCornerAnchors( CAlfAnchorLayout* aAnchor,
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
// CFMRadioAlfRDSViewer::SetRelativeCornerAnchors
// Sets relative rect of the anchor by top left point and size of the rect
// ---------------------------------------------------------------------------
//	
void CFMRadioAlfRDSViewer::SetRelativeCornerAnchors( CAlfAnchorLayout* aAnchor,
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
// CFMRadioAlfRDSViewer::Fade
// Sets the fading animation to the CAlfVisual object.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfRDSViewer::Fade( CAlfVisual* aVisual, TInt aFadingTime, TReal aOpacity ) const
	{
	TAlfTimedValue opacity;
	opacity.SetTarget( aOpacity, aFadingTime ); // and smooth target
	aVisual->SetOpacity( opacity );
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfRDSViewer::IsShowingRdsTextArea
// ---------------------------------------------------------------------------
//
TBool CFMRadioAlfRDSViewer::IsShowingRdsTextArea() const
    {
    return iIsRdsTextVisible;
    }

// ---------------------------------------------------------------------------
// CFMRadioAlfRDSViewer::Deactivate
// from MFMRadioIdleControlInterface
// ---------------------------------------------------------------------------
//
void CFMRadioAlfRDSViewer::Deactivate()
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioAlfRDSViewer::IsRdsTextVisibilityTimerCompleted
// ---------------------------------------------------------------------------
//
TBool CFMRadioAlfRDSViewer::IsRdsTextVisibilityTimerCompleted()
    {
    return iRdsTextVisibilityTimerCompleted;
    }

//  End of File  
