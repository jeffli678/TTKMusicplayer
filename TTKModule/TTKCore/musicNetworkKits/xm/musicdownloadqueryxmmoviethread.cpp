#include "musicdownloadqueryxmmoviethread.h"
#include "musicdownloadqueryyytthread.h"
#include "musicsemaphoreloop.h"
#include "musicnumberutils.h"
#include "musiccoreutils.h"
#include "musictime.h"
#///QJson import
#include "qjson/parser.h"

MusicXMMVInfoConfigManager::MusicXMMVInfoConfigManager(QObject *parent)
    : MusicAbstractXml(parent)
{

}

void MusicXMMVInfoConfigManager::readMVInfoConfig(MusicObject::MusicSongInformation *info)
{
    info->m_timeLength = MusicTime::msecTime2LabelJustified(readXmlTextByTagName("duration").toInt()*1000);

    QDomNodeList resultlist = m_document->elementsByTagName("video");
    for(int i=0; i<resultlist.count(); ++i)
    {
        QDomNodeList infolist = resultlist.at(i).childNodes();
        MusicObject::MusicSongAttribute attr;
        for(int j=0; j<infolist.count(); ++j)
        {
            QDomNode node = infolist.at(j);
            if(node.nodeName() == "bitrate")
            {
                int bitRate = node.toElement().text().toInt();
                if(bitRate <= 375)
                    attr.m_bitrate = MB_250;
                else if(bitRate > 375 && bitRate <= 625)
                    attr.m_bitrate = MB_500;
                else if(bitRate > 625 && bitRate <= 875)
                    attr.m_bitrate = MB_750;
                else if(bitRate > 875)
                    attr.m_bitrate = MB_1000;
            }
            else if(node.nodeName() == "length")
            {
                attr.m_size = node.toElement().text();
            }
            else if(node.nodeName() == "video_url")
            {
                attr.m_url = node.toElement().text();
            }
        }

        if(!attr.m_url.isEmpty())
        {
            info->m_songAttrs.append(attr);
        }
    }
}



MusicDownLoadQueryXMMovieThread::MusicDownLoadQueryXMMovieThread(QObject *parent)
    : MusicDownLoadQueryMovieThread(parent)
{
    m_queryServer = "XiaMi";
    m_pageSize = 30;
}

void MusicDownLoadQueryXMMovieThread::startToSearch(QueryType type, const QString &text)
{
    if(!m_manager)
    {
        return;
    }

    M_LOGGER_INFO(QString("%1 startToSearch %2").arg(getClassName()).arg(text));
    m_searchText = text.trimmed();
    m_currentType = type;
    deleteAll();
    m_interrupt = true;

    QNetworkRequest request;
    if(!m_manager || m_stateCode != MusicNetwork::Init) return;
    makeTokenQueryUrl(&request,
                      MusicUtils::Algorithm::mdII(XM_SONG_DATA_URL, false).arg(text).arg(1).arg(m_pageSize),
                      MusicUtils::Algorithm::mdII(XM_SONG_URL, false));
    if(!m_manager || m_stateCode != MusicNetwork::Init) return;
    setSslConfiguration(&request);

    m_reply = m_manager->get(request);
    connect(m_reply, SIGNAL(finished()), SLOT(downLoadFinished()));
    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(replyError(QNetworkReply::NetworkError)));
}

void MusicDownLoadQueryXMMovieThread::startToPage(int offset)
{
    if(!m_manager)
    {
        return;
    }

    M_LOGGER_INFO(QString("%1 startToSearch %2").arg(getClassName()).arg(offset));
    m_pageTotal = 0;
    m_pageSize = 20;
    QUrl musicUrl = MusicUtils::Algorithm::mdII(XM_AR_MV_URL, false).arg(m_searchText).arg(offset + 1);
    deleteAll();
    m_interrupt = true;

    QNetworkRequest request;
    request.setUrl(musicUrl);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    request.setRawHeader("User-Agent", MusicUtils::Algorithm::mdII(XM_UA_URL_1, ALG_UA_KEY, false).toUtf8());
    setSslConfiguration(&request);

    m_reply = m_manager->get(request);
    connect(m_reply, SIGNAL(finished()), SLOT(pageDownLoadFinished()));
    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(replyError(QNetworkReply::NetworkError)));
}

void MusicDownLoadQueryXMMovieThread::startToSingleSearch(const QString &text)
{
    if(!m_manager)
    {
        return;
    }

    M_LOGGER_INFO(QString("%1 startToSingleSearch %2").arg(getClassName()).arg(text));
    m_searchText = text.trimmed();
    deleteAll();
    m_interrupt = true;

    QTimer::singleShot(MT_MS, this, SLOT(singleDownLoadFinished()));
}

