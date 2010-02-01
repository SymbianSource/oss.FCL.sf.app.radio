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
* Description:  Definition of central repository keys for FM Radio Engine.
*
*/


#ifndef FMRADIOENGINECRKEYS_H
#define FMRADIOENGINECRKEYS_H

// FM Radio Engine UID
const TUid KCRUidFMRadioEngine = {0x2001B25E};

// Radio headset volume
const TUint32 KRadioHeadsetVolume =  0x00000001;

// Radio speaker volume
const TUint32 KRadioSpeakerVolume = 0x00000002;

// The last played radio frequency
const TUint32 KFmRadioFrequency = 0x00000003;

// The last played radio channel
const TUint32 KFmRadioChannel = 0x00000004;

// The radio mode before radio off 
const TUint32 KFmRadioMode = 0x00000005;

// Country code + network ID
const TUint32 KFMRadioCREmulatorNetworkInfo =  0x00000006; 

// number of startups
const TUint32 KRadioCRStartupCount = 0x00000007;

// currently active region
const TUint32 KFMCRCurrentRegion = 0x00000008;

// Japan region support on/off
const TUint32 KFMCRJapanRegionAllowed = 0x00000050;

// Rds af seacrh enabled
const TUint32 KFMCRRdsAfSearchEnabled = 0x00000009;

// The first preset channel's location in central repository
const TUint32 KFmRadioPresetChannel1 = 0x0000000A;

// Is note "'Music Store' got enabled..." viewed already?
const TUint32 KRadioMusicStoreNoteDisplayed = 0x0000000B;

// Which RT+ interactions are supported?
const TUint32 KRadioRTPlusSupportLevel = 0x0000000C;

// Application UID of the song recognition service
const TUint32 KFmRadioSongRecognitionUid = 0x0000000D;

#endif      // FMRADIOENGINECRKEYS_H

// End of file


