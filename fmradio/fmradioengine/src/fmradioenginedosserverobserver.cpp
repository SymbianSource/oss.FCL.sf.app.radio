/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains implementation of the CDosServerObserver class. which is the 
*                       notification interface to the DOS Server.
*
*/


//  INCLUDES
#include <DosSvrServices.h>

#include "debug.h"
#include "fmradioengine.h"
#include "fmradioengineradiosettings.h"
#include "fmradioenginedosserverobserver.h"

// ----------------------------------------------------
// CDosServerObserver::CDosServerObserver
// Default class constructor.
// ----------------------------------------------------
//
CDosServerObserver::CDosServerObserver(CRadioEngine* aEngine, TRadioSettings* aRadioSettings)
                    : iEngine(aEngine), iRadioSettings(aRadioSettings)
    {
    }

// ----------------------------------------------------
// CDosServerObserver::NewL
// Two-phased class constructor.
// ----------------------------------------------------
//
CDosServerObserver *CDosServerObserver::NewL(CRadioEngine* aEngine, TRadioSettings* aRadioSettings)
    {
    CDosServerObserver *self = new (ELeave)CDosServerObserver(aEngine, aRadioSettings);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }


// ----------------------------------------------------
// CDosServerObserver::ConstructL
// Second phase class constructor.
// ----------------------------------------------------
//
void CDosServerObserver::ConstructL()
    {
	iRadioSettings->SetHeadsetConnected();
    RDosServer* server = const_cast< RDosServer* >( &DosServer() );
	User::LeaveIfError( iDosAudio.Open(*server) );
	TRegisterEvent eventList[] = {{KHandsfreeModeChanged, sizeof(EPSHandsFreeMode),EOnlyLast}, {KHeadsetButtonChanged,sizeof(EPSButtonState),	EQueue} };
	StartListeningL(eventList,sizeof(eventList)/sizeof(TRegisterEvent));  
    }

// ----------------------------------------------------
// CDosServerObserver::~CDosServerObserver
// Destructor of CDosServerObserver class.
// ----------------------------------------------------
//
CDosServerObserver::~CDosServerObserver()
    {
    Stop(); 
    }


// ---------------------------------------------------------
// CDosServerObserver::SetAudioRouting
// ---------------------------------------------------------
//
TInt CDosServerObserver::SetAudioRouting(CRadioEngine::TFMRadioAudioOutput aAudioOutput)
	{
	TInt err(KErrNone);
	EPSHandsFreeMode mode = (aAudioOutput == CRadioEngine::EFMRadioOutputHeadset) ? EPSIhfOff : EPSIhfOn;
	err = iDosAudio.SetHandsfreeMode(mode);
	return err;
	}
	
// ---------------------------------------------------------
// CDosServerObserver::HeadsetButtonChangedL
// Handling notification about headset button state change.
// (other items were commented in a header).
// Status : Draft/Proposal/Approved
// ---------------------------------------------------------
//
void CDosServerObserver::HeadsetButtonChangedL(EPSButtonState aState)
   {
   if(aState == EPSButtonUp)
        {
        iEngine->HandleHeadsetButtonPress();
        }

   }
// end of file
