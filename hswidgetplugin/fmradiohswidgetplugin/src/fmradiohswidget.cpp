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
#include <HbFrameDrawer>
#include <HbFrameItem>
#include <HbIcon>
#include <HbIconAnimationManager>
#include <HbIconAnimationDefinition>
#include <HbColorScheme>
#include <HbMarqueeItem>
#include <HbStyleLoader>
#include <QGraphicsLinearLayout>
#include <QGraphicsItem>
#include <QDesktopServices>

// User includes
#include "fmradiohswidget.h"
#include "fmradiohswidgetprocesshandler.h"
#include "fmradiohswidgetprofilereader.h"
#include "fmradiohswidgetradioserviceclient.h"
#include "radioservicedef.h"
#include "radio_global.h"

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
      mPlayButtonState(PlayEnabled),
      mIsFavoriteChannels(false),
      mRadioInformation(QHash<QString, QString>()),
      mProcessHandler(0),
      mProfileMonitor(new FmRadioHsWidgetProfileReader(this)),
      mRadioServiceClient(new FmRadioHsWidgetRadioServiceClient(this))
{
    connect(mRadioServiceClient, SIGNAL(radioInformationChanged(int, QVariant)), this,
        SLOT(handleRadioInformationChange(int, QVariant)));
    connect(mRadioServiceClient, SIGNAL(radioStateChanged(QVariant)), this,
        SLOT(handleRadioStateChange(QVariant)));
    connect(mProfileMonitor, SIGNAL(radioRunning(QVariant)), this,
        SLOT(handleRadioStateChange(QVariant)));
    
    load(KDocml);
    
    handleRadioStateChange(mProfileMonitor->radioStatus());

    //mRadioServiceClient->init();
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
void FmRadioHsWidget::load(const QString docml)
{
    bool loaded = false;

    HbDocumentLoader *documentLoader = new HbDocumentLoader();
    documentLoader->reset();
    documentLoader->load(docml, &loaded);

    if (loaded) {
        HbWidget *widget = qobject_cast<HbWidget*> (documentLoader->findWidget(
            KDocmlObjectNameMainLayout));

        HbFrameItem *frameItem = NULL;

        if (widget) {
            //HbWidget *view = qobject_cast<HbWidget*> (widget);
            QGraphicsLinearLayout *mWidgetLayout = new QGraphicsLinearLayout(Qt::Vertical, this);

            HbFrameDrawer *drawer = new HbFrameDrawer("qtg_fr_hswidget_normal",
                HbFrameDrawer::NinePieces);
            frameItem = new HbFrameItem(drawer, widget);
            frameItem->setPreferredSize(widget->preferredSize());

            mWidgetLayout->addItem(widget);
            setLayout(mWidgetLayout);
        }

        mRadioPushButton = qobject_cast<HbPushButton*> (documentLoader->findWidget(
            KDocmlObjectNameRadioIconPushButton));
        if (mRadioPushButton) {
            if (frameItem) {
                frameItem->stackBefore(mRadioPushButton);
            }
            mRadioPushButton->setBackground(HbIcon("qtg_large_radio"));
            mRadioPushButton->icon().setSize(mRadioPushButton->preferredSize());
            QObject::connect(mRadioPushButton, SIGNAL(clicked()), this, SLOT(radioToForeground()));
        }

        mVerticalSeparatorLabel = qobject_cast<HbLabel*> (documentLoader->findWidget(
            KDocmlObjectNameVerticalSeparatorLabel));
        if (mVerticalSeparatorLabel) {
            mVerticalSeparatorLabel->setIcon(HbIcon("qtg_graf_divider_v_thin"));
        }
        
        HbWidget *controlAreaLayoutWidget = qobject_cast<HbWidget*> (documentLoader->findWidget(
            KDocmlObjectNameControlAreaLayout));
        if (controlAreaLayoutWidget) {
        }

        mPreviousPushButton = qobject_cast<HbPushButton *> (documentLoader->findWidget(
            KDocmlObjectNamePreviousPushButton));
        if (mPreviousPushButton) {
            changeControlButtonFrameBackground(false, Left, mPreviousPushButton);
/*
            HbFrameDrawer *previousButtonFrameDrawer = new HbFrameDrawer("qtg_fr_hsbutton_disabled",
                  HbFrameDrawer::ThreePiecesHorizontal);
            previousButtonFrameDrawer->setFileNameSuffixList(QStringList() << "_l" << "_c" << "_cr");
            mPreviousPushButton->setFrameBackground(previousButtonFrameDrawer);
*/            QObject::connect(mPreviousPushButton, SIGNAL(clicked()), this, SLOT(previousChannel()));
        }

        mPlayPushButton = qobject_cast<HbPushButton *> (documentLoader->findWidget(
            KDocmlObjectNamePlayPushButton));
        if (mPlayPushButton) {
            changeControlButtonFrameBackground(false, Center, mPlayPushButton);
/*            HbFrameDrawer *playButtonFrameDrawer = new HbFrameDrawer("qtg_fr_hsbutton_disabled",
                  HbFrameDrawer::ThreePiecesHorizontal);
            playButtonFrameDrawer->setFileNameSuffixList(QStringList() << "_cl" << "_c" << "_cr");
            mPlayPushButton->setFrameBackground(playButtonFrameDrawer);
*/            QObject::connect(mPlayPushButton, SIGNAL(clicked()), this, SLOT(radioToBackground()));
        }

        mNextPushButton = qobject_cast<HbPushButton *> (documentLoader->findWidget(
            KDocmlObjectNameNextPushButton));
        if (mNextPushButton) {
            changeControlButtonFrameBackground(false, Right, mNextPushButton);
/*            HbFrameDrawer *nextButtonFrameDrawer = new HbFrameDrawer("qtg_fr_hsbutton_disabled",
                  HbFrameDrawer::ThreePiecesHorizontal);
            nextButtonFrameDrawer->setFileNameSuffixList(QStringList() << "_cl" << "_c" << "_r");
            mNextPushButton->setFrameBackground(nextButtonFrameDrawer);
*/            QObject::connect(mNextPushButton, SIGNAL(clicked()), this, SLOT(nextChannel()));
        }
        
        //bool b = QFile::exists(KCss);
        bool a = HbStyleLoader::registerFilePath(KCss);

        mInformationAreaTwoRowsLayout = qobject_cast<QGraphicsWidget *> (
            documentLoader->findObject(KDocmlObjectNameTwoRowsLayout));
        if (mInformationAreaTwoRowsLayout) {
            /*
            QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical, mInformationAreaTwoRowsLayout);
            mInformationFirstRowMarquee = new HbMarqueeItem();
            HbStyle::setItemName(mInformationFirstRowMarquee, "marquee1");
            mInformationFirstRowMarquee->setObjectName("marquee1");
            mInformationFirstRowMarquee->setText(
                "Long text");
            mInformationFirstRowMarquee->setLoopCount(-1);
            mInformationFirstRowMarquee->startAnimation();
            HbFontSpec fs(HbFontSpec::Secondary);
            mInformationFirstRowMarquee->setFontSpec(fs);
            mInformationFirstRowMarquee->setTextColor(HbColorScheme::color("qtc_hs_list_item_title"));
            mInformationFirstRowMarquee->setPreferredSize(layout->preferredSize());
            layout->addItem(mInformationFirstRowMarquee);
         
            mInformationSecondRowMarquee = new HbMarqueeItem();
            mInformationSecondRowMarquee->setObjectName("marquee2");
            mInformationSecondRowMarquee->setText(
                "Long text to test marquee, Long text to test marquee");
            mInformationSecondRowMarquee->setLoopCount(-1);
            mInformationSecondRowMarquee->startAnimation();
            mInformationSecondRowMarquee->setFontSpec(fs);
            mInformationSecondRowMarquee->setTextColor(HbColorScheme::color("qtc_hs_list_item_title"));
            mInformationSecondRowMarquee->setPreferredSize(layout->preferredSize());
            layout->addItem(mInformationSecondRowMarquee);
            */
        }

        mInformationAreaOneRowLayout = qobject_cast<QGraphicsWidget *> (documentLoader->findObject(
            KDocmlObjectNameOneRowLayout));
        if (mInformationAreaOneRowLayout) {
            /*
            QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical, mInformationAreaOneRowLayout);
            mInformationLonelyRowMarquee = new HbMarqueeItem();
            mInformationLonelyRowMarquee->setObjectName("marquee3");
            HbStyle::setItemName(mInformationLonelyRowMarquee, "marquee3");
            mInformationLonelyRowMarquee->setText(
                "Long text");
            mInformationLonelyRowMarquee->setLoopCount(-1);
            mInformationLonelyRowMarquee->startAnimation();
            HbFontSpec fs(HbFontSpec::Secondary);
            mInformationLonelyRowMarquee->setFontSpec(fs);
            mInformationLonelyRowMarquee->setTextColor(HbColorScheme::color("qtc_hs_list_item_title"));
            mInformationLonelyRowMarquee->setPreferredSize(layout->preferredSize());
            layout->addItem(mInformationLonelyRowMarquee);
            */
        }

        mInformationAreaAnimationLayout = qobject_cast<QGraphicsWidget *> (
            documentLoader->findObject(QString(KDocmlObjectNameAnimationLayout)));
        if (mInformationAreaAnimationLayout) {
        }
        
        mInformationFirstRowLabel = qobject_cast<HbLabel *> (documentLoader->findWidget(
            KDocmlObjectNameFirstRowLabel));
        if (mInformationFirstRowLabel) {
            mInformationFirstRowLabel->setTextColor(HbColorScheme::color("qtc_hs_list_item_title"));
        }

        mInformationSecondRowLabel = qobject_cast<HbLabel *> (documentLoader->findWidget(
            KDocmlObjectNameSecondRowLabel));
        if (mInformationSecondRowLabel) {
            mInformationSecondRowLabel->setTextColor(HbColorScheme::color("qtc_hs_list_item_content"));
        }

        mInformationLonelyRowLabel = qobject_cast<HbLabel *> (documentLoader->findWidget(
            KDocmlObjectNameLonelyRowLabel));
        if (mInformationLonelyRowLabel) {
            mInformationLonelyRowLabel->setTextColor(HbColorScheme::color("qtc_hs_list_item_content"));
        }

        mAnimationIcon = qobject_cast<HbLabel *> (documentLoader->findWidget(
            KDocmlObjectNameAnimationIcon));
        if (mAnimationIcon) {
            // Use animation manager to define the frame-by-frame animation.
            HbIconAnimationManager *animationManager = HbIconAnimationManager::global();

            // Create animation definition.
            HbIconAnimationDefinition animationDefinition;
            QList<HbIconAnimationDefinition::AnimationFrame> animationFrameList;

            HbIconAnimationDefinition::AnimationFrame animationFrame;
            QString animationFrameIconName;
            QString animationFrameIconNamePrefix = "qtg_anim_loading_";
            for (int i = 1; i < 11; i++) {
                animationFrame.duration = 100;
                animationFrameIconName.clear();
                animationFrameIconName.append(animationFrameIconNamePrefix);
                animationFrameIconName.append(animationFrameIconName.number(i));
                animationFrame.iconName = animationFrameIconName;
                animationFrameList.append(animationFrame);
            }
            animationDefinition.setPlayMode(HbIconAnimationDefinition::Loop);
            animationDefinition.setFrameList(animationFrameList);
            animationManager->addDefinition("animation", animationDefinition);

            // Construct an icon using the animation definition.
            HbIcon icon("animation");
            
            mAnimationIcon->setIcon(icon);
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
    //handleRadioStateChange(QVariant(NotControllingAudio));
}

/*!
 Unmute FM Radio audio.
 */
void FmRadioHsWidget::unMute()
{
    mRadioServiceClient->doControlFmRadioAudio(FmRadioHsWidgetRadioServiceClient::Unmute);
    //handleRadioStateChange(QVariant(ControllingAudio));
}

/*!
 Slot for previous button clicked.
 */
void FmRadioHsWidget::previousChannel()
{
    mRadioServiceClient->doChangeFmRadioChannel(FmRadioHsWidgetRadioServiceClient::PreviousFavouriteChannel);
}

/*!
 Slot for next button clicked.
 */
void FmRadioHsWidget::nextChannel()
{
    mRadioServiceClient->doChangeFmRadioChannel(FmRadioHsWidgetRadioServiceClient::NextFavouriteChannel);
}

/*!
 Slot for radio button clicked.
 */
void FmRadioHsWidget::radioToForeground()
{
    if (mFmRadioState == NotRunning) {
        handleRadioStateChange(QVariant(Starting));
        mRadioServiceClient->startMonitoring(FmRadioHsWidgetRadioServiceClient::ToForeground);
    }
    else {
        mRadioServiceClient->doChangeFmRadioVisibility(
            FmRadioHsWidgetRadioServiceClient::ToForeground);
    }
}

/*!
 Slot for radio button clicked.
 */
void FmRadioHsWidget::radioToBackground()
{
    if (mFmRadioState == NotRunning) {
        handleRadioStateChange(QVariant(Starting));
        mRadioServiceClient->startMonitoring(FmRadioHsWidgetRadioServiceClient::ToBackground);
    }
    else if (mFmRadioState == Starting) {
        
    }
    else {
        mRadioServiceClient->doChangeFmRadioVisibility(
            FmRadioHsWidgetRadioServiceClient::ToBackground);
    }
}

/*!
 Opening of url to the browser.
 
 /param url Url to be opened.
 */
bool FmRadioHsWidget::openUrl(QUrl url)
{
    return QDesktopServices::openUrl(url);
}

/*!
 Handles changes in FM Radio information.
 
 /param type Type of changed information.
 /param value Information content.
 */
void FmRadioHsWidget::handleRadioInformationChange(
    int notificationId, QVariant value)
{
    if (!value.isValid()) {
        return;
    }
    switch ( notificationId ) {

        case RadioServiceNotification::FavoriteCount:
            if (value.canConvert(QVariant::Int)) {
                int favoriteCount = value.toInt();
                mIsFavoriteChannels = favoriteCount > 0 ? true : false;
                changeChannelButtonsEnabledState(mIsFavoriteChannels);
            }
            break;

        case RadioServiceNotification::RadioStatus:
            if (value.canConvert(QVariant::Int)) {
                int status = value.toInt();
                switch (status) {
                case RadioStatus::Playing:
                    handleRadioStateChange(QVariant(ControllingAudio));
                    break;
                case RadioStatus::Muted:
                    handleRadioStateChange(QVariant(NotControllingAudio));
                    break;
                case RadioStatus::Seeking:
                    handleRadioStateChange(QVariant(Seeking));
                    break;
                case RadioStatus::NoAntenna:
                    handleRadioStateChange(QVariant(AntennaNotConnected));
                    break;                    
                default:
                    break;
                }
            }
            break;

        case RadioServiceNotification::Frequency:
            if (value.canConvert(QVariant::UInt)) {
                const uint frequency = value.toUInt();
                QString freqString;
                freqString.sprintf("%.1f", qreal(frequency) / KFrequencyMultiplier);
                bool frequencyCleared = false;
                if (mRadioInformation.contains(KRadioInformationFrequency)
                    && mRadioInformation[KRadioInformationFrequency].compare(freqString) != 0) {
                    clearRadioInformation();
                    frequencyCleared = true;
                }
                bool frequencyUpdated = updateRadioInformation(KRadioInformationFrequency, freqString);
                if (frequencyCleared || frequencyUpdated) {
                    radioInformationChanged();
                }
            }
        break;

        case RadioServiceNotification::Name:
            if (value.canConvert(QVariant::String)) {
                if (updateRadioInformation(KRadioInformationStationName, value.toString())) {
                    radioInformationChanged();
                }
            }
        break;

        case RadioServiceNotification::Genre:
            if (value.canConvert(QVariant::String)) {
                if (updateRadioInformation(KRadioInformationPty, value.toString())) {
                    radioInformationChanged();
                }
            }
        break;

        case RadioServiceNotification::RadioText:
            if (value.canConvert(QVariant::String)) {
                if (updateRadioInformation(KRadioInformationRt, value.toString())) {
                    radioInformationChanged();
                }
            }
        break;

        case RadioServiceNotification::HomePage:
            if (value.canConvert(QVariant::String)) {
                if (updateRadioInformation(KRadioInformationHomePage, value.toString())) {
                    radioInformationChanged();
                }
            }
        break;
        
        case RadioServiceNotification::Song:
            if (value.canConvert(QVariant::String)) {
                if (updateRadioInformation(KRadioInformationSong, value.toString())) {
                    radioInformationChanged();
                }
            }
        break;


/*    case FmRadioHsWidgetRadioServiceClient::InformationTypeCallSign:
        if (updateRadioInformation(KRadioInformationCallSign, informationText)) {

        }
        break;
*/
/*    case FmRadioHsWidgetRadioServiceClient::InformationTypeDynamicPsName:
        if (updateRadioInformation(KRadioInformationDynamicPsName,
            informationText)) {

        }
        break;
        */
        
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
bool FmRadioHsWidget::updateRadioInformation(const QString informationType, QString information)
{
    // If hash contains this type
    if (mRadioInformation.contains(informationType)) {
        // If new information is empty
        if (information.isEmpty()) {
            // Remove it from the hash
            mRadioInformation.remove(informationType);
            // Return true to indicate the change
            return true;
        }
        // If new information differs from the old one
        if (mRadioInformation[informationType].compare(information) != 0) {
            // Update the information
            mRadioInformation[informationType] = information;
            // And return true to indicate the change
            return true;
        }
    } else { // Hash do not contain the information
        // If new information is not empty
        if (!information.isEmpty()) {
            // Add it to the hash
            mRadioInformation[informationType] = information;
            // Return true to indicate the change
            return true;
        }
    }
    // Return false to indicate that nothing changed
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
        mRadioServiceClient->stopMonitoring();
        //changeControlButtonState(ChannelsDisabledPlay);
        QObject::disconnect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(unMute()));
        QObject::disconnect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(mute()));
        QObject::connect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(radioToBackground()));
        changePlayButtonState(PlayEnabled);
        mIsFavoriteChannels = false;
        changeChannelButtonsEnabledState(false);
        mInformationFirstRowLabel->setPlainText("");
        mInformationSecondRowLabel->setPlainText("");
        mInformationLonelyRowLabel->setPlainText(KFmRadioText);
        changeInformationAreaLayout(OneRow);
        break;
    case Starting:
        mFmRadioState = Starting;
        //changeControlButtonState(ChannelsDisabledStop);
        changePlayButtonState(StopDisabled);
        // TODO: What should the stop button do? Should it close the radio?
        changeChannelButtonsEnabledState(false);
        changeInformationAreaLayout(Animation);
        break;
    case Running:
        mFmRadioState = Running;
        mRadioServiceClient->startMonitoring(FmRadioHsWidgetRadioServiceClient::DoNotChange);
        //changeControlButtonState(ChannelsEnabledStop);
        QObject::disconnect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(radioToBackground()));
        QObject::disconnect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(unMute()));
        QObject::connect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(mute()));
        changePlayButtonState(StopEnabled);
        changeChannelButtonsEnabledState(mIsFavoriteChannels);
        changeInformationAreaLayout(OneRow);
        break;
    case ControllingAudio:
        mFmRadioState = ControllingAudio;
        //changeControlButtonState(ChannelsEnabledStop);
        QObject::disconnect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(radioToBackground()));
        QObject::disconnect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(unMute()));
        QObject::connect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(mute()));
        changePlayButtonState(StopEnabled);
        changeChannelButtonsEnabledState(mIsFavoriteChannels);
        radioInformationChanged();
        break;
    case NotControllingAudio:
        mFmRadioState = NotControllingAudio;
        //changeControlButtonState(ChannelsEnabledPlay);
        QObject::disconnect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(radioToBackground()));
        QObject::disconnect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(mute()));
        QObject::connect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(unMute()));
        changePlayButtonState(PlayEnabled);
        changeChannelButtonsEnabledState(mIsFavoriteChannels);
        radioInformationChanged();
        break;
    case Seeking:
        mFmRadioState = Seeking;
        //changeControlButtonState(AllDisabledStop);
        changePlayButtonState(StopDisabled);
        changeChannelButtonsEnabledState(false);
        changeInformationAreaLayout(Animation);
        break;
    case AntennaNotConnected:
        mFmRadioState = AntennaNotConnected;
        //changeControlButtonState(AllDisabledPlay);
        changePlayButtonState(StopDisabled);
        changeChannelButtonsEnabledState(false);
        mInformationFirstRowLabel->setPlainText("");
        mInformationSecondRowLabel->setPlainText("");
        mInformationLonelyRowLabel->setPlainText(KConnectHeadsetText);
        changeInformationAreaLayout(OneRow);
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
        ((QGraphicsWidget*) mInformationAreaTwoRowsLayout)->hide();
        mInformationAreaAnimationLayout->hide();
        break;
    case TwoRows:
        mInformationAreaOneRowLayout->hide();
        ((QGraphicsWidget*) mInformationAreaTwoRowsLayout)->show();
        mInformationAreaAnimationLayout->hide();
        break;
    case Animation:
        mInformationAreaOneRowLayout->hide();
        ((QGraphicsWidget*) mInformationAreaTwoRowsLayout)->hide();
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
        changeControlButtonFrameBackground(false, Left, mPreviousPushButton);
        iconName.append("qtg_mono_play");
        mPlayPushButton->setIcon(HbIcon(iconName));
        changeControlButtonFrameBackground(false, Center, mPlayPushButton);
        changeControlButtonFrameBackground(false, Right, mNextPushButton);
        break;
    case AllDisabledStop:
        changeControlButtonFrameBackground(false, Left, mPreviousPushButton);
        iconName.append("qtg_mono_stop");
        mPlayPushButton->setIcon(HbIcon(iconName));
        changeControlButtonFrameBackground(false, Center, mPlayPushButton);
        changeControlButtonFrameBackground(false, Right, mNextPushButton);
        break;
    case ChannelsDisabledPlay:
        changeControlButtonFrameBackground(false, Left, mPreviousPushButton);
        QObject::disconnect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(unMute()));
        QObject::disconnect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(mute()));
        QObject::connect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(radioToBackground()));
        iconName.append("qtg_mono_play");
        mPlayPushButton->setIcon(HbIcon(iconName));
        changeControlButtonFrameBackground(true, Center, mPlayPushButton);
        changeControlButtonFrameBackground(false, Right, mNextPushButton);
        break;
    case ChannelsDisabledStop:
        changeControlButtonFrameBackground(false, Left, mPreviousPushButton);
        iconName.append("qtg_mono_stop");
        mPlayPushButton->setIcon(HbIcon(iconName));
        changeControlButtonFrameBackground(true, Center, mPlayPushButton);
        changeControlButtonFrameBackground(false, Right, mNextPushButton);
        break;
    case ChannelsEnabledPlay:
        changeControlButtonFrameBackground(true, Left, mPreviousPushButton);
        QObject::disconnect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(radioToBackground()));
        QObject::disconnect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(mute()));
        QObject::connect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(unMute()));
        iconName.append("qtg_mono_play");
        mPlayPushButton->setIcon(HbIcon(iconName));
        changeControlButtonFrameBackground(true, Center, mPlayPushButton);
        changeControlButtonFrameBackground(true, Right, mNextPushButton);
        break;
    case ChannelsEnabledStop:
        changeControlButtonFrameBackground(true, Left, mPreviousPushButton);
        QObject::disconnect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(radioToBackground()));
        QObject::disconnect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(unMute()));
        QObject::connect(mPlayPushButton, SIGNAL(clicked()), this,
            SLOT(mute()));
        iconName.append("qtg_mono_stop");
        mPlayPushButton->setIcon(HbIcon(iconName));
        changeControlButtonFrameBackground(true, Center, mPlayPushButton);
        changeControlButtonFrameBackground(true, Right, mNextPushButton);
        break;
    default:
        break;
    }
}

