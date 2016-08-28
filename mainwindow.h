#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include "calendar.h"
#include "dragthings.h"
#include "storage.h"
#include <QDesktopWidget>
#include <QPushButton>
#include <QToolButton>
#include <QtWidgets>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QSignalMapper>
#include <QColor>
#include <QComboBox>
#include <QLineEdit>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>
#include <fstream>
#include <string>
#include <QTextStream>
#include <QJsonParseError>
#include <QFile>
#include <QTranslator>
#include <QEvent>

class QPushButton;
class QGridLayout;
class QVBoxLayout;
class QCalendarWidget;
class dragthings;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setSlider();//设置调透明度的
    void settheColor();//设置颜色数组
    bool eventFilter(QObject *, QEvent *);//事件过滤器
    void updatefilename();//拖拽文件完成后更新日历中的note为文件名
    void importFile(QString filename);//导入文件,进行设置
    void changeEvent(QEvent *event);//文件过滤器,过滤出语言变更这个事件
    void retranslate(QWidget *parent);//在变更语言后重新加载主窗口语言

    double winOpacity;  //窗口的透明度
    int theindextoaddfilename;  //droparea时经过计算的对应日期

    calendar *cale;  //日期指针

private slots:
    void changeOpacity(int opa);//改变窗口透明度
    void changeLanguage(bool ch);//更改语言
    void fixWidget(bool fw);//窗口固定
    void returnToNormal();//取消固定
    void updateIndex(int ye, int mo);//更新当前年,月和拖拽情况
    void addnewevents();//增加新事件:下面是4种添加方式
    void addweekevent();
    void addmonthevent();
    void addaweekdayevent();
    void addamonthdayevent();
    void changechoice1(QString cha);//选项的更改和批量增加事件
    void readytoaddnote1();
    void changechoice2(QString cha);
    void readytoaddnote2();
    void changechoice3(QString cha);
    void readytoaddnote3();
    void changechoice4(QString cha);
    void readytoaddnote4();
    void deleteevents();//删除事件:下面是4中删除方式
    void deleteaweekdayevent();
    void deleteamonthdayevent();
    void deleteweekdayevent();
    void deletemonthdayevent();
    void readytodeletenote1();//批量删除事件
    void readytodeletenote2();
    void readytodeletenote3();
    void readytodeletenote4();
    void paintOneDay();//画色的窗口
    void changePaint(QString cha);//改变选项
    void readyToPaint();//上色
    void exportTheCal();//引用cale保存
    void on_openFile_clicked();//打开文件的对话框
    void allowDragging(bool dra);//是否允许拖拽
    void deleteTheConfig();//删除config
    void on_okButton_clicked();//确定删除config


private:
    void createPreviewGroupBox();//创建日历框
    void createGeneralOptionsGroupBox();//创建选项框

    QGroupBox *previewGroupBox;
    QGroupBox *generalOptionsGroupBox;

    QSlider *opacitySlider;

    QPushButton *addeventButton;
    QPushButton *deleteeventButton;
    QPushButton *paintColor;
    QPushButton *inportButton;
    QPushButton *exportButton;
    QPushButton *widgetFixedButton;
    QPushButton *setOption;
    QPushButton *eventsforaweek;//下面4个按钮意思和字面是反着的
    QPushButton *eventsforamonth;
    QPushButton *eventsinadayofweek;
    QPushButton *eventsinadayofmonth;
    QPushButton *deleteadayofweek;
    QPushButton *deleteadayofmonth;
    QPushButton *deleteaweek;
    QPushButton *deleteamonth;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QPushButton *backtonor;
    QPushButton *deleteConfig;
    QPushButton *returnButton;

    QToolButton *colors[8];

    QGridLayout *previewLayout;
    QGridLayout *addeventLayout;
    QGridLayout *deleteeventLayout;

    QHBoxLayout *generalOptionLayout;

    QVBoxLayout *layout;

    QDateEdit *currentDateEdit;
    QDateEdit *minimumDateEdit;
    QDateEdit *maximumDateEdit;

    QCheckBox *firstFridayCheckBox;
    QCheckBox *mayFirstCheckBox;
    QCheckBox *widgetFixedBox;
    QCheckBox *changeToChinese;
    QCheckBox *allowToDrag;

    QDate datea;

    QColor colortobe[8];

    QWidget *colorwin;
    QWidget *dragmode;
    QWidget *backtonormal;

    QComboBox *weekdayevents;
    QComboBox *monthdayevents;
    QComboBox *weekevents;
    QComboBox *monthevents;
    QComboBox *paintaDay;
    QComboBox *paintaColor;

    QTranslator *translator;

    QString dateOfadd1;
    QString dateOfadd2;
    QString dateOfadd3;
    QString dateOfadd4;

    dragthings *droparea[42];

    int firstindex;
    int currentyear;
    int currentmonth;
    int thechoosendate = 1;
    int thechoosencolor = 0;
};

#endif // MAINWINDOW_H
