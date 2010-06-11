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

#define UNIT_TESTS_FOR_10_1
#ifdef UNIT_TESTS_FOR_10_1
#include <e32base.h>
#include <eikenv.h>
#endif
#include "t_radiostation.h"
#include "radiopresetstorage.h"
#include "radiostation.h"
#include "radiostationmodel.h"
#include "radiohistorymodel.h"
#include "radioenginewrapper.h"
#include "radiouiengine.h"
#include "radiologger.h" //Radio::connect

// Constants
const uint KTestFrequency1 = 89000000;
const uint KTestFrequency2 = 89500000;
const uint KTestFrequency3 = 90000000;
const uint KTestFrequency4 = 90500000;
const uint KTestFrequency5 = 91000000;
const uint KTestFrequency6 = 91500000;
const uint KTestGenre1 = 1;
const uint KTestGenre2 = 2;
const QString KTestStationName1 = "Radio Noice";
const QString KTestStationName2 = "RN RENAMED";
const QString KTestUrl1 = "http://qt.nokia.com";
const QString KTestUrl2 = "http://www.radionoice.com";
const QString KTestFrequencyString1 = "89000000";
const QString KTestFrequencyString2 = "89500000";
const QString KTestFrequencyString3 = "90000000";
const QString KTestFrequencyString6 = "91500000";
const QString KTestArtist1 = "Eläkeläiset";
const QString KTestArtist2 = "Jope";
const QString KTestArtist3 = "Motorhead";
const QString KTestTitle1 = "Humppa^2";
const QString KTestTitle2 = "´åäö´ ^&%¤^";
const QString KTestTitle3 = "Dancing queen";
const QString KTestRadioTextRadioText = "NOW! Metallica - Enter sandman in MusicStore for free";
const QString KTestRadioTextPlusArtist = "Metallica";
const QString KTestRadioTextPlusTitle = "Enter sandman";
const QString KTestRadioTextPlusUrl = "www.metallica.com";
const QString KTestRadioTextPlusUnsupportedTag = "*#*#*#";
const QString KTestDynamicPSText = "MAKKARAA";

/*!
 *
 */
int main(int /* argc*/, char *argv[])
{
    TestRadioUiEngine tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testradiouiengine.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}


TestRadioUiEngine::TestRadioUiEngine()
{
}


void TestRadioUiEngine::tunedToFrequency( uint /* frequency */, int /* commandSender */)
{
}


void TestRadioUiEngine::seekingStarted( Seeking::Direction /* direction */)
{
}
    
void TestRadioUiEngine::radioStatusChanged( bool /* radioIsOn */)
{
}
    
void TestRadioUiEngine::rdsAvailabilityChanged( bool /* available */)
{
}
    
void TestRadioUiEngine::volumeChanged( int /* volume */)
{
}
    
void TestRadioUiEngine::muteChanged( bool /* muted */)
{
}

void TestRadioUiEngine::audioRouteChanged( bool /* loudspeaker */)
{
}

void TestRadioUiEngine::scanAndSaveFinished()
{
}

    
void TestRadioUiEngine::headsetStatusChanged( bool /* connected */)
{
}
        
void TestRadioUiEngine::skipPrevious()
{
}
    
void TestRadioUiEngine::skipNext()
{
}

/*!
 * Destructor
 */
TestRadioUiEngine::~TestRadioUiEngine()
{
	delete mRadioStationModel;	
	delete mUiEngine;
}

/*!
 * called before each testfunction is executed
 */
void TestRadioUiEngine::init()
{
}

/*!
 * called after every testfunction
 */
void TestRadioUiEngine::cleanup()
{
}

/*!
 * called before the first testfunction is executed
 */
void TestRadioUiEngine::initTestCase()
{
#ifdef UNIT_TESTS_FOR_10_1
// Workaround for the below panic, occured after porting to 10.1 
// Main Panic E32USER-CBase 44
// Create and install the active scheduler
    CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);
// CCoeEnv::Static() call in CRadioEngineTls returns NULL
    CCoeEnv* env = new CCoeEnv;
#endif 
    mUiEngine = new RadioUiEngine;
    mRadioStationModel = new RadioStationModel( *mUiEngine );
    mhistoryModel = new RadioHistoryModel( *mUiEngine );
    
    mEngineWrapper.reset( new RadioEngineWrapper( mRadioStationModel->stationHandlerIf(), *this ) );
    mPresetStorage.reset( new RadioPresetStorage() );
    mRadioStationModel->initialize( mPresetStorage.data(), mEngineWrapper.data() );
    
    //TODO:: Check why ASSERT fails when mModel->rowCount() == 0 
    if(mRadioStationModel->rowCount()>0)
    {
    	mRadioStationModel->removeAll(); //ASSERT: \"last >= first\" in file qabstractitemmodel.cpp, line 2110	
    }    

    Radio::connect( mRadioStationModel,  SIGNAL(dataChanged(const QModelIndex, const QModelIndex)),
        this,    SLOT(dataChanged(const QModelIndex, const QModelIndex)) );
    
    Radio::connect( mRadioStationModel,           SIGNAL(stationAdded(RadioStation)),
        this,    SLOT(stationAdded(RadioStation)) );
    
    Radio::connect( mRadioStationModel,           SIGNAL(stationDataChanged(RadioStation)),
        this,    SLOT(stationDataChanged(RadioStation)) );
    
    Radio::connect( mRadioStationModel,           SIGNAL(favoriteChanged(RadioStation)),
        this,    SLOT(favoriteChanged(RadioStation)) );
    
    Radio::connect( mhistoryModel,           SIGNAL(itemAdded()),
        this,    SLOT(itemAdded()) );
}

