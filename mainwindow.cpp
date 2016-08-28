#include "mainwindow.h"
#include <QApplication>
#include <QWidget>
#include <iostream>
#include <QFileDialog>
#include <QStringList>
#include <QLocale>

#ifdef Q_OS_LINUX
#include <QtX11Extras/QX11Info>
#include <X11/extensions/shape.h>
#endif

using namespace std;

//**************************************** PUBLIC ****************************************//

MainWindow::MainWindow(QWidget *parent) :
     QWidget(parent)
{
    winOpacity = 1;

    cale = new calendar;

    cale->setLocale(QLocale("en"));//初始化calendar的语言

    createPreviewGroupBox();
    createGeneralOptionsGroupBox();

    currentyear = QDate::currentDate().year();
    currentmonth = QDate::currentDate().month();
    for(int i = 0; i < 42; i++)
    {
        droparea[i]->clear();
    }
    for(int i = 1; i <= 31; i++)
    {
        if(QDate(currentyear,currentmonth,i).isValid())
        {
            if(cale->isdragsth[currentyear-1900][currentmonth][i])
            {
                droparea[i+QDate(currentyear,currentmonth,1).dayOfWeek()-1]->setTexts();
            }
        }
    }
    firstindex = 0 - QDate(currentyear,currentmonth,1).dayOfWeek() % 7;
    //cout << firstindex << endl;

    setWindowOpacity(winOpacity);

    settheColor();

    layout = new QVBoxLayout;
    layout->addWidget(previewGroupBox);
    layout->addWidget(generalOptionsGroupBox);
    //layout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(layout);

    QDesktopWidget* desktopWidget = QApplication::desktop();
    QRect screenRect = desktopWidget->screenGeometry();
    this->resize(QSize(3*screenRect.width()/4, 3*screenRect.height()/4));
    this->setFixedSize(this->width(),this->height());

    connect(cale, SIGNAL(activated(QDate)), cale, SLOT(addNote(QDate)));

    setWindowTitle(tr("Calendar Widget"));

    importFile(QApplication::applicationDirPath() + "/config.json");
}

MainWindow::~MainWindow()
{
    cale->storeInTheEnd();
}

void MainWindow::setSlider()
{
    opacitySlider->setRange(10,100);
    opacitySlider->setValue(100);

    QObject::connect(opacitySlider, SIGNAL(valueChanged(int)), this, SLOT(changeOpacity(int)));
}

void MainWindow::settheColor()
{
    colortobe[0] = QColor(255, 239, 41);
    colortobe[1] = QColor(255, 29, 45);
    colortobe[2] = QColor(19, 34, 255);
    colortobe[3] = QColor(255, 22, 236);
    colortobe[4] = QColor(14, 255, 246);
    colortobe[5] = QColor(26, 255, 24);
    colortobe[6] = QColor(255,255,255);
    colortobe[7] = QColor(0,0,0);
    //qDebug() << colortobe[0].red();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    for(int i = 0; i < 42; i++)
    {
        if(obj == droparea[i])
        {
            if(event->type() == QEvent::Drop && i+firstindex>0)
            {
                theindextoaddfilename = i-QDate(currentyear,currentmonth,1).dayOfWeek()+1;
                //qDebug() << "add note to date " << i-QDate(currentyear,currentmonth,1).dayOfWeek()+1;
                droparea[i]->filedate = QDate(currentyear,currentmonth,i-QDate(currentyear,currentmonth,1).dayOfWeek()+1);
                droparea[i]->dropend = true;
                cale->isdragsth[currentyear-1900][currentmonth][i-QDate(currentyear,currentmonth,1).dayOfWeek()+1] = true;
                return false;
            }
            if(event->type() == QEvent::MouseMove && i+firstindex>0)
            {
                droparea[i]->mouseMoveEnd = true;
                droparea[i]->filedate = QDate(currentyear,currentmonth,i-QDate(currentyear,currentmonth,1).dayOfWeek()+1);
                cale->isdragsth[currentyear-1900][currentmonth][i-QDate(currentyear,currentmonth,1).dayOfWeek()+1] = false;
                return false;
            }
        }
    }
    for(int i = 1; i < 42; i++)
    {
        if(droparea[i]->dropend)
        {
            cale->addNote1(droparea[i]->filedate,droparea[i]->nameOffile);
            droparea[i]->dropend = false;
            cale->dragEnd(QDate(currentyear,currentmonth,i-QDate(currentyear,currentmonth,1).dayOfWeek()+1), droparea[i]->nameOffile);
        }
        if(droparea[i]->mouseMoveEnd)
        {
            cale->deleteNote(droparea[i]->filedate);
            droparea[i]->mouseMoveEnd = false;
            cale->dragEnd(QDate(currentyear,currentmonth,i-QDate(currentyear,currentmonth,1).dayOfWeek()+1));
        }
    }
    return false;
}

void MainWindow::updatefilename()
{
    cale->addNote1(QDate(currentyear,currentmonth,theindextoaddfilename),droparea[theindextoaddfilename]->nameOffile);
}

