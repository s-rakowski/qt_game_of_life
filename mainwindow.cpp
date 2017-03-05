#include <QMessageBox>
#include <QDateTime>
#include <QFileDialog>
#include <QSettings>
#include <QMimeType>
#include <QMimeDatabase>
#include <QTextStream>
#include <QCloseEvent>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "newsimulationdialog.h"
#include "imagemodel.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qsrand(QDateTime::currentMSecsSinceEpoch());

    ui->setupUi(this);
    setCentralWidget(ui->tableView);
    ui->statusBar->addPermanentWidget(ui->zoomSlider, 0);

    filters << "image/png" << "image/bmp" << "text/plain";

    applicationSettings = new QSettings();
    ui->zoomSlider->setValue(applicationSettings->value("cell_size", 8).toInt());


    ui->tableView->setShowGrid(false);
    ui->tableView->horizontalHeader()->hide();
    ui->tableView->verticalHeader()->hide();
    model = new ImageModel();
    ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->setMinimumSectionSize(1);
    ui->tableView->verticalHeader()->setMinimumSectionSize(1);

    delegate = new CellDelegate();
    delegate->setCellSize(ui->zoomSlider->value());
    if(ui->zoomSlider->value() < 8)
        delegate->setBorderSize(0);
    else if(ui->zoomSlider->value() < 32)
        delegate->setBorderSize(1);
    else
        delegate->setBorderSize(3);

    QVariant var = applicationSettings->value("active_color", QColor("black"));
    delegate->setActiveColor(var.value<QColor>());

    var = applicationSettings->value("inactive_color", QColor("white"));
    delegate->setInactiveColor(var.value<QColor>());

    var = applicationSettings->value("background_color", QColor("white"));
    delegate->setBackgroundColor(var.value<QColor>());

    ui->tableView->setItemDelegate(delegate);

    do_initialize(applicationSettings->value("field_size", QSize(100, 100)).toSize());
    do_resize();

    redrawTimer.setInterval(applicationSettings->value("update_interval", 100).toInt());
    redrawTimer.stop();
    redrawTimer.setSingleShot(false);
    connect(&redrawTimer, SIGNAL(timeout()), this, SLOT(on_actionStep_triggered()));
}

MainWindow::~MainWindow()
{
    delete applicationSettings;
    delete model;
    delete delegate;
    delete ui;
}

// Images helper functions
void MainWindow::fastSetPixel(int x, int y, uchar val, QImage* pic)
{
    uchar *ptr = pic->scanLine(y);
    if(y < 0 || x < 0 || x >= pic->width() || y >= pic->height())
        return;

    short int shift = 7 - (x % 8);
    uchar tmp = ptr[x / 8] & ~(uchar)(0x01 << shift);
    ptr[x / 8] = tmp | ((val & 0x01) << shift);
}

uchar MainWindow::fastGetPixel(int x, int y, const QImage &pic)
{
   x = (x + pic.width()) % pic.width();
   y = (y + pic.height()) % pic.height();

   const uchar* ptr = pic.constScanLine(y);

   return (ptr[x / 8] >> (7 - (x % 8))) & 0x01;
}