/*!
 * called after the last testfunction was executed
 */
void TestRadioUiEngine::cleanupTestCase()
{
	delete mRadioStationModel;
	delete mUiEngine;
}

/*!
 * 
 */
void TestRadioUiEngine::dataChanged(const QModelIndex /* topLeft */, const QModelIndex /* bottomRight */)
{
	mEnteredSlots |= DataChanged;
}

/*!
 * 
 */
void TestRadioUiEngine::stationDataChanged( RadioStation /* addedStation */ )
{
	mEnteredSlots |= StationDataChanged;	
}

/*!
 * 
 */
void TestRadioUiEngine::favoriteChanged( RadioStation /* addedStation */)
{
	mEnteredSlots |= FavoriteChanged;
}

/*!
 * 
 */
void TestRadioUiEngine::itemAdded()
{
    mEnteredSlots |= ItemAdded;
}

/*!
 * 
 */
void TestRadioUiEngine::stationAdded( RadioStation addedStation )
{
	mEnteredSlots |= StationAdded;
	
	QVERIFY2(( mStationToBeAdded == addedStation.name() ), "API:RadioStationModel stationAdded 1");
	QVERIFY2(( mExpectedStationCount == mRadioStationModel->rowCount() ), "API:RadioStationModel stationAdded 2");	
}


/*!
 * Testing of implicit sharing a.k.a. copy-on-write
 */
void TestRadioUiEngine::testImplicitSharing()
{	
	RadioStation t_RadioStation_1;
	
	int originalPresetIndex = t_RadioStation_1.presetIndex();
	// before any values assigned into any data field
	QVERIFY2(( originalPresetIndex == RadioStation::SharedNull ), "API:Radiostation init failed 1");
	t_RadioStation_1.setName("Noice");
	originalPresetIndex = t_RadioStation_1.presetIndex();
	// once some value assigned into some other data field
	QVERIFY2(( originalPresetIndex == RadioStation::Invalid ), "API:Radiostation init failed 2");
		
	t_RadioStation_1.setFrequency( KTestFrequency1 );
	//const int newIndex = mModel->findUnusedPresetIndex();
	t_RadioStation_1.setPresetIndex( 2 );
	t_RadioStation_1.setGenre(1);
	t_RadioStation_1.setUrl("http://qt.nokia.com");			
	
	RadioStation* t_RadioStation_2 = new RadioStation(t_RadioStation_1);
	// test that changing the content of copied data doesn't cause
	// detach in the copying data structure
	uint originalFrequency = t_RadioStation_1.frequency();
    t_RadioStation_1.setFrequency( originalFrequency + 1 );
    // should be detached
	bool detached = t_RadioStation_2->isDetached();
	QVERIFY2(detached, "API:Radiostation Implicit sharing/ freq 1");
	
	// test that changing the content of data sets detach true
	originalFrequency = t_RadioStation_2->frequency();
	t_RadioStation_2->setFrequency( originalFrequency + 1 );
	// should be detached
	detached = t_RadioStation_2->isDetached();	
	QVERIFY2(detached, "API:Radiostation Implicit sharing/ freq 2");	
	delete t_RadioStation_2;
	t_RadioStation_2 = NULL;
	
	//-------------------------------------------------------------------------
	// test that changing the content of copied data doesn't cause
	// detach in the copying data structure
	t_RadioStation_2 = new RadioStation(t_RadioStation_1);
	// should not be detached
	detached = t_RadioStation_2->isDetached();
	QVERIFY2(!detached, "API:Radiostation Implicit sharing/ preset index 1");
	originalPresetIndex = t_RadioStation_1.presetIndex();
	t_RadioStation_1.setPresetIndex( originalPresetIndex + 1 );
	// should be detached
	detached = t_RadioStation_2->isDetached();
	QVERIFY2(detached, "API:Radiostation Implicit sharing/ preset index 2");
	
	// test that changing the content of data sets detach true
	originalPresetIndex = t_RadioStation_2->presetIndex();
	//newIndex = mModel->findUnusedPresetIndex();
	t_RadioStation_2->setPresetIndex( originalPresetIndex + 1 );
	// should be detached
	detached = t_RadioStation_2->isDetached();
	QVERIFY2(detached, "API:Radiostation Implicit sharing/ preset index 3");	
    delete t_RadioStation_2;
    t_RadioStation_2 = NULL;
    
    // test that changing the content of copied data doesn't cause
    // detach in the copying data structure
    t_RadioStation_2 = new RadioStation(t_RadioStation_1);
    // should not be detached
    detached = t_RadioStation_2->isDetached();
    QVERIFY2(!detached, "API:Radiostation Implicit sharing/ name 1");
    //QString originalName = t_RadioStation_1.name(); 
    t_RadioStation_1.setName("RadioOne");
    // should be detached
    detached = t_RadioStation_2->isDetached();
    QVERIFY2(detached, "API:Radiostation Implicit sharing/ name 2");
    // test that changing the content of data sets detach true
    t_RadioStation_2->setName("RadioTwo");
    // should be detached
    detached = t_RadioStation_2->isDetached();
    QVERIFY2(detached, "API:Radiostation  Implicit sharing/ name 3");
}

/*!
 * Testing of change flags set by RadioStation class
 */
