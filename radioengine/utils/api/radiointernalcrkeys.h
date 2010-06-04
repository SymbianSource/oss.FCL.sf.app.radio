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

#ifndef RADIOINTERNALCRKEYS_H
#define RADIOINTERNALCRKEYS_H

#include <e32cmn.h>

/** The UID of the category we use. Must be the same as Visual Radio's UID3 ( SID ). */
const TUid KRadioCRUid = { 0x101FF976 };

/**
* The service mode of Visual Radio.
*
* @see TVRCRServiceMode
*/
const TUint32 KRadioCRServiceMode = 0x00000000;

/**
* Possible Visual Radio service modes.
*/
enum TVRCRServiceMode
    {
    EVRCRServiceNone,   /**< Visual Radio has neither SDS nor visual service enabled. */
    EVRCRServiceSds,    /**< Visual Radio has SDS service enabled. */
    };

/////////////////////////////////////////////////////////////////////////////////////
// When adding a new key, add it also to CVRRepositoryManager::CheckRepositoryKeysL()
// to make sure that SIS installation updates cenrep keys.
/////////////////////////////////////////////////////////////////////////////////////

// APPLICATION SETTINGS

/** The base value for application setting keys. */
const TUint32 KRadioCRApplicationSettingsBase                  = { 0x00000010 };

/** The key associated with the active focus location setting. */
const TUint32 KRadioCRActiveFocusLocation                      = { KRadioCRApplicationSettingsBase + 0x00000000 };

/** Removed. Do not use. */
const TUint32 KRadioCRRemoved1                                 = { KRadioCRApplicationSettingsBase + 0x00000001 };

/** Removed. Do not use. */
const TUint32 KRadioCRRemoved2                                 = { KRadioCRApplicationSettingsBase + 0x00000002 };

/** The key associated with the http cache limit setting. */
const TUint32 KRadioCRUiFlags                                  = { KRadioCRApplicationSettingsBase + 0x00000003 };

/** Removed. Do not use. */
const TUint32 KRadioCRRemoved3                                 = { KRadioCRApplicationSettingsBase + 0x00000004 };

/** Removed. Do not use. */
const TUint32 KRadioCRRemoved4                                 = { KRadioCRApplicationSettingsBase + 0x00000005 };

/** Removed. Do not use. */
const TUint32 KRadioCRRemoved5                                 = { KRadioCRApplicationSettingsBase + 0x00000006 };

/** Removed. Do not use. */
const TUint32 KRadioCRRemoved6                                 = { KRadioCRApplicationSettingsBase + 0x00000007 };

/** Removed. Do not use. */
const TUint32 KRadioCRRemoved7                                 = { KRadioCRApplicationSettingsBase + 0x0000000B };

/** Removed. Do not use. */
const TUint32 KRadioCRRemoved8                                 = { KRadioCRApplicationSettingsBase + 0x0000000C };

/** Removed. Do not use. */
const TUint32 KRadioCRRemoved9                                 = { KRadioCRApplicationSettingsBase + 0x0000000D };

/** Removed. Do not use. */
const TUint32 KRadioCRRemoved10                                = { KRadioCRApplicationSettingsBase + 0x0000000E };

/** Removed. Do not use. */
const TUint32 KRadioCRRemoved11                                = { KRadioCRApplicationSettingsBase + 0x0000000F };

/* The key associated with the application launch count setting */
const TUint32 KRadioCRLaunchCount                              = { KRadioCRApplicationSettingsBase + 0x00000010 };

/* The key associated with the application offline mode usability. */
const TUint32 KRadioCROfflineModeFunctionality                 = { KRadioCRApplicationSettingsBase + 0x00000011 };

/** Removed. Do not use. */
const TUint32 KRadioCRRemoved12                                = { KRadioCRApplicationSettingsBase + 0x00000012 };

/* User audio history */
const TUint32 KRadioCRAudioPlayHistory                         = { KRadioCRApplicationSettingsBase + 0x00000014 };


/**
* Removed. Do not use.
*/
enum TVRCRRemoved
    {
    EVRCRRemoved13,
    EVRCRRemoved14
    };


// RADIO SETTINGS

/** The base value for radio setting keys. */
const TUint32 KRadioCRRadioSettingsBase                        = { 0x00000100 };

/** The key associated with the headset volume setting. */
const TUint32 KRadioCRHeadsetVolume                            = { KRadioCRRadioSettingsBase + 0x00000000 };

/** The key associated with the speaker volume setting. */
const TUint32 KRadioCRSpeakerVolume                            = { KRadioCRRadioSettingsBase + 0x00000001 };

/** The key associated with the output mode setting. */
const TUint32 KRadioCROutputMode                               = { KRadioCRRadioSettingsBase + 0x00000002 };

/** The key associated with the audio route setting. */
const TUint32 KRadioCRAudioRoute                               = { KRadioCRRadioSettingsBase + 0x00000003 };

/** The key associated with the headset volume mute setting. */
const TUint32 KRadioCRHeadsetMuteState                         = { KRadioCRRadioSettingsBase + 0x00000004 };

/** The key associated with the speaker volume mute setting. */
const TUint32 KRadioCRSpeakerMuteState                         = { KRadioCRRadioSettingsBase + 0x00000005 };