void MainWindow::importFile(QString filename)
{
    QFile file(filename);
    QTextStream stream(&file);
    QString line;
    int colorr,colorg,colorb,year,month,day;
    QString text;
    //qDebug() << "begin to read";
    //qDebug() << stream.atEnd();
    if(file.open(QIODevice::ReadOnly))
    {
        while(!stream.atEnd())
        {
            //qDebug() << "read a line";
            line = stream.readLine();
            QByteArray byte_array;
            byte_array = line.toLatin1();
            QJsonParseError json_error;
            QJsonDocument parse_doucment = QJsonDocument::fromJson(byte_array, &json_error);
            if(json_error.error == QJsonParseError::NoError)
            {
                if(parse_doucment.isObject())
                {
                    QJsonObject obj = parse_doucment.object();
                    if(obj.contains("color&r"))//知道这是关于颜色的
                    {
                        QJsonValue value = obj.take("color&r");
                        colorr = value.toVariant().toInt();
                        //qDebug() << "r:" << colorr;
                        if(obj.contains("color&g"))
                        {
                            QJsonValue value = obj.take("color&g");
                            colorg = value.toVariant().toInt();
                            //qDebug() << "g:" << colorg;
                        }
                        if(obj.contains("color&b"))
                        {
                            QJsonValue value = obj.take("color&b");
                            colorb = value.toVariant().toInt();
                            //qDebug() << "b:" << colorb;
                        }
                        if(obj.contains("year"))
                        {
                            QJsonValue value = obj.take("year");
                            year = value.toVariant().toInt();
                        }
                        if(obj.contains("month"))
                        {
                            QJsonValue value = obj.take("month");
                            month = value.toVariant().toInt();
                        }
                        if(obj.contains("day"))
                        {
                            QJsonValue value = obj.take("day");
                            day = value.toVariant().toInt();
                        }
                        cale->paintaColor(QDate(year,month,day),QColor(colorr,colorg,colorb));
                    }
                    if(obj.contains("text"))
                    {
                        QJsonValue value = obj.take("text");
                        text = value.toString();
                        //qDebug() << "text:" << text;
                        if(obj.contains("year"))
                        {
                            QJsonValue value = obj.take("year");
                            year = value.toVariant().toInt();
                        }
                        if(obj.contains("month"))
                        {
                            QJsonValue value = obj.take("month");
                            month = value.toVariant().toInt();
                        }
                        if(obj.contains("day"))
                        {
                            QJsonValue value = obj.take("day");
                            day = value.toVariant().toInt();
                        }
                        cale->addNote1(QDate(year,month,day),text);
                    }
                    if(obj.contains("delete"))
                    {
                        if(obj.contains("year"))
                        {
                            QJsonValue value = obj.take("year");
                            year = value.toVariant().toInt();
                        }
                        if(obj.contains("month"))
                        {
                            QJsonValue value = obj.take("month");
                            month = value.toVariant().toInt();
                        }
                        if(obj.contains("day"))
                        {
                            QJsonValue value = obj.take("day");
                            day = value.toVariant().toInt();
                        }
                        //qDebug() << "here delete sth";
                        cale->deleteNote(QDate(year,month,day));
                    }
                    if(obj.contains("filename"))
                    {
                        QJsonValue value = obj.take("filename");
                        text = value.toString();
                        if(obj.contains("year"))
                        {
                            QJsonValue value = obj.take("year");
                            year = value.toVariant().toInt();
                        }
                        if(obj.contains("month"))
                        {
                            QJsonValue value = obj.take("month");
                            month = value.toVariant().toInt();
                        }
                        if(obj.contains("day"))
                        {
                            QJsonValue value = obj.take("day");
                            day = value.toVariant().toInt();
                        }
                        droparea[day+QDate(year,month,1).dayOfWeek()-1]->nameOffile = text;
                        droparea[day+QDate(year,month,1).dayOfWeek()-1]->setTexts();
                        cale->isdragsth[year-1900][month][day] = true;
                    }
                    if(obj.contains("deletefile"))
                    {
                        if(obj.contains("year"))
                        {
                            QJsonValue value = obj.take("year");
                            year = value.toVariant().toInt();
                        }
                        if(obj.contains("month"))
                        {
                            QJsonValue value = obj.take("month");
                            month = value.toVariant().toInt();
                        }
                        if(obj.contains("day"))
                        {
                            QJsonValue value = obj.take("day");
                            day = value.toVariant().toInt();
                        }
                        droparea[day+QDate(year,month,1).dayOfWeek()-1]->clear();
                        cale->isdragsth[year-1900][month][day] = false;
                    }
                }
            }
        }
    }
    file.close();
}

void MainWindow::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if(event->type() == QEvent::LanguageChange)
    {
        retranslate(this);
    }
}

void MainWindow::retranslate(QWidget *parent)
{
    this->setWindowTitle(QApplication::translate("MainWindow","Calendar Widget"));
    previewGroupBox->setTitle(tr("Preview"));
    //previewGroupBox->setTitle(QApplication::translate("MainWindow","Preview"));
    generalOptionsGroupBox->setTitle(QApplication::translate("MainWindow","Option"));
    addeventButton->setText(QApplication::translate("MainWindow","Add events"));
    deleteeventButton->setText(QApplication::translate("MainWindow","Delete events"));
    paintColor->setText(QApplication::translate("MainWindow","Paint"));
    inportButton->setText(QApplication::translate("MainWindow","Import"));
    exportButton->setText(QApplication::translate("MainWindow","Export"));
    deleteConfig->setText(QApplication::translate("MainWindow","Delete config"));
    changeToChinese->setText(QApplication::translate("MainWindow","Chinese"));
    widgetFixedBox->setText(QApplication::translate("MainWindow","Immobilization"));
    allowToDrag->setText(QApplication::translate("MainWindow","Able to drag"));
}

//**********************************************PRIVATE SLOTS**********************************************//

void MainWindow::changeOpacity(int opa)
{
    winOpacity = (double)opa / 100;
    //cout << winOpacity;
    setWindowOpacity(winOpacity);
}

void MainWindow::changeLanguage(bool ch)
{
    if(ch == true)
    {
        translator = new QTranslator(qApp);
        translator->load(":/Johncalendar_zh");
        qApp->installTranslator(translator);

        cale->setLocale(QLocale("zh_CN"));

        //qDebug() << "change to Chinese";
    }
    if(ch == false)
    {
        qApp->removeTranslator(translator);
        cale->setLocale(QLocale("en"));
        //qDebug() << "change to English";
    }
}

void MainWindow::fixWidget(bool fw)//锁定界面不能拖动且鼠标事件传递至下面
{
    if(fw == true)
    {
        #ifdef Q_OS_LINUX
        XShapeCombineRectangles(QX11Info::display(), winId(), ShapeInput, 0,
        0, NULL, 0, ShapeSet, YXBanded);
        #endif
        #ifdef Q_OS_WIN
        SetWindowLong((HWND)winId(), GWL_EXSTYLE, GetWindowLong(winId(), GWL_EXSTYLE) |
        WS_EX_TRANSPARENT | WS_EX_LAYERED);
        #endif

        returnButton = new QPushButton("Return");
        connect(returnButton, SIGNAL(clicked()), this, SLOT(returnToNormal()));
        returnButton->show();
    }
}

void MainWindow::returnToNormal()
{
    #ifdef Q_OS_WIN
    setWindowLong((HWND)winId(), GWL_EXSTYLE, GetWindowLong((HWND)winId(), GWL_EXSTYLE) & (~WS_EX_TRANSPARENT));
    #endif

    #ifdef Q_OS_LINUX
    XRectangle* myrect = new XRectangle;
    myrect->x = 0;
    myrect->y = 0;
    myrect->width = width();
    myrect->height = height();
    XShapeCombineRectangles(QX11Info::display(), winId(), ShapeInput,
                            0, 0, myrect, 1, ShapeSet, YXBanded);
    #endif

    widgetFixedBox->setChecked(false);
    returnButton->close();
}

