/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the class CFMRadioLogo
*
*/


// INCLUDE FILES

#include <AknUtils.h>
#include <alf/alfenv.h>
#include <alf/alfimagevisual.h>
#include <alf/alfevent.h>
#include <alf/alfanchorlayout.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <data_caging_path_literals.hrh>
#include <fmradio.mbg>

#include "fmradiologo.h"
#include "fmradioalfrdsviewobserver.h"
#include "fmradiodefines.h"
#include "debug.h"
#include "fmradiologoobserver.h"

// CONSTANTS

const TInt  KLogoFadeInDefaultDurationTime = 500;
const TInt  KLogoFadeOutDefaultDurationTime = 500;
const TInt  KLogoDisplayPeriod = 2000; // milliseconds = 2 sec
const TReal KDefaultOpacityInVisibleState = 1.0f;
const TReal KDefaultOpacityInHiddenState = 0.0f;
const TInt KLAFVarietyBackgroundImagePortrait = 0;
const TInt KLAFVarietyBackgroundImageLandscape = 1;
// bitmap file for the background of the display
_LIT8( KBitmapAnchorTag, "BitmapAnchorTag" );

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CFMRadioLogo::NewL
// Two-phase constructor of CFMRadioAlfLogo
// ---------------------------------------------------------------------------
//
CFMRadioLogo* CFMRadioLogo::NewL( CAlfEnv& aEnv )
    {
    CFMRadioLogo* self = new ( ELeave ) CFMRadioLogo();
    CleanupStack::PushL( self );
    self->ConstructL( aEnv );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CFMRadioLogo::CFMRadioLogo
// Default constructor
// ----------------------------------------------------------------------------
//
CFMRadioLogo::CFMRadioLogo()
:  iOpacityInVisibleState( KDefaultOpacityInVisibleState ),
   iOpacityInHiddenState( KDefaultOpacityInHiddenState ),
   iIsVisible( ETrue ) // visible by default
    {
    //No implementation needed
    }


// ----------------------------------------------------------------------------
// CFMRadioLogo::SetObserver
// Sets observer
// ----------------------------------------------------------------------------
//
void CFMRadioLogo::SetObserver( MFMRadioLogoObserver* aObserver )
    {
    FTRACE( FPrint( _L( "CFMRadioLogo::SetObserver" ) ) );
    TInt index = iObservers.FindInAddressOrder( aObserver );
    if ( index == KErrNotFound )
        {
        iObservers.InsertInAddressOrder( aObserver );
        }
    }

// ----------------------------------------------------------------------------
// CFMRadioLogo::RemoveObserver
// Removes observer
// ----------------------------------------------------------------------------
//
void CFMRadioLogo::RemoveObserver( MFMRadioLogoObserver* aObserver )
    {
	FTRACE( FPrint( _L( "CFMRadioLogo::RemoveObserver" ) ) );
    TInt index = iObservers.FindInAddressOrder( aObserver );

    if ( index >= 0 )
        {
        iObservers.Remove( index );
        }
    }

// ----------------------------------------------------------------------------
// CFMRadioLogo::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CFMRadioLogo::ConstructL( CAlfEnv& aEnv )
    {
    FTRACE( FPrint( _L( "CFMRadioLogo::ConstructL" ) ) );
    CAlfControl::ConstructL( aEnv );
    CreateImageVisualsL();
    }

// ---------------------------------------------------------------------------
// ~CFMRadioLogo::~CFMRadioLogo
// Destructor
// ---------------------------------------------------------------------------
//
CFMRadioLogo::~CFMRadioLogo()
    {
    FTRACE( FPrint( _L( "CFMRadioLogo::Destructor" ) ) );
    Env().CancelCustomCommands( this );
    iObservers.Close();
    delete iBackgroundBitmapFileName;
    } 

// ---------------------------------------------------------------------------
// CFMRadioLogo::OfferEventL
// From CAlfControl, takes care of alfred event handling. 
// ---------------------------------------------------------------------------
//
TBool CFMRadioLogo::OfferEventL( const TAlfEvent& aEvent )
    {
    FTRACE( FPrint( _L( "CFMRadioLogo::OfferEventL" ) ) );
    TBool eventHandled = EFalse;

    if ( aEvent.IsCustomEvent() )
        {
        switch( aEvent.CustomParameter() )
            {
        	case EFadeInCompleted:
                {
                eventHandled = ETrue;
                Env().Send( TAlfCustomEventCommand( ELogoDisplayTimerCompleted, this ), KLogoDisplayPeriod );
                break;
                }
            case ELogoDisplayTimerCompleted:
                {
                eventHandled = ETrue;
                for ( TInt i = 0 ; i < iObservers.Count() ; i++ )
                    {
                    iObservers[i]->LogoDisplayTimeCompleted();
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
// CFMRadioLogo::SetRect
// Sets the logo rectangle. 
// ---------------------------------------------------------------------------
//
void CFMRadioLogo::SetRect( const TRect& aRect )
    {
    iRect = aRect;
    if ( iLogoAnchor )
        {
        SetAbsoluteCornerAnchors( iLogoAnchor, 0, iRect.iTl, iRect.iBr );
        iLogoAnchor->UpdateChildrenLayout();
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioLogo::CreateImageVisualsL
// Creates visual for the logo
// ---------------------------------------------------------------------------
//
void CFMRadioLogo::CreateImageVisualsL()
    {
    // locate source mbm
    CCoeEnv* coeEnv = CCoeEnv::Static();
    TFindFile finder( coeEnv->FsSession() );
    TInt err = finder.FindByDir( KFMRadioBmpFile, KDC_APP_BITMAP_DIR );
    if ( err == KErrNone )
        {
        iBackgroundBitmapFileName = finder.File().AllocL();
        }

    iLogoAnchor = CAlfAnchorLayout::AddNewL( *this );
    iLogoAnchor->SetTagL( KBitmapAnchorTag );

    iImageVisual = CAlfImageVisual::AddNewL( *this, iLogoAnchor );

    // read bitmap size from layout data
    TRect temp;
    TAknLayoutRect bitmapLayout;
    bitmapLayout.LayoutRect(
            temp,
            AknLayoutScalable_Apps::area_fmrd2_visual_pane_g1( KLAFVarietyBackgroundImagePortrait ).LayoutLine() );

    // image for portrait
    iPortraitImage = TAlfImage( KAknsIIDNone,
                                       bitmapLayout.Rect().Size(),
                                       EAspectRatioPreserved,
                                       iBackgroundBitmapFileName,
                                       EMbmFmradioQgn_indi_radio_default,
                                       EMbmFmradioQgn_indi_radio_default_mask );

    // image for landscape
    bitmapLayout.LayoutRect(
            temp,
            AknLayoutScalable_Apps::area_fmrd2_visual_pane_g1( KLAFVarietyBackgroundImageLandscape ).LayoutLine() );    

    iLandscapeImage = TAlfImage( KAknsIIDNone,
                                 bitmapLayout.Rect().Size(),
                                 EAspectRatioPreserved,
                                 iBackgroundBitmapFileName,
                                 EMbmFmradioQgn_indi_radio_default,
                                 EMbmFmradioQgn_indi_radio_default_mask );
    
    iImageVisual->SetImage( iPortraitImage );
    iImageVisual->SetSecondaryImage( iLandscapeImage );
    }

// ---------------------------------------------------------
// CFMRadioLogo::Show
// Fades in the logo with predefined opacity value.
// ---------------------------------------------------------
//
void CFMRadioLogo::Show()
    {
    // change status only, fade in and the event are allways triggered so don't
    // check visibility here
    iIsVisible = ETrue; 
    Env().CancelCustomCommands( this );
    Fade( iImageVisual, KLogoFadeInDefaultDurationTime, iOpacityInVisibleState );
    Env().Send( TAlfCustomEventCommand( EFadeInCompleted, this ), KLogoFadeInDefaultDurationTime );
    }

// ---------------------------------------------------------
// CFMRadioLogo::Hide
// Fades out the logo with predefined opacity value.
// ---------------------------------------------------------
//
void CFMRadioLogo::Hide()
    {
    if ( iIsVisible )
        {
        iIsVisible = EFalse;
        Fade( iImageVisual, KLogoFadeOutDefaultDurationTime, iOpacityInHiddenState );
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioLogo::SetOpacityInVisibleState
// Sets the indicator opacity in visible state.
// ---------------------------------------------------------------------------
//
void CFMRadioLogo::SetOpacityInVisibleState( const TReal aOpacity )
	{
	iOpacityInVisibleState = aOpacity;
	}

// ---------------------------------------------------------------------------
// CFMRadioLogo::SetOpacityInHiddenState
// Sets the logo opacity value in hidden state.
// ---------------------------------------------------------------------------
//
void CFMRadioLogo::SetOpacityInHiddenState( const TReal aOpacity )
	{
	iOpacityInHiddenState = aOpacity;
	}

// ---------------------------------------------------------------------------
// CFMRadioLogo::SetAbsoluteCornerAnchors
// Sets absolute rect of the anchor by top left and bottom right points.
// ---------------------------------------------------------------------------
//
void CFMRadioLogo::SetAbsoluteCornerAnchors( CAlfAnchorLayout* aAnchor,
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
// CFMRadioLogo::Fade
// Sets the fading animation to the CAlfVisual object.
// ---------------------------------------------------------------------------
//
void CFMRadioLogo::Fade( CAlfVisual* aVisual, TInt aFadingTime, TReal aOpacity )
    {
    TAlfTimedValue opacity;
    opacity.SetTarget( aOpacity, aFadingTime );
    aVisual->SetOpacity( opacity );
    }

// ---------------------------------------------------------------------------
// CFMRadioLogo::SwitchToLandscapeImage
// Use secondary image for landscape and primary for portrait
// ---------------------------------------------------------------------------
//
void CFMRadioLogo::SwitchToLandscapeImage( TBool aShow )
    {
    if ( aShow )
        {
        iImageVisual->SetSecondaryAlpha( TAlfTimedValue( 1.0 ) );
        }
    else
        {
        iImageVisual->SetSecondaryAlpha( TAlfTimedValue( 0.0 ) );
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioLogo::Deactivate
// from MFMRadioIdleControlInterface
// ---------------------------------------------------------------------------
//
void CFMRadioLogo::Deactivate()
    {
    }

//  End of File  
