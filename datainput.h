#ifndef DATAINPUT_H
#define DATAINPUT_H

#include <QDialog>

namespace Ui {
class dataInput;
}

class dataInput : public QDialog
{
    Q_OBJECT

public:
    explicit dataInput(QWidget *parent = 0);
    ~dataInput();

    void init();

public slots:
    void onOkClicked();

signals:
    void sendData(const QStringList);

private:
    Ui::dataInput *ui;
};

#endif // DATAINPUT_H
