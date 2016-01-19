#include "dataprocessing.h"
#include "ui_dataprocessing.h"
#include "datainput.h"
#include "dataanalysis.h"
#include "qtexcel.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QtMath>
//#include <QAxObject>

//void Table2Excel(QTableWidget *table,QString title);
const int  precision =6;//数据保留的小数点后位数

dataProcessing::dataProcessing(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::dataProcessing)
{
    ui->setupUi(this);
    setWindowTitle(tr("介电常数研究"));
    setWindowFlags(Qt::WindowSystemMenuHint);
    setWindowIcon(QIcon(":/pic/dda"));
    setFixedSize(512,400);

    ui->TableList->verticalHeader()->hide();
    ui->TableList->setColumnWidth(0,80);
    ui->TableList->setEditTriggers(QAbstractItemView::NoEditTriggers);

    init();

    connect(ui->OpenButton,&QPushButton::clicked,this,&dataProcessing::onOpenDataClicked);
    connect(ui->dataSetButton,&QPushButton::clicked,this,&dataProcessing::onDataSetClicked);
    connect(ui->calculateButton,&QPushButton::clicked,this,&dataProcessing::onCalculateClicked);
    connect(ui->SaveDataButton,&QPushButton::clicked,this,&dataProcessing::onSaveDataClicked);

    connect(ui->comboBox,SIGNAL(activated(int)),this,SLOT(onDrawCurveClicked()));
    connect(ui->exitButton,&QPushButton::clicked,this,&dataProcessing::onExitClicked);
}

dataProcessing::~dataProcessing()
{
    delete ui;
}

void dataProcessing::init()
{
    inputData<<"1.0"<<"20.0"<<"0.01"
             <<"298.15"<<"2683.15"<<"1000.0"
            <<"1.0"<<"1.0"<<"100.0"<<"10.0";

}

void dataProcessing::onOpenDataClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Excel Files"),
            qApp->applicationDirPath()+"save/dielectricData.xls",
            tr("Excel 文件(*.xls *.xlsx)"));

    if (fileName.isEmpty())
        return;

    QtExcel obj(fileName,this->windowTitle(),ui->TableList);

    for(int i=0;i<ui->TableList->columnCount();++i)
    {
        obj.addField(i,ui->TableList->horizontalHeaderItem(i)->text(), "double");
    }

    int readVal=obj.excel2Table();

    if( readVal > 0)
    {
        QMessageBox::information(this, tr("Read Data Done"),QString(tr("%1 records imported!")).arg(readVal));
    }

}

void dataProcessing::onDataSetClicked()
{
    dataInput* input=new dataInput;//在堆上建立，非阻态
    inputDialog.append(input);
    connect(input,&dataInput::sendData,this,&dataProcessing::receiveData);
    //inputDialog.setModal(false);
    input->show();
    QMessageBox::about(input,tr("提示："),
    tr("本页面为参数设置，请设置完参数后，点击“确认”按钮；点击“关闭”按钮，退出设置。"));
}

