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

namespace CommandSender
{
    enum Sender
    {
        Unspecified,
        FrequencyStrip,
        StationCarousel
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

struct GenreStruct
    {
    int mGenreCode;
    struct{ const char *source; const char *comment; } mTranslation;
    };

//static const char* GenreContext = "Genre";

const GenreStruct EuropeanGenres[] =
    {
     { GenreEurope::RdsNone,                QT_TRANSLATE_NOOP3( GenreContext, "None",                 "txt_genre_europe_none" ) }
    ,{ GenreEurope::RdsNews,                QT_TRANSLATE_NOOP3( GenreContext, "News",                 "txt_genre_europe_news" ) }
    ,{ GenreEurope::RdsCurrentAffairs,      QT_TRANSLATE_NOOP3( GenreContext, "Current affairs",      "txt_genre_europe_current_affairs" ) }
    ,{ GenreEurope::RdsInformation,         QT_TRANSLATE_NOOP3( GenreContext, "Information",          "txt_genre_europe_information" ) }
    ,{ GenreEurope::RdsSport,               QT_TRANSLATE_NOOP3( GenreContext, "Sport",                "txt_genre_europe_sport" ) }
    ,{ GenreEurope::RdsEducation,           QT_TRANSLATE_NOOP3( GenreContext, "Education",            "txt_genre_europe_education" ) }
    ,{ GenreEurope::RdsDrama,               QT_TRANSLATE_NOOP3( GenreContext, "Drama",                "txt_genre_europe_drama" ) }
    ,{ GenreEurope::RdsCulture,             QT_TRANSLATE_NOOP3( GenreContext, "Culture",              "txt_genre_europe_culture" ) }
    ,{ GenreEurope::RdsScience,             QT_TRANSLATE_NOOP3( GenreContext, "Science",              "txt_genre_europe_science" ) }
    ,{ GenreEurope::RdsVariedSpeech,        QT_TRANSLATE_NOOP3( GenreContext, "Varied speech",        "txt_genre_europe_varied_speech" ) }
    ,{ GenreEurope::RdsPopMusic,            QT_TRANSLATE_NOOP3( GenreContext, "Pop music",            "txt_genre_europe_pop_music" ) }
    ,{ GenreEurope::RdsRockMusic,           QT_TRANSLATE_NOOP3( GenreContext, "Rock music",           "txt_genre_europe_rock_music" ) }
    ,{ GenreEurope::RdsEasyListening,       QT_TRANSLATE_NOOP3( GenreContext, "Easy listening",       "txt_genre_europe_easy_listening" ) }
    ,{ GenreEurope::RdsLightClassical,      QT_TRANSLATE_NOOP3( GenreContext, "Light classical",      "txt_genre_europe_light_classical" ) }
    ,{ GenreEurope::RdsSeriousClassical,    QT_TRANSLATE_NOOP3( GenreContext, "Serious classical",    "txt_genre_europe_serious_classical" ) }
    ,{ GenreEurope::RdsOtherMusic,          QT_TRANSLATE_NOOP3( GenreContext, "Other music",          "txt_genre_europe_other_music" ) }
    ,{ GenreEurope::RdsWeather,             QT_TRANSLATE_NOOP3( GenreContext, "Weather",              "txt_genre_europe_weather" ) }
    ,{ GenreEurope::RdsFinance,             QT_TRANSLATE_NOOP3( GenreContext, "Finance",              "txt_genre_europe_finance" ) }
    ,{ GenreEurope::RdsChildrensProgrammes, QT_TRANSLATE_NOOP3( GenreContext, "Childrens programmes", "txt_genre_europe_childrens_programmes" ) }
    ,{ GenreEurope::RdsSocialAffairs,       QT_TRANSLATE_NOOP3( GenreContext, "Social affairs",       "txt_genre_europe_social_affairs" ) }
    ,{ GenreEurope::RdsReligion,            QT_TRANSLATE_NOOP3( GenreContext, "Religion",             "txt_genre_europe_religion" ) }
    ,{ GenreEurope::RdsPhoneIn,             QT_TRANSLATE_NOOP3( GenreContext, "Phone in",             "txt_genre_europe_phone_in" ) }
    ,{ GenreEurope::RdsTravel,              QT_TRANSLATE_NOOP3( GenreContext, "Travel",               "txt_genre_europe_travel" ) }
    ,{ GenreEurope::RdsLeisure,             QT_TRANSLATE_NOOP3( GenreContext, "Leisure",              "txt_genre_europe_leisure" ) }
    ,{ GenreEurope::RdsJazzMusic,           QT_TRANSLATE_NOOP3( GenreContext, "Jazz music",           "txt_genre_europe_jazz_music" ) }
    ,{ GenreEurope::RdsCountryMusic,        QT_TRANSLATE_NOOP3( GenreContext, "Country music",        "txt_genre_europe_country_music" ) }
    ,{ GenreEurope::RdsNationalMusic,       QT_TRANSLATE_NOOP3( GenreContext, "National music",       "txt_genre_europe_national_music" ) }
    ,{ GenreEurope::RdsOldiesMusic,         QT_TRANSLATE_NOOP3( GenreContext, "Oldies music",         "txt_genre_europe_oldies_music" ) }
    ,{ GenreEurope::RdsFolkMusic,           QT_TRANSLATE_NOOP3( GenreContext, "Folk music",           "txt_genre_europe_folk_music" ) }
    ,{ GenreEurope::RdsDocumentary,         QT_TRANSLATE_NOOP3( GenreContext, "Documentary",          "txt_genre_europe_documentary" ) }
    ,{ GenreEurope::RdsAlarmTest,           QT_TRANSLATE_NOOP3( GenreContext, "Alarm test",           "txt_genre_europe_alarm_test" ) }
    ,{ GenreEurope::RdsAlarm,               QT_TRANSLATE_NOOP3( GenreContext, "Alarm",                "txt_genre_europe_alarm" ) }
    };
const int EuropeanGenresCount = sizeof( EuropeanGenres ) / sizeof ( EuropeanGenres[0] );

const GenreStruct AmericanGenres[] =
    {
     { GenreAmerica::RbdsNone,               QT_TRANSLATE_NOOP3( GenreContext, "None",                  "txt_genre_america_none" ) }
    ,{ GenreAmerica::RbdsNews,               QT_TRANSLATE_NOOP3( GenreContext, "News",                  "txt_genre_america_news" ) }
    ,{ GenreAmerica::RbdsInformation,        QT_TRANSLATE_NOOP3( GenreContext, "Information",           "txt_genre_america_information" ) }
    ,{ GenreAmerica::RbdsSports,             QT_TRANSLATE_NOOP3( GenreContext, "Sports",                "txt_genre_america_sports" ) }
    ,{ GenreAmerica::RbdsRock,               QT_TRANSLATE_NOOP3( GenreContext, "Rock",                  "txt_genre_america_rock" ) }
    ,{ GenreAmerica::RbdsClassicRock,        QT_TRANSLATE_NOOP3( GenreContext, "Classic rock",          "txt_genre_america_classic_rock" ) }
    ,{ GenreAmerica::RbdsAdultHits,          QT_TRANSLATE_NOOP3( GenreContext, "Adult hits",            "txt_genre_america_adult_hits" ) }
    ,{ GenreAmerica::RbdsSoftRock,           QT_TRANSLATE_NOOP3( GenreContext, "Soft rock",             "txt_genre_america_soft_rock" ) }
    ,{ GenreAmerica::RbdsTop40,              QT_TRANSLATE_NOOP3( GenreContext, "Top 40",                "txt_genre_america_top_40" ) }
    ,{ GenreAmerica::RbdsCountry,            QT_TRANSLATE_NOOP3( GenreContext, "Country",               "txt_genre_america_country" ) }
    ,{ GenreAmerica::RbdsOldies,             QT_TRANSLATE_NOOP3( GenreContext, "Oldies",                "txt_genre_america_oldies" ) }
    ,{ GenreAmerica::RbdsSoft,               QT_TRANSLATE_NOOP3( GenreContext, "Soft",                  "txt_genre_america_soft" ) }
    ,{ GenreAmerica::RbdsNostalgia,          QT_TRANSLATE_NOOP3( GenreContext, "Nostalgia",             "txt_genre_america_nostalgia" ) }
    ,{ GenreAmerica::RbdsJazz,               QT_TRANSLATE_NOOP3( GenreContext, "Jazz",                  "txt_genre_america_jazz" ) }
    ,{ GenreAmerica::RbdsClassical,          QT_TRANSLATE_NOOP3( GenreContext, "Classical",             "txt_genre_america_classical" ) }
    ,{ GenreAmerica::RbdsRhythmAndBlues,     QT_TRANSLATE_NOOP3( GenreContext, "Rhythm and blues",      "txt_genre_america_rhythm_and_blues" ) }
    ,{ GenreAmerica::RbdsSoftRhythmAndBlues, QT_TRANSLATE_NOOP3( GenreContext, "Soft rhythm and blues", "txt_genre_america_soft_rhythm_and_blues" ) }
    ,{ GenreAmerica::RbdsLanguage,           QT_TRANSLATE_NOOP3( GenreContext, "Language",              "txt_genre_america_language" ) }
    ,{ GenreAmerica::RbdsReligiousMusic,     QT_TRANSLATE_NOOP3( GenreContext, "Religuous music",       "txt_genre_america_religuous_music" ) }
    ,{ GenreAmerica::RbdsReligiousTalk,      QT_TRANSLATE_NOOP3( GenreContext, "Religuous talk",        "txt_genre_america_religuous_talk" ) }
    ,{ GenreAmerica::RbdsPersonality,        QT_TRANSLATE_NOOP3( GenreContext, "Personality",           "txt_genre_america_personality" ) }
    ,{ GenreAmerica::RbdsPublic,             QT_TRANSLATE_NOOP3( GenreContext, "Public",                "txt_genre_america_public" ) }
    ,{ GenreAmerica::RbdsCollege,            QT_TRANSLATE_NOOP3( GenreContext, "College",               "txt_genre_america_college" ) }
    ,{ GenreAmerica::RbdsUnassigned1,        QT_TRANSLATE_NOOP3( GenreContext, "Unassigned 1",          "txt_genre_america_unassigned_1" ) }
    ,{ GenreAmerica::RbdsUnassigned2,        QT_TRANSLATE_NOOP3( GenreContext, "Unassigned 2",          "txt_genre_america_unassigned_2" ) }
    ,{ GenreAmerica::RbdsUnassigned3,        QT_TRANSLATE_NOOP3( GenreContext, "Unassigned 3",          "txt_genre_america_unassigned_3" ) }
    ,{ GenreAmerica::RbdsUnassigned4,        QT_TRANSLATE_NOOP3( GenreContext, "Unassigned 4",          "txt_genre_america_unassigned_4" ) }
    ,{ GenreAmerica::RbdsUnassigned5,        QT_TRANSLATE_NOOP3( GenreContext, "Unassigned 5",          "txt_genre_america_unassigned_5" ) }
    ,{ GenreAmerica::RbdsWeather,            QT_TRANSLATE_NOOP3( GenreContext, "Weather",               "txt_genre_america_weather" ) }
    ,{ GenreAmerica::RbdsEmergencyTest,      QT_TRANSLATE_NOOP3( GenreContext, "Emergency test",        "txt_genre_america_emergency_test" ) }
    ,{ GenreAmerica::RbdsEmergency,          QT_TRANSLATE_NOOP3( GenreContext, "Emergency",             "txt_genre_america_emergency" ) }
    };
const int AmericanGenresCount = sizeof( AmericanGenres ) / sizeof ( AmericanGenres[0] );

#endif // RADIO_GLOBAL_H_
