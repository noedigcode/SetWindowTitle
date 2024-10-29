#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "WindowFunctions.h"
#include "settings.h"

#include <QBasicTimer>
#include <QCloseEvent>
#include <QMainWindow>
#include <QMenu>
#include <QRegularExpression>
#include <QSystemTrayIcon>

#include <iostream>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define APP_VERSION "1.0.2-dev"

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
    void on_lineEdit_title_selectionChanged();
    void on_lineEdit_title_textChanged(const QString &arg1);
    void on_toolButton_crop_clicked();

    void on_lineEdit_regex_textChanged(const QString &arg1);

    void on_pushButton_addPreset_clicked();

    void on_toolButton_presets_remove_clicked();

private:
    Ui::MainWindow *ui;
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);

    Settings mSettings;

    QString mSelectedText;

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
        Settings::PresetPtr preset;
    };
    QMap<HWND, WinData> winDataMap;

    struct Preset
    {
        Settings::PresetPtr setting;
        QRegularExpression re;
    };
    QList<Preset> presets;

    void updateActiveWinWidgets();
};
#endif // MAINWINDOW_H