short int MainWindow::neighbors(int x, int y, const QImage &pic)
{
    const short int dx[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    const short int dy[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    short int num = 0;

    for(int i = 0; i < 8; ++i)
    {
        num += fastGetPixel(x + dx[i], y + dy[i], pic);
    }

    return num;
}

// Helper funtions
void MainWindow::do_resize()
{
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
}

void MainWindow::do_initialize(QSize fieldSize)
{
    generationCounter = 0;

    cellsImage = QImage(fieldSize, QImage::Format_Mono);
    cellsImage.fill(0);

    model->setImage(cellsImage);

    cellsModified = false;
}

void MainWindow::do_calculate()
{
    QImage oldImage = model->image();

    int minx = cellsImage.size().width();
    int miny = cellsImage.size().height();
    int maxx = 0;
    int maxy = 0;

    for(int y = 0; y < cellsImage.size().height(); ++y)
    {
        for(int x = 0; x < cellsImage.size().width(); ++x)
        {
            short int n = neighbors(x, y, oldImage);
            uchar current_state, new_state;
            current_state = fastGetPixel(x, y, oldImage);
            if((current_state == 1) && ((n < 2) || (n > 3)))
                new_state = 0;
            else if((current_state == 0) && (n == 3))
                new_state = 1;
            else
                new_state = current_state;

            if(new_state != current_state)
            {
                maxx = qMax(maxx, x);
                maxy = qMax(maxy, y);
                minx = qMin(minx, x);
                miny = qMin(miny, y);
            }

            fastSetPixel(x, y, new_state, &cellsImage);
        }
    }
    model->setImage(cellsImage, false);
    model->dataChanged(model->index(miny, minx), model->index(maxy, maxx));
}

bool MainWindow::trySave()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setMimeTypeFilters(filters);
    dialog.setWindowTitle(tr("Save game state to a file"));
    if(dialog.exec() == QFileDialog::Accepted)
    {
        QString fname = dialog.selectedFiles().first();
        QMimeDatabase db;
        QList<QMimeType> foundTypes = db.mimeTypesForFileName(fname);

        cellsImage = model->image();
        if(foundTypes.empty() == false && foundTypes.first().name().split('/').first() == "image")
        {
            cellsImage.invertPixels();
            cellsImage.save(fname);
            return true;
        }
        else
        {
            QFile textFile(fname);
            if(!textFile.open(QIODevice::Text | QIODevice::WriteOnly))
            {
                QMessageBox::critical(this, tr("Error"), tr("Could not open the file %1 for writing.").arg(fname));
                return false;
            }
            QTextStream stream(&textFile);
            for(int row = 0; row < cellsImage.height(); ++row)
            {
                for(int column = 0; column < cellsImage.width(); ++column)
                    stream << ((fastGetPixel(column, row, cellsImage) == 1) ? 'O' : '.');
                stream << '\n';
            }
            stream.flush();
            textFile.close();
            return true;
        }
    }
    return false;
}

bool MainWindow::tryOpen(QString &fname)
{
    QMimeDatabase db;
    QMimeType ftype = db.mimeTypeForFile(fname);
    if(ftype.isValid() == false)
        return false;

    if(ftype.name().split('/').first() == "image" && cellsImage.load(fname))
    {
        if(cellsImage.width() > 1000)
            cellsImage = cellsImage.scaledToWidth(1000);
        if(cellsImage.height() > 1000)
            cellsImage = cellsImage.scaledToHeight(1000);
        if(cellsImage.format() != QImage::Format_Mono)
        {
            QImage conv = cellsImage.convertToFormat(QImage::Format_Mono, Qt::MonoOnly | Qt::DiffuseDither | Qt::PreferDither);
            cellsImage = conv;
            conv.invertPixels();
            model->setImage(conv);
        }
        else
        {
            cellsImage.invertPixels();
            model->setImage(cellsImage);
        }
    }
    else if(ftype.inherits("text/plain"))
    {
       QFile textFile(fname);
       if(!textFile.open(QIODevice::ReadOnly | QIODevice::Text))
           return false;
       QTextStream stream(&textFile);

       QString line;
       int rows = 0, columns = 0;
       while(stream.readLineInto(&line))
       {
           ++rows;
           columns = qMax(columns, line.length());
       }
       stream.seek(0);

       cellsImage = QImage(QSize(columns, rows), QImage::Format_Mono);
       cellsImage.fill(0);
       for(int row = 0; row < rows; ++row)
       {
           stream.readLineInto(&line);
           for(int column = 0; column < line.length(); ++column)
               fastSetPixel(column, row, (line[column].toUpper() == 'O'), &cellsImage);
       }
       model->setImage(cellsImage);
       textFile.close();
    }

    generationCounter = 0;
    cellsModified = false;
    do_resize();
    return true;
}

// ---------------

void MainWindow::on_zoomSlider_sliderMoved(int position)
{
    ui->statusBar->showMessage(tr("Cell size set to %1.").arg(position), 500);
    delegate->setCellSize(position);
    if(position < 8)
        delegate->setBorderSize(0);
    else if(position < 32)
        delegate->setBorderSize(1);
    else
        delegate->setBorderSize(3);
    do_resize();
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About Qt Game of Life"),\
                       tr("A simple implementation of Conway's Game of Life with Qt GUI.\nAuthor: Szymon Rakowski\n"
                          "Uses icons by FatCow Web Hosting, which are Creative Commons Attribution 3.0 licensed.\n"
                          "http://www.fatcow.com/free-icons"));
}

void MainWindow::on_actionStart_Stop_toggled(bool arg1)
{
    if(arg1)
    {
        ui->statusBar->showMessage(tr("Start"), 2000);
        redrawTimer.start();
    }
    else
    {
        ui->statusBar->showMessage(tr("Stop"), 2000);
        redrawTimer.stop();
    }
}

