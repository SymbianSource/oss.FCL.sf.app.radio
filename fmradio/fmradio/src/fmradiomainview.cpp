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
* Description:  Implementation of the class CFMRadioMainView
*
*/

// INCLUDE FILES
#include <featmgr.h>
#include <aknViewAppUi.h>
#include <akntitle.h>
#include <avkon.hrh>
#include <avkon.rsg>
#include <avkon.mbg>
#include <data_caging_path_literals.hrh>
#include <akntoolbar.h>
#include <fmradio.rsg>
#include <StringLoader.h>
#include <aknbutton.h>
#include <aknconsts.h>
#include <aknmessagequerydialog.h>
#include <fmradio.mbg>
#include <fmradiouids.h>
#include <akntoolbarextension.h>

#include "fmradiomusicstorehandler.h"
#include "fmradiobacksteppingservicewrapper.h"
#include "fmradiodocument.h"
#include "fmradiordsreceiverbase.h"
#include "fmradioalfvisualizer.h"
#include "fmradioengine.h"
#include "fmradiomainview.h"
#include "fmradiomaincontainer.h"
#include "fmradioappui.h"
#include "fmradio.hrh"
#include "fmradioengineradiosettings.h"
#include "fmradioalfmediaidle.h"
#include "fmradiopubsub.h"
#include "fmradioalfrdsviewer.h"
#include "fmradioidlecontroller.h"
#include "fmradiologo.h"
#include "debug.h"

// A debug flag which is used for differentiating between original version and updated
// version, when set will add identifier on Main View title. Used for testing purposes
#undef __FM_UPDATE_PACKAGE__

// CONSTANTS

#ifdef __FM_UPDATE_PACKAGE__
_LIT( KFMRadioUpdated, " *" );
#endif

// Delay in milliseconds for fading out the RDS indicator
const TInt KFMRadioRdsSignalFadeOutDelay = 1000;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFMRadioMainView::NewL
// Two-phase constructor of CFMRadioMainView
// ---------------------------------------------------------
//
CFMRadioMainView* CFMRadioMainView::NewL(
    CRadioEngine* aRadioEngine, CAlfEnv& aAlfEnv, MChannelListHandler& aObserver )
    {
    CFMRadioMainView* self = new (ELeave) CFMRadioMainView( aRadioEngine, aAlfEnv, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CFMRadioMainView::ConstructL
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CFMRadioMainView::ConstructL()
    {
    BaseConstructL( R_FMRADIO_MAIN_VIEW );
    PrepareToolbar();
    
    if ( !AknLayoutUtils::PenEnabled() )
        {
        ShowToolbar( EFalse );
        }

    iRdsSignalTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    iMusicStoreHandler = CFMRadioMusicStoreHandler::NewL( R_FMRADIO_MAIN_OPTIONS_MENU );
    iMusicStoreHandler->EnableMusicStore( ETrue );
    
    CAknToolbarExtension* toolbarExt = iToolbar->ToolbarExtension();
    CAknButton* operatorStoreButton = NULL;
    operatorStoreButton = static_cast<CAknButton*>( toolbarExt->ControlOrNull( EFMRadioToolbarButtonCmdOperatorMusicStore ) );
    const TDesC& storeName = iMusicStoreHandler->OperatorMusicStoreName();
    
    if ( operatorStoreButton )
        {
        // set text for button name and tooltip
        operatorStoreButton->State()->SetTextL( storeName );
        operatorStoreButton->State()->SetHelpTextL( storeName );
        }
    }

// ---------------------------------------------------------
// CFMRadioMainView::CFMRadioMainView
// default constructor
// ---------------------------------------------------------
//
CFMRadioMainView::CFMRadioMainView(
    CRadioEngine* aRadioEngine,
    CAlfEnv& /*aAlfEnv*/,
    MChannelListHandler& aObserver)
    : iRadioEngine( aRadioEngine ),
      iSeeking( EFalse ),
      iChangeDirection( EFMRadioStationChangeNone ),
      iStartUp( ETrue ),
      iObserver( aObserver )
    {
    }

// ---------------------------------------------------------
// CFMRadioMainView::~CFMRadioMainView
// Class destructor
// ---------------------------------------------------------
//
CFMRadioMainView::~CFMRadioMainView()
    {
    if( iRdsSignalTimer && iRdsSignalTimer->IsActive() )
        {
        iRdsSignalTimer->Cancel();
        }   
    if ( iRdsSignalTimer )
    	{
	    delete iRdsSignalTimer;
	    iRdsSignalTimer = NULL;
    	}

    CAlfEnv* alfEnv = CAlfEnv::Static();
    if ( alfEnv && iContainer )
        {
        iContainer->RdsViewer().RemoveObserver( this );
        iContainer->Logo().RemoveObserver( this );
        }
    
    delete iContainer;
    iContainer = NULL;

    delete iMusicStoreHandler;
    }

// ---------------------------------------------------------
// TUid CFMRadioMainView::Id
// Return the unique identifier corresponding to this view
// ---------------------------------------------------------
//
TUid CFMRadioMainView::Id() const
    {
    return KFMRadioMainViewId;
    }

// ---------------------------------------------------------
// CFMRadioMainView::StopSeekL
// Seek operation has completed. Update the view to reflect
// this fact.
// ---------------------------------------------------------
//
void CFMRadioMainView::StopSeekL()
    {
    FTRACE( FPrint( _L("CFMRadioMainView::StopSeekL  Start") ) );
    if ( iContainer && iSeeking)
        {
        FTRACE( FPrint( _L("CFMRadioMainView::StopSeekL  inside first if") ) );
        if ( !AknLayoutUtils::PenEnabled() )
            {
            Cba()->SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_BACK ); // Change softkeys
            Cba()->SetCommandL( CEikButtonGroupContainer::EMiddleSoftkeyPosition, 0, KNullDesC );
            }
        else
            {
            Cba()->SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_BACK__SELECT ); // Change softkeys
            }
        
        SetToolbarDimmedState( EFalse );
        iContainer->VisualControl()->StopTuningAnimation();
        iSeeking = EFalse;
        Cba()->DrawDeferred(); // Redraw softkeys
        }
    
    CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( iCoeEnv->AppUi() );
    
    TInt presetIndex = iRadioEngine->GetPresetIndex();
    TInt channelCount = iObserver.Channels()->Count();
    
    if ( iRadioEngine->GetRadioMode() == CRadioEngine::ERadioPresetMode && 
            channelCount > 0 &&
            presetIndex < channelCount )
        {
        DisplayChannelL( presetIndex );
        FTRACE( FPrint( _L("CFMRadioMainView::StopSeekL  ERadioPresetMode preset index = %d"),iRadioEngine->GetPresetIndex() ) );
        }
    else
        {
        FTRACE( FPrint( _L("CFMRadioMainView::StopSeekL  inside else") ) );
        TInt channelIndex = appUi->MatchingChannelL( iRadioEngine->GetTunedFrequency() ); //, need to get here!
        if ( channelIndex != KErrNotFound )
            {
            FTRACE( FPrint( _L("CFMRadioMainView::StopSeekL  if channelIndex is found, channelIndex = %d"),channelIndex ) );
            iRadioEngine->TunePresetL( channelIndex ); //update engine to let it know that we've locked onto a preset.
            }
        else
            {
            FTRACE( FPrint( _L("CFMRadioMainView::StopSeekL  else, get tuned freq = %d"),iRadioEngine->GetTunedFrequency() ) );
            DisplayChannelL( KErrNotFound );
            }
        }

    // show default radio logo
    if ( iContainer )
        {
        iContainer->IdleController().ShowControl( iContainer->Logo() );
        }
    }

