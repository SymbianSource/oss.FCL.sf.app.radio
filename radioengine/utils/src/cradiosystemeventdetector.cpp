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
* Description:
*
*/

#include <e32def.h> // Define before audiopolicy defs
#include <e32std.h> // Define before audiopolicy defs
#include <eikdef.h>

#ifndef __WINS__

#include <audiopolicypubsubdata.h>
#include <audiosw_pubsubkeys.h>

#endif //__WINS__

#include <ctsydomainpskeys.h>
#include <publicruntimeids.hrh>
#include <sacls.h>

#ifdef COMPILE_IN_IVALO
#   include <voiceuidomainpskeys.h>
#endif //COMPILE_IN_IVALO
#include <featmgr.h>

// User includes
#include "cradioenginelogger.h"
#include "cradiosystemeventdetector.h"
#include "mradiosystemeventdetectorobserver.h"

/** Granularity for audio category arrays. */
const TInt KVRAudioCategoryArrayGranularity = 3;

// This has to be the last include. 
#ifdef STUB_CONSTELLATION
#   include "RadioStubManager.h"
#   define KUidSystemCategory KStub_KUidSystemCategory
#   define KPSUidCtsyCallInformation KStub_KPSUidCtsyCallInformation
#   define KPSUidVoiceUiAccMonitor KStub_KPSUidVoiceUiAccMonitor
#   define KPSUidMMFAudioServer KStub_KPSUidMMFAudioServer
#endif //STUB_CONSTELLATION

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioSystemEventDetector::CRadioSystemEventDetector( MRadioSystemEventDetectorObserver&  aObserver )
    : iObserver( aObserver )
    , iIsMobileNetworkCoverage( EFalse )
    , iIsWlanCoverage( EFalse )
    , iIsCallActive( EFalse )
    , iIsAudioResourcesAvailable( ETrue )
    , iIsVoiceUiActive( EFalse )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioSystemEventDetector::ConstructL()
    {
//    FeatureManager::InitializeLibL();
//    TBool wlanSupported = FeatureManager::FeatureSupported( KFeatureIdProtocolWlan );
//    FeatureManager::UnInitializeLib();
    TBool wlanSupported = EFalse; //TODO: Check if we have any need for this

    iNetworkStatusObserver = CRadioPropertyObserver::NewL( *this,
                                                           KUidSystemCategory,
                                                           KUidNetworkStatusValue,
                                                           CRadioPropertyObserver::ERadioPropertyInt );
    iNetworkStatusObserver->ActivateL();

    iIsMobileNetworkCoverage = iNetworkStatusObserver->ValueInt() == ESANetworkAvailable;

    // On S60 platform, there is no guaranteed way of seeing whether WLAN is explicitly turned off
    // in the settings, or whether the network is available. For now, we only check the existence of
    // the WLAN support in the feature manager. We might also want to check whether WLAN access points
    // have been configured.
    iIsWlanCoverage = wlanSupported;

    // Initialize call state observer.
    iCallStatusObserver = CRadioPropertyObserver::NewL( *this,
                                                        KPSUidCtsyCallInformation,
                                                        KCTsyCallState,
                                                        CRadioPropertyObserver::ERadioPropertyInt );
    iCallStatusObserver->ActivateL();
    iIsCallActive = iCallStatusObserver->ValueInt() != EPSCTsyCallStateNone;

#ifdef COMPILE_IN_IVALO
    // Initialize voice ui observer.
    iVoiceUiObserver = CRadioPropertyObserver::NewL( *this,
                                                     KPSUidVoiceUiAccMonitor,
                                                     KVoiceUiOpenKey,
                                                     CRadioPropertyObserver::ERadioPropertyInt );
    iVoiceUiObserver->ActivateL();
#endif //COMPILE_IN_IVALO

#ifndef __WINS__
    // Define audio types for not resuming.
    iNoAutoResumeAudioCategories = RArray<TInt>( KVRAudioCategoryArrayGranularity );
    iNoAutoResumeAudioCategories.AppendL( ECatMediaPlayer );
    iNoAutoResumeAudioCategories.AppendL( ECatMobileTv );
    iNoAutoResumeAudioCategories.AppendL( ECatUnknownPlayer );
    iNoAutoResumeAudioCategories.Compress();
    // Start listening audio client events.
    iAudioPolicyObserver = CRadioPropertyObserver::NewL( *this, KPSUidMMFAudioServer, KAudioPolicyAudioClients, CRadioPropertyObserver::ERadioPropertyByteArray );
    iAudioPolicyObserver->ActivateL();
#endif
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioSystemEventDetector::~CRadioSystemEventDetector()
    {
    FeatureManager::UnInitializeLib();
    delete iCallStatusObserver;
    delete iNetworkStatusObserver;
    delete iVoiceUiObserver;
    delete iAudioPolicyObserver;

    iNoAutoResumeAudioCategories.Close();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioSystemEventDetector* CRadioSystemEventDetector::NewL( MRadioSystemEventDetectorObserver&  aObserver )
    {
    CRadioSystemEventDetector* self = new ( ELeave ) CRadioSystemEventDetector( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Handling of the int property changes is done here.
// Observer components are getting notifications in correspondence with what
// has changed
// ---------------------------------------------------------------------------
//
void CRadioSystemEventDetector::HandlePropertyChangeL( const TUid& aCategory,
                                                       const TUint aKey,
                                                       const TInt aValue )
    {
    //TODO: Refactor
    if ( aCategory == KUidSystemCategory && aKey == KUidNetworkStatusValue )
        {
        switch ( aValue )
            {
            case ESANetworkAvailable:
                {
                SetNetworkCoverageL( ETrue, iIsWlanCoverage );
                break;
                }
            case ESANetworkUnAvailable:
                {
                SetNetworkCoverageL( EFalse, iIsWlanCoverage );
                break;
                }
            default:
                {
                break;
                }
            }
        }
    else if ( aCategory == KPSUidCtsyCallInformation && aKey == KCTsyCallState )
        {
        if ( (!iIsCallActive ) && ( aValue > EPSCTsyCallStateNone ))
            {
            iIsCallActive = ETrue;
            iObserver.CallActivatedCallbackL();
            }
        else if ( ( iIsCallActive ) && ( aValue <= EPSCTsyCallStateNone ))
            {
            iIsCallActive = EFalse;
            iObserver.CallDeactivatedCallbackL();
            }
        else
            {
            // No change
            }
        }
#ifdef COMPILE_IN_IVALO
    else if ( aCategory == KPSUidVoiceUiAccMonitor && aKey == KVoiceUiOpenKey )
        {
        switch ( aValue )
            {
            case KVoiceUiIsClose:
                {
                if ( iIsVoiceUiActive )
                    {
                    iIsVoiceUiActive = EFalse;
                    LOG( "Voice UI not active." );
                    if ( iIsAudioResourcesAvailable )
                        {
                        LOG( "Audio resources available. Change informed." );
                        iObserver.AudioResourcesAvailableL();
                        }
                    }
                break;
                }
            case KVoiceUiIsOpen:
                {
                if ( !iIsVoiceUiActive )
                    {
                    iIsVoiceUiActive = ETrue;
                    LOG( "Voice UI active." );
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        }
#endif //COMPILE_IN_IVALO
    else // NOP
        {
        }
    }

#ifndef __WINS__
// ---------------------------------------------------------------------------
// Handling of the byte array property changes is done here.
// Observer components are getting notifications in correspondence with what
// has changed
// ---------------------------------------------------------------------------
//
void CRadioSystemEventDetector::HandlePropertyChangeL( const TUid& aCategory,
                                                       const TUint aKey,
                                                       const TDesC8& aValue )
    {
#if 0
    LOG_METHOD_AUTO;
    LOG_FORMAT( "Category: %d, Key: %d", aCategory, aKey );
    if ( aCategory == KPSUidMMFAudioServer )
        {
        if ( aKey == KAudioPolicyAudioClients )
            {
            TBool atLeastOneAutoResumeAudioPlaying( EFalse );
            TBool atLeastOneNoAutoResumeAudioPlaying( EFalse );
            TBool radioPlaying( EFalse );
            TAudioClientList audioClients;
            audioClients.Copy( aValue );
            // Check all playing audios!
            for ( TInt i = 0; i < audioClients().iNumOfProcesses ; i++ )
                {
                TInt cat = audioClients().iClientCategoryList[i];
                LOG_FORMAT( "Check audio cat %x", cat );
                if ( cat == ECatFmRadio )
                    {
                    radioPlaying = ETrue;
                    }
                else if ( iNoAutoResumeAudioCategories.Find( cat ) != KErrNotFound )
                    {
                    atLeastOneNoAutoResumeAudioPlaying = ETrue;
                    }
                else
                    {
                    atLeastOneAutoResumeAudioPlaying = ETrue;
                    }
                }

            if ( !radioPlaying )
                {
                // Decide audio resource availability from audio category info.
                if ( atLeastOneNoAutoResumeAudioPlaying )
                    {
                    LOG( "Audio resources not available. Change informed." );
                    iIsAudioResourcesAvailable = EFalse;
                    iObserver.AudioAutoResumeForbiddenL();
                    }
                else if ( !atLeastOneAutoResumeAudioPlaying )
                    {
                    if ( !iIsVoiceUiActive )
                        {
                        LOG( "Audio resources available. Change informed." );
                        iIsAudioResourcesAvailable = ETrue;
                        iObserver.AudioResourcesAvailableL();
                        }
                    else
                        {
                        LOG( "Audio resources available. Change not informed." );
                        iIsAudioResourcesAvailable = ETrue;
                        }
                    }
                else
                    {
                    LOG( "Audio resources not available. Change not informed." );
                    iIsAudioResourcesAvailable = EFalse;
                    }
                }
            else // audio resources are considered to be available when radio is playing
                {
                iIsAudioResourcesAvailable = ETrue;
                }
            }
        }
#endif
    }

#else //__WINS__
// ---------------------------------------------------------------------------
// Dummy version for WINS in order to avoid compiler warnings.
// The real implementation of function is above.
// ---------------------------------------------------------------------------
//
void CRadioSystemEventDetector::HandlePropertyChangeL( const TUid& /*aCategory*/,
                                                       const TUint /*aKey*/,
                                                        const TDesC8& /*aValue*/)
    {
    }
#endif

// ---------------------------------------------------------------------------
// Handling of the text property changes is done here.
// Observer components are getting notifications in correspondence with what
// has changed
// ---------------------------------------------------------------------------
//
void CRadioSystemEventDetector::HandlePropertyChangeL( const TUid& /*aCategory*/,
                                                       const TUint /*aKey*/,
                                                       const TDesC& /*aValue*/)
    {
    }

// ---------------------------------------------------------------------------
// This is a callback function which is called when a P&S components returns
// an error
// ---------------------------------------------------------------------------
//
void CRadioSystemEventDetector::HandlePropertyChangeErrorL( const TUid& aCategory,
                                                            const TUint aKey,
                                                            TInt aError )
    {
#ifdef COMPILE_IN_IVALO
    if ( aCategory == KPSUidVoiceUiAccMonitor && aKey == KVoiceUiOpenKey && aError == KErrNotFound )
        {
        HandlePropertyChangeL( KPSUidVoiceUiAccMonitor, KVoiceUiOpenKey, KVoiceUiIsClose );
        }
    else
        {
        iObserver.ErrorCallbackL( aError );
        }
#else
    (void)aCategory;
    (void)aKey;
    (void)aError;
#endif //COMPILE_IN_IVALO
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioSystemEventDetector::IsMobileNetworkCoverage() const
    {
    return iIsMobileNetworkCoverage;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioSystemEventDetector::IsNetworkCoverage() const
    {
    return iIsWlanCoverage || iIsMobileNetworkCoverage;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioSystemEventDetector::IsCallActive() const
    {
    return iIsCallActive;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioSystemEventDetector::IsAudioResourcesAvailable() const
    {
    return iIsAudioResourcesAvailable;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioSystemEventDetector::IsVoiceUiActive() const
    {
    return iIsVoiceUiActive;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioSystemEventDetector::SetNetworkCoverageL( const TBool aIsMobileNetworkCoverage,
                                                     const TBool aIsWlanCoverage )
    {
    LOG_FORMAT( "CRadioSystemEventDetector::SetNetworkCoverageL ( mobile = %d wlan = %d )", aIsMobileNetworkCoverage, aIsWlanCoverage );

    TBool wasCoverage = IsNetworkCoverage();
    iIsMobileNetworkCoverage = aIsMobileNetworkCoverage;
    iIsWlanCoverage = aIsWlanCoverage;
    TBool isCoverage = IsNetworkCoverage();

    if ( isCoverage != wasCoverage )
        {
        if ( isCoverage )
            {
            iObserver.NetworkUpCallbackL();
            }
        else
            {
            iObserver.NetworkDownCallbackL();
            }
        }
    }
