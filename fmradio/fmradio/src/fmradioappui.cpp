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
* Description:  FM Radio App UI implementation
*
*/

// INCLUDE FILES

#include <featmgr.h>
#if defined  __SERIES60_HELP || defined FF_S60_HELPS_IN_USE
#include <hlplch.h>
#include "radio.hlp.hrh"
#endif
#include <akntoolbar.h>
#include <StringLoader.h>
#include <eikmenup.h>
#include <avkon.hrh>
#include <aknlistquerydialog.h> 
#include <AknQueryDialog.h>
#include <AknGlobalNote.h>
#include <fmradio.rsg>
#include <fmradiocommandlineparams.h>
#include <alf/alfenv.h>
#include <AknVolumePopup.h>
#include <fmradiointernalpskeys.h>
#include <fmradiouids.h> 
#include <iaupdate.h>
#include <iaupdateparameters.h>
#include <iaupdateresult.h>
#include <e32property.h>
#include <apgwgnam.h>
#include <centralrepository.h>
#include <settingsinternalcrkeys.h>
#include <activeidle2domainpskeys.h>
#include <akntoolbarextension.h>

#include "fmradiobacksteppingservicewrapper.h"
#include "fmradiodocument.h"
#include "fmradioengine.h"
#include "fmradiovariant.hrh"
#include "fmradiomainview.h"
#include "fmradiochannellistview.h"
#include "fmradiocontroleventobserverimpl.h"
#include "fmradioscanlocalstationsview.h"
#include "fmradio.hrh"
#include "fmradioengine.hrh"
#include "fmradioappui.h"
#include "fmradiordsreceiver.h"
#include "fmradioapp.h"
#include "fmradiopubsub.h"
#include "debug.h"

// Application Uid for Active Idle app
#ifdef __ACTIVE_IDLE
const TUid KFMRadioUidIdleApp = { 0x101FD64C };
#else
const TUid KFMRadioUidIdleApp = KPSUidActiveIdle2; // use this one instead because the "aisystemuids.hrh" is not export
#endif
// CONSTANTS 

// ---------------------------------------------------------------------------------
// CFMInformationNote::CFMInformationNote
// ---------------------------------------------------------------------------------
CFMInformationNote::CFMInformationNote( MInformationNoteInterface& aObserver ) :
    CAknInformationNote ( EFalse ),
    iDialogObserver( aObserver )
    {
    
    }
// ---------------------------------------------------------------------------------
// CFMInformationNote::~CFMInformationNote
// ---------------------------------------------------------------------------------
//
CFMInformationNote::~CFMInformationNote()
    {
    iDialogObserver.DialogTerminated();
    }
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------------
//
CFMRadioAppUi::CFMRadioAppUi() :
    iStartUp( ETrue ),
    iStartupWizardHandled( EFalse ),
    iStartupWizardRunning( EFalse ),
    iTuneFromWizardActivated( EFalse ),
    iInfoNoteOn( EFalse ),
    iPendingViewId( KNullUid ),
    iRegionChanged( EFalse ),
    iRadioInitializedWithFrequency( EFalse )
    {
    }

