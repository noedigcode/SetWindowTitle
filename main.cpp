#include "mainwindow.h"
#include "version.h"

#include <QApplication>
#include <QCommandLineParser>

#include <iostream>

void print(QString msg)
{
    std::cout << msg.toStdString() << std::endl;
}

void printVersion()
{
    print(QString(APP_NAME));
    print(QString("Version %1").arg(APP_VERSION));
    print(QString("Gideon van der Kolf %1-%2").arg(APP_YEAR_FROM).arg(APP_YEAR));
    print("");
    print("Compiled with Qt " + QString(QT_VERSION_STR));
    print("");
}

int main(int argc, char *argv[])
{
    printVersion();

    QApplication a(argc, argv);
    QApplication::setApplicationName(APP_NAME);
    QApplication::setApplicationVersion(APP_VERSION);

    QCommandLineParser parser;
    parser.addHelpOption();
    QCommandLineOption versionOption({"v", "version"}, "Display version information");
    parser.addOption(versionOption);

    parser.process(a);

    if (parser.isSet(versionOption)) {
        // Version was already printed at start
        return 0;
    }

    MainWindow w;
    w.show();
    return a.exec();
}
