/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  FM Radio home screen widget
*
*/

// System includes
#include <HbPushButton>
#include <HbLabel>
#include <HbDocumentLoader>
#include <QGraphicsLinearLayout>

// User includes
#include "fmradiohswidget.h"
#include "fmradiohswidgetprocesshandler.h"
#include "fmradiohswidgetprofilereader.h"
#include "fmradiohswidgetradioserviceclient.h"

/*!
    \ingroup group_fmradiohs_widget
    \class FmRadioHsWidget
    \brief Example implementation for home screen widget.

    FmRadioHsWidget derived from the HbWidget and implements 
    needed functions for the home screen widget. 
*/

// ======== MEMBER FUNCTIONS ========

/*!
    Constructs a widget which is a child of \a parent, with widget flags set to \a flags.
*/
FmRadioHsWidget::FmRadioHsWidget(QGraphicsItem* parent, Qt::WindowFlags flags)
    : HbWidget(parent, flags),
      mRadioPushButton(0),
      mPreviousPushButton(0),
      mPlayPushButton(0),
      mNextPushButton(0),
      mInformationFirstRowLabel(0),
      mInformationSecondRowLabel(0),
      mFmRadioState(Undefined),
      mRadioInformation(QHash<QString, QString>()),
      mRadioInformationFirstRow(0),
      mRadioInformationSecondRow(0),
      mProcessHandler(0),
      mRadioServiceClient(new FmRadioHsWidgetRadioServiceClient(this))
{
    connect(mRadioServiceClient, SIGNAL(radioInformationChanged(int, QVariant)), this,
        SLOT(handleRadioInformationChange(int, QVariant)));
    connect(mRadioServiceClient, SIGNAL(radioStateChanged(QVariant)), this,
        SLOT(handleRadioStateChange(QVariant)));
    
    load(DOCML_WIDGET);
    
    handleRadioStateChange(QVariant(NotRunning));
}

/*!
    Destructor
*/
FmRadioHsWidget::~FmRadioHsWidget()
{
}

/*!
    Called when widget is shown in the home screen
*/
void FmRadioHsWidget::onShow()
{
}

/*!
    Called when widget is hidden from the home screen
*/
void FmRadioHsWidget::onHide()
{
}

/*!
    Loads docml files.
*/
void FmRadioHsWidget::load(QString DOCML)
{
    bool loaded = false;

    HbDocumentLoader *documentLoader = new HbDocumentLoader();
    documentLoader->reset();
    documentLoader->load(DOCML, &loaded);

    if (loaded) {
        HbWidget *widget = qobject_cast<HbWidget*> (documentLoader->findWidget(QString(KDocmlObjectNameMainLayout)));
        if (widget) {
            //HbWidget *view = qobject_cast<HbWidget*> (widget);
            QGraphicsLinearLayout *mWidgetLayout = new QGraphicsLinearLayout(Qt::Vertical, this);
            mWidgetLayout->addItem(widget);
            setLayout(mWidgetLayout);
        }

        mRadioPushButton = qobject_cast<HbPushButton *> (documentLoader->findObject(QString(KDocmlObjectNameRadioIconPushButton)));
        if (mRadioPushButton) {
            // Invalid icon for the mRadioPushButton to make it look not like a button.
            mRadioPushButton->setBackground(HbIcon(" "));
            mRadioPushButton->setIcon(HbIcon("qtg_large_radio"));
            QObject::connect(mRadioPushButton, SIGNAL(clicked()), this, SLOT(radioToForeground()));
        }

        mPreviousPushButton = qobject_cast<HbPushButton *> (documentLoader->findObject(QString(
            KDocmlObjectNamePreviousPushButton)));
        if (mPreviousPushButton) {
            mPreviousPushButton->setIcon(HbIcon("qtg_mono_previous"));
            QObject::connect(mPreviousPushButton, SIGNAL(clicked()), this, SLOT(previousChannel()));
        }

        mPlayPushButton = qobject_cast<HbPushButton *> (documentLoader->findObject(QString(
            KDocmlObjectNamePlayPushButton)));
        if (mPlayPushButton) {
            QObject::connect(mPlayPushButton, SIGNAL(clicked()), this, SLOT(radioToBackground()));
        }

        mNextPushButton = qobject_cast<HbPushButton *> (documentLoader->findObject(QString(
            KDocmlObjectNameNextPushButton)));
        if (mNextPushButton) {
            mNextPushButton->setIcon(HbIcon("qtg_mono_next"));
            QObject::connect(mNextPushButton, SIGNAL(clicked()), this, SLOT(nextChannel()));
        }

        mInformationAreaTwoRowsLayout = qobject_cast<QGraphicsWidget *> (
            documentLoader->findObject(QString(KDocmlObjectNameTwoRowsLayout)));
        if (mInformationAreaTwoRowsLayout) {
        }

        mInformationAreaOneRowLayout = qobject_cast<QGraphicsWidget *> (documentLoader->findObject(
            QString(KDocmlObjectNameOneRowLayout)));
        if (mInformationAreaOneRowLayout) {
        }

        mInformationAreaAnimationLayout = qobject_cast<QGraphicsWidget *> (
            documentLoader->findObject(QString(KDocmlObjectNameAnimationLayout)));
        if (mInformationAreaAnimationLayout) {
        }

        mInformationFirstRowLabel = qobject_cast<HbLabel *> (documentLoader->findObject(QString(
            KDocmlObjectNameFirstRowLabel)));
        if (mInformationFirstRowLabel) {
            mInformationFirstRowLabel->setTextColor("qtc_hs_list_item_title");
        }

        mInformationSecondRowLabel = qobject_cast<HbLabel *> (documentLoader->findObject(QString(
            KDocmlObjectNameSecondRowLabel)));
        if (mInformationSecondRowLabel) {
            mInformationSecondRowLabel->setTextColor("qtc_hs_list_item_content");
        }

        mInformationLonelyRowLabel = qobject_cast<HbLabel *> (documentLoader->findObject(QString(
            KDocmlObjectNameLonelyRowLabel)));
        if (mInformationLonelyRowLabel) {
            mInformationLonelyRowLabel->setTextColor("qtc_hs_list_item_title");
        }

        mAnimatioIcon = qobject_cast<HbLabel *> (documentLoader->findObject(
            QString()));
        if (mAnimatioIcon) {
            mAnimatioIcon->setIcon(HbIcon("qtg_anim_loading_0"));
        }

    }
    delete documentLoader;
}

