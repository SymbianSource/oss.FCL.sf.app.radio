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

#ifndef RADIOSERVICECONST_H
#define RADIOSERVICECONST_H

static const char* RADIO_CONTROL_SERVICE = "com.nokia.services.Radio.IRadioControl";
static const char* RADIO_MONITOR_SERVICE = "com.nokia.services.Radio.IRadioMonitor";

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
    enum NotificationId
    {
        Name,
        Genre,
        RadioText,
        Homepage,
        Song
    };
}

#endif // RADIOSERVICECONST_H