void MainWindow::updateIndex(int ye, int mo)
{
    currentyear = ye;
    //qDebug() << currentyear;
    currentmonth = mo;
    firstindex = cale->getindexs(ye,mo);//update the index
    //cout << firstindex << endl;

    for(int i = 0; i < 42; i++)
    {
        droparea[i]->clear();
    }
    for(int i = 1; i <= 31; i++)
    {
        if(QDate(currentyear,currentmonth,i).isValid())
        {
            if(cale->isdragsth[currentyear-1900][currentmonth][i])
            {
                droparea[i+QDate(currentyear,currentmonth,1).dayOfWeek()-1]->setTexts();
            }
        }
    }
}

void MainWindow::addnewevents()//
{
    addeventLayout = new QGridLayout;

    eventsforaweek = new QPushButton(tr("Add events for a day of a week"));
    eventsforamonth = new QPushButton(tr("Add events for a day of a month"));
    eventsinadayofweek = new QPushButton(tr("Add events for a week"));
    eventsinadayofmonth = new QPushButton(tr("Add events for a month"));

    /*此处的QPushButton的名字起得有问题...两边的名字和功能是反的!!!*/

    addeventLayout->addWidget(eventsforaweek,0,0);
    addeventLayout->addWidget(eventsforamonth,1,0);
    addeventLayout->addWidget(eventsinadayofweek,0,1);
    addeventLayout->addWidget(eventsinadayofmonth,1,1);

    colorwin = new QWidget;
    colorwin->setLayout(addeventLayout);
    colorwin->show();

    connect(eventsforaweek, SIGNAL(clicked()), this, SLOT(addweekevent()));
    connect(eventsforamonth, SIGNAL(clicked()), this, SLOT(addmonthevent()));
    connect(eventsinadayofweek, SIGNAL(clicked()), this, SLOT(addaweekdayevent()));
    connect(eventsinadayofmonth, SIGNAL(clicked()), this, SLOT(addamonthdayevent()));
}

void MainWindow::addweekevent()//每周的一天
{
    colorwin->close();

    dateOfadd1 = "Monday";

    QHBoxLayout *layout = new QHBoxLayout;

    QLabel *label = new QLabel(tr("Please choose a day:"));

    weekdayevents = new QComboBox;
    weekdayevents->addItem(tr("Monday"));
    weekdayevents->addItem(tr("Tuesday"));
    weekdayevents->addItem(tr("Wednesday"));
    weekdayevents->addItem(tr("Thursday"));
    weekdayevents->addItem(tr("Friday"));
    weekdayevents->addItem(tr("Saturday"));
    weekdayevents->addItem(tr("Sunday"));

    okButton = new QPushButton(tr("Ok"));

    layout->addWidget(label);
    layout->addWidget(weekdayevents);
    layout->addWidget(okButton);

    colorwin = new QWidget;
    colorwin->setLayout(layout);
    colorwin->show();

    connect(weekdayevents, SIGNAL(currentIndexChanged(QString)), this, SLOT(changechoice1(QString)));
    connect(okButton, SIGNAL(clicked()), this, SLOT(readytoaddnote1()));

}

void MainWindow::addmonthevent()//每月的一天
{
    colorwin->close();

    dateOfadd2 = "1";

    QHBoxLayout *layout = new QHBoxLayout;

    QLabel *label = new QLabel(tr("Please choose a day:"));

    monthdayevents = new QComboBox;
    monthdayevents->addItem(tr("1"));
    monthdayevents->addItem(tr("2"));
    monthdayevents->addItem(tr("3"));
    monthdayevents->addItem(tr("4"));
    monthdayevents->addItem(tr("5"));
    monthdayevents->addItem(tr("6"));
    monthdayevents->addItem(tr("7"));
    monthdayevents->addItem(tr("8"));
    monthdayevents->addItem(tr("9"));
    monthdayevents->addItem(tr("10"));
    monthdayevents->addItem(tr("11"));
    monthdayevents->addItem(tr("12"));
    monthdayevents->addItem(tr("13"));
    monthdayevents->addItem(tr("14"));
    monthdayevents->addItem(tr("15"));
    monthdayevents->addItem(tr("16"));
    monthdayevents->addItem(tr("17"));
    monthdayevents->addItem(tr("18"));
    monthdayevents->addItem(tr("19"));
    monthdayevents->addItem(tr("20"));
    monthdayevents->addItem(tr("21"));
    monthdayevents->addItem(tr("22"));
    monthdayevents->addItem(tr("23"));
    monthdayevents->addItem(tr("24"));
    monthdayevents->addItem(tr("25"));
    monthdayevents->addItem(tr("26"));
    monthdayevents->addItem(tr("27"));
    monthdayevents->addItem(tr("28"));
    monthdayevents->addItem(tr("29"));
    monthdayevents->addItem(tr("30"));
    monthdayevents->addItem(tr("31"));

    okButton = new QPushButton(tr("Ok"));

    layout->addWidget(label);
    layout->addWidget(monthdayevents);
    layout->addWidget(okButton);

    colorwin = new QWidget;
    colorwin->setLayout(layout);
    colorwin->show();

    connect(monthdayevents, SIGNAL(currentIndexChanged(QString)), this, SLOT(changechoice2(QString)));
    connect(okButton, SIGNAL(clicked()), this, SLOT(readytoaddnote2()));
}

