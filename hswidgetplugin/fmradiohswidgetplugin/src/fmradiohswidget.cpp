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
#include <QGraphicsLinearLayout>
#include <QGraphicsItem>
#include <QDesktopServices>
#include <QTimer>

// User includes
#include "fmradiohswidget.h"
#include "fmradiohswidgetprofilereader.h"
#include "fmradiohswidgetradioserviceclient.h"
#include "radioservicedef.h"
#include "radio_global.h"
#include "radiologger.h"

/*!
    \ingroup group_fmradiohs_widget
    \class FmRadioHsWidget
    \brief Example implementation for home screen widget.

    FmRadioHsWidget implements needed functions for the FM Radio home screen
    widget.
*/

// ======== MEMBER FUNCTIONS ========

/*!
    Constructs a widget which is a child of \a parent, with widget flags set to \a flags.
    
    Constructor should be empty and all the actual construction should be
    done in onInitialize().
*/
FmRadioHsWidget::FmRadioHsWidget(QGraphicsItem* parent, Qt::WindowFlags flags)
    : HbWidget(parent, flags),
      mTunerBackgroundPushButton(NULL),
      mPowerToggleButton(NULL),
      mPreviousPushButton(NULL),
      mNextPushButton(NULL),
      mInformationFirstRowLabel(NULL),
      mInformationSecondRowLabel(NULL),
      mFmRadioState(Undefined),
      mFavoriteStations(false),
      mRadioInformation(QHash<QString, QString>()),
      mProfileMonitor(NULL),
      mRadioServiceClient(NULL),
      mRadioStartingCancelTimer(NULL)
{
    LOG_METHOD_ENTER;
}

/*!
    Destructor
*/
FmRadioHsWidget::~FmRadioHsWidget()
{
    LOG_METHOD;
}

/*!
    Getter for /r mRootPath property.
*/
QString FmRadioHsWidget::rootPath()const
{
    LOG_METHOD_ENTER;
    return mRootPath;
}

/*!
    Sets the mRoothPath propertry to /a roothPath.
*/
void FmRadioHsWidget::setRootPath(const QString &rootPath)
{
    LOG_METHOD_ENTER;
    mRootPath = rootPath;
}

/*!
    Called when widget is initialized. Constructs objects and connects them.
*/
void FmRadioHsWidget::onInitialize()
{
    LOG_METHOD_ENTER;
    mProfileMonitor = new FmRadioHsWidgetProfileReader(this);
    mRadioServiceClient = new FmRadioHsWidgetRadioServiceClient(this);
    mRadioStartingCancelTimer = new QTimer(this);
    
    connect(mRadioServiceClient, SIGNAL(radioInformationChanged(int, QVariant)), this,
        SLOT(handleRadioInformationChange(int, QVariant)));
    connect(mRadioServiceClient, SIGNAL(radioStateChanged(QVariant)), this,
        SLOT(handleRadioStateChange(QVariant)));
    connect(mProfileMonitor, SIGNAL(radioRunning(QVariant)), this,
        SLOT(handleRadioStateChange(QVariant)));

    load(KDocml);
    
    mProfileMonitor->startMonitoringRadioRunningStatus();
}

/*!
    Called when widget is shown in the home screen
*/
void FmRadioHsWidget::onShow()
{
    LOG_METHOD_ENTER;
}

/*!
    Called when widget is hidden from the home screen
*/
void FmRadioHsWidget::onHide()
{
    LOG_METHOD_ENTER;
}

