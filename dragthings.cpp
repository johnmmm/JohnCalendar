#include "dragthings.h"
#include <QDragEnterEvent>
#include <QMimeData>
#include <iostream>
using namespace std;

dragthings::dragthings(QWidget *parent)
    : QLabel(parent)
{
    setFixedSize(100,60);
    setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
    setAlignment(Qt::AlignCenter);
    //setAttribute(Qt::WA_TransparentForMouseEvents);
    setStyleSheet("background:transparent");
    setAcceptDrops(true);
    setAutoFillBackground(true);
}

dragthings::~dragthings()
{

}

void dragthings::dragEnterEvent(QDragEnterEvent *event)
{
    //如果为文件，则支持拖放
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }

    setText(tr("<drop here>"));
    setBackgroundRole(QPalette::Highlight);

    event->acceptProposedAction();
}

void dragthings::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void dragthings::dragLeaveEvent(QDragLeaveEvent *event)
{
    clear();
}

void dragthings::dropEvent(QDropEvent *event)
{
    //注意：这里如果有多文件存在，意思是用户一下子拖动了多个文件，而不是拖动一个目录
    //如果想读取整个目录，则在不同的操作平台下，自己编写函数实现读取整个目录文件名
    QList<QUrl> urls = event->mimeData()->urls();
    if(urls.isEmpty())
        return;

    //往文本框中追加文件名
    foreach(QUrl url, urls)
    {
        QString file_name = url.toLocalFile();//获得了它的文件名
        nameOffile = file_name;
        //添加到...
        //qDebug() << url.fileName();
        idnamefile = url.fileName();
    }

//    setText(tr(nameOffile.toStdString().data()));
//    setStyleSheet("background-color: rgb(255, 239, 41); border-radius: 4px;");
    QLabel *label ;
           label = new QLabel(tr(nameOffile.toStdString().data()), this);

    event->acceptProposedAction();

    putFile(nameOffile,filedate);

    setText(tr("file here"));
}

void dragthings::mousePressEvent(QMouseEvent *event)
{
    //获取鼠标按下时候的坐标
    if (event->button() == Qt::LeftButton)
        startPos = event->pos();
    //QListWidget::mousePressEvent(event);
}

void dragthings::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        int distance = (event->pos() - startPos).manhattanLength();
        //如果长度大于推荐的拖动起始距离，则认为是拖动(存在用户手抖的情况)
        if (distance >= QApplication::startDragDistance())
        {
            if(text().isEmpty())
                return;

            QPoint hotSpot = event->pos() - pos();

            QList<QUrl> urls;
            urls.append(QUrl::fromLocalFile(nameOffile));

            QMimeData *mimeData = new QMimeData();
            mimeData->setUrls(urls);

            QDrag *drag = new QDrag(this);
            drag->setMimeData(mimeData);
            // drag->setObjectName("file");
            drag->setHotSpot(hotSpot);
            drag->exec(Qt::CopyAction);
            clear();

        }
    }
    //QListWidget::mouseMoveEvent(event);
}

void dragthings::clear()
{
    setText(tr(""));
    setStyleSheet("background:transparent");
}

void dragthings::setTexts()
{
    setText(tr("file here"));
}

void dragthings::putFile(QString source, QDate date)
{
    QString id = date.toString("yyyyMMdd");
    QDir dir;
    dir.mkdir(QApplication::applicationDirPath() + "/" + id);
    id = QApplication::applicationDirPath() +"/"+ id + idnamefile;
    //QUrl url(QApplication::applicationDirPath() + "/" + id);
    //qDebug() << url.isValid();
    //qDebug() << "copy file from " << source << " to " << id;
    QFile::copy(source,id);
}