void MainWindow::addaweekdayevent()//连续一周
{
    colorwin->close();

    dateOfadd3 = "1";

    QHBoxLayout *layout = new QHBoxLayout;

    QLabel *label = new QLabel(tr("Please choose the first day of the week(It is the day of the current month):"));

    weekevents = new QComboBox;
    weekevents->addItem(tr("1"));
    weekevents->addItem(tr("2"));
    weekevents->addItem(tr("3"));
    weekevents->addItem(tr("4"));
    weekevents->addItem(tr("5"));
    weekevents->addItem(tr("6"));
    weekevents->addItem(tr("7"));
    weekevents->addItem(tr("8"));
    weekevents->addItem(tr("9"));
    weekevents->addItem(tr("10"));
    weekevents->addItem(tr("11"));
    weekevents->addItem(tr("12"));
    weekevents->addItem(tr("13"));
    weekevents->addItem(tr("14"));
    weekevents->addItem(tr("15"));
    weekevents->addItem(tr("16"));
    weekevents->addItem(tr("17"));
    weekevents->addItem(tr("18"));
    weekevents->addItem(tr("19"));
    weekevents->addItem(tr("20"));
    weekevents->addItem(tr("21"));
    weekevents->addItem(tr("22"));
    weekevents->addItem(tr("23"));
    weekevents->addItem(tr("24"));
    weekevents->addItem(tr("25"));
    weekevents->addItem(tr("26"));
    weekevents->addItem(tr("27"));
    weekevents->addItem(tr("28"));
    weekevents->addItem(tr("29"));
    weekevents->addItem(tr("30"));
    weekevents->addItem(tr("31"));

    okButton = new QPushButton(tr("Ok"));

    layout->addWidget(label);
    layout->addWidget(weekevents);
    layout->addWidget(okButton);

    colorwin = new QWidget;
    colorwin->setLayout(layout);
    colorwin->show();

    connect(weekevents, SIGNAL(currentIndexChanged(QString)), this, SLOT(changechoice3(QString)));
    connect(okButton, SIGNAL(clicked()), this, SLOT(readytoaddnote3()));
}

void MainWindow::addamonthdayevent()//连续一个月
{
    colorwin->close();

    dateOfadd4 = "1";

    QHBoxLayout *layout = new QHBoxLayout;

    QLabel *label = new QLabel(tr("Please choose the first day of the month(It is the day of the current month):"));

    monthevents = new QComboBox;
    monthevents->addItem(tr("1"));
    monthevents->addItem(tr("2"));
    monthevents->addItem(tr("3"));
    monthevents->addItem(tr("4"));
    monthevents->addItem(tr("5"));
    monthevents->addItem(tr("6"));
    monthevents->addItem(tr("7"));
    monthevents->addItem(tr("8"));
    monthevents->addItem(tr("9"));
    monthevents->addItem(tr("10"));
    monthevents->addItem(tr("11"));
    monthevents->addItem(tr("12"));
    monthevents->addItem(tr("13"));
    monthevents->addItem(tr("14"));
    monthevents->addItem(tr("15"));
    monthevents->addItem(tr("16"));
    monthevents->addItem(tr("17"));
    monthevents->addItem(tr("18"));
    monthevents->addItem(tr("19"));
    monthevents->addItem(tr("20"));
    monthevents->addItem(tr("21"));
    monthevents->addItem(tr("22"));
    monthevents->addItem(tr("23"));
    monthevents->addItem(tr("24"));
    monthevents->addItem(tr("25"));
    monthevents->addItem(tr("26"));
    monthevents->addItem(tr("27"));
    monthevents->addItem(tr("28"));
    monthevents->addItem(tr("29"));
    monthevents->addItem(tr("30"));
    monthevents->addItem(tr("31"));

    okButton = new QPushButton(tr("Ok"));

    layout->addWidget(label);
    layout->addWidget(monthevents);
    layout->addWidget(okButton);

    colorwin = new QWidget;
    colorwin->setLayout(layout);
    colorwin->show();

    connect(monthevents, SIGNAL(currentIndexChanged(QString)), this, SLOT(changechoice4(QString)));
    connect(okButton, SIGNAL(clicked()), this, SLOT(readytoaddnote4()));
}

void MainWindow::changechoice1(QString cha)
{
    dateOfadd1 = cha;
}

void MainWindow::readytoaddnote1()
{
    colorwin->close();

    QString memo = QInputDialog::getText( cale, "Events", "Description:" );

    int dayweek;

    if(!memo.isEmpty())
    {
        if(dateOfadd1 == "Monday")
            dayweek = 1;
        else if(dateOfadd1 == "Tuesday")
            dayweek = 2;
        else if(dateOfadd1 == "Wednesday")
            dayweek = 3;
        else if(dateOfadd1 == "Thursday")
            dayweek = 4;
        else if(dateOfadd1 == "Friday")
            dayweek = 5;
        else if(dateOfadd1 == "Saturday")
            dayweek = 6;
        else if(dateOfadd1 == "Sunday")
            dayweek = 7;

        for(int ye = 1900; ye <= 3000; ye++)
        {
            for(int mon = 1; mon <= 12; mon++)
            {
                for(int da = 1; da <= 31; da++)
                {
                    if(QDate(ye,mon,da).isValid() && QDate(ye,mon,da).dayOfWeek() == dayweek)
                    {
                        cale->addNote1(QDate(ye,mon,da), memo);
                    }
                }
            }
        }
    }
}

void MainWindow::changechoice2(QString cha)
{
    dateOfadd2 = cha;
}

void MainWindow::readytoaddnote2()
{
    colorwin->close();

    QString memo = QInputDialog::getText( cale, "Events", "Description:" );

    int i = dateOfadd2.toInt();

    for(int ye = 1900; ye <= 3000; ye++)
    {
        for(int mon = 1; mon <= 12; mon++)
        {
            if(QDate(ye,mon,i).isValid())
            {
                cale->addNote1(QDate(ye,mon,i), memo);
            }
        }
    }

}

void MainWindow::changechoice3(QString cha)
{
    dateOfadd3 = cha;
}

void MainWindow::readytoaddnote3()
{
    colorwin->close();

    QString memo = QInputDialog::getText( cale, "Events", "Description:" );

    int weekday = 7;
    int i = dateOfadd3.toInt();
    bool kuayue = false;
    bool kuanian = false;

    while(weekday > 0)
    {
        int jia = 7 - weekday;
        if(QDate(currentyear,currentmonth,i+jia).isValid() && !kuayue && !kuanian)
            cale->addNote1(QDate(currentyear,currentmonth,i+jia), memo);
        else if(!QDate(currentyear,currentmonth,i+jia).isValid() && !kuayue && !kuanian)
        {
            i = 1 - jia;
            if(currentmonth <= 11)
            {
                kuayue = true;
                continue;
            }
            else if(currentmonth == 12)
            {
                kuanian = true;
                continue;
            }
        }
        else if(QDate(currentyear,currentmonth+1,i+jia).isValid() && kuayue && !kuanian)
            cale->addNote1(QDate(currentyear,currentmonth+1,i+jia), memo);
        else if(QDate(currentyear+1,1,i+jia).isValid() && !kuayue && kuanian)
            cale->addNote1(QDate(currentyear+1,1,i+jia),memo);
        weekday--;
    }
}

