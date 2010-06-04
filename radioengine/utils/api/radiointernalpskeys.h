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

#ifndef RADIOINTERNALPSKEYS_H
#define RADIOINTERNALPSKEYS_H

#include <e32cmn.h>

#include "radiouid.hrh"

/**< The UID of the category we use. Must be the same as Visual Radio's UID3 ( SID ). */
//TODO: Change
const TUid KRadioPSUid = {0x101FF976};

///////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                 ///
/// Data properties. These properties are published by Visual Radio and used by other applications. ///
///                                                                                                 ///
///////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Currently active channel's ID.
* Value <code>KErrNone</code> means that no preset channel is active.
* Type <code>RProperty::EInt</code>.
*/
const TUint32 KRadioPSDataChannel = 0x00000000;

/**
* Currently active frequency.
* Frequency unit is in kHz.
* Type <code>RProperty::EInt</code>.
*/
const TUint32 KRadioPSDataFrequency = 0x00000001;

/**
* Current power state of the radio.
* Type <code>RProperty::EInt</code>.
* @see TRadioRadioPowerState.
*/
const TUint32 KRadioPSDataRadioPowerState = 0x00000002;

/**
* Power state of the radio.
*/
enum TRadioPSRadioPowerState
    {
    ERadioPSRadioPowerStateUninitialized,  /**< The key is not yet initialized. */
    ERadioPSRadioPowerOn,                    /**< Radio power on. */
    ERadioPSRadioPowerOff                    /**< Radio power off. */
    };

/**
* Current volume level.
* Scale is from 1-10.
* Type <code>RProperty::EInt</code>.
*/
const TUint32 KRadioPSDataVolume = 0x00000003;

/**
* Changing of this value means that channel
* data of specified channel ID has changed.
* The value remains indicating the last changed channel ID.
* Value <code>KErrNone</code> means that no channel data has been changed yet.
* Type <code>RProperty::EInt</code>.
*/
const TUint32 KRadioPSDataChannelDataChanged = 0x00000004;

/**
 * Removed. Do not use.
 */
const TUint32 KRadioPSDataRemoved = 0x00000005;

/**
* The current tuning state.
* Type <code>RProperty::EInt</code>.
* @see TRadioPSTuningState.
*/
const TUint32 KRadioPSDataTuningState = 0x00000006;

/**
* Possible tuning states.
*/
enum TRadioPSTuningState
    {
    ERadioPSTuningUninitialized,    /**< Currently not tuning. */
    ERadioPSTuningStarted,             /**< Tuning has been started an lasted less than one second. */
    ERadioPSTuningContinues         /**< Tuning has taken more than one second. */
    };

/**
* Current mute state of radio.
* Type <code>RProperty::EInt</code>.
* @see TRadioPSRadioMuteState.
*/
const TUint32 KRadioPSDataRadioMuteState = 0x00000007;

/**
* Mute state of radio.
*/
enum TRadioPSRadioMuteState
    {
    ERadioPSMuteStateUninitialized,    /**< The key has not yet been initialized. */
    ERadioPSMuteStateOn,               /**< Radio mute state is on. */
    ERadioPSMuteStateOff               /**< Radio mute state is off. */
    };

/**
* Indicates if Visual Radio is running or closing.
* Type <code>RProperty::EInt</code>.
* @see TRadioPSApplicationRunningState.
*/
const TUint32 KRadioPSDataApplicationRunning = 0x00000008;

/**
* Visual Radio application running states.
*/
enum TRadioPSApplicationRunningState
    {
    ERadioPSApplicationUninitialized,  /**< Visual Radio application's state is not yet initialized. */
    ERadioPSApplicationRunning,        /**< Visual Radio application is running. */
    ERadioPSApplicationClosing         /**< Visual Radio application is closing. */
    };

/**
* Indicates the current headset connectivity status.
* Type <code>RProperty::EInt</code>.
* @see TRadioHeadsetStatus
*/
const TUint32 KRadioPSDataHeadsetStatus = 0x00000009;

/**
* Current headset status.
*/
enum TRadioPSHeadsetStatus
    {
    ERadioPSHeadsetUninitialized,  /**< The headset status is uninitialized. */
    ERadioPSHeadsetDisconnected,   /**< The headset is disconnected. */
    ERadioPSHeadsetConnected       /**< The headset is connected. */
    };

/**
* The current frequency decimal count.
* Type <code>RProperty::EInt</code>.
* @see TRadioFrequencyDecimalCount.
*/
const TUint32 KRadioPSDataFrequencyDecimalCount = 0x00000010;

/**
* The current frequency decimal count.
*/
enum TRadioPSFrequencyDecimalCount
    {
    ERadioPSFrequencyDecimalCountUninitialized,    /**< Frequency decimal count has not yet been initialized. */
    ERadioPSFrequencyOneDecimal,                     /**< Frequency accuracy is one decimal. */
    ERadioPSFrequencyTwoDecimals,                    /**< Frequency accuracy is two decimals. */
    ERadioPSFrequencyThreeDecimals                   /**< Frequency accuracy is three decimals. */
    };

/**
* Removed. Do not use.
*/
const TUint32 KRadioPSRemoved1 = 0x00000014;

