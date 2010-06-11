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
* Description: FM Radio widget profile reader
*
*/

#ifndef FMRADIOHSWIDGETPROFILEREADER_H_
#define FMRADIOHSWIDGETPROFILEREADER_H_

// System includes
#include <QObject>
#include <QVariant>

// Forward declarations
class XQSettingsManager;
class XQSettingsKey;

class FmRadioHsWidgetProfileReader : public QObject
{
Q_OBJECT

public:
    explicit FmRadioHsWidgetProfileReader(QObject *parent = 0);
    virtual ~FmRadioHsWidgetProfileReader();
    
signals:
    //void profileChanged(int profile);
    void radioRunning(const QVariant &state);

public slots:
    void itemDeleted(const XQSettingsKey &key);
    void handleChanges(const XQSettingsKey &key, const QVariant &value);
    void startMonitoringRadioRunningStatus();
    
private slots:
    //void currentProfileStatus(const QVariant &value);
    void currentRadioRunningStatus(const QVariant &value);
    
private: // data
    XQSettingsManager *mSettingsManager; // For getting and P&S notifications
    int mRadioStatus;
};

#endif /* FMRADIOHSWIDGETPROFILEREADER_H_ */
