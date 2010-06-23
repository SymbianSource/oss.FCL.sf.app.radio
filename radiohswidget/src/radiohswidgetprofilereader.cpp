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
// TODO: Profile information should be accessed from QtMobility when it is
// ready.
#include <ProfileEngineSDKCRKeys.h>
#include "xqsettingsmanager.h"
#include "xqsettingskey.h"
#include "xqpublishandsubscribeutils.h"

// User includes
#include "radiohswidgetprofilereader.h"
#include "radiohswidget.h"
#include "radioservicedef.h"
#include "radiologger.h"

// Constants
/** Constant for radio running undefined status. */
const int RADIO_RUNNING_STATUS_UNDEFINED(-1);
/** Constant for Off-line profile. */
const int OFFLINE_PROFILE(5);

/*!
    \class RadioHsWidgetProfileReader
    \brief Implementation of P&S key reader and monitor.

    RadioHsWidgetProfileReader implements reader and monitor for P&S keys.
*/

// ======== MEMBER FUNCTIONS ========

/*!
    Constructs a profile reader which is a child of \a parent.
    
    Creates XQSettingsManager for monitoring and accessing the P&S keys.
    Connects to the signals of XQSettingsManager.
*/
RadioHsWidgetProfileReader::RadioHsWidgetProfileReader(
    RadioHsWidget *parent) :
    QObject(parent),
    mParent(*parent),
    mSettingsManager(new XQSettingsManager(this)),
    mRadioStatus(RADIO_RUNNING_STATUS_UNDEFINED)
{
    LOG_METHOD;
    Radio::connect(mSettingsManager, SIGNAL(itemDeleted(XQSettingsKey)), this,
        SLOT(handleDeletedItem(XQSettingsKey)));
    Radio::connect(mSettingsManager, SIGNAL(valueChanged(XQSettingsKey, QVariant)),
        this, SLOT(handleChanges(XQSettingsKey, QVariant)));
}

/*!
    Destructor 
 */
RadioHsWidgetProfileReader::~RadioHsWidgetProfileReader()
{
    LOG_METHOD;
}

/*!
    Start monitoring of radio P&S key. Read also the initial value.
 */
void RadioHsWidgetProfileReader::startMonitoringRadioRunningStatus()
{
    LOG_METHOD;
    XQSettingsKey radioRunningKey(XQSettingsKey::TargetPublishAndSubscribe,
        KRadioPSUid, KRadioStartupKey);
    // Start monitoring.
    mSettingsManager->startMonitoring(radioRunningKey);
    // Read current value.
    radioRunningStatus(
        mSettingsManager->readItemValue(radioRunningKey));
}

/*!
    Reads the current profile of the device and \returns \c true if the
    current profile is offline, \c false otherwise.
 */
bool RadioHsWidgetProfileReader::isCurrentProfileOffline()
{
    LOG_METHOD;
    XQSettingsKey profileKey(XQSettingsKey::TargetCentralRepository,
        KCRUidProfileEngine.iUid, KProEngActiveProfile);
    // Read current value.
    QVariant profile(mSettingsManager->readItemValue(profileKey));
    if (profile.canConvert(QVariant::Int) && profile.toInt() == OFFLINE_PROFILE) {
        return true;
    }
    return false;
}

/*!
    Handling of deletion of listened keys.
 
    \param key Deleted key.
 */
void RadioHsWidgetProfileReader::handleDeletedItem(const XQSettingsKey &key)
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
void RadioHsWidgetProfileReader::handleChanges(const XQSettingsKey &key,
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
        radioRunningStatus(value);
    }
}

/*
 Handling changes in profile information.
 
 \param value
 */
/*
void RadioHsWidgetProfileReader::currentProfileStatus(QVariant value)
{
    if (value.canConvert(QVariant::Int)) {
        emit profileChanged(value.toInt());
    }
}
*/

/*!
    Handling changes in radio running P&S key.
 
    \param value is int representation of time in seconds when radio was
    started.
 */
void RadioHsWidgetProfileReader::radioRunningStatus(
    const QVariant &value)
{
    LOG_METHOD;
    if (value.canConvert(QVariant::Int)) {
        mRadioStatus = value.toInt();
        // Notify the observer that radio is running.
        mParent.handleRadioStateChange(FmRadio::StateRunning);
    } else {
        mRadioStatus = RADIO_RUNNING_STATUS_UNDEFINED;
        // Notify the observer that radio is not running.
        mParent.handleRadioStateChange(FmRadio::StateNotRunning);
    }
}
