/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "t_radiopresetstorage.h"
#include "radiopresetstorage.h"
#include "radiostation.h"
#include "radiostation_p.h"

// Constants
const uint KTestFrequency1 = 89000000;
const uint KTestFrequency2 = 89500000;
const uint KTestFrequency3 = 90000000;
const uint KTestFrequency4 = 90500000;
const uint KMaxNumberOfPresets = 100;
const uint KInvalidPresetIndex = -2;
const uint KFirstPresetIndex = 0;
const uint KSecondPresetIndex = 1;
const uint KThirdPresetIndex = 2;
const uint KFrequencyStep = 100000;

/*!
 *
 */
int main(int /* argc*/, char *argv[])
{
    TestRadioPresetStorage tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testradiopresetstorage.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}


TestRadioPresetStorage::TestRadioPresetStorage()
{
}


/*!
 * Destructor
 */
TestRadioPresetStorage::~TestRadioPresetStorage()
{
}

/*!
 * called before each testfunction is executed
 */
void TestRadioPresetStorage::init()
{
}

/*!
 * called after every testfunction
 */
void TestRadioPresetStorage::cleanup()
{
}

/*!
 * called before the first testfunction is executed
 */
void TestRadioPresetStorage::initTestCase()
{
    mPresetStorage.reset( new RadioPresetStorage() );
    // this is the range used in this test module
    for(int i = 0; i <= mPresetStorage->maxNumberOfPresets(); i++)
    {
        mPresetStorage->deletePreset( i );        
    }    
    QVERIFY2(( mPresetStorage->presetCount() == 0 ), "API:RadioPresetStorage initTestCase 1");    
    // TODO: replace the local constant with the constand defined in preset utility header  
    QVERIFY2(( mPresetStorage->maxNumberOfPresets() == KMaxNumberOfPresets ), "API:RadioPresetStorage initTestCase 2");
}

/*!
 * called after the last testfunction was executed
 */
void TestRadioPresetStorage::cleanupTestCase()
{
	
}

/*!
 * Test saving preset 
 */
void TestRadioPresetStorage::testSavePreset()
{
  int test = mPresetStorage->maxNumberOfPresets();
  int initialPresetCount( 0 );
  //int initialPresetindex( KFirstPresetIndex );  
  RadioStation station;  
  station.setFrequency( KTestFrequency1 );
  station.setPresetIndex( KFirstPresetIndex );
  mPresetStorage->savePreset( *station.data_ptr() );
  QVERIFY2(( mPresetStorage->presetCount() == initialPresetCount + 1 ), "API:RadioPresetStorage testSavePreset 1");
  QVERIFY2(( mPresetStorage->nextPreset( KFirstPresetIndex ) == 0 ), "API:RadioPresetStorage testSavePreset 2");
  
  station.setFrequency( KTestFrequency2 );
  station.setPresetIndex( KSecondPresetIndex );
  mPresetStorage->savePreset( *station.data_ptr() );
  QVERIFY2(( mPresetStorage->presetCount() == initialPresetCount + 2 ), "API:RadioPresetStorage testSavePreset 3");
  QVERIFY2(( mPresetStorage->nextPreset( KFirstPresetIndex ) == KSecondPresetIndex ), "API:RadioPresetStorage testSavePreset 4");
  
  // check that saving preset fails if preset index and frequency are allready in use
  station.setFrequency( KTestFrequency2 );
  station.setPresetIndex( KSecondPresetIndex );
  mPresetStorage->savePreset( *station.data_ptr() );
  QVERIFY2(( mPresetStorage->presetCount() == initialPresetCount + 2 ), "API:RadioPresetStorage testSavePreset 5");
  
  // check that saving preset success when new preset index and frequency are not in use
  station.setFrequency( KTestFrequency3 );
  station.setPresetIndex( KThirdPresetIndex );  
  mPresetStorage->savePreset( *station.data_ptr() );
  QVERIFY2(( mPresetStorage->presetCount() == initialPresetCount + 3 ), "API:RadioPresetStorage testSavePreset 6");
  
  // check that saving with invalid preset index fails
  station.setFrequency( KTestFrequency4 );
  station.setPresetIndex( KInvalidPresetIndex );  
  mPresetStorage->savePreset( *station.data_ptr() );
  QVERIFY2(( mPresetStorage->presetCount() == initialPresetCount + 3 ), "API:RadioPresetStorage testSavePreset 6");
}

/*!
 * Test reading preset 
 */
