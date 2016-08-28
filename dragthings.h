#ifndef DRAGTHINGS_H
#define DRAGTHINGS_H

#include "calendar.h"
#include "mainwindow.h"
#include "storage.h"
#include <QLabel>
#include <QWidget>
#include <QString>
#include <QMouseEvent>
#include <QGridLayout>
#include <QVariant>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QListWidget>
#include <QApplication>
#include <QDropEvent>
#include <QDrag>
#include <QDragMoveEvent>
#include <QDate>
#include <QApplication>

class dragthings : public QLabel
{
    Q_OBJECT

public:
    dragthings(QWidget *parent = 0);
    ~dragthings();


    void dragEnterEvent(QDragEnterEvent *event);//一堆事件
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void clear();//清空拖拽栏
    void setTexts();//在拖拽栏上生成文字
    void putFile(QString source, QDate date);//拖拽进文件的复制及创建文件夹功能

    QString nameOffile;
    QString idnamefile;

    QDate filedate;

    bool dropend = false;
    bool mouseMoveEnd = false;

private slots:

private:
    QPoint startPos;
};

#endif // DRAGTHINGS_H