void MainWindow::changechoice4(QString cha)
{
    dateOfadd4 = cha;
}

void MainWindow::readytoaddnote4()
{
    colorwin->close();

    QString memo = QInputDialog::getText( cale, "Events", "Description:" );

    int monthday = 28;
    bool kuayue = false;
    bool kuanian = false;
    int i = dateOfadd4.toInt();
    if(i == 1)
    {
        for(int m = 1; m <= 31; m++)
        {
            if(QDate(currentyear,currentmonth,m).isValid())
                cale->addNote1(QDate(currentyear,currentmonth,m),memo);
            else
                return;
        }
    }
    else
    {
        while(monthday > 0)
        {
            int jia = 28 - monthday;
            if(QDate(currentyear,currentmonth,i+jia).isValid() && !kuayue && !kuanian)
                cale->addNote1(QDate(currentyear,currentmonth,i+jia), memo);
            else if(!QDate(currentyear,currentmonth,i+jia).isValid() && !kuayue && !kuanian)
            {
                i = 1 - jia;
                if(currentmonth <= 11)
                {
                    kuayue = true;
                    continue;
                }
                else if(currentmonth == 12)
                {
                    kuanian = true;
                    continue;
                }
            }
            else if(QDate(currentyear,currentmonth+1,i+jia).isValid() && kuayue && !kuanian)
                cale->addNote1(QDate(currentyear,currentmonth+1,i+jia), memo);
            else if(QDate(currentyear+1,1,i+jia).isValid() && !kuayue && kuanian)
                cale->addNote1(QDate(currentyear+1,1,i+jia),memo);
            monthday--;
        }
    }
}

void MainWindow::deleteevents()
{
    deleteeventLayout = new QGridLayout;

    deleteadayofweek = new QPushButton(tr("Delete events for a day of a week"));
    deleteadayofmonth = new QPushButton(tr("Delete events for a day of a month"));
    deleteaweek = new QPushButton(tr("Delete events for a week"));
    deleteamonth = new QPushButton(tr("Delete events for a month"));

    deleteeventLayout->addWidget(deleteadayofweek,0,0);
    deleteeventLayout->addWidget(deleteadayofmonth,1,0);
    deleteeventLayout->addWidget(deleteaweek,0,1);
    deleteeventLayout->addWidget(deleteamonth,1,1);

    colorwin = new QWidget;
    colorwin->setLayout(deleteeventLayout);
    colorwin->show();

    connect(deleteadayofweek, SIGNAL(clicked()), this, SLOT(deleteaweekdayevent()));
    connect(deleteadayofmonth, SIGNAL(clicked()), this, SLOT(deleteamonthdayevent()));
    connect(deleteaweek, SIGNAL(clicked()), this, SLOT(deleteweekdayevent()));
    connect(deleteamonth, SIGNAL(clicked()), this, SLOT(deletemonthdayevent()));
}

void MainWindow::deleteaweekdayevent()
{
    colorwin->close();

    dateOfadd1 = "Monday";

    QHBoxLayout *layout = new QHBoxLayout;

    QLabel *label = new QLabel(tr("Please choose a day:"));

    weekdayevents = new QComboBox;
    weekdayevents->addItem(tr("Monday"));
    weekdayevents->addItem(tr("Tuesday"));
    weekdayevents->addItem(tr("Wednesday"));
    weekdayevents->addItem(tr("Thursday"));
    weekdayevents->addItem(tr("Friday"));
    weekdayevents->addItem(tr("Saturday"));
    weekdayevents->addItem(tr("Sunday"));

    okButton = new QPushButton(tr("Ok"));

    layout->addWidget(label);
    layout->addWidget(weekdayevents);
    layout->addWidget(okButton);

    colorwin = new QWidget;
    colorwin->setLayout(layout);
    colorwin->show();

    connect(weekdayevents, SIGNAL(currentIndexChanged(QString)), this, SLOT(changechoice1(QString)));
    connect(okButton, SIGNAL(clicked()), this, SLOT(readytodeletenote1()));
}

void MainWindow::deleteamonthdayevent()
{
    colorwin->close();

    dateOfadd2 = "1";

    QHBoxLayout *layout = new QHBoxLayout;

    QLabel *label = new QLabel(tr("Please choose a day:"));

    monthdayevents = new QComboBox;
    monthdayevents->addItem(tr("1"));
    monthdayevents->addItem(tr("2"));
    monthdayevents->addItem(tr("3"));
    monthdayevents->addItem(tr("4"));
    monthdayevents->addItem(tr("5"));
    monthdayevents->addItem(tr("6"));
    monthdayevents->addItem(tr("7"));
    monthdayevents->addItem(tr("8"));
    monthdayevents->addItem(tr("9"));
    monthdayevents->addItem(tr("10"));
    monthdayevents->addItem(tr("11"));
    monthdayevents->addItem(tr("12"));
    monthdayevents->addItem(tr("13"));
    monthdayevents->addItem(tr("14"));
    monthdayevents->addItem(tr("15"));
    monthdayevents->addItem(tr("16"));
    monthdayevents->addItem(tr("17"));
    monthdayevents->addItem(tr("18"));
    monthdayevents->addItem(tr("19"));
    monthdayevents->addItem(tr("20"));
    monthdayevents->addItem(tr("21"));
    monthdayevents->addItem(tr("22"));
    monthdayevents->addItem(tr("23"));
    monthdayevents->addItem(tr("24"));
    monthdayevents->addItem(tr("25"));
    monthdayevents->addItem(tr("26"));
    monthdayevents->addItem(tr("27"));
    monthdayevents->addItem(tr("28"));
    monthdayevents->addItem(tr("29"));
    monthdayevents->addItem(tr("30"));
    monthdayevents->addItem(tr("31"));

    okButton = new QPushButton(tr("Ok"));

    layout->addWidget(label);
    layout->addWidget(monthdayevents);
    layout->addWidget(okButton);

    colorwin = new QWidget;
    colorwin->setLayout(layout);
    colorwin->show();

    connect(monthdayevents, SIGNAL(currentIndexChanged(QString)), this, SLOT(changechoice2(QString)));
    connect(okButton, SIGNAL(clicked()), this, SLOT(readytodeletenote2()));
}

