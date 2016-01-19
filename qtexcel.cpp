#include "qtexcel.h"
#include <QSqlQuery>
#include <QtDebug>
#include <QVariant>
#include <QStandardItem>

const int SHEETROWSMAX = 50000;

int QtExcel::createConnect(QSqlDatabase &db,QString flag)
{
    if(fieldList.size() <= 0)
    {
        qDebug() << "ExportExcelObject::export2Excel failed: No fields defined.";
        return -1;
    }
     db = QSqlDatabase::addDatabase("QODBC", "excelexport");//创建QODBC驱动通过excelexport链接的数据库
    if(!db.isValid())
    {
        qDebug() << "ExportExcelObject::export2Excel failed: QODBC not supported.";
        return -2;
    }
    // set the dsn string
    QString dsn = QString("DRIVER={Microsoft Excel Driver (*.xls)};DSN='';FIRSTROWHASNAMES=1;READONLY=%1;CREATE_DB=\"%2\";DBQ=%3").
                  arg(flag).arg(excelFilePath).arg(excelFilePath);
    db.setDatabaseName(dsn);//设置数据库的名称为dsn
    if(!db.open())//打开数据库
    {
        qDebug() << "ExportExcelObject::export2Excel failed: Create Excel file failed by DRIVER={Microsoft Excel Driver (*.xls)}.";
        return -3;
    }

    return 0;
}


int QtExcel::createSheet(QSqlQuery &query, int rowCount, int sheetNum)
{
    //query=QSqlQuery(db);//建立一个QSqlQuery实例

    //drop the table if it's already exists
    QString s, sSql = QString("DROP TABLE [%1_表%2]").arg(sheetName).arg(sheetNum);//从数据库中清除一个表格（清除同名的表格）
    query.exec(sSql);//通过一个QSqlQuery实例执行了 SQL 语句
    //===========================================================//
    //create the table (sheet in Excel file)
    sSql = QString("CREATE TABLE [%1_表%2] (").arg(sheetName).arg(sheetNum);//创建一个名为sheetName的表格 CREAT TABLE sheetName (

    for (int i = 0; i < fieldList.size(); i++)//构造CREAT TABLE sheetName ( 之后的语句
    {
        s = QString("[%1] %2").arg(fieldList.at(i)->sFieldName).arg(fieldList.at(i)->sFieldType);
        sSql += s;
        if(i < fieldList.size() - 1)
            sSql += " , ";
    }

    sSql += ")";

    //QSqlQuery supports prepared query execution and the binding of parameter values to placeholders.
    query.prepare(sSql);


    if(!query.exec())//运行query中的SQL语句
    {
        qDebug() << "ExportExcelObject::export2Excel failed: Create Excel sheet failed.";
        return -4;
    }
    //=======================================================//
    //add all rows
    //一次输入一笔资料
    //INSERT INTO "表格名" ("栏位1", "栏位2", ...)VALUES ("值1", "值2", ...);
    sSql = QString("INSERT INTO [%1_表%2] (").arg(sheetName).arg(sheetNum);
    for (int i = 0; i < fieldList.size(); i++)
    {
        sSql += fieldList.at(i)->sFieldName;
        if(i < fieldList.size() - 1)
            sSql += " , ";
    }
    sSql += ") VALUES (";
    for (int i = 0; i < fieldList.size(); ++i)
    {
        sSql += QString(":data%1").arg(i);
        if(i < fieldList.size() - 1)
            sSql += " , ";
    }
    sSql += ")";

    //qDebug() << sSql;
    //INSERT INTO [mydata] (频率（MHz） , 温度（K） , 介电常数 , 介电损耗)
    //VALUES (:data0 , :data1 , :data2 , :data3)

    int r, iRet = 0;
    for(r = 0 ; r < rowCount ; ++r)
    {
        query.prepare(sSql);
        for (int c = 0; c < fieldList.size(); ++c)
        {
            query.bindValue(QString(":data%1").arg(c),
                            tableView->model()->data(tableView->model()->index(r+SHEETROWSMAX*sheetNum,c)));


        }

        if(query.exec())//累计插入多少笔数据
            iRet++;
    }

    return iRet;
}

int QtExcel::table2Excel()
{
    QSqlDatabase db;
    createConnect(db,"false");
    //========================================================//
   QSqlQuery query(db);
   int saveVal=0;
   int rowCount=0;
   int sheetNum=0;
   bool loop=true;

   while(loop)
   {
       if(tableView->model()->rowCount()-SHEETROWSMAX*sheetNum>SHEETROWSMAX)
       {
           rowCount=SHEETROWSMAX;
           loop=true;
       }else{
           rowCount=tableView->model()->rowCount()-SHEETROWSMAX*sheetNum;
           loop=false;
       }

       saveVal+=createSheet(query,rowCount,sheetNum);
       sheetNum++;
   }
   return saveVal;
}

int QtExcel::readSheet(QSqlQuery &query, int sheetNum)
{
    QString rSql=QString("SELECT COUNT(*) FROM  [%1_表%2]").arg(sheetName).arg(sheetNum);
    query.exec(rSql);
    query.next();
    int count = query.value(0).toInt();
    rSql=QString("SELECT [");
    for (int i = 0; i < fieldList.size(); i++)
    {
        rSql += fieldList.at(i)->sFieldName;
        if(i < fieldList.size() - 1)
            rSql += "] , [";
    }
    rSql += QString("] FROM [%1_表%2]").arg(sheetName).arg(sheetNum);
    query.exec(rSql);

    int nowRow=tableView->model()->rowCount();
    if(count+SHEETROWSMAX*sheetNum>nowRow)
    {
        tableView->model()->insertRows(nowRow,count-nowRow+SHEETROWSMAX*sheetNum);
    }else
    {
        tableView->model()->removeRows(count+SHEETROWSMAX*sheetNum,nowRow-count-SHEETROWSMAX*sheetNum);
    }

    int readVal=0;
    while(query.next())
    {
       for(int c=0;c<fieldList.size();++c)
        {
           tableView->model()->setData(tableView->model()->index(readVal+SHEETROWSMAX*sheetNum,c),
                                       QVariant(tr("%1").arg(query.value(c).toDouble())));
        }

       ++readVal;
    }
    return readVal;
}

int QtExcel::excel2Table()
{
    QSqlDatabase db;
    createConnect(db,"true");

    QSqlQuery query(db);
    int sheetNum=0;
    int readVal=0;

    do{
        readVal+=readSheet(query,sheetNum);
        sheetNum++;
    }while(readVal%SHEETROWSMAX==0);


    return readVal;
}