void TestRadioUiEngine::testChangeFlags()
{
	RadioStation t_RadioStation;
		
    QVERIFY2(!t_RadioStation.isValid(), "API:Radiostation Init failure");
    t_RadioStation.setUserDefinedName("");
	t_RadioStation.setUserDefinedName("Radio Noice");
	t_RadioStation.setFrequency( KTestFrequency1 );
	//const int newIndex = mModel->findUnusedPresetIndex();
	t_RadioStation.setPresetIndex( 2 );
	t_RadioStation.setGenre( KTestGenre1 );
	t_RadioStation.setUrl( KTestUrl1 );	
	
	t_RadioStation.resetChangeFlags();
	bool persistentDataChanged = t_RadioStation.hasDataChanged( RadioStation::PersistentDataChanged );
    QVERIFY2(!persistentDataChanged, "API:Radiostation Change flags/ PersistentDataChanged 1");	
    t_RadioStation.setUserDefinedName("Radio Noice+");
	bool nameChanged = t_RadioStation.hasDataChanged( RadioStation::NameChanged );	
	QVERIFY2(nameChanged, "API:Radiostation Change flags/ NameChanged");	
	persistentDataChanged = t_RadioStation.hasDataChanged( RadioStation::PersistentDataChanged );
	QVERIFY2(persistentDataChanged, "API:Radiostation Change flags/ PersistentDataChanged 2");	
	t_RadioStation.resetChangeFlags();
			
	uint originalGenre = t_RadioStation.genre();
	t_RadioStation.setGenre( originalGenre + 1 );
	bool genreChanged = t_RadioStation.hasDataChanged( RadioStation::GenreChanged );
	QVERIFY2(genreChanged, "API:Radiostation Change flags/ GenreChanged");	
	persistentDataChanged = t_RadioStation.hasDataChanged( RadioStation::PersistentDataChanged );
	QVERIFY2(persistentDataChanged, "API:Radiostation Change flags/ PersistentDataChanged 3");	
	t_RadioStation.resetChangeFlags();
	
	QString originalUrl = t_RadioStation.url();
	t_RadioStation.setUrl(originalUrl);
	// previous url set so no url or persistent data change flags should be set
	bool urlChanged = t_RadioStation.hasDataChanged( RadioStation::UrlChanged );
	QVERIFY2(!urlChanged, "API:Radiostation Change flags/ UrlChanged");
	persistentDataChanged = t_RadioStation.hasDataChanged( RadioStation::PersistentDataChanged );
    QVERIFY2(!persistentDataChanged, "API:Radiostation Change flags/ PersistentDataChanged 4");	
	t_RadioStation.resetChangeFlags();
	
	QString originalRadioText = t_RadioStation.radioText();
	t_RadioStation.setRadioText( originalRadioText + "buy sausage" );
	bool radioTextChanged = t_RadioStation.hasDataChanged( RadioStation::RadioTextChanged );
	QVERIFY2(radioTextChanged, "API:Radiostation Change flags/ RadioTextChanged");
	// radio text not stored into cenrep
	persistentDataChanged = t_RadioStation.hasDataChanged( RadioStation::PersistentDataChanged );
	QVERIFY2(!persistentDataChanged, "API:Radiostation Change flags/ PersistentDataChanged 5");
	t_RadioStation.resetChangeFlags();
	t_RadioStation.setRadioText( t_RadioStation.radioText() );
	// because current radio text is reset change flags must not be affected
	radioTextChanged = t_RadioStation.hasDataChanged( RadioStation::RadioTextChanged );
	QVERIFY2(!radioTextChanged, "API:Radiostation Change flags/ RadioTextChanged");
	t_RadioStation.resetChangeFlags();
	
	QVERIFY2((t_RadioStation.psType()==RadioStation::Unknown), "API:Radiostation PS type check");	
	t_RadioStation.setPsType( RadioStation::Dynamic );
	bool psTypeChanged = t_RadioStation.hasDataChanged( RadioStation::PsTypeChanged );
	QVERIFY2(psTypeChanged, "API:Radiostation Change flags/ PsTypeChanged");
	persistentDataChanged = t_RadioStation.hasDataChanged( RadioStation::PersistentDataChanged );
	// PS type not stored as persistent data
	QVERIFY2(!persistentDataChanged, "API:Radiostation Change flags/ PersistentDataChanged 6");
	t_RadioStation.resetChangeFlags();
	
	t_RadioStation.setFavorite(t_RadioStation.isFavorite());
	bool favouriteChanged = t_RadioStation.hasDataChanged( RadioStation::FavoriteChanged );
    QVERIFY2(!favouriteChanged, "API:Radiostation Change flags/ FavoriteChanged");    
    persistentDataChanged = t_RadioStation.hasDataChanged( RadioStation::PersistentDataChanged );
    QVERIFY2(!persistentDataChanged, "API:Radiostation Change flags/ PersistentDataChanged 7");
    t_RadioStation.resetChangeFlags();
    
    // initially there should not be PI code defined
    QVERIFY2(!t_RadioStation.hasPiCode(), "API:Radiostation Pi code check");
    // non-clear channel
    t_RadioStation.setPiCode(0xC004, RadioRegion::Default); //88.1 CBEE-FM - Chatham, ON
    bool piCodeChanged = t_RadioStation.hasDataChanged( RadioStation::PiCodeChanged );
    QVERIFY2(piCodeChanged, "API:Radiostation Change flags/ PiCodeChanged");
    persistentDataChanged = t_RadioStation.hasDataChanged( RadioStation::PersistentDataChanged );
    QVERIFY2(persistentDataChanged, "API:Radiostation Change flags/ PersistentDataChanged 8");
}

/*!
 * Testing of call sign integer to char conversion
 */
