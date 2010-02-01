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
* Description:  Implementation of the class CFMRadioMainContainer
*
*/


// INCLUDE FILES

#include <alf/alfcontrolgroup.h>
#include <alf/alfdisplay.h>
#include <alf/alfroster.h>
#include <alf/alflayoutmetrics.h>
#include <AknLayoutFont.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <AknsBasicBackgroundControlContext.h>
#include <AknsDrawUtils.h>
#include <AknsSkinInstance.h>
#include <AknsUtils.h>
#include <aknconsts.h>
#include <AknDef.h>
#include <StringLoader.h>
#include <barsread.h>
#include <w32std.h>
#include <e32base.h>
#include <fontids.hrh>
#include <gulfont.h>
#include <fmradio.rsg>
#include <fmradiouids.h>

#include "fmradiomaincontainer.h"
#if defined __SERIES60_HELP || defined FF_S60_HELPS_IN_USE
#include "radio.hlp.hrh"
#endif
#include "fmradioengine.h"
#include "fmradioalfrdsviewer.h"
#include "fmradioalfvisualizer.h"
#include "fmradioalfmediaidle.h"
#include "fmradioalfindicator.h"
#include "fmradiologo.h"
#include "fmradiordsreceiverbase.h"
#include "fmradioalfrdsviewer.h"
#include "fmradiomainview.h"
#include "fmradioidlecontroller.h"
#include "debug.h"


// ~3 seconds, 10 frames / second is used
const TInt KFadeFrames = 30;   
const TInt KTenMilliSecondsInMicroSeconds = 100000;
// values from fmradio LAF document
const TInt KLAFVarietyRDSViewerPortrait = 0;
const TInt KLAFVarietyRDSViewerLandscape = 1;
const TInt KLAFVarietyRDSPlusIndicatorPortrait = 0;
const TInt KLAFVarietyRDSPlusIndicatorLandscape = 1;
const TInt KLAFVarietyAFIndicatorPortrait = 0;
const TInt KLAFVarietyAFIndicatorLandscape = 1;
const TInt KLAFVarietyRDSIndicatorPortrait = 0;
const TInt KLAFVarietyRDSIndicatorLandscape = 1;
const TInt KLAFVarietyLogoImagePortrait = 0;
const TInt KLAFVarietyLogoImageLandscape = 1;
const TInt KLAFVarietyIndicatorPanePortrait = 0;
const TInt KLAFVarietyIndicatorPaneLandscape = 1;
const TInt KLAFVarietyVisualPanePortrait = 0;
const TInt KLAFVarietyVisualPaneLandscape = 1;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------------
// CFMRadioTimer::CFMRadioTimer
// ---------------------------------------------------------------------------------
CFMRadioTimer::CFMRadioTimer( MFMRadioTimer* aTimerObserver  ) : 
	CActive( EPriorityStandard ),
	iTimerObserver( aTimerObserver )
	{
	CActiveScheduler::Add( this );
	}
	
// ---------------------------------------------------------------------------------
// CFMRadioTimer::~CFMRadioTimer
// ---------------------------------------------------------------------------------
CFMRadioTimer::~CFMRadioTimer()
	{
	
	Cancel();
	}
	
// ---------------------------------------------------------------------------------
// CFMRadioTimer::RunL
// ---------------------------------------------------------------------------------
void CFMRadioTimer::RunL()
	{
	
	iTimerObserver->TimerTimeOut( iStatus.Int() );
	}
	
// ---------------------------------------------------------------------------------
// CFMRadioTimer::DoCancel
// ---------------------------------------------------------------------------------
void CFMRadioTimer::DoCancel()
	{
	
	TInt ret = KErrCancel;
	iTimerObserver->TimerTimeOut( ret );
	}
	
// ---------------------------------------------------------------------------------
// CFMRadioTimer::Activate
// ---------------------------------------------------------------------------------
void CFMRadioTimer::Activate()
	{
	SetActive();	
	}


