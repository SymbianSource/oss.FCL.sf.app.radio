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
* Description:  This file contains common constant defines used within the
*                 scope of the application.
*
*/


#ifndef FMRADIODEFINES_H
#define FMRADIODEFINES_H

#include <e32std.h>
#include <e32base.h>
#include <eikenv.h>
#include <fmradio.rsg>

#include "fmradioengine.h"

const TInt KHzConversionFactor    = 1000000;

// Tuning step, frequency range high and low limits
const TReal KFrequencyTuningStep        = 0.05;

const TInt KFrequencyMaxDecimalPlaces = 2;
const TInt KFrequencyMaxLength = 6;

// Minimum and maximum volume levels
const TInt KMinimumVolume = 1;
const TInt KMaximumVolume = 10;

// Handy string buffer size declarations
const TInt KBufLen16    = 0x10;

const TInt KDefaultRegionArrayGranularity = 3;

// Channel list item amount
const TInt KMinNumberOfChannelListItems = 0;
const TInt KMaxNumberOfChannelListItems = 99;

// Channel list channel item string length
const TInt KLengthOfChannelItemString = 100;

// Channel list channel index string length
const TInt KLengthOfChIndexStringChList = 5;

// Channel list channel name string length
const TInt KLengthOfChannelNameString = 80;

// Channel list channel item icon index string length
const TInt KLengthOfChannelItemIconIndexString = 3;

// Time after pressing delayed key 1 or 2
const TInt KKeyDelayTimeInMicroseconds = 2000000; // 2 seconds

const TInt KFMRadioProgressUpdateDelayInMicroseconds = 5000; //every .05 seconds.
const TInt KProgressBarMaxValue = 100;
const TInt KProgressBarIntervalChange = 10;

//Decimal notation
const TInt KBaseTen = 10;

// Asterisk key code
const TInt EStdKeyNkpAsteriskInDevice = 0x2A;

// Current mode of the channel list view
enum TFMRadioChannelListViewMode
    {
    ENormalListMode,
    ESaveChannelMode
    };

//const TInt KNumberOfButtonsToDisplay = 5;

// Channel list channel index string formatter
_LIT(KChIndexFormatChList, "%d");
_LIT(KChIndexFormatChMenu, "%d ");
// Channel list channel item icon index string formatter
_LIT(KChIconIndexFormatChList, "%d");

// bitmap file for the button panel
_LIT( KFMRadioBmpFile, "fmradio.mif" );

// Keyboard number key codes
enum TKeyboardKeys
{
   EKeyboardKey0 = 0x30,
   EKeyboardKey1 = 0x31,
   EKeyboardKey2 = 0x32,
   EKeyboardKey3 = 0x33,
   EKeyboardKey4 = 0x34,
   EKeyboardKey5 = 0x35,
   EKeyboardKey6 = 0x36,
   EKeyboardKey7 = 0x37,
   EKeyboardKey8 = 0x38,
   EKeyboardKey9 = 0x39
};

// CONSTANTS FOR ALFRED ENVIRONMENT

// Main view visual controls group id
const TInt KFMRadioVisualControlsGroupId = 1;
const TInt KFMRadioVisualizerGestureControlGroupId = 2; //Do not use for any other purpose
const TInt  KFMRadioMediaIdleId = 312;
const TInt  KFMRadioVisualControlId = 211;
const TInt  KFMRadioRdsIndicatorId = 313;
const TInt  KFMRadioRdsAfIndicatorId = 314;
const TInt  KFMRadioRdsViewer = 316;
const TInt  KFMRadioRdsInteractionIndicatorId = 317;
const TInt  KFMRadioLogoId = 318;
const TReal KFMRadioIndicatorOpacityInHiddenState = 0.2f;
const TReal KFMRadioRdsViewerOpacityHidden = 0.0f;
const TReal KFMRadioRdsViewerOpacityShow = 0.4f;


// Main view transparent layer bottom margin
const TReal KInfoBgLayerBottomMarginAsNormalizedValue = -0.12f;
// Main view transparent layer top margin
const TReal KInfoBgLayerTopMarginAsNormalizedValue    = 0.5f;
// Media Idle content item amount
const TInt KFMRadioMaxNumberOfMediaIdleContentItem = 10;

// Time delay in milliseconds when main view toolbar buttons report long key press event
const TInt KLongPressDelayValueInMicroseconds = 500;
// How many times user is asked to scan local frequencies at application startup
const TInt KMaxStartupTimesToAskSaveWizard = 2;

// Toolbar center (=mute) button state indexes
const TInt KMuteStateIndex = 1;
const TInt KUnMuteStateIndex = 0;

/**
* Local Variation Flags for FM Radio
* For values, see FMRadioVariant.hrh.
* Integer value.
**/
//_LIT( KFMRadioFeatures, "FMRadioFeatures" );

/**
* Upper frequency bound for FM Radio band
* Possible Values are:
* 0 ... 115000
*  108000 default value
* Integer value.
**/
_LIT( KFMRadioFrequencyRangeUpperLimit, "FMRadioFrequencyRangeUpperLimit" );

/**
* Lower frequency bound for FM Radio band
* Possible Values are:
* 0 ... 115000
* 87500 default value
* Integer value.
**/
_LIT( KFMRadioFrequencyRangeLowerLimit, "FMRadioFrequencyRangeLowerLimit" );

/**
 * Direction change mark for strings with numerals in mirrored layout
 */
_LIT( KRightToLeftMark, "\x200F" );
_LIT( KLeftToRightMark, "\x200E" );

/**
 * Volume settings for radio
 */
const TInt KFMRadioMaxVolumeLevel = 20;
const TInt KFMRadioMinVolumeLevel = 0;
const TInt KFMRadioVolumeStepSize = 1;

// Channel list icon index in the icon array
const TInt KNowPlayingIconIndexChList = 0;
const TInt KMoveIconIndexChList = 1;

#endif



// End of file
