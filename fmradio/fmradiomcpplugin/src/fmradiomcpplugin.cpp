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
* Description: Implementation for the FM Radio Content Publisher Plugin.
*
*/

#include <AknUtils.h>
#include <bautils.h>
#include <coemain.h>
#include <data_caging_path_literals.hrh>
#include <data_caging_paths_strings.hrh>
#include <e32cmn.h>
#include <ecom/implementationproxy.h>
#include <liwvariant.h>
#include <StringLoader.h>
#include <aknsconstants.hrh>
#include <AknsItemID.h>
#include <fmradiomcpplugin.rsg>
#include <fmradio.mbg>

#include "fmradioactionhandler.h"
#include "fmradioactionhandler.hrh"
#include "fmradioactionhandlerdefs.h"
#include "fmradioactiveidleengine.h"
#include "fmradioactiveidleenginenotifyhandler.h"
#include "fmradiodefines.h"
#include "debug.h"
#include "fmradiomcpplugin.h"
#include "fmradiomcpplugin.hrh"
#include "AknsConstants.h"

_LIT( KFMRadioMifFileName, "fmradio.mif" );
_LIT( KFMRadioMifDir, "\\resource\\apps\\" );

_LIT( KFMRadioMCPPluginResourceFileName, "fmradiomcpplugin.rsc" );
_LIT8( KFMRadioPluginUid, "plugin_id" );
_LIT8( KFMRadioData, "data" );
_LIT8( KType, "type" );

const TUint KFMRadioPrevious        = 0x001; //000000000001
const TUint KFMRadioNext            = 0x002; //000000000010
const TUint KFMRadioMute            = 0x004; //000000000100
const TUint KFMRadioUnmute          = 0x008; //000000001000
const TUint KFMRadioPreviousDimmed  = 0x010; //000000010000
const TUint KFMRadioNextDimmed      = 0x020; //000000100000
const TUint KFMRadioMuteDimmed      = 0x040; //000001000000
const TUint KFMRadioUnmuteDimmed    = 0x080; //000010000000

CFMRadioMCPPlugin* CFMRadioMCPPlugin::NewL( MMCPPluginObserver* aObserver )
    {
    CFMRadioMCPPlugin* self = new ( ELeave ) CFMRadioMCPPlugin( aObserver );
    CleanupStack::PushL( self );

    self->ConstructL();
    
    CleanupStack::Pop( self );
    return self;
    }

CFMRadioMCPPlugin::~CFMRadioMCPPlugin()
    {
    if ( iResourceOffset > 0 )
        {
        CCoeEnv::Static()->DeleteResourceFile( iResourceOffset );
        }

    delete iEngine;
    iNowPlayingText.Close();
    iLastPlayedText.Close();
    iAntennaNotConnectedText.Close();
    iTuningText.Close();
    iSavedStationFormat.Close();
    iSavedStationFormatNoName.Close();
    iFrequencyFormat.Close();
    iSavedStationFormatMenu.Close();
    iSavedStationFormatNoNameMenu.Close();
    iFrequencyFormatMenu.Close();
    
    iMifFileName.Close();
    }

CFMRadioMCPPlugin::CFMRadioMCPPlugin( MMCPPluginObserver* aObserver )
    : iObserver( aObserver )
    {
    }

void CFMRadioMCPPlugin::ConstructL()
    {
    FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::ConstructL" )));
    InitializeResourceLoadingL();
    
    CCoeEnv* coeEnv = CCoeEnv::Static();

    iNowPlayingText.Assign( StringLoader::LoadL( R_QTN_FMRADIO_SUITE_NOW_PLAYING, coeEnv ) );
    iLastPlayedText.Assign( StringLoader::LoadL( R_QTN_FMRADIO_SUITE_LAST_PLAYED, coeEnv ) );
    
    iAntennaNotConnectedText.Assign( StringLoader::LoadL( R_QTN_FMRADIO_WIDGET_CON_HEADSET, coeEnv ) );
    iTuningText.Assign( StringLoader::LoadL( R_QTN_FMRADIO_WIDGET_TUNING, coeEnv ) );
    iSavedStationFormat.Assign( StringLoader::LoadL( R_QTN_FMRADIO_WIDGET_STATION_SAVED, coeEnv ) );
    iSavedStationFormatNoName.Assign( StringLoader::LoadL( R_QTN_FMRADIO_WIDGET_STATION_SAVED_FREQ, coeEnv ) );
    iFrequencyFormat.Assign( StringLoader::LoadL( R_QTN_FMRADIO_WIDGET_NOTSAVED_FREQUENCY, coeEnv ) );
    
    iSavedStationFormatMenu.Assign( StringLoader::LoadL( R_QTN_FMRADIO_SUITE_PLAYING_SAVED_WITH_NAME, coeEnv ) );
    iSavedStationFormatNoNameMenu.Assign( StringLoader::LoadL( R_QTN_FMRADIO_SUITE_PLAYING_SAVED_WITHOUT_NAME, coeEnv ) );
    iFrequencyFormatMenu.Assign( StringLoader::LoadL( R_QTN_FMRADIO_SUITE_PLAYING_NOT_SAVED, coeEnv ) );
    
    // Release the resource file, because we don't want to keep file handle open.
    // That would prevent updating the binary with SIS. 
    if ( iResourceOffset > 0 )
        {
        coeEnv->DeleteResourceFile( iResourceOffset );
        iResourceOffset = 0;
        }
    
    TFindFile finder( coeEnv->FsSession() );
    TInt err = finder.FindByDir( KFMRadioMifFileName, KFMRadioMifDir );
    if ( err == KErrNone )
        {
        iMifFileName.CreateL( finder.File() );
        }
    
    iEngine = CFMRadioActiveIdleEngine::NewL( *this );
    iEngine->ActivateL();
    
    FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::ConstructL - Exiting." )));    
    }