// --------------------------------------------------------------------------------
// CFMRadioMainContainer::NewL
// Two-phase constructor of CFMRadioDelayTimer
// --------------------------------------------------------------------------------
//
CFMRadioMainContainer* CFMRadioMainContainer::NewL(
    const TRect& aRect,
    CAlfEnv& aAlfEnv,
    CRadioEngine& aRadioEngine )
    {
    CFMRadioMainContainer* self = new (ELeave) CFMRadioMainContainer( aAlfEnv, aRadioEngine  );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------------
// CFMRadioMainContainer::CFMRadioMainContainer
// Default constructor
// --------------------------------------------------------------------------------
//
CFMRadioMainContainer::CFMRadioMainContainer( CAlfEnv& aAlfEnv, CRadioEngine& aRadioEngine ) :
	iAlfEnv ( aAlfEnv ),
	iRadioEngine (aRadioEngine ),
	iLightsOffCounter(0)
	{
	//No implementation needed		
	}
	
// --------------------------------------------------------------------------------
// CFMRadioMainContainer::ConstructL
// EPOC two phased constructor
// --------------------------------------------------------------------------------
//
void CFMRadioMainContainer::ConstructL(
    const TRect& aRect )
    {
    iCoeEnv = CCoeEnv::Static();

    TRAP_IGNORE( iLight = CHWRMLight::NewL(this) );
	iTimer = new(ELeave) CFMRadioTimer( this );
	timer.CreateLocal();
	
    // check the layout orientation
    CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( iCoeEnv->AppUi() );
    TBool isLandscape = appUi->IsLandscapeOrientation();

    TRgb color;

    AknsUtils::GetCachedColor( AknsUtils::SkinInstance(), color, KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 );
	//CreateWindowL(); // create a window for the container
    	
    // Create alfred display
    CAlfDisplay* display = NULL; 
    if ( iAlfEnv.DisplayCount() > 0 )
    	{
    	display = &(iAlfEnv.PrimaryDisplay());
    	}
    else
    	{
    	display = &(iAlfEnv.NewDisplayL( aRect, CAlfEnv::ENewDisplayAsCoeControl ));
    	}

	// Setup background.
	display->SetClearBackgroundL( CAlfDisplay::EClearWithSkinBackground );	
	
	// Create the control group for HUI visual controls
	CAlfControlGroup* group;
	if( display->Roster().Count() > 0)
		{
		group = &( iAlfEnv.ControlGroup( KFMRadioVisualControlsGroupId ) );	
		
		iVisualControl = static_cast<CFMRadioAlfVisualizer*> 
								( display->Roster().FindControl( KFMRadioVisualControlId ) );
		iMediaIdle = static_cast<CFMRadioAlfMediaIdle*> 
								( display->Roster().FindControl( KFMRadioMediaIdleId ) );
		iRdsIndicator = static_cast<CFMRadioAlfIndicator*> 
								( display->Roster().FindControl( KFMRadioRdsIndicatorId ) ); 		
		iRdsAfIndicator = static_cast<CFMRadioAlfIndicator*> 
								( display->Roster().FindControl( KFMRadioRdsAfIndicatorId ) );								
		iRdsViewer = static_cast<CFMRadioAlfRDSViewer*> 
								( display->Roster().FindControl( KFMRadioRdsViewer ) );
		iRdsInteractionIndicator = static_cast<CFMRadioAlfIndicator*> 
						        ( display->Roster().FindControl( KFMRadioRdsInteractionIndicatorId ) ); 								

        iRadioLogo = static_cast<CFMRadioLogo*> 
            ( display->Roster().FindControl( KFMRadioLogoId ) );
        }
	else
		{
	    group = &(iAlfEnv.NewControlGroupL( KFMRadioVisualControlsGroupId ) );
	    // Create the vizualizer control for information of the FMRadio
	    iVisualControl = CFMRadioAlfVisualizer::NewL( iAlfEnv );
	    iVisualControl->SetId( KFMRadioVisualControlId );
	    // Create the media idle
	    iMediaIdle = CFMRadioAlfMediaIdle::NewL( iAlfEnv );
	    iMediaIdle->SetId( KFMRadioMediaIdleId );
	    
	    // RDS
	    HBufC* rdsText = StringLoader::LoadLC( R_QTN_FMRADIO_RDS, iCoeEnv );
	    iRdsIndicator = CFMRadioAlfIndicator::NewL( iAlfEnv );
	    iRdsIndicator->SetId( KFMRadioRdsIndicatorId );
	    iRdsIndicator->SetTextColor( color );
	    iRdsIndicator->SetTextL( *rdsText );
	    iRdsIndicator->SetOpacityInHiddenState( KFMRadioIndicatorOpacityInHiddenState );
	    CleanupStack::PopAndDestroy( rdsText );
	    
	    // AF
	    HBufC* afText = StringLoader::LoadLC( R_QTN_FMRADIO_AF, iCoeEnv );
	    iRdsAfIndicator = CFMRadioAlfIndicator::NewL( iAlfEnv );
	    iRdsAfIndicator->SetId( KFMRadioRdsAfIndicatorId );
	    iRdsAfIndicator->SetTextColor( color );
	    iRdsAfIndicator->SetTextL( *afText );
	    iRdsAfIndicator->SetOpacityInHiddenState( KFMRadioIndicatorOpacityInHiddenState );
	    CleanupStack::PopAndDestroy( afText );
	    
	    iRdsViewer = CFMRadioAlfRDSViewer::NewL( iAlfEnv );
	    iRdsViewer->SetId( KFMRadioRdsViewer );
	    iRdsViewer->SetTextColor( color );
	    iRdsViewer->SetOpacityInHiddenState( KFMRadioRdsViewerOpacityHidden );
	    
        // +
	    HBufC* interactionText = StringLoader::LoadLC( R_QTN_FMRADIO_RTPLUS_INDICATOR_TEXT, iCoeEnv );
	    iRdsInteractionIndicator = CFMRadioAlfIndicator::NewL( iAlfEnv );
        iRdsInteractionIndicator->SetId( KFMRadioRdsInteractionIndicatorId );
        iRdsInteractionIndicator->SetTextColor( color );
        iRdsInteractionIndicator->SetTextL( *interactionText );
        iRdsInteractionIndicator->SetOpacityInHiddenState( KFMRadioIndicatorOpacityInHiddenState );
        CleanupStack::PopAndDestroy( interactionText );
        
        // radio icon
        iRadioLogo = CFMRadioLogo::NewL( iAlfEnv );
        iRadioLogo->SetId( KFMRadioLogoId );
        
	    // Append the controls into the control group.	    
	    group->AppendL( iMediaIdle );
	    group->AppendL( iVisualControl );
        group->AppendL( iRdsIndicator );
        group->AppendL( iRdsAfIndicator );
        group->AppendL( iRdsViewer );
        group->AppendL( iRdsInteractionIndicator );
        group->AppendL( iRadioLogo );
        }

    iIdleController = CFMRadioIdleController::NewL();
    // Add media idle visuals to the controller.
    // Only one of the media idle visuals is visible at a time.
    iIdleController->AddControlItem( *iRdsViewer );
    iIdleController->AddControlItem( *iMediaIdle );
    iIdleController->AddControlItem( *iRadioLogo );

    iMainPaneRect = aRect;
    
    UpdateTextColorFromSkin();
    SizeChanged();
    //Make the control group active on the display        
	display->Roster().ShowL( *group );
	display->SetClearBackgroundL( CAlfDisplay::EClearWithSkinBackground );		
    }

// --------------------------------------------------------------------------------
// CFMRadioMainContainer::Destructor
// --------------------------------------------------------------------------------
//
CFMRadioMainContainer::~CFMRadioMainContainer()
    {
    if ( iTimer->IsActive() )
        {
        iTimer->Cancel();
        }

    delete iTimer;
    iTimer = NULL;
    timer.Close();

    delete iLight;
    iLight = NULL;
    delete iIdleController;
    }

// --------------------------------------------------------------------------------
// CFMRadioMainContainer::SetFaded
// Fades the entire window and controls in the window owned
// by this container control.
// -------------------------------------------------------------------------------
//
void CFMRadioMainContainer::SetFaded( TBool aFaded )
    {
    iFadeStatus = aFaded;
    }

// --------------------------------------------------------------------------------
// CFMRadioMainContainer::DisplayChannelInfoL
// Display the channel information
// --------------------------------------------------------------------------------
//
void CFMRadioMainContainer::DisplayChannelInfoL(
        TInt aChannelNumber,
        const TDesC& aStationName,
        TFMRadioStationChangeType aChangeType,
        TInt aFrequency )
    {
    HBufC* firstLine = NULL;
    HBufC* secondLine = NULL;
    TBool programmeServiceNameExists = iRadioEngine.RdsReceiver().ProgrammeService().Length();
    CFMRadioRdsReceiverBase::TFMRadioProgrammeSeviceType PSNameType = 
        iRadioEngine.RdsReceiver().ProgrammeServiceNameType();
    
    TBool useStaticPsName = EFalse;
    if ( !aStationName.Length() && 
         PSNameType == CFMRadioRdsReceiverBase::EFMRadioPSNameStatic &&
         programmeServiceNameExists )
        {
        useStaticPsName = ETrue;
        }
    const TDesC& stationName = useStaticPsName ? iRadioEngine.RdsReceiver().ProgrammeService() : aStationName;
    
    if ( !stationName.Length() )
        {
        // PS Name is concidered as dynamic
        if( aChannelNumber == KErrNotFound )
            {
            firstLine = FormattedFrequencyStringL( aFrequency, R_QTN_FMRADIO_FREQ );
            CleanupStack::PushL( firstLine );
            if( programmeServiceNameExists )
                {
                secondLine = iRadioEngine.RdsReceiver().ProgrammeService().AllocL();
                }
            }
        else
            {
            TReal frequency = static_cast<TReal>( aFrequency / static_cast<TReal>( KHzConversionFactor ));
    
            // Gets locale decimal separator automatically
            TInt maxDecimalPlaces = iRadioEngine.DecimalCount();
            TRealFormat format( KFrequencyMaxLength, maxDecimalPlaces );
            TBuf<KFrequencyMaxLength> frequencyString;
            frequencyString.Num( frequency, format );
            AknTextUtils::LanguageSpecificNumberConversion( frequencyString );
            
            firstLine = StringLoader::LoadLC( R_QTN_FMRADIO_MEM_LOCATION_FREQ,
                    frequencyString, aChannelNumber, iCoeEnv );
            
            if( programmeServiceNameExists )
                {
                secondLine = iRadioEngine.RdsReceiver().ProgrammeService().AllocL();
                }
            }
        }
    else
        {
        if( aChannelNumber == KErrNotFound )
            {
            firstLine = stationName.AllocLC();
            }
        else
            {
            firstLine = StringLoader::LoadLC( R_QTN_FMRADIO_MEM_LOCATION_NAME,
                    stationName, aChannelNumber, iCoeEnv );
            }
        
        if ( programmeServiceNameExists &&
             PSNameType == CFMRadioRdsReceiverBase::EFMRadioPSNameDynamic )
            {
            // PS Name is concidered as dynamic
            secondLine = iRadioEngine.RdsReceiver().ProgrammeService().AllocL();
            }
        else
            {
            secondLine = FormattedFrequencyStringL( aFrequency, R_QTN_FMRADIO_FREQ );
            }
        }
    CleanupStack::PushL( secondLine );
    
    if( AknLayoutUtils::LayoutMirrored() )
        {
        // Add right to left mark in the beginning so that the text is laid out correctly
        firstLine = firstLine->ReAllocL( firstLine->Length() + KRightToLeftMark().Length() );
        CleanupStack::Pop( 2 ); // secondLine and old firstLine
        CleanupStack::PushL( firstLine );
        CleanupStack::PushL( secondLine );
        firstLine->Des().Insert( 0, KRightToLeftMark );
        
        if ( secondLine )
            {
            secondLine = secondLine->ReAllocL( secondLine->Length() + KRightToLeftMark().Length() );
            CleanupStack::Pop(); // old secondLine
            CleanupStack::PushL( secondLine );
            secondLine->Des().Insert( 0, KRightToLeftMark );
            }
        }
    
    iVisualControl->ChangeStationL( aChangeType, *firstLine, secondLine );
    
    HBufC* frequencyString = FormattedFrequencyStringL( aFrequency, R_QTN_FMRADIO_MEDIA_IDLE_FREQ );
    CleanupStack::PushL( frequencyString );
    iMediaIdle->AddMediaIdleContentL( *frequencyString );
    CleanupStack::PopAndDestroy( frequencyString );
    if( aStationName.Length() )
        {
        iMediaIdle->AddMediaIdleContentL( stationName );
        }
    if ( aStationName != iRadioEngine.RdsReceiver().ProgrammeService() &&
         PSNameType == CFMRadioRdsReceiverBase::EFMRadioPSNameStatic )
        {
        iMediaIdle->AddPsNameToMediaIdleL( iRadioEngine.RdsReceiver().ProgrammeService() );
        }
    
    CleanupStack::PopAndDestroy( 2, firstLine );
    }

// --------------------------------------------------------------------------------
// CFMRadioMainContainer::FormattedFrequencyStringL
// Format the frequency
// --------------------------------------------------------------------------------
//
HBufC* CFMRadioMainContainer::FormattedFrequencyStringL( TInt aFrequency, TInt aResourceId  )
    {
    TReal frequency = static_cast<TReal>( aFrequency / static_cast<TReal>( KHzConversionFactor ));

    // Gets locale decimal separator automatically
    TInt maxDecimalPlaces = iRadioEngine.DecimalCount();
    TRealFormat format( KFrequencyMaxLength, maxDecimalPlaces );
    TBuf<KFrequencyMaxLength> frequencyString;
    frequencyString.Num( frequency, format );
    AknTextUtils::LanguageSpecificNumberConversion( frequencyString );
    
    return StringLoader::LoadL( aResourceId, frequencyString, iCoeEnv );;
    }

// --------------------------------------------------------------------------------
// CFMRadioMainContainer::ShowRTPlusInteractionIndicator
// Hide/show the RT+ interaction indicator with optional fade effecct
// --------------------------------------------------------------------------------
//
void CFMRadioMainContainer::ShowRTPlusInteractionIndicator( TBool aVisible, TBool aFadeEffect)
    {
    if (aVisible)
        iRdsInteractionIndicator->Show( aFadeEffect );
    else
        iRdsInteractionIndicator->Hide( aFadeEffect );
    }

// ---------------------------------------------------------
// CFMRadioMainContainer::ShowRdsInfo
// ---------------------------------------------------------
//
void CFMRadioMainContainer::ShowRdsInfo( TBool aShow, TBool aFade, TBool aShowAfMarquee )
    {    
    if( aShow )
    	{
    	iRdsIndicator->Show( aFade );
    	
    	if( aShowAfMarquee )
        	{
        	iRdsAfIndicator->Show( aFade );
        	}
    	else
    		{
    		iRdsAfIndicator->Hide( aFade ); 
    		}
    	}
    else
    	{
    	iRdsIndicator->Hide( aFade );
    	    	
    	iRdsAfIndicator->Hide( aFade ); 
    	}
    }

// ----------------------------------------------------------------------------------------------------
// CFMRadioMainContainer::HandleResourceChange
// Handles resource changes
// ----------------------------------------------------------------------------------------------------
//
void CFMRadioMainContainer::HandleResourceChange( TInt aType )
    {
	if ( aType ==  KEikDynamicLayoutVariantSwitch )
		{						
		AknLayoutUtils::LayoutMetricsRect(
		AknLayoutUtils::EMainPane, iMainPaneRect);
		SizeChanged();
        TRAP_IGNORE( iAlfEnv.NotifyLayoutChangedL() )
        iIdleController->ShowControl( *iRadioLogo );
		}
	else if ( aType == KAknsMessageSkinChange  )
	  	{
	    UpdateTextColorFromSkin();
	    TRAP_IGNORE( iAlfEnv.NotifySkinChangedL() );
	    }
    }

// --------------------------------------------------------------------------------
// CFMRadioMainContainer::SetRect
// Sets rectangle of the alfred display
// --------------------------------------------------------------------------------
//
void CFMRadioMainContainer::SetRect( const TRect& aRect )
	{
	iMainPaneRect = aRect;
	SizeChanged();
	}

// --------------------------------------------------------------------------------
// CFMRadioMainContainer::SizeChanged
// Called when the view size is changed
// --------------------------------------------------------------------------------
//
void CFMRadioMainContainer::SizeChanged()
    {
    CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( iCoeEnv->AppUi() );
    
    iAlfEnv.PrimaryDisplay().ForceSetVisibleArea( iMainPaneRect ); 
    
    TAknLayoutRect layoutRect;
    TAknLayoutText textLayout;
    TAknLayoutRect indicatorLayoutRect;
    TAknLayoutRect visualPaneLayoutRect;
    
    TRect displayRect( iMainPaneRect.Size() );
    
    iVisualControl->SetMirrored( appUi->IsLayoutMirrored() );
    TBool isLandscape = appUi->IsLandscapeOrientation();
    
    TDisplayOrientation alfDisplayOrientation = EPortrait;
    CFMRadioAlfMediaIdle::TOrientation mediaIdleOrientation = CFMRadioAlfMediaIdle::EPortrait;
    
    TInt lafVarietyRdsIndicator = 0;
    TInt lafVarietyAfIndicator = 0;
    TInt lafVarietyPlusIndicator = 0;
    TInt lafVarietyRdsText = 0;
    TInt lafVarietyLogo = 0;
    TInt lafVarietyIndicatorPane = 0;
    TInt lafVarietyVisualPane = 0;
    TBool useLandscapeImage = EFalse;
    
    if ( isLandscape )
    	{
    	alfDisplayOrientation = ELandscape;
    	mediaIdleOrientation = CFMRadioAlfMediaIdle::ELandscape;
    	
    	lafVarietyRdsIndicator = KLAFVarietyRDSIndicatorLandscape;
    	lafVarietyAfIndicator = KLAFVarietyAFIndicatorLandscape;
    	lafVarietyPlusIndicator = KLAFVarietyRDSPlusIndicatorLandscape;
    	lafVarietyRdsText = KLAFVarietyRDSViewerLandscape;
        lafVarietyLogo = KLAFVarietyLogoImageLandscape;
        lafVarietyIndicatorPane = KLAFVarietyIndicatorPaneLandscape;
        lafVarietyVisualPane = KLAFVarietyVisualPaneLandscape;
        useLandscapeImage = ETrue;
        }
    else
    	{
    	lafVarietyRdsIndicator = KLAFVarietyRDSIndicatorPortrait;
    	lafVarietyAfIndicator = KLAFVarietyAFIndicatorPortrait;
        lafVarietyPlusIndicator = KLAFVarietyRDSPlusIndicatorPortrait;
        lafVarietyRdsText = KLAFVarietyRDSViewerPortrait;
        lafVarietyLogo = KLAFVarietyLogoImagePortrait;
        lafVarietyIndicatorPane = KLAFVarietyIndicatorPanePortrait;
        lafVarietyVisualPane = KLAFVarietyVisualPanePortrait;
        }
    // indicator pane
    indicatorLayoutRect.LayoutRect( displayRect,
                                    AknLayoutScalable_Apps::fmrd2_indi_pane( lafVarietyIndicatorPane ) );
    
    // RDS indicator
    textLayout.LayoutText( indicatorLayoutRect.Rect(),
            AknLayoutScalable_Apps::fmrd2_indi_pane_t3( lafVarietyRdsIndicator ).LayoutLine() );
            
    iRdsIndicator->SetRect( textLayout.TextRect() );
	
    // AF indicator    
    textLayout.LayoutText( indicatorLayoutRect.Rect(),
            AknLayoutScalable_Apps::fmrd2_indi_pane_t2( lafVarietyAfIndicator ).LayoutLine() );
    
    iRdsAfIndicator->SetRect( textLayout.TextRect() );
    
    // + indicator
    textLayout.LayoutText( indicatorLayoutRect.Rect(),
            AknLayoutScalable_Apps::fmrd2_indi_pane_t1( lafVarietyPlusIndicator ).LayoutLine() );
    
    iRdsInteractionIndicator->SetRect( textLayout.TextRect() );

    // RDS Text pane      
    layoutRect.LayoutRect( displayRect,
            AknLayoutScalable_Apps::area_fmrd2_visual_pane( lafVarietyRdsText ).LayoutLine() );
    
    iRdsViewer->SetRect( layoutRect.Rect() );
    
    // radio icon    
    visualPaneLayoutRect.LayoutRect( displayRect,
            AknLayoutScalable_Apps::area_fmrd2_visual_pane( lafVarietyVisualPane ).LayoutLine() );
    
    layoutRect.LayoutRect( visualPaneLayoutRect.Rect(),
            AknLayoutScalable_Apps::area_fmrd2_visual_pane_g1( lafVarietyLogo ).LayoutLine() );

    // change image
    iRadioLogo->SwitchToLandscapeImage( useLandscapeImage );
    iRadioLogo->SetRect( layoutRect.Rect() );
    
    iVisualControl->SetOrientation( alfDisplayOrientation );
    iVisualControl->UpdateLayout( );
    iMediaIdle->SetOrientation( mediaIdleOrientation );
    iMediaIdle->UpdateLayout( );
    }

// --------------------------------------------------------------------------------------------------
// CFMRadioMainContainer::GetHelpContext
// --------------------------------------------------------------------------------------------------
//
void CFMRadioMainContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
#if defined __SERIES60_HELP || defined FF_S60_HELPS_IN_USE
    aContext.iMajor = TUid::Uid( KUidFMRadioApplication );
    aContext.iContext = KFMRADIO_HLP_MAIN;
#endif
    }

