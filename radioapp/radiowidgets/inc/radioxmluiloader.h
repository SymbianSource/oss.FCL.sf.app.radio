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
#include <HbDocumentLoader>

// Constants

namespace DOCML
{

    #ifdef USE_LAYOUT_FROM_E_DRIVE
        static const char* FILE_TUNINGVIEW              = "e:/radiotest/layout/tuningview.docml";
        static const char* FILE_STATIONSVIEW            = "e:/radiotest/layout/stationsview.docml";
        static const char* FILE_HISTORYVIEW             = "e:/radiotest/layout/historyview.docml";
    #else
        static const char* FILE_TUNINGVIEW              = ":/layout/tuningview.docml";
        static const char* FILE_STATIONSVIEW            = ":/layout/stationsview.docml";
        static const char* FILE_HISTORYVIEW             = ":/layout/historyview.docml";
    #endif

    // Generic XML UI definitions
    static const char* NAME_VIEW                        = "view";
    static const char* SECTION_PORTRAIT                 = "portrait";
    static const char* SECTION_LANDSCAPE                = "landscape";
    static const char* NAME_LOUDSPEAKER_ACTION          = "loudspeaker_action";

    // RadioTuningView
    static const char* TV_NAME_HISTORYVIEW_ACTION       = "tv:historyview_action";
    static const char* TV_NAME_STATION_CAROUSEL         = "tv:station_carousel";
    static const char* TV_NAME_FREQUENCY_STRIP          = "tv:frequency_strip";
    static const char* TV_NAME_STATIONS_BUTTON          = "tv:stations_button";
    static const char* TV_NAME_SCAN_BUTTON              = "tv:scan_button";
    static const char* TV_NAME_SPEAKER_BUTTON           = "tv:loudspeaker_button";
    static const char* TV_SECTION_NO_ANTENNA            = "tv:no_antenna";
    static const char* TV_SECTION_SEEKING               = "tv:seeking";
    static const char* TV_SECTION_SCANNING              = "tv:scanning";
    static const char* TV_SECTION_NO_FAVORITES          = "tv:no_favorites";
    static const char* TV_SECTION_NORMAL                = "tv:normal";

    // RadioStationsView
    static const char* SV_NAME_HEADING_BANNER           = "sv:heading_banner";
    static const char* SV_NAME_STATIONS_LIST            = "sv:stations_list";
    static const char* SV_NAME_FAVORITES_BUTTON         = "sv:favorite_stations_button";
    static const char* SV_NAME_LOCALS_BUTTON            = "sv:local_stations_button";
    static const char* SV_NAME_SCAN_ACTION              = "sv:scan_action";
    static const char* SV_NAME_CLEAR_LIST_ACTION        = "sv:clear_list_action";
    static const char* SV_NAME_SCAN_BUTTON              = "sv:scan_button";

    static const char* SV_SECTION_SHOW_ALL_STATIONS     = "show_all_stations";
    static const char* SV_SECTION_SHOW_FAVORITES        = "show_favorites";
    static const char* SV_SECTION_SHOW_SCAN_TEXT        = "show_scan_text";
    static const char* SV_SECTION_HIDE_SCAN_TEXT        = "hide_scan_text";

    // RadioStationsView Context Menu
    static const char* NAME_CONTEXT_MENU                = "sv:context_menu";
    static const char* NAME_CONTEXT_RENAME              = "sv:rename_action";
    static const char* NAME_CONTEXT_FAVORITE            = "sv:toggle_favorite_action";
    static const char* NAME_CONTEXT_DELETE              = "sv:delete_action";
    static const char* NAME_INPUT_QUERY                 = "sv:station_name_query";

    // Station Carousel
    static const char* NAME_INFO_FIRST_ROW              = "tv:InfoFirstRow";
    static const char* NAME_INFO_SECOND_ROW             = "tv:InfoSecondRow";
    static const char* NAME_INFO_THIRD_ROW              = "tv:InfoThirdRow";
    static const char* NAME_INFO_FOURTH_ROW             = "tv:InfoFourthRow";
    static const char* TV_NAME_INFO_TEXT                = "tv:info_text";

    // History View
    static const char* HV_NAME_CLEAR_LIST_ACTION        = "hv:clear_list_action";
    static const char* HV_NAME_HISTORY_LIST             = "hv:history_list";
    static const char* HV_NAME_ALL_SONGS_BUTTON         = "hv:all_songs_button";
    static const char* HV_NAME_TAGGED_SONGS_BUTTON      = "hv:tagged_songs_button";
    static const char* HV_SECTION_SHOW_LIST             = "show_list";
    static const char* HV_SECTION_HIDE_LIST             = "hide_list";
    static const char* HV_SECTION_HISTORY_MODE          = "history_mode";
    static const char* HV_SECTION_FAVORITE_MODE         = "favorite_mode";

}

// Class declaration
class RadioXmlUiLoader : public HbDocumentLoader
{
public:

    RadioXmlUiLoader();

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

#endif // RADIOXMLUILOADER_H_
