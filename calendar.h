#ifndef CALENDAR_H
#define CALENDAR_H

#include <QCalendarWidget>
#include <QPainter>
#include <QColor>
#include <qdate>
#include <qpen>
#include <qbrush>
#include <QVector>
#include <QFileDialog>
#include <QStringList>
#include <QString>

QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QDropEvent;
QT_END_NAMESPACE

class calendar: public QCalendarWidget
{
    Q_OBJECT
public:
    calendar();
    ~calendar();

    void setColor(QColor& color);//上色
    int getindexs(int yea, int mon);//获得当前日期的index
    void addNote1(QDate date, QString con);//批量增加事件
    void deleteNote(QDate date);//批量删除事件
    void paintaColor(QDate date, QColor color);//上色
    void dragEnd(QDate date, QString str);//拖进结束的时候保存
    void dragEnd(QDate date);//拖出结束的时候保存
    void storeTheCal();//保存
    void storeInTheEnd();//结束的时候自动保存
    void removeTheSource();//删除设置的时候删除记录
    QString getNote(QDate date);//获得当前天的note
    bool isdragsth[1101][13][32];//判断当前天是否发生过拖拽
protected:
    virtual void paintCell(QPainter * painter, const QRect & rect, const QDate & date) const;

private slots:
    void addNote( const QDate & );

private:
    QPen m_outlinePen;
    QBrush m_transparentBrush;

    QVector<QDate> dates;
    QVector<QString> todolist;

};

#endif // CALENDAR_H
