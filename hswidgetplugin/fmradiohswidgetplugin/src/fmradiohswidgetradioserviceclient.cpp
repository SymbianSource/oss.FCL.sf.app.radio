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

/*!
 Constructor
 */
FmRadioHsWidgetRadioServiceClient::FmRadioHsWidgetRadioServiceClient(QObject *parent) :
    QObject(parent),
    mRadioInformationServiceRequest(0),
    mRadioControlServiceRequest(0)
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
 Starting of FM Radio.
 
 /param startupState 
 */
void FmRadioHsWidgetRadioServiceClient::doStartFmRadio(FmRadioStartupState startupState)
{
    if (!mRadioControlServiceRequest) {
        createControlServiceRequest();
    }

    int commandId = 0;

    switch (startupState) {
    case StartForeground:
        // TODO: Include header and remove comment.
        commandId = /*RadioServiceCommand::Foreground*/ 6;
        break;
    case StartBackground:
        // TODO: Include header and remove comment.
        commandId = /*RadioServiceCommand::Background*/ 7;
        break;
    default:
        break;
    }
    
    QVariant commandArgument;
    commandArgument.setValue(commandId);
    QList<QVariant> arguments;
    arguments.append(commandArgument);
    mRadioControlServiceRequest->setArguments(arguments);

    bool res = mRadioControlServiceRequest->send();
}

/*!
 Bring FM Radio to foreground.
 
 */
void FmRadioHsWidgetRadioServiceClient::doBringFmRadioToForeground(bool toForeground)
{
    if (!mRadioControlServiceRequest) {
        createControlServiceRequest();
    }
    QVariant commandArgument;
    if (toForeground) {
        // TODO: Include header and remove comment.
        commandArgument.setValue(/*RadioServiceCommand::Foreground*/ 6);
    } else {
        // TODO: Include header and remove comment.
        commandArgument.setValue(/*RadioServiceCommand::Background*/ 7);
    }
    QList<QVariant> arguments;
    arguments.append(commandArgument);
    mRadioControlServiceRequest->setArguments(arguments);
    QList<QVariant> args = mRadioControlServiceRequest->arguments();

    bool res = mRadioControlServiceRequest->send();

    if (!res) {
        int error = mRadioControlServiceRequest->latestError();
        handleRequestError(error);
        // TODO: Handle error
    }
    stopMonitoring();
    startMonitoring();
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
    
    if (!mRadioInformationServiceRequest) {
        createMonitorServiceRequest();
    }
    bool res = mRadioInformationServiceRequest->send();

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
            //QTimer::singleShot(40000, this, SLOT(doGetFmRadioInformation()));
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

/*!
 Changing of FM Radio channel.
 
 /param command Command to execute.
 */
void FmRadioHsWidgetRadioServiceClient::doChangeFmRadioChannel(
    FmRadioChannelChangeCommand command)
{
    if (!mRadioControlServiceRequest) {
        createControlServiceRequest();    }
    int commandId;
    
    switch (command) {
    case PreviousFavouriteChannel:
        // TODO: Include header and remove comment.
        commandId = /*RadioServiceCommand::Previous*/ 2;
        break;
    case NextFavouriteChannel:
        // TODO: Include header and remove comment.
        commandId = /*RadioServiceCommand::Next*/ 3;
        break;
    default:
        break;
    }
    
    QVariant commandArgument;
    commandArgument.setValue(commandId);
    QList<QVariant> arguments;
    arguments.append(commandArgument);
    mRadioControlServiceRequest->setArguments(arguments);

    bool res = mRadioControlServiceRequest->send();

    if (!res) {
        int error = mRadioControlServiceRequest->latestError();
        handleRequestError(error);
    }
    
    stopMonitoring();
    startMonitoring();
}

/*!
 Changing of FM Radio channel.
 
 /param command Command to execute.
 */
void FmRadioHsWidgetRadioServiceClient::doControlFmRadioAudio(
    FmRadioAudioControlCommand command)
{
    if (!mRadioControlServiceRequest) {
        createControlServiceRequest();    }

    int commandId;

    switch (command) {
    case Mute:
        // TODO: Include header and remove comment.
        commandId = /*RadioServiceCommand::Pause*/ 1;
        break;
    case Unmute:
        // TODO: Include header and remove comment.
        commandId = /*RadioServiceCommand::Play*/ 0;
        break;
    default:
        break;
    }
    
    QVariant commandArgument;
    commandArgument.setValue(commandId);
    QList<QVariant> arguments;
    arguments.append(commandArgument);
    mRadioControlServiceRequest->setArguments(arguments);

    bool res = mRadioControlServiceRequest->send();
    stopMonitoring();
    startMonitoring();
}

/*!
 Start FM Radio information listening.
 */
void FmRadioHsWidgetRadioServiceClient::doGetFmRadioInformation()
{
    mRadioInformationServiceRequest->send();
}

/*!
 Handle changes in FM Radio information texts.
 
 /param value
 */
void FmRadioHsWidgetRadioServiceClient::handleFmRadioInformationChange(const QVariant& value)
{
    startMonitoring();
    if (value.isValid() && value.canConvert(QVariant::String)) {
        QString str = value.toString();
        // Extract the number from the beginning of the string.
        int i = str.indexOf(" ");
        QString notificationIdStr = str.left(i);
        bool conversionOk;
        int notificationId = notificationIdStr.toInt(&conversionOk);
        // Rest is the actual string.
        QString msg = str.mid(i);
        
        // If the type was converted ok
        if (conversionOk) {
            emit radioInformationChanged(notificationId, msg);
        }
    }
}

/*!
 Handle control command completion of FM Radio.
 
 /param value
 */
void FmRadioHsWidgetRadioServiceClient::handleFmRadioControlRequestComplete(const QVariant& value)
{
    if (value.isValid() && value.canConvert(QVariant::String)) {
        QString str = value.toString();
    }
    startMonitoring();
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
        // TODO close connection gracefully. Maybe try to esblish it again.
        errorStr = "(/*!< Error in IPC Connection */";
        break;
    case XQService::EConnectionClosed:
        // TODO close connection gracefully. Maybe try to esblish it again.
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
        stopMonitoring();
        startMonitoring();
        break;
    default:
        break;
    }
}