// --------------------------------------------------------------------------------
// CFMRadioMainContainer::UpdateTextColorFromSkin
// Handles updating main area text colors using current skin.
// --------------------------------------------------------------------------------
//
void CFMRadioMainContainer::UpdateTextColorFromSkin()
    {
	// Update text using skin color
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
   
    TRgb color( KRgbBlue );
    AknsUtils::GetCachedColor( skin, color, KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 );  
       
    iVisualControl->SetStationInformationTextColor( color );  
                   
    iMediaIdle->SetPrimaryColor( color );     
    
    AknsUtils::GetCachedColor( skin, color, KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG10 );
    
    iMediaIdle->SetSecondaryColor( color );    
    }

// ----------------------------------------------------------------------------------------------------
// CFMRadioMainContainer::VisualControl
// Returns a pointer to the station information visualizer
// ----------------------------------------------------------------------------------------------------
//
CFMRadioAlfVisualizer* CFMRadioMainContainer::VisualControl()
	{
	return iVisualControl;
	}

// ----------------------------------------------------------------------------------------------------
// CFMRadioMainContainer::MediaIdle
// Returns a pointer to Media Idle
// ----------------------------------------------------------------------------------------------------
//
CFMRadioAlfMediaIdle& CFMRadioMainContainer::MediaIdle()
    {
    return *iMediaIdle;
    }

