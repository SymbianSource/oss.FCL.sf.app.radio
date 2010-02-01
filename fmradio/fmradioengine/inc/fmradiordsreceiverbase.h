/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The RDS receiver base class for FM Radio
*
*/


#ifndef CFMRADIORDSRECEIVERBASE_H
#define CFMRADIORDSRECEIVERBASE_H

#include <RadioRdsUtility.h>

#include "fmradiordsobserver.h"

class TRadioSettings;
class CRadioUtility;
class CFMRadioPubSub;

NONSHARABLE_CLASS( CFMRadioRdsReceiverBase ) : public CBase, public MRadioRdsObserver
    {
public:

    enum TFMRadioProgrammeSeviceType
        {
        EFMRadioPSNameStaticAssumed = 0,// Default, determination is ongoing
        EFMRadioPSNameStatic,           // PS name is static
        EFMRadioPSNameDynamic           // PS name is dynamic
        };
    
    /**
     * The destructor
     */
    virtual ~CFMRadioRdsReceiverBase();

    /**
     * Initializes the RDS receiver
     *
     * @param aRadioUtility A reference to the radio utility
     * @param aPubSub       Pointer to Publish&Subscribe object
     */
    virtual void InitL( CRadioUtility& aRadioUtility, CFMRadioPubSub* aPubSub ) = 0;

    /**
     * Adds an observer wich will be notified of the new RDS data
     * If observer already exists, it is not added
     *
     * @param aObserver pointer of the observer instance.
     */
    IMPORT_C void AddObserver(MFMRadioRdsObserver* aObserver);

    /**
     * Removes the rds observer.
     */
    IMPORT_C void RemoveObserver(MFMRadioRdsObserver* aObserver);

    /**
     * Getter for automatic switching state of alternate frequencies
     *
     * @return ETrue if automatic switching is enabled
     */
    IMPORT_C TBool AutomaticSwitchingEnabled() const;

    /**
     * Getter for the programme service name
     *
     * @return Programme service name
     */
    IMPORT_C const TDesC& ProgrammeService() const;

    /**
     * Returns the nature of the PS name
     *
     * @return The type of PS name
     */
    IMPORT_C TFMRadioProgrammeSeviceType ProgrammeServiceNameType() const;
    
    /**
     * Getter for the RDS signal availability
     *
     * @return ETrue if RDS signal is available, otherwise EFalse
     */
    IMPORT_C TBool SignalAvailable() const;

    /**
     * Getter for the RDS Text+ station URL
     * @return The cached web URL of the station
     */
    IMPORT_C const TDesC& RtPlusProgramUrl() const;
    
    /**
     * Getter for the RDS Text+ artist name
     * @return The cached artist name
     */
    IMPORT_C const TDesC& RtPlusArtist() const;
    
    /**
     * Getter for the RDS Text+ album name
     * @return The cached album name
     */
    IMPORT_C const TDesC& RtPlusAlbum() const;
    
    /**
     * Getter for the RDS Text+ song name
     * @return The cached song name
     */
    IMPORT_C const TDesC& RtPlusSong() const;
    
    /**
     * Setter for automatic switching of alternate frequencies
     * If this method is overridden, the base implementation should be called
     *
     * @param aEnable ETrue, if automatic swithing is to be enabled
     */
    virtual void SetAutomaticSwitchingL( TBool aEnable );
    
    /**
     * Clears the RDS information
     */
    void ClearRdsInformation();
    
    /**
     * Starts receiving the RDS information
     */
    virtual void StartReceiver() = 0;
    
    /**
     * Stops receiving the RDS information
     */
    virtual void StopReceiver() = 0;
    
protected:

    /**
     * The default constructor
     *
     * @param aSettings The radio settings
     */
    CFMRadioRdsReceiverBase( TRadioSettings& aSettings );

    /**
     * Second phase constructor
     */
    void BaseConstructL();

    /**
     * Static callback for the RDS PS name timer
     *
     * @param   aSelf   Pointer to self.
     * @return  KErrNone
     */
    static TInt StaticPsNameTimerCallback( TAny* aSelf );
    
    /**
     * Handles the received RT+ data and passes it to observers
     * @param aRtPlusClass The RT+ class
     * @param aRtPlusData The received data
     */
    void HandleRdsDataRTplusL( TRdsRTplusClass aRtPlusClass, TRdsRadioText& aRtPlusData );
    
// from base class MRadioRdsObserver
    void MrroStationSeekByPTYComplete( TInt aError, TInt aFrequency );
    void MrroStationSeekByTAComplete( TInt aError, TInt aFrequency );
    void MrroStationSeekByTPComplete( TInt aError, TInt aFrequency );
    void MrroGetFreqByPTYComplete( TInt aError, RArray<TInt>& aFreqList );
    void MrroGetFreqByTAComplete( TInt aError, RArray<TInt>& aFreqList );
    void MrroGetPSByPTYComplete( TInt aError, RArray<TRdsPSName>& aPsList );
    void MrroGetPSByTAComplete( TInt aError, RArray<TRdsPSName>& aPsList );
    void MrroRdsDataPI( TInt aPi );
    void MrroRdsDataPTY( TRdsProgrammeType aPty );
    void MrroRdsDataPS( TRdsPSName& aPs );
    void MrroRdsDataRT( TRdsRadioText& aRt );
    void MrroRdsDataCT( TDateTime& aCt );
    void MrroRdsDataTA( TBool aTaOn );
    void MrroRdsDataRTplus( TRdsRTplusClass aRtPlusClass, TRdsRadioText& aRtPlusData );
    void MrroRdsSearchBeginAF();
    void MrroRdsSearchEndAF( TInt aError, TInt aFrequency );
    void MrroRdsStationChangeTA( TInt aFrequency );
    void MrroRdsEventAutomaticSwitchingChange( TBool aAuto );
    void MrroRdsEventAutomaticTrafficAnnouncement( TBool aAuto );
    void MrroRdsEventSignalChange( TBool aSignal );
    
protected: // data
    /** The radio settings */
	TRadioSettings& iSettings;

    /** Publish&Subscribe object. Can be NULL. Not owned. */
    CFMRadioPubSub* iPubSub;
    
private: // data
    
    /** Array of RDS observers. */
    RPointerArray<MFMRadioRdsObserver> iObservers;
    
    /** The programme service name cache */
    HBufC* iPsName;
    
    /** The radio text data cache */
    HBufC* iRadioText;
    
    /** The cached web URL of the station */
    RBuf iProgramWebUrl;
    
    /** The cached artist name */
    RBuf iArtistName;
    
    /** The cached song name */
    RBuf iSongName;
    
    /** The cached album name */
    RBuf iAlbumName;
    
    /** ETrue if the RDS signal is available */
    TBool iSignalAvailable;

    /** ETrue if AF is enabled */
    TBool iAfEnabled;
    
    /** Timer to track the PS name changes */
    CPeriodic* iPsNameTimer;

    /** The type of the PS name */
    TFMRadioProgrammeSeviceType iPsNameType;
    };

#endif // CFMRADIORDSRECEIVERBASE_H
