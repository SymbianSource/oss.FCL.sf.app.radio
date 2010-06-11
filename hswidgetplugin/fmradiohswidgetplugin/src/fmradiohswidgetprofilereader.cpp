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
#include <QDateTime>
#include <ProfileEngineSDKCRKeys.h>
#include "xqsettingsmanager.h"
#include "xqsettingskey.h"
#include "xqpublishandsubscribeutils.h"

// User includes
#include "fmradiohswidgetprofilereader.h"
#include "fmradiohswidget.h"
#include "radioservicedef.h"
#include "radiologger.h"

/*!
 Constructor
 */
FmRadioHsWidgetProfileReader::FmRadioHsWidgetProfileReader(QObject *parent) :
    QObject(parent),
    mSettingsManager(new XQSettingsManager(this)),
    mRadioStatus(-1)
{
    LOG_METHOD;
    connect(mSettingsManager, SIGNAL(itemDeleted(XQSettingsKey)), this,
        SLOT(itemDeleted(XQSettingsKey)));
    connect(mSettingsManager, SIGNAL(valueChanged(XQSettingsKey, QVariant)),
        this, SLOT(handleChanges(XQSettingsKey, QVariant)));
}

/*!
 Destructor 
 */
FmRadioHsWidgetProfileReader::~FmRadioHsWidgetProfileReader()
{
    LOG_METHOD;
}


/*!
 Handling of deletion of listened keys.
 
 \param key Deleted key.
 */
void FmRadioHsWidgetProfileReader::itemDeleted(const XQSettingsKey &key)
{
    LOG_METHOD;
    // Profile information will be used for offline query.
/*
    if (key.uid() == KCRUidProfileEngine.iUid && key.key()
        == KProEngActiveProfile) {
    }
*/
    if (key.uid() == KRadioPSUid && key.key() == KRadioStartupKey) {
        LOG("KRadioStartupKey deleted");
        startMonitoringRadioRunningStatus();
    }
}

/*!
 Notifications from settings manager are handled and routed to appropriate
 private slots.

 \param key Changed key.
 \param value Value of changed key.
 */
void FmRadioHsWidgetProfileReader::handleChanges(const XQSettingsKey &key,
    const QVariant& value)
{
    LOG_SLOT_CALLER;

    // Profile information will be used for offline query.
    /*
    if (key.uid() == KCRUidProfileEngine.iUid && key.key()
        == KProEngActiveProfile) {
        currentProfileStatus(value);
    }
    */
    
    if (key.uid() == KRadioPSUid && key.key()
        == KRadioStartupKey) {
        LOG("KRadioStartupKey changed");
        currentRadioRunningStatus(value);
    }
}


/*!
 Handling changes in profile information.
 
 \param value
 */
/*
void FmRadioHsWidgetProfileReader::currentProfileStatus(QVariant value)
{
    if (value.canConvert(QVariant::Int)) {
        emit profileChanged(value.toInt());
    }
}
*/

/*!
 Handling changes in radio running P&S key.
 
 \param value is int representation of time in seconds when radio was started.
 */
void FmRadioHsWidgetProfileReader::currentRadioRunningStatus(const QVariant &value)
{
    LOG_METHOD;
    if (value.isValid()) {
        if (value.canConvert(QVariant::Int)) {
            mRadioStatus = value.toInt();
            // Emit that radio is running.
            QVariant state(FmRadioHsWidget::Running);
            emit radioRunning(state);
        }
    } else {
        mRadioStatus = -1;
        // Emit that radio is not running.
        QVariant state(FmRadioHsWidget::NotRunning);
        emit radioRunning(state);
    }
}

/*!
 Start monitoring radio P&S key.
 
 */
void FmRadioHsWidgetProfileReader::startMonitoringRadioRunningStatus()
{
    LOG_METHOD;
    XQSettingsKey radioRunningKey(XQSettingsKey::TargetPublishAndSubscribe, KRadioPSUid,
        KRadioStartupKey);
    // Start monitoring.
    mSettingsManager->startMonitoring(radioRunningKey);
    // Read current value.
    currentRadioRunningStatus(mSettingsManager->readItemValue(radioRunningKey));
}