/*!
 Slot for grouping events.
 */
bool FmRadioHsWidget::eventFilter(QObject */*target*/, QEvent */*event*/)
 {
// This event filter is used to get events from information area layout and from it's components.
// Not yet implemented.   
/*
    if (target == mFrequencyLabel) {
        if (event->type() == QEvent::MouseButtonRelease) {
            qDebug() << "MouseButtonRelease";
            return true;
        }
        else if (event->type() == QEvent::MouseButtonPress) {
            qDebug() << "MouseButtonPress";
            return true;
        }
        else if (event->type() == QEvent::KeyPress) {
            qDebug() << "KeyPress";
            return true;
        }
        else {
            return false;
        }
    }
    else {
        // pass the event on to the parent class
        return HbWidget::eventFilter(target, event);
    }
*/
    return false;
}

/*!
 Mute FM Radio audio.
 */
void FmRadioHsWidget::mute()
{
    mRadioServiceClient->doControlFmRadioAudio(FmRadioHsWidgetRadioServiceClient::Mute);
    handleRadioStateChange(QVariant(NotControllingAudio));
}

/*!
 Unmute FM Radio audio.
 */
void FmRadioHsWidget::unMute()
{
    mRadioServiceClient->doControlFmRadioAudio(FmRadioHsWidgetRadioServiceClient::Unmute);
    handleRadioStateChange(QVariant(ControllingAudio));
}

/*!
 Slot for previous button clicked.
 */
void FmRadioHsWidget::previousChannel()
{
    mRadioServiceClient->doChangeFmRadioChannel(FmRadioHsWidgetRadioServiceClient::PreviousFavouriteChannel);
    clearRadioInformation();
}

/*!
 Slot for next button clicked.
 */
void FmRadioHsWidget::nextChannel()
{
    mRadioServiceClient->doChangeFmRadioChannel(FmRadioHsWidgetRadioServiceClient::NextFavouriteChannel);
    clearRadioInformation();
}

/*!
 Slot for radio button clicked.
 */
void FmRadioHsWidget::radioToForeground()
{
    if (mFmRadioState == NotRunning) {
        mRadioServiceClient->startMonitoring();
        handleRadioStateChange(QVariant(Running));
    }
    mRadioServiceClient->doBringFmRadioToForeground(true);
}

/*!
 Slot for radio button clicked.
 */
void FmRadioHsWidget::radioToBackground()
{
    if (mFmRadioState == NotRunning) {
        mRadioServiceClient->startMonitoring();
        handleRadioStateChange(QVariant(Running));
    }
}


/*!
 Handles changes in FM Radio information.
 
 /param type Type of changed information.
 /param value Information content.
 */