// ---------------------------------------------------------
// CFMRadioMainView::FadeAndShowExit
// Fade the view and all controls associated with it. Change
// the soft keys so that "Exit" is displayed
// ---------------------------------------------------------
//
void CFMRadioMainView::FadeAndShowExit(
    TBool aFaded )
    {
    iFaded = aFaded;
    
    if ( iContainer )
        {
        if ( aFaded || iRadioEngine->IsInCall() ) // Needs to be faded - headset missing
            {
            StopDisplayingMenuBar(); // In case options menu is open when headset is disconnected
            SetToolbarDimmedState( ETrue );
            TRAP_IGNORE( Cba()->SetCommandSetL( R_FMRADIO_SOFTKEYS_EXIT ) ); // Change softkeys
            }
        else // Unfade
            {
            SetToolbarDimmedState( EFalse );
            if ( !AknLayoutUtils::PenEnabled() )
                {
                TRAP_IGNORE( Cba()->SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_BACK ); // Change softkeys
                    Cba()->SetCommandL( CEikButtonGroupContainer::EMiddleSoftkeyPosition, 0, KNullDesC ); )
                }
            else
                {
                TRAP_IGNORE ( Cba()->SetCommandSetL( R_AVKON_SOFTKEYS_OPTIONS_BACK__SELECT ); )// Change softkeys
                }
            }
        iContainer->SetFaded( aFaded ); // Unfade controls inside container
        Cba()->DrawDeferred(); // Redraw softkeys
        }
    }

// ---------------------------------------------------------
// CFMRadioMainView::UpdateDisplayForFocusGained
// Update display to override default behavior
// Can be removed when auto resume is implemented
// ---------------------------------------------------------
//
void CFMRadioMainView::UpdateDisplayForFocusGained()
    {
    FTRACE( FPrint( _L("CFMRadioMainView::UpdateDisplayForFocusGained") ) );
    FadeAndShowExit( iFaded );
    DetermineActiveMediaIdleComponent();
    }

// ---------------------------------------------------------
// CFMRadioMainView::SeekL
// Tells the window owning container to display seek note
// and updates softkeys
// ---------------------------------------------------------
//
void CFMRadioMainView::SeekL()
    {
    if ( iContainer )
        {
		HBufC* seekString = StringLoader::LoadLC( R_QTN_FMRADIO_TUNING, iCoeEnv );									
		iContainer->VisualControl()->SetStaticInfoTextL( iChangeDirection, *seekString );
		CleanupStack::PopAndDestroy( seekString );				
        iContainer->RdsViewer().Reset();
        iContainer->MediaIdle().ResetMediaIdleContent();
		iContainer->ShowRTPlusInteractionIndicator( EFalse, ETrue );
        iContainer->IdleController().ShowControl( iContainer->Logo() );
		iContainer->VisualControl()->StartTuningAnimation();
        iSeeking = ETrue;
        SetToolbarDimmedState( ETrue );
        // force update for toolbar extension view
        iToolbar->ToolbarExtension()->SetShown( EFalse );
        }
    Cba()->SetCommandSetL( R_FMRADIO_MAIN_VIEW_TUNING_STATE_SOFTKEYS );

    Cba()->DrawDeferred(); // Redraw softkeys
    }

// ---------------------------------------------------------
// CFMRadioMainView::ShowToolbar
// Sets toolbar visibility.
// ---------------------------------------------------------
//
void CFMRadioMainView::ShowToolbar( TBool aVisible )
	{
	if ( iToolbar )
		{
		iToolbar->SetToolbarVisibility( aVisible );
		iToolbar->UpdateBackground();
		iToolbar->DrawDeferred();			
		}
	}

