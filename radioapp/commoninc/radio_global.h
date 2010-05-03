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

#ifndef RADIO_GLOBAL_H_
#define RADIO_GLOBAL_H_

// System includes
#include <QtGlobal>

// Constants

/**
 * Radio frequency multiplier
 */
const int KFrequencyMultiplier = 1000000;

const int KOneHertz = KFrequencyMultiplier;

/**
 * Desired amount of steps in the volume control in the UI
 */
const int KMaximumVolumeLevel = 20;

/**
 * Id of the offline profile
 */
const int KOfflineProfileId = 5;

/**
 * Radio Text Plus tag ids
 */
namespace RtPlus
{
    enum Tag { Title = 1, Artist = 4, Homepage = 39 };
}

namespace Seeking
{
    enum Direction
    {
        Down,
        Up
    };

    enum State
    {
        NotSeeking,
        SeekingUp,
        SeekingDown
    };
}

namespace TuneReason
{
    enum Reason
    {
        Unspecified,
        FrequencyStrip,
        StationCarousel,
        Seek,
        StationScanInitialization,
        StationScan,
    };
}

namespace RadioRegion
{
    enum Region
    {
        None = -1,
        Default,
        Japan,
        America,
        Poland
    };
}

namespace GenreEurope
{
    enum Europe
    {
        RdsNone,
        RdsNews,
        RdsCurrentAffairs,
        RdsInformation,
        RdsSport,
        RdsEducation,
        RdsDrama,
        RdsCulture,
        RdsScience,
        RdsVariedSpeech,
        RdsPopMusic,
        RdsRockMusic,
        RdsEasyListening,
        RdsLightClassical,
        RdsSeriousClassical,
        RdsOtherMusic,
        RdsWeather,
        RdsFinance,
        RdsChildrensProgrammes,
        RdsSocialAffairs,
        RdsReligion,
        RdsPhoneIn,
        RdsTravel,
        RdsLeisure,
        RdsJazzMusic,
        RdsCountryMusic,
        RdsNationalMusic,
        RdsOldiesMusic,
        RdsFolkMusic,
        RdsDocumentary,
        RdsAlarmTest,
        RdsAlarm
    };
}

namespace GenreAmerica
{
    enum America
    {
        RbdsNone,
        RbdsNews,
        RbdsInformation,
        RbdsSports,
        RbdsTalk,
        RbdsRock,
        RbdsClassicRock,
        RbdsAdultHits,
        RbdsSoftRock,
        RbdsTop40,
        RbdsCountry,
        RbdsOldies,
        RbdsSoft,
        RbdsNostalgia,
        RbdsJazz,
        RbdsClassical,
        RbdsRhythmAndBlues,
        RbdsSoftRhythmAndBlues,
        RbdsLanguage,
        RbdsReligiousMusic,
        RbdsReligiousTalk,
        RbdsPersonality,
        RbdsPublic,
        RbdsCollege,
        RbdsUnassigned1,
        RbdsUnassigned2,
        RbdsUnassigned3,
        RbdsUnassigned4,
        RbdsUnassigned5,
        RbdsWeather,
        RbdsEmergencyTest,
        RbdsEmergency
    };
}

#endif // RADIO_GLOBAL_H_
