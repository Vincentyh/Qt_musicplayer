#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <Phonon>
#include "myplaylist.h"

class QLabel;
class MyPlaylist;


namespace Ui {
class MyWidget;
}

class MyWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit MyWidget(QWidget *parent = 0);
    ~MyWidget();
    
private:
    Ui::MyWidget *ui;
    void initPlayer();
    Phonon::MediaObject *mediaObject;
    QAction *playAction;
    QAction *stopAction;
    QAction *skipBackwardAction;
    QAction *skipForwardAction;
    QLabel *topLabel;
    QLabel *timeLabel;

    MyPlaylist *playlist;
    Phonon::MediaObject *metaInformationReslover;
    QList<Phonon::MediaSource> sources;
    void changeActionState();


private slots:
    void updateTime(qint64 time);
    void setPaused();
    void skipBackward();
    void skipForward();
    void openFile();
    void setPlaylistShow();
    void setLrcShown();
    void stateChanged(Phonon::State newState, Phonon::State oldState);

    void sourceChanged(const Phonon::MediaSource &source);
    void aboutToFinish();
    void metaStateChanged(Phonon::State newState, Phonon::State oldState);
    void tableClicked(int row);
    void clearSources();
};

#endif // MYWIDGET_H
