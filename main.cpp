#include "mainwindow.h"
#include <QApplication>
#include <QSettings>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName(QString("Qt Game of Life"));
    a.setApplicationVersion(QString("1.0"));
    a.setOrganizationName(QString("Szymon"));
    a.setOrganizationDomain(QString("ciupa.ga"));

    QSettings settings;
    QString language = settings.value("language", QString("auto")).toString();

    if(language == "auto")
        language = QLocale::system().name();

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + language, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    QTranslator myTranslator;
    myTranslator.load(QString("gameoflife_") + language);

    a.installTranslator(&myTranslator);

    MainWindow w;
    w.show();

    return a.exec();
}
