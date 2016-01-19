#include <QTableWidget>
#include "dataanalysis.h"
#include "ui_dataanalysis.h"
#include "qwt_plot.h"
#include <qwt_legend.h>
#include <qwt_point_data.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_renderer.h>

class MyZoomer: public QwtPlotZoomer
{
public:
    MyZoomer( QWidget *canvas ):
        QwtPlotZoomer( canvas )
    {
        setTrackerMode( AlwaysOn );
    }

    virtual QwtText trackerTextF( const QPointF &pos ) const
    {
        QColor bg( Qt::white );
        bg.setAlpha( 200 );

        QwtText text = QwtPlotZoomer::trackerTextF( pos );
        text.setBackgroundBrush( QBrush( bg ) );
        return text;
    }
};

dataAnalysis::dataAnalysis(const QTableWidget *dataTable,int nowIndex):
    ui(new Ui::dataAnalysis)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("介电常数研究-曲线绘制"));
    this->setWindowFlags(Qt::WindowSystemMenuHint);
    setWindowIcon(QIcon(":/pic/draw"));
    setFixedSize(500,400);

    //绘制 E-F
    int xH=1;
    int yV=3;

    switch (nowIndex) {
    case 0:
        xH=1;
        yV=3;
        break;

    case 1:
        xH=1;
        yV=4;
        break;

    case 2:
        xH=2;
        yV=3;
        break;

    case 3:
        xH=2;
        yV=4;
        break;
    default:
        break;
    }

    int number=dataTable->rowCount();
    QStringList titleList;
    for(int c=0;c<dataTable->columnCount();++c)
    {
        titleList<<dataTable->horizontalHeaderItem(c)->text();
    }

    saveName=titleList.at(xH)+'_'+titleList.at(yV);

    Fmin=((dataTable->item(0,xH))->text()).toDouble();
    Fmax=((dataTable->item(number-1,xH))->text()).toDouble();
    Emin=((dataTable->item(0,yV))->text()).toDouble();
    Emax=Emin;

    for(int i=0;i<number;++i)
     {
        xs.append(((dataTable->item(i,xH))->text()).toDouble());
        ys.append(((dataTable->item(i,yV))->text()).toDouble());
        Emax=(ys.at(i)>Emax?ys.at(i):Emax);
        Emin=(ys.at(i)<Emin?ys.at(i):Emin);
     }

    ui->qwtPlot->resize(450,350);
    ui->qwtPlot->setAxisTitle(QwtPlot::xBottom,titleList.at(xH));
    ui->qwtPlot->setAxisTitle(QwtPlot::yLeft,titleList.at(yV));

    ui->qwtPlot->setAxisScale(QwtPlot::xBottom,Fmin,Fmax);
    ui->qwtPlot->setAxisScale(QwtPlot::yLeft,Emin,Emax);

    ui->qwtPlot->setBackgroundRole(QPalette::Dark);

    ui->qwtPlot->insertLegend(new QwtLegend, QwtPlot::BottomLegend);

    //使用滚轮放大/缩小
    //(void) new QwtPlotMagnifier(ui->qwtPlot->canvas());

    //使用鼠标左键平移
    //(void) new QwtPlotPanner(ui->qwtPlot->canvas());

    // LeftButton for the zooming
    // MidButton for the panning
    // RightButton: zoom out by 1
    // Ctrl+RighButton: zoom out to full size
    QwtPlotZoomer* zoomer = new MyZoomer( ui->qwtPlot->canvas() );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
        Qt::RightButton );

    QwtPlotPanner *panner = new QwtPlotPanner( ui->qwtPlot->canvas() );
    panner->setAxisEnabled( QwtPlot::yRight, false );
    panner->setMouseButton( Qt::MidButton );

    //绘制曲线
    QwtPointArrayData *const data=new QwtPointArrayData(xs,ys);
    QwtPlotCurve *FECurve=new QwtPlotCurve(titleList.at(xH)+'-'+titleList.at(yV));
    FECurve->setData(data);
    FECurve->setStyle(QwtPlotCurve::Dots);
    FECurve->setCurveAttribute(QwtPlotCurve::Fitted,true);
    FECurve->setPen(QPen(Qt::blue));
    FECurve->attach(ui->qwtPlot);

    ui->qwtPlot->show();
    ui->qwtPlot->replot();

    connect(ui->ExitButton,&QPushButton::clicked,this,&dataAnalysis::close);
    connect(ui->SaveButton,&QPushButton::clicked,this,&dataAnalysis::onSaveClicked);


}

dataAnalysis::~dataAnalysis()
{
    delete ui;
}

void dataAnalysis::onSaveClicked()
{
    QwtPlotRenderer renderer;
    renderer.exportTo( ui->qwtPlot, saveName+".pdf" );
}


