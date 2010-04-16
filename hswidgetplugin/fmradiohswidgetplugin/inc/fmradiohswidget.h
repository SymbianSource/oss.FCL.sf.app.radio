/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  FM Radio home screen widget
*
*/

#ifndef FMRADIOHSWIDGET_H
#define FMRADIOHSWIDGET_H

// Defines
#define DOCML_WIDGET ":/gfx/resource/fmradiohswidget.docml"

// System includes
#include <HbWidget>

// User includes
#include "fmradiohswidgetradioserviceclient.h"

// Forward declarations
class HbLabel;
class HbPushButton;
class XQSettingsManager;
class XQSettingsKey;
class FmRadioHsWidgetProcessHandler;
class FmRadioHsWidgetProfileReader;

// Constants
/**  Key for radio information hash. */
const QString KRadioInformationStationName = "stationName";
/**  Key for radio information hash. */
const QString KRadioInformationCallSign = "callSign";
/**  Key for radio information hash. */
const QString KRadioInformationFrequency = "frequency";
/**  Key for radio information hash. */
const QString KRadioInformationRt = "rt";
/**  Key for radio information hash. */
const QString KRadioInformationDynamicPsName = "dynamicPsName";
/**  Key for radio information hash. */
const QString KRadioInformationPty = "pty";
/**  Text to display in widget. */
const QString KFmRadioText = "FM Radio";
/**  Information text to connect headset. */
const QString KConnectHeadsetText = "Connect wired headset.";
/**  Mhz text. */
const QString KMhzText = "Mhz";
/**  DOCML object name for mainLayout */
const QString KDocmlObjectNameMainLayout = "mainLayout";
/**  DOCML object name for  */
const QString KDocmlObjectNameRadioIconPushButton = "radioIconPushButton";
/**  DOCML object name for  */
const QString KDocmlObjectNamePreviousPushButton = "previousPushButton";
/**  DOCML object name for  */
const QString KDocmlObjectNamePlayPushButton = "playPushButton";
/**  DOCML object name for  */
const QString KDocmlObjectNameNextPushButton = "nextPushButton";
/**  DOCML object name for  */
const QString KDocmlObjectNameTwoRowsLayout = "twoRowsLayout";
/**  DOCML object name for  */
const QString KDocmlObjectNameOneRowLayout = "oneRowLayout";
/**  DOCML object name for  */
const QString KDocmlObjectNameAnimationLayout = "animationLayout";
/**  DOCML object name for  */
const QString KDocmlObjectNameFirstRowLabel = "firstRowLabel";
/**  DOCML object name for  */
const QString KDocmlObjectNameSecondRowLabel = "secondRowLabel";
/**  DOCML object name for  */
const QString KDocmlObjectNameLonelyRowLabel = "lonelyRowLabel";
/**  DOCML object name for  */
const QString KDocmlObjectNameAnimationIcon = "animationIcon";

class FmRadioHsWidget : public HbWidget
{
    Q_OBJECT

public:
    FmRadioHsWidget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~FmRadioHsWidget();

    // enum for commands controlling FM Radio
    enum FmRadioControlCommand
    {
        Mute,
        Unmute,
        PreviousChannel,
        NextChannel
    };

    // enum for FM Radio states
    enum FmRadioState
    {
        Undefined,
        NotRunning,
        Starting,
        Running,
        ControllingAudio,
        NotControllingAudio,
        AutoScanning,
        ConnectType1Headset,
        ConnectType2Headset,
        ConnectType3Headset
    };

    // enum for information area layout states
    enum InformationAreaLayout
    {
        OneRow,
        TwoRows,
        Animation
    };

    // enum for control button states
    enum ControlButtonState
    {
        AllDisabledPlay,
        AllDisabledStop,
        ChannelsDisabledPlay,
        ChannelsDisabledStop,
        ChannelsEnabledPlay,
        ChannelsEnabledStop
    };

public slots:
    //void onInitialize(); // Can be used in future to get on intialize event.
    void onShow();
    void onHide();
    //void onUninitialize(); // Can be used in future to get on unintialize event.

    bool eventFilter(QObject *target, QEvent *event);
    
private slots:
    void load(QString docml);
    
    void mute();
    void unMute();
    void previousChannel();
    void nextChannel();
    void radioToForeground();
    void radioToBackground();

    void handleRadioInformationChange(int notificationId, QVariant value);
    bool updateRadioInformation(const QString informationType, QString information);
    void radioInformationChanged();
    void clearRadioInformation();
    void handleRadioStateChange(QVariant value);
    
    void changeInformationAreaLayout(InformationAreaLayout layout);
    void changeControlButtonState(ControlButtonState buttonState);
    
private:
    // Data
    Q_DISABLE_COPY(FmRadioHsWidget)
    
    HbPushButton *mRadioPushButton;
    HbPushButton *mPreviousPushButton;
    HbPushButton *mPlayPushButton;
    HbPushButton *mNextPushButton;
    QGraphicsWidget *mInformationAreaOneRowLayout;
    QGraphicsWidget *mInformationAreaTwoRowsLayout;
    QGraphicsWidget *mInformationAreaAnimationLayout;
    HbLabel *mInformationFirstRowLabel; // This should be maybe a HbLineEdit for displaying and receiving click on url's.
    HbLabel *mInformationSecondRowLabel;// HbLineEdit may also support marquee/scrolling.
    HbLabel *mInformationLonelyRowLabel;
    HbLabel *mAnimatioIcon;
    
    FmRadioState mFmRadioState; 

    QHash<QString, QString> mRadioInformation;  // Stores the radio information
    QString mRadioInformationFirstRow;
    QString mRadioInformationSecondRow;

    FmRadioHsWidgetProcessHandler *mProcessHandler; // For launching the FM Radio application
    
    FmRadioHsWidgetRadioServiceClient *mRadioServiceClient; // For communicating with the FM Radio through Qt Highway
    
};

#endif // FMRADIOHSWIDGET_H
