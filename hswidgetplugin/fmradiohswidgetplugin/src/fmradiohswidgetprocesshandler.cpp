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
* Description: FM Radio widget process handler
*
*/

// System includes

// User includes
#include "fmradiohswidgetprocesshandler.h"
#include "fmradiohswidget.h"

/*!
 Constructor
 */
FmRadioHsWidgetProcessHandler::FmRadioHsWidgetProcessHandler(QObject *parent) :
    QObject(parent), mProcess(new QProcess(this))
{
    QObject::connect(mProcess, SIGNAL(stateChanged(QProcess::ProcessState)),
        this, SLOT(handleStateChange(QProcess::ProcessState)));
    QObject::connect(mProcess, SIGNAL(error(QProcess::ProcessError error)),
        this, SLOT(handleError(QProcess::ProcessError error)));
    QObject::connect(mProcess,
        SIGNAL(finished(int exitCode, QProcess::ExitStatus exitStatus)),
        this,
        SLOT(handleFinish(int exitCode, QProcess::ExitStatus exitStatus)));
}

/*!
 Destructor 
 */
FmRadioHsWidgetProcessHandler::~FmRadioHsWidgetProcessHandler()
{
}

/*!
 Launching of FM Radio application process. 
 */
void FmRadioHsWidgetProcessHandler::startFmRadioApplication()
{
    if (mProcess->pid() == qint64(0)) {
        QString executablePath = KRadioExecutablePath;
        QStringList arguments;
        arguments << KRadioExecutableArguments;
        mProcess->start(executablePath, arguments);
    }
}

/*!
 Handles state change notifications from FM Radio application process. 
 */
void FmRadioHsWidgetProcessHandler::handleStateChange(
    QProcess::ProcessState state)
{
    switch (state) {
    case QProcess::NotRunning:
        emit fmRadioApplicationStateChanged(QVariant(FmRadioHsWidget::NotRunning));
        break;
    case QProcess::Starting:
        emit fmRadioApplicationStateChanged(QVariant(FmRadioHsWidget::Starting));
        break;
    case QProcess::Running:
        emit fmRadioApplicationStateChanged(QVariant(FmRadioHsWidget::Running));
        break;
    default:
        break;
    }
}

/*!
 Handles error notifications from FM Radio application process. 
 */
void FmRadioHsWidgetProcessHandler::handleError(
    QProcess::ProcessError error)
{
    switch (error) {
    case QProcess::FailedToStart:
        break;
    case QProcess::Crashed:
        break;
    case QProcess::Timedout:
        break;
    case QProcess::WriteError:
        break;
    case QProcess::ReadError:
        break;
    case QProcess::UnknownError:
        break;
    default:
        break;
    }
}

/*!
 Handles finished process notifications from FM Radio application process. 
 */
void FmRadioHsWidgetProcessHandler::handleFinish(int /*exitCode*/,
    QProcess::ExitStatus exitStatus)
{
    switch (exitStatus) {
    case QProcess::NormalExit:
        break;
    case QProcess::CrashExit:
        break;
    default:
        break;
    }
}