void TestRadioUiEngine::TestCallSignChar()
{
	for(uint i = 0; i < KLastCallSignCharCode; i++)
	{
	    RadioStation t_RadioStation;
	    bool passed =  t_RadioStation.callSignChar(i) == static_cast<char>( 'A' + i ); 	    	
	    QVERIFY2(passed, "API:Radiostation TestCallSignChar 1");	
	}
	RadioStation t_RadioStation;
	bool passed = t_RadioStation.callSignChar(KLastCallSignCharCode + 1) == static_cast<char>( '?' );
	QVERIFY2(passed, "API:Radiostation TestCallSignChar 2");
}

/*!
 * Testing of PI code to call sign conversion
 */
void TestRadioUiEngine::testPICodeToCallSign()
{
	RadioStation t_RadioStation;
	
	// boundary values, two typical values and three chars call sign case
	QString callSign = t_RadioStation.piCodeToCallSign( KKxxxCallSignPiFirst - 1 );
	QVERIFY2((callSign==""), "API:Radiostation Call sign <");
	callSign = t_RadioStation.piCodeToCallSign( KKxxxCallSignPiFirst );
	QVERIFY2((callSign=="KAAA"), "API:Radiostation Call sign KAAA");
	callSign = t_RadioStation.piCodeToCallSign( 0x243F );
	QVERIFY2((callSign=="KHRJ"), "API:Radiostation Call sign KHRJ");
	callSign = t_RadioStation.piCodeToCallSign( KWxxxCallSignPiFirst - 1 );
	QVERIFY2((callSign=="KZZZ"), "API:Radiostation Call sign KZZZ");
	callSign = t_RadioStation.piCodeToCallSign( KWxxxCallSignPiFirst );
	QVERIFY2((callSign=="WAAA"), "API:Radiostation Call sign WAAA");
	callSign = t_RadioStation.piCodeToCallSign( 0x74B9 );
    QVERIFY2((callSign=="WMDT"), "API:Radiostation Call sign WMDT");
    callSign = t_RadioStation.piCodeToCallSign( KWxxxCallSignPiLast );
    QVERIFY2((callSign=="WZZZ"), "API:Radiostation Call sign WZZZ");
    callSign = t_RadioStation.piCodeToCallSign( KWxxxCallSignPiLast + 1 );
    QVERIFY2((callSign=="KEX"), "API:Radiostation Call sign KEX");
    callSign = t_RadioStation.piCodeToCallSign( 0x99B5 );
    QVERIFY2((callSign=="WJZ"), "API:Radiostation Call sign WJZ");    
    callSign = t_RadioStation.piCodeToCallSign( 0x99C0 );
    QVERIFY2((callSign==""), "API:Radiostation Call sign ????");
}

/*!
 * Testing of RadioStationModel initial state
 */
void TestRadioUiEngine::testRadioStationModelInit()
{
	//TODO:: Check why ASSERT fails when mModel->rowCount() == 0 
	if( mRadioStationModel->rowCount()>0 )
    {	    
		mRadioStationModel->removeAll();
		QVERIFY2((mRadioStationModel->rowCount()==0), "API:RadioStationModel removeAll()");
	}
		 
	RadioStation foundStation;
	// no stations in the model in this phase
	QVERIFY2(!(mRadioStationModel->findFrequency( KTestFrequency1, foundStation )), 
		"API:RadioStationModel findFrequency 1");	
}

/*!
 * Testing of RadioStationModel addStation method and resulting RadioStationModel signals
 */
void TestRadioUiEngine::testAddStation1()
{
	RadioStation station;
	station.setFrequency( KTestFrequency1 );
	station.setFrequency( KTestFrequency1 );
	station.setGenre( KTestGenre1 );
	station.setGenre( KTestGenre1 );
	station.setUrl( KTestUrl1 );    
	station.setType( RadioStation::LocalStation );
	station.setType( RadioStation::LocalStation );
	station.setName("");
	station.setName( KTestStationName1 );
	station.setDynamicPsText( KTestDynamicPSText );
	station.setPiCode( 0xC004, RadioRegion::America ); //88.1 CBEE-FM - Chatham, ON
    QVERIFY2(!(station.dynamicPsText().compare(KTestDynamicPSText)), "API:RadioStationModel addStation 1");
    station.setDynamicPsText( KTestDynamicPSText );
	// check that adding station increases model row count
	mExpectedStationCount = mRadioStationModel->rowCount() + 1;	
	mStationToBeAdded = station.name();
	mRadioStationModel->addStation( station );
	QVERIFY2((mRadioStationModel->rowCount()==mExpectedStationCount), "API:RadioStationModel addStation 2");
	bool correctSignalsReceived = mEnteredSlots.testFlag( StationDataChanged ) && 
	    mEnteredSlots.testFlag( StationAdded ) && mEnteredSlots.testFlag( DataChanged );
	
	bool frequencyUpdated = station.frequencyMhz().toDouble()*1000000 == KTestFrequency1;
	QVERIFY2(frequencyUpdated, "API:RadioStationModel addStation 1");
	// check that correct signals received
	QVERIFY2(correctSignalsReceived, "API:RadioStationModel addStation 3");
	mEnteredSlots = NoSlotsEntered;
	
	// check that added station can be found by frequency
    QModelIndex index = mRadioStationModel->modelIndexFromFrequency( KTestFrequency1 );
    QVariant stationData = mRadioStationModel->data( index, Qt::DisplayRole );
    QString stationName = stationData.toString().right( KTestStationName1.length() );    
    QVERIFY2(!(stationName.compare(KTestStationName1)), "API:RadioStationModel modelIndexFromFrequency");
}

