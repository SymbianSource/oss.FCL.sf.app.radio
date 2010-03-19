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

// System includes
#include <qobject.h>

typedef struct{ const char* source; const char* comment; } Translation;

#define TRANSLATE(string) tr( string.source, string.comment )
#define QTRANSLATE(string) QObject::tr( string.source, string.comment )

static const char* KContextApplication      = "RadioApplication";
static const char* KContextViewBase         = "RadioViewBase";
static const char* KContextModel            = "RadioStationModel";
static const char* KContextTuningView       = "RadioTuningView";
static const char* KContextStationControl   = "RadioStationControlWidget";
static const char* KContextStationsView     = "RadioStationsView";
static const char* KContextWizardView       = "RadioWizardView";
static const char* KContextPresetScanner    = "RadioFrequencyScanner";
static const char* KContextMenuBar          = "Menu";

//:
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KConnectHeadsetAntenna = QT_TRANSLATE_NOOP3( KContextTuningView, "Connect headset antenna", "txt_rad_list_connect_headset_antenna" );

//: Headset disconnected popup title
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KTitleHeadsetDisconnectedPopup = QT_TRANSLATE_NOOP3( KContextApplication, "Headset disconnected", "txt_fmradio_popup_headset_disconnected" );

//: FMRadio application name
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KApplicationName = QT_TRANSLATE_NOOP3( KContextViewBase, "FM Radio", "txt_fmradio_appname" );

//: Error message telling that radio receiver startup failed
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KErrorEngineStartFailed = QT_TRANSLATE_NOOP3( KContextApplication, "Failed to start radio receiver", "txt_fmradio_popup_start_failed" );

//: Radio frequency megahertz abbreviation
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KFrequencyMhz = QT_TRANSLATE_NOOP3( KContextModel, "%1 Mhz", "txt_fmradio_model_mhz" );

// TUNING VIEW

//: Menu item in the tuning view menu to activate the stations view
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KMenuActivateStationsView = QT_TRANSLATE_NOOP3( KContextTuningView, "Go to Stations View", "txt_fmradio_action_activate_stationsview" );

//: Text in the Station information area when seeking is ongoing
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KHeadingSeeking = QT_TRANSLATE_NOOP3( KContextTuningView, "Seeking", "txt_fmradio_heading_seeking" );

// STATIONS VIEW

//: Menu item in the stations view menu to activate Wizard view
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KMenuScanLocalStations = QT_TRANSLATE_NOOP3( KContextStationsView, "Scan local stations", "txt_fmradio_action_scan_local_stations" );

//: Menu item in the stations view menu to activate the tuning view
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KMenuActivateTuningView = QT_TRANSLATE_NOOP3( KContextStationsView, "Go to Tuning View", "txt_fmradio_action_activate_tuningview" );

//: Filter button to show only the favorite stations in the stations view
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KBtnTextFavorites = QT_TRANSLATE_NOOP3( KContextStationsView, "Favorites", "txt_fmradio_show_favorites" );

//: Filter button to show only the local stations in the stations view
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KBtnTextLocalStations = QT_TRANSLATE_NOOP3( KContextStationsView, "Local stations", "txt_fmradio_show_local_stations" );

//: Heading text to indicate that stations list is currently displaying favorite stations
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KHeadingTextFavorites = QT_TRANSLATE_NOOP3( KContextStationsView, "Favorites", "txt_fmradio_heading_favorites" );

//: Prefix of the text in a button that changes view to tuning view.
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KBtnTextNowPlaying = QT_TRANSLATE_NOOP3( KContextStationsView, "Now Playing: %1", "txt_fmradio_btn_now_playing" );

//: Text in a button that changes view to tuning view (headset disconnected case).
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KBtnTextConnectHeadsetAntenna = QT_TRANSLATE_NOOP3( KContextStationsView, "Connect headset antenna", "txt_fmradio_btn_connect_headset_antenna" );

//: Heading text to indicate that stations list is currently displaying local stations
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KHeadingTextLocalStations = QT_TRANSLATE_NOOP3( KContextStationsView, "Local stations", "txt_fmradio_heading_local_stations" );

//: Title for the progress note for scanning local stations
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KProgressTitleScanStations = QT_TRANSLATE_NOOP3( KContextPresetScanner, "Scanning local stations", "txt_fmradio_title_scan_stations" );

//: Title for the progress note for scanning local stations. Shows the amount of stations found so far
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KProgressTitleStationsFound = QT_TRANSLATE_NOOP3( KContextPresetScanner, "%1 stations found", "txt_fmradio_title_stations_found" );