// ---------------------------------------------------------
// CFMRadioMainView::HandleForegroundEventL
// ---------------------------------------------------------
//
void CFMRadioMainView::HandleForegroundEventL(TBool aForeground)
    {
    CAknView::HandleForegroundEventL(aForeground);
    CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( AppUi() );
    if ( aForeground )
        {	 
        if ( iStartUp )
            {
            iStartUp = EFalse;
            CFMRadioRdsReceiverBase& receiver = iRadioEngine->RdsReceiver();
            TBool afEnabled = iRadioEngine->RadioSettings().IsRdsAfSearchEnabled();
            iContainer->ShowRdsInfo( receiver.SignalAvailable(), ETrue, afEnabled );
            TBool showMusicStore = IsRTPlusInterActionIndicatorNeeded();
            iContainer->ShowRTPlusInteractionIndicator(showMusicStore, ETrue);
            }
        }
    else
        {
        CAlfEnv* env = CAlfEnv::Static();
        if ( iContainer && env )
            {
            iContainer->IdleController().DeactivateControls();
            }
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioMainView::RdsDataProgrammeService
// ---------------------------------------------------------------------------
//
void CFMRadioMainView::RdsDataProgrammeService( const TDesC& aProgramService )
    {
	TRAP_IGNORE( 
		RdsDataProgrammeServiceL( aProgramService ) 
		);
    }

// ---------------------------------------------------------------------------
// CFMRadioMainView::RdsDataPsNameIsStatic
// ---------------------------------------------------------------------------
//
void CFMRadioMainView::RdsDataPsNameIsStatic( TBool aStatic )
    {
    TRAP_IGNORE( HandleStaticProgrammeServiceL( aStatic ) )
    }

// ---------------------------------------------------------------------------
// CFMRadioMainView::RdsDataProgrammeServiceL
// ---------------------------------------------------------------------------
//
void CFMRadioMainView::RdsDataProgrammeServiceL( const TDesC& aProgramService )
    {
    FTRACE(FPrint(_L("CFMRadioMainView::RdsDataProgrammeServiceL('%S')"), &aProgramService));
    if( iContainer )
        {
        CAlfEnv* alfEnv = CAlfEnv::Static();
        if( alfEnv )
            {
            if(  aProgramService.Length() > 0 )
                {
                TInt presetIndexToDisplay = KErrNotFound;
                TInt presetIndex = iRadioEngine->GetPresetIndex();
                // When we haven't preset name, ps name will save as preset name
                if ( iRadioEngine->GetRadioMode() == CRadioEngine::ERadioPresetMode &&
                        presetIndex < iObserver.Channels()->Count() )
                    {
                    presetIndexToDisplay = presetIndex;
                    }
                SetStationChangeType( EFMRadioStationChangeNone );
                DisplayChannelL( presetIndexToDisplay );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioMainView::HandleStaticProgrammeServiceL
// ---------------------------------------------------------------------------
//
void CFMRadioMainView::HandleStaticProgrammeServiceL( TBool aStatic )
    {
    FTRACE(FPrint(_L("CFMRadioMainView::HandleStaticProgrammeServiceL(%d)"), aStatic));
    
    if( iContainer )
        {
        if ( aStatic )
            {
            // When we haven't got a name for preset, a static ps name will be saved
            TInt channelCount = iObserver.Channels()->Count();
            TInt currentPresetIx = iRadioEngine->GetPresetIndex();
            
            if ( aStatic && iRadioEngine->GetRadioMode() == CRadioEngine::ERadioPresetMode &&
                    currentPresetIx != KErrNotFound &&
                    channelCount > 0 &&
                    currentPresetIx < channelCount )
                {
                TDesC& stationName = iObserver.Channels()->At( currentPresetIx ).iChannelInformation;
                TInt presetFreq = iObserver.Channels()->At( currentPresetIx ).iChannelFrequency;
        
                if ( stationName.Length() == 0 && 
                        presetFreq != KErrNotFound && 
                        iRadioEngine->RdsReceiver().ProgrammeService().Length() )
                    {  
                    iObserver.Channels()->At( currentPresetIx ).iChannelInformation = iRadioEngine->RdsReceiver().ProgrammeService();
                    iObserver.UpdateChannelsL( EStoreIndexToRepository, currentPresetIx, 0 );
                    // Update station information display
                    DisplayChannelL( currentPresetIx );
                    }  
                }
            iContainer->MediaIdle().AddPsNameToMediaIdleL( iRadioEngine->RdsReceiver().ProgrammeService() );
            }
        }
    }

// ---------------------------------------------------------
// CFMRadioMainView::RdsDataRadioText
// ---------------------------------------------------------
//
void CFMRadioMainView::RdsDataRadioText( const TDesC& aRadioText )
    {
    FTRACE(FPrint(_L("CFMRadioMainView::RdsDataRadioText('%S')"), &aRadioText));
    if ( iContainer && aRadioText.Length() > 0 )
        {
            TRAP_IGNORE( iContainer->SetRdsRadioTextL( aRadioText ) );
        }
    }

// ---------------------------------------------------------
// CFMRadioMainView::RdsDataRadioTextPlus
// ---------------------------------------------------------
//
void CFMRadioMainView::RdsDataRadioTextPlus( const TInt /*aRadioTextPlusClass*/, const TDesC& /*aRadioText*/ )
    {
    if ( IsRTPlusInterActionIndicatorNeeded() )
        {
        iContainer->ShowRTPlusInteractionIndicator( ETrue, ETrue );
        if ( !iRadioEngine->MusicStoreNoteDisplayed() )
            {
            TRAP_IGNORE( ShowRTPlusFirstTimeDialogL() )
            }
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioMainView::IsRTPlusInterActionIndicatorNeeded
// ---------------------------------------------------------------------------
//
TBool CFMRadioMainView::IsRTPlusInterActionIndicatorNeeded()
    {
    CFMRadioRdsReceiverBase& receiver = iRadioEngine->RdsReceiver();
    TBool songInformation = receiver.RtPlusSong().Length() || 
                            receiver.RtPlusArtist().Length() ||
                            receiver.RtPlusAlbum().Length();
    TBool webLink = receiver.RtPlusProgramUrl().Length() &&
                    iRadioEngine->GetRTPlusSupportLevel() == EFMRadioAllInteractions;

    return songInformation || webLink;
    }

// ---------------------------------------------------------------------------
// CFMRadioMainView::ShowRTPlusFirstTimeDialogL
// ---------------------------------------------------------------------------
//
void CFMRadioMainView::ShowRTPlusFirstTimeDialogL()
    {
    // to avoid further calls
    iRadioEngine->SetMusicStoreNoteDisplayed();
    
    HBufC* noteTxt = StringLoader::LoadLC( R_QTN_FMRADIO_RTPLUS_FIRSTTIME, iCoeEnv );

    CAknMessageQueryDialog* rdsFirstTimeDialog = new (ELeave) CAknMessageQueryDialog;
    CleanupStack::PushL( rdsFirstTimeDialog );
    rdsFirstTimeDialog->SetMessageTextL( *noteTxt );
    CleanupStack::Pop( rdsFirstTimeDialog );
    rdsFirstTimeDialog->ExecuteLD( R_FMRADIO_RTPLUS_FIRSTTIME_NOTE );

    CleanupStack::PopAndDestroy( noteTxt );
    }

// ---------------------------------------------------------------------------
// CFMRadioMainView::RdsAfSearchBegin
// ---------------------------------------------------------------------------
//
void CFMRadioMainView::RdsAfSearchBegin()
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioMainView::RdsAfSearchEnd
// ---------------------------------------------------------------------------
//
void CFMRadioMainView::RdsAfSearchEnd( TUint32 /*aFrequency*/, TInt /*aError*/ )
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioMainView::RdsAfSearchStateChange
// ---------------------------------------------------------------------------
//
void CFMRadioMainView::RdsAfSearchStateChange( TBool aEnabled )
    { 
    CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>(AppUi());
    if( appUi->ActiveView() == KFMRadioMainViewId )
        {
        CFMRadioRdsReceiverBase& receiver = iRadioEngine->RdsReceiver();
        TBool show = receiver.SignalAvailable();
        iContainer->ShowRdsInfo( show, ETrue, aEnabled );
       }
    }

// ---------------------------------------------------------------------------
// CFMRadioMainView::RdsAvailable
// ---------------------------------------------------------------------------
//
void CFMRadioMainView::RdsAvailable( TBool aAvailable )
    {
    CAlfEnv* alfEnv = CAlfEnv::Static();
    if( alfEnv )
        {
        CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>(AppUi());

        if( appUi->ActiveView() == KFMRadioMainViewId )
            {
            if( aAvailable )
                {
                iRdsSignalTimer->Cancel();
                TBool afEnabled = iRadioEngine->RadioSettings().IsRdsAfSearchEnabled();
                iContainer->ShowRdsInfo( ETrue, ETrue, afEnabled );
                }
            else
                {
                // Start timer only if it wasn't activated before
                if( !iRdsSignalTimer->IsActive() )
                    {
                    // Used for conversing milliseconds to format timer understand, ie. microseconds
                    const TInt timerMultiplier = 1000;

                    // Start timer to fade rds icon after a delay
                    iRdsSignalTimer->Start( TTimeIntervalMicroSeconds32( KFMRadioRdsSignalFadeOutDelay * timerMultiplier ),
                            TTimeIntervalMicroSeconds32(0), TCallBack( StaticRdsSignalTimerCallback, this ) );
                    }
                }
            }
        }
    }

// ---------------------------------------------------------
// CFMRadioMainView::StaticRdsSignalTimerCallback
// ---------------------------------------------------------
TInt CFMRadioMainView::StaticRdsSignalTimerCallback( TAny* aSelfPtr )
    {
    CFMRadioMainView* self = reinterpret_cast<CFMRadioMainView*>(aSelfPtr);
    if(self)
        {
        self->iRdsSignalTimer->Cancel();
        if(self->iContainer)
            {
            self->iContainer->ShowRdsInfo(EFalse, ETrue);
            }
        }
    return KErrNone;
    }

// ---------------------------------------------------------
// CFMRadioMainView::DisplayChannelL
// Inform the window owning container that it should display
// the frequency and name of the channel specified.
// ---------------------------------------------------------
//
void CFMRadioMainView::DisplayChannelL( TInt aChannelIndex )
    {
    FTRACE(FPrint(_L("DisplayChannelL Start, index: %d "), aChannelIndex));

    if ( iContainer )
        { 
        iContainer->MediaIdle().ResetMediaIdleContent();
        
        if ( aChannelIndex != KErrNotFound )
            {
            CRadioEngine::TStationName stationName;
            TInt channelNumber = aChannelIndex + 1;
            TInt channelFrequency = KErrNotFound;
            
            iRadioEngine->GetPresetNameAndFrequencyL( aChannelIndex, stationName, channelFrequency );
            
            TFMRadioStationChangeType seekDirection = iChangeDirection;
            
            // Hide index number as the frequency was set by RDS AF search.
            if ( iRadioEngine->FrequencySetByRdsAf() && channelFrequency != iRadioEngine->GetTunedFrequency() )
                {
                channelNumber = KErrNotFound;
                channelFrequency = iRadioEngine->GetTunedFrequency();
                seekDirection = EFMRadioStationChangeNone;
                }
    
            // Update view
            iContainer->DisplayChannelInfoL( channelNumber, 
                                             stationName, 
                                             seekDirection, 
                                             channelFrequency );
            }
        else
            {
            iContainer->DisplayChannelInfoL( aChannelIndex, 
                                             KNullDesC, 
                                             iChangeDirection, 
                                             iRadioEngine->GetTunedFrequency() );
            }
        
        FTRACE( FPrint(_L("DisplayChannelL End") ) );
        }
    }

// ---------------------------------------------------------
// CFMRadioMainView::HandleCommandL
// All commands are handled by the AppUi
// ---------------------------------------------------------
//
void CFMRadioMainView::HandleCommandL( TInt aCommand )
    {
    CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( AppUi() );
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
            {
            const TVwsViewId viewId( TUid::Uid( KUidFMRadioApplication ), Id() );
            if ( !appUi->BackSteppingWrapper().HandleBackCommandL( viewId ) )
                {
                // Command not consumed by Back Stepping Service, handle here by going background
                TApaTask task( iEikonEnv->WsSession() );
                task.SetWgId( iEikonEnv->RootWin().Identifier() );
                task.SendToBackground();
                }
            break;
            }
        case EMPXPbvCmdInternetGoToWeb:
            {
            LaunchBrowserL( aCommand );
            break;
            }
        case EFMRadioCmdChannelList:
            {
            HandleSaveChannelQueryL();
            appUi->HandleCommandL( aCommand );
            break;
            }
        case EFMRadioRockerButtonKeyUp:
        case EFMRadioRockerButtonKeyDown:
            {
            // NOP
            break;
            }
        case EAknSoftkeySelect: // MSK
            {
            if ( !iFaded && !iSeeking )
                {
                HandleSaveChannelQueryL();
                appUi->HandleCommandL( EFMRadioCmdChannelList );
                }
            break;
            }
        case EFMRadioRockerButtonKeyLeft:
            {
            if ( !iFaded && !iSeeking )
                {
                appUi->HandleCommandL( EFMRadioCmdPrevChannel );
                }
            break;
            }
        case EFMRadioRockerButtonKeyLeftLong:
            {
            if ( !iFaded && !iSeeking )
                {
                appUi->HandleCommandL( EFMRadioCmdSeekDown );
                }
            break;
            }
        case EFMRadioRockerButtonKeyRight:
            {
            if ( !iFaded && !iSeeking )
                {
                appUi->HandleCommandL( EFMRadioCmdNextChannel );
                }
            break;
            }
         case EFMRadioRockerButtonKeyRightLong:
            {
            if ( !iFaded && !iSeeking )
                {
                appUi->HandleCommandL( EFMRadioCmdSeekUp );
                }
            break;
            }
         case EFMRadioCmdErase:
             {
             if ( appUi->EraseChannelL( iRadioEngine->GetPresetIndex() ) )
                 {
                 // delete has been confirmed, update station info
                 SetStationChangeType( EFMRadioStationChangeNone );
                 iRadioEngine->SetTunerModeOn();
                 DisplayChannelL( KErrNotFound );
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
         default:
            {
            if ( iMusicStoreHandler->IsMusicStoreCommandId( aCommand ) )
                {
                LaunchBrowserL( aCommand );
                }
            else
                {
                appUi->HandleCommandL( aCommand );
                }
            break;
            }
        }
    }

// ----------------------------------------
// Launch Browser
// ----------------------------------------
void CFMRadioMainView::LaunchBrowserL( TInt aCommandId )
    {
    CFMRadioRdsReceiverBase& receiver = iRadioEngine->RdsReceiver();
    switch ( aCommandId )
        {
        case EMPXPbvCmdInternetGoToWeb:
            iMusicStoreHandler->LaunchWebPageL( receiver.RtPlusProgramUrl() );
            break;
        default:
            iMusicStoreHandler->LaunchMusicStoreL( aCommandId, 
                                                   receiver.RtPlusSong(),
                                                   receiver.RtPlusArtist(),
                                                   receiver.RtPlusAlbum() );
            break;
        }
    }

// ----------------------------------------------------------------------------------------------------
// CFMRadioMainView::NotifyRdsTextTimerCompletion()
// From MFMRadioAlfRdsViewObserver
// Called when same RT has stayed on display for 60 sec.
// ----------------------------------------------------------------------------------------------------
//
void CFMRadioMainView::NotifyRdsTextTimerCompletion()
    {
    DetermineActiveMediaIdleComponent();
    }
// ---------------------------------------------------------
// CFMRadioMainView::DoActivateL
// Activate the main view
// ---------------------------------------------------------
//
void CFMRadioMainView::DoActivateL(
        const TVwsViewId& /*aPrevViewId*/,
        TUid /*aCustomMessageId*/,
        const TDesC8& /*aCustomMessage*/)
    {
    CAknTitlePane* titlePane = static_cast<CAknTitlePane*>( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    titlePane->SetTextToDefaultL();
#ifdef __FM_UPDATE_PACKAGE__
    const TDesC* titleText = titlePane->Text();
    TBufC<2> titleAddition( KFMRadioUpdated );
    HBufC* newTitle = HBufC::NewLC( titleText->Length() + titleAddition.Length() );
    newTitle->Des().Copy( *titleText );
    newTitle->Des().Append( titleAddition );
    titlePane->SetTextL( *newTitle, ETrue );
    CleanupStack::PopAndDestroy( newTitle );
#endif

    if ( !iContainer )
        {
        CAlfEnv* alfEnv = CAlfEnv::Static();
        CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( AppUi() );
        iContainer = CFMRadioMainContainer::NewL( ClientRect() , *alfEnv, *iRadioEngine );
        iContainer->RdsViewer().SetObserver( this );
        iContainer->Logo().SetObserver( this );
        }
    if ( !AknLayoutUtils::PenEnabled() )
        {
        // explicitly remove the middle softkey that shouldn't be there.
        Cba()->SetCommandL( CEikButtonGroupContainer::EMiddleSoftkeyPosition, 0, KNullDesC );
        }

    SetStationChangeType( EFMRadioStationChangeNone );

    if ( !iStartUp )
        {
        StopSeekL(); //used to trigger a UI update
        }
    else
        {
        AppUi()->HandleCommandL( EFMRadioCmdUpdateVolume );
        }

    AppUi()->HandleCommandL( EFMRadioCmdMainViewActive );

    iRadioEngine->RdsReceiver().AddObserver( this );

    TBool afEnabled = iRadioEngine->RadioSettings().IsRdsAfSearchEnabled();

    iContainer->ShowRdsInfo( iRadioEngine->RdsReceiver().SignalAvailable(), EFalse, afEnabled );
    FadeAndShowExit(iFaded);
    }

// ---------------------------------------------------------
// CFMRadioMainView::DoDeactivate
// Deactivate the main view
// ---------------------------------------------------------
//
void CFMRadioMainView::DoDeactivate()
    {
    CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>(AppUi());
    
    if ( appUi->RadioEngine() )
        {
        iRadioEngine->RdsReceiver().RemoveObserver( this );
    	}
    
    if ( iContainer )
        {
        CAlfEnv* alfEnv = CAlfEnv::Static();
        if ( alfEnv )
            {
            iContainer->RdsViewer().RemoveObserver( this );
            iContainer->RdsViewer().Reset();
            iContainer->Logo().RemoveObserver( this );
            iContainer->IdleController().HideAllControls();
            }
        delete iContainer;
	    iContainer = NULL;
        }
    }
	
// ---------------------------------------------------------
// CFMRadioMainView::DynInitMenuPaneL
// Sets the state of menu items dynamically according to the
// state of application data.
// ---------------------------------------------------------
//
void CFMRadioMainView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    iMusicStoreHandler->InitializeMenuL( aResourceId, aMenuPane );
    
    if ( aResourceId == R_FMRADIO_MAIN_OPTIONS_MENU ) // Main menu
        {
        if ( IsWebLinkAvailable() )
            {
            aMenuPane->SetItemDimmed( EMPXPbvCmdInternetGoToWeb, EFalse );
            }
        else
            {
            aMenuPane->SetItemDimmed( EMPXPbvCmdInternetGoToWeb, ETrue );
            }

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
	            else
	                {
	                aMenuPane->DeleteMenuItem( EFMRadioCmdDeactivateIhf );
	                }
	            }
	        else
	            {
	            // Delete existing menu option and add a new one
	            aMenuPane->DeleteMenuItem( EFMRadioCmdDeactivateIhf );
	            aMenuPane->SetItemTextL( EFMRadioCmdActivateIhf,R_QTN_FMRADIO_OPTIONS_ACTIVATE );
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
        CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( iCoeEnv->AppUi() );
        if ( appUi->MatchingChannelL( iRadioEngine->GetTunedFrequency() ) == KErrNotFound ) // current frequency not saved
           	{
           	aMenuPane->SetItemDimmed( EFMRadioCmdRename, ETrue );
           	aMenuPane->SetItemDimmed( EFMRadioCmdSaveChannel, EFalse );
            aMenuPane->SetItemDimmed( EFMRadioCmdErase, ETrue );
           	}
        else
        	{
        	aMenuPane->SetItemDimmed( EFMRadioCmdRename, EFalse );
        	aMenuPane->SetItemDimmed( EFMRadioCmdSaveChannel, ETrue );
            aMenuPane->SetItemDimmed( EFMRadioCmdErase, EFalse );
        	}
        }
        
    if ( aResourceId == R_FMRADIO_OPTIONS_AF_SUB_MENU ) // Rds af search sub menu
        {
        TBool afEnabled = iRadioEngine->RadioSettings().IsRdsAfSearchEnabled();
	    if ( afEnabled )
	    	{
			aMenuPane->SetItemButtonState( EFMRadioCmdDisableRdsAfSearch, EEikMenuItemSymbolIndeterminate );        	
        	aMenuPane->SetItemButtonState( EFMRadioCmdEnableRdsAfSearch, EEikMenuItemSymbolOn );
        	}
        else
        	{      	
			aMenuPane->SetItemButtonState( EFMRadioCmdEnableRdsAfSearch, EEikMenuItemSymbolIndeterminate );
        	aMenuPane->SetItemButtonState( EFMRadioCmdDisableRdsAfSearch, EEikMenuItemSymbolOn );
        	}
        }
    
    }

// ---------------------------------------------------------
// CFMRadioMainView::LayoutChangedL
// ---------------------------------------------------------
//
void CFMRadioMainView::LayoutChangedL( TInt aType )
    {
    if ( iContainer ) 
    	{
    	iContainer->HandleResourceChange( aType );
    	}
    }
    
// ---------------------------------------------------------
// CFMRadioMainView::SetStationChangeType
// ---------------------------------------------------------
//
void CFMRadioMainView::SetStationChangeType( TFMRadioStationChangeType aChangeType )
	{
	iChangeDirection = aChangeType;
	}
	
// ---------------------------------------------------------
// CFMRadioMainView::PrepareViewForChannelChange
// ---------------------------------------------------------
//
void CFMRadioMainView::PrepareViewForChannelChange()
	{
	TRAP_IGNORE( PrepareViewForChannelChangeL() );
	}
	
// ---------------------------------------------------------
// CFMRadioMainView::PrepareViewForChannelChangeL
// ---------------------------------------------------------
//
void CFMRadioMainView::PrepareViewForChannelChangeL()
	{	
	switch ( iChangeDirection ) 
		{
		case EFMRadioStationChangeNext: // fall through
        case EFMRadioStationChangePrevious:
        case EFMRadioStationChangeScanUp:
        case EFMRadioStationChangeScanDown:
			{
			if( iContainer )
				{
				iContainer->VisualControl()->ScrollOutCurrentStationInformationL( iChangeDirection );
				}
			}
            break;
		default:
			{
			break;
			}
		}	
    if ( iContainer )
        {
        iContainer->RdsViewer().Reset();
        }

    if ( iContainer )
        {
        iContainer->ShowRTPlusInteractionIndicator( EFalse, ETrue);
        }
    // force update for toolbar extension view
    iToolbar->ToolbarExtension()->SetShown( EFalse );
    }

// --------------------------------------------------------------------------------
// CFMRadioMainView::PrepareToolbar
// Prepare toolbar for displaying basic radio command buttons
// --------------------------------------------------------------------------------
//
void CFMRadioMainView::PrepareToolbar()
	{
	// get toolbar from view
   	iToolbar = Toolbar();
   	
   	// Set observer
    iToolbar->SetToolbarObserver( this );

   	CAknButton* leftButton = NULL;
 	CAknButton* rightButton = NULL;	
   	leftButton = static_cast<CAknButton*>( iToolbar->ControlOrNull( EFMRadioToolbarButtonCmdTuneDown ) );
    rightButton = static_cast<CAknButton*>( iToolbar->ControlOrNull( EFMRadioToolbarButtonCmdTuneUp) );
    
    //set delay after buttons report long key press	
   	if ( leftButton )
   		{
    	leftButton->SetLongPressInterval( KLongPressDelayValueInMicroseconds );
    	}
	if ( rightButton )
		{
		rightButton->SetLongPressInterval( KLongPressDelayValueInMicroseconds );
		}		          	
	}

// --------------------------------------------------------------------------------
// CFMRadioMainView::OfferToolbarEventL
// Handle commands from toolbar.
// --------------------------------------------------------------------------------
//
void CFMRadioMainView::OfferToolbarEventL( TInt aCommandId )
    {
    // Event modifiers are valid only during CAknToolbar::OfferToolbarEventL
    TBool isHandled = EFalse;
    if ( iToolbar->EventModifiers() == CAknToolbar::ELongPress )
        {
        switch( aCommandId )
            {
            case EFMRadioToolbarButtonCmdTuneDown:
                {
                isHandled = ETrue;
                AppUi()->HandleCommandL( EFMRadioCmdSeekDown );
                break;
                }
            case EFMRadioToolbarButtonCmdTuneUp:
                {
                isHandled = ETrue;
                AppUi()->HandleCommandL( EFMRadioCmdSeekUp );
                break;
                }
            default:
                {
                // fall through.
                // dont break here because extension list buttons
                // seem to report longpress also after the tuning buttons
                // are pressed long.
                }
            }
        }
        
    if ( iToolbar->EventModifiers() != CAknToolbar::ELongPress && 
        iToolbar->EventModifiers() != CAknToolbar::ELongPressEnded )
        {
        switch( aCommandId )
            {
            case EFMRadioToolbarButtonCmdTuneDown:
                {
                isHandled = ETrue;
                AppUi()->HandleCommandL( EFMRadioCmdPrevChannel );
                break;
                }
            case EFMRadioToolbarButtonCmdTuneUp:
                {
                isHandled = ETrue;
                AppUi()->HandleCommandL( EFMRadioCmdNextChannel );
                break;
                }
            default:
                {
                // fall through.
                // dont break here because extension list buttons
                // seem to report longpress also after the tuning buttons
                // are pressed long.
                }
            }
        }
    if ( !isHandled )
        {
        switch( aCommandId )
            {
            case EFMRadioToolbarButtonCmdNokiaMusicStore:
                {
                HandleCommandL( EFMRadioMusicStoreNokiaMusicShop );
                break;
                }
            case EFMRadioToolbarButtonCmdOperatorMusicStore:
                {
                HandleCommandL( EFMRadioMusicStoreOperator );
                break;
                }
            case EFMRadioCmdRename:
                {
                iToolbar->ToolbarExtension()->SetShown( EFalse );
                HandleCommandL( aCommandId );
                break;
                }
            case EFMRadioCmdErase:
                {
                iToolbar->ToolbarExtension()->SetShown( EFalse );
                HandleCommandL( aCommandId );
                break;
                }
            case EFMRadioCmdChannelList:
                {
                iToolbar->ToolbarExtension()->SetShown( EFalse );
                HandleCommandL( aCommandId );
                break;
                }
            default:
                {
                HandleCommandL( aCommandId );
                break;
                }
            }
        }
    }

// --------------------------------------------------------------------------------
// CFMRadioMainView::DynInitToolbarL
// from MAknToolbarObserver
// --------------------------------------------------------------------------------
//
void CFMRadioMainView::DynInitToolbarL( TInt /*aResourceId*/, CAknToolbar* aToolbar )
    {
    if ( iToolbar == aToolbar )
        {
        CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( iCoeEnv->AppUi() );
        CAknToolbarExtension* toolbarExtension = iToolbar->ToolbarExtension();
        
        if ( appUi->MatchingChannelL( iRadioEngine->GetTunedFrequency() ) == KErrNotFound )
            {
            // current frequency not saved
            // show save
            toolbarExtension->HideItemL( EFMRadioCmdSaveChannel, EFalse );
            // hide rename
            toolbarExtension->HideItemL( EFMRadioCmdRename, ETrue );
            // dim delete
            toolbarExtension->SetItemDimmed( EFMRadioCmdErase, ETrue );
            }
        else
            {
            // show rename
            toolbarExtension->HideItemL( EFMRadioCmdRename, EFalse );
            // hide save
            toolbarExtension->HideItemL( EFMRadioCmdSaveChannel, ETrue );
            // undim delete
            toolbarExtension->SetItemDimmed( EFMRadioCmdErase, EFalse );
            }
        
        if ( IsWebLinkAvailable() )
            {
            // show web button
            toolbarExtension->HideItemL( EMPXPbvCmdInternetGoToWeb, EFalse );
            }
        else
            {
            // hide web button
            toolbarExtension->HideItemL( EMPXPbvCmdInternetGoToWeb, ETrue );
            }
        
        TBool musicStoreEnabled = iMusicStoreHandler->IsMusicStoreEnabled();
        
        if ( iMusicStoreHandler->OperatorMusicStoreAvailable() && musicStoreEnabled )
            {
            // show operator store
            toolbarExtension->HideItemL( EFMRadioToolbarButtonCmdOperatorMusicStore, EFalse );
            // hide speaker button
            toolbarExtension->HideItemL( EFMRadioToolbarButtonCmdToggleIhf, ETrue );
            }
        else
            {
            // hide operator store
            toolbarExtension->HideItemL( EFMRadioToolbarButtonCmdOperatorMusicStore, ETrue );
            // show and update speaker button
            UpdateToolbarSpeakerButtonStatus();
            toolbarExtension->HideItemL( EFMRadioToolbarButtonCmdToggleIhf, EFalse );
            }
        
        if ( iMusicStoreHandler->NokiaMusicStoreAvailable() && musicStoreEnabled )
            {
            // show Nokia Music Store
            toolbarExtension->HideItemL( EFMRadioToolbarButtonCmdNokiaMusicStore, EFalse );
            }
        else
            {
            // hide Nokia Music Store
            toolbarExtension->HideItemL( EFMRadioToolbarButtonCmdNokiaMusicStore, ETrue );
            }
        }
    }

// --------------------------------------------------------------------------------
// CFMRadioMainView::SetToolbarDimmedState
// Set the current state of the buttons in the button group
// --------------------------------------------------------------------------------
//
void CFMRadioMainView::SetToolbarDimmedState( TBool aState )
    {	
    iToolbar->SetDimmed( aState );
    iToolbar->DrawDeferred();
    }

// ---------------------------------------------------------------------------
// CFMRadioMainView::HandleSaveChannelQueryL
// show query if the current channel is not saved
// ---------------------------------------------------------------------------
//
void CFMRadioMainView::HandleSaveChannelQueryL()
    {
    CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*>( iCoeEnv->AppUi() );
    TInt channelIndex = appUi->MatchingChannelL( iRadioEngine->GetTunedFrequency() );
    
    if ( KErrNotFound == channelIndex )
        {
        TBool accepted = EFalse; // Operation accepted or discarded
        CAknQueryDialog* saveQuery = CAknQueryDialog::NewL();
        accepted = saveQuery->ExecuteLD( R_FMRADIO_QUERY_SAVE_CURRENT );
        
        if ( accepted )
            {
            AppUi()->HandleCommandL( EFMRadioCmdSaveChannel );
            }
        }
    }

// --------------------------------------------------------------------------------
// CFMRadioMainView::SetEmphasis
// From MEikMenuObserver. 
// --------------------------------------------------------------------------------
//
void CFMRadioMainView::SetEmphasis( CCoeControl* aMenuControl, TBool aEmphasis )
    {
    CAknView::SetEmphasis( aMenuControl, aEmphasis );

    if ( iContainer )
        {
        if ( aEmphasis ) // options menu opened
            {
            iOptionsMenuOpen = ETrue;
            // deactivate controls to help improve UI performance
            iContainer->IdleController().DeactivateControls();
            }
        else
            {
            if ( iOptionsMenuOpen )
                {
                // menu has been been really opened so update status
                iOptionsMenuOpen = EFalse;
                DetermineActiveMediaIdleComponent();
                }
            }
        }
    }

// --------------------------------------------------------------------------------
// CFMRadioMainView::LogoDisplayTimeCompleted
// from MFMRadioAlfLogoObserver 
// --------------------------------------------------------------------------------
//
void CFMRadioMainView::LogoDisplayTimeCompleted()
    {
    DetermineActiveMediaIdleComponent();
    }

// --------------------------------------------------------------------------------
// CFMRadioMainView::DetermineActiveMediaIdleComponent
// --------------------------------------------------------------------------------
//
void CFMRadioMainView::DetermineActiveMediaIdleComponent()
    {
    CFMRadioAppUi* appUi = static_cast<CFMRadioAppUi*> (AppUi());
    TBool wizardHandled = appUi->IsStartupWizardHandled();

     if ( iContainer &&
          !iOptionsMenuOpen &&
          wizardHandled &&
          IsForeground() )
        {
        
        if ( iContainer->RdsViewer().IsRdsTextVisibilityTimerCompleted() )
            {
            // same text displayed for a while, ok to start idle
            iContainer->IdleController().ShowControl( iContainer->MediaIdle() );
            }
        else
            {
            // display the rds text until the NotifyRdsTextTimerCompletion() callback
            iContainer->IdleController().ShowControl( iContainer->RdsViewer() );
            }
        }
    }

// --------------------------------------------------------------------------------
// CFMRadioMainView::IsWebLinkAvailabe
// --------------------------------------------------------------------------------
//
TBool CFMRadioMainView::IsWebLinkAvailable()
    {
    TBool response = EFalse;
    CFMRadioRdsReceiverBase& receiver = iRadioEngine->RdsReceiver();
    if ( receiver.RtPlusProgramUrl().Length() && 
         iRadioEngine->GetRTPlusSupportLevel() == EFMRadioAllInteractions )
        {
        response = ETrue;
        }
    return response;
    }

// --------------------------------------------------------------------------------
// CFMRadioMainView::UpdateToolbarSpeakerButtonStatus
// --------------------------------------------------------------------------------
//
void CFMRadioMainView::UpdateToolbarSpeakerButtonStatus()
    {
    if ( iToolbar && iContainer )
        {
        CAknToolbarExtension* toolbarExtension = iToolbar->ToolbarExtension();
        
        if ( iRadioEngine->IsAudioRoutingPossible() )
            {
            toolbarExtension->SetItemDimmed( EFMRadioToolbarButtonCmdToggleIhf, EFalse );
            CAknButton* speakerButton = NULL;
            
            speakerButton = static_cast<CAknButton*>( toolbarExtension->ControlOrNull( EFMRadioToolbarButtonCmdToggleIhf ) );
            
            if ( speakerButton )
                {
                if ( iRadioEngine->GetAudioOutput() == CRadioEngine::EFMRadioOutputIHF )
                    {
                    speakerButton->SetCurrentState( 1, ETrue );
                    }
                else
                    {
                    speakerButton->SetCurrentState( 0, ETrue );
                    }
                }
            }
        else
            {
            toolbarExtension->SetItemDimmed( EFMRadioToolbarButtonCmdToggleIhf, ETrue );
            }
        }
    }

// --------------------------------------------------------------------------------
// CFMRadioMainView::ShowDefaultLogo
// --------------------------------------------------------------------------------
//
void CFMRadioMainView::ShowDefaultLogo()
    {
    // show default radio logo
    if ( iContainer )
        {
        iContainer->IdleController().ShowControl( iContainer->Logo() );
        }
    }

// End of File