/*!
 * Testing of RadioStationModel saveStation method and resulting RadioStationModel signals
 */
void TestRadioUiEngine::testSaveStation1()
{
	RadioStation newStation1;
    // check that find by frequency works
    // this is needed for preset index to be updated into newStation1
    QVERIFY2((mRadioStationModel->findFrequency( KTestFrequency1, newStation1 )), 
        "API:RadioStationModel findFrequency 2");
    newStation1.setType( RadioStation::Favorite );
    mRadioStationModel->saveStation( newStation1 );    
    bool correctSignalsReceived = mEnteredSlots.testFlag( StationDataChanged ) && 
            mEnteredSlots.testFlag( FavoriteChanged ) && mEnteredSlots.testFlag( DataChanged );;
    QVERIFY2(correctSignalsReceived, "API:RadioStationModel saveStation 1");
    
    newStation1.setGenre(newStation1.genre()+1);
    mRadioStationModel->saveStation( newStation1 );
    correctSignalsReceived = mEnteredSlots.testFlag( StationDataChanged  ) && mEnteredSlots.testFlag( DataChanged );
    // check that correct signals received
    QVERIFY2(correctSignalsReceived, "API:RadioStationModel saveStation 2");
}

/*!
 * Testing of RadioStationModel addStation method and rowCount methods
 */
void TestRadioUiEngine::testAddStation2()
{
    RadioStation newStation2;
	newStation2.setFrequency( KTestFrequency2 );
    QVERIFY2(!newStation2.hasRds(), "API:RadioStationModel addStation 1");
	newStation2.setGenre( KTestGenre2 );
	// setting genre should set this true
	QVERIFY2(newStation2.hasRds(), "API:RadioStationModel addStation 2");
	newStation2.setUrl( KTestUrl2 );
    newStation2.setType( RadioStation::LocalStation | RadioStation::Favorite );
    newStation2.setName("Radio ice");
    // check that adding station increases model row count
    mExpectedStationCount = mRadioStationModel->rowCount()+1;
    mStationToBeAdded = newStation2.name();
    mRadioStationModel->addStation( newStation2 );
    
    QVERIFY2((mRadioStationModel->rowCount()==mExpectedStationCount), "API:RadioStationModel addStation 3");
    mEnteredSlots = NoSlotsEntered;
}

/*!
 * Testing of RadioStationModel saveStation method special cases
 */
void TestRadioUiEngine::testSaveStation2()
{
    RadioStation newStation1;
    // this is needed for preset index to be updated into newStation1
    QVERIFY2((mRadioStationModel->findFrequency( KTestFrequency1, newStation1 )), 
        "API:RadioStationModel findFrequency 2");
    
	newStation1.setFrequency( KTestFrequency2 );	
	mExpectedStationCount = mRadioStationModel->rowCount();
	// updating existing station data must not increase station count
	mRadioStationModel->saveStation( newStation1 );
	QVERIFY2((mRadioStationModel->rowCount()==mExpectedStationCount), "API:RadioStationModel saveStation 1");    
    // because frequency tried to be updated no signals should be received either
	bool correctSignalsReceived = mEnteredSlots.testFlag( NoSlotsEntered );
    QVERIFY2(correctSignalsReceived, "API:RadioStationModel saveStation 2");
    mEnteredSlots = NoSlotsEntered;
    
    // original frequency resumed
    newStation1.setFrequency( KTestFrequency1 );
    newStation1.setGenre(newStation1.genre()+1);
    mExpectedStationCount = mRadioStationModel->rowCount();
    // now that frequency resumed signal should be received also
    mRadioStationModel->saveStation( newStation1 );
    correctSignalsReceived = mEnteredSlots.testFlag( StationDataChanged ) && mEnteredSlots.testFlag( DataChanged );
    QVERIFY2(correctSignalsReceived, "API:RadioStationModel saveStation 3");
    // updating existing station data must not increase station count
    QVERIFY2((mRadioStationModel->rowCount()==mExpectedStationCount), "API:RadioStationModel saveStation 2");
    
    mExpectedStationCount = mRadioStationModel->rowCount();
    mStationToBeAdded = newStation1.name();
    // adding station must fail because the frequency is the same as previously used frequency
    mRadioStationModel->addStation( newStation1 );
    QVERIFY2((mRadioStationModel->rowCount()==mExpectedStationCount), "API:RadioStationModel addStation 3");
}

/*!
 * Testing of RadioStationModel addStation method special cases
 */
void TestRadioUiEngine::testAddStation3()
{
    RadioStation newStation3;
    newStation3.setFrequency( KTestFrequency2 );
    newStation3.setGenre(3);
    newStation3.setUrl("http://www.radio4noice.com");
    newStation3.setType( RadioStation::LocalStation | RadioStation::Favorite );
    newStation3.setName("Radio e");
    // adding station must fail because frequency is the same as previously used frequency
    mExpectedStationCount = mRadioStationModel->rowCount();
    mStationToBeAdded = newStation3.name();
    mRadioStationModel->addStation( newStation3 );    
    QVERIFY2((mRadioStationModel->rowCount()==mExpectedStationCount), "API:RadioStationModel addStation 4");
    // adding station must success because the station frequency is different now
    newStation3.setFrequency( KTestFrequency2 + 1 );
    mExpectedStationCount = mRadioStationModel->rowCount() + 1;
    mRadioStationModel->addStation( newStation3 );
    QVERIFY2((mRadioStationModel->rowCount()==mExpectedStationCount), "API:RadioStationModel addStation 5");
    // test that station can be found by frequency range
    QList<RadioStation> stations;
    stations = mRadioStationModel->stationsInRange( KTestFrequency1, KTestFrequency3 );
    QVERIFY2((stations.count()==3), "API:RadioStationModel stationsInRange");
}

