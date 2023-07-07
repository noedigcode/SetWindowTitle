#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "WindowFunctions.h"

#include <QBasicTimer>
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>

#include <iostream>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define APP_VERSION "1.0.0"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_changeTitle_clicked();
    void on_pushButton_unsetTitle_clicked();
    void on_lineEdit_title_returnPressed();

private:
    Ui::MainWindow *ui;
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);

    bool wantToQuit = false;
    void quit();

    QSystemTrayIcon trayIcon {this};
    void setupTrayIcon();

    void print(const QString &msg);

    QBasicTimer timer;
    void timerEvent(QTimerEvent *event);

    HWND lastActiveWin = 0;

    struct WinData
    {
        bool setTitle = false;
        QString title;
    };
    QMap<HWND, WinData> winDataMap;

    void updateActiveWinWidgets();
};
#endif // MAINWINDOW_H
