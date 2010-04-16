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

// System includes
#include <e32cmn.h>
#include <UiklafInternalCRKeys.h>

// User includes
#include "cradioaccessoryobserver.h"
#include "cradiorepositorymanager.h"
#include "cradiosystemeventcollectorimp.h"
#include "cradiosystemeventdetector.h"
#include "radioengineutils.h"
#include "cradioenginelogger.h"

const TInt KVRObserverArrayGranularity( 2 );

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioSystemEventCollectorImp::CRadioSystemEventCollectorImp() :
    iSystemEventObservers( KVRObserverArrayGranularity )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioSystemEventCollectorImp::ConstructL()
    {
    RadioEngineUtils::InitializeL();
    iSystemEventDetector = CRadioSystemEventDetector::NewL( *this );

    iHeadsetObserver = CRadioAccessoryObserver::NewL();
    iHeadsetObserver->SetObserver( this );

    TInt criticalLevel;
    CRadioRepositoryManager::GetRepositoryValueL( KCRUidUiklaf,
        KUikOODDiskCriticalThreshold, criticalLevel );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioSystemEventCollectorImp* CRadioSystemEventCollectorImp::NewL()
    {
    CRadioSystemEventCollectorImp* self = new( ELeave ) CRadioSystemEventCollectorImp;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioSystemEventCollectorImp::~CRadioSystemEventCollectorImp()
    {
    delete iSystemEventDetector;
    delete iHeadsetObserver;

    iSystemEventObservers.Close();
    RadioEngineUtils::Release();
    }

// ---------------------------------------------------------------------------
// Adds a system event observer
// ---------------------------------------------------------------------------
//
void CRadioSystemEventCollectorImp::AddObserverL( MRadioSystemEventObserver* aHeadsetObserver )
    {
    iSystemEventObservers.AppendL( aHeadsetObserver );
    }

// ---------------------------------------------------------------------------
// Removes a system event observer
// ---------------------------------------------------------------------------
//
 void CRadioSystemEventCollectorImp::RemoveObserver( MRadioSystemEventObserver* aSystemObserver )
    {
    TInt objectIndex = iSystemEventObservers.Find( aSystemObserver );

    if ( objectIndex != KErrNotFound )
        {
        iSystemEventObservers.Remove( objectIndex );
        }
    }

// ---------------------------------------------------------------------------
// Getter for mobile network state
// ---------------------------------------------------------------------------
//
TBool CRadioSystemEventCollectorImp::IsMobileNetworkCoverage() const
    {
    return iSystemEventDetector->IsMobileNetworkCoverage();
    }

// ---------------------------------------------------------------------------
// Getter for network state
// ---------------------------------------------------------------------------
//
TBool CRadioSystemEventCollectorImp::IsNetworkCoverage() const
    {
    return iSystemEventDetector->IsNetworkCoverage();
    }

// ---------------------------------------------------------------------------
// Getter for call state
// ---------------------------------------------------------------------------
//
TBool CRadioSystemEventCollectorImp::IsCallActive() const
    {
    return iSystemEventDetector->IsCallActive();
    }

// ---------------------------------------------------------------------------
// Getter for audio resource state
// ---------------------------------------------------------------------------
//
TBool CRadioSystemEventCollectorImp::IsAudioResourcesAvailable() const
    {
    return iSystemEventDetector->IsAudioResourcesAvailable();
    }

// ---------------------------------------------------------------------------
// Getter for headset connection status
// ---------------------------------------------------------------------------
TBool CRadioSystemEventCollectorImp::IsHeadsetConnectedL() const
    {
    return iHeadsetObserver->IsHeadsetConnectedL();
    }

// ---------------------------------------------------------------------------
// Getter Vocie UI state
// ---------------------------------------------------------------------------
TBool CRadioSystemEventCollectorImp::IsVoiceUiActive() const
    {
    return iSystemEventDetector->IsVoiceUiActive();
    }

// ---------------------------------------------------------------------------
// Notifies the observers of system event
// ---------------------------------------------------------------------------
//
void CRadioSystemEventCollectorImp::NotifyObserversL( TRadioSystemEventType aEvent )
    {
    for ( TInt i = 0; i < iSystemEventObservers.Count(); ++i )
        {
        iSystemEventObservers[i]->HandleSystemEventL( aEvent );
        }
    }

// ---------------------------------------------------------------------------
// From class MRadioSystemEventDetectorObserver.
// ---------------------------------------------------------------------------
//
void CRadioSystemEventCollectorImp::NetworkUpCallbackL()
    {
    NotifyObserversL( ERadioNetworkCoverageUp );
    }

// ---------------------------------------------------------------------------
// From class MRadioSystemEventDetectorObserver.
// ---------------------------------------------------------------------------
//
void CRadioSystemEventCollectorImp::NetworkDownCallbackL()
    {
    NotifyObserversL( ERadioNetworkCoverageDown );
    }

// ---------------------------------------------------------------------------
// From class MRadioSystemEventDetectorObserver.
// ---------------------------------------------------------------------------
//
void CRadioSystemEventCollectorImp::CallActivatedCallbackL()
    {
    NotifyObserversL( ERadioCallActivated );
    }

// ---------------------------------------------------------------------------
// From class MRadioSystemEventDetectorObserver.
// ---------------------------------------------------------------------------
//
void CRadioSystemEventCollectorImp::CallDeactivatedCallbackL()
    {
    NotifyObserversL( ERadioCallDeactivated );
    }

// ---------------------------------------------------------------------------
// From class MRadioSystemEventDetectorObserver.
// ---------------------------------------------------------------------------
//
void CRadioSystemEventCollectorImp::AudioResourcesAvailableL()
    {
    NotifyObserversL( ERadioAudioResourcesAvailable );
    }

// ---------------------------------------------------------------------------
// From class MRadioSystemEventDetectorObserver.
// ---------------------------------------------------------------------------
//
void CRadioSystemEventCollectorImp::AudioAutoResumeForbiddenL()
    {
    NotifyObserversL( ERadioAudioAutoResumeForbidden );
    }

// ---------------------------------------------------------------------------
// From class MRadioSystemEventDetectorObserver.
// ---------------------------------------------------------------------------
//
void CRadioSystemEventCollectorImp::ErrorCallbackL( TInt DEBUGVAR( aError ) )
    {
    LOG_FORMAT( "CRadioSystemEventCollectorImp::ErrorCallbackL - %d", aError );
    // P&S get fail not considered as a critical issue.
    }

// ---------------------------------------------------------------------------
// From class MRadioHeadsetEventObserver.
// ---------------------------------------------------------------------------
//
void CRadioSystemEventCollectorImp::HeadsetConnectedCallbackL()
    {
    NotifyObserversL( ERadioHeadsetConnected );
    }

// ---------------------------------------------------------------------------
// From class MRadioHeadsetEventObserver.
// ---------------------------------------------------------------------------
//
void CRadioSystemEventCollectorImp::HeadsetDisconnectedCallbackL()
    {
    NotifyObserversL( ERadioHeadsetDisconnected );
    }

// ---------------------------------------------------------------------------
// From class MRadioAudioRoutingObserver.
// ---------------------------------------------------------------------------
//
void CRadioSystemEventCollectorImp::AudioRouteChangedL( RadioEngine::TRadioAudioRoute aRoute )
    {
    TRadioSystemEventType ev = ERadioAudioRouteHeadset;
    if ( aRoute != RadioEngine::ERadioHeadset )
        {
        ev = ERadioAudioRouteSpeaker;
        }

    NotifyObserversL( ev );
    }
