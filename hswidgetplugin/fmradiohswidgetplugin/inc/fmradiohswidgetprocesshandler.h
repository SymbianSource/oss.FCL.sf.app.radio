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

#ifndef FMRADIOHSWIDGETPROCESSHANDLER_H_
#define FMRADIOHSWIDGETPROCESSHANDLER_H_

// System includes
#include <QObject>
#include <QProcess>

// User includes

// Forward declarations
#include <QVariant>

// Constants
/**  FM Radio application executable. */
const QString KRadioExecutablePath = "/sys/bin/qtfmradio.exe"; // TODO: Remove the hard coded path and use some util to determine the path instead.
/**  Arguments for FM Radio application executable. */
const QString KRadioExecutableArguments = "";

class FmRadioHsWidgetProcessHandler : public QObject
{
Q_OBJECT

public:
    FmRadioHsWidgetProcessHandler(QObject *parent = 0);
    ~FmRadioHsWidgetProcessHandler();

public:
    void startFmRadioApplication();

signals:
    void fmRadioApplicationStateChanged(QVariant state);

public slots:
    void handleStateChange(QProcess::ProcessState state);
    void handleError(QProcess::ProcessError error);
    void handleFinish(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QProcess* mProcess;
        
};

#endif /* FMRADIOHSWIDGETPROCESSHANDLER_H_ */
