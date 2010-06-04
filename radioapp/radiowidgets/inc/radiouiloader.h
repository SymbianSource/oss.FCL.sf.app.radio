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

#ifndef RADIOUILOADER_H_
#define RADIOUILOADER_H_

// System includes
#include <HbDocumentLoader>

// Constants

namespace DOCML
{
    static QString FILE_MAINVIEW                    = ":/layout/mainview.docml";
    static QString FILE_STATIONSVIEW                = ":/layout/stationsview.docml";
    static QString FILE_HISTORYVIEW                 = ":/layout/historyview.docml";

    // Generic XML UI definitions
    static QString NAME_VIEW                        = "view";
    static QString SECTION_PORTRAIT                 = "portrait";
    static QString SECTION_LANDSCAPE                = "landscape";
    static QString NAME_LOUDSPEAKER_ACTION          = "loudspeaker_action";

    // RadioMainView
    static QString MV_NAME_HISTORYVIEW_ACTION       = "mv:historyview_action";
    static QString MV_NAME_STATION_CAROUSEL         = "mv:station_carousel";
    static QString MV_NAME_FREQUENCY_STRIP          = "mv:frequency_strip";
    static QString MV_NAME_STATIONS_BUTTON          = "mv:stations_button";
    static QString MV_NAME_SCAN_BUTTON              = "mv:scan_button";
    static QString MV_NAME_SPEAKER_BUTTON           = "mv:loudspeaker_button";
    static QString MV_SECTION_NO_ANTENNA            = "mv:no_antenna";
    static QString MV_SECTION_SEEKING               = "mv:seeking";
    static QString MV_SECTION_SCANNING              = "mv:scanning";
    static QString MV_SECTION_NO_FAVORITES          = "mv:no_favorites";
    static QString MV_SECTION_NORMAL                = "mv:normal";

    // RadioStationsView
    static QString SV_NAME_HEADING_BANNER           = "sv:heading_banner";
    static QString SV_NAME_STATIONS_LIST            = "sv:stations_list";
    static QString SV_NAME_FAVORITES_BUTTON         = "sv:favorite_stations_button";
    static QString SV_NAME_LOCALS_BUTTON            = "sv:local_stations_button";
    static QString SV_NAME_SCAN_ACTION              = "sv:scan_action";
    static QString SV_NAME_CLEAR_LIST_ACTION        = "sv:clear_list_action";
    static QString SV_NAME_SCAN_BUTTON              = "sv:scan_button";

    static QString SV_SECTION_SHOW_ALL_STATIONS     = "show_all_stations";
    static QString SV_SECTION_SHOW_FAVORITES        = "show_favorites";
    static QString SV_SECTION_SHOW_SCAN_TEXT        = "show_scan_text";
    static QString SV_SECTION_HIDE_SCAN_TEXT        = "hide_scan_text";

    // RadioStationsView Context Menu
    static QString NAME_CONTEXT_MENU                = "sv:context_menu";
    static QString NAME_CONTEXT_RENAME              = "sv:rename_action";
    static QString NAME_CONTEXT_FAVORITE            = "sv:toggle_favorite_action";
    static QString NAME_CONTEXT_DELETE              = "sv:delete_action";
    static QString NAME_INPUT_QUERY                 = "sv:station_name_query";

    // Station Carousel
    static QString MV_NAME_INFO_FIRST_ROW           = "mv:InfoFirstRow";
    static QString MV_NAME_INFO_SECOND_ROW          = "mv:InfoSecondRow";
    static QString MV_NAME_INFO_THIRD_ROW           = "mv:InfoThirdRow";
    static QString MV_NAME_INFO_FOURTH_ROW          = "mv:InfoFourthRow";
    static QString MV_NAME_INFO_TEXT                = "mv:info_text";

    // History View
    static QString HV_NAME_CLEAR_LIST_ACTION        = "hv:clear_list_action";
    static QString HV_NAME_HISTORY_LIST             = "hv:history_list";
    static QString HV_NAME_ALL_SONGS_BUTTON         = "hv:all_songs_button";
    static QString HV_NAME_TAGGED_SONGS_BUTTON      = "hv:tagged_songs_button";
    static QString HV_SECTION_SHOW_LIST             = "hv:show_list";
    static QString HV_SECTION_HIDE_LIST             = "hv:hide_list";
    static QString HV_SECTION_HISTORY_MODE          = "hv:history_mode";
    static QString HV_SECTION_FAVORITE_MODE         = "hv:tagged_mode";
    static QString HV_NAME_CONTEXT_MENU             = "hv:context_menu";

    // History View Context Menu
    static QString HV_NAME_CONTEXT_TAG              = "hv:toggle_tag_action";
    static QString HV_NAME_CONTEXT_SEARCH           = "hv:search_from_other_store_action";

}

// Class declaration
class RadioUiLoader : public HbDocumentLoader
{
public:

    RadioUiLoader();

    /*!
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

    /*!
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

};

#endif // RADIOUILOADER_H_
