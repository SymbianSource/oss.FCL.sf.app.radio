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

// System includes
#include <xqserviceglobal.h>

// User includes
#include "fmradiohswidgetradioserviceclient.h"
#include "fmradiohswidget.h"
#include "radioservicedef.h"
#include "radionotificationdata.h"
#include "radiologger.h"

/*!
 Constructor
 */
FmRadioHsWidgetRadioServiceClient::FmRadioHsWidgetRadioServiceClient(QObject *parent) :
    QObject(parent),
    mRequestPending(false),
    mRadioMonitorRequest(NULL),
    mRadioControlRequest(NULL),
    mDataInitialized(false)
{
    LOG_METHOD;
}

/*!
 Destructor 
 */
FmRadioHsWidgetRadioServiceClient::~FmRadioHsWidgetRadioServiceClient()
{
    LOG_METHOD;
    stopMonitoring();
}

/*!
 Close FM Radio application.
 
 */
void FmRadioHsWidgetRadioServiceClient::doCloseFmRadio()
{
    LOG_METHOD;
    QVariant commandArgument;
    commandArgument.setValue(static_cast<int>(RadioServiceCommand::PowerOff));
    doSendControlRequest(commandArgument, DoNotChange);
}

/*!
 Power on FM Radio application.
 This is used only when radio is in Closing state. This merely cancels the
 radio power off delay and unmutes the audio. Currently the starting of radio
 is done with startMonitoring(). 
 */
void FmRadioHsWidgetRadioServiceClient::doPowerOnFmRadio()
{
    LOG_METHOD;
    QVariant commandArgument;
    commandArgument.setValue(static_cast<int>(RadioServiceCommand::PowerOn));
    doSendControlRequest(commandArgument, DoNotChange);
}


/*!
 Bring FM Radio to foreground.
 
 */
void FmRadioHsWidgetRadioServiceClient::doChangeFmRadioVisibility(
    const FmRadioVisibilty visibility)
{
    LOG_METHOD;
    QVariant commandArgument;
    switch (visibility) {
    case ToForeground:
        LOG("ToForeground");
        commandArgument.setValue(static_cast<int>(RadioServiceCommand::Foreground));
        break;
    case ToBackground:
        LOG("ToBackground");
        commandArgument.setValue(static_cast<int>(RadioServiceCommand::Background));
    case DoNotChange:
    default:
        LOG("DoNotChange or default");
        break;
    }
    doSendControlRequest(commandArgument, visibility);
}

/*!
 Changing of FM Radio station.
 
 /param command Command to execute.
 */
void FmRadioHsWidgetRadioServiceClient::doChangeFmRadioStation(
    const FmRadioStationChangeCommand command)
{
    LOG_METHOD;
    QVariant commandArgument;
    switch (command) {
    case PreviousFavouriteStation:
        LOG("PreviousFavouriteChannel");
        commandArgument.setValue(static_cast<int>(RadioServiceCommand::Previous));
        break;
    case NextFavouriteStation:
        LOG("NextFavouriteChannel");
        commandArgument.setValue(static_cast<int>(RadioServiceCommand::Next));
        break;
    default:
        LOG("default");
        break;
    }
    doSendControlRequest(commandArgument, DoNotChange);
}

/*!
 Start FM Radio information listening.
 */
void FmRadioHsWidgetRadioServiceClient::doSendControlRequest(
    const QVariant &argument, const FmRadioVisibilty visibility)
{
    LOG_METHOD;
    if (!argument.isValid()) {
        return;
    }
    
    // If there is not mRadioControlRequest already.
    if (!mRadioControlRequest) {
        // Create it.
        createControlServiceRequest();
    }
    
    // Set argument list for request.
    QList<QVariant> arguments;
    arguments.append(argument);
    mRadioControlRequest->setArguments(arguments);
    
    prepareRequestInfo(mRadioControlRequest, visibility);
    
    LOG("Send request");
    bool res = mRadioControlRequest->send();

    if (!res) {
        LOG("Send failed");
        int error = mRadioControlRequest->lastError();
        handleRequestError(error);
    }
}