/*!
 * Testing of RadioStationModel saveStation method special cases
 */
void TestRadioUiEngine::testSaveStation3()
{
    RadioStation newStation3;
    RadioStation foundStation;
    QVERIFY2(mRadioStationModel->findFrequency( KTestFrequency2 + 1, newStation3 ),			
        "API:RadioStationModel findFrequency 4");
    
    newStation3.setFrequency( KTestFrequency3 );
    // because frequency or preset index don't have change flag the frequency must not be updated 
    mRadioStationModel->saveStation( newStation3 );

    QVERIFY2(!(mRadioStationModel->findFrequency( KTestFrequency3, foundStation )),			
        "API:RadioStationModel findFrequency 3");

    int stationGenre;
    stationGenre = newStation3.genre();	
    newStation3.setGenre( newStation3.genre() + 1 );
    // allthough genre changed so the frequency update must not become effective  
    mRadioStationModel->saveStation( newStation3 );	

    QVERIFY2(!(mRadioStationModel->findFrequency( KTestFrequency3, foundStation )),			
        "API:RadioStationModel findFrequency 4");
    

    mEnteredSlots = NoSlotsEntered;
    foreach( const RadioStation& station, mRadioStationModel->list() ) 
    {
        if ( station.frequency() == KTestFrequency2 ) 
        {        
            mRadioStationModel->setFavoriteByFrequency( KTestFrequency2, !station.isFavorite() );
        }
    }
    bool correctSignalsReceived = mEnteredSlots.testFlag( StationDataChanged ) && 
        mEnteredSlots.testFlag( FavoriteChanged ) && mEnteredSlots.testFlag( DataChanged );
    QVERIFY2(correctSignalsReceived, "API:RadioStationModel findFrequency 5");
}



/*!
 * Test that stations are stored into RadioStationModel in ascending frequency order  
 */
void TestRadioUiEngine::testSortByFrequency()
{
	int role = RadioStationModel::RadioStationRole;
	int previousFrequency(0); // int not initialized zero as default
	
	for (int i = 0; i< mRadioStationModel->rowCount(); i++ )
	{
		QModelIndex index = mRadioStationModel->index( i, 0 );
		QVariant stationData = mRadioStationModel->data( index, role );
		RadioStation station = stationData.value<RadioStation>();
		// stations must be obtainded at ascending frequency order
		QVERIFY2((station.frequency()>previousFrequency), "API:RadioStationModel testSortByFrequency");
		previousFrequency = station.frequency();
	}
}

/*!
 * Test that preset indexes match
 */
void TestRadioUiEngine::testFindPresetIndex()
{
	RadioStation station;
	for (int i = 0; i< mRadioStationModel->rowCount(); i++ )
    {
		QVERIFY2((mRadioStationModel->findPresetIndex(i) != RadioStation::NotFound), 
	        "API:RadioStationModel testFindPresetIndex 1");
		QVERIFY2((mRadioStationModel->findPresetIndex( i, station ) != RadioStation::NotFound), 
			"API:RadioStationModel testFindPresetIndex 2");
		QVERIFY2((station.presetIndex() == i), "API:RadioStationModel testFindPresetIndex 3");
	}
}

/*!
 * Test that preset can be removed from model by frequency
 */
void TestRadioUiEngine::testRemoveByFrequency()
{
	RadioStation station;
	int initialStationCount( mRadioStationModel->rowCount() );
	int presetIndex(0);
	
	// find valid preset index
	for(int i = 0; i<mRadioStationModel->rowCount(); i++)
	{
		presetIndex = mRadioStationModel->findPresetIndex( i, station );
		if(presetIndex!=RadioStation::NotFound)
			break;
	}
	QVERIFY2((presetIndex != RadioStation::NotFound), "API:RadioStationModel testRemoveByFrequency 1");
		
	mRadioStationModel->removeByFrequency( station.frequency() );	
	QVERIFY2((mRadioStationModel->rowCount()==(initialStationCount-1)), "API:RadioStationModel testRemoveByFrequency 2");
}

/*!
 * Test that station can be removed from model by preset index 
 */
void TestRadioUiEngine::testRemoveByPresetIndex()
{
	RadioStation station;
	station.setFrequency( KTestFrequency4 );
	mStationToBeAdded = "";
	mExpectedStationCount = mRadioStationModel->rowCount() + 1;
	mRadioStationModel->addStation( station );
	// for updating station preset index
	QVERIFY2((mRadioStationModel->findFrequency( KTestFrequency4, station )), 
			"API:RadioStationModel testRemoveByPresetIndex 1");
	
	int initialStationCount( mRadioStationModel->rowCount() );
	mRadioStationModel->removeByPresetIndex( station.presetIndex() );    
    QVERIFY2((mRadioStationModel->rowCount()==(initialStationCount-1)), "API:RadioStationModel testRemoveByPresetIndex 2");
}

/*!
 * Test RadioStationModel method removeStation
 */
void TestRadioUiEngine::testRemoveStation()
{
	RadioStation station;
	station.setFrequency( KTestFrequency5 );
	mStationToBeAdded = "";
	mExpectedStationCount = mRadioStationModel->rowCount() + 1;
	mRadioStationModel->addStation( station );
	int initialStationCount( mRadioStationModel->rowCount() );
	// for updating station preset index
	QVERIFY2(mRadioStationModel->findFrequency( KTestFrequency5, station ),			
	        "API:RadioStationModel testRemoveStation");
	
	mRadioStationModel->removeStation( station );	
	QVERIFY2((mRadioStationModel->rowCount()==(initialStationCount-1)), "API:RadioStationModel testRemoveStation");
}


