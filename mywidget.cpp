#include "mywidget.h"
#include "ui_mywidget.h"
#include <QLabel>
#include <QToolBar>
#include <QVBoxLayout>
#include <QTime>
#include <QMessageBox>
#include <QFileInfo>
#include <QFileDialog>
#include <QDesktopServices>


MyWidget::MyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyWidget)
{
    ui->setupUi(this);

    initPlayer();
}

MyWidget::~MyWidget()
{
    delete ui;
}

void MyWidget::initPlayer()
{
    setWindowTitle(tr("音乐播放器"));
    setWindowIcon(QIcon("./images/icon.png"));
    setMinimumSize(320,160);
    setMaximumSize(320,160);
    mediaObject = new Phonon::MediaObject(this);
    Phonon::AudioOutput *audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory,this);
    Phonon::createPath(mediaObject, audioOutput);

    connect(mediaObject, SIGNAL(tick(qint64)), this, SLOT(updateTime(qint64)));

    topLabel = new QLabel(tr("<a href = \"http://www.baidu.com\"> www.baidu.com </a>"));
    topLabel->setTextFormat(Qt::RichText);
    topLabel->setOpenExternalLinks(true);
    topLabel->setAlignment(Qt::AlignCenter);

    Phonon::SeekSlider *seekSlider = new Phonon::SeekSlider(mediaObject,this);

    QToolBar *widgetBar = new QToolBar(this);

    timeLabel = new QLabel(tr("00:00 / 00:00"), this);
    timeLabel->setToolTip(tr("当前时间 / 总时间"));
    timeLabel->setAlignment(Qt::AlignCenter);
    timeLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    //timeLabel->show();
    //创建图标，用于控制是否显示播放列表
    QAction *PLAction = new QAction(tr("PL"), this);
    PLAction->setShortcut(QKeySequence("F4"));
    PLAction->setToolTip(tr("播放列表"));
    connect(PLAction, SIGNAL(triggered()), this, SLOT(setPlaylistShow()));
    //创建图标，用于控制是否显示歌词
    QAction *LRCAction = new QAction(tr("LRC"), this);
    LRCAction->setShortcut(QKeySequence("F4"));
    LRCAction->setToolTip(tr("歌词"));
    connect(LRCAction, SIGNAL(triggered()), this, SLOT(setLrcShown()));
    //添加到工具栏
    widgetBar->addAction(PLAction);
    widgetBar->addSeparator();
    widgetBar->addWidget(timeLabel);
    widgetBar->addAction(LRCAction);
    widgetBar->addSeparator();

    //创建播放工具栏
    QToolBar *toolBar = new QToolBar(this);
    playAction = new QAction(this);
    playAction->setIcon(QIcon("./images/play.png"));
    playAction->setText(tr("播放"));
    playAction->setShortcut(QKeySequence("F5"));
    connect(playAction, SIGNAL(triggered()), this, SLOT(setPaused()));
    stopAction = new QAction(this);
    stopAction->setIcon(QIcon("./images/stop.png"));
    stopAction->setText(tr("停止"));
    stopAction->setShortcut(QKeySequence(tr("F6")));
    connect(stopAction, SIGNAL(triggered()), mediaObject, SLOT(stop()));
    //上一首
    skipBackwardAction  = new QAction(this);
    skipBackwardAction->setIcon(QIcon("./images/skipBackward.png"));
    skipBackwardAction->setText(tr("上一首(Ctrl + Left)"));
    skipBackwardAction->setShortcut(QKeySequence(tr("Ctrl + Left")));
    //skipBackwardAction->setEnabled(false);
    connect(skipBackwardAction, SIGNAL(triggered()), this, SLOT(skipBackward()));
    //下一首
    skipForwardAction = new QAction(this);
    skipForwardAction->setIcon(QIcon("./images/skipForward.png"));
    skipForwardAction->setText(tr("下一首(Ctrl + Right)"));
    skipForwardAction->setShortcut(QKeySequence(tr("Ctrl + Right")));
    connect(skipForwardAction, SIGNAL(triggered()), this, SLOT(skipForward()));
    //打开文件
    QAction *openAction = new QAction(this);
    openAction->setIcon(QIcon("./images/open.png"));
    openAction->setText(tr("播放文件"));
    openAction->setShortcut(QKeySequence(tr("Ctrl + O")));
    connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));
    //音量控制部件
    Phonon::VolumeSlider *volumeSlider = new Phonon::VolumeSlider(audioOutput, this);
    volumeSlider->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    toolBar->addAction(playAction);
    toolBar->addSeparator();
    toolBar->addAction(stopAction);
    toolBar->addSeparator();
    toolBar->addAction(skipBackwardAction);
    toolBar->addSeparator();
    toolBar->addAction(skipForwardAction);
    toolBar->addSeparator();
    toolBar->addWidget(volumeSlider);
    toolBar->addSeparator();
    toolBar->addAction(openAction);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(topLabel);
    mainLayout->addWidget(seekSlider);
    mainLayout->addWidget(widgetBar);
    mainLayout->addWidget(toolBar);
    setLayout(mainLayout);

    //mediaObject->setCurrentSource(Phonon::MediaSource("./music.mp3"));
    playlist = new MyPlaylist(this);
    connect(playlist, SIGNAL(cellChanged(int,int)), this, SLOT(tableClicked(int)));
    connect(playlist, SIGNAL(playlistClean()), this, SLOT(clearSources()));
    //解析媒体信息的元信息解析器
    metaInformationReslover = Phonon::MediaObject(this);
    Phonon::AudioOutput *metaInformationAudioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    Phonon::createPath(metaInformationReslover, metaInformationAudioOutput);
    connect(metaInformationReslover, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
            this, SLOT(metaStateChanged(Phonon::State,Phonon::State)));
    connect(mediaObject, SIGNAL(currentSourceChanged(Phonon::MediaSource)),
            this, SLOT(sourceChanged(Phonon::MediaSource)));
    connect(mediaObject, SIGNAL(aboutToFinish()), this, SLOT(aboutToFinish()));
    //设置动作图标初始状态
    playAction->setEnabled(false);
    stopAction->setEnabled(false);
    skipBackwardAction->setEnabled(false);
    skipForwardAction->setEnabled(false);
    topLabel->setFocus();


    connect(mediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
            this, SLOT(stateChanged(Phonon::State,Phonon::State)));
}