// ---------------------------------------------------------------------------
// Initialize resource file for loading resources.
// ---------------------------------------------------------------------------
//
void CFMRadioMCPPlugin::InitializeResourceLoadingL()
    {
    CCoeEnv* coeEnv = CCoeEnv::Static();
    RFs fs( coeEnv->FsSession() );
    TFileName fileName; 
    TFileName baseResource;
    TFindFile finder( fs );
    TLanguage language( ELangNone );
    
    TParsePtrC parse( KFMRadioMCPPluginResourceFileName );
    _LIT( resourceFileWildExt, ".r*" );

    // Make sure to find all resource files, not only .rsc files as it may be so
    // that there is only .r01, .r02, etc. files available
    fileName.Copy( parse.Name() );
    fileName.Append( resourceFileWildExt );

    // TFindFile applies search order that is from 
    // drive Y to A, then Z
    CDir* entries = NULL;
    TInt err = finder.FindWildByDir( fileName , KDC_RESOURCE_FILES_DIR, entries );
    delete entries;
    entries = NULL;
    TBool found = EFalse;
    while ( !found && err == KErrNone )
        {
        // Found file
        fileName.Zero();
        TParsePtrC foundPath( finder.File() );
        fileName.Copy( foundPath.DriveAndPath() );
        fileName.Append( KFMRadioMCPPluginResourceFileName );
        BaflUtils::NearestLanguageFile( fs, fileName, language );
        if ( language != ELangNone && BaflUtils::FileExists( fs, fileName ) )
            {
            found = ETrue;
            iResourceOffset = coeEnv->AddResourceFileL( fileName );
            }
        else
            {
            if ( language == ELangNone &&
                 !baseResource.Compare( KNullDesC ) &&
                 BaflUtils::FileExists( fs, fileName ) )
                {
                baseResource.Copy( fileName );
                }
            err = finder.FindWild( entries );
            delete entries;
            entries = NULL;
            }
        }

    if ( !found && baseResource.Compare( KNullDesC ) )
        {
        // If we found *.rsc then better to use that than nothing
        if ( BaflUtils::FileExists( fs, baseResource ) )
            {
            iResourceOffset = coeEnv->AddResourceFileL( baseResource );
            found = ETrue;
            }
        }

    if( !found )
        {
        User::Leave( KErrNotFound );
        }
    }

void CFMRadioMCPPlugin::Deactivate()
    {
    iActive = EFalse;
    }

void CFMRadioMCPPlugin::ActivateL()
    {
    FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::ActivateL()" )));
    iActive = ETrue;
    iObserver->BecameActiveL( this );
    
    iObserver->PublishTextL( this, EMusicWidgetDefaultText, KNullDesC );

    iObserver->PublishImageL( this, EMusicMenuMusicInfoImage1,
                    KAknsIIDQgnIndiRadioDefault, 
                    iMifFileName, 
                    EMbmFmradioQgn_indi_radio_default, 
                    EMbmFmradioQgn_indi_radio_default_mask);

    iObserver->PublishImageL( this, EMusicWidgetImage1,
                    KAknsIIDQgnIndiRadioDefault, 
                    iMifFileName, 
                    EMbmFmradioQgn_indi_radio_default, 
                    EMbmFmradioQgn_indi_radio_default_mask);

    InstallFMRadioCommandActionL( KFMRadioCommandValueStartNowPlaying, EMusicWidgetTrigger1 );
    InstallFMRadioCommandActionL( KFMRadioCommandValueStartNowPlaying, EMusicMenuMusicInfoTrigger );
    HandleChannelChangeL( iEngine->Channel() );
    }

