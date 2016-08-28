#include "calendar.h"
#include "storage.h"
#include <QtWidgets>
#include <QInputDialog>
#include <QTextCharFormat>
#include <QDebug>
#include <QtGlobal>
//#include "draglabel.h"


calendar::calendar()
{
    m_outlinePen.setColor(Qt::red);
    m_transparentBrush.setColor(Qt::transparent);
    //setAcceptDrops(true);

    for(int i = 0; i <= 1100; i++)
    {
        for(int j = 0; j <= 12; j++)
        {
            for(int k = 0; k <= 31; k++)
            {
                isdragsth[i][j][k] = false;
            }
        }
    }
}

calendar::~calendar()
{

}


void calendar::addNote( const QDate &date )
{
    //qDebug()<<"come in addNote"<<endl;
    QString memo = QInputDialog::getText( this,
            "Memo", "Description:" );

    if( !memo.isEmpty() )
    {
        QBrush brush;
        brush.setColor(Qt::yellow);

        QTextCharFormat cf = this->dateTextFormat(date);
        cf.setBackground(brush);
        this->setDateTextFormat(date, cf);

        if(dates.contains(date))
        {
            int index = dates.indexOf(date);
            todolist.replace(index, memo);
        }
        else
        {
            dates.append(date);
            todolist.append(memo);
        }
    }
    source.storeToFile(date,memo);
    source.storeToFile(date,Qt::yellow);
}

void calendar::setColor(QColor& color)
{
   m_outlinePen.setColor(color);
}

int calendar::getindexs(int yea, int mon)
{
    int indexs = dates.indexOf(QDate(yea,mon,1))-QDate(yea,mon,1).dayOfWeek()%7+1;
    return indexs;
}

void calendar::addNote1(QDate date, QString con)
{
    QBrush brush;
    brush.setColor(Qt::yellow);

    QTextCharFormat cf = this->dateTextFormat(date);
    cf.setBackground(brush);
    this->setDateTextFormat(date, cf);

    if(dates.contains(date))
    {
        int index = dates.indexOf(date);
        todolist.replace(index, con);
    }
    else
    {
        dates.append(date);
        todolist.append(con);
    }
    source.storeToFile(date,con);
    source.storeToFile(date,Qt::yellow);
}

void calendar::deleteNote(QDate date)
{
    QBrush brush;
    brush.setColor(Qt::white);

    QTextCharFormat cf = this->dateTextFormat(date);
    cf.setBackground(brush);
    this->setDateTextFormat(date, cf);

    if(dates.contains(date))
    {
        int index = dates.indexOf(date);
        dates.remove(index);
        todolist.remove(index);
    }
    source.storeToFile(date);
    source.storeToFile(date,Qt::white);
}

void calendar::paintaColor(QDate date, QColor color)
{
    QBrush brush;
    brush.setColor(color);

    QTextCharFormat cf = this->dateTextFormat(date);
    cf.setBackground(brush);
    this->setDateTextFormat(date, cf);

    source.storeToFile(date,color);
}

void calendar::dragEnd(QDate date, QString str)
{
    source.storeToFile1(date, str);
}

void calendar::dragEnd(QDate date)
{
    source.storeToFile1(date);
}

void calendar::storeTheCal()
{
    QFileDialog fd;
    QStringList filters;
    filters << tr("JSON file (*.json)")
            << tr("All files (*)");
    fd.setNameFilters(filters);
    fd.setWindowTitle(tr("Export"));
    fd.setFileMode(QFileDialog::AnyFile);
    fd.setAcceptMode(QFileDialog::AcceptSave);
    if(fd.exec())
    {
        source.exportFile(fd.selectedFiles()[0]);
        //qDebug() << "begin to store";
    }
}

void calendar::storeInTheEnd()
{
    source.exportFile(QApplication::applicationDirPath() + "/config.json");
}

void calendar::removeTheSource()
{
    source.json_str.clear();
}

QString calendar::getNote(QDate date)
{
    QString note;
    note = todolist.at(dates.indexOf(date));
    return note;
}

void calendar::paintCell(QPainter *painter, const QRect &rect, const QDate &date) const
{
   //qDebug()<<"come in paintCell"<<endl;
   QCalendarWidget::paintCell(painter, rect, date);
   Q_ASSERT(dates.size()==todolist.size());
   for (int i = 0; i < dates.size(); i++)
   {
       if (date == dates.at(i))
       {
           painter->setPen(m_outlinePen);
           painter->setBrush(m_transparentBrush);
           painter->drawRect(rect.adjusted(0, 0, -1, -1));
           painter->drawText(rect,todolist.at(i));
       }
   }

}


