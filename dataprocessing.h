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
    Ui::dataProcessing *ui;
    QVector<dataInput*> inputDialog;
    QVector<dataAnalysis*> analysisDialog;
    QStringList inputData;
};

#endif // DATAPROCESSING_H