void MainWindow::on_actionSettings_triggered()
{
    SettingsDialog mySettings(this);
    if(mySettings.exec() == QDialog::Accepted)
    {
        delegate->setActiveColor(mySettings.getActiveColor());
        delegate->setInactiveColor(mySettings.getInactiveColor());
        delegate->setBackgroundColor(mySettings.getBorderColor());
        model->dataChanged(model->index(0, 0), model->index(model->rowCount() - 1, model->columnCount() - 1));
        redrawTimer.setInterval(mySettings.getInterval());

        applicationSettings->setValue("active_color", mySettings.getActiveColor());
        applicationSettings->setValue("inactive_color", mySettings.getInactiveColor());
        applicationSettings->setValue("background_color", mySettings.getBorderColor());
        applicationSettings->setValue("update_interval", mySettings.getInterval());
        QString oldLanguage = applicationSettings->value("language").toString();
        if(oldLanguage != mySettings.getLanguage())
            QMessageBox::warning(this, tr("Warning"), tr("You must restart this application in order to make the language change effective."));
        applicationSettings->setValue("language", mySettings.getLanguage());
        ui->statusBar->showMessage(tr("Settings saved."), 2000);
    }
}

void MainWindow::on_actionNew_triggered()
{
    NewSimulationDialog newDialog(this);
    if(newDialog.exec() == QDialog::Accepted)
    {
        QSize fsize = QSize(newDialog.getWidth(), newDialog.getHeight());
        applicationSettings->setValue("field_size", fsize);
        do_initialize(fsize);
        do_resize();
    }

}

void MainWindow::on_actionStep_triggered()
{
    do_calculate();
    generationCounter++;
    ui->statusBar->showMessage(tr("Generation number: %1").arg(generationCounter));
}

void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    if(redrawTimer.isActive() == false)
    {
        int col = model->data(index).toInt();
        if(col == 0)
            col = 1;
        else
            col = 0;
        model->setData(index, QVariant(col), Qt::EditRole);
        cellsModified = true;
    }
}

void MainWindow::on_tableView_entered(const QModelIndex &index)
{
    if(redrawTimer.isActive() == false)
        ui->statusBar->showMessage(QString("X = %1, Y = %2").arg(index.column()).arg(index.row()));
}

void MainWindow::on_actionErase_the_grid_triggered()
{
    do_initialize(cellsImage.size());
    do_resize();
}

void MainWindow::on_actionInvert_all_cells_triggered()
{
    cellsImage = model->image();
    cellsImage.invertPixels();
    model->setImage(cellsImage);
}

void MainWindow::on_actionRandomize_all_triggered()
{
    for(int y = 0; y < cellsImage.size().height(); ++y)
    {
        uchar *ptr = cellsImage.scanLine(y);
        for(int x = 0; x < (cellsImage.size().width() / 8); ++x)
            ptr[x] = qrand() % 256;

        if(cellsImage.size().width() % 8 != 0)
            ptr[cellsImage.size().width() / 8] = qrand() % 255;
    }
    model->setImage(cellsImage);
}

void MainWindow::on_actionReset_generation_counter_triggered()
{
    generationCounter = 0;
}

void MainWindow::on_actionSave_triggered()
{
    trySave();
}

void MainWindow::on_actionQuit_triggered()
{
    close();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(cellsModified == false)
    {
        applicationSettings->setValue("cell_size", ui->zoomSlider->value());
        event->accept();
    }
    else
    {
        QMessageBox::StandardButton res = QMessageBox::question(this, tr("Question"), \
            tr("Do you want to save current state?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, \
                                                    QMessageBox::Cancel);
        if(res == QMessageBox::No || (res == QMessageBox::Yes && trySave()))
        {
            applicationSettings->setValue("cell_size", ui->zoomSlider->value());
            event->accept();
        }
        else
            event->ignore();
    }
}

void MainWindow::on_actionLoad_triggered()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setMimeTypeFilters(filters);
    dialog.setWindowTitle(tr("Load game state from a file"));
    if(dialog.exec() == QFileDialog::Accepted)
    {
        if(tryOpen(dialog.selectedFiles().first()) == false)
            QMessageBox::critical(this, tr("Error"), tr("Could not open file %1.").arg(dialog.selectedFiles().first()));
    }
}
