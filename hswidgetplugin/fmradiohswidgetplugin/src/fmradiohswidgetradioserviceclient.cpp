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

/*!
 Constructor
 */
FmRadioHsWidgetRadioServiceClient::FmRadioHsWidgetRadioServiceClient(QObject *parent) :
    QObject(parent),
    mRequestPending(false),
    mRadioMonitorRequest(0),
    mRadioControlRequest(0),
    mDataInitialized(false)
{
}

/*!
 Destructor 
 */
FmRadioHsWidgetRadioServiceClient::~FmRadioHsWidgetRadioServiceClient()
{
    stopMonitoring();
}

/*!
 * Initialize all data
 */
void FmRadioHsWidgetRadioServiceClient::init()
{
    const bool radioIsRunning = false; //TODO: Find out if radio is running. Use P&S key for now
    if ( radioIsRunning ) {
        //startMonitoring();
    }
}


/*!
 Starting of FM Radio.
 
 /param startupState 
 */
/*
void FmRadioHsWidgetRadioServiceClient::doStartFmRadio(FmRadioStartupState startupState)
{
    if (!mRadioControlRequest) {
        createControlServiceRequest();
    }

    QVariant commandArgument;
    switch (startupState) {
    case StartForeground:
        // TODO: Include header and remove comment.
        commandArgument.setValue(*//*RadioServiceCommand::Foreground*/ //6);
/*        break;
    case StartBackground:
        // TODO: Include header and remove comment.
        commandArgument.setValue(*//*RadioServiceCommand::Background*/ //7);
/*        break;
    default:
        break;
    }
    QList<QVariant> arguments;
    arguments.append(commandArgument);
    mRadioControlRequest->setArguments(arguments);

    bool res = mRadioControlRequest->send();
}
*/
/*!
 Bring FM Radio to foreground.
 
 */
void FmRadioHsWidgetRadioServiceClient::doChangeFmRadioVisibility(FmRadioVisibilty visibility)
{
    QVariant commandArgument;
    switch (visibility) {
    case ToForeground:
        commandArgument.setValue((int) RadioServiceCommand::Foreground);
        break;
    case ToBackground:
        commandArgument.setValue((int) RadioServiceCommand::Background);
    case DoNotChange:
    default:
        break;
    }
    doSendControlRequest(commandArgument, visibility);
}

/*!
 Changing of FM Radio channel.
 
 /param command Command to execute.
 */
void FmRadioHsWidgetRadioServiceClient::doChangeFmRadioChannel(
    FmRadioChannelChangeCommand command)
{
    QVariant commandArgument;
    switch (command) {
    case PreviousFavouriteChannel:
        commandArgument.setValue((int) RadioServiceCommand::Previous);
        break;
    case NextFavouriteChannel:
        commandArgument.setValue((int) RadioServiceCommand::Next);
        break;
    default:
        break;
    }
    doSendControlRequest(commandArgument, DoNotChange);
}

/*!
 Changing of FM Radio channel.
 
 /param command Command to execute.
 */
void FmRadioHsWidgetRadioServiceClient::doControlFmRadioAudio(
    FmRadioAudioControlCommand command)
{
    QVariant commandArgument;
    switch (command) {
    case Mute:
        commandArgument.setValue((int) RadioServiceCommand::Pause);
        break;
    case Unmute:
        commandArgument.setValue((int) RadioServiceCommand::Play);
        break;
    default:
        break;
    }
    doSendControlRequest(commandArgument, DoNotChange);
}

/*!
 Start FM Radio information listening.
 */
void FmRadioHsWidgetRadioServiceClient::doSendControlRequest(QVariant &argument,
    FmRadioVisibilty visibility)
{
    if (!mRadioControlRequest) {
        createControlServiceRequest();
    }

    QList<QVariant> arguments;
    arguments.append(argument);
    mRadioControlRequest->setArguments(arguments);
    
    prepareRequestInfo(mRadioControlRequest, visibility);

    bool res = mRadioControlRequest->send();

    if (!res) {
        int error = mRadioControlRequest->lastError();
        handleRequestError(error);
    }
    
    //stopMonitoring();
    //startMonitoring();
}

/*!
 Start FM Radio information listening.
 */
