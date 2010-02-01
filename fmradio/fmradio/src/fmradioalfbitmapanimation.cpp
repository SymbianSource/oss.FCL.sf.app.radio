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
* Description:  Tuning animation for fmradio
*
*/

#include <aknlayoutscalable_apps.cdl.h>
#include <AknUtils.h>
#include <alf/alfimage.h>
#include <alf/alfimagevisual.h>
#include <alf/alfevent.h>
#include <AknsItemID.h>
#include <barsread.h>
#include <data_caging_path_literals.hrh>
#include <fmradio.rsg>
#include <alf/alfanchorlayout.h>
#include <alf/alfenv.h>

#include "fmradioalfbitmapanimation.h"
#include "fmradiodefines.h"

// CONSTANTS
//const TInt KBmpAnimationDelay = 90;
const TInt KBmpAnimationStartupDelay = 500;
const TInt KBmpAnimationFadeInDefaultDurationTime = 200;
const TInt KBmpAnimationFadeOutDefaultDurationTime = 200;
const TInt KbmpAnimationStopDelay = 210;

const TReal KDefaultOpacityInVisibleState = 1.0f;
const TReal KDefaultOpacityInHiddenState = 0.0f;

_LIT8( KBmpAnimationAnchorTag, "BitmapAnimationAnchorTag" );
// values from fmradio LAF document
const TInt KLAFVarietyTuningAnimationPortrait = 7;

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CFMRadioAlfBitmapAnimation::CFMRadioAlfBitmapAnimation
// Default constructor
// ----------------------------------------------------------------------------
//
CFMRadioAlfBitmapAnimation::CFMRadioAlfBitmapAnimation() :
	iOpacityInVisibleState( KDefaultOpacityInVisibleState ),
 	iOpacityInHiddenState( KDefaultOpacityInHiddenState ),
 	iAnimationState( EBmpAnimationStopped )
	{
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfBitmapAnimation::ConstructL
// ---------------------------------------------------------------------------
//
void CFMRadioAlfBitmapAnimation::ConstructL( CAlfEnv& aEnv )
    {
    CAlfControl::ConstructL( aEnv );     
    CreateBitmapAnimationArrayL();
    CreateImageAnimationVisualL();
    }


// ---------------------------------------------------------------------------
// CFMRadioAlfBitmapAnimation::NewL
// Two-phase constructor of CFMRadioAlfBitmapAnimation
// ---------------------------------------------------------------------------
//
CFMRadioAlfBitmapAnimation* CFMRadioAlfBitmapAnimation::NewL( CAlfEnv& aEnv ) 
    {
    CFMRadioAlfBitmapAnimation* self = new ( ELeave ) CFMRadioAlfBitmapAnimation;
    CleanupStack::PushL( self );
    self->ConstructL( aEnv );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CFMRadioAlfBitmapAnimation::~CFMRadioAlfBitmapAnimation
// Destructor
// ---------------------------------------------------------------------------
//
CFMRadioAlfBitmapAnimation::~CFMRadioAlfBitmapAnimation()
	{
	iFrames.Reset();
	iFrames.Close();
	delete iBitmapFileName;
	}


// ----------------------------------------------------------------------------
// CFMRadioAlfBitmapAnimation::OfferEventL
// From CAlfControl, takes care of alfred event handling.
// ----------------------------------------------------------------------------
//
TBool CFMRadioAlfBitmapAnimation::OfferEventL( const TAlfEvent& aEvent )
	{
	TBool eventHandled = EFalse;
	
	if ( aEvent.IsCustomEvent() )
		{
		switch( aEvent.CustomParameter() )
			{
			case EBmpAnimationStart:
				{				
				eventHandled = ETrue;
				iAnimationState = EBmpAnimationRunning;
				StartAnimation();
				break;
				}
			case EBmpAnimationContinue:
				{
				eventHandled = ETrue;				
				ContinueBmpAnimation();							
				break;
				}
			case EBmpAnimationStop:
				{
				eventHandled = ETrue;				
				Env().CancelCustomCommands( this );
				iAnimationState = EBmpAnimationStopped;
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
	
// ----------------------------------------------------------------------------
// CFMRadioAlfBitmapAnimation::CreateImageAnimationVisualL
// ----------------------------------------------------------------------------
//		
void CFMRadioAlfBitmapAnimation::CreateImageAnimationVisualL()
	{
			
	//CAlfAnchorLayout* imageAnchorLayout = CAlfAnchorLayout::AddNewL( *this );
	//imageAnchorLayout->SetTagL( KBitmapAnimationAnchorTag );
	// Create an anchor for the indicator layout 
	iBmpAnimationAnchor = CAlfAnchorLayout::AddNewL( *this );
	iBmpAnimationAnchor->SetTagL( KBmpAnimationAnchorTag );
	
	iAnimationImageVisual = CAlfImageVisual::AddNewL( *this, iBmpAnimationAnchor ); 	
	iAnimationImageVisual->SetImage( iFrames[0] );
			
	TAlfTimedValue opacity;	
	opacity.SetTarget( iOpacityInHiddenState, 0 );
		
	iAnimationImageVisual->SetOpacity( opacity );							
	}
	

// ----------------------------------------------------------------------------
// CFMRadioAlfBitmapAnimation::CreateBitmapAnimationArrayL
// ----------------------------------------------------------------------------
//
void CFMRadioAlfBitmapAnimation::CreateBitmapAnimationArrayL()
    {
    CCoeEnv* coeEnv = CCoeEnv::Static();

    TResourceReader resourceReader;
    coeEnv->CreateResourceReaderLC( resourceReader, R_FMRADIO_WAIT_ANIM_TUNING );

    iFrameInterval = resourceReader.ReadInt32();
    TInt bitmapCount = resourceReader.ReadInt16();

    TFindFile finder( coeEnv->FsSession() );
    TInt err = finder.FindByDir( KFMRadioBmpFile, KDC_APP_BITMAP_DIR );
    if ( err == KErrNone )
        {
        iBitmapFileName = finder.File().AllocL();
        }
    
    // read bitmap size from layout data
    TRect tempRect;
    TAknLayoutRect bitmapLayout;
    bitmapLayout.LayoutRect( 
            tempRect,
            AknLayoutScalable_Apps::area_fmrd2_info_pane_g1( KLAFVarietyTuningAnimationPortrait ) );
    
    
    for ( TInt i = 0; i < bitmapCount; i++ )
        {
        TFileName bmpFile = resourceReader.ReadTPtrC();
        TInt bitmapId = resourceReader.ReadInt32();
        TInt bitmapMaskId = resourceReader.ReadInt32();
        TInt xx = resourceReader.ReadInt16();
        TInt xxx = resourceReader.ReadInt8();

        TAlfImage alfImage( KAknsIIDNone,
                            bitmapLayout.Rect().Size(),
                            EAspectRatioPreserved,
                            iBitmapFileName,
                            bitmapId,
                            bitmapMaskId );

        iFrames.Append( alfImage );
        }
    CleanupStack::PopAndDestroy(); //resourceReader
    }

// ----------------------------------------------------------------------------
// CFMRadioAlfBitmapAnimation::StartBmpAnimation
// ----------------------------------------------------------------------------
//
void CFMRadioAlfBitmapAnimation::StartAnimation()
	{
	iCurrentFrameIndex = 0;
	iAnimationImageVisual->SetImage( iFrames[iCurrentFrameIndex] );
	
	Show( ETrue );
		
	Env().Send( TAlfCustomEventCommand( EBmpAnimationContinue, this ), iFrameInterval );
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfBitmapAnimation::ContinueBmpAnimation
// ----------------------------------------------------------------------------
//
void CFMRadioAlfBitmapAnimation::ContinueBmpAnimation()
	{
	iCurrentFrameIndex++;
	
	if ( iCurrentFrameIndex == 9 )
		{
		iCurrentFrameIndex = 0;
		}
	iAnimationImageVisual->SetImage( iFrames[iCurrentFrameIndex] );
	Env().Send( TAlfCustomEventCommand( EBmpAnimationContinue, this ), iFrameInterval );
	}

// ---------------------------------------------------------
// CFMRadioAlfIndicator::Show
// Shows the indicator with the defined opacity value.
// ---------------------------------------------------------
//
void CFMRadioAlfBitmapAnimation::Show( TBool aShowWithFading )
	{
	TAlfTimedValue opacity;
		
	if ( aShowWithFading )
		{				
		opacity.SetTarget( iOpacityInVisibleState, KBmpAnimationFadeInDefaultDurationTime );
		
		iAnimationImageVisual->SetOpacity( opacity );				
		}
	else
		{
		opacity.SetValueNow( iOpacityInVisibleState );
		iAnimationImageVisual->SetOpacity( opacity );
		}	
	}

// ---------------------------------------------------------
// CFMRadioAlfIndicator::Hide
// Hides the indicator with the defined opacity value.
// ---------------------------------------------------------
//
void CFMRadioAlfBitmapAnimation::Hide( TBool aHideWithFading )
	{
	TAlfTimedValue opacity;
	
	if ( aHideWithFading )
		{				
		opacity.SetTarget( iOpacityInHiddenState, KBmpAnimationFadeOutDefaultDurationTime );
		
		iAnimationImageVisual->SetOpacity( opacity );
		}
	else
		{	
		opacity.SetValueNow( iOpacityInHiddenState );
		iAnimationImageVisual->SetOpacity( opacity );
		}
	}

// ---------------------------------------------------------
// CFMRadioAlfIndicator::StopBmpAnimation
// ---------------------------------------------------------
//
void CFMRadioAlfBitmapAnimation::StopBmpAnimation()
	{	
	if ( iAnimationState == EBmpAnimationStartup )
		{
		Env().CancelCustomCommands( this, EBmpAnimationStart );
		iAnimationState = EBmpAnimationStopped;
		}
	else if ( iAnimationState == EBmpAnimationRunning )
		{
		Hide( ETrue );
		
		Env().Send( TAlfCustomEventCommand( EBmpAnimationStop, this ), KbmpAnimationStopDelay );			
		}		
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfBitmapAnimation::SetRect
// Sets the Bmpanimation rectangle. 
// ---------------------------------------------------------------------------
//
void CFMRadioAlfBitmapAnimation::SetRect( const TRect& aRect )
	{
	iRect = aRect;
	if ( iBmpAnimationAnchor )
		{
		SetAbsoluteCornerAnchors( iBmpAnimationAnchor, 0, iRect.iTl, iRect.iBr );
		iBmpAnimationAnchor->UpdateChildrenLayout();
		}
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfBitmapAnimation::SetAbsoluteCornerAnchors
// Sets absolute rect of the anchor by top left and bottom right points.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfBitmapAnimation::SetAbsoluteCornerAnchors( CAlfAnchorLayout* aAnchor,
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
// CFMRadioAlfBitmapAnimation::StartBmpAnimation
// ---------------------------------------------------------------------------
//	
void CFMRadioAlfBitmapAnimation::StartBmpAnimation()
	{
	if ( iAnimationState == EBmpAnimationRunning )
	    {
	    Env().CancelCustomCommands( this );
	    Hide( EFalse ); // hide without fade
	    }
	else if ( iAnimationState == EBmpAnimationStartup )
        {
        Env().CancelCustomCommands( this, EBmpAnimationStart );        
        }	
	iAnimationState = EBmpAnimationStartup;	
	Env().Send( TAlfCustomEventCommand( EBmpAnimationStart, this ), KBmpAnimationStartupDelay );
	}
			
// End of file