void MyWidget::setPaused()
{
    if(mediaObject->state() == Phonon::PlayingState)
        mediaObject->pause();
    else
        mediaObject->play();
}

void MyWidget::updateTime(qint64 time)
{
    qint64 totalTimeValue = mediaObject->totalTime();
    QTime totalTime(0, (totalTimeValue / 60000) % 60, (totalTimeValue / 1000) % 60);
    QTime currentTime(0, (time / 60000) % 60, (time / 1000) % 60);
    QString str = currentTime.toString("mm:ss") + " / " + totalTime.toString("mm:ss");
    timeLabel->setText(str);
}

void MyWidget::stateChanged(Phonon::State newState, Phonon::State oldState)
{
    switch(newState)
    {
    case Phonon::ErrorState:

        if(mediaObject->errorType() == Phonon::FatalError)
        {
            QMessageBox::warning(this, tr("致命错误"), mediaObject->errorString());
        }else
        {
            QMessageBox::warning(this, tr("错误"), mediaObject->errorString());
        }
        break;

    case Phonon::PlayingState:
        stopAction->setEnabled(true);
        playAction->setIcon(QIcon("./images/pause.png"));
        playAction->setText(tr("暂停a"));
        topLabel->setText(QFileInfo(mediaObject->currentSource().fileName()).baseName());
        break;

    case Phonon::StoppedState:
        stopAction->setEnabled(false);
        stopAction->setIcon(QIcon("./images/play.png"));
        stopAction->setText(tr("播放"));
        playAction->setIcon(QIcon("./images/play.png"));
        topLabel->setText(tr("<a href = \"http://www.baidu.com\"> www.baidu.com </a>"));
        timeLabel->setText(tr("00:00 / 00:00"));
        break;
    case Phonon::PausedState:
        stopAction->setEnabled(true);
        playAction->setIcon(QIcon("./image/play.png"));
        playAction->setText(tr("播放a"));
        topLabel->setText(QFileInfo(mediaObject->currentSource().fileName()).baseName() + tr("\n") + tr("已暂停"));
        break;
    default:
        ;
    }
}

void MyWidget::skipBackward()
{
}

void MyWidget::skipForward()
{
}

void MyWidget::openFile()
{
    QStringList list = QFileDialog::getOpenFileNames(this, tr("打开音乐文件"),
                        QDesktopServices::storageLocation(QDesktopServices::MusicLocation));
    if(list.isEmpty())
        return;

    int index = sources.size();
    foreach(QString string, list)
    {
        Phonon::MediaSource source(string);
        sources.append(source);
    }
    if(!sources.isEmpty())
    {
        metaInformationReslover->setCurrentSource(sources.at(index));
    }
}

void MyWidget::setPlaylistShow()
{
    if(playlist->isHidden())
    {
        playlist->move(frameGeometry().bottomLeft());
        playlist->show();
    }else{
        playlist->hide();
    }
}

void MyWidget::setLrcShown()
{
}

void MyWidget::metaStateChanged(Phonon::State newState, Phonon::State oldState)
{
    if(newState == Phonon::ErrorState)
    {
        QMessageBox::warning(this, tr("打开文件出错"), metaInformationReslover->errorString());
        while(!sources.isEmpty() && !(sources.takeLast() == metaInformationReslover->currentSource()))
        {};
        return;
    }

    if(newState != Phonon::StoppedState && newState != Phonon::PausedState)
        return;

    if(metaInformationReslover->currentSource().type() == Phonon::MediaSource::Invalid)
        return;

    QMap<QString, QString> metaData = metaInformationReslover->metaData();

    QString title = metaData.value("TITLE");
    if(title == "")
    {
        QString str = metaInformationReslover->currentSource().fileName();
        title = QFileInfo(str).baseName();

    }
    QTableWidgetItem *titleItem = new QTableWidgetItem(title);
    titleItem->setFlags(titleItem->flags() ^ Qt::ItemIsEditable);

    QTableWidgetItem *artistItem = new QTableWidgetItem(metaData.value("ARTIST"));
    artistItem->setFlags(artistItem->flags()^Qt::ItemIsEditable);

    qint64 totalTime = metaInformationReslover->totalTime();
    QTime time(0, (totalTime/60000)%60, (totalTime/60000) % 60);
    QTableWidgetItem *timeItem = new QTableWidgetItem(time.toString("mm::ss"));

    int currentRow = playlist->rowCount();
    playlist->insertRow(currentRow);
    playlist->setItem(currentRow, 0, titleItem);
    playlist->setItem(currentRow, 1, artistItem);
    playlist->setItem(currentRow, 2, timeItem);

    int index = sources.indexOf(metaInformationReslover->currentSource()) + 1;
    if(sources.size() > index)
    {
        metaInformationReslover->setCurrentSource(sources.at(index));
    }else{
        if(playlist->selectedItems().isEmpty())
        {
            if(mediaObject->state() != Phonon::PlayingState && mediaObject->state() != Phonon::PausedState)
            {
                mediaObject->setCurrentSource(sources.at(0));
            }else{
                playlist->selectRow(0);
                changeActionState();
            }
        }else{
            changeActionState();
        }
    }
}

