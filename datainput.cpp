#include "datainput.h"
#include "ui_datainput.h"

dataInput::dataInput(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dataInput)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("介电常数研究-参数设置"));
    this->setWindowFlags(Qt::WindowSystemMenuHint);
    setWindowIcon(QIcon(":/pic/input"));
    setFixedSize(500,400);
    init();

    QRegExp double_100g("1000|([0-9]{0,4}[\\.][0-9]{1,7})");

    ui->FStart->setValidator(new QRegExpValidator(double_100g,ui->FStart));
    ui->FEnd->setValidator(new QRegExpValidator(double_100g,ui->FEnd));
    ui->FStep->setValidator(new QRegExpValidator(double_100g,ui->FStep));

    ui->TStart->setValidator(new QRegExpValidator(double_100g,ui->TStart));
    ui->TEnd->setValidator(new QRegExpValidator(double_100g,ui->TEnd));
    ui->TStep->setValidator(new QRegExpValidator(double_100g,ui->TStep));

    ui->MEX->setValidator(new QRegExpValidator(double_100g,ui->MEX));

    connect(ui->OkButton,&QPushButton::clicked,this,&dataInput::onOkClicked);
    connect(ui->CancelButton,&QPushButton::clicked,this,&dataInput::close);
}

dataInput::~dataInput()
{
    delete ui;
}

void dataInput::init()
{
    ui->y2o3Pure->setChecked(true);

    ui->FStart->setText("1.0");
    ui->FEnd->setText("20.0");
    ui->FStep->setText("0.01");

    ui->TStart->setText("298.15");
    ui->TEnd->setText("2683.15");
    ui->TStep->setText("0");

    ui->MEX->setText("0");
}

void dataInput::onOkClicked()
{
    QStringList dataInputStr;

    QString kind="y2o3";
    if(ui->y2o3Pure->isChecked())
        kind="y2o3";
    if(ui->Na2O->isChecked())
        kind="na2o";
    if(ui->Al2O3->isChecked())
        kind="al2o3";

    dataInputStr<<ui->FStart->text()
                <<ui->FEnd->text()
                <<ui->FStep->text()
                <<ui->TStart->text()
                <<ui->TEnd->text()
                <<ui->TStep->text()
                <<ui->MEX->text()
                <<kind;
    emit sendData(dataInputStr);
}