/*!
 * Test setting and unsetting station type favourite
 */
void TestRadioUiEngine::testSetFavorite()
{	
	RadioStation station;	
	station.setFrequency( KTestFrequency6 );
	station.setType( RadioStation::Favorite );	
    QVERIFY2(station.isFavorite(), "API:RadioStationModel testSetFavorite 1");
	mRadioStationModel->addStation( station );
	mEnteredSlots = NoSlotsEntered;	
	
	mRadioStationModel->setFavoriteByFrequency( KTestFrequency6, false );	
	bool correctSignalsReceived = mEnteredSlots.testFlag( StationDataChanged ) && 
	    mEnteredSlots.testFlag( FavoriteChanged ) && mEnteredSlots.testFlag( DataChanged );
	QVERIFY2(correctSignalsReceived, "API:RadioStationModel testSetFavorite 2");
	
	mRadioStationModel->findFrequency( KTestFrequency6, station );
	QVERIFY2(!station.isFavorite(), "API:RadioStationModel testSetFavorite 3");
	mEnteredSlots = NoSlotsEntered;
	
	mRadioStationModel->setFavoriteByPreset( station.presetIndex(), true );
	correctSignalsReceived = mEnteredSlots.testFlag( StationDataChanged ) && 
		mEnteredSlots.testFlag( FavoriteChanged ) && mEnteredSlots.testFlag( DataChanged );
	QVERIFY2(correctSignalsReceived, "API:RadioStationModel testSetFavorite 4");
    
	// test toggling the favorite 
	QModelIndex index = mRadioStationModel->modelIndexFromFrequency( KTestFrequency6 );
    mRadioStationModel->setData( index, KTestFrequencyString6 ,RadioStationModel::ToggleFavoriteRole);
    RadioStation station1 = mRadioStationModel->stationAt( index.row() );
    QVERIFY2(!station1.isFavorite(), "API:RadioStationModel testToggleFavourite");

}

/*!
 * Test renaming station
 */
void TestRadioUiEngine::testRenameStation()
{	
	RadioStation station;
	int presetIndex(0);
	// find valid preset index
    for( int i = 0; i<mRadioStationModel->rowCount(); i++ )
	{
		if( mRadioStationModel->findPresetIndex( i, station ) != RadioStation::NotFound )
		{
			presetIndex = i;
			break;
		}
	}   
    QString initialStationName( station.name() );
    mEnteredSlots = NoSlotsEntered;
	mRadioStationModel->renameStation( presetIndex, initialStationName + "Renamed" );	
	bool correctSignalsReceived = mEnteredSlots.testFlag( StationDataChanged ) && 
        mEnteredSlots.testFlag( DataChanged );
	QVERIFY2(correctSignalsReceived, "API:RadioStationModel testRenameStation 1");	
    QVERIFY2(initialStationName!=mRadioStationModel->stationAt(presetIndex).name(), "API:RadioStationModel testRenameStation 2");
        
    station.setUserDefinedName( KTestStationName2 );
    QVERIFY2(station.isRenamed(), "API:RadioStationModel testRenameStation 1");
    
}

/*!
 * Test setting radio text
 */
void TestRadioUiEngine::testSetRadioTextPlus()
{
    RadioStation station;
    mRadioStationModel->findFrequency( KTestFrequency6, station );
    station.setRadioText( "" );
    station.setRadioText( KTestRadioTextRadioText );
    station.setRadioText( KTestRadioTextRadioText );
    station.setRadioTextPlus( RtPlus::Artist, KTestRadioTextPlusArtist );
    station.setRadioTextPlus( RtPlus::Title, KTestRadioTextPlusTitle );
    station.setRadioTextPlus( RtPlus::Homepage, KTestRadioTextPlusUrl );
    station.setRadioTextPlus( RtPlus::Homepage + 1, KTestRadioTextPlusUnsupportedTag );
    mRadioStationModel->saveStation( station );
    bool effective = station.radioText().contains( KTestRadioTextPlusArtist, Qt::CaseSensitive );
    QVERIFY2(effective, "API:RadioStationModel testSetRadioTextPlus 1");
    effective = station.radioText().contains( KTestRadioTextPlusTitle, Qt::CaseSensitive );
    QVERIFY2(effective, "API:RadioStationModel testSetRadioTextPlus 2");
    effective = !station.url().compare( KTestRadioTextPlusUrl );
    QVERIFY2(effective, "API:RadioStationModel testSetRadioTextPlus 3");
}

/*!
 * Test RadioStationModel API
 */	
void TestRadioUiEngine::testRadioStationModel()
{
	testRadioStationModelInit();
	testAddStation1();
	testSaveStation1();
	testAddStation2();
	testSaveStation2();
	testAddStation3();
	testSaveStation3();
	testSortByFrequency();
	testFindPresetIndex();
	testRemoveByFrequency();
	testRemoveByPresetIndex();
	testRemoveStation();
	testSetFavorite();
	testRenameStation();
	testSetRadioTextPlus();
}
/*!
 * Test RadioHistoryModel API
 */
void TestRadioUiEngine::testhistoryModel()
    {
    testHistoryModelInit();
    testHistoryModelAddItem();
    testHistoryModelFindItem();
    testHistoryModelUpdateItem();
    testHistoryModelSetData();
    testAddRadioTextPlus();
    testClearRadioTextPlus();
    }