void MainWindow::deleteweekdayevent()
{
    colorwin->close();

    dateOfadd3 = "1";

    QHBoxLayout *layout = new QHBoxLayout;

    QLabel *label = new QLabel(tr("Please choose the first day of the week(It is the day of the current month):"));

    weekevents = new QComboBox;
    weekevents->addItem(tr("1"));
    weekevents->addItem(tr("2"));
    weekevents->addItem(tr("3"));
    weekevents->addItem(tr("4"));
    weekevents->addItem(tr("5"));
    weekevents->addItem(tr("6"));
    weekevents->addItem(tr("7"));
    weekevents->addItem(tr("8"));
    weekevents->addItem(tr("9"));
    weekevents->addItem(tr("10"));
    weekevents->addItem(tr("11"));
    weekevents->addItem(tr("12"));
    weekevents->addItem(tr("13"));
    weekevents->addItem(tr("14"));
    weekevents->addItem(tr("15"));
    weekevents->addItem(tr("16"));
    weekevents->addItem(tr("17"));
    weekevents->addItem(tr("18"));
    weekevents->addItem(tr("19"));
    weekevents->addItem(tr("20"));
    weekevents->addItem(tr("21"));
    weekevents->addItem(tr("22"));
    weekevents->addItem(tr("23"));
    weekevents->addItem(tr("24"));
    weekevents->addItem(tr("25"));
    weekevents->addItem(tr("26"));
    weekevents->addItem(tr("27"));
    weekevents->addItem(tr("28"));
    weekevents->addItem(tr("29"));
    weekevents->addItem(tr("30"));
    weekevents->addItem(tr("31"));

    okButton = new QPushButton(tr("Ok"));

    layout->addWidget(label);
    layout->addWidget(weekevents);
    layout->addWidget(okButton);

    colorwin = new QWidget;
    colorwin->setLayout(layout);
    colorwin->show();

    connect(weekevents, SIGNAL(currentIndexChanged(QString)), this, SLOT(changechoice3(QString)));
    connect(okButton, SIGNAL(clicked()), this, SLOT(readytodeletenote3()));
}

void MainWindow::deletemonthdayevent()
{
    colorwin->close();

    dateOfadd4 = "1";

    QHBoxLayout *layout = new QHBoxLayout;

    QLabel *label = new QLabel(tr("Please choose the first day of the month(It is the day of the current month):"));

    monthevents = new QComboBox;
    monthevents->addItem(tr("1"));
    monthevents->addItem(tr("2"));
    monthevents->addItem(tr("3"));
    monthevents->addItem(tr("4"));
    monthevents->addItem(tr("5"));
    monthevents->addItem(tr("6"));
    monthevents->addItem(tr("7"));
    monthevents->addItem(tr("8"));
    monthevents->addItem(tr("9"));
    monthevents->addItem(tr("10"));
    monthevents->addItem(tr("11"));
    monthevents->addItem(tr("12"));
    monthevents->addItem(tr("13"));
    monthevents->addItem(tr("14"));
    monthevents->addItem(tr("15"));
    monthevents->addItem(tr("16"));
    monthevents->addItem(tr("17"));
    monthevents->addItem(tr("18"));
    monthevents->addItem(tr("19"));
    monthevents->addItem(tr("20"));
    monthevents->addItem(tr("21"));
    monthevents->addItem(tr("22"));
    monthevents->addItem(tr("23"));
    monthevents->addItem(tr("24"));
    monthevents->addItem(tr("25"));
    monthevents->addItem(tr("26"));
    monthevents->addItem(tr("27"));
    monthevents->addItem(tr("28"));
    monthevents->addItem(tr("29"));
    monthevents->addItem(tr("30"));
    monthevents->addItem(tr("31"));

    okButton = new QPushButton(tr("Ok"));

    layout->addWidget(label);
    layout->addWidget(monthevents);
    layout->addWidget(okButton);

    colorwin = new QWidget;
    colorwin->setLayout(layout);
    colorwin->show();

    connect(monthevents, SIGNAL(currentIndexChanged(QString)), this, SLOT(changechoice4(QString)));
    connect(okButton, SIGNAL(clicked()), this, SLOT(readytodeletenote4()));
}

void MainWindow::readytodeletenote1()
{
    colorwin->close();

    int dayweek;

    if(dateOfadd1 == "Monday")
        dayweek = 1;
    else if(dateOfadd1 == "Tuesday")
        dayweek = 2;
    else if(dateOfadd1 == "Wednesday")
        dayweek = 3;
    else if(dateOfadd1 == "Thursday")
        dayweek = 4;
    else if(dateOfadd1 == "Friday")
        dayweek = 5;
    else if(dateOfadd1 == "Saturday")
        dayweek = 6;
    else if(dateOfadd1 == "Sunday")
        dayweek = 7;

    for(int ye = 1900; ye <= 3000; ye++)
    {
        for(int mon = 1; mon <= 12; mon++)
        {
            for(int da = 1; da <= 31; da++)
            {
                if(QDate(ye,mon,da).isValid() && QDate(ye,mon,da).dayOfWeek() == dayweek)
                {
                    cale->deleteNote(QDate(ye,mon,da));
                }
            }
        }
    }
}

void MainWindow::readytodeletenote2()
{
    colorwin->close();

    int i = dateOfadd2.toInt();

    for(int ye = 1900; ye <= 3000; ye++)
    {
        for(int mon = 1; mon <= 12; mon++)
        {
            if(QDate(ye,mon,i).isValid())
            {
                cale->deleteNote(QDate(ye,mon,i));
            }
        }
    }
}

void MainWindow::readytodeletenote3()
{
    colorwin->close();

    int weekday = 7;
    int i = dateOfadd3.toInt();
    bool kuayue = false;
    bool kuanian = false;

    while(weekday > 0)
    {
        int jia = 7 - weekday;
        if(QDate(currentyear,currentmonth,i+jia).isValid() && !kuayue && !kuanian)
            cale->deleteNote(QDate(currentyear,currentmonth,i+jia));
        else if(!QDate(currentyear,currentmonth,i+jia).isValid() && !kuayue && !kuanian)
        {
            i = 1 - jia;
            if(currentmonth <= 11)
            {
                kuayue = true;
                continue;
            }
            else if(currentmonth == 12)
            {
                kuanian = true;
                continue;
            }
        }
        else if(QDate(currentyear,currentmonth+1,i+jia).isValid() && kuayue && !kuanian)
            cale->deleteNote(QDate(currentyear,currentmonth+1,i+jia));
        else if(QDate(currentyear+1,1,i+jia).isValid() && !kuayue && kuanian)
            cale->deleteNote(QDate(currentyear+1,1,i+jia));
        weekday--;
    }
}