void MusicDownLoadQueryXMMovieThread::downLoadFinished()
{
    if(!m_reply || !m_manager)
    {
        deleteAll();
        return;
    }

    M_LOGGER_INFO(QString("%1 downLoadFinished").arg(getClassName()));
    emit clearAllItems();
    m_musicSongInfos.clear();
    m_interrupt = false;

    if(m_reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = m_reply->readAll();///Get all the data obtained by request

        QJson::Parser parser;
        bool ok;
        QVariant data = parser.parse(bytes, &ok);
        if(ok)
        {
            QVariantMap value = data.toMap();
            if(value.contains("data"))
            {
                value = value["data"].toMap();
                value = value["data"].toMap();
                QVariantList datas = value["songs"].toList();
                foreach(const QVariant &var, datas)
                {
                    if(var.isNull())
                    {
                        continue;
                    }

                    value = var.toMap();
                    MusicObject::MusicSongInformation musicInfo;
                    musicInfo.m_singerName = MusicUtils::String::illegalCharactersReplaced(value["singers"].toString());
                    musicInfo.m_songName = MusicUtils::String::illegalCharactersReplaced(value["songName"].toString());
                    musicInfo.m_timeLength = MusicTime::msecTime2LabelJustified(value["length"].toInt());

                    musicInfo.m_songId = value["mvId"].toString();
                    if(m_interrupt || !m_manager || m_stateCode != MusicNetwork::Init) return;
                    readFromMusicMVAttribute(&musicInfo, false);
                    if(m_interrupt || !m_manager || m_stateCode != MusicNetwork::Init) return;

                    if(musicInfo.m_songAttrs.isEmpty())
                    {
                        continue;
                    }

                    MusicSearchedItem item;
                    item.m_songName = musicInfo.m_songName;
                    item.m_singerName = musicInfo.m_singerName;
                    item.m_time = musicInfo.m_timeLength;
                    item.m_type = mapQueryServerString();
                    emit createSearchedItem(item);
                    m_musicSongInfos << musicInfo;
                }
            }
        }
    }

    ///extra yyt movie
    if(m_queryExtraMovie && m_currentType == MovieQuery)
    {
        MusicSemaphoreLoop loop;
        MusicDownLoadQueryYYTThread *d = new MusicDownLoadQueryYYTThread(this);
        connect(d, SIGNAL(createSearchedItem(MusicSearchedItem)), SIGNAL(createSearchedItem(MusicSearchedItem)));
        connect(d, SIGNAL(downLoadDataChanged(QString)), &loop, SLOT(quit()));
        d->startToSearch(MusicDownLoadQueryYYTThread::MovieQuery, m_searchText);
        loop.exec();
        m_musicSongInfos << d->getMusicSongInfos();
    }

    emit downLoadDataChanged(QString());
    deleteAll();
    M_LOGGER_INFO(QString("%1 downLoadFinished deleteAll").arg(getClassName()));
}

void MusicDownLoadQueryXMMovieThread::pageDownLoadFinished()
{
    if(!m_reply || !m_manager)
    {
        deleteAll();
        return;
    }

    M_LOGGER_INFO(QString("%1 pageDownLoadFinished").arg(getClassName()));
    m_interrupt = false;

    if(m_reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = m_reply->readAll();///Get all the data obtained by request

        m_pageTotal = DEFAULT_LEVEL_HIGHER;
        QString html(bytes);
        QRegExp regx;
        regx.setMinimal(true);
        ///////////////////////////////////////////////////////////////////
        regx.setPattern("<p class=\"counts\">.*(\\d+).*</p>");
        if(html.indexOf(regx) != -1)
        {
            m_pageTotal = regx.cap(1).toInt();
        }
        ///////////////////////////////////////////////////////////////////
        regx.setPattern("<div class=\"mv_item100_block\">(.*)</div>");
        int pos = html.indexOf(regx);
        while(pos != -1)
        {
            if(m_interrupt) return;

            QString cap = regx.cap(1);
            QRegExp partial(".*src=\"([^\"]+).*href=\"([^\"]+).*title=\"([^\"]+).*>");
            partial.setMinimal(true);
            int pos_p = cap.indexOf(partial);
            while(pos_p != -1)
            {
                if(m_interrupt) return;

                MusicResultsItem info;
                info.m_id = partial.cap(2).remove("/mv/");
                info.m_coverUrl = partial.cap(1).remove("@1e_1c_100Q_160w_90h").replace("https://", "http://");
                info.m_name = partial.cap(3);
                info.m_updateTime.clear();
                emit createMovieInfoItem(info);

                pos_p += partial.matchedLength();
                pos_p = partial.indexIn(cap, pos_p);
            }

            pos += regx.matchedLength();
            pos = regx.indexIn(html, pos);
        }
    }

    emit downLoadDataChanged(QString());
    deleteAll();
    M_LOGGER_INFO(QString("%1 pageDownLoadFinished deleteAll").arg(getClassName()));
}