/*!
    Loads docml files.
*/
void FmRadioHsWidget::load(const QString &docml)
{
    LOG_METHOD_ENTER;

    HbDocumentLoader *documentLoader = new HbDocumentLoader();
    documentLoader->reset();
    bool loaded = false;
    documentLoader->load(docml, &loaded);

    if (loaded) {
        // Find mainLayout
        HbWidget *mainLayout = qobject_cast<HbWidget*> (documentLoader->findWidget(
            KDocmlObjectNameMainLayout));
        // For drawing frame backgrounds
        HbFrameItem *frameItem = NULL;

        if (mainLayout) {
            QGraphicsLinearLayout *mWidgetLayout = new QGraphicsLinearLayout(Qt::Vertical, this);
            
            // Temporarily use graphics from resources. Change to the system
            // graphics when they are available.
            HbFrameDrawer *drawer = new HbFrameDrawer(
                ":/ui/resource/qtg_graf_hsradio_bg.png",
                HbFrameDrawer::OnePiece);
            //HbFrameDrawer *drawer = new HbFrameDrawer("qtg_fr_hswidget_normal",
            //    HbFrameDrawer::NinePieces);
            frameItem = new HbFrameItem(drawer, mainLayout);
            frameItem->setPreferredSize(mainLayout->preferredSize());

            mWidgetLayout->addItem(mainLayout);
            setLayout(mWidgetLayout);
        }
        
        // Find stacked layout for tuner area.
        HbWidget *tunerStackedLayout = qobject_cast<HbWidget*> (documentLoader->findWidget(
            KDocmlObjectNameTunerStackedLayout));
        if (tunerStackedLayout) {

            // Find stacked layout for information area.
            HbWidget *tunerInformationStackedLayout = qobject_cast<HbWidget*> (documentLoader->findWidget(
                KDocmlObjectNameTunerInformationStackedLayout));
            if (tunerInformationStackedLayout) {

                // MarqueeItem tests.
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
                /*
                }
                */

                // Find lonely label
                mInformationLonelyRowLabel = qobject_cast<HbLabel *> (documentLoader->findWidget(
                    KDocmlObjectNameLonelyRowLabel));
                if (mInformationLonelyRowLabel) {
                    QColor color = HbColorScheme::color("qtc_radio_tuner_normal");
                    if (color.isValid()) {
                        mInformationLonelyRowLabel->setTextColor(color);
                    }
                }

                // Find layout for two rows
                mInformationAreaTwoRowsLayout = qobject_cast<QGraphicsWidget *> (
                    documentLoader->findObject(KDocmlObjectNameTwoRowsLayout));
                if (mInformationAreaTwoRowsLayout) {
                    // Find first row
                    mInformationFirstRowLabel = qobject_cast<HbLabel *> (documentLoader->findWidget(
                        KDocmlObjectNameFirstRowLabel));
                    if (mInformationFirstRowLabel) {
                        QColor color = HbColorScheme::color("qtc_radio_tuner_normal");
                        if (color.isValid()) {
                            mInformationFirstRowLabel->setTextColor(color);
                        }
                    }

                    // Find second row
                    mInformationSecondRowLabel = qobject_cast<HbLabel *> (documentLoader->findWidget(
                        KDocmlObjectNameSecondRowLabel));
                    if (mInformationSecondRowLabel) {
                        QColor color = HbColorScheme::color("qtc_radio_tuner_normal");
                        if (color.isValid()) {
                            mInformationSecondRowLabel->setTextColor(color);
                        }
                    }

                }


                mAnimationIcon = qobject_cast<HbLabel *> (documentLoader->findWidget(
                    KDocmlObjectNameAnimationIcon));
                if (mAnimationIcon) {
                    // Use animation manager to define the frame-by-frame animation.
                    HbIconAnimationManager *animationManager = HbIconAnimationManager::global();

                    // Create animation definition.
                    HbIconAnimationDefinition animationDefinition;
                    QList<HbIconAnimationDefinition::AnimationFrame> animationFrameList;

                    // This should be probably done by loading axml instead.
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

            // Find push button for tuner area.
            mTunerBackgroundPushButton = qobject_cast<HbPushButton*> (documentLoader->findWidget(
                KDocmlObjectNameTunerBackgroundPushButton));
            if (mTunerBackgroundPushButton) {
                // Test different states.
                //bool p = mTunerBackgroundPushButton->setProperty("state", "normal");
                // Try css for controlling the appearance.
                //bool b = QFile::exists(KCss);
                //bool cssLoaded = HbStyleLoader::registerFilePath(KCss);
                //HbStyle::setItemName(mTunerBackgroundPushButton, KDocmlObjectNameTunerBackgroundPushButton);
                // Use the frame background.
                HbFrameDrawer *tunerBackgroundButtonFrameDrawer = new HbFrameDrawer(
                    "qtg_fr_tuner", HbFrameDrawer::ThreePiecesHorizontal);
                tunerBackgroundButtonFrameDrawer->setFillWholeRect(true);
                mTunerBackgroundPushButton->setFrameBackground(tunerBackgroundButtonFrameDrawer);
                
                // Connect the button's clicked signal. 
                connect(mTunerBackgroundPushButton, SIGNAL(clicked()), this, SLOT(radioToForeground()));
            }

        }

        // Find layout for control buttons.
        HbWidget *controlButtonsLayout = qobject_cast<HbWidget*> (documentLoader->findWidget(
            KDocmlObjectNameContolButtonsLayout));
        if (controlButtonsLayout) {
            if (frameItem) {
                // Stack widget's background behind it.
                frameItem->stackBefore(controlButtonsLayout);
            }
            
            // Find power button.
            mPowerToggleButton = qobject_cast<HbPushButton *> (documentLoader->findWidget(
                KDocmlObjectNameowerToggleButton));
            if (mPowerToggleButton) {
                // If power button is lathced type, use this.
                //mPowerToggleButton->setCheckable(true);
                changeControlButtonFrameBackground(false, Left, mPowerToggleButton);
                
                // Use the graphics from resources until release contains proper graphics. 
                mPowerToggleButton->setIcon(HbIcon(":/ui/resource/mono_power.png"));
                // Connect the button's clicked signal.
                connect(mPowerToggleButton, SIGNAL(clicked()), this, SLOT(radioToBackground()));
            }
            
            // Find previous button.
            mPreviousPushButton = qobject_cast<HbPushButton *> (documentLoader->findWidget(
                KDocmlObjectNamePreviousPushButton));
            if (mPreviousPushButton) {
                changeControlButtonFrameBackground(false, Center, mPreviousPushButton);
                // Connect the button's clicked signal.
                connect(mPreviousPushButton, SIGNAL(clicked()), this, SLOT(previousChannel()));
            }

            // Find next button.
            mNextPushButton = qobject_cast<HbPushButton *> (documentLoader->findWidget(
                KDocmlObjectNameNextPushButton));
            if (mNextPushButton) {
                changeControlButtonFrameBackground(false, Right, mNextPushButton);
                // Connect the button's clicked signal.
                connect(mNextPushButton, SIGNAL(clicked()), this, SLOT(nextChannel()));
            }
        }

    }
    // Loader is not needed anymore so it is deleted.
    delete documentLoader;
}

/*!
 Slot for closing FM Radio application from power button.
 */
void FmRadioHsWidget::closeRadio()
{
    LOG_SLOT_CALLER;
    mRadioServiceClient->doCloseFmRadio();
}

/*!
 Slot for previous button clicked.
 */
void FmRadioHsWidget::previousChannel()
{
    LOG_SLOT_CALLER;
    clearRadioInformation();
    mRadioServiceClient->doChangeFmRadioStation(FmRadioHsWidgetRadioServiceClient::PreviousFavouriteStation);
}

/*!
 Slot for next button clicked.
 */
void FmRadioHsWidget::nextChannel()
{
    LOG_SLOT_CALLER;
    clearRadioInformation();
    mRadioServiceClient->doChangeFmRadioStation(FmRadioHsWidgetRadioServiceClient::NextFavouriteStation);
}

/*!
 Slot for bringing the radio application to foreground.
 */
void FmRadioHsWidget::radioToForeground()
{
    LOG_SLOT_CALLER;
    // If radio is not running start it to foreground by monitor request.
    if (mFmRadioState == NotRunning) {
        handleRadioStateChange(QVariant(Starting));
        //mRadioServiceClient->doPowerOnFmRadio(FmRadioHsWidgetRadioServiceClient::ToForeground);
        mRadioServiceClient->startMonitoring(FmRadioHsWidgetRadioServiceClient::ToForeground);
    }
    else {
        if (mFmRadioState == Closing) {
            // Radio is closing but user wants to power it up again.
            mRadioServiceClient->doPowerOnFmRadio();
            mRadioServiceClient->stopMonitoring();
            mRadioServiceClient->startMonitoring(FmRadioHsWidgetRadioServiceClient::ToBackground);
            handleRadioStateChange(QVariant(Running));
        }
        // If radio is running, bring it to the foreground.
        mRadioServiceClient->doChangeFmRadioVisibility(
            FmRadioHsWidgetRadioServiceClient::ToForeground);
    }
}

/*!
 Slot for putting the radio application to the background.
 */
void FmRadioHsWidget::radioToBackground()
{
    LOG_SLOT_CALLER;
    // If radio is not running start it to background by monitor request.
    if (mFmRadioState == NotRunning) {
        handleRadioStateChange(QVariant(Starting));
        mRadioServiceClient->startMonitoring(FmRadioHsWidgetRadioServiceClient::ToBackground);
    }
    else if (mFmRadioState == Starting) {
        // Do nothing if radio is starting.
    }
    else if (mFmRadioState == Closing) {
        // Radio is closing but user wants to power it up again.
        mRadioServiceClient->doPowerOnFmRadio();
        mRadioServiceClient->stopMonitoring();
        mRadioServiceClient->startMonitoring(FmRadioHsWidgetRadioServiceClient::ToBackground);
        handleRadioStateChange(QVariant(Running));
    }
    else {
        // If radio is running, put it to the background.
        // This is little bit useless because the radio is in background if
        // user is able to click the widget.
        mRadioServiceClient->doChangeFmRadioVisibility(
            FmRadioHsWidgetRadioServiceClient::ToBackground);
    }
}

/*!
 Handles changes in FM Radio information.

 /param type Type of changed information.
 /param value Information content.
 */
void FmRadioHsWidget::handleRadioInformationChange(const int notificationId,
    const QVariant &value)
{
    LOG_METHOD;
    LEVEL2(LOG_SLOT_CALLER);
    if (!value.isValid()) {
        // Value is not valid so return.
        return;
    }
    switch ( notificationId ) {

        case RadioServiceNotification::FavoriteCount:
            LEVEL2(LOG("FavoriteCount"));
            if (value.canConvert(QVariant::Int)) {
                int favoriteCount = value.toInt();
                // If there are favorite stations, enable the next/previous
                // buttons.
                mFavoriteStations = favoriteCount > 0 ? true : false;
                LEVEL2(LOG_FORMAT("favoriteCount: %d, mFavoriteStations: %d",
                    favoriteCount, mFavoriteStations));
            changeStationButtonsEnabledState(mFavoriteStations);
            }
            break;
            
        case RadioServiceNotification::CurrentIsFavorite:
            LEVEL2(LOG("CurrentIsFavorite"));
            if (value.canConvert(QVariant::Bool)) {
                bool currentIsFavorite = value.toBool();
                LEVEL2(LOG_FORMAT("currentIsFavorite: %d", currentIsFavorite));
                // If favorite count is 1 and current station is favorite
                // disable next/prev buttons.
                if (currentIsFavorite) {
                    changeStationButtonsEnabledState(false);
                } else {
                    // Else eneble them.
                    changeStationButtonsEnabledState(true);
                }
            }
            break;

        case RadioServiceNotification::RadioStatus:
            LEVEL2(LOG("RadioStatus"));
            if (value.canConvert(QVariant::Int)) {
                int status = value.toInt();
                switch (status) {
                case RadioStatus::Playing:
                    LEVEL2(LOG("Playing"));
                    handleRadioStateChange(QVariant(ControllingAudio));
                    break;
                case RadioStatus::Muted:
                    LEVEL2(LOG("Muted"));
                    handleRadioStateChange(QVariant(NotControllingAudio));
                    break;
                case RadioStatus::Seeking:
                    LEVEL2(LOG("Seeking"));
                    handleRadioStateChange(QVariant(Seeking));
                    break;
                case RadioStatus::NoAntenna:
                    LEVEL2(LOG("NoAntenna"));
                    handleRadioStateChange(QVariant(AntennaNotConnected));
                    break;                    
                case RadioStatus::PoweringOff:
                    LEVEL2(LOG("PoweringOff"));
                    handleRadioStateChange(QVariant(Closing));
                    break;
                default:
                    LEVEL2(LOG("default"));
                    break;
                }
            }
            break;

        case RadioServiceNotification::Frequency:
            LEVEL2(LOG("Frequency"));
            if (value.canConvert(QVariant::UInt)) {
                const uint frequency = value.toUInt();
                QString frequencyString;
                // Format the frequency to human readable text.
                frequencyString.sprintf("%.1f", qreal(frequency) / FREQUENCY_MULTIPLIER);
                LEVEL2(LOG_FORMAT("frequency: %s", GETSTRING(frequencyString)));
                // TODO: Remove comment when localisation is working on device.
                //frequencyString = hbTrId("txt_rad_list_l1_mhz").arg(freqString);
                bool frequencyCleared = false;
                // If widget has some frequency information and new frequency
                // differs from that
                if (mRadioInformation.contains(KRadioInformationFrequency)
                    && mRadioInformation[KRadioInformationFrequency].compare(frequencyString) != 0) {
                    // Clear all infromation from widget because station has changed.
                    clearRadioInformation();
                    frequencyCleared = true;
                }
                // If widget do not have any frquency information, update it.
                bool frequencyUpdated = updateRadioInformation(KRadioInformationFrequency, frequencyString);
                if (frequencyCleared || frequencyUpdated) {
                    // Information changed, update the UI.
                    radioInformationChanged();
                }
            }
        break;

        case RadioServiceNotification::Name:
            LEVEL2(LOG("Name"));
            if (value.canConvert(QVariant::String)) {
                if (updateRadioInformation(KRadioInformationStationName, value.toString())) {
                    LEVEL2(LOG_FORMAT("name: %s", GETSTRING(value.toString())));
                    radioInformationChanged();
                }
            }
        break;

        case RadioServiceNotification::Genre:
            LEVEL2(LOG("Genre"));
            if (value.canConvert(QVariant::String)) {
                if (updateRadioInformation(KRadioInformationPty, value.toString())) {
                    LEVEL2(LOG_FORMAT("genre: %s", GETSTRING(value.toString())));
                    radioInformationChanged();
                }
            }
        break;

        case RadioServiceNotification::RadioText:
            LEVEL2(LOG("RadioText"));
            if (value.canConvert(QVariant::String)) {
                if (updateRadioInformation(KRadioInformationRt, value.toString())) {
                    LEVEL2(LOG_FORMAT("radio text: %s", GETSTRING(value.toString())));
                    radioInformationChanged();
                }
            }
        break;
        
        case RadioServiceNotification::DynamicPS:
            LOG("DynamicPS");
            if (value.canConvert(QVariant::String)) {
                if (updateRadioInformation(KRadioInformationDynamicPsName, value.toString())) {
                    LEVEL2(LOG_FORMAT("dynamicPS: %s", GETSTRING(value.toString())));
                    radioInformationChanged();
                }
            }
        break;

    default:
        LOG("default");
        break;
    }
}

/*!
 Check if the the radio information is changed. If it is changed update it.

 /param informationType Type of the information.
 /param information  Information text.

 /return bool If information is updated, return true. Return false otherwise.
 */
bool FmRadioHsWidget::updateRadioInformation(const QString &informationType,
    const QString &information)
{
    LOG_METHOD_RET("%d");
    // If hash contains this type of information.
    if (mRadioInformation.contains(informationType)) {
        // If new information is empty.
        if (information.isEmpty()) {
            // Remove old information from the hash.
            LEVEL2(LOG_FORMAT("informationType: %s removed", GETSTRING(informationType)));
            mRadioInformation.remove(informationType);
            // Return true to indicate the change.
            return true;
        }
        // If new information differs from the old one.
        if (mRadioInformation[informationType].compare(information) != 0) {
            // Update the information.
            LEVEL2(LOG_FORMAT("informationType: %s = %s", GETSTRING(informationType), GETSTRING(information)));
            mRadioInformation[informationType] = information;
            // And return true to indicate the change.
            return true;
        }
    } else { // Hash do not contain this type of information.
        // If new information is not empty.
        if (!information.isEmpty()) {
            // Add it to the hash.
            LEVEL2(LOG_FORMAT("informationType: %s = %s", GETSTRING(informationType), GETSTRING(information)));
            mRadioInformation[informationType] = information;
            // Return true to indicate the change.
            return true;
        }
    }
    // Return false to indicate that nothing changed.
    return false;
}

/*!
 Formatting radio information texts after a change.
 */
void FmRadioHsWidget::radioInformationChanged()
{
    LOG_METHOD_ENTER;
    // Clear the rows.
    mRadioInformationFirstRow.clear();
    mRadioInformationSecondRow.clear();

    // First row contains station name.
    if (mRadioInformation.contains(KRadioInformationStationName)) {
        mRadioInformationFirstRow.append(mRadioInformation.value(KRadioInformationStationName));
    } else if (mRadioInformation.contains(KRadioInformationFrequency)) {
        // Or frequency.
        mRadioInformationFirstRow.append(mRadioInformation.value(KRadioInformationFrequency));
    }
    LEVEL2(LOG_FORMAT("mRadioInformationFirstRow: %s", GETSTRING(mRadioInformationFirstRow)));
    
    // Second row of information contains radio text.
    if (mRadioInformation.contains(KRadioInformationRt)) {
        mRadioInformationSecondRow.append(mRadioInformation.value(KRadioInformationRt));
    } else if (mRadioInformation.contains(KRadioInformationDynamicPsName)) {
        // Or Dynamic PS name.
        mRadioInformationSecondRow.append(mRadioInformation.value(
            KRadioInformationDynamicPsName));
    } else if (mRadioInformation.contains(KRadioInformationPty)) {
        // Or PTY.
        mRadioInformationSecondRow.append(mRadioInformation.value(KRadioInformationPty));
    }
    LEVEL2(LOG_FORMAT("mRadioInformationSecondRow: %s", GETSTRING(mRadioInformationSecondRow)));
    
    // If second row is empty.
    if (mRadioInformationSecondRow.isEmpty()) {
        // Show only the lonely row.
        mInformationLonelyRowLabel->setPlainText(mRadioInformationFirstRow);
        changeInformationAreaLayout(OneRow);
    }
    else {
        // Else display both rows.
        mInformationFirstRowLabel->setPlainText(mRadioInformationFirstRow);
        mInformationSecondRowLabel->setPlainText(mRadioInformationSecondRow);
        changeInformationAreaLayout(TwoRows);
    }
}

/*!
 Clears the radio station information. For example, when the station is
 changed, old information should be cleared.
 */
void FmRadioHsWidget::clearRadioInformation()
{
    LOG_METHOD_ENTER;
    if (!mRadioInformation.isEmpty()) {
        LEVEL2(LOG("clear radioInformation"));
        mRadioInformation.clear();
    }
}

/*!
 Handles changes in FM Radio state.

 /param value New state of the radio application.
 */
void FmRadioHsWidget::handleRadioStateChange(const QVariant &value)
{
    LOG_METHOD;
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
        LEVEL2(LOG("Undefined"));
        // Something went wrong. Widget should not be in this state after onInitialize().
        mFmRadioState = Undefined;
        break;
    case NotRunning:
        LEVEL2(LOG("NotRunning"));
        mFmRadioState = NotRunning;
        mRadioServiceClient->stopMonitoring();
        changePowerButtonOn(false);
        mFavoriteStations = false;
        changeStationButtonsEnabledState(false);
        clearRadioInformation();
        mInformationFirstRowLabel->setPlainText("");
        mInformationSecondRowLabel->setPlainText("");
        mInformationLonelyRowLabel->setPlainText(hbTrId("txt_rad_list_fm_radio"));
        changeInformationAreaLayout(OneRow);
        break;
    case Starting:
        LEVEL2(LOG("Starting"));
        mFmRadioState = Starting;
        changePowerButtonOn(true);
        changeStationButtonsEnabledState(false);
        changeInformationAreaLayout(Animation);
        // This timer is workaround to recover from situation where radio is
        // started from widget but user answers no to the offline start dialog.
        // Stop timer if it is running.
        stopRadioStartingCancelTimer();
        // Set timer as single shot.
        mRadioStartingCancelTimer->setSingleShot(true);
        // Connect timeout.
        connect(mRadioStartingCancelTimer, SIGNAL(timeout()), this,
            SLOT(cancelRadioStartingState()));
        // Start to timeout after delay.
        mRadioStartingCancelTimer->start(KRadioStartingStateCancelDelay);
        break;
    case Running:
        LEVEL2(LOG("Running"));
        mFmRadioState = Running;
        // Stop timer if it is running because radio is now running.
        stopRadioStartingCancelTimer();
        mRadioServiceClient->startMonitoring(
            FmRadioHsWidgetRadioServiceClient::DoNotChange);
        changePowerButtonOn(true);
        changeStationButtonsEnabledState(mFavoriteStations);
        changeInformationAreaLayout(OneRow);
        break;
    case ControllingAudio:
        LEVEL2(LOG("ControllingAudio"));
        mFmRadioState = ControllingAudio;
        changeStationButtonsEnabledState(mFavoriteStations);
        radioInformationChanged();
        break;
    case NotControllingAudio:
        LEVEL2(LOG("NotControllingAudio"));
        mFmRadioState = NotControllingAudio;
        changeStationButtonsEnabledState(mFavoriteStations);
        radioInformationChanged();
        break;
    case Seeking:
        LEVEL2(LOG("Seeking"));
        mFmRadioState = Seeking;
        changeStationButtonsEnabledState(false);
        changeInformationAreaLayout(Animation);
        break;
    case AntennaNotConnected:
        LEVEL2(LOG("AntennaNotConnected"));
        mFmRadioState = AntennaNotConnected;
        changeStationButtonsEnabledState(false);
        mInformationFirstRowLabel->setPlainText("");
        mInformationSecondRowLabel->setPlainText("");
        mInformationLonelyRowLabel->setPlainText(hbTrId("txt_rad_info_connect_wired_headset"));
        changeInformationAreaLayout(OneRow);
        break;
    case Closing:
        LEVEL2(LOG("Closing"));
        mFmRadioState = Closing;
        changePowerButtonOn(false);
        changeStationButtonsEnabledState(false);
        clearRadioInformation();
        mInformationFirstRowLabel->setPlainText("");
        mInformationSecondRowLabel->setPlainText("");
        mInformationLonelyRowLabel->setPlainText(hbTrId("txt_rad_list_fm_radio"));
        changeInformationAreaLayout(OneRow);
        break;
    default:
        LOG("default");
        break;
    }
}

/*!
 Changes visible widgets of information area stacked layout.

 /param InformationAreaLayout The layout to switch visible.
 */
void FmRadioHsWidget::changeInformationAreaLayout(const InformationAreaLayout layout)
{
    LOG_METHOD;
    switch (layout) {
    case OneRow:
        LEVEL2(LOG("OneRow"));
        mInformationLonelyRowLabel->show();
        mInformationAreaTwoRowsLayout->hide();
        mAnimationIcon->hide();
        break;
    case TwoRows:
        LEVEL2(LOG("TwoRows"));
        mInformationLonelyRowLabel->hide();
        mInformationAreaTwoRowsLayout->show();
        mAnimationIcon->hide();
        break;
    case Animation:
        LEVEL2(LOG("Animation"));
        mInformationLonelyRowLabel->hide();
        mInformationAreaTwoRowsLayout->hide();
        mAnimationIcon->show();
        break;
    default:
        LOG("default");
        break;
    }
}

/*!
 Changes state of power button.

 */
void FmRadioHsWidget::changePowerButtonOn(const bool isPowerOn)
{
    LOG_METHOD;
    QString iconName;
    if (isPowerOn) {
        LEVEL2(LOG("Power on"));
        // Change icon to reflect power on state.
        iconName.append("qtg_mono_power");
        mPowerToggleButton->setIcon(HbIcon(":/ui/resource/mono_power.png"));
        mPowerToggleButton->setText("Off");
        // Connect clicked to closeRadio slot.
        disconnect(mPowerToggleButton, SIGNAL(clicked()), this,
            SLOT(radioToBackground()));
        connect(mPowerToggleButton, SIGNAL(clicked()), this,
            SLOT(closeRadio()));
    } else {
        LEVEL2(LOG("Power off"));
        // Change icon to reflect power off state.
        iconName.append("qtg_mono_power");
        mPowerToggleButton->setIcon(HbIcon(":/ui/resource/mono_power.png"));
        mPowerToggleButton->setText("On");
        // Connect clicked to radioToBackground slot.
        disconnect(mPowerToggleButton, SIGNAL(clicked()), this,
            SLOT(closeRadio()));
        connect(mPowerToggleButton, SIGNAL(clicked()), this,
            SLOT(radioToBackground()));
    }
    changeControlButtonFrameBackground(true, Left, mPowerToggleButton);
}

/*!
 Changes enabled state of station buttons.

 */
void FmRadioHsWidget::changeStationButtonsEnabledState(const bool enabled)
{
    LOG_METHOD_ENTER;
    changeControlButtonFrameBackground(enabled, Center, mPreviousPushButton);
    changeControlButtonFrameBackground(enabled, Right, mNextPushButton);
}

/*!
 Changes background of control button.

 /param enabled Is button enabled or disabled.
 /param position Position of the control button in button group.
 /param button The button to change the background.
 */
void FmRadioHsWidget::changeControlButtonFrameBackground(const bool enabled,
    const ControlButtonPosition position, HbPushButton *button)
{
    LOG_METHOD_ENTER;
    QString frameGraphicsName("qtg_fr_hsbutton_");
    if (enabled) {
        frameGraphicsName.append("normal");
    } else {
        frameGraphicsName.append("disabled");
    }
    LEVEL2(LOG_FORMAT("frameGraphicsName: %s", GETSTRING(frameGraphicsName)));
    HbFrameDrawer *frameDrawer = new HbFrameDrawer(frameGraphicsName,
        HbFrameDrawer::ThreePiecesHorizontal);
    switch (position) {
    case Left:
        LEVEL2(LOG("Left"));
        frameDrawer->setFileNameSuffixList(QStringList() << "_l" << "_c" << "_cr");
        break;
    case Center:
        LEVEL2(LOG("Center"));
        frameDrawer->setFileNameSuffixList(QStringList() << "_cl" << "_c" << "_cr");
        break;
    case Right:
        LEVEL2(LOG("Right"));
        frameDrawer->setFileNameSuffixList(QStringList() << "_cl" << "_c" << "_r");
        break;
    default:
        LOG("default");
        break;
    }
    button->setFrameBackground(frameDrawer);
    button->setEnabled(enabled);
}

/*!
 Radio did not start on time. Let's reset the widget's state.

 */
void FmRadioHsWidget::cancelRadioStartingState()
{
    LOG_SLOT_CALLER;
    handleRadioStateChange(QVariant(NotRunning));
}

/*!
 Stop the timer canceling radio starting state.

 */
void FmRadioHsWidget::stopRadioStartingCancelTimer()
{
    LOG_METHOD_ENTER;
    if (mRadioStartingCancelTimer->isActive()) {
         mRadioStartingCancelTimer->stop();
     }
}