/*!
 Changes enabled state of channel buttons.
 
 */
void FmRadioHsWidget::changePlayButtonState(PlayButtonState buttonState)
{
    QString iconName;
    bool enabled = false;
    switch (buttonState) {
    case PlayDisabled:
        iconName.append("qtg_mono_play");
        mPlayPushButton->setIcon(HbIcon(iconName));
        enabled = false;
        break;
    case PlayEnabled:
        iconName.append("qtg_mono_play");
        mPlayPushButton->setIcon(HbIcon(iconName));
        enabled = true;
        break;
    case StopDisabled:
        iconName.append("qtg_mono_stop");
        mPlayPushButton->setIcon(HbIcon(iconName));
        enabled = false;
        break;
    case StopEnabled:
        iconName.append("qtg_mono_stop");
        mPlayPushButton->setIcon(HbIcon(iconName));
        enabled = true;
        break;
    default:
        break;
    }
    changeControlButtonFrameBackground(enabled, Center, mPlayPushButton);
}

/*!
 Changes enabled state of channel buttons.
 
 */
void FmRadioHsWidget::changeChannelButtonsEnabledState(bool enabled)
{
    changeControlButtonFrameBackground(enabled, Left, mPreviousPushButton);
    changeControlButtonFrameBackground(enabled, Right, mNextPushButton);
}

/*!
 Changes background of control button.
 
 /param enabled Is button enabled or disabled.
 /param position Position of the control button in button group.
 /param button The button to change the background.
 */
void FmRadioHsWidget::changeControlButtonFrameBackground(bool enabled,
    ControlButtonPosition position, HbPushButton *button)
{
    QString frameGraphicsName("qtg_fr_hsbutton_");
    if (enabled) {
        frameGraphicsName.append("normal");
    } else {
        frameGraphicsName.append("disabled");
    }
    HbFrameDrawer *frameDrawer = new HbFrameDrawer(frameGraphicsName,
        HbFrameDrawer::ThreePiecesHorizontal);
    switch (position) {
    case Left:
        frameDrawer->setFileNameSuffixList(QStringList() << "_l" << "_c" << "_cr");
        break;
    case Center:
        frameDrawer->setFileNameSuffixList(QStringList() << "_cl" << "_c" << "_cr");
        break;
    case Right:
        frameDrawer->setFileNameSuffixList(QStringList() << "_cl" << "_c" << "_r");
        break;
    default:
        break;
    }
    button->setFrameBackground(frameDrawer);
    button->setEnabled(enabled);
}
