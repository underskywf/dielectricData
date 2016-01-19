///////////////////////////////////////////////////////////////////////////////

#ifndef QTEXCEL_H
#define QTEXCEL_H

#include <QObject>
#include <QTableView>
#include <QStringList>
#include <QSqlDatabase>

class EEO_Field
{
public:
    EEO_Field(const int ic, const QString &sf, const QString &st):
            iCol(ic),sFieldName(sf),sFieldType(st){}

    int     iCol;
    QString sFieldName;
    QString sFieldType;
};

class QtExcel : public QObject
{
    Q_OBJECT
public:
    QtExcel(const QString &filepath,  //库文件路径
            const QString &sheettitle,//表单名称
            QTableView *tableviewTemp):   //Qt列表item
           excelFilePath(filepath),sheetName(sheettitle),tableView(tableviewTemp)
    {

    }

    ~QtExcel() {QSqlDatabase::removeDatabase("excelexport");}//删除数据库 excelexport

    int createConnect(QSqlDatabase&,QString);

public:
    void addField(const int iCol, const QString &fieldname, const QString &fieldtype)
         {fieldList << new EEO_Field(iCol, fieldname, fieldtype);}

    int createSheet(QSqlQuery &query,int rowCount,int sheetNum);
    int table2Excel();

    int readSheet(QSqlQuery &query,int sheetNum);
    int excel2Table();

private:
    QString excelFilePath;
    QString sheetName;
    QTableView *tableView;
    QList<EEO_Field *> fieldList;
};

#endif // QTEXCEL_H