void MusicDownLoadQueryXMMovieThread::singleDownLoadFinished()
{
    M_LOGGER_INFO(QString("%1 singleDownLoadFinished").arg(getClassName()));

    emit clearAllItems();
    m_musicSongInfos.clear();
    m_interrupt = false;

    MusicObject::MusicSongInformation musicInfo;
    musicInfo.m_songId = m_searchText;
    if(m_interrupt || !m_manager || m_stateCode != MusicNetwork::Init) return;
    readFromMusicMVAttribute(&musicInfo, true);
    if(m_interrupt || !m_manager || m_stateCode != MusicNetwork::Init) return;

    if(!musicInfo.m_songAttrs.isEmpty())
    {
        MusicSearchedItem item;
        item.m_songName = musicInfo.m_songName;
        item.m_singerName = musicInfo.m_singerName;
        item.m_time = musicInfo.m_timeLength;
        item.m_type = mapQueryServerString();
        emit createSearchedItem(item);
        m_musicSongInfos << musicInfo;
    }

    emit downLoadDataChanged(QString());
    deleteAll();
    M_LOGGER_INFO(QString("%1 singleDownLoadFinished deleteAll").arg(getClassName()));
}

void MusicDownLoadQueryXMMovieThread::readFromMusicMVAttribute(MusicObject::MusicSongInformation *info, bool more)
{
    if(info->m_songId.isEmpty() || !m_manager)
    {
        return;
    }

    QUrl musicUrl = MusicUtils::Algorithm::mdII(XM_MV_ATTR_URL, false).arg(info->m_songId);

    QNetworkRequest request;
    request.setUrl(musicUrl);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    request.setRawHeader("User-Agent", MusicUtils::Algorithm::mdII(XM_UA_URL_1, ALG_UA_KEY, false).toUtf8());
    setSslConfiguration(&request);

    MusicSemaphoreLoop loop;
    QNetworkReply *reply = m_manager->get(request);
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));
    loop.exec();

    if(!reply || reply->error() != QNetworkReply::NoError)
    {
        return;
    }

    QString text(reply->readAll());
    QRegExp regx;

    if(more)
    {
        regx.setPattern("<h1([^<]+)>([^<]+)</h1>");
        int pos = text.indexOf(regx);
        while(pos != -1)
        {
            info->m_songName = regx.cap(2);
            pos += regx.matchedLength();
            pos = regx.indexIn(text, pos);
            break;
        }

        regx.setPattern("<a target=\"_blank\" title=([^<]+)>([^<]+)</a>");
        pos = text.indexOf(regx);
        while(pos != -1)
        {
            info->m_singerName = regx.cap(2);
            pos += regx.matchedLength();
            pos = regx.indexIn(text, pos);
            break;
        }
    }

    regx.setPattern("vid:\"(\\d+)\",uid:\"(\\d+)\"");
    int pos = text.indexOf(regx);
    QString uid, vid;
    while(pos != -1)
    {
        vid = regx.cap(1);
        uid = regx.cap(2);
        pos += regx.matchedLength();
        pos = regx.indexIn(text, pos);
        break;
    }

    readFromMusicMVAttribute(info, vid, uid);
}

void MusicDownLoadQueryXMMovieThread::readFromMusicMVAttribute(MusicObject::MusicSongInformation *info, const QString &vid, const QString &uid)
{
    if(vid.isEmpty() || uid.isEmpty() || !m_manager)
    {
        return;
    }

    QUrl musicUrl = MusicUtils::Algorithm::mdII(XM_MV_QUERY_URL, false).arg(vid).arg(uid);

    QNetworkRequest request;
    request.setUrl(musicUrl);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    request.setRawHeader("User-Agent", MusicUtils::Algorithm::mdII(XM_UA_URL_1, ALG_UA_KEY, false).toUtf8());
    setSslConfiguration(&request);

    MusicSemaphoreLoop loop;
    QNetworkReply *reply = m_manager->get(request);
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));
    loop.exec();

    if(!reply || reply->error() != QNetworkReply::NoError)
    {
        return;
    }

    MusicXMMVInfoConfigManager xml;
    xml.fromByteArray(reply->readAll());
    xml.readMVInfoConfig(info);

    for(int i=0; i<info->m_songAttrs.count(); ++i)
    {
        MusicObject::MusicSongAttribute *attr = &info->m_songAttrs[i];
        QString urlPrefix = attr->m_url;
        QStringList urls;
        int round = attr->m_size.toLongLong()/20000000 + 1;
        int start = 0, end = 0;
        while(round > 0)
        {
            end = start + 20000000;
            if(end >= attr->m_size.toLongLong())
            {
                end = attr->m_size.toLongLong() - 1;
            }
            urls << (urlPrefix + QString("/start_%1/end_%2/1.flv").arg(start).arg(end));
            start = end + 1;
            --round;
        }

        if(urls.count() > 1)
        {
            attr->m_multiPart = true;
        }
        attr->m_size = MusicUtils::Number::size2Label(attr->m_size.toLongLong());
        attr->m_url = urls.join(TTK_STR_SPLITER);
        attr->m_format = MusicUtils::Core::fileSuffix(attr->m_url);
    }
}