// ---------------------------------------------------------------------------------
// CFMRadioAppUi::ConstructL
// 2nd phase constructor. Instanciates all member objects
// ---------------------------------------------------------------------------------
//
void CFMRadioAppUi::ConstructL()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::ConstructL()") ) );
    
    BaseConstructL( EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible );
    
    FeatureManager::InitializeLibL();
    iFeatureManagerInitialized = ETrue;

    // initialise local variation key with all features disabled.
    iFMRadioVariationFlags = 0;
    iAudioResourceAvailable = ETrue;
    iAlreadyClean = EFalse;

    iRadioEngine = CRadioEngine::NewL( *this ); // Construct a radio engine object
    
    // Fill channels array from engine
    GetChannelsArrayL();
    
    if ( iRadioEngine->IsInCall() ) 
        {
        iCurrentRadioState = EFMRadioStateOffForPhoneCall;
        }
    
    iBsWrapper = CFMRadioBackSteppingServiceWrapper::NewL( TUid::Uid( KUidFMRadioApplication ) );
    AddViewActivationObserverL( iBsWrapper );    

    iMainView = CFMRadioMainView::NewL( iRadioEngine, *iAlfEnv, *this  );
    AddViewL( iMainView );      // transfer ownership to CAknViewAppUi
    iChannelListView = CFMRadioChannelListView::NewL( iRadioEngine, *this );
    AddViewL( iChannelListView );     // transfer ownership to CAknViewAppUi
    iScanLocalStationsView = CFMRadioScanLocalStationsView::NewL( *iRadioEngine, *this );
    AddViewL( iScanLocalStationsView ); // transfer ownership to CAknViewAppUi
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::SecondaryConstructL()
// All leaving function calls are made here, so that nothing is left on the stack if the ConstructL leaves
// The reason for this behaviour is that the UI framework doesn't handle leaves from the AppUi correctly.
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::SecondaryConstructL()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::SecondaryConstructL()") ) );
    
    iLayoutChangeObserver = iMainView;

    iRadioEngine->PubSubL().PublishPresetCountL( iChannels.Count() );
    
    // Create vertical volume popup for speaker and headset
    iIhfVolumePopupControl = CAknVolumePopup::NewL( NULL, ETrue );
    iIhfVolumePopupControl->SetRange( KFMRadioMinVolumeLevel, KFMRadioMaxVolumeLevel );
    iIhfVolumePopupControl->SetStepSize( KFMRadioVolumeStepSize );
    // Set observer for control events
    iIhfVolumePopupControl->SetObserver( this );
    
    iHeadsetVolumePopupControl = CAknVolumePopup::NewL( NULL, ETrue );
    iHeadsetVolumePopupControl->SetRange( KFMRadioMinVolumeLevel, KFMRadioMaxVolumeLevel );
    iHeadsetVolumePopupControl->SetStepSize( KFMRadioVolumeStepSize );
    iHeadsetVolumePopupControl->SetObserver( this );
    
    iActiveVolumePopupControl = iHeadsetVolumePopupControl; //initialize it to Headset

    // Create and set our observer for control events.
    iControlEventObserver = CFMRadioControlEventObserverImpl::NewL( *this );
    iRadioEngine->PubSubL().SetControlEventObserver( iControlEventObserver );

    // For monitoring side volume key events
    iSvkEvents = CFMRadioSvkEvents::NewL(*this);

    iRadioEngine->PubSubL().PublishApplicationRunningStateL( EFMRadioPSApplicationRunning );

    UpdateLandscapeInformation();	

    // Create alfred environment
    iAlfEnv = CAlfEnv::NewL();

    // Create alfred display
    TRect rect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);	
    if( !IsLandscapeOrientation() )
        {
        TRect toolBarRect = iMainView->Toolbar()->Rect();
        rect.SetHeight( rect.Height() - toolBarRect.Height() );	
        }
    iAlfEnv->NewDisplayL( rect, CAlfEnv::ENewDisplayAsCoeControl );
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::HandleTunerReadyCallback
// Tuner is initialized and ready.
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::HandleTunerReadyCallback()
    {
    iRadioEngine->InitializeRadio();
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::~CFMRadioAppUi
// Destructor
// Frees reserved resources
// ---------------------------------------------------------------------------
//
CFMRadioAppUi::~CFMRadioAppUi()
    {
    if ( iRadioEngine )
        {
        TRAP_IGNORE(
            iRadioEngine->PubSubL().PublishApplicationRunningStateL( EFMRadioPSApplicationClosing ) )
        }

    FTRACE( FPrint( _L("CFMRadioAppUi::~CFMRadioAppUi()") ) );
    if ( iAlreadyClean == EFalse )
        {
        Cleanup();
        }

    if ( iFeatureManagerInitialized )
        {
        FeatureManager::UnInitializeLib();
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::Cleanup()
// Perform object cleanup. This would normally be performed in the destructor,
// but since it can also be called from SecondaryConstructL it was moved into
// a seperate function.
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::Cleanup()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::Cleanup()") ) );

    delete iIhfVolumePopupControl;
    iIhfVolumePopupControl = NULL;
    delete iHeadsetVolumePopupControl;
    iHeadsetVolumePopupControl = NULL;

    if (iRadioEngine)
        {
        delete iRadioEngine;
        iRadioEngine = NULL;
        }
    if (iSvkEvents)
        {
        delete iSvkEvents;
        }
    if (iGlobalOfflineQuery)
        {
        delete iGlobalOfflineQuery;
        }
    if (iLocalActivateOfflineQuery)
        {
        delete iLocalActivateOfflineQuery;
        }
    if (iLocalContinueOfflineQuery)
        {
        delete iLocalContinueOfflineQuery;
        }
    delete iControlEventObserver;
    delete iAlfEnv;
    
    iAlreadyClean = ETrue;
    
    if ( iConnectHeadsetGlobalNote )
        {
        delete iConnectHeadsetGlobalNote;
        iConnectHeadsetGlobalNote = NULL;
        }
    if ( iConnectHeadsetQuery )
        {
        delete iConnectHeadsetQuery;
        iConnectHeadsetQuery = NULL;
        }

     iChannels.ResetAndDestroy();
     iChannels.Close();
        
    if ( iUpdate )
        {
        delete iUpdate;
        iUpdate = NULL;
        }
    if ( iParameters )
        {
        delete iParameters;
        iParameters = NULL;
        }
    if ( iSettingsRepository )
        {
        delete iSettingsRepository;
        iSettingsRepository = NULL;
        }
    if ( iBsWrapper )
        {
        RemoveViewActivationObserver( iBsWrapper );
        }
    delete iBsWrapper;
    iBsWrapper = NULL;
    
    delete iStartupForegroundCallback;
    iStartupForegroundCallback = NULL;
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::HandleCommandL
// Interprets view's menu-,softkey and other commands and acts
// accordingly by calling the appropriate command handler
// function for further action.
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::HandleCommandL( TInt aCommand )
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::HandleCommandL state=%d command=%d "), iCurrentRadioState, aCommand) );
    if ( iCurrentRadioState == EFMRadioStateOn )
        {
        switch ( aCommand )
            {
            case EFMRadioCmdChannelList:
                ActivateLocalViewL( iChannelListView->Id() );
                break;
            case EFMRadioCmdSeekUp:
                ScanUpL();
                break;
            case EFMRadioCmdSeekDown:
                ScanDownL();
                break;
            case EFMRadioCmdSaveChannel:
                SaveChannelToLastIntoListL();
                break;
            case EFMRadioCmdScanLocalStations:
                ActivateLocalViewL( iScanLocalStationsView->Id() );
                break;
            case EFMRadioCmdScanLocalStationsScan:
                // continue scanning
                StartLocalStationsSeekL();
                break;
            case EFMRadioCmdListenCh:
                if ( iChannels.Count() > 0 )
                    {
                    PlayChannel( iChannelListView->CurrentlySelectedChannel() );
                    }
                break;
            case EFMRadioCmdRename:
                RenameCurrentChannelL();
                break;
            case EFMRadioCmdActivateIhf:
                SetAudioOutput( CRadioEngine::EFMRadioOutputIHF );
                break;
            case EFMRadioCmdDeactivateIhf:
                SetAudioOutput( CRadioEngine::EFMRadioOutputHeadset );
                break;
            case EFMRadioCmdEnableRdsAfSearch:
                iRadioEngine->SetRdsAfSearchEnable( ETrue );
                break;
            case EFMRadioCmdDisableRdsAfSearch:
                iRadioEngine->SetRdsAfSearchEnable( EFalse );
                break;
            case EFMRadioCmdUpdateVolume:
                UpdateVolume( EDirectionNone );
                break;
            case EFMRadioCmdNextChannel:
                {
                TInt channelCount = iChannels.Count();
                if ( channelCount >= 1 )
                    {
                    iMainView->SetStationChangeType( EFMRadioStationChangeNext );
                    PlayChannel( iChannelListView->NextChannel() );
                    }
                else
                    {
                    if ( AknLayoutUtils::PenEnabled() )
                        {
                        DisplayInformationNoteL( R_QTN_FMRADIO_USE_LONG_TAP );
                        }
                    }
                break;
                }
            case EFMRadioCmdPrevChannel:
                {
                TInt channelCount = iChannels.Count();
                if ( channelCount >= 1 )
                    {
                    iMainView->SetStationChangeType( EFMRadioStationChangePrevious );
                    PlayChannel( iChannelListView->PreviousChannel() );
                    }
                else
                    {
                    if ( AknLayoutUtils::PenEnabled() )
                        {
                        DisplayInformationNoteL( R_QTN_FMRADIO_USE_LONG_TAP );
                        }
                    }
                break;
            	}
            case EFMRadioCmdMuteOn:
            	iRadioEngine->SetMuteOn( ETrue );
            	break;
            case EFMRadioCmdMuteOff:
            	iRadioEngine->SetMuteOn( EFalse );
            	break;
            case EFMRadioCmdMute:
            	HandleMuteCommand();
            	break;	
            case EAknCmdHelp:
            case EFMRadioCmdHelp:
                {
#if defined __SERIES60_HELP || defined FF_S60_HELPS_IN_USE
                if ( ActiveView() == KFMRadioMainViewId )
                    {
                    HlpLauncher::LaunchHelpApplicationL( iCoeEnv->WsSession(), GetCurrentHelpContextL() );
                    }
                else
                    {
                    CArrayFix<TCoeHelpContext>* buf = CCoeAppUi::AppHelpContextL();
                    HlpLauncher::LaunchHelpApplicationL( iCoeEnv->WsSession(), buf );
                    }
#endif
                }
                break;
            default:
                break;
            }
        }
    else if ( iCurrentRadioState == EFMRadioStateBusySeek ||
             iCurrentRadioState == EFMRadioStateBusyScanLocalStations )
        {
        switch ( aCommand )
            {
            case EFMRadioCmdStopTuning:
            case EAknSoftkeyCancel:
                CancelSeek();
                break;
            default:
                break;
            }
        }
    switch ( aCommand )
        {
        case EFMRadioCmdScanLocalStationsViewActive:
            iLayoutChangeObserver = iScanLocalStationsView;
            break;
        case EFMRadioCmdChannelListViewActive:
            iLayoutChangeObserver = iChannelListView;
            break;
        case EFMRadioCmdMainViewActive:
            iLayoutChangeObserver = iMainView;
            break;
        case EAknSoftkeyExit:
        case EEikCmdExit:
        case EAknCmdExit:
            Exit();
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::ActiveView
// ---------------------------------------------------------------------------
//
TUid CFMRadioAppUi::ActiveView() const
    {
    return iView ? iView->Id() : KNullUid;
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::RadioEngine
// ---------------------------------------------------------------------------
//
CRadioEngine* CFMRadioAppUi::RadioEngine()
    {
    return iRadioEngine;
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::Document
// ---------------------------------------------------------------------------
//
CFMRadioDocument* CFMRadioAppUi::Document() const
    {
    return static_cast<CFMRadioDocument*>( CEikAppUi::Document() );
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::BackSteppingWrapper
// ---------------------------------------------------------------------------
//
CFMRadioBackSteppingServiceWrapper& CFMRadioAppUi::BackSteppingWrapper() const
    {
    return *iBsWrapper;
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::TurnRadioOn
// Turn the fm radio hardware on
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::TurnRadioOn()
    {
    FTRACE(FPrint(_L("CFMRadioAppUi::TurnRadioOn()")));
    if ( !iRadioEngine->IsRadioOn() )
        {
        // Force the mute state only if it wasn't already forced
        if ( iMuteStatusBeforeRadioInit == EFMUninitialized )
            {
            if ( iRadioEngine->IsMuteOn() )
                {
                FTRACE(FPrint(_L("CFMRadioAppUi::iMuteStatusBeforeRadioInit = ETrue;()")));
                iMuteStatusBeforeRadioInit = EFMMuted;	
                }
            else
                {
                FTRACE(FPrint(_L("CFMRadioAppUi::iMuteStatusBeforeRadioInit = EFalse;()")));
                iMuteStatusBeforeRadioInit = EFMUnmuted;
                }
            }
        iCurrentRadioState = EFMRadioStateBusyRadioOn;
        // use mute here so we have no audio until tune event
        iRadioEngine->SetMuteOn( ETrue );
        iRadioEngine->RadioOn();
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::PlayChannel
// Tune the radio hardware to the frequency saved at the specified channel
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::PlayChannel( TInt aIndex )
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::PlayChannel(%d)"), aIndex ) );
    if ( aIndex >= KMinNumberOfChannelListItems &&
         aIndex < KMaxNumberOfChannelListItems )
        {
        // Activate the channel by index
        TRAP_IGNORE( iRadioEngine->TunePresetL( aIndex ) )
        iMainView->PrepareViewForChannelChange();

        // Update channel list
        iChannelListView->SetNowPlayingChannel( aIndex );
        iChannelListView->SetLastListenedChannel( aIndex );
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::EraseCurrentChannelL
// Delete channel from the channel list
// ---------------------------------------------------------------------------
//
TBool CFMRadioAppUi::EraseChannelL( TInt aIndex )
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::EraseCurrentChannelL()") ) );

    TBool accepted = ConfirmChannelListDeleteL( aIndex );
    
    if ( accepted )
        {        
        UpdateChannelsL( EDeleteChannel, aIndex, 0 ); 
        UpdateChannelsL( EStoreAllToRepository, 0, 0 );
        }
    return accepted;
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::RenameCurrentChannelL
// Rename the currently selected channel
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::RenameCurrentChannelL()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::RenameCurrentChannelL()") ) );
    
    TInt channelIndex = KErrNotFound;
    if ( ActiveView() == KFMRadioChannelListViewId )
        {
        channelIndex = iChannelListView->CurrentlySelectedChannel();
        }
    else
        {
        channelIndex = iRadioEngine->GetPresetIndex();
        }
    // Display the text query
    CRadioEngine::TStationName channelName = iChannels[ channelIndex ]->PresetName();
   
    CAknTextQueryDialog* dlg = new (ELeave) CAknTextQueryDialog( channelName, CAknQueryDialog::ENoTone );
    
    if ( dlg->ExecuteLD( R_FMRADIO_RENAME_QUERY ) )
        {
        FTRACE(FPrint(_L("CFMRadioAppUi::RenameCurrentChannelL()")));
        
        iChannels[channelIndex]->SetPresetNameL( channelName );
        
        iChannelListView->UpdateChannelListContentL( channelIndex,
                        iChannels[ channelIndex ]->PresetName(),
                        iChannels[ channelIndex ]->PresetFrequency() );
        
        UpdateChannelsL( EStoreIndexToRepository, channelIndex, 0 );
        
        iMainView->SetStationChangeType( EFMRadioStationChangeNone );
        // A channel can also be renamed from the main view
        iMainView->DisplayChannelL( channelIndex );
        // show default logo for a while
        // so that animation is stopped and it has the right channel name
        // after restart
        iMainView->ShowDefaultLogo();
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::SaveChannelToLastIntoListL
// Display listbox menu to allow user to specify a channel
// to save currently tuned frequency to.
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::SaveChannelToLastIntoListL()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::SaveChannelToLastIntoListL()") ) );
    SaveChannelL( iChannels.Count() + 1 );
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::SaveChannel
// Save currently tuned frequency to the currently selected channel
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::SaveChannelL( TInt aIndex )
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::SaveChannelL(%d)"), aIndex ) );

    TBool continueWithSave = ETrue;
    // check if channel list is full
    if ( aIndex == KMaxNumberOfChannelListItems + 1 )
        {
        CAknQueryDialog* query = CAknQueryDialog::NewL();
        if ( query->ExecuteLD( R_FMRADIO_REPLACE_EXISTING_CHANNELS_QUERY ) )
            {
            UpdateChannelsL( ERemoveAllFromRepository, 0, 0  );
            }
        else
            {
            continueWithSave = EFalse;
            }
        }

    if ( continueWithSave )
        {
        CFMRadioRdsReceiverBase& receiver = iRadioEngine->RdsReceiver();
        CFMRadioRdsReceiverBase::TFMRadioProgrammeSeviceType type = 
                receiver.ProgrammeServiceNameType();
        const TDesC& channelName = type == CFMRadioRdsReceiverBase::EFMRadioPSNameStatic ?
                receiver.ProgrammeService() : KNullDesC;
        
        CFMRadioPreset* preset = CFMRadioPreset::NewL();
        CleanupStack::PushL( preset );
        preset->SetPresetNameL( channelName );
        preset->SetPresetFrequency( iRadioEngine->GetTunedFrequency() );
                
        const TDesC& webUrl = receiver.RtPlusProgramUrl();
        if ( webUrl.Length() )
            {
            preset->SetPresetUrlL( webUrl );
            }        
        iChannels.AppendL( preset );
        CleanupStack::Pop( preset ); 
        
        UpdateChannelsL( EStoreAllToRepository, 0, 0 );
        
        iMainView->SetStationChangeType( EFMRadioStationChangeNone );
        PlayChannel( iChannels.Count() - 1 );
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::ConfirmChannelListDeleteL
// Display confirmation dialog for channel deletion
// ---------------------------------------------------------------------------
//
TBool CFMRadioAppUi::ConfirmChannelListDeleteL( TInt aIndex )
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::ConfirmChannelListActionL( %d )"), aIndex ) );
    TInt accepted = EFalse; // Operation accepted or discarded
    TInt stringResourceId = 0; // Resource id of the prompt text
    TInt queryResourceId = R_FMRADIO_ERASE_CH_CONFIRMATION_QUERY;  // Resource id of the used query dialog

    RBuf channelData;
    channelData.CleanupClosePushL();
    
    if ( iChannels[ aIndex ]->PresetName().Length() )
        {
        stringResourceId = R_QTN_FMRADIO_QUERY_DELETE;
        channelData.CreateL( iChannels[ aIndex ]->PresetName() );
        }
    else
        {
        stringResourceId = R_QTN_FMRADIO_QUERY_DELETE_MHZ;
        TInt maxDecimalPlaces = iRadioEngine->DecimalCount();
        TInt channelfreq = iChannels[ aIndex ]->PresetFrequency();
        TReal realFrequency = static_cast<TReal>( channelfreq / static_cast<TReal>( KHzConversionFactor ));
        
        channelData.CreateL( KFrequencyMaxLength );
        
        TRealFormat format( KFrequencyMaxLength, maxDecimalPlaces );
        channelData.Num( realFrequency, format );
        AknTextUtils::LanguageSpecificNumberConversion( channelData );
        }
    // Excecute a confirmation query with string and query resource id
    HBufC* prompt = StringLoader::LoadLC( stringResourceId, channelData, iCoeEnv );
    CAknQueryDialog* queryDlg = CAknQueryDialog::NewL();
    accepted = queryDlg->ExecuteLD( queryResourceId, *prompt );
    CleanupStack::PopAndDestroy( prompt );
    CleanupStack::PopAndDestroy( &channelData );
    return accepted;
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::ScanUpL
// 
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::ScanUpL()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::ScanUpL()") ) );
    // Frequency is always 0 when asking engine for automatical tuning. Frequency
    // parameter is then ignored by engine when automatical tuning (seek) is requested.
    iCurrentRadioState = EFMRadioStateBusySeek;

    if ( iMainView->IsForeground() )
        {
        iMainView->SetStationChangeType( EFMRadioStationChangeScanUp );
        iMainView->SeekL(); // Show seek wait note
        }
    iRadioEngine->ScanUp();
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::ScanDownL
// 
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::ScanDownL()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::ScanDownL()") ) );
    // Frequency is always 0 when asking engine for automatical tuning. Frequency
    // parameter is then ignored by engine when automatical tuning (seek) is requested.
    iCurrentRadioState = EFMRadioStateBusySeek;
    
    if ( iMainView->IsForeground() )
        {
        iMainView->SetStationChangeType( EFMRadioStationChangeScanDown );
        iMainView->SeekL(); // Show seek wait note
        }
    iRadioEngine->ScanDown();
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::TuneL
// 
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::TuneL( TInt aFrequency )
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::TuneL(%d)"), aFrequency ) );
    // Frequency is always 0 when asking engine for automatical tuning. Frequency
    // parameter is then ignored by engine when automatical tuning (seek) is requested.
    iCurrentRadioState = EFMRadioStateBusySeek;
    if (iMainView->IsForeground())
        {
        iMainView->SeekL(); // Show seek wait note
        }
    iRadioEngine->Tune(aFrequency);
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::UpdateVolume
// Update the radio volume
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::UpdateVolume( CFMRadioAppUi::TFMRadioDirections aDirection )
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::UpdateVolume(%d)"), UpdateVolume ) );

    if ( !iRadioEngine->IsInCall() )
        {
        TInt volumeControlLevel = iRadioEngine->GetVolume();

        TInt newVol = 0;
        if ( aDirection == EDirectionDown )
            {
            TRAP_IGNORE( ShowVolumePopupL(); )
            newVol = volumeControlLevel - 1;
            
            if ( newVol > KFMRadioMinVolumeLevel )
                {
                iRadioEngine->SetVolume( newVol );
                }
            else
                {
                iRadioEngine->SetMuteOn( ETrue );
                iRadioEngine->SetVolume( newVol );
                iActiveVolumePopupControl->SetValue( KFMRadioMinVolumeLevel );
                }
            }
        else if ( aDirection == EDirectionUp )
            {
            TRAP_IGNORE( ShowVolumePopupL(); )
            if ( iRadioEngine->IsMuteOn() )
                {
                iRadioEngine->SetMuteOn( EFalse );
                }
            newVol = volumeControlLevel + 1;
            
            if ( newVol <= KFMRadioMaxVolumeLevel )
                {
                iRadioEngine->SetVolume( newVol );
                }
            else
                {
                iActiveVolumePopupControl->SetValue( KFMRadioMaxVolumeLevel );
                }
            }
        else if ( aDirection == EDirectionNone )
            {
            //we just wish to update the UI's volume control
            HandleVolumeChangedCallback();
            }
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::SetAudioOutput
// Set the audio output of the radio.
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::SetAudioOutput(
    CRadioEngine::TFMRadioAudioOutput aAudioOutput )
    {
    iRadioEngine->SetAudioOutput( aAudioOutput );
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::CancelSeek
// Cancel outstanding seek request.
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::CancelSeek()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::CancelSeek()") ) );
    if ( iCurrentRadioState == EFMRadioStateBusySeek )
        {
        iRadioEngine->CancelScan(); // Seek request to engine
        }
    HandleStopSeekCallback();
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::DisplayErrorNoteL
// Displays an error note with the text contained in the passed in reference
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::DisplayErrorNoteL( TInt aErrorNote )
    {
    // Show headset missing note
    CAknErrorNote* errorNote = new ( ELeave ) CAknErrorNote( ETrue );
    HBufC* noteText = StringLoader::LoadLC( aErrorNote, iCoeEnv );
    errorNote->ExecuteLD( *noteText );
    CleanupStack::PopAndDestroy( noteText );
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::DisplayInformationNoteL
// Displays an information note
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::DisplayInformationNoteL( TInt aInfoNote )
    {
    if ( !iInfoNoteOn && IsForeground() )
        {
        CFMInformationNote* infonote = new ( ELeave ) CFMInformationNote( *this );
        HBufC* noteText = StringLoader::LoadLC( aInfoNote, iCoeEnv );
        iInfoNoteOn = ETrue;
        infonote->ExecuteLD( *noteText );
        CleanupStack::PopAndDestroy( noteText );
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::ExitApplication
// Shutdown the application.
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::ExitApplication()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::ExitApplication() - Start") ) );
    //this method is called twice. First will trigger the RadioOff command
    if ( iCurrentRadioState == EFMRadioStateOff ||
         iCurrentRadioState == EFMRadioStateOffForPhoneCall ||
         iCurrentRadioState == EFMRadioStateOffBeforePhoneCall )
        {
        //once we receive a radio off complete event we finish shutting down.
        FTRACE( FPrint( _L("CFMRadioAppUi::ExitApplication() - calling exit") ) );
        Exit();
        }
    else
        {
        if ( iCurrentRadioState != EFMRadioStateExiting )
            {
            iCurrentRadioState = EFMRadioStateExiting;
            FTRACE( FPrint( _L("CFMRadioAppUi::ExitApplication() - turning off, and starting timer") ) );
            iRadioEngine->RadioOff(); // Shutdown radio
            }
        }
    }

// ---------------------------------------------------------------------------
// From class CCoeAppUi
// CFMRadioAppUi::HandleForegroundEventL
// Application has gone to foreground/background.
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::HandleForegroundEventL( TBool aForeground )
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::HandleForegroundEventL(%d)"), aForeground ) );
    CAknViewAppUi::HandleForegroundEventL( aForeground );
    
    if ( aForeground )
        {
        if ( iStartUp )
            {
            iStartUp = EFalse;
            // Rest of startup processing may show a waiting dialog. It cannot be 
            // done while processing a foreground event. So a call back is used.
            iStartupForegroundCallback = new( ELeave ) CAsyncCallBack( 
                TCallBack( StaticStartupForegroundCallback, this ), CActive::EPriorityStandard );
            iStartupForegroundCallback->CallBack();
            }
        else
            {
            // Restores all resources that were released with a call to Release().
            // After this the Hitchcock user interface should be in the same state
            // in terms of resources as it was prior to the Release() call.
            iAlfEnv->RestoreL();
            
            // Check if offline profile is activated after/when playing e.g. music player.
            // In that case we didn't show the offline query as global, so we need
            // to show it now.
            if ( IsOfflineProfileActivatedWhenRadioAudioDisabled() )
                {
                // Do not re-show the offline query if user hasn't yet answered to it.
                if ( !iShowingLocalOfflineContinueQuery )
                    {
                    iRadioEngine->RadioOff();
                    TInt res(0);
                    iShowingLocalOfflineContinueQuery = ETrue;
                                                                                    
                    iOfflineQueryDialogActivated = ETrue;
                    
                    if ( !iLocalContinueOfflineQuery )
                        {
                        iLocalContinueOfflineQuery = CAknQueryDialog::NewL();
                        }
                                        
                    res = iLocalContinueOfflineQuery->ExecuteLD( R_FMRADIO_CONTINUE_IN_OFFLINE_QUERY );
                    
                    iLocalContinueOfflineQuery = NULL;
                    iOfflineQueryDialogActivated = EFalse;
                    iShowingLocalOfflineContinueQuery = EFalse;
                                                            
                    if ( res )
                        {
                        iOfflineProfileActivatedWhenRadioAudioDisabled = EFalse;
                        iRadioEngine->RadioOn();
                        HandlePendingViewActivationL();
                        }
                    else
                        {
                        Exit();
                        }
                    }
                }
            TryToResumeAudioL();
            }  
        }
    else 
        {
        // Releases as many resources of the Hitchcock as possible. 
        iAlfEnv->Release();
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::HandleRadioEngineCallBack
// Notification from Radio Engine informing the UI when requests have
// completed or certain events have occured that needs to be handled.
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::HandleRadioEngineCallBack(
    MRadioEngineStateChangeCallback::TFMRadioNotifyEvent aEventCode,
    TInt aErrorCode )
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::HandleRadioEngineCallBack - event %d error code: %d"), aEventCode, aErrorCode ) );
    TInt err = KErrNone;
    if ( aErrorCode == KErrNone )
        {
        switch ( aEventCode )
            {
            case EFMRadioEventTunerReady:
                HandleTunerReadyCallback();
                break;
            case EFMRadioEventRadioOn:
                {
                iCurrentRadioState = EFMRadioStateOn;
                TRAPD( err, HandleStartupWizardL() );
                iStartupWizardHandled = ETrue;
                const TUid activeViewTuid = ActiveView();
                
                if ( activeViewTuid == KFMRadioChannelListViewId )
                    {
                    CAknToolbar* toolbar = iChannelListView->Toolbar();
                    if ( toolbar )
                        {
                        iChannelListView->UpdateToolbar();
                        toolbar->DrawDeferred();
                        }
                    }
                if( !IsStartupWizardRunning() || err != KErrNone )
                    {
                    //we use method because the behaviour we require is the same.
                    if ( !iTuneFromWizardActivated && iMuteStatusBeforeRadioInit != EFMUninitialized )
                        {
                        TBool mute = EFalse; 
                        if ( iMuteStatusBeforeRadioInit == EFMMuted )
                            {
                            mute = ETrue;
                            }
                        iRadioEngine->SetMuteOn( mute );
                        iMuteStatusBeforeRadioInit = EFMUninitialized;
                        }
                    HandleStopSeekCallback(); 
                    }
                }
                break;
            case EFMRadioEventRadioOff:
                iCurrentRadioState = EFMRadioStateOff;
                
                if ( !iOfflineQueryDialogActivated )
                    {
                    ExitApplication();
                    }
                
                break;
            case EFMRadioEventFMRadioInitialized:
                TRAP( err, HandleInitializedCallbackL() );
                break;
            case EFMRadioEventTune:
                if ( iTuneFromWizardActivated )
                    {
                    iTuneFromWizardActivated = EFalse;
                    iRadioEngine->SetMuteOn( EFalse );
                    iMuteStatusBeforeRadioInit = EFMUninitialized;
                    }
                if ( IsStartupWizardHandled() )
                    {
                    HandleStopSeekCallback();
                    }
                break;
            case EFMRadioEventSetMuteState:
                HandleSetMuteStateCallback();
                break;
            case EFMRadioEventVolumeUpdated:
                HandleVolumeChangedCallback();
                break;
            case EFMRadioEventSetAudioOutput:
                HandleAudioOutputSetCallback();
                break;
            case EFMRadioEventButtonPressed:
                iMainView->SetStationChangeType( EFMRadioStationChangeNext );
                PlayChannel( iChannelListView->NextChannel() );
                break;
            case EFMRadioEventHeadsetDisconnected:
                HandleHeadsetDisconnectedCallback();
                break;
            case EFMRadioEventHeadsetReconnected:
                HandleHeadsetReconnectedCallback();
                break;
            case EFMRadioEventAudioResourceLost:
            	iAudioLost = ETrue;
                TRAP( err, HandleAudioResourceNotAvailableL(aErrorCode) );
                break;
            case EFMRadioEventAudioResourcePaused:
                TRAP( err, HandleAudioResourceNotAvailableL(aErrorCode) );
                break;                
            case EFMRadioEventAudioResourceAvailable:
                TRAP( err, HandleAudioResourceAvailableL() );
                break;
            case EFMRadioEventCallStarted:
                if ( iCurrentRadioState != EFMRadioStateOff && 
                     iCurrentRadioState != EFMRadioStateOffBeforePhoneCall )
                    {
                    iCurrentRadioState = EFMRadioStateOffForPhoneCall;
                    }
                else
                    {
                    iCurrentRadioState = EFMRadioStateOffBeforePhoneCall;
                    }
                FadeViewsAndShowExit( ETrue );
                break;
            case EFMRadioEventCallEnded:
                {
                if ( iAudioLost )
                    {
                    TRAP_IGNORE( TryToResumeAudioL() );
                    iCurrentRadioState = EFMRadioStateOff;
                    }
                else if ( iCurrentRadioState == EFMRadioStateOffForPhoneCall )
                    {
                    TurnRadioOn();
                    }
                else
                    {
                    // Do nothing.
                    }
                FadeViewsAndShowExit( EFalse );
                break;
                }
            case EFMRadioEventStandbyMode:
                ExitApplication();
                break;
            case EFMRadioEventFlightModeEnabled:
                TRAP( err, HandleFlightModeEnabledCallbackL() );
                break;
            case EFMRadioEventFlightModeDisabled:
                HandleFlightModeDisabledCallback();
                break;
            case EFMRadioEventFMTransmitterOn:
                TRAP_IGNORE( HandleFMTransmitterOnCallbackL() );
                break;
            case EFMRadioEventFreqRangeChanged:
                HandleFreqRangeChangedCallback();
                break;
            case EFMRadioEventScanLocalStationsCanceled:
                {
                HandleStopSeekCallback();
                SetStartupWizardRunning( EFalse );
                break;	
                }
            default:
                break;
            }
        }
    else
        {
        switch ( aEventCode )
            {
            case EFMRadioEventAudioResourceLost:
            	iAudioLost = ETrue;
                TRAP( err, HandleAudioResourceNotAvailableL( aErrorCode ) );
                break;
            case EFMRadioEventAudioResourcePaused:
                TRAP( err, HandleAudioResourceNotAvailableL( aErrorCode ) );
                break;
            case EFMRadioEventTune:
                if ( iTuneFromWizardActivated )
                    {
                    iTuneFromWizardActivated = EFalse;
                    iRadioEngine->SetMuteOn( EFalse );
                    iMuteStatusBeforeRadioInit = EFMUninitialized;
                    }

                if ( aErrorCode == KFmRadioErrAntennaNotConnected )
                    {
                    iScanLocalStationsView->SetScanCanceled(EFMRadioCancelScanByHeadsetDisconnect);
                    }

                HandleStopSeekCallback();
                break;
            case EFMRadioEventCallStarted:
                {
                FTRACE( FPrint( _L("CFMRadioAppUi::HandleRadioEngineCallBack() error -> EFMRadioEventCallStarted  ") ) );            		            	
                if ( iCurrentRadioState != EFMRadioStateOff && 
                     iCurrentRadioState != EFMRadioStateOffBeforePhoneCall )
                    {
                    iCurrentRadioState = EFMRadioStateOffForPhoneCall;
                    }
                else
                    {
                    iCurrentRadioState = EFMRadioStateOffBeforePhoneCall;
                    }
                FadeViewsAndShowExit( ETrue );
                break;
                }
            default:
                FTRACE( FPrint( _L("CFMRadioAppUi::HandleRadioEngineCallBack() failed to process event.") ) );
                break;
            }
        }
    FTRACE( FPrint( _L("END CFMRadioAppUi::HandleRadioEngineCallBack()") ) );
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::HandleVolumeChangedCallback
// Processes "volume changed" callback
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::HandleVolumeChangedCallback()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::HandleVolumeChangedCallback()  Start") ) );
    if ( !iRadioEngine->IsMuteOn() )
        {
        TInt newVolume = iRadioEngine->GetVolume();  // current volume
        FTRACE( FPrint( _L("CFMRadioAppUi::HandleVolumeChangedCallback() - volume now %d "), newVolume ) );
        iActiveVolumePopupControl->SetValue( newVolume );
        }
    else
        {
        iActiveVolumePopupControl->SetValue( KFMRadioMinVolumeLevel );	
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::HandleInitializedCallbackL
// The initialization of the radio is completed, the radio can be turned on if
// not in call.
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::HandleInitializedCallbackL()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::HandleInitializedCallbackL()  Start") ) );
    iRadioInitializedWithFrequency = ETrue;
    if( iCurrentRadioState != EFMRadioStateOffForPhoneCall && 
        iCurrentRadioState != EFMRadioStateOffBeforePhoneCall )
        {
        TurnRadioOn();
        if ( iRadioEngine->GetAudioOutput() == CRadioEngine::EFMRadioOutputIHF )
            {
            iActiveVolumePopupControl = iIhfVolumePopupControl;
            }
        else
            {
            iActiveVolumePopupControl = iHeadsetVolumePopupControl;
            }
        HandleVolumeChangedCallback();    
        }
    }
// ---------------------------------------------------------------------------
// CFMRadioAppUi::FadeViewsAndShowExit
// fade and show exit for all views
// ---------------------------------------------------------------------------
//

void CFMRadioAppUi::FadeViewsAndShowExit( TBool aState )
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::FadeViewsAndShowExit( Tbool %d )"), aState ) );
    iMainView->FadeAndShowExit( aState );
    iChannelListView->FadeAndShowExit( aState );
    iScanLocalStationsView->FadeAndShowExit( aState );
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::HandleAudioResourceAvailableL
// When audio resource is available, we would resume the radio.
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::HandleAudioResourceAvailableL()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::HandleAudioResourceAvailableL") ) );

    iAudioResourceAvailable = ETrue;

    if ( !IsOfflineProfileActivatedWhenRadioAudioDisabled() &&
        !( iGlobalOfflineQuery && iGlobalOfflineQuery->IsActive() ) )
        {
        if ( ( ( iFMRadioVariationFlags & KFMRadioInternalAntennaSupported) ||
            iRadioEngine->IsHeadsetConnected() ) && !iAudioLost && !iRadioEngine->IsInCall() )
            {
            TurnRadioOn();   
            }
        else if ( iAudioLost &&
                 iRadioEngine->IsHeadsetConnected() )
            {
            TryToResumeAudioL();
            }
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::HandleAudioResourceNotAvailableL
// Handles the cases when the radio is interrupted due to:
// 1. Call setup/in progress
// 2. Higher priority audio
// ---------------------------------------------------------------------------

void CFMRadioAppUi::HandleAudioResourceNotAvailableL( TInt FDEBUGVAR(aError) )
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::HandleAudioResourceNotAvailableL()  error code: %d"),
    aError ) );

    SetStartupWizardRunning( EFalse );
    iAudioResourceAvailable = EFalse;
    
    if ( iCurrentRadioState != EFMRadioStateOffForPhoneCall &&
            iCurrentRadioState != EFMRadioStateOffBeforePhoneCall )
        {
        iCurrentRadioState = EFMRadioStateOff;
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::HandleHeadsetDisconnectedCallback
// Take actions when the headset is disconnected.
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::HandleHeadsetDisconnectedCallback()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::HandleHeadsetDisconnectedCallback" ) ) );

    if ( iFMRadioVariationFlags & KFMRadioInternalAntennaSupported )
        {
        SetAudioOutput( CRadioEngine::EFMRadioOutputIHF );
        }
    else
        {
        // radio is already turned off, and if it was tuning it was canceled
        iCurrentRadioState = EFMRadioStateOff;
        TRAP_IGNORE( ShowConnectHeadsetDialogL() );
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::HandleHeadsetReconnectedCallback
// Take actions when the headset is reconnected.
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::HandleHeadsetReconnectedCallback()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::HandleHeadsetReconnectedCallback()") ) );
    if ( iConnectHeadsetQuery )
        {
        delete iConnectHeadsetQuery;
        iConnectHeadsetQuery = NULL;
        }
    
    //compare bitmask to see if feature supported
    if ( iRadioInitializedWithFrequency &&
         !( iFMRadioVariationFlags & KFMRadioInternalAntennaSupported ) &&
         iCurrentRadioState != EFMRadioStateOffForPhoneCall && 
         iCurrentRadioState != EFMRadioStateOffBeforePhoneCall )
        {
        // active offline query controls radio on/off
        if ( iAudioLost )
            {
            TRAP_IGNORE( TryToResumeAudioL() ); 
            }
        else if ( ( iGlobalOfflineQuery && !iGlobalOfflineQuery->IsActive() ) ||
              !iGlobalOfflineQuery )
            {
            TurnRadioOn();
            }
        HandleVolumeChangedCallback();
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::HandleManualTuneFailedCallback
// Processes "manual tune failed" callback
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::HandleManualTuneFailedCallback()
    {
    HandleStopSeekCallback();
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::HandleVolumeUpdateFailedCallback
// Processes "volume update failed" callback
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::HandleVolumeUpdateFailedCallback()
    {
    HandleVolumeChangedCallback();
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::HandleMuteCallback
// Processes "radio volume muted" callback
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::HandleSetMuteStateCallback()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::HandleSetMuteStateCallback()")) );
    HandleVolumeChangedCallback();
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::HandleStopSeekCallback
// Processes "seek operation completed" callback
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::HandleStopSeekCallback()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::HandleStopSeekCallback() state %d "), iCurrentRadioState) );
    
    if ( iCurrentRadioState == EFMRadioStateBusyManualTune ||
         iCurrentRadioState == EFMRadioStateBusySeek ||
         iCurrentRadioState == EFMRadioStateBusyScanLocalStations )
        {
        iCurrentRadioState = EFMRadioStateOn; // Update state
        }

    const TUid activeViewTuid = ActiveView();
    
    if ( activeViewTuid == KFMRadioMainViewId )
        {
        TRAP_IGNORE( iMainView->StopSeekL() )
        }
    else if ( activeViewTuid == KFMRadioChannelListViewId )
        {
        TRAP_IGNORE( iChannelListView->StopSeekL() )
        }
    else if ( activeViewTuid == KFMRadioScanLocalStationsViewId )
        {
        TRAP_IGNORE( iScanLocalStationsView->StopSeekL() )
        }
    else
        {
        // nop
        }
    FTRACE( FPrint( _L("end CFMRadioAppUi::HandleStopSeekCallback") ) );
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::HandleAudioOutputSetCallback
// Processes "set audio output completed" callback
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::HandleAudioOutputSetCallback()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::HandleAudioOutputSetCallback()")) );
    if ( iRadioEngine->GetAudioOutput() == CRadioEngine::EFMRadioOutputIHF )
        {
        iActiveVolumePopupControl = iIhfVolumePopupControl;
        }
    else
        {
        iActiveVolumePopupControl = iHeadsetVolumePopupControl;
        }
    // restore volume for current output
    TInt volumeLevel = iRadioEngine->GetVolume();
    iRadioEngine->SetVolume( volumeLevel );
    
    if ( iCurrentRadioState != EFMRadioStateBusyScanLocalStations )
        {
        iRadioEngine->SetMuteOn( EFalse );
        }
    
    TUid view = ActiveView();
    
    if ( view == KFMRadioScanLocalStationsViewId )
        {
        iScanLocalStationsView->UpdateToolbar();
        }
    else if ( view == KFMRadioChannelListViewId )
        {
        iChannelListView->UpdateToolbar();
        }
    else if ( view == KFMRadioMainViewId )
        {
        CAknView* mainView = View( KFMRadioMainViewId );
        if ( mainView )
            {
            // force update for toolbar extension view
            mainView->Toolbar()->ToolbarExtension()->SetShown( EFalse );
            }
        }
    else
        {
        // NOP
        }

    // So the views can dynamically change the option menu
    StopDisplayingMenuBar(); // force update for menu bar
    // Force UI update
    HandleVolumeChangedCallback();
    }

// --------------------------------------------------------------------------------
// CFMRadioAppUi::HandleWsEventL
// --------------------------------------------------------------------------------
//
void CFMRadioAppUi::HandleWsEventL( const TWsEvent& aEvent, CCoeControl* aDestination )
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::HandleWsEventL - event %d "), aEvent.Type() ) );
    switch ( aEvent.Type() )
        {
        case KAknUidValueEndKeyCloseEvent:
            {
            if ( IsForeground() )
                {
                const TUid KPhoneAppUid = {0x100058B3}; // hardcoded value for phone app uid
                TApaTaskList taskList( iEikonEnv->WsSession() );
                TApaTask phoneTask = taskList.FindApp( KPhoneAppUid );
                
                if ( phoneTask.Exists() )
                    {
                    // Bring phone to foreground
                    phoneTask.BringToForeground();
                    }
                else
                    {
                    // Phone app should always be there, but this is a backup 
                    // plan, just set radio to background
                    TApaTask task( iEikonEnv->WsSession() );
                    task.SetWgId( iEikonEnv->RootWin().Identifier() );
                    task.SendToBackground();
                    }
                }
            break;
            }
        case EEventFocusLost:
            {
            // being sent to background, cancel any seek expect local stations scan
            if ( iCurrentRadioState == EFMRadioStateBusySeek )
                {
                iRadioEngine->CancelScan();
                HandleStopSeekCallback();
                }
            CAknViewAppUi::HandleWsEventL( aEvent, aDestination );
            break;
            }
        case EEventFocusGained:
            // override default behavior of unfading the ui
            // can be taken out when auto resume is implemented
            CAknViewAppUi::HandleWsEventL(aEvent, aDestination);
            if (iMainView)
                {
                iMainView->UpdateDisplayForFocusGained();
                }
            if (iChannelListView)
                {
                iChannelListView->UpdateDisplayForFocusGained();
                }
            if (iScanLocalStationsView)
                {
                iScanLocalStationsView->UpdateDisplayForFocusGained();
                }
            break;
        default:
            CAknViewAppUi::HandleWsEventL(aEvent, aDestination);
            break;
        }
    }

// ----------------------------------------------------------------------------------------------------
// CFMRadioAppUi::FMRadioSvkChangeVolumeL
// Handles the change in the Volume that is needed as per the Side Volume Key
// events.
// ----------------------------------------------------------------------------------------------------
//
void CFMRadioAppUi::FMRadioSvkChangeVolumeL( TInt aVolumeChange )
    {  
    FTRACE( FPrint( _L("CFMRadioAppUi::FMRadioSvkRemoveVolumeL() aVolumeChange : %d"), aVolumeChange ) );
    
    TBool headsetConnected = iRadioEngine->IsHeadsetConnected();
    
    if ( iCurrentRadioState != EFMRadioStateBusyScanLocalStations && IsStartupWizardHandled() &&
        headsetConnected )
        {
        if ( aVolumeChange > 0 ) // up direction
            {
            UpdateVolume( EDirectionUp );
            }
        else // down direction
            {
            UpdateVolume( EDirectionDown );
            }
        }
    }

// ----------------------------------------------------------------------------------------------------
// CFMRadioAppUi::FMRadioSvkRemoveVolumeL
// Handles the remove Volume option that is needed depending on the duration of the
// Side Volume Key press
// ----------------------------------------------------------------------------------------------------
//
void CFMRadioAppUi::FMRadioSvkRemoveVolumeL()
   {
   FTRACE( FPrint( _L("CFMRadioAppUi::FMRadioSvkRemoveVolumeL( )") ) );
   }

// ----------------------------------------------------------------------------------------------------
// CFMRadioAppUi::FMRadioHeadsetEvent
// Handles the Headset button pressed
// ----------------------------------------------------------------------------------------------------

void CFMRadioAppUi::FMRadioHeadsetEvent( TAccessoryEvent aEvent )
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::FMRadioHeadsetEvent event: %d "), aEvent ) );
    switch ( aEvent )
        {
        case EForward:
            {
            if ( ActiveView() == KFMRadioScanLocalStationsViewId &&
                    iCurrentRadioState != EFMRadioStateBusyScanLocalStations )
                {
                // change channels in scan local stations view
                TRAP_IGNORE( iScanLocalStationsView->HandleScanListRemConEventL( aEvent ) )
                }
            else
                {
                TRAP_IGNORE( HandleCommandL( EFMRadioCmdNextChannel ) )
                }
            break;
            }
        case ERewind:
            {
            if ( ActiveView() == KFMRadioScanLocalStationsViewId &&
                    iCurrentRadioState != EFMRadioStateBusyScanLocalStations )
                {
                TRAP_IGNORE( iScanLocalStationsView->HandleScanListRemConEventL( aEvent ) )
                }
            else
                {
                TRAP_IGNORE( HandleCommandL( EFMRadioCmdPrevChannel ) )
                }
            break;
            }
        case EStop:
            {
            if ( iCurrentRadioState != EFMRadioStateBusyScanLocalStations && IsStartupWizardHandled() )
                {
                iRadioEngine->SetMuteOn( ETrue );
                }
            break;
            }
        case EPausePlay:
            {
            if ( iCurrentRadioState != EFMRadioStateBusyScanLocalStations && IsStartupWizardHandled() )
                {
                HandleMuteCommand();
                }
            break;
            }
        default:
            break;
        }
    }

// ----------------------------------------------------------------------------------------------------
// CFMRadioAppUi::HandleResourceChangeL
// ----------------------------------------------------------------------------------------------------
//
void CFMRadioAppUi::HandleResourceChangeL( TInt aType )
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::HandleResourceChangeL type: %d "), aType ) );
    CAknViewAppUi::HandleResourceChangeL( aType );
    
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        UpdateLandscapeInformation();
        iLayoutChangeObserver->LayoutChangedL( aType );
        }
    else if ( aType == KAknsMessageSkinChange )
        {
        iLayoutChangeObserver->LayoutChangedL( aType );
        }
    }

// ----------------------------------------------------------------------------------------------------
// Handles MCoeControl events
// ----------------------------------------------------------------------------------------------------
void CFMRadioAppUi::HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType )
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::HandleControlEventL - aEventID = %d"), aEventType) );
    if ( (aEventType == EEventStateChanged) &&
         ( aControl == iActiveVolumePopupControl ) )
        {
        TInt volumeControlLevel = iActiveVolumePopupControl->Value();
        
        if ( iRadioEngine->IsMuteOn() && volumeControlLevel == 1 )
            {
            // Volume has been muted and volume is changed from
            // popup. Restore volume to the previous level.
            volumeControlLevel = iRadioEngine->GetVolume();
            volumeControlLevel++;
            if ( volumeControlLevel > KFMRadioMaxVolumeLevel )
                {
                volumeControlLevel = KFMRadioMaxVolumeLevel;
                }
            iActiveVolumePopupControl->SetValue( volumeControlLevel );
            }
            
        // Set mute
        if ( volumeControlLevel == KFMRadioMinVolumeLevel )
            {
            // Mute status could change from toolbar, svk or pointer event
            if ( !iRadioEngine->IsMuteOn() )
                {
                iRadioEngine->SetMuteOn( ETrue );
                }
            }
        else
            {
            iRadioEngine->SetMuteOn( EFalse );	
            }

        // Make sure volume level is in range
        if ( volumeControlLevel < KFMRadioMinVolumeLevel )
            {
            volumeControlLevel = KFMRadioMinVolumeLevel;
            }
        else if ( volumeControlLevel > KFMRadioMaxVolumeLevel )
            {
            volumeControlLevel = KFMRadioMaxVolumeLevel;
            }

        // Set volume. If volume is muted we dont want to set it to iRadioEngine
        if ( !iRadioEngine->IsMuteOn() )
            {
            iRadioEngine->SetVolume( volumeControlLevel );
            }
        }
    }

// ----------------------------------------------------------------------------------------------------
// DialogTerminated
// ----------------------------------------------------------------------------------------------------
void CFMRadioAppUi::DialogTerminated()
    {
    iInfoNoteOn = EFalse;
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::UpdateLandscapeInformation
// ---------------------------------------------------------------------------
//    
void CFMRadioAppUi::UpdateLandscapeInformation()
    {
    TSize screenSize;
    
    AknLayoutUtils::LayoutMetricsSize( AknLayoutUtils::EScreen, screenSize );
        
    // Is it portrait or landscape, compare height and width, since we can't 
    // be sure if EGraphicsOrientationNormal is portrait or landscape
    if ( screenSize.iHeight < screenSize.iWidth )
        {
        iLandscape = ETrue;    
        }
    else
        {
        iLandscape = EFalse;
        }
    }
// ---------------------------------------------------------------------------
// CFMRadioAppUi::UpdateChannelsL
// ---------------------------------------------------------------------------
//    
void CFMRadioAppUi::UpdateChannelsL( TMoveoperations aOperation,
        TInt aIndex, TInt aMovedToNewIndex )
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::UpdateChannelsL(%d, %d, %d)"), aOperation, aIndex, aMovedToNewIndex ) );
    
    // Move channel operation moves channel from aIndex to aMovedToNewIndex value
    if ( EMoveChannels == aOperation )
        {
        CFMRadioPreset* channelInfo  = iChannels[ aIndex ];
        iChannels.Remove( aIndex );
        CleanupStack::PushL( channelInfo );
        iChannels.InsertL( channelInfo, aMovedToNewIndex );
        CleanupStack::Pop( channelInfo);
        }
    // Delete channel deletes channel from list 
    else if ( EDeleteChannel == aOperation )
        {
        const CFMRadioPreset* channelinfo  = iChannels[ aIndex ];
        iChannels.Remove( aIndex );
        delete channelinfo;
        
        for ( TInt index = 0; index < iChannels.Count(); index++ )
            {
            iRadioEngine->SetPresetNameFrequencyL( 
                    index,
                    iChannels[ index ]->PresetName(),
                    iChannels[ index ]->PresetFrequency() );
            
            iRadioEngine->SaveUrlToPresetL( index, iChannels[ index ]->PresetUrl() );
            }
        iRadioEngine->DeletePresetL( iChannels.Count() );
        }

    // Store all channels to repository
    else if ( EStoreAllToRepository == aOperation )
        {
        for ( TInt index = 0; index < iChannels.Count(); index++ )
            {
            iRadioEngine->SetPresetNameFrequencyL( 
                    index, 
                    iChannels[ index ]->PresetName(), 
                    iChannels[ index ]->PresetFrequency() );
            
            iRadioEngine->SaveUrlToPresetL( index, iChannels[ index ]->PresetUrl() );
            }
        }

    // Store specified index to repository and same index 
    else if ( EStoreIndexToRepository == aOperation )
        {
        iRadioEngine->SetPresetNameFrequencyL( 
             aIndex, 
             iChannels[ aIndex ]->PresetName(), 
             iChannels[ aIndex ]->PresetFrequency() );
        
        iRadioEngine->SaveUrlToPresetL( aIndex, iChannels[ aIndex ]->PresetUrl() );
        }
    else if ( ERemoveAllFromRepository == aOperation )
        {
        iRadioEngine->DeletePresetL( -1 ); // reset all presets
        iChannels.ResetAndDestroy();
        }
    
    iRadioEngine->PubSubL().PublishPresetCountL( iChannels.Count() );
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::AddChannelToListL
// ---------------------------------------------------------------------------
// 
void CFMRadioAppUi::AddChannelToListL( const TDesC& aChannelName, TInt aChannelFreq )
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::AddChannelToListL(%S, %d)"), &aChannelName, aChannelFreq ) );
    CFMRadioPreset* preset = CFMRadioPreset::NewL();
    CleanupStack::PushL( preset );
    preset->SetPresetNameL( aChannelName );
    preset->SetPresetFrequency( aChannelFreq );
    iChannels.AppendL( preset );
    CleanupStack::Pop( preset );
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::IsLandscapeOrientation
// ---------------------------------------------------------------------------
//
TBool CFMRadioAppUi::IsLandscapeOrientation() const
    {
    return iLandscape;
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::ChooseLayoutResource
// ---------------------------------------------------------------------------
//
TInt CFMRadioAppUi::ChooseLayoutResource( TInt aNormalPortraitRes, 
        TInt aMirroredPortraitRes, TInt aNormalLandscapeRes, 
        TInt aMirroredLandscapeRes ) const
    {
    TInt resId = 0;
    
    if ( IsLandscapeOrientation() )
        {
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            resId = aMirroredLandscapeRes;
            }
        else
            {
            resId = aNormalLandscapeRes;
            }
        }
    else
        {
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            resId = aMirroredPortraitRes;
            }
        else
            {
            resId = aNormalPortraitRes;
            }
        }
    return resId;
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::IsLayoutMirrored
// ---------------------------------------------------------------------------
//
TBool CFMRadioAppUi::IsLayoutMirrored() const
    {
    return AknLayoutUtils::LayoutMirrored();
    }
    
// ---------------------------------------------------------------------------
// CFMRadioAppUi::HandleFlightModeEnabledCallbackL
// Displays continue offline query.
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::HandleFlightModeEnabledCallbackL()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::HandleFlightModeEnabledCallbackL" ) ) );

    // If off-line profile is activated when no other app
    // (that would disable auto resume) is playing (or has played)
    // audio, a global confirmation query is displayed.
    if ( iAudioResourceAvailable )
        {
        iRadioEngine->RadioOff();
        
        if ( !iGlobalOfflineQuery )
            {
            iGlobalOfflineQuery = CFMRadioGlobalConfirmationQuery::NewL( this );                                
            }
        
        HBufC* text = StringLoader::LoadLC( R_QTN_FMRADIO_QUERY_OFFLINE_USE, iCoeEnv );
        
        iOfflineQueryDialogActivated = ETrue;
        iGlobalOfflineQuery->ShowQueryL( *text, R_AVKON_SOFTKEYS_YES_NO, R_QGN_NOTE_QUERY_ANIM ); 
        
        CleanupStack::PopAndDestroy( text );
        }
            
    // Do the next when offline query wasn't already on the screen.
    // That could happen when user changes profiles without
    // answering to the query.
    else if (!iGlobalOfflineQuery || !iGlobalOfflineQuery->IsActive())
        {
        iOfflineProfileActivatedWhenRadioAudioDisabled = ETrue;
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::HandleFlightModeDisabledCallback
// Close query dialog.
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::HandleFlightModeDisabledCallback()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::HandleFlightModeDisabledCallback" ) ) );
    iOfflineProfileActivatedWhenRadioAudioDisabled = EFalse;
    
    // Close continue offline query
    if ( iLocalContinueOfflineQuery && iLocalContinueOfflineQuery->IsVisible() )
        {
        TRAP_IGNORE( iLocalContinueOfflineQuery->DismissQueryL() );
        }
    // Close activate offline query
    if ( iLocalActivateOfflineQuery && iLocalActivateOfflineQuery->IsVisible() )
        {
        TRAP_IGNORE( iLocalActivateOfflineQuery->DismissQueryL() );
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::StaticStartupForegroundCallback
// takes care of some delayed initialisation
// ---------------------------------------------------------------------------
//
TInt CFMRadioAppUi::StaticStartupForegroundCallback( TAny* aSelfPtr )
    {
    CFMRadioAppUi* self = reinterpret_cast<CFMRadioAppUi*>( aSelfPtr );
    if ( self )
        {
        delete self->iStartupForegroundCallback;
        self->iStartupForegroundCallback = NULL;

        TRAPD( err, self->HandleStartupForegroundEventL() )
        if ( err )
            {
            // Shutdown the application
            CEikonEnv* eikonEnv = CEikonEnv::Static();
            TApaTask task( eikonEnv->WsSession() );
            task.SetWgId( eikonEnv->RootWin().Identifier() );
            task.EndTask();
            }
        }
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::HandleStartupForegroundEventL
// Note that leaving from this fuction will cause exit of
// FM Radio (on purpose)!
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::HandleStartupForegroundEventL()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::HandleStartupForegroundEventL" ) ) );
    HandleOfflineModeAtStartUpL();
    TFMRadioRegionSetting region = HandleRegionsAtStartUpL();
    iRadioEngine->SetRegionIdL( region );
    HandlePendingViewActivationL();
    iRadioEngine->RequestTunerControl();
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::HandleOfflineModeAtStartUpL
// Handles offline mode at startup.
// ---------------------------------------------------------------------------
//	
void CFMRadioAppUi::HandleOfflineModeAtStartUpL()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::HandleOfflineModeAtStartUpL" ) ) );
    if ( iRadioEngine->IsOfflineProfileL() )
        {
        iOfflineQueryDialogActivated = ETrue;
        
        if ( AknLayoutUtils::PenEnabled() )
            {
            // We must hide toolbar, otherwise query dialog softkeys doesn't work 
            ShowToolbar( EFalse );
            }
        
        iLocalActivateOfflineQuery = CAknQueryDialog::NewL();
        TInt result = iLocalActivateOfflineQuery->ExecuteLD( R_FMRADIO_ACTIVATE_IN_OFFLINE_QUERY );
        iLocalActivateOfflineQuery = NULL;
        
        iOfflineQueryDialogActivated = EFalse;
       
        if ( result )
            {
            if ( AknLayoutUtils::PenEnabled() )
                {
                ShowToolbar( ETrue );
                }
            }
        else
            {
            Exit();
            }
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::ShowToolbar
// Sets toolbar visibility. This function is used to hide toolbar component
// while query dialog is shown.
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::ShowToolbar( TBool aVisible )
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::ShowToolbar(%d)"), aVisible ) );
    if ( iLayoutChangeObserver == iMainView )
        {
        iMainView->ShowToolbar( aVisible );	
        }
    else if ( iLayoutChangeObserver == iScanLocalStationsView )
        {
        iScanLocalStationsView->ShowToolbar( aVisible );
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::IsOfflineProfileActivatedWhenRadioAudioDisabled
// ---------------------------------------------------------------------------
//
TBool CFMRadioAppUi::IsOfflineProfileActivatedWhenRadioAudioDisabled() const
    {
    return iOfflineProfileActivatedWhenRadioAudioDisabled;
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::HandleFMTransmitterOnCallback
// Displays note about transmitter being activated and
// fade the UI.
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::HandleFMTransmitterOnCallbackL()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::HandleFMTransmitterOnCallbackL()") ) );
    iCurrentRadioState = EFMRadioStateOff;
    
    CAknGlobalNote* fmTransmitterGlobalNote = CAknGlobalNote::NewLC();
    
    if ( IsStartupWizardHandled() )
        {
        // Wizard is allready handled so this callback is not coming
        // in startup phase
        HBufC* closeAppText = StringLoader::LoadLC( R_QTN_FMRADIO_NOTE_FMTX_CLOSE_APPLICATION, iCoeEnv  );
        fmTransmitterGlobalNote->ShowNoteL( EAknGlobalInformationNote, *closeAppText );
        CleanupStack::PopAndDestroy( closeAppText );
        }
    else
        {
        HBufC* unableToStartText = StringLoader::LoadLC( R_QTN_FMRADIO_NOTE_FMTX_UNABLE_TO_START, iCoeEnv );
        fmTransmitterGlobalNote->ShowNoteL( EAknGlobalInformationNote, *unableToStartText );
        CleanupStack::PopAndDestroy( unableToStartText );
        }
    CleanupStack::PopAndDestroy( fmTransmitterGlobalNote );
    // check if we have any dialogs open and close them manually
    if ( IsDisplayingDialog() )
        {
        TKeyEvent key;
        key.iRepeats = 0;
        key.iCode = EKeyEscape;
        key.iModifiers = 0;
        
        iCoeEnv->SimulateKeyEventL( key, EEventKey );
        // in case there is still dialog on the screen 
        if ( IsDisplayingDialog() )
            {
            iCoeEnv->SimulateKeyEventL( key, EEventKey );
            }
        }
    // Shutdown the application
    TApaTask task( iCoeEnv->WsSession() );
    task.SetWgId( iCoeEnv->RootWin().Identifier() );
    task.EndTask();
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::HandleFreqRangeChangedCallback
// Frequency range changed.
// Current handling is to exit the application since
// the UI specification doesn't specify how this should
// be handled.
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::HandleFreqRangeChangedCallback()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::HandleFreqRangeChangedCallback()") ) );
    iCurrentRadioState = EFMRadioStateOff;
    Exit();
    } 
// ---------------------------------------------------------------------------
// CFMRadioAppUi::HandleMuteCommand
// Handles mute keypress from main view
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::HandleMuteCommand()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::HandleMuteCommand" ) ) );
    if ( iRadioEngine->IsMuteOn() )
        {
        iRadioEngine->SetMuteOn( EFalse );
        }
    else
        {
        iRadioEngine->SetMuteOn( ETrue );
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::HandleRegionsAtStartUpL
// get region automatically from network or manually by user choice if network
// is not available
// ---------------------------------------------------------------------------
//
TFMRadioRegionSetting CFMRadioAppUi::HandleRegionsAtStartUpL()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::HandleRegionsAtStartUpL()")) );
    TFMRadioRegionSetting region = iRadioEngine->GetRegionL();
    
    if ( region == EFMRadioRegionNone )
        {
        ShowToolbar( EFalse );
        TInt index = 0;
        
        CDesCArraySeg* regionList = new( ELeave ) CDesCArraySeg( KDefaultRegionArrayGranularity );
        CleanupStack::PushL( regionList );
        
        iRadioEngine->FillListWithRegionDataL( *regionList );
        
        CAknListQueryDialog* dlg = new ( ELeave ) CAknListQueryDialog( &index );
        
        dlg->PrepareLC( R_FMRADIO_SELECT_REGION_QUERY );
        CTextListBoxModel* model = 
        static_cast<CAknListQueryControl*>( dlg->ListControl() )->listbox()->Model();
        model->SetItemTextArray( regionList );
        model->SetOwnershipType( ELbmDoesNotOwnItemArray ); // Array won't be deleted here
            	
        TInt ret = dlg->RunLD();
        CleanupStack::PopAndDestroy( regionList );
        
        if ( ret != EAknSoftkeyExit )
            {
            // We should have a proper region selected by user
            region = iRadioEngine->RegionIdAtIndex( index );
            }
        
        if ( region == EFMRadioRegionNone )
            {
            Exit();// This will eventually exit FMRadio.
            }
        else
            {
            HBufC* bandSetText = StringLoader::LoadLC( R_QTN_FMRADIO_CONF_FREQ_BAND_SET_MANUAL, iCoeEnv );
            CAknInformationNote* note = new( ELeave ) CAknInformationNote( ETrue );
            note->ExecuteLD( *bandSetText );
            CleanupStack::PopAndDestroy( bandSetText );
            if ( AknLayoutUtils::PenEnabled() )
                {
                ShowToolbar( ETrue );
                }
            }
        }
    else
        {
        // Region is known        
        if ( region != iRadioEngine->RegionId() )
            {
            TInt previousRegion = iRadioEngine->RegionId();
            
           if ( previousRegion != EFMRadioRegionNone )
                {
                // Show note only if the region has changed from a concrete 
                // one to another concrete one. EFMRadioRegionNone is not 
                // a concrete one (first startup case) -- don't show note.
                iRegionChanged = ETrue;
                HBufC* bandSetText = StringLoader::LoadLC( R_QTN_FMRADIO_CONF_FREQ_BAND_SET_AUTOM, iCoeEnv );
                CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );			    
                note->ExecuteLD( *bandSetText );
                CleanupStack::PopAndDestroy( bandSetText );
                }
            }
        }
    return region;
    }

// ---------------------------------------------------------------------------
// From class MFMRadioGlobalConfirmationQueryObserver. Callback function 
// that is called when global confirmation query is dismissed.
// CFMRadioAppUi::GlobalConfirmationQueryDismissedL
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::GlobalConfirmationQueryDismissedL(TInt aSoftKey)
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::GlobalConfirmationQueryDismissedL()")) );
    iOfflineQueryDialogActivated = EFalse;
    
    if ( aSoftKey == EAknSoftkeyYes )
        {
        iOfflineProfileActivatedWhenRadioAudioDisabled = EFalse;
        iRadioEngine->RadioOn();
        HandlePendingViewActivationL();
        }    
    else
        {
        // Shutdown the application
        TApaTask task( iCoeEnv->WsSession() );
        task.SetWgId( iCoeEnv->RootWin().Identifier() );
        task.EndTask();
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::NumberOfChannelsStored
// return number of channels saved to the presets
// ---------------------------------------------------------------------------
//    
TInt CFMRadioAppUi::NumberOfChannelsStored() const
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::NumberOfChannelsStored()")) );
    return iChannels.Count();
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::HandleStartupWizard
// Determine if "Scan all channels" in scan local stations view should be started.
// ---------------------------------------------------------------------------
// 
void CFMRadioAppUi::HandleStartupWizardL()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::HandleStartupWizardL()")) );
    if ( !IsStartupWizardHandled() || iRegionChanged )
        {
        // Call to iRadioEngine->UpdatedStartupCount()
        // increases startup count by one and returns amount of app startups.
        TInt startupCount = iRadioEngine->UpdatedStartupCount();
        if ( startupCount <= KMaxStartupTimesToAskSaveWizard &&
                NumberOfChannelsStored() == 0 )
            {
            SetStartupWizardRunning( ETrue );
            ActivateLocalViewL( iScanLocalStationsView->Id() );
            }
        else if ( iRegionChanged ) //force rescan
            {
            iRegionChanged = EFalse;
            UpdateChannelsL( ERemoveAllFromRepository, 0, 0 );
            SetStartupWizardRunning( ETrue );
            ActivateLocalViewL( iScanLocalStationsView->Id() );
            }
        else // check IAD update here
            {
            HandleIADUpdateCheckL();
            }
        }
    }

// --------------------------------------------------------------------------
// CFMRadioAppUi::IsStartupWizardRunning
// Return startup scanning wizard status.
// ---------------------------------------------------------------------------
// 	
TBool CFMRadioAppUi::IsStartupWizardRunning() const
    {
    return iStartupWizardRunning;
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::SetStartupWizardRunning
// Set startup scanning wizard status.
// ---------------------------------------------------------------------------
// 
void CFMRadioAppUi::SetStartupWizardRunning( const TBool aRunningState )
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::SetStartupWizardRunning() - state = %d"), aRunningState) );
    iStartupWizardRunning = aRunningState;	
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::StartLocalStationsSeekL
// Start local stations scan
// ---------------------------------------------------------------------------
// 
void CFMRadioAppUi::StartLocalStationsSeekL()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::StartLocalStationsSeekL()")) );
    iCurrentRadioState = EFMRadioStateBusyScanLocalStations;
    iRadioEngine->ScanUp();
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::TryToResumeAudioL
// Show Audio Preempted query
// ---------------------------------------------------------------------------
// 
void CFMRadioAppUi::TryToResumeAudioL()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::TryToResumeAudioL" ) ) );
    TFMRadioRegionSetting region = iRadioEngine->GetRegionL();
    if ( iAudioLost && 
         IsForeground() &&
         !iOfflineQueryDialogActivated && 
         region != EFMRadioRegionNone )
        {
        iAudioLost = EFalse;
        TurnRadioOn();
        }
    }
    
// ---------------------------------------------------------------------------
// CFMRadioAppUi::ShowConnectHeadsetDialogL
// Show connect headset query with only exit softkey
// ---------------------------------------------------------------------------
// 	
void CFMRadioAppUi::ShowConnectHeadsetDialogL()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::ShowConnectHeadsetDialogL" ) ) );
    if ( !iConnectHeadsetQuery && !iConnectHeadsetGlobalNote )
        {
        HBufC* noteTxt = StringLoader::LoadLC( R_QTN_FMRADIO_ERROR_CON_HEADSET, iCoeEnv  );
        
        // if radio is in background, show also global note	
        if ( !IsForeground() )
            {
            iConnectHeadsetGlobalNote = CAknGlobalNote::NewL();
            iConnectHeadsetGlobalNote->ShowNoteL( EAknGlobalInformationNote, *noteTxt );
            delete iConnectHeadsetGlobalNote;
            iConnectHeadsetGlobalNote = NULL;
            }
        
        iConnectHeadsetQuery = CAknQueryDialog::NewL( CAknQueryDialog::EErrorTone );									
        TInt ret = iConnectHeadsetQuery->ExecuteLD( R_FMRADIO_CONNECT_HEADSET_NOTE, *noteTxt );
            
        CleanupStack::PopAndDestroy( noteTxt );
        FTRACE( FPrint( _L("CFMRadioAppUi::ShowConnectHeadsetDialogL() return value from dialog: %d"), ret ) );
        if ( iConnectHeadsetQuery )
            {
            iConnectHeadsetQuery = NULL;
            if ( ret == EAknSoftkeyExit || ret == EAknSoftkeyOk )
                {
                TApaTask task( iCoeEnv->WsSession() );
                task.SetWgId( iCoeEnv->RootWin().Identifier() );
                task.EndTask();
                }
            else
                {
                HandlePendingViewActivationL();
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::ShowVolumePopupL
// Show currently active volume popup
// ---------------------------------------------------------------------------
// 
void CFMRadioAppUi::ShowVolumePopupL()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::ShowVolumePopupL" ) ) );
    if ( !iRadioEngine->IsInCall() )
        {
        if ( iActiveVolumePopupControl )
            {
            if ( IsForeground() || ( IsActiveIdleEnabled() && IsIdleAppForeground() ) )
                {
                iActiveVolumePopupControl->ShowVolumePopupL();
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::GetCurrentHelpContext
// get help context based on currently active view
// ---------------------------------------------------------------------------
//	
CArrayFix<TCoeHelpContext>* CFMRadioAppUi::GetCurrentHelpContextL() const
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::GetCurrentHelpContextL" ) ) );
#if defined __SERIES60_HELP || defined FF_S60_HELPS_IN_USE
    // currently main container and manual tuning container are derived from CBase
    // so lets create help array manually for them
        CArrayFixFlat<TCoeHelpContext>* helpArray = new( ELeave )
        CArrayFixFlat<TCoeHelpContext>( 1 );
        CleanupStack::PushL( helpArray );
        helpArray->AppendL( TCoeHelpContext( TUid::Uid( KUidFMRadioApplication ), KFMRADIO_HLP_MAIN ) );
        CleanupStack::Pop( helpArray );
        return helpArray;
#else
    return AppHelpContextL();
#endif
}

// ---------------------------------------------------------------------------
// CFMRadioChannelListView::GetChannelsArrayL
// Get channels array list from radio engine
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::GetChannelsArrayL()
    {
    FTRACE( FPrint( _L("CFMRadioChannelListView::GetChannelsArrayL  Start") ) );
    
    // Note: KMaxNumberOfChannelListItems can be setted to any supported max value
    for ( TInt index = 0; index < KMaxNumberOfChannelListItems; index++ )
        {
        TBuf<KPresetNameLength> chName;
        TInt chFrequency;
        
        iRadioEngine->GetPresetNameAndFrequencyL( index,
                                                  chName, 
                                                  chFrequency );

        if ( chFrequency != KErrNotFound )
            {
            FTRACE( FPrint( _L("CFMRadioChannelListView::ChannelInUse  inside if") ) );
            CFMRadioPreset* preset = CFMRadioPreset::NewL();
            CleanupStack::PushL( preset );
            preset->SetPresetNameL( chName );
            preset->SetPresetFrequency( chFrequency );
            
            RBuf url;
            url.CleanupClosePushL();
            url.CreateL( KFMRadioWebLinkMaxLength );
            TInt err = iRadioEngine->PresetUrlL( index, url );
            if ( !err )
                {
                preset->SetPresetUrlL( url );
                }
            
            iChannels.AppendL( preset );
            CleanupStack::PopAndDestroy( &url );
            CleanupStack::Pop( preset );
            }
        }
    }

// ---------------------------------------------------------------------------
// From class CEikAppUi.
// Handles window server messages.
// ---------------------------------------------------------------------------
//
MCoeMessageObserver::TMessageResponse CFMRadioAppUi::HandleMessageL( 
        TUint32 aClientHandleOfTargetWindowGroup, TUid aMessageUid, 
        const TDesC8& aMessageParameters )
    {
    FTRACE(FPrint(_L(" *** S60 FMRadio-- CFMRadioAppUi::HandleMessageL" )));

    TMessageResponse resp = CAknViewAppUi::HandleMessageL( 
        aClientHandleOfTargetWindowGroup, aMessageUid, aMessageParameters );
        
    if ( resp == EMessageNotHandled && aMessageUid == TUid::Uid( KUidFMRadioApplication ) )
        {
        ProcessCommandTailL( aMessageParameters );
        resp = EMessageHandled;
        }

    return resp;
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::ProcessCommandParametersL
// Processes the shell commands
// ---------------------------------------------------------------------------
//
TBool CFMRadioAppUi::ProcessCommandParametersL( TApaCommand aCommand, 
                                                TFileName& aDocumentName, 
                                                const TDesC8& aTail )
    {
    FTRACE(FPrint(_L("CFMRadioAppUi::ProcessCommandParametersL(%d, %S)" ), aCommand, &aTail ));
    SecondaryConstructL();
    
    TBool ret = CAknViewAppUi::ProcessCommandParametersL( aCommand, aDocumentName, aTail );
    ProcessCommandTailL( aTail );

    return ret;
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::ProcessCommandParametersL
// Processes the command tail in any situation
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::ProcessCommandTailL( const TDesC8& aTail )
    {
    TLex8 tailLex( aTail );
    if ( tailLex.Peek() != 0 )
        {
        TPtrC8 command = tailLex.NextToken();
        if ( command == KFMRadioCommandFrequency )
            {
            // tune to frequency
            TLex8 frequencyLex( tailLex.NextToken() );
            TInt32 frequency = 0;
            TInt min = 0;
            TInt max = 0;
            iRadioEngine->GetFrequencyBandRange( min, max );
            TInt ret = frequencyLex.BoundedVal( frequency, max );
            if ( ret == KErrNone && frequency >= min )
                {
                iRadioEngine->CancelScan();
                TuneL( frequency );
                }
            }
        else if ( command == KFMRadioCommandStationIndex )
            {
            // tune to station index
            TLex8 indexLex( tailLex.NextToken() );
            TInt32 index = 0;
            TInt ret = indexLex.BoundedVal( index, iChannels.Count() - 1 );
            if ( ret == KErrNone && index >= 0 )
                {
                iRadioEngine->CancelScan();
                PlayChannel( index );
                }
            }
        else if( command == KFMRadioCommandActivateView )
            {
            TLex8 viewIdLex( tailLex.NextToken() );
            TInt viewId = 0;
            TInt ret = viewIdLex.Val( viewId );
            if ( ret == KErrNone )
                {
                TUid uid = TUid::Uid( viewId );
                CAknView* view = View( uid );
                // Currently this API supports only activating main view or channel list view
                if ( view && ( uid == KFMRadioMainViewId || uid == KFMRadioChannelListViewId ) )
                    {
                    if ( iStartUp )
                        {
                        SetDefaultViewL( *view );
                        }
                    else
                        {
                        TFMRadioRegionSetting region = iRadioEngine->GetRegionL();  
                        
                        if ( iOfflineQueryDialogActivated || 
                             iConnectHeadsetQuery || 
                             region == EFMRadioRegionNone )
                            {
                            iPendingViewId = view->Id(); 
                            }
                        else
                            {
                            iChannelListView->CancelMoveL();
                            ActivateLocalViewL( view->Id() );
                            }
                        TApaTask task( iCoeEnv->WsSession() );
                        task.SetWgId( iCoeEnv->RootWin().Identifier() );
                        task.BringToForeground();
                        }
                    }
                }
            }
        else
            {
            }
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::Channels
// Returns channels array
// ---------------------------------------------------------------------------
//
RPointerArray<CFMRadioPreset>& CFMRadioAppUi::Channels()
	{
	return iChannels;
	}

// ---------------------------------------------------------------------------
// CFMRadioAppUi::HandleIADUpdateCheckL
// Initialize software update check using IAD api
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::HandleIADUpdateCheckL()
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::HandleIADUpdateCheckL") ) );
    if ( FeatureManager::FeatureSupported( KFeatureIdIAUpdate ) )
        {
        FTRACE( FPrint( _L("CFMRadioAppUi::HandleIADUpdateCheckL - KFeatureIdIAUpdate supported") ) );
        TRAPD( err, iUpdate = CIAUpdate::NewL( *this ) )
        if ( err && err != KErrNotSupported )
            {
            User::Leave( err );
            }
        if ( !err )
            {
            iParameters = CIAUpdateParameters::NewL();
            // Use SIS package UID
            iParameters->SetUid( TUid::Uid( KUidFMRadioApplication ) );
            iUpdate->CheckUpdates( *iParameters );
            }
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::CheckUpdatesComplete
// from base class MIAUpdateObserver
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::CheckUpdatesComplete( TInt aErrorCode, TInt aAvailableUpdates )
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::CheckUpdatesComplete(%d, %d)"), aErrorCode, aAvailableUpdates ) );
    if ( aErrorCode == KErrNone )
        {
        if ( aAvailableUpdates > 0 )
            {
            // There were some updates available. Let's ask if the user wants to update them.
            iUpdate->UpdateQuery();
            }
        else
            {
            // No updates available. CIAUpdate object could be deleted already now, since we are not
            // going to call anything else from the API. This will also close down the IAUpdate server.
            delete iUpdate;
            iUpdate = NULL;
            delete iParameters;
            iParameters = NULL;
            }
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::UpdateComplete
// from base class MIAUpdateObserver
// ---------------------------------------------------------------------------
//
void CFMRadioAppUi::UpdateComplete( TInt FDEBUGVAR( aErrorCode ), CIAUpdateResult* aResultDetails )
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::UpdateComplete(%d)"), aErrorCode ) );
    // The update process that the user started from IAUpdate UI is now completed.
    // If the client application itself was updated in the update process, this callback
    // is never called, since the client is not running anymore.

    delete aResultDetails; // Ownership was transferred, so this must be deleted by the client

    // We do not need the client-server session anymore, let's delete the object to close the session
    delete iUpdate;
    iUpdate = NULL;
    delete iParameters;
    iParameters = NULL;	
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::UpdateQueryComplete
// from base class MIAUpdateObserver
// ---------------------------------------------------------------------------
//	
void CFMRadioAppUi::UpdateQueryComplete( TInt aErrorCode, TBool aUpdateNow )			
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::UpdateQueryComplete(%d, %d)"), aErrorCode, aUpdateNow ) );
    if ( aErrorCode == KErrNone )
        {
        if ( aUpdateNow )
            {
            // User choosed to update now, so let's launch the IAUpdate UI.
            iUpdate->ShowUpdates( *iParameters );
            }
        else
            {
            // The answer was 'Later'. CIAUpdate object could be deleted already now, since we are not
            // going to call anything else from the API. This will also close down the IAUpdate server.
            delete iUpdate;
            iUpdate = NULL;
            delete iParameters;
            iParameters = NULL;
            }
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::AutoTuneInMainView
// Set autotune true, if scan is interrupted
// ---------------------------------------------------------------------------
//	
void CFMRadioAppUi::AutoTuneInMainView ( TBool /*aTune*/ )
    {
    FTRACE( FPrint( _L("CFMRadioAppUi::AutoTuneInMainView()")));
    }

// ---------------------------------------------------------------------------
// CFMRadioAppUi::IsStartupWizardHandled
// ---------------------------------------------------------------------------
//
TBool CFMRadioAppUi::IsStartupWizardHandled() const
    {
    return iStartupWizardHandled;
    }

// -----------------------------------------------------------------------------
// Check if Active Idle app is on foreground
// -----------------------------------------------------------------------------
//
TBool CFMRadioAppUi::IsIdleAppForeground()
    {
    FTRACE( FPrint( _L( "CFMRadioAppUi::IsIdleAppForeground" ) ) );
    TBool isIdleActive( EFalse );
    // Gets the window group id of the app in foreground
    RWsSession& wsSession = iCoeEnv->Static()->WsSession();
    TInt windowGroupId = wsSession.GetFocusWindowGroup();
    if ( windowGroupId >= 0 )
        {
        TRAP_IGNORE(
            {
            CApaWindowGroupName* wgName = CApaWindowGroupName::NewL(
                wsSession, windowGroupId );
            
            isIdleActive = ( wgName->AppUid() == KFMRadioUidIdleApp );
            delete wgName;
            } );
        }
    return isIdleActive;
    }

// -----------------------------------------------------------------------------
// Check if Active Idle is enabled
// -----------------------------------------------------------------------------
//
TBool CFMRadioAppUi::IsActiveIdleEnabled()
    {
    FTRACE( FPrint( _L( "CFMRadioAppUi::IsActiveIdleEnabled" ) ) );
    TBool res( EFalse );

#ifdef __ACTIVE_IDLE
    if (! FeatureManager::FeatureSupported( KFeatureIdActiveIdle ) )
        return EFalse;
#endif
    if ( !iSettingsRepository )
        {
        TRAP_IGNORE(
            {
            iSettingsRepository = CRepository::NewL(
                KCRUidPersonalizationSettings );
            } );
        }

    if ( iSettingsRepository )
        {
        TInt value( 0 );
        TInt err( iSettingsRepository->Get( KSettingsActiveIdleState, value ) );
        if ( !err && value )
            {
            res = ETrue;
            }
        }
    return res;
    }

// ---------------------------------------------------------
// CFMRadioAppUi::MatchingChannel
// Return the index of the first channel that matches the
// frequency specified.
// ---------------------------------------------------------
//
TInt CFMRadioAppUi::MatchingChannelL( TInt aFrequency )
    {
    TInt channelIndex = KErrNotFound;
    TInt numberOfChannels = iChannels.Count();
    
    for ( TInt i = 0; i < numberOfChannels; i++ )
        {
        TInt presetFrequency = iChannels[ i ]->PresetFrequency();
        
        if ( aFrequency == presetFrequency )
            {
            channelIndex = i;
            break;
            }
        }
    FTRACE(FPrint(_L("CFMRadioAppUi::MatchingChannel i[%d] aFrequency[%d]"), channelIndex, aFrequency));
    return channelIndex;
    }

// ---------------------------------------------------------
// CFMRadioAppUi::HandlePendingViewActivationL
// Activates the view that is pending
// ---------------------------------------------------------
//
void CFMRadioAppUi::HandlePendingViewActivationL()
    {
    if ( iPendingViewId != TUid::Null() )
        {
        ActivateLocalViewL( iPendingViewId );
        iPendingViewId == TUid::Null();
        }
    }

// ---------------------------------------------------------
// CFMRadioAppUi::ActiveVolumePopup
// ---------------------------------------------------------
//
CAknVolumePopup* CFMRadioAppUi::ActiveVolumePopup() const
    {
    return iActiveVolumePopupControl;
    }

// ---------------------------------------------------------
// CFMRadioAppUi::RadioState
// ---------------------------------------------------------
//
CFMRadioAppUi::TRadioState CFMRadioAppUi::RadioState() const
    {
    return iCurrentRadioState;
    }
	
// End of File
