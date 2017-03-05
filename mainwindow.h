#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QImage>
#include <QTimer>
#include <QSettings>
#include "imagemodel.h"
#include "celldelegate.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_zoomSlider_sliderMoved(int position);

    void on_actionAbout_Qt_triggered();

    void on_actionAbout_triggered();

    void on_actionStart_Stop_toggled(bool arg1);

    void on_actionSettings_triggered();

    void on_actionNew_triggered();

    void on_actionStep_triggered();

    void on_tableView_clicked(const QModelIndex &index);

    void on_tableView_entered(const QModelIndex &index);

    void on_actionErase_the_grid_triggered();

    void on_actionInvert_all_cells_triggered();

    void on_actionRandomize_all_triggered();

    void on_actionReset_generation_counter_triggered();

    void on_actionSave_triggered();

    void on_actionQuit_triggered();

    void on_actionLoad_triggered();

protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::MainWindow *ui;
    QStringList filters;
    QImage cellsImage;
    QTimer redrawTimer;
    ImageModel *model;
    CellDelegate *delegate;
    QSettings *applicationSettings;
    int generationCounter;
    bool cellsModified;

    void do_resize();
    void do_initialize(QSize fieldSize);
    void do_calculate();

    bool trySave();
    bool tryOpen(QString &fname);

    void fastSetPixel(int x, int y, uchar val, QImage *pic);
    uchar fastGetPixel(int x, int y, const QImage &pic);
    short neighbors(int x, int y, const QImage &pic);
};

#endif // MAINWINDOW_H