// ---------------------------------------------------------------------------
// CFMRadioMainContainer::SetRdsRadioTextL
// Updates the rds information.
// ---------------------------------------------------------------------------
//
void CFMRadioMainContainer::SetRdsRadioTextL( const TDesC& aRadioText )
    {
    // Trim the text and check if there is actual content before showing it
    HBufC* text = aRadioText.AllocLC();
    text->Des().Trim();
    if ( text->Length() )
        {
        if ( iRdsViewer->SetTextL( *text ) )
            {
            iIdleController->ShowControl( *iRdsViewer );
            }
        }
    CleanupStack::PopAndDestroy( text );
    }

// ----------------------------------------------------------------------------------------------------
// TimerTimeOut
// ----------------------------------------------------------------------------------------------------
void CFMRadioMainContainer::TimerTimeOut( TInt aErrorStatus )
	{
	
	if ( aErrorStatus == KErrNone && iLightsOffCounter < KFadeFrames )
		{		
		iAlfEnv.RefreshCallBack( &( iAlfEnv ) );
		TTimeIntervalMicroSeconds32 time( KTenMilliSecondsInMicroSeconds );
				
		timer.After(iTimer->iStatus, time );
			
		iTimer->Activate();
		
		iLightsOffCounter++;
					
		}
	else
		{
		timer.Cancel();
		}
	
	}	

