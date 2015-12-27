#ifndef MUSICBARRAGEWIDGET_H
#define MUSICBARRAGEWIDGET_H

/* =================================================
 * This file is part of the Music Player project
 * Copyright (c) 2014 - 2015 Greedysky Studio
 * All rights reserved!
 * Redistribution and use of the source code or any derivative
 * works are strictly forbiden.
   =================================================*/

#include "musiclibexportglobal.h"

#include <QPropertyAnimation>
#include <QLabel>

#define NUMBER 5

class MUSIC_WIDGET_EXPORT MusicBarrageAnimation : public QPropertyAnimation
{
    Q_OBJECT
public:
    explicit MusicBarrageAnimation(QObject *parent = 0);
    MusicBarrageAnimation(QObject *target, const QByteArray &propertyName, QObject *parent = 0);
    ~MusicBarrageAnimation() = default;

    void setSize(const QSize &size);

public slots:
    void animationFinished();

protected:
    void init();
    QSize m_parentSize;

};


class MUSIC_WIDGET_EXPORT MusicBarrageWidget : public QObject
{
    Q_OBJECT
public:
    explicit MusicBarrageWidget(QObject *parent = 0);
    ~MusicBarrageWidget();

    void start();
    void pause();
    void stop();

    void barrageStateChanged(bool on);
    void setSize(const QSize &size);

    void setLabelBackground(const QColor &color);
    void setLabelTextSize(int size);

protected:
    void deleteItems();
    void createLabel();
    void createAnimation();

    QWidget *m_parentClass;
    QList<QLabel*> m_labels;
    QList<MusicBarrageAnimation*> m_animations;
    bool m_barrageState;
    QSize m_parentSize;

};

#endif // MUSICBARRAGEWIDGET_H