/** The key associated with the radio power state. */
const TUint32 KRadioCRVisualRadioPowerState                    = { KRadioCRRadioSettingsBase + 0x00000006 };

/** The key associated with the tuned frequency setting.  */
const TUint32 KRadioCRTunedFrequency                           = { KRadioCRRadioSettingsBase + 0x00000007 };

/** The key associated with the default minimum volume level setting.  */
const TUint32 KRadioCRDefaultMinVolumeLevel                    = { KRadioCRRadioSettingsBase + 0x00000008 };

/** The key associated with the current region setting. */
const TUint32 KRadioCRCurrentRegion                            = { KRadioCRRadioSettingsBase + 0x0000000A };

/** Removed. Do not use. */
const TUint32 KRadioCRRemoved15                                = { KRadioCRRadioSettingsBase + 0x0000000B };

/** The key associated with the RDS support setting */
const TUint32 KRadioCRRdsSupport                               = { KRadioCRRadioSettingsBase + 0x0000000C };

/** The key associated with the alternate frequency search setting */
const TUint32 KRadioCRRdsAfSearch                              = { KRadioCRRadioSettingsBase + 0x0000000D };

/*** Region Japan allowed. */
const TUint32 KRadioCRRegionAllowedJapan                       = { KRadioCRRadioSettingsBase + 0x0000000E };

/*** Region America allowed. */
const TUint32 KRadioCRRegionAllowedAmerica                     = { KRadioCRRadioSettingsBase + 0x0000000F };

/*** Region Other allowed. */
const TUint32 KRadioCRRegionAllowedDefault                     = { KRadioCRRadioSettingsBase + 0x00000010 };

/*** Default region. */
const TUint32 KRadioCRDefaultRegion                            = { KRadioCRRadioSettingsBase + 0x00000011 };


// PRESET SETTINGS

/** The base value for preset setting keys. */
const TUint32 KRadioCRPresetSettingsBase                       = { 0x00001000 };

/** The key associated with the maximum preset count setting. */
const TUint32 KRadioCRPresetCount                              = { KRadioCRPresetSettingsBase + 0x00000000 };

// CORE SETTINGS

/** The base value for core setting keys. */
const TUint32 KRadioCRCoreSettingsBase                         = { 0x00010000 };

/** The key associated with the global name server address setting. */
const TUint32 KRadioCRGlobalNameServer                         = { KRadioCRCoreSettingsBase + 0x00000000 };

/** The key associated with the default global name server address setting. */
const TUint32 KRadioCRDefaultGlobalNameServer                  = { KRadioCRCoreSettingsBase + 0x00000001 };

/** The key associated with the local name server address setting. */
const TUint32 KRadioCRLocalNameServer                          = { KRadioCRCoreSettingsBase + 0x00000002 };

/** The key associated with the default local name server address setting. */
const TUint32 KRadioCRDefaultLocalNameServer                   = { KRadioCRCoreSettingsBase + 0x00000003 };

/** The key associated with the directory server url setting. */
const TUint32 KRadioCRDirectoryServerUrl                       = { KRadioCRCoreSettingsBase + 0x00000004 };

/** The key associated with the directory server base url setting. */
const TUint32 KRadioCRDirectoryServerBaseUrl                   = { KRadioCRCoreSettingsBase + 0x00000005 };

/** The key associated with the directory server country code setting. */
const TUint32 KRadioCRDirectoryServerCountryCode               = { KRadioCRCoreSettingsBase + 0x00000006 };

/** The key associated with the directory server network id setting. */
const TUint32 KRadioCRDirectoryServerNetworkId                 = { KRadioCRCoreSettingsBase + 0x00000007 };

/** The key associated with the default directory server url setting. */
const TUint32 KRadioCRDefaultDirectoryServerUrl                = { KRadioCRCoreSettingsBase + 0x00000008 };

/** The key associated with the default directory server base url setting. */
const TUint32 KRadioCRDefaultDirectoryServerBaseUrl            = { KRadioCRCoreSettingsBase + 0x00000009 };

/** The key associated with the internet access point setting. */
const TUint32 KRadioCRInternetAccessPoint                      = { KRadioCRCoreSettingsBase + 0x0000000A };

/** The key associated with the default internet access point setting. */
const TUint32 KRadioCRDefaultInternetAccessPoint               = { KRadioCRCoreSettingsBase + 0x0000000B };

/** The key associated with application id setting.  */
const TUint32 KRadioCRApplicationId                            = { KRadioCRCoreSettingsBase + 0x0000000C };

/** The key associated with the network id setting. */
const TUint32 KRadioCRNetworkId                                = { KRadioCRCoreSettingsBase + 0x0000000D };

/** The key associated with the subscriber id setting. */
const TUint32 KRadioCRSubscriberId                             = { KRadioCRCoreSettingsBase + 0x0000000E };

/** The key associated with the connection destination setting. */
const TUint32 KRadioCRConnectionDestinationId                  = { KRadioCRCoreSettingsBase + 0x0000000F };

/** The key associated with the country code setting. */
const TUint32 KRadioCRCountryCode                              = { KRadioCRCoreSettingsBase + 0x00000010 };

#endif // RADIOINTERNALCRKEYS_H