/*!
 Start FM Radio information listening.
 */
void FmRadioHsWidgetRadioServiceClient::doSendMonitorRequest(
    const FmRadioVisibilty visibility)
{
    LOG_METHOD;
    prepareRequestInfo(mRadioMonitorRequest, visibility);
    if (!mRequestPending) {
        LOG("Send request");
        mRequestPending = mRadioMonitorRequest->send();
    }
}

/*!
 Handle changes in FM Radio information texts.
 
 /param value
 */
void FmRadioHsWidgetRadioServiceClient::handleFmRadioInformationChange(
    const QVariant &value)
{
    LOG_METHOD;
    LEVEL2(LOG_SLOT_CALLER);
    // Request is not pending anymore.
    mRequestPending = false;
    // If first request was refresh operation.
    if (!mDataInitialized) {
        LOG("Set operation to KRadioServiceMonitorOperation");
        // Change the operation to the monitoring.
        mRadioMonitorRequest->setOperation(KRadioServiceMonitorOperation);
        // Data is now initialized.
        mDataInitialized = true;
    }
    // Request notifications again.
    startMonitoring(DoNotChange);
    
    // If valid and right kind of data was received.
    if ( value.isValid() && value.canConvert( QVariant::List ) ) {
        QVariantList notificationList = value.toList();
        // Iterate through the list.
        foreach ( const QVariant& variant, notificationList ) {
            // Extract notification data.
            RadioNotificationData notification = variant.value<RadioNotificationData>();
            // And it's type.
            const int notificationId = notification.mType;
            // Emit the new information.
            emit radioInformationChanged( notificationId, notification.mData );
        }
    }
}

/*!
 Handles request error.
 
 /param int Error value.
 */
void FmRadioHsWidgetRadioServiceClient::handleRequestError(const int error)
{
    LOG_METHOD;
    QString errorStr;
    QVariant var(FmRadioHsWidget::NotRunning);
    switch (error) {
    case XQService::ENoError:
        errorStr = "No error";
        break;
    case XQService::EConnectionError:
        errorStr = "Error in IPC Connection";
        break;
    case XQService::EConnectionClosed:
        errorStr = "IPC Connection is closed";
        stopMonitoring();
        handleFmRadioStateChange(var);
        break;
    case XQService::EServerNotFound:
        errorStr = "Can not find server";
        break;
    case XQService::EIPCError:
        errorStr = "Known IPC error defined by SDK";
        break;
    case XQService::EUnknownError:
        errorStr = "Unknown IPC error";
        break;
    case XQService::ERequestPending:
        errorStr = "Already pending request";
        break;
    default:
        errorStr = "default case";
        break;
    }
    LOG(GETSTRING(errorStr));
}

/*!
 Handle changes in FM Radio state.
 
 /param value New state of FM Radio.
 */
void FmRadioHsWidgetRadioServiceClient::handleFmRadioStateChange(
    const QVariant &value)
{
    LOG_METHOD;
    if (value.isValid()) {
        emit radioStateChanged(value);
    }
}

/*!
 Handle error.
 
 /param errorCode
 /param errorMessage
 */
void FmRadioHsWidgetRadioServiceClient::handleError(const int errorCode,
    const QString &errorMessage)
{
    Q_UNUSED(errorMessage)
    LOG_METHOD;
    LEVEL2(LOG_SLOT_CALLER);
    handleRequestError(errorCode);
}

/*!
 Creates control service request object.
 */
