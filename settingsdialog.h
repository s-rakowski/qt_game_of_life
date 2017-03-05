#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    void setActiveColor(const QColor &col);
    void setInactiveColor(const QColor &col);
    void setBorderColor(const QColor &col);

    QColor getActiveColor();
    QColor getInactiveColor();
    QColor getBorderColor();
    int getInterval();
    QString getLanguage();


private slots:
    void on_activeColorButton_clicked();

    void on_borderColorButton_clicked();

    void on_inactiveColorButton_clicked();

private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