// ---------------------------------------------------------------------------
// CFMRadioMCPPlugin::UpdatePublishedToolBarL
// ---------------------------------------------------------------------------
//
void CFMRadioMCPPlugin::UpdatePublishedToolBarL( TUint aToolBarState )
    {
    FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdatePublishedToolBarL - Entering." ))); 
    if ( iActive )
        {
        TAknsItemID iconId;
        if ( KFMRadioPrevious & aToolBarState )
            {
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdatePublishedToolBarL - 1 Enter" ))); 
            iconId.Set( EAknsMajorGeneric, EAknsMinorGenericQgnPropImageTbPrev );
            iObserver->PublishImageL( this, 
                                      EMusicWidgetToolbarB1, 
                                      iconId, 
                                      iMifFileName, 
                                      EMbmFmradioQgn_prop_image_tb_prev, 
                                      EMbmFmradioQgn_prop_image_tb_prev_mask );
            InstallFMRadioCommandActionL( KFMRadioCommandValueStepPrevious, EMusicWidgetTB1Trigger );
            iTbPreviousState = KFMRadioPrevious;
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdatePublishedToolBarL - 1 Exit" ))); 
            }

        if ( KFMRadioPreviousDimmed & aToolBarState )
            {
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdatePublishedToolBarL - 2 Enter" )));
            iconId.Set( EAknsMajorGeneric, EAknsMinorGenericQgnPropImageTbPrevDimmed ); 
            iObserver->PublishImageL( this, 
                                      EMusicWidgetToolbarB1, 
                                      iconId,
                                      iMifFileName, 
                                      EMbmFmradioQgn_prop_image_tb_prev_dimmed, 
                                      EMbmFmradioQgn_prop_image_tb_prev_dimmed_mask );
            InstallEmptyActionL( EMusicWidgetTB1Trigger );
            iTbPreviousState = KFMRadioPreviousDimmed; 
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdatePublishedToolBarL - 2 Exit" ))); 
            }
        
        if ( KFMRadioNext & aToolBarState )
            {
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdatePublishedToolBarL - 3 Enter" ))); 
            iconId.Set( EAknsMajorGeneric, EAknsMinorGenericQgnPropImageTbNext );
            iObserver->PublishImageL( this, 
                                      EMusicWidgetToolbarB3, 
                                      iconId, 
                                      iMifFileName, 
                                      EMbmFmradioQgn_prop_image_tb_next, 
                                      EMbmFmradioQgn_prop_image_tb_next_mask );
            InstallFMRadioCommandActionL( KFMRadioCommandValueStepNext, EMusicWidgetTB3Trigger );
            iTbNextState = KFMRadioNext;
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdatePublishedToolBarL - 3 Exit" ))); 
            }
        
        if ( KFMRadioNextDimmed & aToolBarState )
            {
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdatePublishedToolBarL - 4 Enter" ))); 
            iconId.Set( EAknsMajorGeneric, EAknsMinorGenericQgnPropImageTbNextDimmed );
            iObserver->PublishImageL( this, 
                                      EMusicWidgetToolbarB3, 
                                      iconId, 
                                      iMifFileName, 
                                      EMbmFmradioQgn_prop_image_tb_next_dimmed, 
                                      EMbmFmradioQgn_prop_image_tb_next_dimmed_mask );
            InstallEmptyActionL( EMusicWidgetTB3Trigger );
            iTbNextState = KFMRadioNextDimmed;            
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdatePublishedToolBarL - 4 Exit" ))); 
            }
        
        if ( KFMRadioMute & aToolBarState )
            {
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdatePublishedToolBarL - 5 Enter" ))); 
            iconId.Set( EAknsMajorGeneric, EAknsMinorGenericQgnIndiMusicWidgetTbRadioSound );
            iObserver->PublishImageL( this, 
                                      EMusicWidgetToolbarB2, 
                                      iconId, 
                                      iMifFileName, 
                                      EMbmFmradioQgn_indi_music_widget_tb_radio_sound, 
                                      EMbmFmradioQgn_indi_music_widget_tb_radio_sound_mask );
            InstallFMRadioCommandActionL( KFMRadioCommandValueUnmute, EMusicWidgetTB2Trigger );
            iTbMuteUnmuteState = KFMRadioMute;
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdatePublishedToolBarL - 5 Exit" ))); 
            }
        
        if ( KFMRadioUnmute & aToolBarState )
            {
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdatePublishedToolBarL - 6 Enter" ))); 
            iconId.Set( EAknsMajorGeneric, EAknsMinorGenericQgnIndiMusicWidgetTbRadioSoundMuted );
            iObserver->PublishImageL( this,
                                      EMusicWidgetToolbarB2,
                                      iconId,
                                      iMifFileName,
                                      EMbmFmradioQgn_indi_music_widget_tb_radio_sound_muted,
                                      EMbmFmradioQgn_indi_music_widget_tb_radio_sound_muted_mask );
            InstallFMRadioCommandActionL( KFMRadioCommandValueMute, EMusicWidgetTB2Trigger );
            iTbMuteUnmuteState = KFMRadioUnmute;            
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdatePublishedToolBarL - 6 Exit" ))); 
            }
        
        if ( KFMRadioMuteDimmed & aToolBarState )
            {
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdatePublishedToolBarL - 7 Enter" ))); 
            iconId.Set( EAknsMajorGeneric, EAknsMinorGenericQgnIndiMusicWidgetTbRadioSoundDimmed );
            iObserver->PublishImageL( this, 
                                      EMusicWidgetToolbarB2, 
                                      iconId, 
                                      iMifFileName, 
                                      EMbmFmradioQgn_indi_music_widget_tb_radio_sound_dimmed, 
                                      EMbmFmradioQgn_indi_music_widget_tb_radio_sound_dimmed_mask );
            InstallEmptyActionL( EMusicWidgetTB2Trigger );
            iTbMuteUnmuteState = KFMRadioMuteDimmed; 
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdatePublishedToolBarL - 7 Exit" ))); 
            }
        
        if ( KFMRadioUnmuteDimmed & aToolBarState )
            {
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdatePublishedToolBarL - 8 Enter" ))); 
            iconId.Set( EAknsMajorGeneric, EAknsMinorGenericQgnIndiMusicWidgetTbRadioSoundMutedDimmed );
            iObserver->PublishImageL( this, 
                                      EMusicWidgetToolbarB2, 
                                      iconId, 
                                      iMifFileName, 
                                      EMbmFmradioQgn_indi_music_widget_tb_radio_sound_muted_dimmed, 
                                      EMbmFmradioQgn_indi_music_widget_tb_radio_sound_muted_dimmed_mask );
            InstallEmptyActionL( EMusicWidgetTB2Trigger );
            iTbMuteUnmuteState = KFMRadioUnmuteDimmed;
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdatePublishedToolBarL - 8 Exit" ))); 
            }
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioMCPPlugin::UpdateToolBarL
// ---------------------------------------------------------------------------
//
void CFMRadioMCPPlugin::UpdateToolBarL( TBool aForceApplicationClosing )
    {
    if ( iActive )
        {
        TUint muteDimmedState = iEngine->MuteState() == EFMRadioPSMuteStateOn ?
                KFMRadioUnmuteDimmed : KFMRadioMuteDimmed;
        // Update Next and Previous buttons.
        if ( iEngine->AntennaStatus() == EFMRadioPSHeadsetDisconnected )
            {
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdateToolBarL - 1" )));
            UpdatePublishedToolBarL( KFMRadioPreviousDimmed | muteDimmedState |  KFMRadioNextDimmed);
            return;
            }
        else if ( iEngine->ApplicationRunningState() != EFMRadioPSApplicationRunning || aForceApplicationClosing )
            {
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdateToolBarL - 2" )));
            UpdatePublishedToolBarL( KFMRadioPreviousDimmed | muteDimmedState |  KFMRadioNextDimmed);
            return;
            }
        else if ( iEngine->PowerState() != EFMRadioPSRadioPowerOn )
            {
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdateToolBarL - 3" )));
            UpdatePublishedToolBarL( KFMRadioPreviousDimmed | muteDimmedState |  KFMRadioNextDimmed);
            return;
            }
        else if ( iEngine->TuningState() != EFMRadioPSTuningUninitialized )
            {
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdateToolBarL - 4" )));
            UpdatePublishedToolBarL( KFMRadioPreviousDimmed |  KFMRadioNextDimmed );
            }
        else if ( iPresetCount > 1 || ( iPresetCount == 1 && iEngine->Channel() == KErrNotFound ) )
            {
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdateToolBarL - 5" )));
            UpdatePublishedToolBarL( KFMRadioPrevious | KFMRadioNext );
            }
        else
            {
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdateToolBarL - 6" )));
            UpdatePublishedToolBarL( KFMRadioPreviousDimmed | KFMRadioNextDimmed );
            }

        // Update Mute/Unmute button
        if ( iEngine->ApplicationRunningState() != EFMRadioPSApplicationRunning || aForceApplicationClosing )
            {
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdateToolBarL - 7" )));
            UpdatePublishedToolBarL( muteDimmedState );
            }
        else if ( iEngine->TuningState() != EFMRadioPSTuningUninitialized )
            {
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdateToolBarL - 8" )));
            UpdatePublishedToolBarL( muteDimmedState );
            }
        else
            {
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdateToolBarL - 9" )));
            HandleMuteStateChangeL( iEngine->MuteState() );
            }
        }
    }
// ---------------------------------------------------------------------------
// CFMRadioMCPPlugin::UpdateMusicWidgetTextL
// ---------------------------------------------------------------------------
//
void CFMRadioMCPPlugin::UpdateMusicWidgetTextL( TBool aForceApplicationClosing )
    {
    FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdateMusicWidgetTextL" )));
    if ( iActive )
        {
        TInt rightToLeftCharWidth = AknLayoutUtils::LayoutMirrored() ? sizeof(TText16) : 0;
        
        // Homescreen publishing
        if ( iEngine->AntennaStatus() == EFMRadioPSHeadsetDisconnected && !aForceApplicationClosing )
            {
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdateMusicWidgetTextL - Publishing HS antenna not connected: \"%S\""), &iAntennaNotConnectedText ));        
            iObserver->PublishTextL( this, EMusicWidgetText1, iAntennaNotConnectedText );
            }
        else if ( iEngine->TuningState() != EFMRadioPSTuningUninitialized && !aForceApplicationClosing )
            {
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdateMusicWidgetTextL - Publishing HS tuning: \"%S\""), &iTuningText ));        
            iObserver->PublishTextL( this, EMusicWidgetText1, iTuningText );
            }
        else if ( iPresetCount && iEngine->Channel() != KErrNotFound )
            {
            if( iEngine->ChannelName().Length() )
                {
                // Station saved and name is available
                RBuf name;
                // For widget
                // Preserve space for the channel name, format string and maximum index number
                name.CreateL( iEngine->ChannelName().Length() + 
                              iSavedStationFormat.Length() + 
                              KDefaultRealWidth + 
                              rightToLeftCharWidth );
                name.CleanupClosePushL();
                StringLoader::Format( name, iSavedStationFormat, KErrNotFound, iEngine->Channel() + 1 );
                
                HBufC* tempName = name.AllocL();
                StringLoader::Format( name, *tempName, KErrNotFound, iEngine->ChannelName() );
                delete tempName;
                tempName = NULL;
                
                if ( rightToLeftCharWidth )
                    {
                    //E.g. 1. Name -> Name .1
                    name.Insert( 0, KRightToLeftMark );
                    }
                
                FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdateMusicWidgetTextL - Publishing Music Widget saved station: \"%S\""), &name ));
                iObserver->PublishTextL( this, EMusicWidgetText1, name );
                CleanupStack::PopAndDestroy(); // name

                // For Music Suite
                // Preserve space for the channel name, format string and maximum index number
                name.CreateL( iEngine->ChannelName().Length() + 
                              iSavedStationFormatMenu.Length() + 
                              KDefaultRealWidth + 
                              rightToLeftCharWidth );
                name.CleanupClosePushL();
                StringLoader::Format( name, iSavedStationFormatMenu, KErrNotFound, iEngine->Channel() + 1 );
                
                tempName = name.AllocL();
                StringLoader::Format( name, *tempName, KErrNotFound, iEngine->ChannelName() );
                delete tempName;
                tempName = NULL;
                
                if ( rightToLeftCharWidth )
                    {
                    //E.g. 1. Name -> Name .1
                    name.Insert( 0, KRightToLeftMark );
                    }
                
                FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdateMusicWidgetTextL - Publishing Music Suite saved station: \"%S\""), &name ));
                iObserver->PublishTextL( this, EMusicMenuMusicInfoLine2, name );
                CleanupStack::PopAndDestroy(); // name
                }
            else
                {
                // Station saved, but name for it is not available
                HBufC* frequencyString = FrequencyStringLC( iEngine->Frequency() );
                RBuf formattedFrequency;
                
                // For widget
                formattedFrequency.CreateL( iSavedStationFormatNoName.Length() + 
                                            frequencyString->Length() + 
                                            KDefaultRealWidth + 
                                            rightToLeftCharWidth );
                formattedFrequency.CleanupClosePushL();
                StringLoader::Format( formattedFrequency, iSavedStationFormatNoName, KErrNotFound, iEngine->Channel() + 1 );
                
                HBufC* tempFrequency = formattedFrequency.AllocL();
                StringLoader::Format( formattedFrequency, *tempFrequency, KErrNotFound, *frequencyString );
                delete tempFrequency;
                tempFrequency = NULL;
                AknTextUtils::LanguageSpecificNumberConversion( formattedFrequency );

                if ( rightToLeftCharWidth )
                    {
                    //E.g. 1. Name -> Name .1
                    formattedFrequency.Insert( 0, KRightToLeftMark );
                    }

                FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdateMusicWidgetTextL - Publishing Music Widget saved station with frequency: \"%S\""), &formattedFrequency ));
                iObserver->PublishTextL( this, EMusicWidgetText1, formattedFrequency );
                CleanupStack::PopAndDestroy(); // formattedFrequency
                
                // For Music Suite
                formattedFrequency.CreateL( iSavedStationFormatNoNameMenu.Length() + 
                                            frequencyString->Length() + 
                                            KDefaultRealWidth + 
                                            rightToLeftCharWidth );
                formattedFrequency.CleanupClosePushL();
                StringLoader::Format( formattedFrequency, iSavedStationFormatNoNameMenu, KErrNotFound, iEngine->Channel() + 1 );
                
                tempFrequency = formattedFrequency.AllocL();
                StringLoader::Format( formattedFrequency, *tempFrequency, KErrNotFound, *frequencyString );
                delete tempFrequency;
                tempFrequency = NULL;
                
                AknTextUtils::LanguageSpecificNumberConversion( formattedFrequency );
                if ( rightToLeftCharWidth )
                    {
                    //E.g. 1. Name -> Name .1
                    formattedFrequency.Insert( 0, KRightToLeftMark );
                    }

                FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdateMusicWidgetTextL - Publishing Music Suite saved station with frequency: \"%S\""), &formattedFrequency ));
                iObserver->PublishTextL( this, EMusicMenuMusicInfoLine2, formattedFrequency );
                CleanupStack::PopAndDestroy( 2, frequencyString ); // formattedFrequency, frequencyString
                }
            }
        else if ( iEngine->RDSProgramService().Length() > 0 )
            {
            FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdateMusicWidgetTextL - Publishing HS PS name: \"%S\""), &iEngine->RDSProgramService() ));
            iObserver->PublishTextL( this, EMusicWidgetText1, iEngine->RDSProgramService() );
            iObserver->PublishTextL( this, EMusicMenuMusicInfoLine2, iEngine->RDSProgramService() );
            }
        else
            {
            TInt currentFrequency = iEngine->Frequency();
            // publish only valid frequency
            if ( currentFrequency > 0 )
                {
                HBufC* frequencyString = FrequencyStringLC( currentFrequency );
                
                RBuf formattedFrequency;
                // For widget
                // Preserve space for the format string and maximum index number
                formattedFrequency.CreateL( iFrequencyFormat.Length() + 
                                            frequencyString->Length() + 
                                            rightToLeftCharWidth );
                formattedFrequency.CleanupClosePushL();
                StringLoader::Format( formattedFrequency, iFrequencyFormat, KErrNotFound, *frequencyString );
                AknTextUtils::LanguageSpecificNumberConversion( formattedFrequency );
               
                if ( rightToLeftCharWidth )
                    {
                    //E.g. 1. Name -> Name .1
                    formattedFrequency.Insert( 0, KRightToLeftMark );
                    }
                
                FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdateMusicWidgetTextL - Publishing Music Widget frequency: \"%S\""), &formattedFrequency ));
                iObserver->PublishTextL( this, EMusicWidgetText1, formattedFrequency );
                CleanupStack::PopAndDestroy(); // formattedFrequency
                
                // For Music Suite
                // Preserve space for the format string and maximum index number
                formattedFrequency.CreateL( iFrequencyFormatMenu.Length() + 
                                            frequencyString->Length() + 
                                            rightToLeftCharWidth );
                formattedFrequency.CleanupClosePushL();
                StringLoader::Format( formattedFrequency, iFrequencyFormatMenu, KErrNotFound, *frequencyString );
                AknTextUtils::LanguageSpecificNumberConversion( formattedFrequency );
               
                if ( rightToLeftCharWidth )
                    {
                    //E.g. 1. Name -> Name .1
                    formattedFrequency.Insert( 0, KRightToLeftMark );
                    }
                
                FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::UpdateMusicWidgetTextL - Publishing Music Suite frequency: \"%S\""), &formattedFrequency ));
                iObserver->PublishTextL( this, EMusicMenuMusicInfoLine2, formattedFrequency );
                CleanupStack::PopAndDestroy( 2, frequencyString ); // formattedFrequency, frequencyString
                }
            else
                {
                // Publish empty text instead of 0.00 MHz
                iObserver->PublishTextL( this, EMusicWidgetText1, KNullDesC );
                iObserver->PublishTextL( this, EMusicMenuMusicInfoLine2, KNullDesC );
                }
            }
        if( iEngine->PowerState() == EFMRadioPSRadioPowerOn && !aForceApplicationClosing )
            {
            iObserver->PublishTextL( this, EMusicMenuMusicInfoLine1, iNowPlayingText );
            }
        else
            {
            iObserver->PublishTextL( this, EMusicMenuMusicInfoLine1, iLastPlayedText );
            }
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioMCPPlugin::FrequencyStringLC
// ---------------------------------------------------------------------------
//
HBufC* CFMRadioMCPPlugin::FrequencyStringLC( TInt aFrequency )
    {
    TReal frequency = static_cast<TReal>( aFrequency / static_cast<TReal>( KHzConversionFactor ));
    TRealFormat format( KFrequencyMaxLength, iEngine->FrequencyDecimalCount() );
    TBuf<KDefaultRealWidth> frequencyString;
    frequencyString.Num( frequency, format );
    
    return frequencyString.AllocLC();
    }

