#ifndef MUSICOTHERDEFINE_H
#define MUSICOTHERDEFINE_H

/* =================================================
 * This file is part of the TTK Music Player project
 * Copyright (c) 2015 - 2017 Greedysky Studio
 * All rights reserved!
 * Redistribution and use of the source code or any derivative
 * works are strictly forbiden.
   =================================================*/

#include <QObject>

//MusicLrcMakerWidget
#define LRC_MAKER_INDEX_0           0
#define LRC_MAKER_INDEX_1           1
#define LRC_MAKER_INDEX_2           2
#define LRC_MAKER_INDEX_3           3

//MusicCloudFileManagerDialog
#define CLOUD_MANAGER_INDEX_0       0
#define CLOUD_MANAGER_INDEX_1       1

//MusicConnectMobileWidget
#define CONNECT_MOBILE_INDEX_0      0

//MusicSongsSummariziedWidget
#ifndef MUSIC_MOBILE
#define MUSIC_NORMAL_LIST           0
#define MUSIC_LOVEST_LIST           1
#define MUSIC_NETWORK_LIST          2
#define MUSIC_RECENT_LIST           3
#else
#define MUSIC_NORMAL_LIST           0
#define MUSIC_DOWNLOAD_LIST         1
#define MUSIC_RECENT_LIST           2
#define MUSIC_LOVEST_LIST           3
#define MUSIC_DOWNMV_LIST           4
#define MUSIC_MUSICRG_LIST          5
#endif

//MusicIdentifySongsWidget
#define IDENTIFY_SONGS_INDEX_0      0

//MusicLocalSongsManagerWidget
#define LOCAL_MANAGER_INDEX_0       0
#define LOCAL_MANAGER_INDEX_1       1

//MusicSoundKMicroWidget
#define SOUND_KMICRO_INDEX_0        0
#define SOUND_KMICRO_INDEX_1        1

//MusicTimerWidget
#define TIMER_MANAGER_INDEX_0       0
#define TIMER_MANAGER_INDEX_1       1
#define TIMER_MANAGER_INDEX_2       2

//MusicUserWindow
#define USER_WINDOW_INDEX_0         0
#define USER_WINDOW_INDEX_1         1

//MusicVideoPlayWidget
#define VIDEO_WINDOW_INDEX_0        0
#define VIDEO_WINDOW_INDEX_1        1

//MusicPlaylistFoundWidget
#define PLAYLIST_WINDOW_INDEX_0     0
#define PLAYLIST_WINDOW_INDEX_1     1

//MusicSettingWidget
#define SETTING_WINDOW_INDEX_0      0
#define SETTING_WINDOW_INDEX_1      1
#define SETTING_WINDOW_INDEX_2      2
#define SETTING_WINDOW_INDEX_3      3
#define SETTING_WINDOW_INDEX_4      4
#define SETTING_WINDOW_INDEX_5      5
#define SETTING_WINDOW_INDEX_6      6
#define SETTING_WINDOW_INDEX_7      7
#define SETTING_WINDOW_INDEX_8      8

//MusicSourceUpdateWidget
#define SOURCE_UPDATE_INDEX_0       0
#define SOURCE_UPDATE_INDEX_1       1

//MusicApplication
#define APP_WINDOW_INDEX_0          0
#define APP_WINDOW_INDEX_1          1
#define APP_WINDOW_INDEX_2          2
#define APP_WINDOW_INDEX_3          3
#define APP_WINDOW_INDEX_4          4
#define APP_WINDOW_INDEX_5          5
#define APP_WINDOW_INDEX_6          6
#define APP_WINDOW_INDEX_7          7
#define APP_WINDOW_INDEX_8          8


#endif // MUSICOTHERDEFINE_H
