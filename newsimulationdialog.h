#ifndef NEWSIMULATIONDIALOG_H
#define NEWSIMULATIONDIALOG_H

#include <QDialog>

namespace Ui {
class NewSimulationDialog;
}

class NewSimulationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewSimulationDialog(QWidget *parent = 0);
    ~NewSimulationDialog();
    int getWidth();
    int getHeight();
    void setWidth(int val);
    void setHeight(int val);

private:
    Ui::NewSimulationDialog *ui;
};

#endif // NEWSIMULATIONDIALOG_H