void CFMRadioMCPPlugin::HandleRadioVolumeChangeL( TInt /* aVolume */ )
    {
    }

void CFMRadioMCPPlugin::HandleTuningStateChangeL( TFMRadioPSTuningState FDEBUGVAR( aTuningState ) )
    {
    FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::HandleTuningStateChangeL: aTuningState=[%d]"), aTuningState));

    if ( iActive )
        {
        UpdateMusicWidgetTextL();
        UpdateToolBarL();
        }
 
    FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::HandleTuningStateChangeL - Exiting." )));    
    }

void CFMRadioMCPPlugin::HandleFrequencyChangeL( TInt FDEBUGVAR( aFrequency ) )
    {
    FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::HandleFrequencyChangeL: aFrequency=%d"), aFrequency));
    }

void CFMRadioMCPPlugin::HandleChannelChangeL( TInt FDEBUGVAR( aId ) )
    {
    FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::HandleChannelChangeL: aId=[%d]"), aId ));  
    if ( iActive )
        {
        UpdateMusicWidgetTextL();
        UpdateToolBarL();
        }
    FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::HandleChannelChangeL - Exiting." ))); 
    }

void CFMRadioMCPPlugin::HandleChannelModifyL( TInt aId )
    {
    FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::HandleChannelModifyL: aId=[%d]"), aId ));
    if ( iActive && aId == iEngine->Channel() )
        {
        UpdateMusicWidgetTextL();
        }
    FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::HandleChannelModifyL - Exiting." )));
    }

