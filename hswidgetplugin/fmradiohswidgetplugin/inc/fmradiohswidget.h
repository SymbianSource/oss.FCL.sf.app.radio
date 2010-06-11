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

// System includes
#include <HbWidget>

// User includes
#include "fmradiohswidgetradioserviceclient.h"

// Forward declarations
class HbLabel;
class HbPushButton;
class HbMarqueeItem;
class XQSettingsManager;
class XQSettingsKey;
class FmRadioHsWidgetProcessHandler;
class FmRadioHsWidgetProfileReader;
class QGraphicsLinearLayout;

// Constants
/** Path to docml file */
const QString KDocml = ":/ui/resource/fmradiohswidget.docml";
/** Path to css file */
const QString KCss = ":/ui/resource/fmradiohswidgetplugin.css";
/**  Key for radio information hash. */
const QString KRadioInformationStationName = "stationName";
/**  Key for radio information hash. */
const QString KRadioInformationFrequency = "frequency";
/**  Key for radio information hash. */
const QString KRadioInformationRt = "rt";
/**  Key for radio information hash. */
const QString KRadioInformationDynamicPsName = "dynamicPsName";
/**  Key for radio information hash. */
const QString KRadioInformationPty = "pty";
/**  DOCML object name for mainLayout */
const QString KDocmlObjectNameMainLayout = "mainLayout";
/**  DOCML object name for tunerBackgroundPushButton */
const QString KDocmlObjectNameTunerBackgroundPushButton = "tunerBackgroundPushButton";
/**  DOCML object name for tunerStackedLayout */
const QString KDocmlObjectNameTunerStackedLayout = "tunerStackedLayout";
/**  DOCML object name for tunerInformationStackedLayout */
const QString KDocmlObjectNameTunerInformationStackedLayout = "tunerInformationStackedLayout";
/**  DOCML object name for controlButtons */
const QString KDocmlObjectNameContolButtonsLayout = "controlButtons";
/**  DOCML object name for powerToggleButton */
const QString KDocmlObjectNameowerToggleButton = "powerToggleButton";
/**  DOCML object name for previousPushButton */
const QString KDocmlObjectNamePreviousPushButton = "previousPushButton";
/**  DOCML object name for nextPushButton */
const QString KDocmlObjectNameNextPushButton = "nextPushButton";
/**  DOCML object name for twoRowsLayout */
const QString KDocmlObjectNameTwoRowsLayout = "twoRowsLayout";
/**  DOCML object name for firstRowLabel */
const QString KDocmlObjectNameFirstRowLabel = "firstRowLabel";
/**  DOCML object name for secondRowLabel */
const QString KDocmlObjectNameSecondRowLabel = "secondRowLabel";
/**  DOCML object name for lonelyRowLabel */
const QString KDocmlObjectNameLonelyRowLabel = "lonelyRowLabel";
/**  DOCML object name for animationIcon */
const QString KDocmlObjectNameAnimationIcon = "animationIcon";
/** Delay which after the widget returns to the state where radio is not running. */
const int KRadioStartingStateCancelDelay = 10000;

class FmRadioHsWidget : public HbWidget
{
    Q_OBJECT
    Q_PROPERTY(QString rootPath READ rootPath WRITE setRootPath)
public:
    FmRadioHsWidget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~FmRadioHsWidget();

public:
    // Enum for commands controlling FM Radio.
    enum FmRadioControlCommand
    {
        Mute,
        Unmute,
        PreviousChannel,
        NextChannel
    };

    // Enum for FM Radio states.
    enum FmRadioState
    {
        Undefined,
        NotRunning,
        Starting,
        Running,
        ControllingAudio,
        NotControllingAudio,
        Seeking,
        AntennaNotConnected,
        Closing
    };

    // Enum for information area layout states.
    enum InformationAreaLayout
    {
        OneRow,
        TwoRows,
        Animation
    };

    // Enum for control button position.
    enum ControlButtonPosition
    {
        Left,
        Center,
        Right
    };
    
    QString rootPath()const;
    void setRootPath(const QString &rootPath);

public slots:
    void onInitialize();
    void onShow();
    void onHide();
    //void onUninitialize(); // Can be used in future to get on unintialize event.

private slots:
    void load(const QString &docml);
    void closeRadio();
    void previousChannel();
    void nextChannel();
    void radioToForeground();
    void radioToBackground();

    void handleRadioInformationChange(const int notificationId,
        const QVariant &value);
    bool updateRadioInformation(const QString &informationType,
        const QString &information);
    void radioInformationChanged();
    void clearRadioInformation();
    void handleRadioStateChange(const QVariant &value);
    
    void changeInformationAreaLayout(const InformationAreaLayout layout);
    void changePowerButtonOn(const bool isPowerOn);
    void changeStationButtonsEnabledState(const bool enabled);
    void changeControlButtonFrameBackground(const bool enabled,
        const ControlButtonPosition position, HbPushButton *button);
    
    void cancelRadioStartingState();
    void stopRadioStartingCancelTimer();
    
private:
    // Data
    Q_DISABLE_COPY(FmRadioHsWidget)
    
    QString mRootPath;
    
    // UI components.
    HbPushButton *mRadioPushButton;
    HbPushButton *mTunerBackgroundPushButton;
    HbPushButton *mPowerToggleButton;
    HbPushButton *mPreviousPushButton;
    HbPushButton *mNextPushButton;
    QGraphicsWidget *mInformationAreaTwoRowsLayout;
    HbLabel *mInformationFirstRowLabel;
    HbLabel *mInformationSecondRowLabel;
    HbLabel *mInformationLonelyRowLabel;
    HbLabel *mAnimationIcon;
    
    // Stores the state of the FM Radio application.
    FmRadioState mFmRadioState;
    // Stores the presence of favorite stations.
    bool mFavoriteStations;

    // Stores the radio information.
    QHash<QString, QString> mRadioInformation;
    QString mRadioInformationFirstRow;
    QString mRadioInformationSecondRow;

    // For reading P&S keys.
    FmRadioHsWidgetProfileReader *mProfileMonitor;
    
    // For communicating with the FM Radio through Qt Highway.
    FmRadioHsWidgetRadioServiceClient *mRadioServiceClient;
    
    // For cancelinf the starting state after a delay.
    QTimer *mRadioStartingCancelTimer;
    
};

#endif // FMRADIOHSWIDGET_H
