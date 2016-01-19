#ifndef DATAANALYSIS_H
#define DATAANALYSIS_H

#include <QDialog>
class QTableWidget;

namespace Ui {
class dataAnalysis;
}

class dataAnalysis : public QDialog
{
    Q_OBJECT

public:
    explicit dataAnalysis(const QTableWidget*,int);
    ~dataAnalysis();

public slots:
    void onSaveClicked();

public:
    Ui::dataAnalysis *ui;
    QVector<double> xs;
    QVector<double> ys;
    double Fmin,Fmax,Emin,Emax;
    QString saveName;
};

#endif // DATAANALYSIS_H