void CFMRadioMCPPlugin::HandleMuteStateChangeL( TFMRadioPSRadioMuteState aMuteState )
    {
    if ( iActive )
        {
        switch( aMuteState )
            {
            case EFMRadioPSMuteStateOn:
                if ( iEngine->ApplicationRunningState() != EFMRadioPSApplicationRunning )
                    {
                    UpdatePublishedToolBarL( KFMRadioUnmuteDimmed );
                    }
                else
                    {
                    UpdatePublishedToolBarL( KFMRadioUnmute );
                    }
                break;
            case EFMRadioPSMuteStateOff:
            case EFMRadioPSMuteStateUninitialized:
                if ( iEngine->ApplicationRunningState() != EFMRadioPSApplicationRunning )
                    {
                    UpdatePublishedToolBarL( KFMRadioMuteDimmed );
                    }
                 else
                    {
                    UpdatePublishedToolBarL( KFMRadioMute );
                    }
                break;
            default:break;
            }
        }    
    }

void CFMRadioMCPPlugin::HandleApplicationRunningStateChangeL( TFMRadioPSApplicationRunningState aRunningState )
    {
    FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::HandleApplicationRunningStateChangeL - aRunningState=%d"), aRunningState ));
    
    switch ( aRunningState )
        {
        case EFMRadioPSApplicationRunning:
            break;
        case EFMRadioPSApplicationClosing:
            UpdateMusicWidgetTextL( ETrue );
            UpdateToolBarL( ETrue );
            Deactivate();
            break;
        default:
            break;
        }
    FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::HandleApplicationRunningStateChangeL - Exiting." )));
    }

