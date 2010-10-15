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
* Description: STIF testclass declaration
*
*/

#ifndef UTILSTESTMODULE_H
#define UTILSTESTMODULE_H

// System includes
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <centralrepository.h> 
#include <e32property.h>
// User includes
#include "trace.h"
#include "mradiosystemeventobserver.h"
#include "mradioaudioroutingobserver.h"
#include "cradioaudiorouter.h"
#include "cradioroutableaudio.h"


// CONSTANTS
const TInt KErrBadTestParameter= -1000;         // Error on configuration file

const TInt KErrEventPending =-2000;             // Timeout and not callback 
const TInt KErrCallbackErrorCode = -2001;       // Callback return error code
const TInt KErrUnexpectedValue =-2002;          // Unexpected value at setting    
const TInt KExpectedFrequencyChange = -2003;        // Is not finding more than one station
const TInt KErrTimeoutController = -2007;       // Unexpected notification

// MACROS
#define TEST_CLASS_VERSION_MAJOR 1
#define TEST_CLASS_VERSION_MINOR 1
#define TEST_CLASS_VERSION_BUILD 1
// Logging path
_LIT( KUtilsTestModuleLogPath, "\\logs\\testframework\\UtilsTestModule\\" ); 
// Log file
_LIT( KUtilsTestModuleLogFile, "UtilsTestModule.txt" ); 
_LIT( KUtilsTestModuleLogFileWithTitle, "UtilsTestModule_[%S].txt" );

// FORWARD DECLARATIONS
class CRadioSystemEventCollector;
class SRadioStubManager;
class RChunk;

// CLASS DECLARATION

/**
*  CUtilsTestModule test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CUtilsTestModule) : public CScriptBase,
        public MRadioSystemEventObserver,
        public MRadioAudioRoutingObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CUtilsTestModule* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CUtilsTestModule();

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

        // MRadioSystemEventObserver =>
        
        /**
         * Called when system event occurs.
         * @param aEventType Type of the event
         */
        virtual void HandleSystemEventL( TRadioSystemEventType aEventType );

        // <= MRadioSystemEventObserver

        // MRadioAudioRoutingObserver =>
        
        /**
         * Notifies of a requested audio routing change. Changes requested
         * by other applications do not generate a notification.
         *
         * @param aRoute Current audio route
         */
        virtual void AudioRouteChangedL( RadioEngine::TRadioAudioRoute aRoute );

        // <= MRadioAudioRoutingObserver

    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );
        // Test case functions
        TInt CreateSystemEventCollectorL( CStifItemParser& aItem );
        TInt DestroySystemEventCollector( CStifItemParser& aItem );
        TInt OOMTestForCreation( CStifItemParser& aItem );
        TInt OOMTestForAudioRouterCreation( CStifItemParser& aItem );
        TInt OOMTestForRoutableAudioCreation( CStifItemParser& aItem );
        
        TInt IsNetworkCoverage( CStifItemParser& aItem );
        TInt IsMobileNetworkCoverage( CStifItemParser& aItem );
        TInt IsCallActive( CStifItemParser& aItem );
        TInt IsAudioResourcesAvailable( CStifItemParser& aItem );
        TInt IsHeadsetConnectedL( CStifItemParser& aItem );

        // Tst functions
        TInt tstSetPropertyValueForHeadset( CStifItemParser& aItem );
        TInt tstSetPropertyValueForNetworkStatus( CStifItemParser& aItem );
        TInt tstSetPropertyValueForCallState( CStifItemParser& aItem );
        TInt tstDefineAndAttachTestProperties ();

        TInt CreateAudioRouterL( CStifItemParser& aItem );
        TInt DestroyAudioRouter( CStifItemParser& aItem );
        
        TInt CreateRoutableAudioL( CStifItemParser& aItem );
        TInt DestroyRoutableAudio( CStifItemParser& aItem );
        
    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CUtilsTestModule( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below. 
        */

        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt ExampleL( CStifItemParser& aItem );
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        /**
        * Opens shared nenory used to control stubs.
        * @since ?Series60_version
        */
        TInt InitSharedMemory();

        /**
        * Creates System Event Collector.
        * @since ?Series60_version
        */
        TInt CreateSystemEventCollector2L();

        /**
        * Creates Audio Router.
        * @since ?Series60_version
        */
        TInt CreateAudioRouter2L();
        
        /**
        * Creates Routable Audio.
        * @since ?Series60_version
        */
        TInt CreateRoutableAudio2L();

    private:    // Data
        
        // RadioStubManager pointer points inside RadioStubManagerChunk
        SRadioStubManager* iRadioStubManager;

        // RadioStubManagerChunk handle
        RChunk iRadioStubManagerChunk;

        /** System Event Collector */
        CRadioSystemEventCollector* iSystemEventCollector;
        
        RProperty iPropertyHeadsetStatus;
        RProperty iPropertyNetworkStatus;
        RProperty iPropertyCallState;
        
        RadioEngine::TRadioAudioRoute iRoute;
        CRadioAudioRouter* iAudioRouter;
        CRadioRoutableAudio* iRoutableAudio;
    };

#endif      // UTILSTESTMODULE_H

// End of File