void MainWindow::readytodeletenote4()
{
    colorwin->close();

    int monthday = 28;
    bool kuayue = false;
    bool kuanian = false;
    int i = dateOfadd4.toInt();
    if(i == 1)
    {
        for(int m = 1; m <= 31; m++)
        {
            if(QDate(currentyear,currentmonth,m).isValid())
                cale->deleteNote(QDate(currentyear,currentmonth,m));
            else
                return;
        }
    }
    else
    {
        while(monthday > 0)
        {
            int jia = 28 - monthday;
            if(QDate(currentyear,currentmonth,i+jia).isValid() && !kuayue && !kuanian)
                cale->deleteNote(QDate(currentyear,currentmonth,i+jia));
            else if(!QDate(currentyear,currentmonth,i+jia).isValid() && !kuayue && !kuanian)
            {
                i = 1 - jia;
                if(currentmonth <= 11)
                {
                    kuayue = true;
                    continue;
                }
                else if(currentmonth == 12)
                {
                    kuanian = true;
                    continue;
                }
            }
            else if(QDate(currentyear,currentmonth+1,i+jia).isValid() && kuayue && !kuanian)
                cale->deleteNote(QDate(currentyear,currentmonth+1,i+jia));
            else if(QDate(currentyear+1,1,i+jia).isValid() && !kuayue && kuanian)
                cale->deleteNote(QDate(currentyear+1,1,i+jia));
            monthday--;
        }
    }
}

void MainWindow::paintOneDay()
{
    QHBoxLayout *clayout = new QHBoxLayout;

    paintaDay = new QComboBox;
    paintaDay->addItem(tr("1"));
    paintaDay->addItem(tr("2"));
    paintaDay->addItem(tr("3"));
    paintaDay->addItem(tr("4"));
    paintaDay->addItem(tr("5"));
    paintaDay->addItem(tr("6"));
    paintaDay->addItem(tr("7"));
    paintaDay->addItem(tr("8"));
    paintaDay->addItem(tr("9"));
    paintaDay->addItem(tr("10"));
    paintaDay->addItem(tr("11"));
    paintaDay->addItem(tr("12"));
    paintaDay->addItem(tr("13"));
    paintaDay->addItem(tr("14"));
    paintaDay->addItem(tr("15"));
    paintaDay->addItem(tr("16"));
    paintaDay->addItem(tr("17"));
    paintaDay->addItem(tr("18"));
    paintaDay->addItem(tr("19"));
    paintaDay->addItem(tr("20"));
    paintaDay->addItem(tr("21"));
    paintaDay->addItem(tr("22"));
    paintaDay->addItem(tr("23"));
    paintaDay->addItem(tr("24"));
    paintaDay->addItem(tr("25"));
    paintaDay->addItem(tr("26"));
    paintaDay->addItem(tr("27"));
    paintaDay->addItem(tr("28"));
    paintaDay->addItem(tr("29"));
    paintaDay->addItem(tr("30"));
    paintaDay->addItem(tr("31"));

    paintaColor = new QComboBox;
    paintaColor->addItem(tr("Yellow"));
    paintaColor->addItem(tr("Red"));
    paintaColor->addItem(tr("Blue"));
    paintaColor->addItem(tr("Pink"));
    paintaColor->addItem(tr("Cyan"));
    paintaColor->addItem(tr("Green"));
    paintaColor->addItem(tr("White"));
    paintaColor->addItem(tr("Black"));

    okButton = new QPushButton(tr("Ok"));

    clayout->addWidget(paintaDay);
    clayout->addWidget(paintaColor);
    clayout->addWidget(okButton);

    colorwin = new QWidget;
    colorwin->setLayout(clayout);
    colorwin->show();

    connect(paintaDay, SIGNAL(currentIndexChanged(QString)), this, SLOT(changePaint(QString)));
    connect(paintaColor, SIGNAL(currentIndexChanged(QString)), this, SLOT(changePaint(QString)));
    connect(okButton, SIGNAL(clicked()), this, SLOT(readyToPaint()));

}

void MainWindow::changePaint(QString cha)
{
    if(cha.size() <= 2)
    {
        thechoosendate = cha.toInt();
    }
    else
    {
        if(cha == "Yellow")
            thechoosencolor = 0;
        else if(cha == "Red" )
            thechoosencolor = 1;
        else if(cha == "Blue")
            thechoosencolor = 2;
        else if(cha == "Pink")
            thechoosencolor = 3;
        else if(cha == "Cyan")
            thechoosencolor = 4;
        else if(cha == "Green")
            thechoosencolor = 5;
        else if(cha == "White")
            thechoosencolor = 6;
        else if(cha == "Black")
            thechoosencolor = 7;
    }
}

void MainWindow::readyToPaint()
{
    if(QDate(currentyear,currentmonth,thechoosendate).isValid())
        cale->paintaColor(QDate(currentyear,currentmonth,thechoosendate), colortobe[thechoosencolor]);
}

void MainWindow::exportTheCal()
{
    cale->storeTheCal();
}

void MainWindow::on_openFile_clicked()
{
    QFileDialog fd;
    QStringList filters;
    filters << tr("JSON file (*.json)")
            << tr("All files (*)");
    fd.setNameFilters(filters);
    fd.setWindowTitle(tr("Import"));
    fd.setFileMode(QFileDialog::ExistingFile);
    fd.setAcceptMode(QFileDialog::AcceptOpen);
    if(fd.exec())
    {
        importFile(fd.selectedFiles()[0]);
    }

}

void MainWindow::allowDragging(bool dra)
{
    if(dra == true)
    {
        for(int i = 0; i < 42; i++)
        {
            droparea[i]->setAcceptDrops(true);
            droparea[i]->setAttribute(Qt::WA_TransparentForMouseEvents,false);
        }
    }
    if(dra == false)
    {
        for(int i = 0; i < 42; i++)
        {
            //droparea[i]->setAcceptDrops(false);
            droparea[i]->setAttribute(Qt::WA_TransparentForMouseEvents);
            //qDebug() << "Unable to drag";
        }
    }
}

void MainWindow::deleteTheConfig()
{
    QLabel *label = new QLabel(tr("Are you sure??"));
    okButton = new QPushButton(tr("Ok"));

    QHBoxLayout *layouts = new QHBoxLayout;
    layouts->addWidget(label);
    layouts->addWidget(okButton);

    colorwin = new QWidget;
    colorwin->setLayout(layouts);
    colorwin->show();

    connect(okButton, SIGNAL(clicked()), this, SLOT(on_okButton_clicked()));
}

