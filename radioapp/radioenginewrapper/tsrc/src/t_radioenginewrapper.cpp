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
* Description: radiostation test implementation
*
*/

#include <e32base.h>
#include <eikenv.h>
#include <e32property.h>
#include <radiomonitor.h>

#include "t_radioenginewrapper.h"
//#include "radiopresetstorage.h"
//#include "radiostation.h"
//#include "radiostationmodel.h"
//#include "radioplaylogmodel.h"
#include "radioenginewrapper.h"
#include "radiologger.h" //Radio::connect
#include <cradiosettings.h>
#include "mradioenginesettings.h"
#include <radiostubmanager.h>
#include "trace.h"
#include "RadioClientServer.h"
#include "radiointernalcrkeys.h"

#define STUB  mRadioStubManager
#define TUNER  mRadioStubManager->iTuner
#define PLAYER  mRadioStubManager->iPlayer
#define RDS  mRadioStubManager->iRds
#define ACCESSORYOBSERVER  mRadioStubManager->iAccessoryObserver
#define REMCONTARGET  mRadioStubManager->iRemConTarget

// Constants
const uint KTestFrequency1 = 89000000;
const QString KTestStationName1 = "Radio Noice";

// CONSTANTS
_LIT_SECURITY_POLICY_PASS(KRadioServerReadPolicy);
_LIT_SECURITY_POLICY_C1(KRadioServerWritePolicy, ECapabilityWriteUserData);

/*!
 *
 */