void FmRadioHsWidgetRadioServiceClient::doSendMonitorRequest(FmRadioVisibilty visibility)
{
    prepareRequestInfo(mRadioMonitorRequest, visibility);
    if (!mRequestPending) {
        //FmRadioHsWidgetRadioServiceSingleton::instance()->sendRequest();
        mRequestPending = mRadioMonitorRequest->send();
    }
}

/*!
 Handle changes in FM Radio information texts.
 
 /param value
 */
void FmRadioHsWidgetRadioServiceClient::handleFmRadioInformationChange(const QVariant& value)
{
    mRequestPending = false;
    if (!mDataInitialized) {
        mRadioMonitorRequest->setOperation(KRadioServiceMonitorOperation);
        mDataInitialized = true;
    }
    startMonitoring(DoNotChange);
    if ( value.isValid() && value.canConvert( QVariant::List ) ) {
        QVariantList notificationList = value.toList();
        foreach ( const QVariant& variant, notificationList ) {
            RadioNotificationData notification = variant.value<RadioNotificationData>();
            const int notificationId = notification.mType;
            emit radioInformationChanged( notificationId, notification.mData );
        }
    }
}

/*!
 Handles request error.
 
 /param int Error value.
 */
void FmRadioHsWidgetRadioServiceClient::handleRequestError(int error)
{
    QString errorStr;
    QVariant var(FmRadioHsWidget::NotRunning);
    switch (error) {
    case XQService::ENoError:
        errorStr = "No error";
        break;
    case XQService::EConnectionError:
        errorStr = "(/*!< Error in IPC Connection */";
        break;
    case XQService::EConnectionClosed:
        errorStr = "/*!< IPC Connection is closed */";
        stopMonitoring();
        handleFmRadioStateChange(var);
        break;
    case XQService::EServerNotFound:
        errorStr = "/*!< Can not find server */";
        break;
    case XQService::EIPCError:
        errorStr = "/*!< Known IPC error defined by SDK */";
        break;
    case XQService::EUnknownError:
        errorStr = "/*!< Unknown IPC error */";
        break;
    case XQService::ERequestPending:
        errorStr = "/*!< Already pending request */";
        break;
    default:
        break;
    }
}

/*!
 Handle changes in FM Radio state.
 
 /param value New state of FM Radio.
 */
void FmRadioHsWidgetRadioServiceClient::handleFmRadioStateChange(QVariant& value)
{
    if (value.isValid()) {
        emit radioStateChanged(value);
    }
}

/*!
 Handle request completion.
 
 /param value
 */
void FmRadioHsWidgetRadioServiceClient::requestCompleted(const QVariant& value)
{
    if (value.isValid()) {
    }
}

/*!
 Handle error.
 
 /param errorCode
 /param errorMessage
 */
void FmRadioHsWidgetRadioServiceClient::handleError(int errorCode, const QString& errorMessage)
{
    int e = errorCode;
    QString em = errorMessage;
    handleRequestError(e);
}

/*!
 Creates control service request object.
 */
void FmRadioHsWidgetRadioServiceClient::createControlServiceRequest()
{
    if (!mRadioControlRequest) {
        QString fullInterfaceName = /*KRadioServiceName +"."+*/ KRadioServiceControlInterfaceName;
        mRadioControlRequest = mApplicationManager.create(fullInterfaceName,
            KRadioServiceControlOperation, false);

        if (mRadioControlRequest) {
            mRadioControlRequest->setEmbedded(false);
            mRadioControlRequest->setSynchronous(true);
            //TODO: Do backgound set through XQRequestInfo in MCL wk14.
            //mRadioControlRequest->setBackground(true);

            /*
            bool b = connect(mRadioControlRequest, SIGNAL(requestOk(const QVariant&)), this,
                SLOT(requestCompleted(const QVariant&)));
            bool t = connect(mRadioControlRequest, SIGNAL(requestError(int,const QString&)), this,
                SLOT(handleError(int,const QString&)));
            */
        }
    }
}

/*!
 Creates monitor service request object.
 */