/*!
 * Testing of RadioHistoryModel initial state
 */
void TestRadioUiEngine::testHistoryModelInit()
{
    //TODO:: Check why ASSERT fails when mhistoryModel->rowCount() == 0 
    if( mhistoryModel->rowCount()>0 )
    {       
        mhistoryModel->removeAll();
        QVERIFY2((mRadioStationModel->rowCount()==0), "API:RadioHistoryModel removeAll() 1");
    }
    QVERIFY2((mhistoryModel->rowCount()==0), "API:RadioHistoryModel removeAll() 2");           
}

/*!
 *
 */
void TestRadioUiEngine::testHistoryModelAddItem()
{
    int expectedHistoryItemCount( mhistoryModel->rowCount() + 1 );
    mEnteredSlots = NoSlotsEntered;
    mhistoryModel->addItem( KTestArtist1, KTestTitle1 );    
    bool correctSignalsReceived = mEnteredSlots.testFlag( ItemAdded );
    // check that correct signals received
    QVERIFY2(correctSignalsReceived, "API:RadioHistoryModel addItem() 1");
    // check that item count increases
    QVERIFY2((mhistoryModel->rowCount()==expectedHistoryItemCount), "API:RadioHistoryModel addItem() 2");
    
    // check that artist/title stored into the model conforms to the one read from the model
    QModelIndex index = mhistoryModel->index( 0, 0 );
    QStringList stringList = mhistoryModel->data( index, Qt::DisplayRole ).value<QStringList>();        
    QString artistTitle = stringList.at(0);    
    QVERIFY2(!(artistTitle.compare(KTestArtist1+" - "+KTestTitle1)), "API:RadioHistoryModel addItem() 3");
    
    
    expectedHistoryItemCount = mhistoryModel->rowCount();
    // trying to add an item that allready exists must not increase the item count
    mhistoryModel->addItem( KTestArtist1, KTestTitle1 );
    QVERIFY2((mhistoryModel->rowCount()==expectedHistoryItemCount), "API:RadioHistoryModel addItem() 4");
}

/*!
 *
 */
void TestRadioUiEngine::testHistoryModelFindItem()
{
    RadioHistoryItem item;
    mhistoryModel->findItem( KTestArtist1, KTestTitle1, item );
    QVERIFY2((item.artist()==KTestArtist1), "API:RadioHistoryModel findItem() 1");
    QVERIFY2((item.title()==KTestTitle1), "API:RadioHistoryModel findItem() 2");
    // try to find an item that doesn't exist
    int ret = mhistoryModel->findItem( KTestArtist1+"+", KTestTitle1, item );
    QVERIFY2(ret==-1, "API:RadioHistoryModel findItem() 3");
}

/*!
 *
 */
void TestRadioUiEngine::testHistoryModelUpdateItem()
{
    RadioHistoryItem item;    
    mhistoryModel->findItem( KTestArtist1, KTestTitle1, item );
    item.setTitle( KTestTitle2 );    
    // update an existing item
    mhistoryModel->updateItem( 0, item, true );
    
    RadioHistoryItem foundItem;
    // original item must not be found any more
    int ret = mhistoryModel->findItem(KTestArtist1, KTestTitle1, foundItem);
    QVERIFY2(ret==-1, "API:RadioHistoryModel updateItem() 1");
    // but the updated one instead
    mhistoryModel->findItem(KTestArtist1, KTestTitle2, foundItem);
    QVERIFY2((foundItem.title()==KTestTitle2), "API:RadioHistoryModel updateItem() 2"); 
}

/*!
 *
 */
void TestRadioUiEngine::testHistoryModelSetData()
{
   RadioHistoryItem foundItem;
   mhistoryModel->findItem(KTestArtist1, KTestTitle2, foundItem);
   QVERIFY2(!foundItem.isFavorite(), "API:RadioHistoryModel SetData() 1");
   
   QModelIndex index = mhistoryModel->index( 0, 0 );
   QString artistTitle = KTestArtist1 + KTestTitle2;
   mhistoryModel->setData( index, artistTitle, RadioHistoryModel::SetFavoriteRole );
   mhistoryModel->findItem(KTestArtist1, KTestTitle2, foundItem);
   // item should be now favorite
   QVERIFY2(foundItem.isFavorite(), "API:RadioHistoryModel SetData() 2");    
}

/*!
 *
 */
void TestRadioUiEngine::testAddRadioTextPlus()
{   
    mhistoryModel->addRadioTextPlus( RtPlus::Artist, KTestArtist1 );
    mhistoryModel->addRadioTextPlus( RtPlus::Title, KTestTitle1 );
}

/*!
 *
 */
void TestRadioUiEngine::testClearRadioTextPlus()
{   
    mhistoryModel->clearRadioTextPlus();
}

/*!
 * Test RadioHistoryModelItem API
 */
void TestRadioUiEngine::testHistoryModelItem()
{
    RadioHistoryItem* item = new RadioHistoryItem( KTestArtist3, KTestTitle3 );
    item->setArtist( KTestArtist3 );
    QVERIFY2(!(item->artist().compare(KTestArtist3)), "API:testHistoryModelItem setArtist()");
    item->setTitle( KTestTitle3 );
    QVERIFY2(!(item->title().compare(KTestTitle3)), "API:testHistoryModelItem setTitle()");
    QVERIFY2(!item->isFavorite(), "API:testHistoryModelItem isFavorite() 1");
    item->setFavorite();
    QVERIFY2(item->isFavorite(), "API:testHistoryModelItem isFavorite() 2");
    item->setFavorite();
    delete item;
    item = NULL;
}