void dataProcessing::onCalculateClicked()
{
    double FStartVal=1.0;
    double FEndVal=20.0;
    double FStepVal=0.001;

    double TStartVal=298.15;
    double TEndVal=2683.15;
    double TStepVal=1000.0;

    double VAVal=1.0;
    double VBVal=1.0;
    double VB1Val=100.0;
    double VEVal=10.0;

    //频率参数
    FStartVal=(inputData.at(0)).toDouble();
    FEndVal=(inputData.at(1)).toDouble();
    FStepVal=(inputData.at(2)).toDouble();
    //温度参数
    TStartVal=(inputData.at(3)).toDouble();
    TEndVal=(inputData.at(4)).toDouble();
    TStepVal=(inputData.at(5)).toDouble();
    //其它参数
    VAVal=(inputData.at(6)).toDouble();
    VBVal=(inputData.at(7)).toDouble();
    VB1Val=(inputData.at(8)).toDouble();
    VEVal=(inputData.at(9)).toDouble();

    double frequency=FStartVal;
    double temp =TStartVal;
    double temperature =TStartVal;
    double Epsilon=0.0;
    double TANEpsilon=0.0;
    int FNumber=1,TNumber=1;
    if(FStepVal>0)
    {
        for(FNumber=1;FStartVal<FEndVal;++FNumber)
            FStartVal+=FStepVal;
    }

    if(TStepVal>0)
    {
        for(TNumber=1;TStartVal<TEndVal;++TNumber)
            TStartVal+=TStepVal;
    }
    ui->TableList->setRowCount(FNumber*TNumber);

    for(int i=0;i<FNumber;++i)
    {
        temperature=temp;
        if(i>0) frequency+=FStepVal;
        if(frequency>FEndVal) frequency=FEndVal;

        for(int j=0;j<TNumber;++j)
        {
            if(j>0) temperature+=TStepVal;
            if(temperature>TEndVal) temperature=TEndVal;

            Epsilon=VEVal+VAVal*qPow(10,18)/(temperature+frequency*qPow(10,9)*frequency*qPow(10,9)*
                                             VB1Val*VB1Val*qExp(2*VBVal/temperature));
            TANEpsilon=
            VAVal*qPow(10,18)*frequency*qPow(10,9)*
                    VB1Val*qExp(VBVal/temperature)/(VAVal*qPow(10,18)+
                                                    temperature*VEVal*(1+frequency*qPow(10,9)*
                                                                       frequency*qPow(10,9)*VB1Val*VB1Val*
                                                                       qExp(2*VBVal/temperature)));

            ui->TableList->setItem(i*TNumber+j, 0, new QTableWidgetItem(tr("%1").arg(i*TNumber+j+1)));
                                                                            //QString::number((i*TNumber+j+1),'g',7));
            ui->TableList->setItem(i*TNumber+j, 1, new QTableWidgetItem(tr("%1").arg(frequency)));
            ui->TableList->setItem(i*TNumber+j, 2, new QTableWidgetItem(tr("%1").arg(temperature)));
            ui->TableList->setItem(i*TNumber+j, 3, new QTableWidgetItem(tr("%1").arg(
                                                                            QString::number(Epsilon,'f', precision))));
            ui->TableList->setItem(i*TNumber+j, 4, new QTableWidgetItem(tr("%1").arg(
                                                                            QString::number(TANEpsilon,'f',precision))));
        }
    }

}

void dataProcessing::onSaveDataClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Excel Files"),
            qApp->applicationDirPath()+"save/dielectricData.xls",
            tr("Excel 文件(*.xls)"));

    if (fileName.isEmpty())
        return;

    QtExcel obj(fileName, this->windowTitle(), ui->TableList);
    for(int i=0;i<ui->TableList->columnCount();++i)
    {
        obj.addField(i,ui->TableList->horizontalHeaderItem(i)->text(), "double");
    }

    int retVal = obj.table2Excel();
    if( retVal > 0)
    {
        QMessageBox::information(this, tr("Save Data Done"),QString(tr("%1 records exported!")).arg(retVal));
    }
}

void dataProcessing::onDrawCurveClicked()
{
    dataAnalysis *analysis=new dataAnalysis(ui->TableList,ui->comboBox->currentIndex());
    analysisDialog.append(analysis);
    analysis->show();
}

void dataProcessing::onExitClicked()
{
    while(!(inputDialog.isEmpty()))   inputDialog.takeFirst()->close();
    while(!(analysisDialog.isEmpty())) analysisDialog.takeFirst()->close();
    this->close();
}

void dataProcessing::receiveData(const QStringList tempData)
{
    inputData=tempData;

    QMessageBox::about(this,tr("提示:"),
                       tr("参数设置完成，请点击”计算“按钮，生成数据列表。"));
}

