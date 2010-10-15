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
* Description: This file contains testclass implementation.
*
*/

// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <e32cmn.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <data_caging_path_literals.hrh>
#include <centralrepository.h>
#include<RadioStubManager.h>
#include <ctsydomainpskeys.h>
#include <publicruntimeids.hrh>
#include <sacls.h>

#include "UtilsTestModule.h"
#include "cradiosystemeventcollector.h"
#include "radioengineutils.h"


// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;
//_LIT( KStifScriptEmptyString, "EMPTYSTRING" );
//_LIT( KEmptyString, "" );
_LIT_SECURITY_POLICY_PASS(KRadioServerReadPolicy);
_LIT_SECURITY_POLICY_C1(KRadioServerWritePolicy, ECapabilityWriteUserData);

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPresetUtilityTestModule::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CUtilsTestModule::Delete() 
    {
    FUNC_LOG;

    }

// -----------------------------------------------------------------------------
// CUtilsTestModule::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CUtilsTestModule::RunMethodL( 
    CStifItemParser& aItem ) 
    {
    FUNC_LOG;

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", CUtilsTestModule::ExampleL ),
        // [test cases entries] - Do not remove
        ENTRY( "CreateSystemEventCollectorL", CUtilsTestModule::CreateSystemEventCollectorL ),
        ENTRY( "DestroySystemEventCollector", CUtilsTestModule::DestroySystemEventCollector ),
        ENTRY( "IsNetworkCoverage", CUtilsTestModule::IsNetworkCoverage ),
        ENTRY( "IsMobileNetworkCoverage", CUtilsTestModule::IsMobileNetworkCoverage ),
        ENTRY( "IsCallActive", CUtilsTestModule::IsCallActive ),
        ENTRY( "IsAudioResourcesAvailable", CUtilsTestModule::IsAudioResourcesAvailable ),
        ENTRY( "IsHeadsetConnectedL", CUtilsTestModule::IsHeadsetConnectedL ),
        ENTRY( "OOMTestForCreation", CUtilsTestModule::OOMTestForCreation ),
        ENTRY( "tstSetPropertyValueForHeadset", CUtilsTestModule::tstSetPropertyValueForHeadset ),
        ENTRY( "tstSetPropertyValueForNetworkStatus", CUtilsTestModule::tstSetPropertyValueForNetworkStatus ),
        ENTRY( "tstSetPropertyValueForCallState", CUtilsTestModule::tstSetPropertyValueForCallState ),
        ENTRY( "CreateAudioRouterL", CUtilsTestModule::CreateAudioRouterL ),
        ENTRY( "DestroyAudioRouter", CUtilsTestModule::DestroyAudioRouter ),
        ENTRY( "OOMTestForAudioRouterCreation", CUtilsTestModule::OOMTestForAudioRouterCreation ),
        ENTRY( "CreateRoutableAudioL", CUtilsTestModule::CreateRoutableAudioL ),
        ENTRY( "DestroyRoutableAudio", CUtilsTestModule::DestroyRoutableAudio ),
        ENTRY( "OOMTestForRoutableAudioCreation", CUtilsTestModule::OOMTestForRoutableAudioCreation )
        //ADD NEW ENTRY HERE

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CUtilsTestModule::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUtilsTestModule::ExampleL( CStifItemParser& aItem )
    {
    FUNC_LOG;

    // Print to UI
    _LIT( KSettingsTestModule, "UtilsTestModule" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KSettingsTestModule, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KSettingsTestModule, 
                                KParam, i, &string );
        ++i;
        }

    return KErrNone;

    }

