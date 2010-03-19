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


#ifndef T_RADIOSTATION_H_
#define T_RADIOSTATION_H_


// INCLUDES
#include <QtTest/QtTest>

#include "radioenginewrapperobserver.h"
#include "radiostation.h"

class RadioUiEngine;
class RadioStationModel;
class RadioPlayLogModel;
class RadioPresetStorage;
class RadioEngineWrapper;
class RadioEngineWrapperObserver;

class TestRadioUiEngine : public QObject, RadioEngineWrapperObserver
{
    Q_OBJECT

    /**
     * Flags to indicate which slots have been entered since calling API method. 
     * Declared to use QFlags<> to ease flag usage and to enforce type safety.
     */
    enum SlotEnteredFlag
    {
    	NoSlotsEntered               = 0
        ,StationDataChanged     = 1 << 0
        ,FavoriteChanged        = 1 << 1
        ,StationAdded           = 1 << 2
        ,DataChanged            = 1 << 3
        ,ItemAdded              = 1 << 4
    };
    Q_DECLARE_FLAGS( Slots, SlotEnteredFlag )    
    
public:

    TestRadioUiEngine();
    ~TestRadioUiEngine();    

public slots:
    void dataChanged(const QModelIndex topLeft, const QModelIndex bottomRight);
    void stationAdded( RadioStation addedStation );    
    void stationDataChanged( RadioStation station );        
    void favoriteChanged( RadioStation station );
    void itemAdded();
    
private slots:
    // test framework called slots 
    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();
    void testImplicitSharing();
    void testChangeFlags();
    void TestCallSignChar();
    void testPICodeToCallSign();
    void testRadioStationModel();
    void testPlayLogModel();
    void testPlayLogModelItem();
    
private:

    // from base class RadioEngineWrapperObserver
    void tunedToFrequency( uint frequency, int commandSender );
    void seekingStarted( Seeking::Direction direction );
    void radioStatusChanged( bool radioIsOn );
    void rdsAvailabilityChanged( bool available );
    void volumeChanged( int volume );
    void muteChanged( bool muted );
    void audioRouteChanged( bool loudspeaker );
    void scanAndSaveFinished();
    void headsetStatusChanged( bool connected );
    void skipPrevious();
    void skipNext();
    // subfunctions used by the test framework called slots
    void testRadioStationModelInit();
    void testAddStation1();
    void testSaveStation1();
    void testAddStation2();
    void testSaveStation2();
    void testAddStation3();
    void testSaveStation3();
    void testSortByFrequency();
    void testFindPresetIndex();
    void testRemoveByFrequency();
    void testRemoveByPresetIndex();
    void testRemoveStation();

    void testSetFavorite();

    void testRenameStation();
    void testSetRadioTextPlus();
    void testPlayLogModelInit();
    void testPlayLogModelAddItem();
    void testPlayLogModelFindItem();
    void testPlayLogModelUpdateItem();
    void testPlayLogModelSetData();
    void testAddRadioTextPlus();
    void testClearRadioTextPlus();
    void testRadioPlayLogItem();
    
private:
	RadioUiEngine* mUiEngine;
	QScopedPointer<RadioEngineWrapper>  mEngineWrapper;
	RadioStationModel* mRadioStationModel;
	RadioPlayLogModel* mPlayLogModel;
	QScopedPointer<RadioPresetStorage>  mPresetStorage;
	int mExpectedStationCount;	
	/**
	 * Internal book keeping used to determine which slots have been entered since calling RadioStationModel 
	 * API method. Used to conclude if the correct signals from RadioStationModel have been received.
	 */
	Slots mEnteredSlots;
	QString mStationToBeAdded;
	QString mStationToBeSaved;
};

#endif /* T_RADIOSTATION_H_ */