void MainWindow::on_okButton_clicked()
{
    colorwin->close();
    QFile::remove(QApplication::applicationDirPath()+"/config.json");
    cale->removeTheSource();
}

//********************************************************* PRIVATE *********************************************************//

void MainWindow::createPreviewGroupBox()
{
    previewGroupBox = new QGroupBox(tr("Preview"));


    cale->setMinimumDate(QDate(1900,1,1));
    cale->setMaximumDate(QDate(3000,1,1));
    cale->setGridVisible(true);

    connect(cale, SIGNAL(currentPageChanged(int,int)), this, SLOT(updateIndex(int,int)));

    previewLayout = new QGridLayout;
    previewLayout->addWidget(cale,0,0,1000,100);

    for(int i = 0; i < 42; i++)
    {
        droparea[i] = new dragthings;
        droparea[i]->installEventFilter(this);
    }
    previewLayout->addWidget(droparea[0],992,60,1,1);
    previewLayout->addWidget(droparea[1],992,65,1,1);
    previewLayout->addWidget(droparea[2],992,70,1,1);
    previewLayout->addWidget(droparea[3],992,75,1,1);
    previewLayout->addWidget(droparea[4],992,80,1,1);
    previewLayout->addWidget(droparea[5],992,85,1,1);
    previewLayout->addWidget(droparea[6],992,90,1,1);

    previewLayout->addWidget(droparea[7],993,60,1,1);
    previewLayout->addWidget(droparea[8],993,65,1,1);
    previewLayout->addWidget(droparea[9],993,70,1,1);
    previewLayout->addWidget(droparea[10],993,75,1,1);
    previewLayout->addWidget(droparea[11],993,80,1,1);
    previewLayout->addWidget(droparea[12],993,85,1,1);
    previewLayout->addWidget(droparea[13],993,90,1,1);

    previewLayout->addWidget(droparea[14],994,60,1,1);
    previewLayout->addWidget(droparea[15],994,65,1,1);
    previewLayout->addWidget(droparea[16],994,70,1,1);
    previewLayout->addWidget(droparea[17],994,75,1,1);
    previewLayout->addWidget(droparea[18],994,80,1,1);
    previewLayout->addWidget(droparea[19],994,85,1,1);
    previewLayout->addWidget(droparea[20],994,90,1,1);

    previewLayout->addWidget(droparea[21],996,60,1,1);
    previewLayout->addWidget(droparea[22],996,65,1,1);
    previewLayout->addWidget(droparea[23],996,70,1,1);
    previewLayout->addWidget(droparea[24],996,75,1,1);
    previewLayout->addWidget(droparea[25],996,80,1,1);
    previewLayout->addWidget(droparea[26],996,85,1,1);
    previewLayout->addWidget(droparea[27],996,90,1,1);

    previewLayout->addWidget(droparea[28],997,60,1,1);
    previewLayout->addWidget(droparea[29],997,65,1,1);
    previewLayout->addWidget(droparea[30],997,70,1,1);
    previewLayout->addWidget(droparea[31],997,75,1,1);
    previewLayout->addWidget(droparea[32],997,80,1,1);
    previewLayout->addWidget(droparea[33],997,85,1,1);
    previewLayout->addWidget(droparea[34],997,90,1,1);

    previewLayout->addWidget(droparea[35],998,60,1,1);
    previewLayout->addWidget(droparea[36],998,65,1,1);
    previewLayout->addWidget(droparea[37],998,70,1,1);
    previewLayout->addWidget(droparea[38],998,75,1,1);
    previewLayout->addWidget(droparea[39],998,80,1,1);
    previewLayout->addWidget(droparea[40],998,85,1,1);
    previewLayout->addWidget(droparea[41],998,90,1,1);

    previewGroupBox->setLayout(previewLayout);
}

void MainWindow::createGeneralOptionsGroupBox()
{
    generalOptionsGroupBox = new QGroupBox(tr("Option"));

    opacitySlider = new QSlider(Qt::Horizontal);
    addeventButton = new QPushButton(tr("Add events"));
    deleteeventButton = new QPushButton(tr("Delete events"));
    paintColor = new QPushButton(tr("Paint"));
    inportButton = new QPushButton(tr("Import"));
    exportButton = new QPushButton(tr("Export"));
    deleteConfig = new QPushButton(tr("Delete config"));
    changeToChinese = new QCheckBox(tr("Chinese"));
    changeToChinese->setChecked(false);
    widgetFixedBox = new QCheckBox(tr("Immobilization"));
    widgetFixedBox->setChecked(false);
    allowToDrag = new QCheckBox("Able to drag");
    allowToDrag->setChecked(true);

    generalOptionLayout = new QHBoxLayout;
    generalOptionLayout->addWidget(opacitySlider);
    generalOptionLayout->addWidget(addeventButton);
    generalOptionLayout->addWidget(deleteeventButton);
    generalOptionLayout->addWidget(paintColor);
    generalOptionLayout->addWidget(inportButton);
    generalOptionLayout->addWidget(exportButton);
    generalOptionLayout->addWidget(deleteConfig);
    generalOptionLayout->addWidget(changeToChinese);
    generalOptionLayout->addWidget(widgetFixedBox);
    generalOptionLayout->addWidget(allowToDrag);
    generalOptionsGroupBox->setLayout(generalOptionLayout);
    generalOptionsGroupBox->setMaximumHeight(80);

    setSlider();

    connect(addeventButton, SIGNAL(clicked()), this, SLOT(addnewevents()));
    connect(deleteeventButton, SIGNAL(clicked()), this, SLOT(deleteevents()));
    connect(paintColor, SIGNAL(clicked()), this, SLOT(paintOneDay()));
    connect(inportButton, SIGNAL(clicked()), this, SLOT(on_openFile_clicked()));
    connect(exportButton, SIGNAL(clicked()), this, SLOT(exportTheCal()));
    connect(deleteConfig, SIGNAL(clicked()), this, SLOT(deleteTheConfig()));
    connect(changeToChinese, SIGNAL(toggled(bool)), this, SLOT(changeLanguage(bool)));
    connect(widgetFixedBox, SIGNAL(toggled(bool)), this, SLOT(fixWidget(bool)));
    connect(allowToDrag, SIGNAL(toggled(bool)), this, SLOT(allowDragging(bool)));
}