void CFMRadioMCPPlugin::HandleFrequencyDecimalCountChangeL( TFMRadioPSFrequencyDecimalCount /* aDecimalCount */ )
    {
    FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::HandleFrequencyDecimalCountChangeL." )));    
    if ( iActive )
        {
        UpdateMusicWidgetTextL();
        }
    FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::HandleFrequencyDecimalCountChangeL - Exiting." )));  
    }

void CFMRadioMCPPlugin::HandleAntennaStatusChangeL( TFMRadioPSHeadsetStatus /*aAntennaStatus*/ )
    {
    FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::HandleAntennaStatusChangeL - Enter")));    
    if ( iActive )
        {
        UpdateMusicWidgetTextL();
        UpdateToolBarL();
        }
    FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::HandleAntennaStatusChangeL - Exiting.")));    
    }

void CFMRadioMCPPlugin::HandlePresetListCountChangeL( TInt aPresetCount )
    {
    FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::HandlePresetListCountChangeL(%d) - Enter"), aPresetCount));    
    iPresetCount = aPresetCount;
    if ( iActive )
        {
        if( !aPresetCount )
            {
            UpdateMusicWidgetTextL();
            }
        UpdateToolBarL();
        }
    FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::HandlePresetListCountChangeL - Exiting.")));    
    }

