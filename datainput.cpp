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

    ui->VA->setValidator(new QRegExpValidator(double_100g,ui->VA));
    ui->VB->setValidator(new QRegExpValidator(double_100g,ui->VB));
    ui->VB1->setValidator(new QRegExpValidator(double_100g,ui->VB1));
     ui->VE->setValidator(new QRegExpValidator(double_100g,ui->VE));

    connect(ui->OkButton,&QPushButton::clicked,this,&dataInput::onOkClicked);
    connect(ui->CancelButton,&QPushButton::clicked,this,&dataInput::close);
}

dataInput::~dataInput()
{
    delete ui;
}

void dataInput::init()
{
    ui->FStart->setText("1.0");
    ui->FEnd->setText("20.0");
    ui->FStep->setText("0.01");

    ui->TStart->setText("298.15");
    ui->TEnd->setText("2683.15");
    ui->TStep->setText("0");

    ui->VA->setText("10.0");
    ui->VB->setText("10");
    ui->VB1->setText("10");
    ui->VE->setText("10");
}

void dataInput::onOkClicked()
{
    QStringList dataInputStr;
    dataInputStr<<ui->FStart->text()
                <<ui->FEnd->text()
                <<ui->FStep->text()
                <<ui->TStart->text()
                <<ui->TEnd->text()
                <<ui->TStep->text()
                <<ui->VA->text()
                <<ui->VB->text()
                <<ui->VB1->text()
               <<ui->VE->text();
    emit sendData(dataInputStr);
}


