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
* Description:  The class implements the core functionality of the
*                Radio Engine
*
*/


#ifndef FMRADIOENGINE_H
#define FMRADIOENGINE_H

#include <e32base.h>
#include <fmradiointernalpskeys.h>
#include <RadioUtility.h>
#include <RadioFmTunerUtility.h>
#include <RadioPlayerUtility.h>
#include <RadioPresetUtility.h>
#include <RadioFmPresetUtility.h>
#include <etelmm.h>

#include "fmradiosystemeventdetectorobserver.h"
#include "fmradionetworkchangeobserver.h"
#include "fmradiordsobserver.h"
#include "fmradioengine.hrh"
#include "fmradioheadseteventobserver.h"

#ifndef __ACCESSORY_FW
#include <DosSvrServices.h>
#endif

// CONSTANTS
const TUint8 KPresetNameLength = 32;
const TInt KDefaultRadioFrequency = 87500000;
_LIT(KFMRadioEngineResourceFile, "fmradioengine.rsc");
const TInt KFMRadioFreqMultiplier = 1000;

// FORWARD DECLARATIONS
class CAudioOutput;
class CCentralRepositoryHandler;
class CDesC16Array;
class CFMRadioAccessoryConnection;
class CFMRadioContextPublisher;
class CFMRadioMobileNetworkInfoListener;
class CFMRadioPubSub;
class CFMRadioRdsReceiverBase;
class CFMRadioSystemEventDetector;
class CRadioStateHandler;
class MRadioEngineStateChangeCallback;
class TRadioSettings;

#ifndef __ACCESSORY_FW
class RDosServer;
class CDosServerObserver;
#endif

// CLASS DECLARATION

/**
 * This class implements the core functionality of the Radio Engine
 *
 * @since S60 3.2
 */