void FmRadioHsWidget::handleRadioInformationChange(
    int notificationId, QVariant value)
{
    QString informationText;
    if (value.canConvert(QVariant::String)) {
        informationText = value.toString();
    } else {
        return;
    }
    
    switch (notificationId) {
    /*
     Name = 0,
     Genre,
     RadioText,
     Homepage,
     Song
     */
    case 0: // Name
        if (updateRadioInformation(KRadioInformationStationName,
            informationText)) {
            radioInformationChanged();
        }
        break;
/*    case FmRadioHsWidgetRadioServiceClient::InformationTypeCallSign:
        if (updateRadioInformation(KRadioInformationCallSign, informationText)) {

        }
        break;
    case FmRadioHsWidgetRadioServiceClient::InformationTypeFrequency:
        if (updateRadioInformation(KRadioInformationFrequency,
            informationText)) {

        }
        break;
 */   case 2: // RadioText
        if (updateRadioInformation(KRadioInformationRt, informationText)) {
            radioInformationChanged();
        }
        break;
/*    case FmRadioHsWidgetRadioServiceClient::InformationTypeDynamicPsName:
        if (updateRadioInformation(KRadioInformationDynamicPsName,
            informationText)) {

        }
        break;
*/    case 1: // Genre
        if (updateRadioInformation(KRadioInformationPty, informationText)) {
            radioInformationChanged();
        }
        break;
    default:
        break;
    }
}

/*!
 Check if the the radio information is changed. If it is changed update it.
 
 /param informationType Type of the information. 
 /param information  Information text.
 
 /return bool If information is updated, return true. Return false otherwise.
 */
bool FmRadioHsWidget::updateRadioInformation(const QString informationType,
    QString information)
{
    if (!information.isEmpty()) {
        // Check if new information differs from the old one 
        if (!(mRadioInformation[informationType].compare(information) == 0)) {
            // Update the information
            mRadioInformation[informationType] = information;
            // Return true to indicate change
            return true;
        }
    } else {
        // Information is empty, lets remove it from the hash
        int removeCount = mRadioInformation.remove(informationType);
        // If some key or keys were removed
        if (removeCount > 0) {
            // Return true to indicate change
            return true;
        }
    }
    // Return false because nothing changed
    return false;
}

/*!
 Formatting radio information texts after change. 
 */
void FmRadioHsWidget::radioInformationChanged()
{
    mRadioInformationFirstRow = "";
    mRadioInformationSecondRow = "";

    // Lets formulate the first row, first station name
    if (mRadioInformation.contains(KRadioInformationStationName)) {
        mRadioInformationFirstRow.append(mRadioInformation.value(KRadioInformationStationName));
    }
    // Second call sign
    if (mRadioInformation.contains(KRadioInformationCallSign)) {
        mRadioInformationFirstRow.append(" " + mRadioInformation.value(KRadioInformationCallSign));
    }
    // Third frequency
    if (mRadioInformation.contains(KRadioInformationFrequency)) {
        mRadioInformationFirstRow.append(" " + mRadioInformation.value(KRadioInformationFrequency)
            + " " + KMhzText);
    }
    // Second row of information contains first rt
    if (mRadioInformation.contains(KRadioInformationRt)) {
        mRadioInformationSecondRow.append(mRadioInformation.value(KRadioInformationRt));
    }
    // Second is dynamic ps name
    if (mRadioInformation.contains(KRadioInformationDynamicPsName)) {
        mRadioInformationSecondRow.append(" " + mRadioInformation.value(
            KRadioInformationDynamicPsName));
    }
    // Third is pty 
    if (mRadioInformation.contains(KRadioInformationPty)) {
        mRadioInformationSecondRow.append(" " + mRadioInformation.value(KRadioInformationPty));
    }

    if (mRadioInformationSecondRow.isEmpty()) {
        mInformationLonelyRowLabel->setPlainText(mRadioInformationFirstRow);
        changeInformationAreaLayout(OneRow);
    }
    else {
        mInformationFirstRowLabel->setPlainText(mRadioInformationFirstRow);
        mInformationSecondRowLabel->setPlainText(mRadioInformationSecondRow);
        changeInformationAreaLayout(TwoRows);
    }
}

/*!
 Clears the radio station information. For example, when the channel is
 changed, old information should be cleared.
 */
void FmRadioHsWidget::clearRadioInformation()
{
    if (!mRadioInformation.isEmpty()) {
        mRadioInformation.clear();
    }
}

/*!
 Handles changes in FM Radio state.
 
 /param value Information content.
 */