//-----------------------------------------------------------------------------
// CPresetUtilityTest::CreateSystemEventCollectorL
//-----------------------------------------------------------------------------
TInt CUtilsTestModule::CreateSystemEventCollectorL(CStifItemParser& /*aItem*/)
    {
    FUNC_LOG;

    TInt err(KErrNone);
    err = InitSharedMemory();
    if ( KErrNone == err )
        {
        tstDefineAndAttachTestProperties();
        }
    if ((!iSystemEventCollector) && (KErrNone == err))
        {
        TRAP( err, RadioEngineUtils::InitializeL(); );
        if (KErrNone != err)
            {
            INFO_1( "RadioEngineUtils::InitializeL() failed err = %i", err )
            iRadioStubManagerChunk.Close();
            }
        else
            {
            TRAP( err, iSystemEventCollector = CRadioSystemEventCollector::NewL(); );
            if ( KErrNone == err )
                {
                TRAP( err, iSystemEventCollector->AddObserverL( this ); );
                if ( KErrNone != err )
                    {
                    INFO_1( "iSystemEventCollector->AddObserverL() failed err = %i", err )
                    iRadioStubManagerChunk.Close();
                    delete iSystemEventCollector;
                    iSystemEventCollector = NULL;
                    RadioEngineUtils::Release();
                    }
                }
            else
                {
                INFO_1( "CRadioSystemEventCollector::NewL() failed err = %i", err )
                iRadioStubManagerChunk.Close();
                RadioEngineUtils::Release();
                }
            }
        }
    if ( KErrNoMemory == err )
        {
        User::Leave( err );
        }
    MRadioAudioRoutingObserver* observer(NULL);
    iSystemEventCollector->AsObserver( observer );
    if (NULL == observer)
        {
        INFO("Null pointer got for MRadioAudioRoutingObserver from MRadioAudioRoutingObserver from iSystemEventCollector->AsObserver.");   
        }
    return err;
    }

//-----------------------------------------------------------------------------
// CUtilsTestModule::DestroySystemEventCollector
//-----------------------------------------------------------------------------
TInt CUtilsTestModule::DestroySystemEventCollector(CStifItemParser& /*aItem*/)
    {
    FUNC_LOG;
    if (iSystemEventCollector)
        {
        iSystemEventCollector->RemoveObserver(this);
        iRadioStubManagerChunk.Close();
        delete iSystemEventCollector;
        iSystemEventCollector = NULL;
        RadioEngineUtils::Release();
        iPropertyHeadsetStatus.Delete(KStub_KRadioPSUid, SRadioStubManager::EStub_KRadioTestPSKeyHeadsetConnected);
        iPropertyHeadsetStatus.Close();
        iPropertyNetworkStatus.Delete(KStub_KUidSystemCategory, KUidNetworkStatusValue);
        iPropertyNetworkStatus.Close();
        iPropertyCallState.Delete(KStub_KPSUidCtsyCallInformation, KCTsyCallState);
        iPropertyCallState.Close();
        }
    return KErrNone;
    }

