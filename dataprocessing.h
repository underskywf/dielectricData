#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

#include <QMainWindow>

class dataInput;
class dataAnalysis;

namespace Ui {
class dataProcessing;
}

class dataProcessing : public QMainWindow
{
    Q_OBJECT

public:
    explicit dataProcessing(QWidget *parent = 0);
    ~dataProcessing();
    void init();

public slots:

    void onOpenDataClicked();
    void onDataSetClicked();
    void onCalculateClicked();
    void onSaveDataClicked();
    void onDrawCurveClicked();
    void onExitClicked();

    void receiveData(const QStringList inputData);

private:
    void y2o3Pure(double &Epsilon1,double &Epsilon11,const double &temperature,const double &frequency );
    void y2o3Na2O(double &Epsilon1,double &Epsilon11,const double &temperature,const double &frequency );
    void y2o3Al2O3(double &Epsilon1,double &Epsilon11,const double &temperature,const double &frequency );

private:
    Ui::dataProcessing *ui;
    QVector<dataInput*> inputDialog;
    QVector<dataAnalysis*> analysisDialog;
    QStringList inputData;

    double MEXVal;
};

#endif // DATAPROCESSING_H
