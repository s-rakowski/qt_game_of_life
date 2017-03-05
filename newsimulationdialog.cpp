#include "newsimulationdialog.h"
#include "ui_newsimulationdialog.h"
#include "qsettings.h"

NewSimulationDialog::NewSimulationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewSimulationDialog)
{
    ui->setupUi(this);
    QSettings settings;
    QSize fsize = settings.value("field_size", QSize(100, 100)).toSize();
    ui->heightBox->setValue(fsize.height());
    ui->widthBox->setValue(fsize.width());
}

NewSimulationDialog::~NewSimulationDialog()
{
    delete ui;
}

int NewSimulationDialog::getWidth()
{
    return ui->widthBox->value();
}

int NewSimulationDialog::getHeight()
{
    return ui->heightBox->value();
}

void NewSimulationDialog::setWidth(int val)
{
    ui->widthBox->setValue(val);
}

void NewSimulationDialog::setHeight(int val)
{
    ui->heightBox->setValue(val);
}