//-----------------------------------------------------------------------------
// CUtilsTestModule::OOMTestForCreation
//-----------------------------------------------------------------------------
TInt CUtilsTestModule::OOMTestForCreation(CStifItemParser& /*aItem*/)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    err = InitSharedMemory();
    if (KErrNone == err)
        {
        RadioEngineUtils::InitializeL();
        TInt i(-1);
        for (err = KErrNoMemory; KErrNoMemory == err; __UHEAP_MARKEND)
            {
            TRAP ( err,
                        {
                        ++i;
                        INFO_1("loop count = %i", i );
                        __UHEAP_MARK;
                        __UHEAP_FAILNEXT( i );
                        TRAP( err, CreateSystemEventCollector2L(); );
                        if ( iSystemEventCollector )
                            {
                            delete iSystemEventCollector;
                            iSystemEventCollector = NULL;
                            }
                        }
            );
            }
        __UHEAP_RESET;
        RadioEngineUtils::Release();
        iRadioStubManagerChunk.Close();
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CUtilsTestModule::IsNetworkCoverage()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CUtilsTestModule::IsNetworkCoverage(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TBool expectedNetworkCoverageStatus(EFalse);
    // read parameters
    if (aItem.GetNextInt(expectedNetworkCoverageStatus))
        {
        err = KErrBadTestParameter;
        ERROR(err, "Missing Parameter: expectedNetworkCoverageStatus.");
        }
    TBool networkCoverageStatus(iSystemEventCollector->IsNetworkCoverage());
    if (expectedNetworkCoverageStatus != networkCoverageStatus)
        {
        INFO_2(
                "Failed: networkCoverageStatus=%i, expectedNetworkCoverageStatus=%i.",
                networkCoverageStatus, expectedNetworkCoverageStatus);
        err = KErrUnexpectedValue;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CUtilsTestModule::IsMobileNetworkCoverage()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CUtilsTestModule::IsMobileNetworkCoverage(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TBool expectedMobileNetworkCoverageStatus(EFalse);
    // read parameters
    if (aItem.GetNextInt(expectedMobileNetworkCoverageStatus))
        {
        err = KErrBadTestParameter;
        ERROR(err, "Missing Parameter: expectedMobileNetworkCoverageStatus.");
        }
    TBool mobileNetworkCoverageStatus(
            iSystemEventCollector->IsMobileNetworkCoverage());
    if (expectedMobileNetworkCoverageStatus != mobileNetworkCoverageStatus)
        {
        INFO_2(
                "Failed: mobileNetworkCoverageStatus=%i, expectedMobileNetworkCoverageStatus=%i.",
                mobileNetworkCoverageStatus,
                expectedMobileNetworkCoverageStatus);
        err = KErrUnexpectedValue;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CUtilsTestModule::IsCallActive()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CUtilsTestModule::IsCallActive(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TBool expectedCallActiveStatus(EFalse);
    // read parameters
    if (aItem.GetNextInt(expectedCallActiveStatus))
        {
        err = KErrBadTestParameter;
        ERROR(err, "Missing Parameter: expectedCallActiveStatus.");
        }
    TBool callActiveStatus(iSystemEventCollector->IsCallActive());
    if (expectedCallActiveStatus != callActiveStatus)
        {
        INFO_2("Failed: callActiveStatus=%i, expectedCallActiveStatus=%i.",
                callActiveStatus, expectedCallActiveStatus);
        err = KErrUnexpectedValue;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CUtilsTestModule::IsAudioResourcesAvailable()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CUtilsTestModule::IsAudioResourcesAvailable(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TBool expectedAudioResourcesAvailableStatus(EFalse);
    // read parameters
    if (aItem.GetNextInt(expectedAudioResourcesAvailableStatus))
        {
        err = KErrBadTestParameter;
        ERROR(err,
                "Missing Parameter: expectedAudioResourcesAvailableStatus.");
        }
    TBool audioResourcesAvailableStatus(
            iSystemEventCollector->IsAudioResourcesAvailable());
    if (expectedAudioResourcesAvailableStatus
            != audioResourcesAvailableStatus)
        {
        INFO_2(
                "Failed: audioResourcesAvailableStatus=%i, expectedAudioResourcesAvailableStatus=%i.",
                audioResourcesAvailableStatus,
                expectedAudioResourcesAvailableStatus);
        err = KErrUnexpectedValue;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CUtilsTestModule::IsHeadsetConnectedL()
// Returns: Errors.
// -----------------------------------------------------------------------------
TInt CUtilsTestModule::IsHeadsetConnectedL(CStifItemParser& aItem)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TBool expectedHeadsetConnectedStatus(EFalse);
    // read parameters
    if (aItem.GetNextInt(expectedHeadsetConnectedStatus))
        {
        err = KErrBadTestParameter;
        ERROR(err, "Missing Parameter: expectedHeadsetConnectedStatus.");
        }
    TBool
            headsetConnectedStatus(
                    iSystemEventCollector->IsHeadsetConnectedL());
    if (expectedHeadsetConnectedStatus != headsetConnectedStatus)
        {
        INFO_2(
                "Failed: headsetConnectedStatus=%i, expectedHeadsetConnectedStatus=%i.",
                headsetConnectedStatus, expectedHeadsetConnectedStatus);
        err = KErrUnexpectedValue;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CUtilsTestModule::tstSetPropertyValueForHeadset()
// Returns: System error.
// -----------------------------------------------------------------------------
TInt CUtilsTestModule::tstSetPropertyValueForHeadset( CStifItemParser& aItem )
    {
    FUNC_LOG;
    TInt err( KErrNone );
    TBool headsetConnectedStatus(EFalse);
    // read parameters
    if (aItem.GetNextInt(headsetConnectedStatus))
        {
        err = KErrBadTestParameter;
        ERROR(err, "Missing Parameter: headsetConnectedStatus.");
        }
    err = iPropertyHeadsetStatus.Set(KStub_KRadioPSUid, SRadioStubManager::EStub_KRadioTestPSKeyHeadsetConnected, headsetConnectedStatus);
    ERROR(err, "Headset property Attach() failed!");
    return err;
    }

// -----------------------------------------------------------------------------
// CUtilsTestModule::tstSetPropertyValueForNetworkStatus()
// Returns: System error.
// -----------------------------------------------------------------------------
TInt CUtilsTestModule::tstSetPropertyValueForNetworkStatus( CStifItemParser& aItem )
    {
    FUNC_LOG;
    TInt err( KErrNone );
    TInt networkStatus(NULL);
    // read parameters
    if (aItem.GetNextInt(networkStatus))
        {
        err = KErrBadTestParameter;
        ERROR(err, "Missing Parameter: networkStatus.");
        }
    err = iPropertyNetworkStatus.Set(KStub_KUidSystemCategory, KUidNetworkStatusValue, networkStatus);
    ERROR(err, "Network status property Attach() failed!");
    return err;
    }

// -----------------------------------------------------------------------------
// CUtilsTestModule::tstSetPropertyValueForCallState()
// Returns: System error.
// -----------------------------------------------------------------------------
TInt CUtilsTestModule::tstSetPropertyValueForCallState( CStifItemParser& aItem )
    {
    FUNC_LOG;
    TInt err( KErrNone );
    TInt callState(EFalse);
    // read parameters
    if (aItem.GetNextInt(callState))
        {
        err = KErrBadTestParameter;
        ERROR(err, "Missing Parameter: callState.");
        }
    err = iPropertyCallState.Set(KStub_KPSUidCtsyCallInformation, KCTsyCallState, callState);
    ERROR(err, "Call state property Attach() failed!");
    return err;
    }


// -----------------------------------------------------------------------------
// CUtilsTestModule::HandleSystemEventL()
// Returns: none.
// -----------------------------------------------------------------------------
void CUtilsTestModule::HandleSystemEventL(TRadioSystemEventType aEventType)
    {
    FUNC_LOG;
    INFO_1("aEventType = %d", aEventType);

    switch (aEventType)
        {
        case ERadioAudioResourcesAvailable:
            {
            INFO("ERadioAudioResourcesAvailable");
            }
            break;

        case ERadioAudioAutoResumeForbidden:
            {
            INFO("ERadioAudioAutoResumeForbidden");
            }
            break;

        case ERadioAudioRouteHeadset:
            {
            INFO("ERadioAudioRouteHeadset");
            }
            break;

        case ERadioAudioRouteSpeaker:
            {
            INFO("ERadioAudioRouteSpeaker");
            }
            break;

        case ERadioCallActivated:
            {
            INFO("ERadioCallActivated");
            }
            break;

        case ERadioCallDeactivated:
            {
            INFO("ERadioCallDeactivated");
            }
            break;

        case ERadioHeadsetConnected:
            {
            INFO("ERadioHeadsetConnected");
            }
            break;

        case ERadioHeadsetDisconnected:
            {
            INFO("ERadioHeadsetDisconnected");
            }
            break;
        default:
            {
            INFO("Unexpected event type");
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUtilsTestModule::tstDefineAndAttachTestProperties()
// Returns: none.
// -----------------------------------------------------------------------------
TInt CUtilsTestModule::tstDefineAndAttachTestProperties()
    {
    FUNC_LOG;
    TInt err(RProperty::Define(KStub_KRadioPSUid,
            SRadioStubManager::EStub_KRadioTestPSKeyHeadsetConnected,
            RProperty::EInt, KRadioServerReadPolicy,
            KRadioServerWritePolicy));
    ERROR(err, "Property SRadioStubManager::EStub_KRadioTestPSKeyHeadsetConnected Define() failed!");
    err = iPropertyHeadsetStatus.Attach(KStub_KRadioPSUid, SRadioStubManager::EStub_KRadioTestPSKeyHeadsetConnected);
    ERROR(err, "Property SRadioStubManager::EStub_KRadioTestPSKeyHeadsetConnected Attach() failed!");
    err = iPropertyCallState.Set(KStub_KRadioPSUid, SRadioStubManager::EStub_KRadioTestPSKeyHeadsetConnected, EFalse);
    ERROR(err, "Property SRadioStubManager::EStub_KRadioTestPSKeyHeadsetConnected Set() failed!");

    err = RProperty::Define(KStub_KUidSystemCategory,
            KUidNetworkStatusValue,
            RProperty::EInt, KRadioServerReadPolicy,
            KRadioServerWritePolicy);
    ERROR(err, "Property KUidNetworkStatusValue Define() failed!");
    err = iPropertyNetworkStatus.Attach(KStub_KUidSystemCategory, KUidNetworkStatusValue);
    ERROR(err, "Property KUidNetworkStatusValue Attach() failed!");
    err = iPropertyNetworkStatus.Set(KStub_KUidSystemCategory, KUidNetworkStatusValue, ESANetworkAvailable);
    ERROR(err, "Property KUidNetworkStatusValue Set() failed!");
   

    err = RProperty::Define(KStub_KPSUidCtsyCallInformation,
            KCTsyCallState,
            RProperty::EInt, KRadioServerReadPolicy,
            KRadioServerWritePolicy);
    ERROR(err, "Property KCTsyCallState Define() failed!");
    err = iPropertyCallState.Attach(KStub_KPSUidCtsyCallInformation, KCTsyCallState);
    ERROR(err, "Property KCTsyCallState Attach() failed!");
    err = iPropertyCallState.Set(KStub_KPSUidCtsyCallInformation, KCTsyCallState, EPSCTsyCallStateNone);
    ERROR(err, "Property KCTsyCallState Set() failed!");
    
    return err;
    }

// -----------------------------------------------------------------------------
// CUtilsTestModule::InitSharedMemory
// Returns: errors.
// -----------------------------------------------------------------------------
//
TInt CUtilsTestModule::InitSharedMemory()

    {
    FUNC_LOG;
    TInt err = iRadioStubManagerChunk.CreateGlobal(
            KRadioStubManagerLocalChunkName, sizeof(SRadioStubManager),
            sizeof(SRadioStubManager), EOwnerThread);
    if (KErrAlreadyExists == err)
        {
        INFO("Memory chunk KRadioStubManagerLocalChunkName already exists.");
        err = iRadioStubManagerChunk.OpenGlobal(
                KRadioStubManagerLocalChunkName, EFalse, // == Read | Write
                EOwnerThread);
        }
    if (KErrNone == err)
        {
        if (sizeof(SRadioStubManager) > iRadioStubManagerChunk.MaxSize())
            {
            err = KErrCorrupt;
            INFO_1(
                    "Improper size for memory chunk KRadioStubManagerLocalChunkName. Err = %i",
                    err);
            }
        else
            {
            TUint8* basePtr = iRadioStubManagerChunk.Base();
            if (!basePtr)
                {
                err = KErrCorrupt;
                INFO_1(
                        "Getting base pointer of memory chunk KRadioStubManagerLocalChunkName failed. Err=%i",
                        err);
                }
            else
                {
                iRadioStubManager = (SRadioStubManager*) basePtr;
                // Zero configuration/control data
                iRadioStubManager->FillZ();
                }
            }
        }
    else
        {
        INFO_1(
                "Creation of memory chunk KRadioStubManagerLocalChunkName failed. Err = %i",
                err);
        }
    INFO_1("return value, Err = %i", err);
    return err;
    }

// -----------------------------------------------------------------------------
// CUtilsTestModule::CreateSystemEventCollector2L
// Returns: errors.
// -----------------------------------------------------------------------------
//
TInt CUtilsTestModule::CreateSystemEventCollector2L()
    {
    FUNC_LOG;
    TInt err(KErrNone);
    if (!iSystemEventCollector)
        {
        iSystemEventCollector = CRadioSystemEventCollector::NewL();
        iSystemEventCollector->AddObserverL(this);
        }
    else
        {
        INFO( "KErrAlreadyExists" )
        err = KErrAlreadyExists;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CUtilsTestModule::AudioRouteChangedL
// Returns: none
// -----------------------------------------------------------------------------
//
void CUtilsTestModule::AudioRouteChangedL( RadioEngine::TRadioAudioRoute aRoute )
    {
    FUNC_LOG;
    INFO_1( "aRoute = %i",  aRoute );
    iRoute = aRoute;
    
    }

//-----------------------------------------------------------------------------
// CUtilsTestModule::CreateAudioRouterL
//-----------------------------------------------------------------------------
TInt CUtilsTestModule::CreateAudioRouterL(CStifItemParser& /*aItem*/)
    {
    FUNC_LOG;

    TInt err(KErrNone);
    err = InitSharedMemory();
    if ((!iAudioRouter) && (KErrNone == err))
        {
        TRAP( err, RadioEngineUtils::InitializeL(); );
        if (KErrNone != err)
            {
            INFO_1( "RadioEngineUtils::InitializeL() failed err = %i", err )
            iRadioStubManagerChunk.Close();
            }
        else
            {
            TRAP( err, iAudioRouter = CRadioAudioRouter::NewL( *this ); );
            if ( KErrNone != err )
                {
                INFO_1( "CRadioAudioRouter::NewL() failed err = %i", err )
                iRadioStubManagerChunk.Close();
                RadioEngineUtils::Release();
                }
            }
        }
    if ( KErrNoMemory == err )
        {
        User::Leave( err );
        }
    return err;
    }

//-----------------------------------------------------------------------------
// CUtilsTestModule::DestroyAudioRouter
//-----------------------------------------------------------------------------
TInt CUtilsTestModule::DestroyAudioRouter(CStifItemParser& /*aItem*/)
    {
    FUNC_LOG;
    if (iAudioRouter)
        {
        iRadioStubManagerChunk.Close();
        delete iAudioRouter;
        iAudioRouter = NULL;
        RadioEngineUtils::Release();
        }
    return KErrNone;
    }

//-----------------------------------------------------------------------------
// CUtilsTestModule::OOMTestForAudioRouterCreation
//-----------------------------------------------------------------------------
TInt CUtilsTestModule::OOMTestForAudioRouterCreation(CStifItemParser& /*aItem*/)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    err = InitSharedMemory();
    if (KErrNone == err)
        {
        RadioEngineUtils::InitializeL();
        TInt i(-1);
        for (err = KErrNoMemory; KErrNoMemory == err; __UHEAP_MARKEND)
            {
            TRAP ( err,
                        {
                        ++i;
                        INFO_1("loop count = %i", i );
                        __UHEAP_MARK;
                        __UHEAP_FAILNEXT( i );
                        TRAP( err, CreateAudioRouter2L(); );
                        if ( iAudioRouter )
                            {
                            delete iAudioRouter;
                            iAudioRouter = NULL;
                            }
                        }
            );
            }
        __UHEAP_RESET;
        RadioEngineUtils::Release();
        iRadioStubManagerChunk.Close();
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CUtilsTestModule::CreateAudioRouter2L
// Returns: errors.
// -----------------------------------------------------------------------------
//
TInt CUtilsTestModule::CreateAudioRouter2L()
    {
    FUNC_LOG;
    TInt err(KErrNone);
    if (!iAudioRouter)
        {
        iAudioRouter = CRadioAudioRouter::NewL( *this );
        }
    else
        {
        INFO( "KErrAlreadyExists" )
        err = KErrAlreadyExists;
        }
    return err;
    }

//-----------------------------------------------------------------------------
// CUtilsTestModule::CreateRoutableAudioL
//-----------------------------------------------------------------------------
TInt CUtilsTestModule::CreateRoutableAudioL(CStifItemParser& /*aItem*/)
    {
    FUNC_LOG;

    TInt err( CreateRoutableAudio2L() );
    return err;
    }

//-----------------------------------------------------------------------------
// CUtilsTestModule::CreateRoutableAudio2L
//-----------------------------------------------------------------------------
TInt CUtilsTestModule::CreateRoutableAudio2L()
    {
    FUNC_LOG;

    TInt err(KErrNone);
    if (!iRoutableAudio)
        {
        TRAP( err, iRoutableAudio = new (ELeave) CRadioRoutableAudio( iAudioRouter ); );
        if ( KErrNone != err )
            {
            INFO_1( "new CRadioRoutableAudio failed err = %i", err )
            }
        }
    if ( KErrNoMemory == err )
        {
        User::Leave( err );
        }
    return err;
    }

//-----------------------------------------------------------------------------
// CUtilsTestModule::DestroyRoutableAudio
//-----------------------------------------------------------------------------
TInt CUtilsTestModule::DestroyRoutableAudio(CStifItemParser& /*aItem*/)
    {
    FUNC_LOG;
    if (iRoutableAudio)
        {
        delete iRoutableAudio;
        iRoutableAudio = NULL;
        // Note that previous delete call destroyed also Audio Router.
        iAudioRouter = NULL;
        iRadioStubManagerChunk.Close();
        RadioEngineUtils::Release();
        }
    return KErrNone;
    }

//-----------------------------------------------------------------------------
// CUtilsTestModule::OOMTestForRoutableAudioCreation
//-----------------------------------------------------------------------------
TInt CUtilsTestModule::OOMTestForRoutableAudioCreation(CStifItemParser& /*aItem*/)
    {
    FUNC_LOG;
    TInt err(KErrNone);
    TInt i(-1);
    for (err = KErrNoMemory; KErrNoMemory == err; __UHEAP_MARKEND)
        {
        ++i;
        INFO_1("loop count = %i", i );
        __UHEAP_MARK;
        __UHEAP_FAILNEXT( i );
        TRAP( err, CreateRoutableAudio2L(); );
        if ( iRoutableAudio )
            {
            delete iRoutableAudio;
            iRoutableAudio = NULL;
            // Note that previous delete call destroyed also Audio Router.
            iAudioRouter = NULL;
            iRadioStubManagerChunk.Close();
            RadioEngineUtils::Release();
            }
        }
    __UHEAP_RESET;
    return err;
    }

// -----------------------------------------------------------------------------
// CUtilsTestModule::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CUtilsTestModule::?member_function(
   CItemParser& aItem )
   {
   FUNC_LOG;

   ?code

   }
*/


// ========================== OTHER EXPORTED FUNCTIONS =========================
// None


//  [End of File] - Do not remove
