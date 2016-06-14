#ifndef MYPLAYLIST_H
#define MYPLAYLIST_H

#include <QTableWidget>



class MyPlaylist : public QTableWidget
{
    Q_OBJECT
public:
    explicit MyPlaylist(QWidget *parent = 0);

protected:
    void contextMenuEvent(QContextMenuEvent *);
    void closeEvent(QCloseEvent *);
signals:
    void playlistClean();
    
public slots:
    void clearPlaylist();
    
};

#endif // MYPLAYLIST_H