/*
void Table2Excel(QTableWidget *table,QString title)
{
    QString fileName = QFileDialog::getSaveFileName(table, "保存",
            QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+".xlsx",
            "Excel 文件(*.xls *.xlsx)");
    if (fileName!="")
    {
        QAxObject *excel = new QAxObject;
        if (excel->setControl("Excel.Application")) //连接Excel控件
        {
            excel->dynamicCall("SetVisible (bool Visible)","false");//不显示窗体
            excel->setProperty("DisplayAlerts", false);//不显示任何警告信息。如果为true那么在关闭是会出现类似“文件已修改，是否保存”的提示
            QAxObject *workbooks = excel->querySubObject("WorkBooks");//获取工作簿集合
            workbooks->dynamicCall("Add");//新建一个工作簿
            QAxObject *workbook = excel->querySubObject("ActiveWorkBook");//获取当前工作簿
            QAxObject *worksheet = workbook->querySubObject("Worksheets(int)", 1);
            int i,j,colcount=table->columnCount();
            QAxObject *cell,*col;
            //标题行
            cell=worksheet->querySubObject("Cells(int,int)", 1, 1);
            cell->dynamicCall("SetValue(const QString&)", title);
            cell->querySubObject("Font")->setProperty("Size", 18);
            //调整行高
            worksheet->querySubObject("Range(const QString&)", "1:1")->setProperty("RowHeight", 30);
            //合并标题行
            QString cellTitle;
            cellTitle.append("A1:");
            cellTitle.append(QChar(colcount - 1 + 'A'));
            cellTitle.append(QString::number(1));
            QAxObject *range = worksheet->querySubObject("Range(const QString&)", cellTitle);
            range->setProperty("WrapText", true);
            range->setProperty("MergeCells", true);
            range->setProperty("HorizontalAlignment", -4108);//xlCenter
            range->setProperty("VerticalAlignment", -4108);//xlCenter
            //列标题
            for(i=0;i<colcount;i++)
            {
                QString columnName;
                columnName.append(QChar(i  + 'A'));
                columnName.append(":");
                columnName.append(QChar(i + 'A'));
                col = worksheet->querySubObject("Columns(const QString&)", columnName);
                col->setProperty("ColumnWidth", table->columnWidth(i)/6);
                cell=worksheet->querySubObject("Cells(int,int)", 2, i+1);
                columnName=table->horizontalHeaderItem(i)->text();
                cell->dynamicCall("SetValue(const QString&)", columnName);
                cell->querySubObject("Font")->setProperty("Bold", true);
                cell->querySubObject("Interior")->setProperty("Color",QColor(191, 191, 191));
                cell->setProperty("HorizontalAlignment", -4108);//xlCenter
                cell->setProperty("VerticalAlignment", -4108);//xlCenter
            }
            //数据区
            for(i=0;i<table->rowCount();i++){
                for (j=0;j<colcount;j++)
                {
                    worksheet->querySubObject("Cells(int,int)", i+3, j+1)->dynamicCall("SetValue(const QString&)", table->item(i,j)?table->item(i,j)->text():"");
                }
            }
            //画框线
            QString lrange;
            lrange.append("A2:");
            lrange.append(colcount - 1 + 'A');
            lrange.append(QString::number(table->rowCount() + 2));
            range = worksheet->querySubObject("Range(const QString&)", lrange);
            range->querySubObject("Borders")->setProperty("LineStyle", QString::number(1));
            range->querySubObject("Borders")->setProperty("Color", QColor(0, 0, 0));
            //调整数据区行高
            QString rowsName;
            rowsName.append("2:");
            rowsName.append(QString::number(table->rowCount() + 2));
            range = worksheet->querySubObject("Range(const QString&)", rowsName);
            range->setProperty("RowHeight", 20);
            workbook->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(fileName));//保存至fileName
            workbook->dynamicCall("Close()");//关闭工作簿
            excel->dynamicCall("Quit()");//关闭excel
            delete excel;
            excel=NULL;
            if (QMessageBox::question(NULL,"完成","文件已经导出，是否现在打开？",QMessageBox::Yes|QMessageBox::No)==QMessageBox::Yes)
            {
                QDesktopServices::openUrl(QUrl("file:///" + QDir::toNativeSeparators(fileName)));
            }
        }
        else
        {
            QMessageBox::warning(NULL,"错误","未能创建 Excel 对象，请安装 Microsoft Excel。",QMessageBox::Apply);
        }
    }
}
*/