// ---------------------------------------------------------------------------
// CFMRadioAlfMediaIdle::LightStatusChangedL
// From MHWRMLightObserver
// Prepares media idle to handle new channel information
// ---------------------------------------------------------------------------
//	
void CFMRadioMainContainer::LightStatusChanged(TInt aTarget, 
                                        CHWRMLight::TLightStatus aStatus)
	{
    FTRACE( FPrint( _L("CFMRadioMainContainer::LightStatusChanged( Target: %d Lights: %d )"), aTarget, aStatus ) );
	if ( CHWRMLight::ELightOff == aStatus && aTarget == 1 )
		{
		CAlfDisplay* display = NULL; 
		
	    if( iAlfEnv.DisplayCount() > 0 )
	    	{	    	
	    	display = &(iAlfEnv.PrimaryDisplay());

	    	if ( display->Roster().Count() > 0 )
				{
                iIdleController->DeactivateControls();
				

				iAlfEnv.RefreshCallBack( &(iAlfEnv) );
				
				if ( !iTimer->IsActive() )
					{			
					TTimeIntervalMicroSeconds32 time( KTenMilliSecondsInMicroSeconds );
					// Just to make sure, that dimming is really happening
					iLightsOffCounter = 0;
					timer.After(iTimer->iStatus, time );
					iTimer->Activate();
					}
				}	
	    	}
	    }
	else if (  CHWRMLight::ELightOn == aStatus && aTarget == 1 )
		{
		if ( iTimer )
			{		
			if ( iTimer->IsActive() )
				{
				iTimer->Cancel();
				}
			}
			
		if ( iLightsOffCounter != 0 )
            {
            CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( iCoeEnv->AppUi() );
            CFMRadioMainView* mainView = static_cast<CFMRadioMainView*>( appUi->View( KFMRadioMainViewId ) );
            mainView->DetermineActiveMediaIdleComponent();
            
            iLightsOffCounter = 0;
            }
        }
    }

// --------------------------------------------------------------------------------
// CFMRadioMainContainer::RdsViewer
// --------------------------------------------------------------------------------
//
CFMRadioAlfRDSViewer& CFMRadioMainContainer::RdsViewer() const
    {
    return *iRdsViewer;
    }

// --------------------------------------------------------------------------------
// CFMRadioMainContainer::IdleController
// --------------------------------------------------------------------------------
//
CFMRadioIdleController& CFMRadioMainContainer::IdleController() const
    {
    return *iIdleController;
    }

// --------------------------------------------------------------------------------
// CFMRadioMainContainer::Logo
// --------------------------------------------------------------------------------
//
CFMRadioLogo& CFMRadioMainContainer::Logo() const
    {
    return *iRadioLogo;
    }
// End of File