class CRadioEngine : public CBase,
                     public MRadioPlayerObserver,
                     public MRadioFmTunerObserver,
                     public MRadioPresetObserver,
                     public MFMRadioSystemEventDetectorObserver,
                     public MFMRadioNetworkChangeObserver,
                     public MFMRadioRdsObserver,
                     public MFMRadioHeadsetEventObserver

    {
public:

    typedef TBuf<KPresetNameLength> TStationName;

    enum TRadioMode
        {
        ERadioTunerMode,
        ERadioPresetMode
        };

    enum TFMRadioAudioMode
        {
        EFMRadioStereo = 0,
        EFMRadioMono
        };

    enum TFMRadioAudioOutput
        {
        EFMRadioOutputHeadset = 0,
        EFMRadioOutputIHF
        };
    
public:

    /**
    * Two-phased class constructor.
    * @return pointer to CRadioEngine class instance
    */
    IMPORT_C static CRadioEngine* NewL( MRadioEngineStateChangeCallback& aCallback );
    /**
    * Destructor of CRadioEngine class.
    */
    IMPORT_C virtual ~CRadioEngine();

public: // New functions

    /**
    * Set radio settings to their default values.
    * @since Series 60 3.0
    * @return none
    */
    IMPORT_C void InitializeRadio();
    /**
    * Retrieves the current radio mode .
    * @since Series 60 3.0
    * @return tune/preset
    */
    IMPORT_C TRadioMode GetRadioMode() const;

    /**
    * Gets the current audio mode.
    * @since Series 60 3.0
    * @return stereo/mono
    * @retval 0 = stereo
    * @retval 1 = mono
    */
    IMPORT_C TInt GetAudioMode() const;

    /**
    * Set the radio's audio mode.
    * @since Series 60 3.0
    * @param aAudioMode the new audio mode
    * @return none
    */
    IMPORT_C void SetAudioMode( const TFMRadioAudioMode aAudioMode );

    /**
    * get the currently selected channel index.
    * @since Series 60 3.0
    * @return none
    */
    IMPORT_C TInt GetPresetIndex() const;

    /**
    * Update the channel located at the index to use the frequency specified.
    * @since Series 60 3.0
    * @param aIndex index into the channel array.
    * @param aStationName name of the channel
    * @param aFrequency frequency value to be used.
    * @return none
    */
    IMPORT_C void SetPresetNameFrequencyL( TInt aIndex, const TStationName& aStationName, TInt aFrequency );

    /**
    * Retrieve the channel frequency at the index specified
    * @since Series 60 3.0
    * @param aIndex  index into the channel array.
    * @return the frequency
    */
    IMPORT_C TInt GetPresetFrequencyL(TInt aIndex) const;

    /**
    * Retrieve the channel name at the index specified
    * @since Series 60 3.0
    * @param aIndex  index into the channel array.
    * @return the channel name
    */
    IMPORT_C CRadioEngine::TStationName& GetPresetNameL( TInt aIndex );
    
    /**
    * Retrieve the channel name and the frequency at the index specified
    * @param aIndex station index
    * @param aName stored station name
    * @param aFrequency preset frequency
    */
    IMPORT_C void GetPresetNameAndFrequencyL( TInt aIndex, TDes& aName, TInt& aFrequency );

    /**
    * Starts the radio.
    * @since Series 60 3.0
    * @return none
    */
    IMPORT_C void RadioOn();

    /**
    * Stops the radio.
    * @since Series 60 3.0
    * @return none
    */
    IMPORT_C void RadioOff();

    /**
     * Returns the radio status
     * @since ??
     * @return ETrue = radio is on, EFalse = radio is off
     */
    IMPORT_C TBool IsRadioOn();
    
    /**
     * Checks if offine profile is current active profile
     * @since ??
     * @return True / False
     */
    IMPORT_C TBool IsOfflineProfileL();
    
    /**
    * Tune to the specified frequency
    * @since Series 60 3.0
    * @param aFrequency - frequency to lock onto.
    * @param aRadioMode - The radio mode to be used. Tuner mode by default
    * @return none
    */
    IMPORT_C void Tune( TInt aFrequency, TRadioMode aRadioMode = ERadioTunerMode );
    
    /*
     * Sets tuner mode on i.e. preset channel is not active.
     * Active preset Channel P&S data is also cleared.
     * Useful for example when preset channel is deleted.
     */
    IMPORT_C void SetTunerModeOn();
    
    /**
    * Scan up to the next available frequency.
    * @since Series 60 3.0
    * @return none
    */
    IMPORT_C void ScanUp();

    /**
    * Scan down to the last available frequency.
    * @since Series 60 3.0
    * @return none
    */
    IMPORT_C void ScanDown();

    /**
    * Cancel previously requested scan, and
    * return to the already tuned frequency.
    * @since Series 60 3.0
    * @return none
    */
    IMPORT_C void CancelScan();

    /**
    * Sets the audio mute state
    * @since Series 60 3.0
    * @param aMuteOn - flag to determine whether mute should be turned on or off
    * @return none
    */
    IMPORT_C void SetMuteOn(TBool aMuteOn);

    /**
    * Gets the audio mute state
    * @since Series 60 3.0
    * @return ETrue or EFalse to indicate whether mute is currently on.
    */
    IMPORT_C TBool IsMuteOn() const;

    /**
    * Sets the volume level of the FM radio
    * @since Series 60 3.0
    * @param aVolume - the volume to be used.
    * @return none
    */
    IMPORT_C void SetVolume(TInt aVolume);

    /**
    * Gets the volumelevel.
    * @since Series 60 3.0
    * @return the current volume
    */
    IMPORT_C TInt GetVolume() const;

    /**
    * Gets the max volumelevel.
    * @since Series 60 3.0
    * @return the max volume
    */
    IMPORT_C TInt GetMaxVolume() const;

    /**
    * Sets the audio Output
    * @since Series 60 3.0
    * @param aAudioOutput - where should the audio be routed?
    * @return none
    */
    IMPORT_C void SetAudioOutput( const TFMRadioAudioOutput aAudioOutput );

    /**
    * Sets the rds af search enabled
    * @param aEnabled ETrue if rds af search is enabled; EFalse otherwise.
    * @return none
    */
    IMPORT_C void SetRdsAfSearchEnable( TBool aEnabled );
     
     /**
     * Check whether flight mode is currently enabled
     * @since Series 60 3.0
     * @return ETrue or EFalse dependent on whether flightmode is enabled
     */
     IMPORT_C TBool IsRdsAfSearchEnabled() const;
     
     /**
      * @return reference to settings
      */
     IMPORT_C TRadioSettings& RadioSettings( );
     
    /**
    * Retrieve the current audio output
    * @since Series 60 3.0
    * @return EFMRadioOutputIHF or EFMRadioOutputHeadset
    */
    IMPORT_C TFMRadioAudioOutput GetAudioOutput() const;

    /**
    * Tunes to the chosen channel.
    * @since Series 60 3.0
    * @param aIndex - the index number of the channel you wish to tune to.
    * @return none
    */
    IMPORT_C void TunePresetL(TInt aIndex);

    /**
    * Retrieves the current frequency.
    * @since Series 60 3.0
    * @return the frequency
    */
    IMPORT_C TInt GetTunedFrequency() const;

    /**
    * Check whether flight mode is currently enabled
    * @since Series 60 3.0
    * @return ETrue or EFalse dependent on whether flightmode is enabled
    */
    IMPORT_C TBool IsFlightModeEnabled() const;

    /**
    * Get the frequency range (in Hertz) of the specified band.
    * This function should be used to enquire the frequency range
    * of the bands that GetCapabilities reports as supported.
    * @since Series 60 3.0
    * @param aBottomFrequency The variable to set to the lowest frequency allowed
    * @param aTopFrequency The variable to set to the highest frequency allowed
    * @return A standard system error code
    **/
    IMPORT_C TInt GetFrequencyBandRange( TInt& aBottomFrequency, TInt& aTopFrequency );

    /**
    * Check whether call is in progress
    * @since Series 60 3.1
    * @return ETrue or EFalse to indicate whether call is in progress
    */
    IMPORT_C TBool IsInCall() const;
    
    /**
    * Has the frequency been set by RDS AF search or not.
    *
    * @return ETrue if frequency was set by RDS AF, otherwise EFalse
    */
    IMPORT_C TBool FrequencySetByRdsAf() const;

    /**
    * Getter for RDS receiver
    *
    * @return Reference to CFMRadioRdsReceiver
    */
    IMPORT_C CFMRadioRdsReceiverBase& RdsReceiver();
    /**
    * Notifies UI that the headset button has been pressed
    */
    void HandleHeadsetButtonPress();
    /**
    * Switches the engine into flight mode, and notifies the UI of
    * the change in status.
    */
    void HandleFlightModeEnabled();
    /**
    * Switches the engine out of flight mode, and notifies the UI of
    * the change in status
    */
    void HandleFlightModeDisabled();
    /**
    * Calls the User Interface callback function, indicating the event code and
    * error code.
    */
    void HandleCallback(TInt aEventCode, TInt aErrorCode );

    /**
    * Gets the headset status
    * @return ETrue if headset connected
    */
    IMPORT_C TBool IsHeadsetConnected() const;

    /**
     * From MRadioPlayerObserver.
     * Called when Radio state changed.
     *
     * @since S60 3.2
     * @param aState Radio player state
     * @param aError A standard system error code, only used when aState is ERadioPlayerIdle
     */
    void MrpoStateChange( TPlayerState aState, TInt aError );

    /**
     * From MRadioPlayerObserver.
     * Called when volume changes. This may be caused by other applications.
     *
     * @since S60 3.2
     * @param aVolume Current volume.
     */
    void MrpoVolumeChange( TInt aVolume );

    /**
     * From MRadioPlayerObserver.
     * Called when mute setting changes. This may be caused by other applications.
     *
     * @since S60 3.2
     * @param aMute ETrue indicates audio is muted.
     */
    void MrpoMuteChange( TBool aMute );

    /**
     * From MRadioPlayerObserver.
     * Called when mute setting changes. This may be caused by other applications.
     *
     * Called when balance setting changes. This may be caused by other applications.
     *
     * @since S60 3.2
     * @param aLeftPercentage
     *        Left speaker volume percentage. This can be any value from zero to 100.
     *        Zero value means left speaker is muted.
     * @param aRightPercentage
     *        Right speaker volume percentage. This can be any value from zero to 100.
     *        Zero value means right speaker is muted.
     */
    void MrpoBalanceChange( TInt aLeftPercentage, TInt aRightPercentage );


    /**
     * From MRadioFmTunerObserver.
     * Called when Request for tuner control completes.
     *
     * @since S60 3.2
     * @param aError A standard system error code or FM tuner error (TFmRadioTunerError).
     */
    void MrftoRequestTunerControlComplete( TInt aError );

    /**
     * From MRadioFmTunerObserver.
     * Set frequency range complete event. This event is asynchronous and is received after
     * a call to CRadioFmTunerUtility::SetFrequencyRange.
     *
     * @since S60 3.2
     * @param aError A standard system error code or FM tuner error (TFmRadioTunerError).
     */
    void MrftoSetFrequencyRangeComplete( TInt aError );

    /**
     * From MRadioFmTunerObserver.
     * Set frequency complete event. This event is asynchronous and is received after a call to
     * CRadioFmTunerUtility::SetFrequency.
     *
     * @since S60 3.2
     * @param aError A standard system error code or FM tuner error (TFmRadioTunerError).
     */
    void MrftoSetFrequencyComplete( TInt aError );

    /**
     * From MRadioFmTunerObserver.
     * Station seek complete event. This event is asynchronous and is received after a call to
     * CRadioFmTunerUtility::StationSeek.
     *
     * @since S60 3.2
     * @param aError A standard system error code or FM tuner error (TFmRadioTunerError).
     * @param aFrequency The frequency(Hz) of the radio station that was found.
     */
    void MrftoStationSeekComplete( TInt aError, TInt aFrequency );

    /**
     * From MRadioFmTunerObserver.
     * Called when FM Transmitter status changes (if one is present in the device). Tuner receiver
     * is forced to be turned off due to hardware conflicts when FM transmitter is activated.
     *
     * @since S60 3.2
     * @param aActive ETrue if FM transmitter is active; EFalse otherwise.
     */
    void MrftoFmTransmitterStatusChange( TBool aActive );

    /**
     * From MRadioFmTunerObserver.
     * Called when antenna status changes.
     *
     * @since S60 3.2
     * @param aAttached ETrue if antenna is attached; EFalse otherwise.
     */
    void MrftoAntennaStatusChange( TBool aAttached );

    /**
     * From MRadioFmTunerObserver.
     * Called when offline mode status changes.
     * @since S60 3.2
     *
     * @param aOfflineMode ETrue if offline mode is enabled; EFalse otherwise.
     */
    void MrftoOfflineModeStatusChange( TBool aOfflineMode );

    /**
     * From MRadioFmTunerObserver.
     * Called when the frequency range changes. This may be caused by other applications.
     *
     * @since S60 3.2
     * @param aBand New frequency range.
     */
    void MrftoFrequencyRangeChange(TFmRadioFrequencyRange aBand );

    /**
     * From MRadioFmTunerObserver.
     * Called when the tuned frequency changes. This may be caused by other
     * applications or RDS if AF/TA is enabled.
     *
     * @since S60 3.2
     * @param aNewFrequency The new tuned frequency(Hz).
     */
    void MrftoFrequencyChange( TInt aNewFrequency );

    /**
     * From MRadioFmTunerObserver.
     * Called when the forced mono status change. This may be caused by other applications.
     *
     * @since S60 3.2
     * @param aForcedMono ETrue if forced mono mode is enabled; EFalse otherwise.
     */
    void MrftoForcedMonoChange( TBool aForcedMono );

    /**
     * From MRadioFmTunerObserver.
     * Called when the squelch (muting the frequencies without broadcast) status change.
     * This may be caused by other applications.
     *
     * @since S60 3.2
     * @param aSquelch ETrue if squelch is enabled; EFalse otherwise.
     */
    void MrftoSquelchChange( TBool aSquelch );

    /**
     * From MRadioPresetObserver.
     * Called when a preset changes.
     *
     * NOTE: EPresetDeleted with aIndex == 0, indicates that all presets have been deleted.
     *
     * @since S60 3.2
     * @param aChange Change event type
     * @param aIndex Index to the preset that has changed. Zero means all presets.
     */
    void MrpeoPresetChanged( TPresetChangeEvent aChange, TInt aIndex );
     /**
 	* From base class MFMRadioSystemEventDetectorObserver
	* This callback notifies when networks comes up.
	*/
	void NetworkUpCallbackL();
	/**
	* From base class MFMRadioSystemEventDetectorObserver
	* This callback notifies when network goes down.
	*/
	void NetworkDownCallbackL();
	
	/**
	* From base class MFMRadioSystemEventDetectorObserver
	* This callback notifies when audio resources become available.
	*/
	void AudioResourcesAvailableL();
	
	/**
    * From base class MFMRadioSystemEventDetectorObserver
	* This callback notifies when audio auto resuming is forbidden.
	*/
	void AudioAutoResumeForbiddenL();
	/**
	* From base class MFMRadioSystemEventDetectorObserver
	* This callback notifies when call becomes active.
	*/
	void CallActivatedCallbackL();

	/**
	* From base class MFMRadioSystemEventDetectorObserver
	* This callback notifies when call becomes deactive.
	*/
	void CallDeactivatedCallbackL();	
	/**
    * From base class MFMRadioNetworkChangeObserver
    * 
    * @see MFMRadioNetworkChangeObserver::NetworkIdChanged();
    */
    void NetworkIdChanged();

    // from base class MFMRAdioRdsObserver
    void RdsDataProgrammeService( const TDesC& aProgramService );
    void RdsDataPsNameIsStatic( TBool aStatic );
    void RdsDataRadioText( const TDesC& aRadioText );
    void RdsDataRadioTextPlus( const TInt aRTPlusClass, const TDesC& aRadioTextPlus );
    
    void RdsAfSearchBegin();
    void RdsAfSearchEnd( TUint32 aFrequency, TInt aError );
    void RdsAfSearchStateChange( TBool aEnabled );
    void RdsAvailable( TBool aAvailable );
    	
	// from MFMRadioHeadsetEventObserver
	void HeadsetAccessoryConnectedCallbackL();
	void HeadsetAccessoryDisconnectedCallbackL();
		
	/**
    * Initialize fmradioengine resource file information
    */
	void InitializeResourceLoadingL();	
	/**
    * Convert FMRadio region info to frequency range used by fmradiotunerutility
    * @param aRegionId fmradio region
    * @return frequency range used by fmradiotunerutility
    */
	TFmRadioFrequencyRange TunerFrequencyRangeForRegionId( const TInt aRegionId ) const;
	/**
    * Determine current region
    * @return fmradio region
    */
	IMPORT_C TFMRadioRegionSetting GetRegionL() const;
	/**
    * fill the given list with fmradio regions
	* @param aArray array to add regions
    */
	IMPORT_C void FillListWithRegionDataL( CDesC16Array& aArray ) const;
	/**
    * Returns region from the array of supported regions
    * @param aIndex index for region
    * @return fmradio region
    */
	IMPORT_C TFMRadioRegionSetting RegionIdAtIndex( const TInt aIndex ) const;
	/**
    * Returns current region Id
    * @return current region Id.
    */
	IMPORT_C TInt RegionId() const;
	/**
    * Change current region
    * @param aRegion region to set
    */
	IMPORT_C void SetRegionIdL( TInt aRegion ) const;
	/**
    * Request tuner control from fmradiotunerutility
    */
	IMPORT_C void RequestTunerControl() const;
	/**
    * Returns decimal count for current region MHz information
    * @return decimal count for current region
    */
	IMPORT_C TInt DecimalCount() const;
	/**
    * Return step size for tuning.
    * @return step size
    */
	IMPORT_C TUint32 FrequencyStepSize() const;
	/**
    * Returns a reference to P&S interface.
    * Method leaves if iPubSub is not allocated
    *
    * @return PS& interface.
    */	
	IMPORT_C CFMRadioPubSub& PubSubL() const;
	/**
	* increases startup count by one and returns amount of app startups.
	* 
	* @return app startup count
	*/
	IMPORT_C TInt UpdatedStartupCount() const;
	/**
    * Cancel seek and notify UI
    */
    IMPORT_C void CancelScanLocalStationsScan();
	/**
     * Is routing between loudspeaker and headset possible
     *
     * @return ETrue, if routing is possible
     */
    IMPORT_C TBool IsAudioRoutingPossible() const;
	
    /**
     * Has RT+ interaction tutorial dialog been shown to user.
     * 
     * @return ETrue after the first time user sees the dialog.
     */
    IMPORT_C TBool MusicStoreNoteDisplayed();
    
    /**
     * Sets music store tutorial note displayed value to true.
     * It needs to be false only before supported RT+ features
     * become available for the first time.
     */
    IMPORT_C void SetMusicStoreNoteDisplayed();
    
    /**
     * Get level of support for RT+ interactions.
     * Values are defined in fmradioengine.hrh:TFMRadioRTPlusLevel.
     * 
     * @return which interactions are supported as a TFMRadioRTPlusLevel value
     */
    IMPORT_C TFMRadioRTPlusLevel GetRTPlusSupportLevel();
        
    /**
     * Sets the currently active preset index
     * @param aIndex new active preset index
     */    
    IMPORT_C void SetCurrentPresetIndex( const TInt aIndex );
    
    /* Gets saved preset url.
     * @param aIndex channel index
     * @param aUrl string to be modified with saved url
     * @return error code. KErrNotFound if no url is saved or KErrOverflow
     * if the aUrl is not big enough. See TFmPresetName
     */
    IMPORT_C TInt PresetUrlL( TInt aIndex, TDes& aUrl );
    
    /*
     * Delete preset. With index -1 all preset are reseted
     * @param aIndex index to delete
     */
    IMPORT_C void DeletePresetL( TInt aIndex ); 
    
    /*
     * Saves given url to preset
     * @param aIndex channel index
     * @param aUrl url to save 
     */
    IMPORT_C void SaveUrlToPresetL( TInt aIndex, const TDesC& aUrl );
    
private:

    /**
    * Default class constructor.
    */
    CRadioEngine( MRadioEngineStateChangeCallback& aCallback );

    /**
    * Second phase class constructor.
    */
    void ConstructL();
    
    /**
    * ConnectLineL
    */
    void ConnectLineL();

    /**
     * Updates the frequency into settings and P&S
     * 
     * @param aFrequency The frequency to be set
     */
    void StoreAndPublishFrequency( TInt aFrequency );
    
private:

   //the internal radio engine states
   enum TRadioEngineState
       {
        EStateRadioOff,
        EStateRadioOn,
        };

    // The Radio Utility
    CRadioUtility* iRadioUtility; // own
    // The Tuner Utility
    CRadioFmTunerUtility* iFmTunerUtility;
    // The Player Utility
    CRadioPlayerUtility* iPlayerUtility;
    // The Preset Utility
    CRadioFmPresetUtility* iPresetUtility; // own
    // Receives and notifies of the RDS data
    CFMRadioRdsReceiverBase* iRdsReceiver;  
    // Capabilities of the tuner
    TFmTunerCapabilities iTunerCaps;
    // Multiplier used to scale UI volume setting to volume level used by the player utility 
    TReal iUiVolumeFactor;
     // pointer to DosServerObserver
#ifndef __ACCESSORY_FW
    CDosServerObserver *iDosServerObserver; //own
#endif
    // Audio routing API
    CAudioOutput* iAudioOutput; //own
    // pointer to CCentralRepositoryHandler
    CCentralRepositoryHandler* iCentralRepositoryHandler; // own
    // Used to provide async behavior in some callbacks
    CRadioStateHandler *iStateHandler;
    // flag to indicate if auto resume is supported
    TBool iAutoResume;
    // flag to indicate if a call is in progress
    TBool iInCall;
    // pointer to TRadioSettings
    TRadioSettings* iRadioSettings; // own
    // the current radio state
    TRadioEngineState iCurrentRadioState;
    // a reference of MRadioEngineStateChangeCallback object
    MRadioEngineStateChangeCallback& iCallback;
    // temperary frequency
    TInt iTempFrequency;
    // a flag indicating if tune prset is requestd
    TBool iTunePresetRequested;
    // Indicates a request to initialized the radio is pending
    TBool iInitializeRadioRequestExists;
    // Indicates if the tuner control has been granted.
    TBool iTunerControl;
    // High and low frequency of the current band.
    TInt iBottomFrequency;
    TInt iTopFrequency;
    // ETrue if frequency was set by RDS AF
    TBool iFrequencySetByRdsAf;
    //Listens changes in network availability
    CFMRadioSystemEventDetector* iSystemEventDetector;
   	// for fmradio engine resource file
    TInt iFMRadioEngineResourceOffset;
    //Listens changes in network ID and country code 
    CFMRadioMobileNetworkInfoListener* iNetworkInfoListener;
    //P&S interaction interface for FMRadio actions.
	CFMRadioPubSub* iPubSub;
    // accessory observer
    CFMRadioAccessoryConnection* iHeadsetObserver;
    // tel server session
    RTelServer iTelServer;
    // phone
    RMobilePhone iPhone;
	// line
	RMobileLine iLine;
	// tsy name
	TFileName iTsyName;
    // conrrectly constructer
    TBool ilineConstructed;
    // for storing station name for method GetPresetNameL
   	TStationName iStationName;
    // Local cache for tuning state so that it doesn't get published before should
   	TFMRadioPSTuningState iTuningState;
    // If last time audio is set to IHF with no headset connected.
    TBool iHFOptionActivated;
    };

#endif  //FMRADIOENGINE_H

// End of file