void FmRadioHsWidget::handleRadioStateChange(QVariant value)
{
    int state;
    if (value.canConvert(QVariant::Int)) {
        state = value.toInt();
    } else {
        return;
    }
    
    if (state == mFmRadioState) {
        // State did not change, so return.
        return;
    }
    
    switch (state) {
    case Undefined:
        // TODO: Some error occured because we do not have the state information. Handle this.
        mFmRadioState = Undefined;
        break;
    case NotRunning:
        mFmRadioState = NotRunning;
        //mRadioServiceClient->stopMonitoring();
        changeControlButtonState(ChannelsDisabledPlay);
        mInformationFirstRowLabel->setPlainText(KFmRadioText);
        mInformationSecondRowLabel->setPlainText("");
        mInformationLonelyRowLabel->setPlainText(KFmRadioText);
        changeInformationAreaLayout(OneRow);
        break;
    case Starting:
        mFmRadioState = Starting;
        changeControlButtonState(ChannelsDisabledStop);
        changeInformationAreaLayout(Animation);
        break;
    case Running:
        mFmRadioState = Running;
        changeControlButtonState(ChannelsEnabledStop);
        changeInformationAreaLayout(OneRow);
        break;
    case ControllingAudio:
        // TODO: Implement
        changeControlButtonState(ChannelsEnabledStop);
        break;
    case NotControllingAudio:
        // TODO: Implement
        changeControlButtonState(ChannelsEnabledPlay);
        break;
    case AutoScanning:
        // TODO: Implement
        break;
    case ConnectType1Headset:
        // TODO: Implement
        break;
    case ConnectType2Headset:
        // TODO: Implement
        break;
    case ConnectType3Headset:
        // TODO: Implement
        break;
    default:
        break;
    }
}

/*!
 Changes visible layout of information area.
 
 /param InformationAreaLayout The layout to switch visible.
 */
void FmRadioHsWidget::changeInformationAreaLayout(InformationAreaLayout layout)
{
    switch (layout) {
    case OneRow:
        mInformationAreaOneRowLayout->show();
        mInformationAreaTwoRowsLayout->hide();
        mInformationAreaAnimationLayout->hide();
        break;
    case TwoRows:
        mInformationAreaOneRowLayout->hide();
        mInformationAreaTwoRowsLayout->show();
        mInformationAreaAnimationLayout->hide();
        break;
    case Animation:
        mInformationAreaOneRowLayout->hide();
        mInformationAreaTwoRowsLayout->hide();
        mInformationAreaAnimationLayout->show();
        break;
    default:
        break;
    }
}

/*!
 Changes state of the control buttons.
 
 /param ControlButtonState State of the control buttons.
 */
void FmRadioHsWidget::changeControlButtonState(ControlButtonState buttonState)
{
    QString iconName;
    switch (buttonState) {
    case AllDisabledPlay:
        mPreviousPushButton->setEnabled(false);
        iconName.append("qtg_mono_play");
        mPlayPushButton->setIcon(HbIcon(iconName));
        mPlayPushButton->setEnabled(false);
        mNextPushButton->setEnabled(false);
        break;
    case AllDisabledStop:
        mPreviousPushButton->setEnabled(false);
        iconName.append("qtg_mono_stop");
        mPlayPushButton->setIcon(HbIcon(iconName));
        mPlayPushButton->setEnabled(false);
        mNextPushButton->setEnabled(false);
        break;
    case ChannelsDisabledPlay:
        mPreviousPushButton->setEnabled(false);
        QObject::disconnect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(unMute()));
        QObject::disconnect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(mute()));
        QObject::connect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(radioToBackground()));
        iconName.append("qtg_mono_play");
        mPlayPushButton->setIcon(HbIcon(iconName));
        mPlayPushButton->setEnabled(true);
        mNextPushButton->setEnabled(false);
        break;
    case ChannelsDisabledStop:
        mPreviousPushButton->setEnabled(false);
        iconName.append("qtg_mono_stop");
        mPlayPushButton->setIcon(HbIcon(iconName));
        mPlayPushButton->setEnabled(true);
        mNextPushButton->setEnabled(false);
        break;
    case ChannelsEnabledPlay:
        mPreviousPushButton->setEnabled(true);
        QObject::disconnect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(radioToBackground()));
        QObject::disconnect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(mute()));
        QObject::connect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(unMute()));
        iconName.append("qtg_mono_play");
        mPlayPushButton->setIcon(HbIcon(iconName));
        mPlayPushButton->setEnabled(true);
        mNextPushButton->setEnabled(true);
        break;
    case ChannelsEnabledStop:
        mPreviousPushButton->setEnabled(true);
        QObject::disconnect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(radioToBackground()));
        QObject::disconnect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(unMute()));
        QObject::connect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(mute()));
        iconName.append("qtg_mono_stop");
        mPlayPushButton->setIcon(HbIcon(iconName));
        mPlayPushButton->setEnabled(true);
        mNextPushButton->setEnabled(true);
        break;
    default:
        break;
    }
}
