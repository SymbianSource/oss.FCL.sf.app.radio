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
* Description:  The class implements functions handling radio engine state changes 
*
*/


#ifndef FMRADIOENGINERADIOSTATEHANDLER_H
#define FMRADIOENGINERADIOSTATEHANDLER_H

//INCLUDES
#include "fmradioenginestatechangecallback.h"

// CONSTANTS
const TUint8 KRadioStateEventCallback     = 0x01;

// FORWARD DECLARATIONS
class CRadioEngine;


// CLASS DECLARATION
/**
* This class implements the state machines for radio engine state transition.
*/
class CRadioStateHandler : public CActive
    {
    public:
        /**
        * Method for creating new instances of this class.
        * @param aEngine a pointer to CRadioEngine
        * @return - A pointer to new CRadioEngineTimer object
        **/
        static CRadioStateHandler *NewL(CRadioEngine* aEngine);
        /**
        * Destructor
        **/
        virtual ~CRadioStateHandler();
    public:
        /**
        * Trigger the active object to call the client callback function.
        * @since Series 60 2.7
        * @return none
        */
        void Callback(MRadioEngineStateChangeCallback::TFMRadioNotifyEvent aEventCode, TInt aErrorCode );
    private:
        /**
        * c++ default constructor
        **/
        CRadioStateHandler(CRadioEngine* aEngine);
        /**
        * 2nd phase constructor
        **/
        void ConstructL();
        /**
        * Cancel the timeout timer. This generally means that a response was received from the chip, so the 
        * timeout is no longer needed or the state machine was transitioned.
        * @since Series 60 2.7
        * @return none
        **/
        void DoCancel();
        /**
        * State Change Request/timeout handler
        * @since Series 60 2.7
        * @return none
        */
        void RunL();
        /**
        * handle any errors that occur in the RunL
        * @since Series 60 2.7
        * @return none
        */
    	TInt RunError( TInt aError );
    private:
        // pointer to the session who's timeout we are mananging.
        CRadioEngine* iEngine;            
        // Temporary storage for event code
        TInt iCallbackEventCode;
        // Temporary storage for error code        
        TInt iCallbackErrorCode;
   
    };  // FMRADIOENGINERADIOSTATEHANDLER_H   
            
// End of File
    
#endif