void FmRadioHsWidgetRadioServiceClient::createMonitorServiceRequest()
{
    if (!mRadioMonitorRequest) {
		QString operation = mDataInitialized ? KRadioServiceMonitorOperation
            : KRadioServiceRefreshOperation;
        QString fullInterfaceName = /*KRadioServiceName +"."+*/ KRadioServiceMonitorInterfaceName;
        
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
            KRadioServiceMonitorOperation, false);
        */

        mRadioMonitorRequest = mApplicationManager.create(
            fullInterfaceName, operation, false);

        if (mRadioMonitorRequest) {
            connect(mRadioMonitorRequest, SIGNAL(requestOk(const QVariant&)),
                this, SLOT(handleFmRadioInformationChange(const QVariant&)));
            connect(mRadioMonitorRequest,
                SIGNAL(requestError(int,const QString&)), this,
                SLOT(handleError(int,const QString&)));

            mRadioMonitorRequest->setSynchronous(false);
            mRadioMonitorRequest->setEmbedded(false);
        }
    }
}

/*!
 Start radio monitoring.
 */
void FmRadioHsWidgetRadioServiceClient::startMonitoring(FmRadioVisibilty visibility)
{
    //FmRadioHsWidgetRadioServiceSingleton::instance(this)->requestNotifications(this);

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
    //FmRadioHsWidgetRadioServiceSingleton::instance(this)->cancelNotifications(this);

    if (mRadioMonitorRequest) {
        delete mRadioMonitorRequest;
        mRadioMonitorRequest = NULL;
    }
    mRequestPending = false;
    mDataInitialized = false;
}

/*!
 Prepares the visibility of the request.
 */
void FmRadioHsWidgetRadioServiceClient::prepareRequestInfo(XQAiwRequest *request,
    FmRadioVisibilty visibility)
{
    XQRequestInfo info;
    switch (visibility) {
    case ToForeground:
        //info.setForeground(true);
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
        bool a = request->isBackground();
    }
    //bool f = info.isForeground();
    bool b = info.isBackground();
}

void FmRadioHsWidgetRadioServiceClient::test()
{
    // Test is kept for example. We have to switch to use the XQAiwRequest and this includes workin example code. 
    //XQAiwRequest* req;
/*
    QUrl uri("application://101FF976");
    QString a = "Uri=" + uri.toString();
    if (uri.isValid()) {
        QString b = "isValid";
    }
    QString c = "Uri authority=" + uri.authority();
*/
/*
    QList<XQAiwInterfaceDescriptor> list = mApplicationManager.list(KRadioServiceName, KRadioServiceMonitorInterfaceName);
    foreach (XQAiwInterfaceDescriptor d, list) {
        QString in = d.interfaceName();
        QString sn = d.serviceName();
    }

    list = mApplicationManager.list(KRadioServiceName, KRadioServiceMonitorInterfaceName, "");
    foreach (XQAiwInterfaceDescriptor d, list) {
        QString in = d.interfaceName();
        QString sn = d.serviceName();
    }

    list = mApplicationManager.list(KRadioServiceMonitorInterfaceName, "");
    foreach (XQAiwInterfaceDescriptor d, list) {
        QString in = d.interfaceName();
        QString sn = d.serviceName();
    }
*/
    //req = mApplicationManager.create(uri, false);

    /*req = mApplicationManager.create(KRadioServiceMonitorInterfaceName, KRadioServiceMonitorOperation,
        false);*/

/* ///
    if (!mRadioMonitorRequest) {
        createMonitorServiceRequest();
    }
    bool res = mRadioMonitorRequest->send();
///
*/
    
    /*if (req) {*/
/*
        // Connect signals once
        bool a = connect(req, SIGNAL(requestOk(const QVariant&)), this,
            SLOT(handleOk(const QVariant&)));
        bool b = connect(req, SIGNAL(requestError(int,const QString&)), this,
            SLOT(handleError(int,const QString&)));
*/
        /*bool b = connect(req, SIGNAL(requestOk(const QVariant&)), this,
            SLOT(handleFmRadioInformationChange(QVariant)));
        bool c = connect(req, SIGNAL(requestError(int,const QString&)), this,
            SLOT(handleError(int,const QString&)));*/
/*
        QList<QVariant> arg;
        arg << "";
        req->setArguments(arg);
*/
/*
        QString op = req->operation();
        req->setOperation(KRadioServiceMonitorOperation);
        op = req->operation();
*/
        /*bool res = req->send();*/

        /*if (res) {
            //QTimer::singleShot(40000, this, SLOT(doSendMonitorRequest()));
        }
        else {
            // Request failed.
            int x = 2;
        }

    }
    else {
        int r;
        r = 5;
    }*/
}
