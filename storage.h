#ifndef STORAGE_H
#define STORAGE_H

#include <fstream>
#include <string>
#include <QColor>
#include <QDate>
#include <QString>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>
#include <QDebug>
#include <QFileInfo>
#include <QDir>

using namespace std;

class Storage
{
    //Q_OBJECT

public:
    Storage();
    ~Storage();
    void storeToFile(QDate date, QString str);//判定是不是文件以存储文件
    void storeToFile(QDate date, QColor color);//存储文件的5中格式,文字,颜色,删除,拖拽,删除拖拽
    void storeToFile(QDate date);
    void storeToFile1(QDate date, QString str);
    void storeToFile1(QDate date);
    void exportFile(QString filename);//输出文件
    QStringList json_str ;

private slots:


private:
    string configpath;

};

static Storage source ;//在cale中调用的source,用来保存


#endif // STORAGE_H