void FmRadioHsWidgetRadioServiceClient::createControlServiceRequest()
{
    LOG_METHOD;
    if (!mRadioControlRequest) {
        // Following commented code is for debugging when changing service and interface names.
        //QString fullInterfaceName = /*KRadioServiceName +"."+*/ KRadioServiceControlInterfaceName;
        /*
        QList<XQAiwInterfaceDescriptor> list;
        list = mApplicationManager.list(KRadioServiceName, fullInterfaceName, "");
        XQAiwInterfaceDescriptor interfaceDescriptor;
        foreach (XQAiwInterfaceDescriptor d, list)
            {
                QString in = d.interfaceName();
                QString sn = d.serviceName();
                if (sn == KRadioServiceName && in == fullInterfaceName) {
                    interfaceDescriptor = d;
                }
            }
        */
        /*
        mRadioMonitorRequest = mApplicationManager.create(interfaceDescriptor,
            KRadioServiceControlOperation, false);
        */
        
        LOG("Create request");
        mRadioControlRequest = mApplicationManager.create(
            KRadioServiceControlInterfaceName, KRadioServiceControlOperation,
            false);
        
        if (mRadioControlRequest) {
            LOG("Request created");
            // Request is synchronous.
            mRadioControlRequest->setSynchronous(true);
            // Request is not embedded.
            mRadioControlRequest->setEmbedded(false);
        }
    }
}

/*!
 Creates monitor service request object.
 */
void FmRadioHsWidgetRadioServiceClient::createMonitorServiceRequest()
{
    LOG_METHOD;
    if (!mRadioMonitorRequest) {
        // If data is not initialized, set operation to refresh,
        // otherwise to monitor operation.
		QString operation = mDataInitialized ? KRadioServiceMonitorOperation
            : KRadioServiceRefreshOperation;
        QString fullInterfaceName = /*KRadioServiceName +"."+*/ KRadioServiceMonitorInterfaceName;

        // Following commented code is for debugging when changing service and interface names.
        /*
        QList<XQAiwInterfaceDescriptor> list;
        list = mApplicationManager.list(KRadioServiceName, fullInterfaceName, "");
        XQAiwInterfaceDescriptor interfaceDescriptor;
        foreach (XQAiwInterfaceDescriptor d, list)
            {
                QString in = d.interfaceName();
                QString sn = d.serviceName();
                if (sn == KRadioServiceName && in == fullInterfaceName) {
                    interfaceDescriptor = d;
                }
            }
        */
        /*
        mRadioMonitorRequest = mApplicationManager.create(interfaceDescriptor,
            operation, false);
        */
        
        LOG("Create request");
        mRadioMonitorRequest = mApplicationManager.create(
            KRadioServiceMonitorInterfaceName, operation, false);
        
        if (mRadioMonitorRequest) {
            LOG("Request created");
            // Connect request to handle it's completion.
            connect(mRadioMonitorRequest, SIGNAL(requestOk(const QVariant&)),
                this, SLOT(handleFmRadioInformationChange(const QVariant&)));
            // Connect request to handle it's error.
            connect(mRadioMonitorRequest,
                SIGNAL(requestError(int,const QString&)), this,
                SLOT(handleError(int,const QString&)));
            
            // Request is asynchronous.
            mRadioMonitorRequest->setSynchronous(false);
            // Request is not embedded.
            mRadioMonitorRequest->setEmbedded(false);
        }
    }
}

/*!
 Start radio monitoring.
 */
void FmRadioHsWidgetRadioServiceClient::startMonitoring(FmRadioVisibilty visibility)
{
    LOG_METHOD;
    // If there is no mRadioMonitorRequest create it.
    if (!mRadioMonitorRequest) {
        createMonitorServiceRequest();
    }
    doSendMonitorRequest(visibility);
}

/*!
 Stops radio monitoring.
 */
void FmRadioHsWidgetRadioServiceClient::stopMonitoring()
{
    LOG_METHOD;
    // Delete the mRadioMonitorRequest. 
    if (mRadioMonitorRequest) {
        delete mRadioMonitorRequest;
        mRadioMonitorRequest = NULL;
        mRequestPending = false;
        mDataInitialized = false;
    }
}

/*!
 Prepares the visibility of the request.
 */
void FmRadioHsWidgetRadioServiceClient::prepareRequestInfo(
    XQAiwRequest *request, const FmRadioVisibilty visibility)
{
    LOG_METHOD;
    XQRequestInfo info;
    switch (visibility) {
    case ToForeground:
        info.setForeground(true);
        break;
    case ToBackground:
        info.setBackground(true);
        break;
    case DoNotChange:
    default:
        break;
    }
    if (request) {
        request->setInfo(info);
    }
}
