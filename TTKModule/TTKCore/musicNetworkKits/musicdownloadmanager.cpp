#include "musicdownloadmanager.h"
#include "musicobject.h"
#include "musicconnectionpool.h"
#ifndef MUSIC_MOBILE
#include "musicdownloadstatusobject.h"
#include "musicdownloadrecordwidget.h"
#include "musiccloudtablewidget.h"
#endif

void MusicDownLoadManager::connectNetworkMultiValue(QObject *object)
{
#ifndef MUSIC_MOBILE
    m_queueList << object;
    QObject *to = M_CONNECTION_PTR->value( MusicDownloadStatusObject::getClassName() );
    if(to)
    {
        QObject::connect(object, SIGNAL(downLoadDataChanged(QString)), to, SLOT(showDownLoadInfoFinished(QString)));
    }
#else
    Q_UNUSED(object);
#endif
}

void MusicDownLoadManager::removeNetworkMultiValue(QObject *object)
{
    int index = m_queueList.indexOf(object);
    if(index != -1)
    {
        m_queueList.takeAt(index);
    }
}

void MusicDownLoadManager::connectMusicDownload(const MusicDownLoadPair &pair)
{
#ifndef MUSIC_MOBILE
    QString className;
    switch(pair.m_type)
    {
        case MusicNetwork::NormalDownload : className = MusicDownloadRecordTableWidget::getClassName(); break;
        case MusicNetwork::CloudDownload : className = MusicCloudDownloadTableWidget::getClassName(); break;
        case MusicNetwork::CloudUpload : className = MusicCloudUploadTableWidget::getClassName(); break;
        default: break;
    }
    QObject *to = M_CONNECTION_PTR->value(className);
    if(to && pair.m_object)
    {
        QObject::connect(pair.m_object, SIGNAL(downloadProgressChanged(float, QString, qint64)), to, SLOT(downloadProgressChanged(float, QString, qint64)));
        QObject::connect(pair.m_object, SIGNAL(createDownloadItem(QString, qint64)), to, SLOT(createDownloadItem(QString, qint64)));
    }

    QObject::connect(pair.m_object, SIGNAL(downloadProgressChanged(float, QString, qint64)), SLOT(downloadProgressChanged(float, QString, qint64)));
    m_pairList << pair;
#else
    Q_UNUSED(pair);
#endif
}

void MusicDownLoadManager::reconnectMusicDownload(const MusicDownLoadPair &pair)
{
    int index = m_pairList.indexOf(pair);
    if(index != -1)
    {
        MusicDownLoadPair *p = &m_pairList[index];
        QObject::disconnect(p->m_object, SIGNAL(downloadProgressChanged(float, QString, qint64)), pair.m_object, SLOT(downloadProgressChanged(float, QString, qint64)));
        QObject::disconnect(p->m_object, SIGNAL(createDownloadItem(QString, qint64)), pair.m_object, SLOT(createDownloadItem(QString, qint64)));

        QObject::connect(p->m_object, SIGNAL(downloadProgressChanged(float, QString, qint64)), pair.m_object, SLOT(downloadProgressChanged(float, QString, qint64)));
        QObject::connect(p->m_object, SIGNAL(createDownloadItem(QString, qint64)), pair.m_object, SLOT(createDownloadItem(QString, qint64)));
    }
}

void MusicDownLoadManager::removeMusicDownload(const MusicDownLoadPair &pair)
{
    int index = m_pairList.indexOf(pair);
    if(index != -1)
    {
        m_pairList.takeAt(index);
    }
}

void MusicDownLoadManager::downloadProgressChanged(float percent, const QString &total, qint64 time)
{
    Q_UNUSED(total);
    if(percent >= 100)
    {
        removeMusicDownload(MusicDownLoadPair(time));
    }
}
