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
/**  Radio control operation name. */
const QString KRadioServiceControlOperation = "command(int)";
/**  Radio monitor interface name. */
const QString KRadioServiceMonitorInterfaceName = "IRadioMonitor";
/**  Radio monitor operation name. */
const QString KRadioServiceMonitorOperation = "requestNotifications()";
/**  Radio refresh operation name. */
const QString KRadioServiceRefreshOperation = "requestAllData()";

class FmRadioHsWidgetRadioServiceClient : public QObject
{
Q_OBJECT

public:
    explicit FmRadioHsWidgetRadioServiceClient(QObject *parent = 0);
    virtual ~FmRadioHsWidgetRadioServiceClient();
    
    // Enum for station changing command.
    enum FmRadioStationChangeCommand
    {
        PreviousFavouriteStation,
        NextFavouriteStation
    };
    
    // Enum for type of radio information.
    enum FmRadioInformationType
    {
        InformationTypeStationName,
        InformationTypeCallSign,
        InformationTypeFrequency,
        InformationTypeRt,
        InformationTypeDynamicPsName,
        InformationTypePty
    };
    
    // Enum for controlling the visibility of the radio application.
    enum FmRadioVisibilty
    {
        DoNotChange,
        ToForeground,
        ToBackground
    };

signals:
    void radioInformationChanged(const int notificationId,
        const QVariant &value);
    void radioStateChanged(const QVariant &value);

public slots:
    void doCloseFmRadio();
    void doPowerOnFmRadio();
    void doChangeFmRadioVisibility(const FmRadioVisibilty visibility);
    void doChangeFmRadioStation(const FmRadioStationChangeCommand command);
    void handleFmRadioInformationChange(const QVariant &value);
    void handleFmRadioStateChange(const QVariant &value);
    void startMonitoring(const FmRadioVisibilty visibility);
    void stopMonitoring();

protected slots:

private slots:
    void handleError(const int errorCode, const QString &errorMessage);
    void handleRequestError(const int error);
    
    void createControlServiceRequest();
    void createMonitorServiceRequest();

    void doSendMonitorRequest(const FmRadioVisibilty visibility);
    void doSendControlRequest(const QVariant &argument,
        const FmRadioVisibilty visibility);
    
    void prepareRequestInfo(XQAiwRequest *request,
        const FmRadioVisibilty visibility);
    
private: // data
    bool mRequestPending;
    XQApplicationManager mApplicationManager;
    XQAiwRequest* mRadioMonitorRequest;
    XQAiwRequest* mRadioControlRequest;
    
    bool mDataInitialized;
};

#endif /* FMRADIOHSWIDGETRADIOSERVICECLIENT_H_ */
