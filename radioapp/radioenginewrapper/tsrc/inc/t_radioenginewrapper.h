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


#ifndef T_RADIOENGINEWRAPPER_H_
#define T_RADIOENGINEWRAPPER_H_


// INCLUDES
#include <QtTest/QtTest>
#include <e32property.h>
#include <centralrepository.h>

#include "radioenginewrapperobserver.h"
#include "radiostationhandlerif.h"
//#include "radiostation.h"
#include "radiostubmanager.h"
#include "t_schedulerstartandstoptimer.h"

class RadioUiEngine;
class RadioStationModel;
class RadioPlayLogModel;
class RadioPresetStorage;
class RadioEngineWrapper;
//class RadioStationHandlerIf;

class TestRadioEngineWrapper : public QObject, RadioStationHandlerIf, RadioEngineWrapperObserver, MSchedulerStartAndStopTimerObserver
{
    Q_OBJECT

    /**
     * Flags to indicate which slots have been entered since calling API method. 
     * Declared to use QFlags<> to ease flag usage and to enforce type safety.
     */
    enum SlotEnteredFlag
    {
    	NoSlotsEntered         = 0
        ,HeadsetConnected       = 1 << 0
        ,SeekingStarted         = 1 << 1
        ,TunedToFrequency       = 1 << 2
        ,MuteChanged            = 1 << 3
        ,VolumeChanged          = 1 << 4
        ,ItemAdded              = 1 << 5
    };
    Q_DECLARE_FLAGS( Slots, SlotEnteredFlag )    
    
public:

    TestRadioEngineWrapper();
    ~TestRadioEngineWrapper();    

public slots:
//    void dataChanged(const QModelIndex topLeft, const QModelIndex bottomRight);
//    void stationAdded( RadioStation addedStation );    
//    void stationDataChanged( RadioStation station );        
//    void favoriteChanged( RadioStation station );
//    void itemAdded();
    
private slots:
    // test framework called slots 
    void initTestCase();
    void init();
    void cleanup();
    
    void testIsEngineConstructed();
    
    void testRadioSettingsReference();
    
    void testRegion();
    
    void testRadioOnOff();

    void testTuning();
    
    void testTuningWithDelay();
    
    void testMuteToggling();
    
    void testVolumeSetting();
    
    void testLoudSpeakerUsage();
    
    void testScanning();
    
    void cleanupTestCase();
    
private:

    // from base class RadioStationHandlerIf =>
    uint currentFrequency() const;
    int currentPresetIndex() const;

    void setCurrentStation( uint frequency );

    bool containsFrequency( uint frequency );

    bool containsPresetIndex( int presetIndex );

    void startDynamicPsCheck();

    void addScannedFrequency( uint frequency );

    void removeLocalStations();

    void setCurrentPsName( uint frequency, const QString& name );
    void setCurrentRadioText( uint frequency, const QString& radioText );
    void setCurrentRadioTextPlus( uint frequency, int rtClass, const QString& rtItem );
    void setCurrentPiCode( uint frequency, int piCode );
    void setCurrentGenre( uint frequency, int genre );
    // <= from base class RadioStationHandlerIf 
    
    // c =>
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
    // <= from base class RadioStationHandlerIf 
      
    // subfunctions used by the test framework called slots =>
    // void testRadioStationModelInit();

    // from base class MSchedulerStartAndStopTimerObserver =>
    void Timeout( TUint aTimerId );    
    void CreateMUT();
    void DeleteMUT();
    // <=
    
    void tstSetTunerCababilities(uint category = 0);
    void tstSetFrequency( TUint aFrequency );
    TInt tstGetFrequency();
    void tstSetScanningData( TUint aCount, TInt aMinFreq, TInt aFrequencyStepSize );
    void tstDefineAndAttachRadioServerProperties();
    void tstCreateCRObjects();
    
private:
    RadioEngineWrapper* mEngineWrapper;

    // RadioStubManager pointer points inside RadioStubManagerChunk
    SRadioStubManager* mRadioStubManager;
    
    // RadioStubManagerChunk handle
    RChunk mRadioStubManagerChunk;

    // Active scheduler
    // Workaround for the below panic, occured after porting to 10.1 
    // Main Panic E32USER-CBase 44
    // Create and install the active scheduler
    CActiveScheduler* mScheduler;
    
    //QScopedPointer<RadioEngineWrapper>  mEngineWrapper;

    //int mExpectedStationCount;
    
	Slots mEnteredSlots;
	//QString mStationToBeAdded;
	
	CSchedulerStopAndStartTimer* mSchedulerTimer;

    RProperty mPropertyPlayerState;
    RProperty mPropertyAntennaStatus;
    RProperty mPropertyOfflineMode;
    RProperty mPropertyTransmitterStatus;
    RProperty mPropertyFrequency;
    RProperty mPropertyFrequencyRange;
    RProperty mPropertyForceMonoReception;
    RProperty mPropertySquelch;
    RProperty mPropertyVolume;
    RProperty mPropertyMuteStatus;
    RProperty mPropertyBalance;

    CRepository* mRadioCR;
    
    TInt mLastRecordedVolume; 
};

#endif /* T_RADIOENGINEWRAPPER_H_ */
