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
* Description:  Implementation of the class CFMRadioAlfVisualizer
*
*/


// INCLUDE FILES

#include <aknlayoutscalable_apps.cdl.h>
#include <alf/alfbatchbuffer.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfenv.h>
#include <alf/alftextvisual.h>
#include <alf/alfevent.h>
#include <alf/alftransformation.h>
#include <alf/alfgradientbrush.h>
#include <alf/alfbrusharray.h>
#include <alf/alfborderbrush.h>
#include <alf/alfimagevisual.h>
#include <alf/alfimagebrush.h>
#include <alf/alftexturemanager.h>
#include <alf/alfdisplay.h>
#include <alf/alfviewportlayout.h>
#include <alf/alfdecklayout.h>
#include <alf/alfutil.h>
#include <alf/alftextstyle.h>
#include <AknsItemID.h>
#include <e32math.h>
#include <eikenv.h>
#include <fmradio.mbg>
#include <aknview.h>
#include <aknViewAppUi.h>
#include <AknVolumePopup.h>

#include "fmradioalfvisualizer.h"
#include "fmradioappui.h"
#include "fmradioalfbitmapanimation.h"
#include "fmradio.hrh"
#include "debug.h"

using namespace GestureHelper;

// CONSTANTS
const TInt KFMRadioStationInformationFadeDurationTime = 200;
const TInt KFMRadioStationInformationScrollDurationTime = 300;
// The delay, in microseconds, after which long keypress is activated. Taken from S60 UI style guide
const TInt KFMRadioRockerLongPressDelay = 600000;

// values from fmradio LAF document
const TInt KLAFVarietyTuningAnimationPortrait = 7;
const TInt KLAFVarietyTuningAnimationLandscape = 7;
const TInt KLAFVarietyTuningTextPortrait = 7;
const TInt KLAFVarietyTuningTextLandscape = 7;
const TInt KLAFVarietyInfoPaneFirstLinePortrait = 0;
const TInt KLAFVarietyInfoPaneFirstLineLandscape = 1;
const TInt KLAFVarietyInfoPaneSecondLinePortrait = 0;
const TInt KLAFVarietyInfoPaneSecondLineLandscape = 1;
const TInt KLAFVarietyStationInfoPanePortrait = 0;
const TInt KLAFVarietyStationInfoPaneLandscape = 1;


// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CFMRadioAlfVisualizer::NewL
// Two-phase constructor of CFMRadioAlfVisualizer
// ---------------------------------------------------------------------------
//
CFMRadioAlfVisualizer* CFMRadioAlfVisualizer::NewL( CAlfEnv& aEnv )
	{
	CFMRadioAlfVisualizer* self = new (ELeave) CFMRadioAlfVisualizer( );
	CleanupStack::PushL(self);
	self->ConstructL( aEnv );
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfVisualizer::CFMRadioAlfVisualizer
// Default constructor
// ----------------------------------------------------------------------------
//
CFMRadioAlfVisualizer::CFMRadioAlfVisualizer( ) 
:  iOrientation ( EOrientationNone )
	{
	//No implementation needed
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfVisualizer::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::ConstructL( CAlfEnv& aEnv )
	{	    
    CGestureControl::ConstructL( *this, aEnv, aEnv.PrimaryDisplay(), KFMRadioVisualizerGestureControlGroupId );

    CCoeEnv* coeEnv = CCoeEnv::Static();
    iAppUi = static_cast<CFMRadioAppUi*>( coeEnv->AppUi() );  
	
	SetDisplayStyle( EFMRadioDisplayNoDisplay );  
    AddInformationLayersL();
    
    iBmpAnimation = CFMRadioAlfBitmapAnimation::NewL( aEnv );
    CAlfControlGroup& group = aEnv.ControlGroup( KFMRadioVisualControlsGroupId );
    group.AppendL( iBmpAnimation );
    
    UpdateLayout();
    
    if ( AknLayoutUtils::PenEnabled() )
        {
        iLongPressTimer = CPeriodic::NewL( CActive::EPriorityStandard );
        }
    }

// ---------------------------------------------------------------------------
// ~CFMRadioAlfVisualizer::~CFMRadioAlfVisualizer
// Destructor
// ---------------------------------------------------------------------------
//
CFMRadioAlfVisualizer::~CFMRadioAlfVisualizer()
	{
	CAlfTextStyleManager& manager = Env().TextStyleManager();
	manager.DeleteTextStyle( iFirstLineTextStyleId );
	manager.DeleteTextStyle( iSecondLineTextStyleId );
	manager.DeleteTextStyle( iTuningTextStyleId );

	delete iLongPressTimer;
	}
    	
// ----------------------------------------------------------------------------
// CFMRadioAlfVisualizer::UpdateLayout
// Calculates display layout
// ----------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::UpdateLayout()
    {
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
    TRect displayRect( mainPaneRect.Size() );

    TInt lafVarietyAnimation = 0;
    TInt lafVarietyTuningText = 0;
    TInt lafVarietyStationInfoPane = 0;
    TInt lafVarietyFirstLineText = 0;
    TInt lafVarietySecondLineText = 0;

    TFMRadioDisplayStyle displayStyle = DisplayStyle();

    // Adjust positions of the visual elements 
    if ( iOrientation == EPortrait || iOrientation == EOrientationNone )
        {
        lafVarietyAnimation = KLAFVarietyTuningAnimationPortrait;
        lafVarietyTuningText = KLAFVarietyTuningTextPortrait;
        lafVarietyStationInfoPane = KLAFVarietyStationInfoPanePortrait;
        lafVarietyFirstLineText = KLAFVarietyInfoPaneFirstLinePortrait;
        lafVarietySecondLineText = KLAFVarietyInfoPaneSecondLinePortrait;
        }
    else if ( iOrientation == ELandscape )
        {
        lafVarietyAnimation = KLAFVarietyTuningAnimationLandscape;
        lafVarietyTuningText = KLAFVarietyTuningTextLandscape;
        lafVarietyStationInfoPane = KLAFVarietyStationInfoPaneLandscape;
        lafVarietyFirstLineText = KLAFVarietyInfoPaneFirstLineLandscape;
        lafVarietySecondLineText  = KLAFVarietyInfoPaneSecondLineLandscape;
        }

    iStationInformationLayout.LayoutRect( displayRect,
                                          AknLayoutScalable_Apps::area_fmrd2_info_pane( lafVarietyStationInfoPane ).LayoutLine() );

    if ( EFMRadioDisplayDoubleLine == displayStyle )
        {    
        iFirstLineLayout.LayoutText( iStationInformationLayout.Rect(),
                                     AknLayoutScalable_Apps::area_fmrd2_info_pane_t1( lafVarietyFirstLineText ).LayoutLine() );
        }
    else // use bigger area for first line because it is the only visible line
        {
        iFirstLineLayout.LayoutText( iStationInformationLayout.Rect(),
                                     AknLayoutScalable_Apps::area_fmrd2_info_pane_t3( lafVarietyFirstLineText ).LayoutLine() );
        }

    iSecondLineLayout.LayoutText( iStationInformationLayout.Rect(),
                                  AknLayoutScalable_Apps::area_fmrd2_info_pane_t2( lafVarietySecondLineText ).LayoutLine() );

    iTuningTextLayout.LayoutText( iStationInformationLayout.Rect(),
                                  AknLayoutScalable_Apps::area_fmrd2_info_pane_t4( lafVarietyTuningText ).LayoutLine() );

    iTuningAnimationLayout.LayoutRect( iStationInformationLayout.Rect(),
                                       AknLayoutScalable_Apps::area_fmrd2_info_pane_g1( lafVarietyAnimation ) );

    CAlfTextStyleManager& manager = Env().TextStyleManager();
    CAlfTextStyle* style = NULL;
    style = manager.TextStyle( iFirstLineTextStyleId );
    if ( style->TextSizeInPixels() != iFirstLineLayout.TextRect().Height() )
        {  // Update text size according to height of layout in LS & PT modes
        style->SetTextSizeInPixels( iFirstLineLayout.TextRect().Height(), ETrue );
        iFirstLineVisualText->SetTextStyle( iFirstLineTextStyleId );
        }

    style = manager.TextStyle( iSecondLineTextStyleId );
    if ( style->TextSizeInPixels() != iSecondLineLayout.TextRect().Height() )
        {  // Update text size according to height of layout in LS & PT modes
        style->SetTextSizeInPixels( iSecondLineLayout.TextRect().Height(), ETrue );
        iSecondLineVisualText->SetTextStyle( iSecondLineTextStyleId );
        }

    style = manager.TextStyle( iTuningTextStyleId );
    if ( style->TextSizeInPixels() != iTuningTextLayout.TextRect().Height() )
        {  // Update text size according to height of layout in LS & PT modes
        style->SetTextSizeInPixels( iTuningTextLayout.TextRect().Height(), ETrue );
        iTuningVisualText->SetTextStyle( iTuningTextStyleId );
        }

    SetAbsoluteCornerAnchors( iStationInformationAnchorLayout, 0, iFirstLineLayout.TextRect().iTl, iFirstLineLayout.TextRect().iBr );
    SetAbsoluteCornerAnchors( iStationInformationAnchorLayout, 1, iSecondLineLayout.TextRect().iTl, iSecondLineLayout.TextRect().iBr );
    SetAbsoluteCornerAnchors( iStationInformationAnchorLayout, 2, iTuningTextLayout.TextRect().iTl, iTuningTextLayout.TextRect().iBr );
    iStationInformationAnchorLayout->UpdateChildrenLayout();
    iBmpAnimation->SetRect( iTuningAnimationLayout.Rect() );
    }

// ----------------------------------------------------------------------------
// CFMRadioAlfVisualizer::Orientation
// Return orientation of the display
// ----------------------------------------------------------------------------
//
TDisplayOrientation CFMRadioAlfVisualizer::Orientation()
	{
	return iOrientation;
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfVisualizer::Orientation
// Set orientation of the display
// ----------------------------------------------------------------------------
//		
void CFMRadioAlfVisualizer::SetOrientation( TDisplayOrientation aOrientation )
	{
	iOrientation = aOrientation;
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfVisualizer::SetMirrored
// ----------------------------------------------------------------------------
//		
void CFMRadioAlfVisualizer::SetMirrored( const TBool aIsMirrored  )
	{
	iMirrored = aIsMirrored;
	}

	
// ---------------------------------------------------------------------------
// CFMRadioAlfVisualizer::IsMirroredL
// ---------------------------------------------------------------------------
//
TBool CFMRadioAlfVisualizer::IsMirrored() const
    {
	return iMirrored;
    }
	
// ---------------------------------------------------------------------------
// CFMRadioAlfVisualizer::AddInformationLayersL
// Creates the needed drawing layers and visual objects
// for visualiazing the station information. 
// ---------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::AddInformationLayersL()
	{   
	CreateTextStylesForStationInformationL();	
	// Create an anchor for layout of texts
	iStationInformationAnchorLayout = CAlfAnchorLayout::AddNewL( *this );	
	
	// Create anchor to be used when preset channel is changed
	iStationInformationTempAnchorLayout = CAlfAnchorLayout::AddNewL( *this );															
	iStationInformationTempAnchorLayout->SetOpacity( TAlfTimedValue( 0.0 ) );

	for ( TInt i = 0; i < 3; i++ )
		{
		CAlfTextVisual* tempTextVisual = CAlfTextVisual::AddNewL( *this, iStationInformationTempAnchorLayout ); 	
		tempTextVisual->SetWrapping( CAlfTextVisual::ELineWrapTruncate );
		tempTextVisual->SetAlign( EAlfAlignHCenter, EAlfAlignVCenter );
		}

	// Create visual object for the station name
	iFirstLineVisualText = CAlfTextVisual::AddNewL( *this, iStationInformationAnchorLayout );
	iFirstLineVisualText->SetTextStyle( iFirstLineTextStyleId );	
	iFirstLineVisualText->SetWrapping( CAlfTextVisual::ELineWrapTruncate );
	iFirstLineVisualText->SetAlign( EAlfAlignHCenter, EAlfAlignVCenter );
	// Create visual object for frequency information
	iSecondLineVisualText = CAlfTextVisual::AddNewL( *this, iStationInformationAnchorLayout );
	iSecondLineVisualText->SetTextStyle( iSecondLineTextStyleId );	
	iSecondLineVisualText->SetWrapping( CAlfTextVisual::ELineWrapTruncate );
	iSecondLineVisualText->SetAlign( EAlfAlignHCenter, EAlfAlignVCenter );
	// Visual for tuning text
	iTuningVisualText = CAlfTextVisual::AddNewL( *this, iStationInformationAnchorLayout );
	iTuningVisualText->SetTextStyle( iTuningTextStyleId );  
	iTuningVisualText->SetWrapping( CAlfTextVisual::ELineWrapTruncate );
	iTuningVisualText->SetAlign( EAlfAlignHCenter, EAlfAlignVCenter );	
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfVisualizer::CreateTextStylesForStationInformationL
// Creates the text styles to be used by station information visualiazing
// ---------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::CreateTextStylesForStationInformationL()
	{
	CAlfTextStyleManager& manager = Env().TextStyleManager();
	
	iFirstLineTextStyleId = manager.CreatePlatformTextStyleL(
	        EAknLogicalFontPrimaryFont,
	        EAlfTextStyleNormal );

	iSecondLineTextStyleId = manager.CreatePlatformTextStyleL(
	        EAknLogicalFontSecondaryFont,
	        EAlfTextStyleNormal );

	iTuningTextStyleId = manager.CreatePlatformTextStyleL(
	        EAknLogicalFontSecondaryFont,
	        EAlfTextStyleNormal );
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfVisualizer::SetVisualText
// Sets text to the defined station information visual.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::SetVisualTextL( TStationInformationVisual aVisual, const TDesC& aText )
	{
	if( aVisual & EFirstLineTextVisual )
		{
		iFirstLineVisualText->SetTextL( aText );
		iFirstLineVisualText->SetSize( iFirstLineVisualText->TextExtents() );
		}
	if( aVisual & ESecondLineTextVisual )
		{
		iSecondLineVisualText->SetTextL( aText );
		iSecondLineVisualText->SetSize( iSecondLineVisualText->TextExtents() );
		}
	if ( aVisual & ETuningTextVisual )
	    {
	    iTuningVisualText->SetTextL( aText );
	    iTuningVisualText->SetSize( iTuningVisualText->TextExtents() );	    	    
	    }
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfVisualizer::ShowVisual
// Shows the defined station information visual. 
// ---------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::ShowVisual( TStationInformationVisual aVisual, TBool aFade )
    {
    if ( aVisual & EFirstLineTextVisual )
        {
        if ( aFade )
            {
            FadeIn( *iFirstLineVisualText, KFMRadioStationInformationFadeDurationTime );
            }
        else
            {
            FadeIn( *iFirstLineVisualText, 0 );
            }
        }
    if ( aVisual & ESecondLineTextVisual )
        {
        if ( aFade )
            {
            FadeIn( *iSecondLineVisualText, KFMRadioStationInformationFadeDurationTime );
            }
        else
            {
            FadeIn( *iSecondLineVisualText, 0 );
            }
        }
    if ( aVisual & ETuningTextVisual )
        {
        if ( aFade )
            {
            FadeIn( *iTuningVisualText, KFMRadioStationInformationFadeDurationTime );
            }
        else
            {
            FadeIn( *iTuningVisualText, 0 );
            }
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAlfVisualizer::HideVisual
// Hides the defined station information visual. 
// ---------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::HideVisual( TStationInformationVisual aVisual )
    {
    if ( aVisual & EFirstLineTextVisual )
        {
        FadeOut( *iFirstLineVisualText, 0 );
        }
    if ( aVisual & ESecondLineTextVisual )
        {
        FadeOut( *iSecondLineVisualText, 0 );
        }
    if ( aVisual & ETuningTextVisual )
        {
        FadeOut( *iTuningVisualText, 0 );
        }    
    }

// ---------------------------------------------------------------------------
// CFMRadioAlfVisualizer::SetStationInformationTextColor
// Change text color of the text visuals.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::SetStationInformationTextColor( const TRgb& aTextColor )
	{
	iStationInformationTextColor = aTextColor;
	iFirstLineVisualText->SetColor( aTextColor );	
	iSecondLineVisualText->SetColor( aTextColor );
	iTuningVisualText->SetColor( aTextColor );
	};

// ----------------------------------------------------------------------------
// CFMRadioAlfVisualizer::ScrollOutCurrentStationInformationL
// Scrolls out the current station information lines by hiding actual visuals and scrolling out
// the created temporary information lines
// ----------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::ScrollOutCurrentStationInformationL( TFMRadioStationChangeType aChangeType )
    {
    // Don't start any effects if the current station info is already disappeared
    if ( iStationInformationAnchorLayout->Opacity().Target() != 0 )
        {
        // Hide the actual station information visuals
        CreateTemporaryStationInformationVisualsL();
        AnimateTemporaryStationInformationVisuals( aChangeType );
        // Hide the original and set it to origin by default
		HideVisual( EAllVisuals );
        FadeOut( *iStationInformationAnchorLayout, 0 );
        iStationInformationAnchorLayout->SetPos( TAlfRealPoint() );
        // Flush manually
        Env().BatchBufferHandler().FlushBatchBuffer();
        }	
    }
	
// ----------------------------------------------------------------------------
// CFMRadioAlfVisualizer::CreateTemporaryStationInformationVisualsL
// Creates copy of current station information
// ----------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::CreateTemporaryStationInformationVisualsL()
	{
    // update layout and attributes for child visuals											
    for ( TInt i = 0, count = iStationInformationAnchorLayout->Count(); i < count; i++ )
        {
        TPoint childPosition;
        TSize childSize;
        
        iStationInformationAnchorLayout->ChildPos( i , childPosition );
        iStationInformationAnchorLayout->ChildSize( i , childSize );			
        TAlfRealRect childRect( TRect( childPosition, childSize ) );			

        CAlfTextVisual* textVisual = static_cast<CAlfTextVisual*>( &iStationInformationAnchorLayout->Visual( i ) ); 
        CAlfTextVisual* tempTextVisual = static_cast<CAlfTextVisual*>( &iStationInformationTempAnchorLayout->Visual( i ) );
                    
        tempTextVisual->SetTextL( textVisual->Text() );						
        tempTextVisual->SetTextStyle( textVisual->TextStyle() );			
        tempTextVisual->SetColor( iStationInformationTextColor );
        tempTextVisual->SetOpacity( textVisual->Opacity() );

        SetAbsoluteCornerAnchors( iStationInformationTempAnchorLayout, i, childRect.iTl, childRect.iBr ); 
        }
    iStationInformationTempAnchorLayout->UpdateChildrenLayout();

    // Flush manually
    Env().BatchBufferHandler().FlushBatchBuffer();
     
    iStationInformationTempAnchorLayout->SetOpacity( iStationInformationAnchorLayout->Opacity() );
    iStationInformationTempAnchorLayout->SetPos( iStationInformationAnchorLayout->Pos() );
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfVisualizer::AnimateTemporaryStationInformationVisuals
// Animates temporary station infoout of the display
// ----------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::AnimateTemporaryStationInformationVisuals( TFMRadioStationChangeType aChangeType )
    {
    FadeOut( *iStationInformationTempAnchorLayout, KFMRadioStationInformationFadeDurationTime );
    switch ( aChangeType )
        {
        // Swipe visuals in the opposite direction to give the right impression 
        case EFMRadioStationChangePrevious:
            {	
            ScrollToRight( *iStationInformationTempAnchorLayout, KFMRadioStationInformationScrollDurationTime  );
            break;
            }
        case EFMRadioStationChangeNext:
            {
            ScrollToLeft( *iStationInformationTempAnchorLayout, KFMRadioStationInformationScrollDurationTime  );				 					 					 					 					 					 					 	
            break;
            }
        case EFMRadioStationChangeScanUp:
            {
            ScrollUp( *iStationInformationTempAnchorLayout, KFMRadioStationInformationScrollDurationTime  );                                                                                                                                            
            break;
            }
        case EFMRadioStationChangeScanDown:
            {
            ScrollDown( *iStationInformationTempAnchorLayout, KFMRadioStationInformationScrollDurationTime  );                                                                                                                                            
            break;
            }
        default:
            break;
        };
	} 

// ---------------------------------------------------------------------------
// From class MGestureObserver.
// Called when user makes gestures.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::HandleGestureL( const MGestureEvent& aEvent )
    {
    TGestureCode eventCode( aEvent.Code( MGestureEvent::EAxisBoth ) );
    FTRACE( FPrint( _L("CFMRadioAlfVisualizer::HandleGestureL(eventCode=%d)"), eventCode ) );
    
    CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( CCoeEnv::Static()->AppUi() );
    // handle gestures only if there is no call or tuning ongoing
    if ( !appUi->RadioEngine()->IsInCall() &&
            appUi->RadioState() != CFMRadioAppUi::EFMRadioStateBusySeek )
        {
        switch ( eventCode )
            {
            case EGestureTap:
                {
                CAknVolumePopup* volPopup = appUi->ActiveVolumePopup();
                if ( volPopup )
                    {
                    if ( volPopup->IsVisible() )
                        {
                        volPopup->CloseVolumePopup();
                        }
                    else
                        {
                        volPopup->ShowVolumePopupL();
                        }
                    }
                break;
                }
            case EGestureSwipeLeft:
                {
                if ( iDragAxis == EFMRadioDragAxisX )
                    {
                    if ( appUi->NumberOfChannelsStored() )
                        {
                        iRevertGestureDrag = EFalse;
                        }
                    appUi->HandleCommandL( EFMRadioCmdNextChannel );
                    }
                break;
                }
            case EGestureSwipeRight:
                {
                if ( iDragAxis == EFMRadioDragAxisX )
                    {
                    if ( appUi->NumberOfChannelsStored() )
                        {
                        iRevertGestureDrag = EFalse;
                        }
                    appUi->HandleCommandL( EFMRadioCmdPrevChannel );
                    }
                break;
                }
            case EGestureSwipeUp:
                {
                if ( iDragAxis == EFMRadioDragAxisY )
                    {
                    iRevertGestureDrag = EFalse;
                    appUi->HandleCommandL( EFMRadioCmdSeekUp );
                    }
                break;
                }
            case EGestureSwipeDown:
                {
                if ( iDragAxis == EFMRadioDragAxisY )
                    {
                    iRevertGestureDrag = EFalse;
                    appUi->HandleCommandL( EFMRadioCmdSeekDown );
                    }
                break;
                }
            case EGestureStart:
                {
                iDragAxis = EFMRadioDragAxisNone;
                iRevertGestureDrag = ETrue;
                iDragStartPosition = iStationInformationAnchorLayout->Pos();
                break;
                }
            case EGestureDrag:
                {
                TPoint delta = aEvent.CurrentPos() - aEvent.StartPos();
                
                // Determine the dragging axis
                if ( iDragAxis == EFMRadioDragAxisNone )
                    {
                    TInt absX = Abs( delta.iX );
                    TInt absY = Abs( delta.iY );
                    if ( absX > absY )
                        {
                        iDragAxis = EFMRadioDragAxisX;
                        }
                    else if ( absY > absX )
                        {
                        iDragAxis = EFMRadioDragAxisY;
                        }
                    else
                        {
                        //X and Y are equal, can't determine the axis
                        }
                    }
                
                
                if ( iRevertGestureDrag && iDragAxis != EFMRadioDragAxisNone ) // gesture is ongoing, ok to move visual
                    {
                    TAlfRealPoint newPosition;
                    newPosition.iX = iDragStartPosition.iX.ValueNow();
                    newPosition.iY = iDragStartPosition.iY.ValueNow();
                    
                    if ( iDragAxis == EFMRadioDragAxisX )
                        {
                        newPosition.iX += delta.iX;
                        }
                    else
                        {
                        newPosition.iY += delta.iY;
                        }
                    iStationInformationAnchorLayout->SetPos( newPosition );
                    }
                break;
                }
            case EGestureUnknown:
            case EGestureReleased:
                {
                if ( iRevertGestureDrag )
                    {
                    RevertVisual( *iStationInformationAnchorLayout );
                    }
                break;
                }
            case EGestureHoldLeft:
            case EGestureHoldRight:
            case EGestureHoldUp:
            case EGestureHoldDown:
                {
                // gesture "timed out"
                if ( iRevertGestureDrag )
                    {
                    // change revert status so that the following
                    // EGestureDrag event doesn't change visual position
                    // anymore
                    iRevertGestureDrag = EFalse;
                    RevertVisual( *iStationInformationAnchorLayout );
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        }
    else
        {
        FTRACE(FPrint(_L("CFMRadioAlfVisualizer::HandleGestureL() call or tuning ongoing -> gestures ignored")));
        }
    }
// ----------------------------------------------------------------------------
// CFMRadioAlfVisualizer::OfferEventL
// From CAlfControl, takes care of alfred event handling.
// ----------------------------------------------------------------------------
//
TBool CFMRadioAlfVisualizer::OfferEventL( const TAlfEvent& aEvent )
	{		
    if ( aEvent.IsKeyEvent() && AknLayoutUtils::PenEnabled() )
        {
        const TKeyEvent& kEvent = aEvent.KeyEvent();
        if ( kEvent.iScanCode  == EStdKeyRightArrow ||
            kEvent.iScanCode == EStdKeyLeftArrow ||
            kEvent.iScanCode == EStdKeyUpArrow ||
            kEvent.iScanCode == EStdKeyDownArrow )
            {
            iKeyScanCode = kEvent.iScanCode;

            if ( aEvent.Code() == EEventKeyUp )
                {
                iLongPressTimer->Cancel();
                if ( !iLongKeyTriggered )
                    {
                    TriggerCommandL();
                    }
                }
                else if ( aEvent.Code() == EEventKeyDown )
                {
                iLongKeyTriggered = EFalse;
                // Start the long key press timer
                iLongPressTimer->Cancel();
                iLongPressTimer->Start( KFMRadioRockerLongPressDelay, 
                                        0, 
                                        TCallBack( CFMRadioAlfVisualizer::StaticLongPressCallBack, this ) );
                }
            return ETrue;
            }
        }
    if ( aEvent.IsPointerEvent() )
        {
        return CGestureControl::OfferEventL( aEvent );
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// Sets the display style in use.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::SetDisplayStyle( TFMRadioDisplayStyle aDisplayStyle )
    {
    iDisplayStyle = aDisplayStyle;
    }
    		
// ---------------------------------------------------------------------------
// Returns the display style.
// ---------------------------------------------------------------------------
//
TFMRadioDisplayStyle CFMRadioAlfVisualizer::DisplayStyle() const
    {
    return iDisplayStyle;
    }

// ---------------------------------------------------------------------------
// CFMRadioAlfVisualizer::SetAbsoluteCornerAnchors
// Sets absolute rect of the anchor by top left and bottom right points.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::SetAbsoluteCornerAnchors( CAlfAnchorLayout* aAnchor,
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
// CFMRadioAlfVisualizer::ScrollRightToLeftWithFadeIn
// Sets and starts scrolling and fading animations to CAlfVisual.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::ScrollRightToLeftWithFadeIn( CAlfVisual& aVisual ) const
	{
	TInt visualWidthInDisplayCoord = aVisual.DisplayRect().Width();
    
	TAlfTimedPoint targetPoint;
    targetPoint.iX.SetValueNow( visualWidthInDisplayCoord );
    targetPoint.iX.SetTarget( 0, KFMRadioStationInformationScrollDurationTime );
	aVisual.SetPos( targetPoint );
	
	FadeIn( aVisual, KFMRadioStationInformationFadeDurationTime);
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfVisualizer::ScrollLeftToRightWithFadeIn
// Sets and starts scrolling and fading animations to CAlfVisual.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::ScrollLeftToRightWithFadeIn( CAlfVisual& aVisual ) const
	{	
	TInt visualWidthInDisplayCoord = aVisual.DisplayRect().Width();
	TAlfTimedPoint targetPoint;
	targetPoint.iX.SetValueNow( - visualWidthInDisplayCoord );
	targetPoint.iX.SetTarget( 0, KFMRadioStationInformationScrollDurationTime );
	aVisual.SetPos( targetPoint );

	FadeIn( aVisual, KFMRadioStationInformationFadeDurationTime);
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfVisualizer::ScrollInFromTop
// Sets and starts scrolling and fading animations to CAlfVisual.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::ScrollInFromTop( CAlfVisual& aVisual ) const
    {   
    TRect displayArea = Env().PrimaryDisplay().VisibleArea();
    TAlfTimedPoint targetPoint;
    targetPoint.iY.SetValueNow( -displayArea.Height() );
    targetPoint.iY.SetTarget( 0, KFMRadioStationInformationScrollDurationTime );
    aVisual.SetPos( targetPoint );

    FadeIn( aVisual, KFMRadioStationInformationFadeDurationTime);
    }

// ---------------------------------------------------------------------------
// CFMRadioAlfVisualizer::ScrollInFromBottom
// Sets and starts scrolling and fading animations to CAlfVisual.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::ScrollInFromBottom( CAlfVisual& aVisual ) const
    {   
    TRect displayArea = Env().PrimaryDisplay().VisibleArea();
    TAlfTimedPoint targetPoint;
    targetPoint.iY.SetValueNow( displayArea.Height() );
    targetPoint.iY.SetTarget( 0, KFMRadioStationInformationScrollDurationTime );
    aVisual.SetPos( targetPoint );

    FadeIn( aVisual, KFMRadioStationInformationFadeDurationTime);
    }

// ---------------------------------------------------------------------------
// CFMRadioAlfVisualizer::RevertVisual
// Reverts the visual to the origin
// ---------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::RevertVisual( CAlfVisual& aVisual )
    {
    TAlfTimedPoint targetPoint;
    targetPoint.iX.SetTarget( 0, KFMRadioStationInformationScrollDurationTime );    
    targetPoint.iY.SetTarget( 0, KFMRadioStationInformationScrollDurationTime );
    aVisual.SetPos( targetPoint ); 
    TAlfTimedValue opacity;
    opacity.SetTarget( 1.0f, KFMRadioStationInformationFadeDurationTime );  
    aVisual.SetOpacity( opacity );
    }

// ----------------------------------------------------------------------------
// CFMRadioAlfVisualizer::ScrollToLeft
// Sets and starts scrolling animation to CAlfVisual.
// The visual object is scrolled from current position to 
// left side of the display
// ----------------------------------------------------------------------------
//	
void CFMRadioAlfVisualizer::ScrollToLeft( CAlfVisual& aVisual, TInt aTime ) const
    {
    TInt visualWidth = aVisual.DisplayRect().Width();
    TAlfTimedPoint timedPosition = aVisual.Pos();
    TAlfRealPoint positionLeft( - ( visualWidth ), timedPosition.iY.ValueNow() );
    aVisual.SetPos( positionLeft, aTime );
    }

// ----------------------------------------------------------------------------
// CFMRadioAlfVisualizer::ScrollToRight
// Sets and starts scrolling animation to CAlfVisual.
// The visual object is scrolled from current position to 
// right side of the display
// ----------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::ScrollToRight( CAlfVisual& aVisual, TInt aTime ) const
    {
    TRect displayArea = Env().PrimaryDisplay().VisibleArea();
    TAlfTimedPoint timedPosition = aVisual.Pos();
    TAlfRealPoint positionRight( displayArea.Width(), timedPosition.iY.ValueNow() );
    
    aVisual.SetPos( positionRight, aTime );
    }

// ----------------------------------------------------------------------------
// CFMRadioAlfVisualizer::ScrollUp
// Sets and starts scrolling animation to CAlfVisual.
// The visual object is scrolled up
// ----------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::ScrollUp( CAlfVisual& aVisual, TInt aTime ) const
    {
    TInt visualHeight = aVisual.DisplayRect().Height();
    TAlfTimedPoint timedPosition = aVisual.Pos();
    TAlfRealPoint positionUp( timedPosition.iX.ValueNow(), -visualHeight );
    aVisual.SetPos( positionUp, aTime );
    }

// ----------------------------------------------------------------------------
// CFMRadioAlfVisualizer::ScrollDown
// Sets and starts scrolling animation to CAlfVisual.
// The visual object is scrolled down
// ----------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::ScrollDown( CAlfVisual& aVisual, TInt aTime ) const
    {
    TRect displayArea = Env().PrimaryDisplay().VisibleArea();
    TAlfTimedPoint timedPosition = aVisual.Pos();
    TAlfRealPoint positionDown( timedPosition.iX.ValueNow(), displayArea.Height() );
    aVisual.SetPos( positionDown, aTime );
    }

// ---------------------------------------------------------------------------
// CFMRadioAlfVisualizer::FadeIn
// Sets fading in animation to CAlfVisual.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::FadeIn( CAlfVisual& aVisual, TInt aTime, TReal aOpacity ) const
	{
	TAlfTimedValue opacity;
	opacity.SetValueNow( 0.0f ); // immediate change
	opacity.SetTarget( aOpacity, aTime ); // and smooth target
	aVisual.SetOpacity( opacity );
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfVisualizer::FadeOut
// Sets fading out animation to CAlfVisual.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::FadeOut( CAlfVisual& aVisual, TInt aTime, TReal aOpacity ) const
	{	
	TAlfTimedValue opacity;
	opacity.SetTarget( aOpacity, aTime ); 
	aVisual.SetOpacity( opacity );
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfVisualizer::ChangeStationL
// Changes station to next with scrolling and fading station's 
// information (name, frequency, etc.).
// ---------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::ChangeStationL( TFMRadioStationChangeType aChangeType, 
                                            const TDesC& aFirstLine, 
                                            const TDesC* aSecondLine ) 
	{	
    const TDesC& secondLine = aSecondLine ? *aSecondLine : KNullDesC;
    TBool firstLineFade = iFirstLineVisualText->Text() != aFirstLine;
    TBool secondLineFade = iSecondLineVisualText->Text() != secondLine;
	
    if ( aChangeType != EFMRadioStationChangeNone || firstLineFade || secondLineFade )
        {
    	Env().CancelCommands( iFirstLineVisualText );
    	Env().CancelCommands( iSecondLineVisualText );

        if ( secondLine.Length() )
            {
            SetDisplayStyle( EFMRadioDisplayDoubleLine );
            }
        else
            {
            SetDisplayStyle( EFMRadioDisplaySingleLine );
            }
    
        FadeOut( *iTuningVisualText, 0 );
        LoadTextVisualIdentity( iFirstLineVisualText );
        LoadTextVisualIdentity( iSecondLineVisualText );
        iFirstLineVisualText->SetTextL( aFirstLine );
        iSecondLineVisualText->SetTextL( secondLine );
        UpdateLayout();

    	// Set the horizontal animation	
    	switch ( aChangeType )
    	    {
    	    case EFMRadioStationChangePrevious:
    			{
    			// Direction of the animation is left to right
                ShowVisual( EFirstLineTextVisual, 0 );
                ShowVisual( ESecondLineTextVisual, 0 );
    			ScrollLeftToRightWithFadeIn( *iStationInformationAnchorLayout );
    			}
    			break;
    	    case EFMRadioStationChangeNext:
    			{
    			// Direction of the animation is right to left
                ShowVisual( EFirstLineTextVisual, 0 );
                ShowVisual( ESecondLineTextVisual, 0 ); 
    			ScrollRightToLeftWithFadeIn( *iStationInformationAnchorLayout );
    			}
    			break;
            case EFMRadioStationChangeScanUp:
                {
                ShowVisual( EFirstLineTextVisual, 0 );
                ShowVisual( ESecondLineTextVisual, 0 ); 
                ScrollInFromBottom( *iStationInformationAnchorLayout );
                }
                break;
            case EFMRadioStationChangeScanDown:
                {
                ShowVisual( EFirstLineTextVisual, 0 );
                ShowVisual( ESecondLineTextVisual, 0 ); 
                ScrollInFromTop( *iStationInformationAnchorLayout );
                }
                break;
    	    case EFMRadioStationChangeNone:
    	    	{	
    			ShowVisual( EFirstLineTextVisual, firstLineFade );
    			ShowVisual( ESecondLineTextVisual, secondLineFade );
    	    	}
    	    	break;
    	    default:
    		    break;	   
    	    } 
        }
	}

// ---------------------------------------------------------------------------
// CFMRadioAlfVisualizer::SetStaticInfoTextL
// Resets the station information and sets static text to the third line.
// ---------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::SetStaticInfoTextL( TFMRadioStationChangeType aChangeType, const TDesC& aInfoText )
	{	 
	ScrollOutCurrentStationInformationL( aChangeType );
	SetVisualTextL( ETuningTextVisual, aInfoText );
	UpdateLayout();
	ShowVisual( ETuningTextVisual, ETrue );
	// Make the new station info visible
    FadeIn( *iStationInformationAnchorLayout, 0 );
	}

// ----------------------------------------------------------------------------
// CFMRadioAlfVisualizer::Translate
// Translates the visual object with passed arguments
// ----------------------------------------------------------------------------
//	
void CFMRadioAlfVisualizer::Translate( CAlfTextVisual* aTextVisual, const TAlfTimedValue& aX, const TAlfTimedValue& aY )
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

// ---------------------------------------------------------------------------
// Triggers the command to view handling
// ---------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::TriggerCommandL()
    {
    TVwsViewId viewId( KNullUid, KNullUid );
    CAknViewAppUi* appUi = static_cast<CAknViewAppUi*>( CCoeEnv::Static()->AppUi() );
    TInt err = appUi->GetActiveViewId( viewId );
    RProcess process;
    TSecureId id = process.SecureId();

    if ( !err && viewId.iAppUid.iUid == id.iId )
        {
        CAknView* view = appUi->View( viewId.iViewUid );
        if ( view )
            {
            // command to be generated
            TInt commandId = 0;
            
            switch ( iKeyScanCode )
                {
                case EStdKeyLeftArrow:
                    {
                    if ( iLongKeyTriggered )
                        {
                        commandId = EFMRadioRockerButtonKeyLeftLong;
                        break;
                        }
                    else
                        {
                        commandId = EFMRadioRockerButtonKeyLeft;                		
                        }
                    break;
                    }
                case EStdKeyRightArrow:
                    {
                    if ( iLongKeyTriggered )
                        {
                        commandId = EFMRadioRockerButtonKeyRightLong;
                        }
                    else
                        {
                        commandId = EFMRadioRockerButtonKeyRight;
                        }
                    break;
                    }
                case EStdKeyUpArrow:
                    {
                    commandId = EFMRadioRockerButtonKeyUp;
                    break;
                    }
                case EStdKeyDownArrow:
                    {
                    commandId = EFMRadioRockerButtonKeyDown;
                    break;
                    }
                }
            // send command to view
            view->ProcessCommandL( commandId );
            }
        }
    }

// ---------------------------------------------------------------------------
// Callback that is invoked when a long key press has occurred.
// ---------------------------------------------------------------------------
//
TInt CFMRadioAlfVisualizer::StaticLongPressCallBack( TAny* aSelf )
    {
    CFMRadioAlfVisualizer* self = static_cast<CFMRadioAlfVisualizer*>( aSelf );
    if ( self )
        {
        self->iLongPressTimer->Cancel();
        self->iLongKeyTriggered = ETrue;
        
        TRAP_IGNORE( self->TriggerCommandL() )
        }
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CFMRadioAlfVisualizer::LoadTextVisualIdentity
// Removes all transformations of the visual object such as the scaling and translating.
// ----------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::LoadTextVisualIdentity( CAlfTextVisual* aTextVisual )
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
// CFMRadioAlfVisualizer::StartTuningAnimation
// Starts tuning animation after delay
// ----------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::StartTuningAnimation()
    {
    iBmpAnimation->StartBmpAnimation(); 
    }

// ----------------------------------------------------------------------------
// CFMRadioAlfVisualizer::StopTuningAnimation
// Stops tuning animation by fading it away
// ----------------------------------------------------------------------------
//
void CFMRadioAlfVisualizer::StopTuningAnimation()
    {
    iBmpAnimation->StopBmpAnimation(); 
    }

//  End of File  

