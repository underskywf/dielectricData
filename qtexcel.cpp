#include "qtexcel.h"
#include <QSqlQuery>
#include <QtDebug>
#include <QVariant>
#include <QStandardItem>

/*
 * 用QODBC 链接Excel
 * 然后对QTableView执行SQL语句来处理数据
*/

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
        //QSqlDatabase::removeDatabase("excelexport");
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
    //http://www.1keydata.com/cn/sql/sql-create.php
    /*
     * 表格被分为栏位 (column) 及列位 (row)。每一列代表一笔资料，而每一栏代表一笔资料的一部份。
     * 举例来说，如果我们有一个记载顾客资料的表格，那栏位就有可能包括姓、名、地址、城市、国家、
     * 生日．．．等等。当我们对表格下定义时，我们需要注明栏位的标题，以及那个栏位的资料种类。
     * 那，资料种类是什么呢？资料可能是以许多不同的形式存在的。它可能是一个整数 (例如 1)，、一个实数(例如 0.55)
     * 、一个字串 (例如 'sql')、一个日期/时间 (例如 '2000-JAN-25 03:22:22')、或甚至是 以二进法 (binary) 的状
     * 态存在。当我们在对一个表格下定义时，我们需要对每一个栏位的资料种类下定义。(例如 '姓' 这个栏位的资料种类是 char(50)
     * ━━代表这是一个 50 个字符的字串)。我们需要注意的一点是不同的数据库有不同的资料种类，所以在对表格做出定义之前最好先参考
     * 一下数据库本身的说明。
     * CREATE TABLE 的语法是：
     * CREATE TABLE "表格名"("栏位 1" "栏位 1 资料种类","栏位 2" "栏位 2 资料种类",... );
     *
     * 若我们要建立我们上面提过的顾客表格，我们就打入以下的 SQL：
     * CREATE TABLE Customer(First_Name char(50),Last_Name char(50),Address char(50),City char(50),
     * Country char(25),Birth_Date datetime);
     */

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
            //tableView->setItem(readVal,c,new QTableWidgetItem(tr("%1").
            //                                              arg(query.value(c).toDouble())));

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

/*
 *  QSqlQuery query;
    query.prepare("INSERT INTO person (id, forename, surname) "
                  "VALUES (?, ?, ?)");
    query.bindValue(0, 1001);
    query.bindValue(1, "Bart");
    query.bindValue(2, "Simpson");
    query.exec();
 *
 */
/*
 * 综上所述，存储主要步骤为:
 * 1.判断栏位（colum）是否为空
 * 2.建立数据库（驱动类型，链接，名称）
 * 3.创建表单（删除同名表单，之后 CREAT TABLE [表单名] ( "栏位1" "栏位1种类"{,"栏位2" "栏位2种类"...});
 * 4.插入资料
 * 5.绑定值
 *
 * 读取主要步骤为：
 *
 */