void TestRadioPresetStorage::testReadPreset()
{
  RadioStation station;
  RadioStationIf* preset = static_cast<RadioStationIf*>( station.data_ptr() );
  mPresetStorage->readPreset( KFirstPresetIndex, *preset );  
  QVERIFY2(( preset->frequency() == KTestFrequency1 ), "API:RadioPresetStorage testReadPreset 1");
  
  RadioStation station1;
  RadioStationIf* preset1 = static_cast<RadioStationIf*>( station1.data_ptr() );
  mPresetStorage->readPreset( KSecondPresetIndex, *preset1 );
  QVERIFY2(( preset->frequency() == KTestFrequency2 ), "API:RadioPresetStorage testReadPreset 2");  
  
  RadioStation station2;
  RadioStationIf* preset2 = static_cast<RadioStationIf*>( station2.data_ptr() );
  mPresetStorage->readPreset( KThirdPresetIndex, *preset2 );
  QVERIFY2(( preset->frequency() == KTestFrequency3 ), "API:RadioPresetStorage testReadPreset 3");
  
  RadioStation station3;
  RadioStationIf* preset3 = static_cast<RadioStationIf*>( station3.data_ptr() );
  // test the value returned with invalid preset index
  mPresetStorage->readPreset( KThirdPresetIndex + 1, *preset3 );
  // TODO: change the value to KErrNotFound when preset utility update done
  QVERIFY2(( preset->frequency() == 87500000 ), "API:RadioPresetStorage testReadPreset 4");
}

/*!
 * Test deleting preset
 */
void TestRadioPresetStorage::testDeletePreset()
{   
    int initialPresetCount( mPresetStorage->presetCount() );
    
    QVERIFY2(( mPresetStorage->firstPreset() == KFirstPresetIndex ), "API:RadioPresetStorage testDeletePreset 1");
    QVERIFY2(( mPresetStorage->nextPreset( KFirstPresetIndex ) == KSecondPresetIndex ), "API:RadioPresetStorage testDeletePreset 2");
    mPresetStorage->deletePreset( KSecondPresetIndex );
    QVERIFY2(( mPresetStorage->nextPreset( KFirstPresetIndex ) == KThirdPresetIndex ), "API:RadioPresetStorage testDeletePreset 3");
    QVERIFY2(( mPresetStorage->presetCount() == (initialPresetCount - 1)), "API:RadioPresetStorage testDeletePreset 4");       
    
    RadioStation station;
    RadioStationIf* preset = static_cast<RadioStationIf*>( station.data_ptr() );
    mPresetStorage->readPreset( KSecondPresetIndex, *preset );
    // TODO: change the value to KErrNotFound when preset utility update done
    QVERIFY2(( preset->frequency() == 87500000 ), "API:RadioPresetStorage testDeletePreset 5");
    
    mPresetStorage->deletePreset( KFirstPresetIndex );
    QVERIFY2(( mPresetStorage->firstPreset() == KThirdPresetIndex ), "API:RadioPresetStorage testDeletePreset 6");
    mPresetStorage->deletePreset( KThirdPresetIndex );
    QVERIFY2(( mPresetStorage->presetCount() == 0), "API:RadioPresetStorage testDeletePreset 7");
}
  
/*!
 * Test filling preset storage and handling boundary values
 */
void TestRadioPresetStorage::testStessTest()
{
    for(int i = 0; i < mPresetStorage->maxNumberOfPresets(); i++)
    {
        RadioStation station;
        station.setFrequency( 87500000 + i*KFrequencyStep );
        station.setPresetIndex( i );  
        mPresetStorage->savePreset( *station.data_ptr() );
    }
    QVERIFY2(( mPresetStorage->presetCount() == mPresetStorage->maxNumberOfPresets()), "API:RadioPresetStorage testStessTest 1");
    int test = mPresetStorage->presetCount();
    RadioStation station;
    station.setFrequency( 87500000 + mPresetStorage->maxNumberOfPresets()*KFrequencyStep + KFrequencyStep);
    station.setPresetIndex( mPresetStorage->maxNumberOfPresets() );  
    mPresetStorage->savePreset( *station.data_ptr() );
    int r = mPresetStorage->presetCount();
    // preset count must not increase
    QVERIFY2(( mPresetStorage->presetCount() == mPresetStorage->maxNumberOfPresets()), "API:RadioPresetStorage testStessTest 2");
    
    for(int i = 0; i <= mPresetStorage->maxNumberOfPresets(); i++)
    {
        mPresetStorage->deletePreset( i );        
    }  

    // test deleting with invalid preset index
    mPresetStorage->deletePreset( KInvalidPresetIndex );
    QVERIFY2(( mPresetStorage->presetCount() == 0), "API:RadioPresetStorage testStessTest 3");
}
