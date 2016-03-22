#include "dataprocessing.h"
#include "ui_dataprocessing.h"
#include "datainput.h"
#include "dataanalysis.h"
#include "qtexcel.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QtMath>

const int  precision =6;//数据保留的小数点后位数
const double e0=8.854e-12;//真空介电常数
const double pi=3.1415926;
const double kb=8.614e-5;//波尔兹曼常数，eV/K
const double h=6.63e-34;//普朗克常数，J*s
const double my2o3=225.81;//氧化钇摩尔质量，g/mol
const double rouy2o3=5.01e+3;//氧化钇密度，Kg/m3
const double ay2o3=1.060e-9;//氧化钇晶格常数，m
const double e_r_inf=3.58;//氧化钇光频相对介电常数
const double e_r_s=9.77;//氧化钇静态相对介电常数
const double tao=1e-13;//本征离子弛豫时间，s
const double e=1.602e-19;//元电荷，C
const double me=9.11e-31;//电子静止质量，Kg
const double E0=0.5;//氧化钇本征离子活化能，eV
const double Ea=0.1;//钠离子活化能，eV
const double Eg=6;//氧化钇材料禁带宽度，eV
const double Ni=4.03e+28;//氧化钇单位体积内的离子数，个/m-3
const double mna=3.82e-26;//钠离子质量
const double e_r_s_al2o3=10;//氧化铝静态介电常数
const double e_r_inf_al2o3=1.63*1.63;//氧化铝光频介电常数
const double Eal2o3=0.25;//氧化铝激活能
const double roual2o3=3.9e+3;//氧化铝粉密度，Kg/m3


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
            <<"0.0"<<"y2o3";

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
        obj.addField(i,ui->TableList->horizontalHeaderItem(i)->text(), "char(20)");
    }

    int readVal=obj.excel2Table();

    if( readVal > 0)
    {
        QMessageBox::information(this, tr("数据读取结束："),QString(tr("共读取%1条数据！")).arg(readVal));
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

    QString kind="y2o3";


    //频率参数
    FStartVal=(inputData.at(0)).toDouble();
    FEndVal=(inputData.at(1)).toDouble();
    FStepVal=(inputData.at(2)).toDouble();
    //温度参数
    TStartVal=(inputData.at(3)).toDouble();
    TEndVal=(inputData.at(4)).toDouble();
    TStepVal=(inputData.at(5)).toDouble();
    //其它参数
    MEXVal=(inputData.at(6)).toDouble();

    kind=inputData.at(7);

    double frequency=FStartVal;
    double temp =TStartVal;
    double temperature =TStartVal;
    double Epsilon1=1.0;
    double Epsilon11=1.0;
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

            if(kind=="y2o3")
            y2o3Pure(Epsilon1,Epsilon11,temperature,frequency);
            if(kind=="na2o")
            y2o3Na2O(Epsilon1,Epsilon11,temperature,frequency);
            if(kind=="al2o3")
            y2o3Al2O3(Epsilon1,Epsilon11,temperature,frequency);

            TANEpsilon=Epsilon11/Epsilon1;

            ui->TableList->setItem(i*TNumber+j, 0, new QTableWidgetItem(tr("%1").arg(i*TNumber+j+1)));                                                                           
            ui->TableList->setItem(i*TNumber+j, 1, new QTableWidgetItem(tr("%1").arg(frequency)));
            ui->TableList->setItem(i*TNumber+j, 2, new QTableWidgetItem(tr("%1").arg(temperature)));
            ui->TableList->setItem(i*TNumber+j, 3, new QTableWidgetItem(tr("%1").arg(
                                                                            QString::number(Epsilon1,'f', precision))));
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
        obj.addField(i,ui->TableList->horizontalHeaderItem(i)->text(), "char(20)");
    }

    int retVal = obj.table2Excel();
    if( retVal > 0)
    {
        QMessageBox::information(this, tr("数据存储结束："),QString(tr("共保存%1条数据！")).arg(retVal));
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
void dataProcessing::y2o3Pure(double &Epsilon1,double &Epsilon11
                              ,const double &temperature,const double &frequency )
{
    //氧化钇材料总电导率
    double gama=Ni*9*e*e*qPow(ay2o3,2)/(tao*6*kb*temperature)*qExp(-E0/(kb*temperature))
            +10e-8*e*qPow(2*pi*me*kb*temperature*e/(h*h),3/2)
            *qExp(-Eg/(2*kb*temperature));


    double b_inf=-1.01e-5;//单位K^-1
    //double b_s=2.06e-3-7.83*(-4*qPow(temperature,3))/(qPow(2600,4)-qPow(temperature,4));
    double b_s=2.06e-5;

    double b_inf_T=1+b_inf*(temperature-300);
    double b_s_T=1+b_s*(temperature-300);

    //氧化钇陶瓷介电响应
    Epsilon1=e_r_inf*b_inf_T+(e_r_s*b_s_T-e_r_inf*b_inf_T)/(
                1+4*pi*pi*tao*tao*frequency*frequency*1e18);
    Epsilon11=(e_r_s*b_s_T-e_r_inf*b_inf_T)*2*pi*tao*frequency*1e9/(
                1+4*pi*pi*tao*tao*frequency*frequency*1e18)+gama/(2*pi*e0*frequency*1e9);

}

void dataProcessing::y2o3Na2O(double &Epsilon1,double &Epsilon11
                              ,const double &temperature,const double &frequency )
{
    //理想氧化钇陶瓷
    y2o3Pure(Epsilon1,Epsilon11,temperature,frequency);

    //杂质离子数
    double n_na=MEXVal*1e-6*rouy2o3/mna;

    //杂质离子电导率
    double gama_na=n_na*e*e*ay2o3*ay2o3/(6*kb*e*temperature*tao)*
            qExp(-Ea/(kb*temperature));

    //热离子极化率
    double alpha_T=e*e*ay2o3*ay2o3/(12*kb*e*temperature);

    //极化弛豫时间
    double tao_T=(e_r_s+2)*tao/(2*(e_r_inf+2))*qExp(Ea/(kb*temperature));


    //介电参数
    Epsilon1=Epsilon1+MEXVal*1e-6*alpha_T*rouy2o3/(e0*mna*(1+
              4*pi*pi*tao_T*tao_T*frequency*frequency*1e18));

    Epsilon11=Epsilon11
            +gama_na/(2*pi*frequency*1e9*e0)
            +MEXVal*(1e-6)*rouy2o3/(e0*mna)*2*pi*tao_T*alpha_T*frequency*1e9/(
             1+4*pi*pi*tao_T*tao_T*frequency*frequency*1e18);

}

void dataProcessing::y2o3Al2O3(double &Epsilon1,double &Epsilon11
                               ,const double &temperature,const double &frequency )
{
    double e_r_s_m=qExp(MEXVal*1e-6*rouy2o3/roual2o3*qLn(e_r_s_al2o3)
                        +(1-MEXVal*1e-6*rouy2o3/roual2o3)*qLn(e_r_s));
    double e_r_inf_m=qExp(MEXVal*1e-6*rouy2o3/roual2o3*qLn(e_r_inf_al2o3)
                        +(1-MEXVal*1e-6*rouy2o3/roual2o3)*qLn(e_r_inf));

    double tao_m=tao/2*(1+2*e_r_s/e_r_s_al2o3)*(
                (e_r_s+2)/(e_r_inf+2))*qExp(Eal2o3/(kb*temperature));

    Epsilon1=e_r_inf_m+(e_r_s_m-e_r_inf_m)/(
                1+4*pi*pi*tao_m*tao_m*frequency*frequency*1e18);
    Epsilon11=(e_r_s_m-e_r_inf_m)*2*pi*tao_m*frequency*1e9/(
                1+4*pi*pi*tao_m*tao_m*frequency*frequency*1e18);
}