/**
* Flag to indicate if loudspeaker is in use
*/
const TUint32 KRadioPSDataLoudspeakerStatus = 0x00000015;
/**
* Current speaker status.
*/
enum TRadioPSLoudspeakerStatus
    {
    ERadioPSLoudspeakerUninitialized,  /**< The speaker status is uninitialized. */
    ERadioPSLoudspeakerNotInUse,   /**< The speaker is not in use. */
    ERadioPSLoudpeakerInUse       /**< The speaker is in use. */
    };

/**
* Rds Program Service information, property type is text.
*/
const TUint32 KRadioPSDataRdsProgramService = 0x00000016;

/**
* Rds Radio Text information.
* Type <code>RProperty::EText</code>.
*/
const TUint32 KRadioPSDataRdsRadioText = 0x00000017;

/**
 * Name of the current channel.
 * Type <code>RProperty::EText</code>.
 */
const TUint32 KRadioPSDataChannelName = 0x00000018;

/**
* The amount of presets in active preset list.
* In situations where the amount is undefined, value KErrNotFound
* is used.
* Type <code>RProperty::EInt</code>.
*/
const TUint32 KRadioPSDataActivePresetListCount = 0x00000019;

/**
* The active focus in active preset list.
* In situations where the amount is undefined, value KErrNotFound
* is used.
* Type <code>RProperty::EInt</code>.
*/
const TUint32 KRadioPSDataActivePresetListFocus = 0x00000020;

/**
 * Current antenna state of the radio.
 * Type <code>RProperty::EInt</code>.
 * @see TRadioRadioAntennaState.
 */
const TUint32 KRadioPSDataRadioAntennaState = 0x00000021;

/**
* Power state of the radio.
*/
enum TRadioPSRadioAntennaState
    {
    ERadioPSRadioAntennaStateUninitialized,  /**< The key is not yet initialized. */
    ERadioPSRadioAntennaAttached,            /**< Antenna attached. */
    ERadioPSRadioAntennaDetached             /**< Antenna detached. */
    };

//////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                    ///
/// Control properties. These properties are published by other applications and used by Visual Radio. ///
///                                                                                                    ///
//////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Steps up or down a channel in channel list.
* Type <code>RProperty::EInt</code>.
* @see TRadioPSStepToChannel.
*/
const TUint32 KRadioPSControlStepToChannel = 0x00001000;

/**
* Step direction of channel step.
*/
enum TRadioPSStepToChannel
    {
    ERadioPSStepToChannelUninitialized,    /**< Value of the key has not yet been initialized. */
    ERadioPSStepToChannelUp,                /**< Step to next channel up. */
    ERadioPSStepToChannelDown                /**< Step to next channel down. */
    };

/**
* Switches to the specified channel ID.
* Visual Radio ignores channel IDs that are out of scope.
* Type <code>RProperty::EInt</code>.
*/
const TUint32 KRadioPSControlSetChannel = 0x00001001;

/**
* Seeks to next active frequency up or down.
* Type <code>RProperty::EInt</code>.
* @see TRadioPSSeek.
*/
const TUint32 KRadioPSControlSeek = 0x00001002;

/**
* Seek direction.
*/
enum TRadioPSSeek
    {
    ERadioPSSeekUninitialized, /**< Value of the key has not yet been initialized. */
    ERadioPSSeekUp,            /**< Seek up. */
    ERadioPSSeekDown            /**< Seek down. */
    };

/**
* Steps up or down frequency, using the frequency step that is defined in radio.
* Type <code>RProperty::EInt</code>.
* @see TRadioPSStepToFrequency.
*/
const TUint32 KRadioPSControlStepToFrequency = 0x00001003;

/**
* Frequency step direction.
*/
enum TRadioPSStepToFrequency
    {
    ERadioPSStepToFrequencyUninitialized,  /**< The key has not yet been initialized. */
    ERadioPSStepToFrequencyUp,                /**< Step up a frequency. */
    ERadioPSStepToFrequencyDown            /**< Step down a frequency. */
    };

/**
* Switches to specified frequency.
* Frequency unit is kHz.
* Type <code>RProperty::EInt</code>.
*/
const TUint32 KRadioPSControlSetFrequency = 0x00001004;

/**
* Increases or decreases volume level.
* Type <code>RProperty::EInt</code>.
* @see TRadioPSAdjustVolume.
*/
const TUint32 KRadioPSControlAdjustVolume = 0x00001005;

/**
* Volume step direction.
*/
enum TRadioPSAdjustVolume
    {
    ERadioPSAdjustVolumeUninitialized = 0, /**< The key has not yet been initialized. */
    ERadioPSIncreaseVolume,                /**< Increase volume. */
    ERadioPSDecreaseVolume                    /**< Decrease volume. */
    };

/**
* Sets mute state on or off.
* Type <code>RProperty::EInt</code>.
* @see TRadioPSRadioMuteState.
*/
const TUint32 KRadioPSControlSetRadioMuteState = 0x00001006;

/**
* Sets the output source
* Type <code>RProperty::EInt</code>.
* @see TRadioPSAudioOutput
*/
const TUint32 KRadioPSControlSetAudioOutput = 0x00001008;

/**
 * Audio output target.
 */
enum TRadioPSAudioOutput
    {
    ERadioPSAudioOutputUninitialized = 0,   /**< The key has not yet been initialized. */
    ERadioPSAudioOutputHeadset,             /**< Headset in use. */
    ERadioPSAudioOutputSpeaker              /**< Speaker in use. */
    };

#endif // RADIOINTERNALPSKEYS_H