int main(int /* argc*/, char *argv[])
{
FUNC_LOG;
    TestRadioEngineWrapper tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testradiouiengine.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

TestRadioEngineWrapper::TestRadioEngineWrapper()
{
FUNC_LOG;
}

// from base class RadioStationHandlerIf

uint TestRadioEngineWrapper::currentFrequency() const
{
FUNC_LOG;
return 0;
}

int TestRadioEngineWrapper::currentPresetIndex() const
{
FUNC_LOG;
return 0;
}

void TestRadioEngineWrapper::setCurrentStation( uint /*frequency*/ )
{
FUNC_LOG;
}

bool TestRadioEngineWrapper::containsFrequency( uint /*frequency*/ )
{
FUNC_LOG;
return 1;
}

bool TestRadioEngineWrapper::containsPresetIndex( int /*presetIndex*/ )
{
FUNC_LOG;
return 1;
}

void TestRadioEngineWrapper::startDynamicPsCheck()
{
FUNC_LOG;
}

void TestRadioEngineWrapper::addScannedFrequency( uint /*frequency*/ )
{
FUNC_LOG;
}

void TestRadioEngineWrapper::removeLocalStations()
{
FUNC_LOG;
}

void TestRadioEngineWrapper::setCurrentPsName( uint /*frequency*/, const QString& /*name*/ )
{
FUNC_LOG;
}

void TestRadioEngineWrapper::setCurrentRadioText( uint /*frequency*/, const QString& /*radioText*/ )
{
FUNC_LOG;
}

void TestRadioEngineWrapper::setCurrentRadioTextPlus( uint /*frequency*/, int /*rtClass*/, const QString& /*rtItem*/ )
{
FUNC_LOG;
}

void TestRadioEngineWrapper::setCurrentPiCode( uint /*frequency*/, int /*piCode*/ )
{
FUNC_LOG;
}

void TestRadioEngineWrapper::setCurrentGenre( uint /*frequency*/, int /*genre*/ )
{
FUNC_LOG;
}

//  from base class RadioEngineWrapperObserver

void TestRadioEngineWrapper::tunedToFrequency( uint frequency, int /* commandSender */)
{
    FUNC_LOG;
    mEnteredSlots |= TunedToFrequency;
    TInt err = mRadioCR->Set( KRadioCRTunedFrequency , (TInt)frequency );
    QVERIFY2( KErrNone == err, "Setting key KRadioCRTunedFrequency failed!" );
}


void TestRadioEngineWrapper::seekingStarted( Seeking::Direction /* direction */)
{
FUNC_LOG;
mEnteredSlots |= SeekingStarted;
}
    
void TestRadioEngineWrapper::radioStatusChanged( bool /* radioIsOn */)
{
FUNC_LOG;
}
    
void TestRadioEngineWrapper::rdsAvailabilityChanged( bool /* available */)
{
FUNC_LOG;
}
    
void TestRadioEngineWrapper::volumeChanged( int volume )
{
FUNC_LOG;
mEnteredSlots |= VolumeChanged;
mLastRecordedVolume = volume;
}
    
void TestRadioEngineWrapper::muteChanged( bool /* muted */)
{
FUNC_LOG;
mEnteredSlots |= MuteChanged;
}

void TestRadioEngineWrapper::audioRouteChanged( bool /* loudspeaker */)
{
FUNC_LOG;
}

void TestRadioEngineWrapper::scanAndSaveFinished()
{
FUNC_LOG;
}

    
void TestRadioEngineWrapper::headsetStatusChanged( bool /* connected */)
{
FUNC_LOG;
mEnteredSlots |= HeadsetConnected;
}
        
void TestRadioEngineWrapper::skipPrevious()
{
FUNC_LOG;
}
    
void TestRadioEngineWrapper::skipNext()
{
FUNC_LOG;
}

/*!
 * Destructor
 */
TestRadioEngineWrapper::~TestRadioEngineWrapper()
{
FUNC_LOG;
mRadioStubManagerChunk.Close();
delete mEngineWrapper;
mEngineWrapper = NULL;
delete mScheduler;
mScheduler = NULL;
}

/*!
 * called before each testfunction is executed
 */
void TestRadioEngineWrapper::init()
{
FUNC_LOG;
}

/*!
 * called after every testfunction
 */
void TestRadioEngineWrapper::cleanup()
{
FUNC_LOG;
}

/*!
 * called before the first testfunction is executed
 */
 void TestRadioEngineWrapper::initTestCase()
{
    FUNC_LOG;
    // Workaround for the below panic, occured after porting to 10.1 
    // Main Panic E32USER-CBase 44
    // Create and install the active scheduler
    mScheduler = new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(mScheduler);

    TInt err = mRadioStubManagerChunk.CreateGlobal(
            KRadioStubManagerLocalChunkName,
            sizeof(SRadioStubManager),
            sizeof(SRadioStubManager),
            EOwnerThread );
    QVERIFY2( KErrNone == err, "Creation of memory chunk KRadioStubManagerLocalChunkName failed." );
    QVERIFY2( sizeof(SRadioStubManager) <= mRadioStubManagerChunk.MaxSize(), "Improper size for memory chunk KRadioStubManagerLocalChunkName." );
    TUint8* basePtr = mRadioStubManagerChunk.Base();
    QVERIFY2( 0 != basePtr, "Getting base pointer of memory chunk KRadioStubManagerLocalChunkName failed." );
    mRadioStubManager = (SRadioStubManager*)basePtr;
    // Zero configuration/control data 
    
    mRadioStubManager->FillZ();
    TRAP( err, mSchedulerTimer = CSchedulerStopAndStartTimer::NewL( *this ) );
    QVERIFY2(  KErrNone == err, "CSchedulerStopAndStartTimer not constructed!" );     
    mSchedulerTimer->StartTimer( 1000000, CSchedulerStopAndStartTimer::ETimerIdCreateMUT );
}
 
void TestRadioEngineWrapper::testIsEngineConstructed()
{
    FUNC_LOG;
    QVERIFY2(  mEngineWrapper->isEngineConstructed(), "Radio Engine not constructed!" );;
}

void TestRadioEngineWrapper::testRadioSettingsReference()
{
    FUNC_LOG;
    QVERIFY2(  0 != &mEngineWrapper->settings(), "Settings reference illegal!" );    
}

void TestRadioEngineWrapper::testRegion()
{   
    FUNC_LOG;
    QVERIFY2( mEngineWrapper->region() >= RadioRegion::None, "Illegal Region Id!");
    QVERIFY2( mEngineWrapper->region() <= RadioRegion::Poland, "Illegal Region Id!");
    QVERIFY2(  mEngineWrapper->frequencyStepSize() != 0, "Region Step Size illegal!" );
    QVERIFY2(  mEngineWrapper->minFrequency()  <  mEngineWrapper->maxFrequency(), "Region Minimum Frequency illegal!" );
    QVERIFY2(  mEngineWrapper->maxFrequency()  > mEngineWrapper->minFrequency(), "Region Maximum Frequency illegal!" );
    QVERIFY2(  mEngineWrapper->isFrequencyValid( mEngineWrapper->minFrequency() + mEngineWrapper->frequencyStepSize() ), "Legal frequency not accepted!" );
    QVERIFY2(  mEngineWrapper->isFrequencyValid( mEngineWrapper->maxFrequency() - mEngineWrapper->frequencyStepSize() ), "Legal frequency not accepted!" );
    QVERIFY2(  mEngineWrapper->isFrequencyValid( mEngineWrapper->minFrequency() ), "Legal minimum frequency not accepted!" );
    QVERIFY2(  mEngineWrapper->isFrequencyValid( mEngineWrapper->maxFrequency() ), "Legal maximum frequency not accepted!" );
    QVERIFY2(  !mEngineWrapper->isFrequencyValid( mEngineWrapper->maxFrequency() + mEngineWrapper->frequencyStepSize() ), "Illegal frequency accepted!" );
    QVERIFY2(  !mEngineWrapper->isFrequencyValid( mEngineWrapper->minFrequency() - mEngineWrapper->frequencyStepSize() ), "Illegal frequency accepted!" );
    QVERIFY2(  !mEngineWrapper->isFrequencyValid( 0 + mEngineWrapper->frequencyStepSize() ), "Illegal frequency accepted!" );
    QVERIFY2(  !mEngineWrapper->isFrequencyValid( 0 - mEngineWrapper->frequencyStepSize() ), "Illegal frequency accepted!" );
}

void TestRadioEngineWrapper::testRadioOnOff()
{
    FUNC_LOG;
    QVERIFY2(  !mEngineWrapper->isRadioOn(), "Radio is not off!" );

    TInt err(KErrNone);
    TBool antennaAttached = ETrue;
    QVERIFY2(KErrNone == err, "Attaching property ERadioServPsAntennaStatus failed!" );
    err = RProperty::Set( KStub_KRadioServerPropertyCategory, ERadioServPsAntennaStatus,  antennaAttached );
    INFO_1("RProperty::Set( KStub_KRadioServerPropertyCategory, ERadioServPsAntennaStatus,  antennaAttached ) err: %d", err);
    QVERIFY2( KErrNone == err, "Setting property ERadioServPsAntennaStatus failed!" );
    mSchedulerTimer->StartTimer( 1000000 );
    QVERIFY2(  mEngineWrapper->isAntennaAttached(), "Headset/Antenna not connected!");
    QVERIFY2(  mEnteredSlots &= HeadsetConnected, "Headset/Antenna not connected!");

    err = mRadioCR->Set( KRadioCRVisualRadioPowerState, ETrue );
    QVERIFY2( KErrNone == err, "Setting key KRadioCRVisualRadioPowerState failed!" );
    
    mSchedulerTimer->StartTimer( 1000000 );
    QVERIFY2(  mEngineWrapper->isRadioOn(), "Radio is not on!" );
    //QVERIFY2(  !mEngineWrapper->isFrequencyValid( 0 ), "Zero frequency accepted!" );
}
 
void TestRadioEngineWrapper::testTuning()
    {
    FUNC_LOG;
    QVERIFY2( mEngineWrapper->isRadioOn(), "Radio is not on!" );

    mEngineWrapper->tuneFrequency( mEngineWrapper->minFrequency() );
    mSchedulerTimer->StartTimer( 1000000 );
    QVERIFY2(  mEngineWrapper->currentFrequency() ==  mEngineWrapper->minFrequency(), "Tuning to minimum frequency failed!");

    uint freq_A = mEngineWrapper->currentFrequency();    
    mSchedulerTimer->StartTimer( 1000 );
    tstSetFrequency( freq_A + mEngineWrapper->frequencyStepSize() );   
    mEngineWrapper->startSeeking( Seeking::Up );
    QVERIFY2(  mEnteredSlots &= SeekingStarted, "Seeking upwards not started!");
    mEnteredSlots &= !SeekingStarted;
    
    TInt err = mPropertyFrequency.Set( KStub_KRadioServerPropertyCategory, ERadioServPsFrequency,  tstGetFrequency() );
    QVERIFY2( KErrNone == err, "Setting property ERadioServPsFrequency failed!" );
    mSchedulerTimer->StartTimer( 1000000 );

    QVERIFY2(  mEnteredSlots &= TunedToFrequency, "Not Tuned to next upward frequency!");
    mEnteredSlots &= !TunedToFrequency;
    uint freq_B = mEngineWrapper->currentFrequency();
    QVERIFY2(  (freq_B - mEngineWrapper->frequencyStepSize()) == freq_A, "Seeking upwards failed!");


    tstSetFrequency( mEngineWrapper->maxFrequency() );   
    mEngineWrapper->tuneFrequency( mEngineWrapper->maxFrequency() );
    mSchedulerTimer->StartTimer( 1000000 );
    QVERIFY2( mEngineWrapper->currentFrequency() == mEngineWrapper->maxFrequency(), "Current frequency not the maximum one!" );

    mEngineWrapper->startSeeking( Seeking::Down );
    QVERIFY2(  mEnteredSlots &= SeekingStarted, "Seeking downwards not started!");
    mEnteredSlots &= !SeekingStarted;

    tstSetFrequency(  mEngineWrapper->currentFrequency() - (2 * mEngineWrapper->frequencyStepSize()) );   
    err = mPropertyFrequency.Set( KStub_KRadioServerPropertyCategory, ERadioServPsFrequency,  tstGetFrequency() );
    QVERIFY2( KErrNone == err, "Setting property ERadioServPsFrequency failed!" );
    mSchedulerTimer->StartTimer( 1000000 );

    QVERIFY2(  mEnteredSlots &= TunedToFrequency, "Not Tuned to next downward frequency!");
    mEnteredSlots &= !TunedToFrequency;
    QVERIFY2(  mEngineWrapper->currentFrequency() ==  mEngineWrapper->maxFrequency()- (2 * mEngineWrapper->frequencyStepSize()), "Seeking downwards failed!");

    mEngineWrapper->tuneFrequency( (mEngineWrapper->maxFrequency() + mEngineWrapper->minFrequency())/2 );
    mSchedulerTimer->StartTimer( 1000000 );
    QVERIFY2(  mEngineWrapper->currentFrequency() ==  (mEngineWrapper->maxFrequency() + mEngineWrapper->minFrequency())/2, "Tuning to average frequency failed!");

    uint freq_C = mEngineWrapper->currentFrequency();
    TUNER.iSetFrequencyError.SetStubError( KRadioServErrFrequencyOutOfBandRange );
    mEngineWrapper->tuneFrequency( mEngineWrapper->maxFrequency() + mEngineWrapper->frequencyStepSize() );
    mSchedulerTimer->StartTimer( 1000000 );
    
    // Should be == not != as now. This is to continue after bug
    QVERIFY2( mEngineWrapper->currentFrequency() != freq_C, "Tuning over maximum frequency succeeded?");
    QVERIFY2( !mEngineWrapper->isFrequencyValid( mEngineWrapper->currentFrequency() ), "A frequency over maximum accepted?");

    uint freq_D = mEngineWrapper->currentFrequency();
    TUNER.iSetFrequencyError.SetStubError( KRadioServErrFrequencyOutOfBandRange );
    mEngineWrapper->tuneFrequency( mEngineWrapper->minFrequency() - mEngineWrapper->frequencyStepSize() );
    mSchedulerTimer->StartTimer( 1000000 );
    // Should be == not != as now. This is to continue after bug
    QVERIFY2(  mEngineWrapper->currentFrequency() != freq_D, "Tuning below minimum frequency succeeded?");
    QVERIFY2( !mEngineWrapper->isFrequencyValid( mEngineWrapper->currentFrequency() ), "A frequency below minimum accepted?");
    }

void TestRadioEngineWrapper::testTuningWithDelay()
    {
    FUNC_LOG;
    QVERIFY2( mEngineWrapper->isRadioOn(), "Radio is not on!" );
    uint freq_A = mEngineWrapper->currentFrequency();
    uint freq_B = mEngineWrapper->currentFrequency();
    mEngineWrapper->tuneWithDelay( freq_B );
    mSchedulerTimer->StartTimer( 1000000 );
    QVERIFY2(  mEngineWrapper->currentFrequency() == freq_B, "Tuning with delay failed?");
    mEngineWrapper->tuneWithDelay( freq_A );
    mSchedulerTimer->StartTimer( 1000000 );
    QVERIFY2(  mEngineWrapper->currentFrequency() == freq_A, "Tuning with delay failed?");    
    }

void TestRadioEngineWrapper::testMuteToggling()
    {
    FUNC_LOG;
    QVERIFY2( mEngineWrapper->isRadioOn(), "Radio is not on!" );
    TBool muted_1( mEngineWrapper->isMuted() );
    mEngineWrapper->toggleMute();
    
    mSchedulerTimer->StartTimer( 1000000 );
    
    TBool muted_2( mEngineWrapper->isMuted() );
    QVERIFY2(  mEnteredSlots &= MuteChanged, "Mute not changed!");
    mEnteredSlots &= !MuteChanged;    
    QVERIFY2( muted_1 != muted_2, "Mute() not working!" );
    mEngineWrapper->toggleMute();
    
    mSchedulerTimer->StartTimer( 1000000 );
    
    TBool muted_3( mEngineWrapper->isMuted() );
    QVERIFY2(  mEnteredSlots &= MuteChanged, "Mute not changed!");
    mEnteredSlots &= !MuteChanged;    
    QVERIFY2( muted_1 == muted_3, "Mute status not changed to original value!") ;
    }


void TestRadioEngineWrapper::testVolumeSetting()
    {
    FUNC_LOG;
    // SetVoliúme(() seems to accept any value. Probably because the real check point
    //  is missing from test constellation.
    QVERIFY2( mEngineWrapper->isRadioOn(), "Radio is not on!" );
    INFO_1( "mLastRecordedVolume: %i", mLastRecordedVolume );
    TInt volume_1( 1000 );
    mEngineWrapper->setVolume( volume_1 );
    
    mSchedulerTimer->StartTimer( 1000000 );
    
    INFO_1( "mLastRecordedVolume: %i", mLastRecordedVolume );
    QVERIFY2(  mEnteredSlots &= VolumeChanged, "Volume not changed!");
    mEnteredSlots &= !VolumeChanged;    
    QVERIFY2( volume_1 == mLastRecordedVolume, "Volume has unexpected value!") ;

    volume_1 = mLastRecordedVolume;
    TInt volume_2( mLastRecordedVolume * 30 );
    mEngineWrapper->setVolume( volume_2 );
    
    mSchedulerTimer->StartTimer( 1000000 );
    
    INFO_1( "mLastRecordedVolume: %i", mLastRecordedVolume );
    QVERIFY2( volume_2 == mLastRecordedVolume, "Volume has unexpected value!") ;
    }

void TestRadioEngineWrapper::testLoudSpeakerUsage()
    {
    FUNC_LOG;
    QVERIFY2( mEngineWrapper->isRadioOn(), "Radio is not on!" );
    TBool isLoudSpeakerUsed( mEngineWrapper->isUsingLoudspeaker() );
    INFO_1( "isLoudSpeakerUsed: %i", isLoudSpeakerUsed );   
    }

void TestRadioEngineWrapper::testScanning()
    {
    FUNC_LOG;
    QVERIFY2( mEngineWrapper->isRadioOn(), "Radio is not on!" );
    tstSetScanningData( 10, mEngineWrapper->minFrequency(), mEngineWrapper->frequencyStepSize() );
    QVERIFY2( TUNER.iScanStations.iCount == 10, "Scanning not initialized!" );
    mEngineWrapper->scanFrequencyBand();
    mSchedulerTimer->StartTimer( 1000000 );
    
    INFO_1( "TUNER.iScanStations.iCount: %i", TUNER.iScanStations.iCount );
    QVERIFY2( TUNER.iScanStations.iCount == 0, "Scanning not completed!" );
    }

/*!
 * called after the last testfunction was executed
 */
void TestRadioEngineWrapper::cleanupTestCase()
{
    FUNC_LOG;
	delete mEngineWrapper;
	mEngineWrapper = NULL;
    
    //mSchedulerTimer->StartTimer( 1000000, CSchedulerStopAndStartTimer::ETimerIdDeleteMUT );
        
	delete mScheduler;
	mScheduler = NULL;
    mRadioStubManagerChunk.Close();
    mPropertyPlayerState.Close();
    mPropertyAntennaStatus.Close();
    mPropertyOfflineMode.Close();
    mPropertyTransmitterStatus.Close();
    mPropertyFrequency.Close();
    mPropertyFrequencyRange.Close();
    mPropertyForceMonoReception.Close();
    mPropertySquelch.Close();
    mPropertyVolume.Close();
    mPropertyMuteStatus.Close();
    mPropertyBalance.Close();
}

void TestRadioEngineWrapper::tstSetTunerCababilities(uint /*category*/)
    {
    FUNC_LOG;
    TUNER.iCaps.iFrequencyRange = ERsRangeFmEuroAmerica; 
    TUNER.iCaps.iCapabilities = 
        TRsTunerCapabilities::ETunerFunctionAvailableInOfflineMode | 
        TRsTunerCapabilities::ETunerFunctionRds | 
        TRsTunerCapabilities::ETunerFunctionDualTuner; 
    }

void TestRadioEngineWrapper::Timeout( TUint aTimerId )
    {
    FUNC_LOG;    
    if ( CSchedulerStopAndStartTimer::ETimerIdCreateMUT == aTimerId )
        {
        INFO("ETimerIdCreateMUT elapsed");
        }
    else if ( CSchedulerStopAndStartTimer::ETimerIdDeleteMUT == aTimerId )
        {
        INFO("ETimerIdDeleteMUT elapsed");
        }
    else if ( CSchedulerStopAndStartTimer::ETimerIdRunMUT == aTimerId )
        {
        INFO("ETimerIdRunMUT elapsed");
        }
    else
        {
        INFO("Unknown timer elapsed");        
        }
    }

void TestRadioEngineWrapper::CreateMUT()
    {
    FUNC_LOG;
    tstDefineAndAttachRadioServerProperties();
    tstCreateCRObjects();
    TInt err( KErrNone );
    TRAP( err, mEngineWrapper = new (ELeave) RadioEngineWrapper( *this, *this ) );
    QVERIFY2(  KErrNone == err, "Radio Engine not constructed!" ); 
    tstSetTunerCababilities();    
    }

void TestRadioEngineWrapper::DeleteMUT()
    {
    FUNC_LOG;
    delete mEngineWrapper;
    mEngineWrapper = NULL;    
    }

void TestRadioEngineWrapper::tstSetFrequency( TUint aFrequency )
    {
    FUNC_LOG;
    TUNER.iFrequency = aFrequency;
    }

TInt TestRadioEngineWrapper::tstGetFrequency()
    {
    FUNC_LOG;
    return TUNER.iFrequency;
    }

void TestRadioEngineWrapper::tstSetScanningData( TUint aCount, TInt aMinFreq, TInt aFrequencyStepSize )
    {
    FUNC_LOG;
    TUNER.iScanStations.iCount = aCount;
    TUNER.iScanStations.iMinFreq = aMinFreq;
    TUNER.iScanStations.iFrequencyStepSize = aFrequencyStepSize;
    }


void TestRadioEngineWrapper::tstDefineAndAttachRadioServerProperties()
    {
    FUNC_LOG;
    QVERIFY2(KErrNone == RProperty::Define(KStub_KRadioServerPropertyCategory, ERadioServPsPlayerState, RProperty::EByteArray,
                                 KRadioServerReadPolicy, KRadioServerWritePolicy), "Property Define() failed!");
    QVERIFY2(KErrNone == RProperty::Define(KStub_KRadioServerPropertyCategory, ERadioServPsAntennaStatus, RProperty::EInt,
                            KRadioServerReadPolicy, KRadioServerWritePolicy), "Property Define() failed!");
    QVERIFY2(KErrNone == RProperty::Define(KStub_KRadioServerPropertyCategory, ERadioServPsOfflineMode, RProperty::EInt,
                            KRadioServerReadPolicy, KRadioServerWritePolicy), "Property Define() failed!");
    QVERIFY2(KErrNone == RProperty::Define(KStub_KRadioServerPropertyCategory, ERadioServPsTransmitterStatus, RProperty::EInt,
                            KRadioServerReadPolicy, KRadioServerWritePolicy), "Property Define() failed!");
    QVERIFY2(KErrNone == RProperty::Define(KStub_KRadioServerPropertyCategory, ERadioServPsFrequency, RProperty::EInt,
                            KRadioServerReadPolicy, KRadioServerWritePolicy), "Property Define() failed!");
    QVERIFY2(KErrNone == RProperty::Define(KStub_KRadioServerPropertyCategory, ERadioServPsFrequencyRange, RProperty::EInt,
                            KRadioServerReadPolicy, KRadioServerWritePolicy), "Property Define() failed!");
    QVERIFY2(KErrNone == RProperty::Define(KStub_KRadioServerPropertyCategory, ERadioServPsForceMonoReception, RProperty::EInt,
                            KRadioServerReadPolicy, KRadioServerWritePolicy), "Property Define() failed!");
    QVERIFY2(KErrNone == RProperty::Define(KStub_KRadioServerPropertyCategory, ERadioServPsSquelch, RProperty::EInt,
                            KRadioServerReadPolicy, KRadioServerWritePolicy), "Property Define() failed!");
    QVERIFY2(KErrNone == RProperty::Define(KStub_KRadioServerPropertyCategory, ERadioServPsVolume, RProperty::EInt,
                            KRadioServerReadPolicy, KRadioServerWritePolicy), "Property Define() failed!");
    QVERIFY2(KErrNone == RProperty::Define(KStub_KRadioServerPropertyCategory, ERadioServPsMuteStatus, RProperty::EInt,
                            KRadioServerReadPolicy, KRadioServerWritePolicy), "Property Define() failed!");
    QVERIFY2(KErrNone == RProperty::Define(KStub_KRadioServerPropertyCategory, ERadioServPsBalance, RProperty::EByteArray,
                            KRadioServerReadPolicy, KRadioServerWritePolicy), "Property Define() failed!");
    
    
    QVERIFY2(KErrNone == mPropertyPlayerState.Attach( KStub_KRadioServerPropertyCategory, ERadioServPsPlayerState ), "Property Attach() failed!");
    QVERIFY2(KErrNone == mPropertyAntennaStatus.Attach( KStub_KRadioServerPropertyCategory, ERadioServPsAntennaStatus ), "Property Attach() failed!");
    QVERIFY2(KErrNone == mPropertyOfflineMode.Attach( KStub_KRadioServerPropertyCategory, ERadioServPsOfflineMode ), "Property Attach() failed!");
    QVERIFY2(KErrNone == mPropertyTransmitterStatus.Attach( KStub_KRadioServerPropertyCategory, ERadioServPsTransmitterStatus ), "Property Attach() failed!");
    QVERIFY2(KErrNone == mPropertyFrequency.Attach( KStub_KRadioServerPropertyCategory, ERadioServPsFrequency ), "Property Attach() failed!");
    QVERIFY2(KErrNone == mPropertyFrequencyRange.Attach( KStub_KRadioServerPropertyCategory, ERadioServPsFrequencyRange ), "Property Attach() failed!");
    QVERIFY2(KErrNone == mPropertyForceMonoReception.Attach( KStub_KRadioServerPropertyCategory, ERadioServPsForceMonoReception ), "Property Attach() failed!");
    QVERIFY2(KErrNone == mPropertySquelch.Attach( KStub_KRadioServerPropertyCategory, ERadioServPsSquelch ), "Property Attach() failed!");
    QVERIFY2(KErrNone == mPropertyVolume.Attach( KStub_KRadioServerPropertyCategory, ERadioServPsVolume ), "Property Attach() failed!");
    QVERIFY2(KErrNone == mPropertyMuteStatus.Attach( KStub_KRadioServerPropertyCategory, ERadioServPsMuteStatus ), "Property Attach() failed!");
    QVERIFY2(KErrNone == mPropertyBalance.Attach( KStub_KRadioServerPropertyCategory, ERadioServPsBalance ), "Property Attach() failed!");
    }

void TestRadioEngineWrapper::tstCreateCRObjects()
    {
    TInt err( KErrNone );
    TRAP( err, mRadioCR = CRepository::NewL(KStub_KRadioCRUid) );
    QVERIFY2(  KErrNone == err, "Radio Central Repository create/open failed!" ); 
    }


