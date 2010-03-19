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
* Description:
*
*/

#ifndef RADIOXMLUILOADER_H_
#define RADIOXMLUILOADER_H_

// System includes
#include <hbdocumentloader.h>

// Constants
#ifdef USE_LAYOUT_FROM_E_DRIVE
    static const char* DOCML_TUNINGVIEW_FILE           = "e:/radiotest/layout/tuningview.docml";
    static const char* DOCML_STATIONSVIEW_FILE         = "e:/radiotest/layout/stationsview.docml";
    static const char* DOCML_WIZARDVIEW_FILE           = "e:/radiotest/layout/wizardview.docml";
    static const char* DOCML_PLAYLOGVIEW_FILE          = "e:/radiotest/layout/playlogview.docml";
#else
    static const char* DOCML_TUNINGVIEW_FILE           = ":/layout/tuningview.docml";
    static const char* DOCML_STATIONSVIEW_FILE         = ":/layout/stationsview.docml";
    static const char* DOCML_WIZARDVIEW_FILE           = ":/layout/wizardview.docml";
    static const char* DOCML_PLAYLOGVIEW_FILE          = ":/layout/playlogview.docml";
#endif

// XML UI definitions
static const char* DOCML_NAME_VIEW                 = "view";
static const char* DOCML_LAYOUT_PORTRAIT           = "portrait";
static const char* DOCML_LAYOUT_LANDSCAPE          = "landscape";
static const char* DOCML_NAME_EXITACTION           = "mExitAction";
static const char* DOCML_NAME_LOUDSPEAKERACTION    = "mLoudspeakerAction";

// RadioTuningView
static const char* DOCML_NAME_FAVORITEACTION       = "tv:FavoriteAction";
static const char* DOCML_NAME_PLAYLOGVIEWACTION    = "tv:PlayLogViewAction";
static const char* DOCML_NAME_STATIONSVIEWACTION   = "tv:StationsViewAction";
static const char* DOCML_NAME_STATIONCAROUSEL      = "tv:mStationCarousel";
static const char* DOCML_NAME_STATIONCONTROLWIDGET = "tv:mStationControlWidget";
static const char* DOCML_NAME_FREQUENCYSTRIP       = "tv:mFrequencyStrip";

// RadioStationsView
static const char* DOCML_NAME_HEADINGBANNER        = "sv:mHeadingBanner";
static const char* DOCML_NAME_STATIONSLIST         = "sv:mStationsList";
static const char* DOCML_NAME_FAVORITESBUTTON      = "sv:FavoritesButton";
static const char* DOCML_NAME_LOCALSBUTTON         = "sv:LocalStationsButton";
static const char* DOCML_NAME_TUNINGVIEWACTION     = "sv:mTuningViewAction";
static const char* DOCML_NAME_WIZARDVIEWACTION     = "sv:mWizardViewAction";
static const char* DOCML_NAME_SCANSTATIONSACTION   = "sv:mScanStationsAction";
static const char* DOCML_NAME_REMOVESTATIONSACTION = "sv:mRemoveAllStations";
static const char* DOCML_NAME_SCANBUTTON           = "sv:mScanButton";

// RadioStationsView Context Menu
static const char* DOCML_NAME_CONTEXT_MENU         = "sv:mContextMenu";
static const char* DOCML_NAME_CONTEXT_RENAME       = "sv:mRenameStationAction";
static const char* DOCML_NAME_CONTEXT_FAVORITE     = "sv:mFavoriteAction";
static const char* DOCML_NAME_CONTEXT_DELETE       = "sv:mDeleteStationAction";

// Station Info Widget
static const char* DOCML_NAME_INFO_FIRST_ROW       = "tv:InfoFirstRow";
static const char* DOCML_NAME_INFO_SECOND_ROW      = "tv:InfoSecondRow";
static const char* DOCML_NAME_INFO_THIRD_ROW       = "tv:InfoThirdRow";
static const char* DOCML_NAME_INFO_FOURTH_ROW      = "tv:InfoFourthRow";

// Station Control Widget
static const char* DOCML_NAME_STATIONSVIEWBUTTON   = "tv:mStationsViewButton";
static const char* DOCML_NAME_TAGBUTTON            = "tv:mTagButton";
static const char* DOCML_NAME_RECOGNIZEBUTTON      = "tv:mRecognizeButton";

// Wizard View
static const char* DOCML_NAME_WV_STATIONSVIEWACTION = "wv:mStationsViewAction";
static const char* DOCML_NAME_WV_HEADINGBANNER     = "wv:mHeadingBanner";
static const char* DOCML_NAME_WV_STATIONSLIST      = "wv:mStationsList";
static const char* DOCML_NAME_WV_MARKALLBUTTON     = "wv:mMarkAllButton";
static const char* DOCML_NAME_WV_UNMARKALLBUTTON   = "wv:mUnMarkAllButton";
static const char* DOCML_NAME_WV_DONEBUTTON        = "wv:mDoneButton";

// Play Log View
static const char* DOCML_NAME_PLV_TUNINGVIEWACTION      = "plv:mTuningViewAction";
static const char* DOCML_NAME_PLV_STATIONSVIEWACTION    = "plv:mStationsViewAction";
static const char* DOCML_NAME_PLV_REMOVEALLACTION       = "plv:mRemoveAllAction";
static const char* DOCML_NAME_PLAYLOGLIST               = "plv:mPlayLogList";
static const char* DOCML_NAME_ALLSONGSBUTTON            = "plv:AllSongsButton";
static const char* DOCML_NAME_FAVORITESONGSBUTTON       = "plv:FavoriteSongsButton";

// Forward declarations
class RadioMainWindow;

// Class declaration
class RadioXmlUiLoader : public HbDocumentLoader
{

public:

    RadioXmlUiLoader( RadioMainWindow& mainWindow );

    /**
     * Returns the requested widget casted to correct type
     *
     * @param name Name of the widget
     * @return Pointer to the widget
     */
    template<class T>
    T* findWidget( QString name )
    {
        return qobject_cast<T*>( HbDocumentLoader::findWidget( name ) );
    }

    /**
     * Returns the requested object casted to correct type
     *
     * @param name Name of the object
     * @return Pointer to the object
     */
    template<class T>
    T* findObject( QString name )
    {
        return qobject_cast<T*>( HbDocumentLoader::findObject( name ) );
    }

private:

    QObject *createObject( const QString& type, const QString &name );

private: // data

    /*!
     * Reference to the main window
     */
    RadioMainWindow& mMainWindow;

};

#endif // RADIOXMLUILOADER_H_
