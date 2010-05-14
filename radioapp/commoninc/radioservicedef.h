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

#ifndef RADIOSERVICES_H
#define RADIOSERVICES_H

static QString RADIO_CONTROL_SERVICE = "com.nokia.services.Radio.IRadioControl";
static QString RADIO_MONITOR_SERVICE = "com.nokia.services.Radio.IRadioMonitor";

const long KRadioPSUid = 0x101FF976;
const unsigned long KRadioStartupKey = 0x00000014;

namespace RadioServiceCommand
{
    enum CommandId
    {
        Play,
        Pause,
        Previous,
        Next,
        SeekUp,
        SeekDown,
        Foreground,
        Background
    };
}

namespace RadioServiceNotification
{
    enum Type {
        FavoriteCount = 1,
        RadioStatus,
        Frequency,
        Name,
        Genre,
        RadioText,
        HomePage,
        Song
    };
}

namespace RadioStatus
{
    enum Status {
        UnSpecified,
        Playing,
        Muted,
        Seeking,
        NoAntenna
    };
}

#endif // RADIOSERVICES_H