void CFMRadioMCPPlugin::HandlePresetListFocusChangeL( TInt FDEBUGVAR( aPresetFocus ) )
    {
    FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::HandlePresetListFocusChangeL - %d"), aPresetFocus ));    
    }

void CFMRadioMCPPlugin::HandleRDSProgramServiceChangeL( const TDesC& /*aProgramService*/ )
    {
    FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::HandleRDSProgramServiceChangeL." )));    
    if ( iActive )
        {
        UpdateMusicWidgetTextL();
        }
    FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::HandleRDSProgramServiceChangeL - Exiting." )));    
    }

void CFMRadioMCPPlugin::HandleRDSRadioTextChangeL( const TDesC& /*aRdsText*/ )
    {
    }

void CFMRadioMCPPlugin::HandleChannelNameChangeL( const TDesC& /*aName*/ )
    {
    FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::HandleChannelNameChangeL." )));    
    }

void CFMRadioMCPPlugin::HandlePowerStateChangeL( TFMRadioPSRadioPowerState aPowerState )
    {
    FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::HandlePowerStateChangeL(%d)."), aPowerState ));
    if( aPowerState == EFMRadioPSRadioPowerOn )
        {
        ActivateL();
        }
    if ( iActive )
        {
        UpdateMusicWidgetTextL();
        UpdateToolBarL();
        }
    FTRACE(FPrint(_L(" *** S60 FMRadio -- CFMRadioMCPPlugin::HandlePowerStateChangeL - Exiting." ))); 
    }