// Stations view context sentive menu texts

//: Title for the "rename station" menu item in the stationsview context sensitive menu
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KMenuItemRenameStation = QT_TRANSLATE_NOOP3( KContextStationsView, "Rename station", "txt_fmradio_menu_rename_station" );

//: Title for the "remove renaming" menu item in the stationsview context sensitive menu
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KMenuItemRemoveRenaming = QT_TRANSLATE_NOOP3( KContextStationsView, "Remove renaming", "txt_fmradio_menu_remove_renaming" );

//: Title for the input query used to rename the station
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KQueryEnterStationName = QT_TRANSLATE_NOOP3( KContextStationsView, "Enter station name", "txt_fmradio_query_enter_station_name" );

//: Title for the "set as favorite" menu item in the stationsview context sensitive menu
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KMenuItemSetAsFavorite = QT_TRANSLATE_NOOP3( KContextStationsView, "Set as favorite", "txt_fmradio_menu_set_favorite" );

//: Title for the "remove favorite" menu item in the stationsview context sensitive menu
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KMenuItemRemoveFavorite = QT_TRANSLATE_NOOP3( KContextStationsView, "Remove favorite", "txt_fmradio_menu_remove_favorite" );

//: Title for the "remove station" menu item in the stationsview context sensitive menu
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KMenuItemRemoveStation = QT_TRANSLATE_NOOP3( KContextStationsView, "Remove station", "txt_fmradio_menu_remove_station" );

// WIZARD VIEW

//: Heading text to indicate that user can select favourite stations from the stations list
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KHeadingTextSelectFavorites = QT_TRANSLATE_NOOP3( KContextWizardView, "Select favourite stations", "txt_fmradio_heading_select_favorites" );

//: Button for marking all Wizard view station listbox items
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KBtnTextMarkAll = QT_TRANSLATE_NOOP3( KContextWizardView, "Mark All", "txt_fmradio_mark_all" );

//: Button for unmarking all Wizard view station listbox items
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KBtnTextUnmarkAll = QT_TRANSLATE_NOOP3( KContextWizardView, "Unmark All", "txt_fmradio_unmark_all" );

//: Confirmation query for starting in offline mode
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KQueryTextStartOffline = QT_TRANSLATE_NOOP3( KContextWizardView, "Start in offline mode?", "txt_fmradio_start_offline" );
//: Confirmation for favourite station selections in Wizard view
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KBtnTextDone = QT_TRANSLATE_NOOP3( KContextWizardView, "Done", "txt_fmradio_done" );

// STATION CONTROL WIDGET

//: Button to tag the current song in the station control widget in tuning view
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KBtnTagSong = QT_TRANSLATE_NOOP3( KContextStationControl, "Tag this song", "txt_fmradio_btn_tag_song" );

//: Button to open play history in the station control widget in tuning view
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KBtnPlayHistory = QT_TRANSLATE_NOOP3( KContextStationControl, "Play history", "txt_fmradio_btn_play_history" );

//: Button to open a web address in the station control widget in tuning view
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KBtnGotoWeb = QT_TRANSLATE_NOOP3( KContextStationControl, "Go to web address", "txt_fmradio_btn_goto_web" );

//: Button to mark current station as favorite in the station control widget in tuning view
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KBtnSetFavorite = QT_TRANSLATE_NOOP3( KContextStationControl, "Set as favorite", "txt_fmradio_btn_set_favorite" );

//: Button to remove the favorite status from the current station in the station control widget in tuning view
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KBtnRemoveFavorite = QT_TRANSLATE_NOOP3( KContextStationControl, "Remove favorite", "txt_fmradio_btn_remove_favorite" );

// Generic texts

//: Menu item in the view menu to exit the application
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KMenuExit = QT_TRANSLATE_NOOP3( KContextMenuBar, "Exit", "txt_fmradio_action_exit" );

//: Menu item to route audio to loudspeaker
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KMenuUseLoudspeaker = QT_TRANSLATE_NOOP3( KContextMenuBar, "Use loudspeaker", "txt_fmradio_menu_use_loudspeaker" );

//: Menu item to route audio to headset
//* loc-layout_id foo_dialog
//* loc-blank False
//* loc-viewid vid1
//* loc-positionid pid1
const Translation KMenuUseHeadset = QT_TRANSLATE_NOOP3( KContextMenuBar, "Use headset", "txt_fmradio_menu_use_headset" );
