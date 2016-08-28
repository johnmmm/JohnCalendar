#include "storage.h"


Storage::Storage()
{

}

Storage::~Storage()
{

}

void Storage::storeToFile(QDate date, QString str)
{
    QJsonObject json;
    json.insert("year",date.year());
    json.insert("month",date.month());
    json.insert("day",date.day());
    json.insert("text",str);
    QJsonDocument document;
    document.setObject(json);
    QByteArray byte_array = document.toJson(QJsonDocument::Compact);
    QString str1(byte_array);
    json_str.push_back(str1);
//    qDebug() << "store the str";
//    qDebug() << json_str.size();
}

void Storage::storeToFile(QDate date, QColor color)
{
    QJsonObject json;
    json.insert("year",date.year());
    json.insert("month",date.month());
    json.insert("day",date.day());
    json.insert("color&r",color.red());
    json.insert("color&g",color.green());
    json.insert("color&b",color.blue());
    QJsonDocument document;
    document.setObject(json);
    QByteArray byte_array = document.toJson(QJsonDocument::Compact);
    QString str(byte_array);
    json_str.push_back(str);
//    qDebug() << "store the color";
//    qDebug() << json_str.size();
}

void Storage::storeToFile(QDate date)
{
    QJsonObject json;
    json.insert("year",date.year());
    json.insert("month",date.month());
    json.insert("day",date.day());
    json.insert("delete",1);
    QJsonDocument document;
    document.setObject(json);
    QByteArray byte_array = document.toJson(QJsonDocument::Compact);
    QString str(byte_array);
    json_str.push_back(str);
}

void Storage::storeToFile1(QDate date, QString str)
{
    QJsonObject json;
    json.insert("year",date.year());
    json.insert("month",date.month());
    json.insert("day",date.day());
    json.insert("filename",str);
    QJsonDocument document;
    document.setObject(json);
    QByteArray byte_array = document.toJson(QJsonDocument::Compact);
    QString str1(byte_array);
    json_str.push_back(str1);
}

void Storage::storeToFile1(QDate date)
{
    QJsonObject json;
    json.insert("year",date.year());
    json.insert("month",date.month());
    json.insert("day",date.day());
    json.insert("deletefile",1);
    QJsonDocument document;
    document.setObject(json);
    QByteArray byte_array = document.toJson(QJsonDocument::Compact);
    QString str1(byte_array);
    json_str.push_back(str1);
}

void Storage::exportFile(QString filename)
{
    ofstream fout(filename.toStdString().data());
    //qDebug() << "!!!!into the file";
    //qDebug() << json_str.size();
    for(int i = 0; i < json_str.size(); i++)
    {
        fout << json_str.at(i).toStdString() << endl;
        //qDebug() << "into the file";
    }
}