void CFMRadioMCPPlugin::InstallEmptyActionL( TMCPTriggerDestination aDestination )
    {
    CLiwDefaultMap* mapTrigger = CLiwDefaultMap::NewLC();
    iObserver->PublishActionL( this, aDestination, mapTrigger );
    CleanupStack::PopAndDestroy( mapTrigger );
    }

void CFMRadioMCPPlugin::InstallFMRadioCommandActionL( const TDesC& aCommand, TMCPTriggerDestination aDestination )
    {
    CLiwDefaultMap* mapTrigger = CLiwDefaultMap::NewLC();
    CLiwDefaultMap* mapData = CLiwDefaultMap::NewLC();

    mapTrigger->InsertL( KFMRadioPluginUid, TLiwVariant( TUid::Uid( KFMRadioActionHandlerImplementationUid ) ) );
    mapData->InsertL( KType, TLiwVariant( aCommand ) );
    mapData->InsertL( KFMRadioCommand , TLiwVariant( aCommand ) );
    mapTrigger->InsertL( KFMRadioData, TLiwVariant( mapData ) );
    
    if ( iActive )
        {
        iObserver->PublishActionL( this, aDestination, mapTrigger );
        }
    
    CleanupStack::PopAndDestroy( mapData );
    CleanupStack::PopAndDestroy( mapTrigger );
    }

/** Provides a key-value pair table, that is used to identify the correct construction function for the requested interface. */
const TImplementationProxy KFMRadioMCPPluginImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KFMRadioMCPPluginImplementationUid, CFMRadioMCPPlugin::NewL )
    };

const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount );

// ---------------------------------------------------------------------------
// Returns the implementations provided by this ECOM plugin DLL.
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( KFMRadioMCPPluginImplementationTable ) / sizeof( TImplementationProxy );
    return KFMRadioMCPPluginImplementationTable;
    }
