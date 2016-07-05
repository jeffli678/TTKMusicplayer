#ifndef MUSICCONNECTTRANSFERWIDGET_H
#define MUSICCONNECTTRANSFERWIDGET_H

/* =================================================
 * This file is part of the TTK Music Player project
 * Copyright (c) 2014 - 2016 Greedysky Studio
 * All rights reserved!
 * Redistribution and use of the source code or any derivative
 * works are strictly forbiden.
   =================================================*/

#include "musicsong.h"
#include "musicobject.h"
#include "musicabstractmovedialog.h"

namespace Ui {
class MusicConnectTransferWidget;
}
class MusicFileSenderServer;

/*! @brief The class of the transfer file to mobile widget.
 * @author Greedysky <greedysky@163.com>
 */
class MUSIC_TOOL_EXPORT MusicConnectTransferWidget : public MusicAbstractMoveDialog
{
    Q_OBJECT
public:
    explicit MusicConnectTransferWidget(QWidget *parent = 0);
    /*!
     * Object contsructor.
     */
    virtual ~MusicConnectTransferWidget();

    static QString getClassName();
    /*!
     * Get class object name.
     */
    void openTransferFiles(int mode);
    /*!
     * Open transfer files by mode.
     */

Q_SIGNALS:
    void getMusicLists(MusicSongsList &songs, QStringList &names);
    /*!
     * Get music datas from container.
     */

public Q_SLOTS:
    void currentPlayListSelected(int index);
    /*!
     * Current playList selected.
     */
    void selectedAllItems(bool check);
    /*!
     * Select all items.
     */
    void startToTransferUSBFiles();
    /*!
     * Start to transfer usb files.
     */
    void startToTransferWIFIFiles();
    /*!
     * Start to transfer wifi files.
     */
    void reflashRemovableDir();
    /*!
     * Reflash removable dir.
     */
    void switchDiffDevice();
    /*!
     * Switch to different device.
     */
    void musicSearchIndexChanged(int row, int col);
    /*!
     * Search file from list.
     */
    virtual int exec();
    /*!
     * Override exec function.
     */

protected:
    void initColumns();
    /*!
     * Create the left button column
     */
    void createAllItems(const MusicSongs &songs);
    /*!
     * Create all items.
     */
    QStringList getSelectedFiles();
    /*!
     * Get selected files.
     */
    QString getRemovableDrive();
    /*!
     * Get removable drive name.
     */

    Ui::MusicConnectTransferWidget *ui;
    int m_currentIndex;
    MusicSongs m_currentSongs;
    MusicObject::MIntsListMap m_searchfileListCache;
    MusicFileSenderServer *m_sendServer;

};

#endif // MUSICCONNECTTRANSFERWIDGET_H