/*!
 Handles request error.
 
 /param int Error value.
 */
void FmRadioHsWidgetRadioServiceClient::handleRequestError2(int error)
{
    QString errorStr;
    switch (error) {
    case XQService::ENoError:
        errorStr = "No error";
        break;
    case XQService::EConnectionError:
        // TODO close connection gracefully. Maybe try to esblish it again.
        errorStr = "(/*!< Error in IPC Connection */";
        break;
    case XQService::EConnectionClosed:
        errorStr = "/*!< IPC Connection is closed */";
        stopMonitoring();
        startMonitoring();
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
        stopMonitoring();
        startMonitoring();
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
 Handle ok.
 
 /param result
 */
void FmRadioHsWidgetRadioServiceClient::handleOk(const QVariant& result)
{
    if (!result.isNull() && result.isValid() && result.toBool()) {
    }
}

/*!
 Handle error.
 
 /param errorCode
 /param errorMessage
 */
void FmRadioHsWidgetRadioServiceClient::handleError(int /*errorCode*/, const QString& /*errorMessage*/)
{
    //int e = errorCode;
    //QString em = errorMessage;
}

/*!
 Creates control service request object.
 */
void FmRadioHsWidgetRadioServiceClient::createControlServiceRequest()
{
    if (!mRadioControlServiceRequest) {
        QString fullServiceName = KRadioServiceName + "." + KRadioServiceControlInterfaceName;
        mRadioControlServiceRequest = new XQServiceRequest(fullServiceName,
            KRadioServiceControlOperation, false);
        bool a = connect(mRadioControlServiceRequest, SIGNAL(requestCompleted(QVariant)), this,
            SLOT(requestCompleted(QVariant)));
        bool b = connect(mRadioControlServiceRequest, SIGNAL(requestError(int)), this,
            SLOT(handleRequestError2(int)));
    }
}

/*!
 Creates monitor service request object.
 */
void FmRadioHsWidgetRadioServiceClient::createMonitorServiceRequest()
{
    if (!mRadioInformationServiceRequest) {
        QString fullServiceName = KRadioServiceName + "." + KRadioServiceMonitorInterfaceName;
        mRadioInformationServiceRequest = new XQServiceRequest(fullServiceName,
            KRadioServiceMonitorOperation, false);
        bool a = connect(mRadioInformationServiceRequest, SIGNAL(requestCompleted(QVariant)), this,
            SLOT(handleFmRadioInformationChange(QVariant)));
        bool b = connect(mRadioInformationServiceRequest, SIGNAL(requestError(int)), this,
            SLOT(handleRequestError(int)));
    }
}

/*!
 Start radio monitoring.
 */
void FmRadioHsWidgetRadioServiceClient::startMonitoring()
{
    if (!mRadioInformationServiceRequest) {
        createMonitorServiceRequest();
    }
    doGetFmRadioInformation();
}

/*!
 Stops radio monitoring.
 */
void FmRadioHsWidgetRadioServiceClient::stopMonitoring()
{
    if (mRadioInformationServiceRequest) {
        delete mRadioInformationServiceRequest;
        mRadioInformationServiceRequest = NULL;
    }
    if (mRadioControlServiceRequest) {
        delete mRadioControlServiceRequest;
        mRadioControlServiceRequest = NULL;
    }
}
