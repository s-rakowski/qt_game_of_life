#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QColorDialog>
#include <QSettings>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    setModal(true);
    ui->setupUi(this);

    QSettings settings;
    QVariant var = settings.value("active_color", QColor("black"));
    setActiveColor(var.value<QColor>());

    var = settings.value("inactive_color", QColor("white"));
    setInactiveColor(var.value<QColor>());

    var = settings.value("background_color", QColor("white"));
    setBorderColor(var.value<QColor>());

    ui->intervalBox->setValue(settings.value("update_interval", 100).toInt());

    QVariant settingsLanguage = settings.value("language", QString("auto"));
    QVariant languageItem;
    QString str;

    for(int i = 0; i < ui->languageBox->count(); ++i)
    {
        languageItem = ui->languageBox->itemData(i, Qt::DisplayRole);
        str = languageItem.toString().split('/').at(1);
        if(str == settingsLanguage.toString())
        {
            ui->languageBox->setCurrentIndex(i);
            break;
        }
    }
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::setActiveColor(const QColor &col)
{
    QPalette pal = ui->activeColorLabel->palette();
    pal.setColor(QPalette::Window, col);
    ui->activeColorLabel->setPalette(pal);
}

void SettingsDialog::setInactiveColor(const QColor &col)
{
    QPalette pal = ui->inactiveColorLabel->palette();
    pal.setColor(QPalette::Window, col);
    ui->inactiveColorLabel->setPalette(pal);
}

void SettingsDialog::setBorderColor(const QColor &col)
{
    QPalette pal = ui->borderColorLabel->palette();
    pal.setColor(QPalette::Window, col);
    ui->borderColorLabel->setPalette(pal);
}

QColor SettingsDialog::getActiveColor()
{
    return ui->activeColorLabel->palette().color(QPalette::Window);
}

QColor SettingsDialog::getInactiveColor()
{
    return ui->inactiveColorLabel->palette().color(QPalette::Window);
}

QColor SettingsDialog::getBorderColor()
{
    return ui->borderColorLabel->palette().color(QPalette::Window);
}

int SettingsDialog::getInterval()
{
    return ui->intervalBox->value();
}

QString SettingsDialog::getLanguage()
{
    QVariant languageItem = ui->languageBox->itemData(ui->languageBox->currentIndex(), Qt::DisplayRole);
    return languageItem.toString().split('/').at(1);
}

void SettingsDialog::on_activeColorButton_clicked()
{
    QColor col = QColorDialog::getColor(getActiveColor(), this);
    if(col.isValid())
        setActiveColor(col);
}

void SettingsDialog::on_borderColorButton_clicked()
{
    QColor col = QColorDialog::getColor(getBorderColor(), this);
    if(col.isValid())
        setBorderColor(col);
}

void SettingsDialog::on_inactiveColorButton_clicked()
{
    QColor col = QColorDialog::getColor(getInactiveColor(), this);
    if(col.isValid())
        setInactiveColor(col);
}
