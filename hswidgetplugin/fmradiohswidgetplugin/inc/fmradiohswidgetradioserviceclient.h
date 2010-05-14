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
* Description: FM Radio widget radio service client
*
*/

#ifndef FMRADIOHSWIDGETRADIOSERVICECLIENT_H_
#define FMRADIOHSWIDGETRADIOSERVICECLIENT_H_

// System includes
#include <QObject>
#include <xqservicerequest.h>
#include <xqappmgr.h>

// Constants
/**  Radio service name. */
const QString KRadioServiceName = "com.nokia.services.Radio";
/**  Radio control interface name. */
const QString KRadioServiceControlInterfaceName = "IRadioControl";
/**  Radio monitor interface name. */
const QString KRadioServiceControlOperation = "command(int)";
/**  Radio monitor interface name. */
const QString KRadioServiceMonitorInterfaceName = "IRadioMonitor";
/**  Radio monitor interface name. */
const QString KRadioServiceMonitorOperation = "requestNotifications()";
/**  Radio refresh operation name. */
const QString KRadioServiceRefreshOperation = "requestAllData()";

class FmRadioHsWidgetRadioServiceClient : public QObject
{
Q_OBJECT

public:
    FmRadioHsWidgetRadioServiceClient(QObject *parent = 0);
    virtual ~FmRadioHsWidgetRadioServiceClient();
    
    enum FmRadioStartupState
    {
        StartForeground,
        StartBackground
    };
    enum FmRadioChannelChangeCommand
    {
        PreviousFavouriteChannel,
        NextFavouriteChannel
    };
    enum FmRadioAudioControlCommand
    {
        Mute,
        Unmute
    };
    enum FmRadioInformationType
    {
        InformationTypeStationName,
        InformationTypeCallSign,
        InformationTypeFrequency,
        InformationTypeRt,
        InformationTypeDynamicPsName,
        InformationTypePty
    };
    enum FmRadioVisibilty
    {
        DoNotChange,
        ToForeground,
        ToBackground
    };

    void init();

signals:
    void radioInformationChanged(int notificationId, QVariant value);
    void radioStateChanged(QVariant value);

public slots:
    void doChangeFmRadioVisibility(FmRadioVisibilty visibility);
    void test();
    void doChangeFmRadioChannel(FmRadioChannelChangeCommand command);
    void doControlFmRadioAudio(FmRadioAudioControlCommand command);
    void handleFmRadioInformationChange(const QVariant& value);
    void handleFmRadioStateChange(QVariant& value);
    void startMonitoring(FmRadioVisibilty visibility);
    void stopMonitoring();

protected slots:
    void requestCompleted(const QVariant& value);

private slots:
    //void handleOk(const QVariant &result);
    void handleError(int errorCode, const QString& errorMessage);
    void handleRequestError(int error);
    
    void createControlServiceRequest();
    void createMonitorServiceRequest();

    void doSendMonitorRequest(FmRadioVisibilty visibility);
    void doSendControlRequest(QVariant &argument, FmRadioVisibilty visibility);
    
    void prepareRequestInfo(XQAiwRequest *request, FmRadioVisibilty visibility);
    
private: // data
    bool mRequestPending;
    XQApplicationManager mApplicationManager;
    XQAiwRequest* mRadioMonitorRequest;
    XQAiwRequest* mRadioControlRequest;
    
    bool                mDataInitialized;

};

#endif /* FMRADIOHSWIDGETRADIOSERVICECLIENT_H_ */
