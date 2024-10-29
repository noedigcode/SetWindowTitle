#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QIcon icon;
    icon.addFile("://appicon_32.png");
    icon.addFile("://appicon_16.png");
    this->setWindowIcon(icon);

    this->setWindowTitle(QString("%1 %2").arg(this->windowTitle()).arg(APP_VERSION));
    this->setWindowFlag(Qt::WindowStaysOnTopHint, true);

    qApp->installEventFilter(this);
    timer.start(1000, this);
    setupTrayIcon();
    updateActiveWinWidgets();

    mSettings.load();
    foreach (Settings::PresetPtr settingPreset, mSettings.presets) {
        Preset p;
        p.setting = settingPreset;
        p.re.setPattern(p.setting->regexPattern);
        presets.append(p);
        ui->listWidget_presets->addItem(QString("re(%1) --> %2")
                                        .arg(p.setting->regexPattern)
                                        .arg(p.setting->windowTitle));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::print(const QString& msg)
{
    std::cout << msg.toStdString() << std::endl;
}

void MainWindow::timerEvent(QTimerEvent* /*event*/)
{
    // Update GUI with currently active window

    HWND h = GetForegroundWindow();

    if ((h) && (!winDataMap.contains(h)) && (h != (HWND)this->winId())) {
        // Add window to data structure
        winDataMap.insert(h, WinData());
    }

    // Ignore our window
    if ((h != (HWND)this->winId())) {
        lastActiveWin = h;
    }

    // Update widgets if line edit does not have focus, as not to interrupt
    // user while typing
    if (!ui->lineEdit_title->hasFocus()) {
        updateActiveWinWidgets();
    }

    // ----------------------------------------------------

    QList<HWND> allWindows = WindowFunctions::getAllWindowHandles();
    foreach (HWND h, allWindows) {
        if (!WindowFunctions::isWindow(h)) { continue; }
        if (winDataMap.contains(h)) {
            WinData& w = winDataMap[h];
            if (w.setTitle) {
                // Title already set, skip
                continue;
            }
        }

        QString title = WindowFunctions::getWindowTitle(h);
        foreach (const Preset& p, presets) {
            QRegularExpressionMatch match = p.re.match(title);
            if (match.hasMatch()) {
                if (!winDataMap.contains(h)) {
                    winDataMap.insert(h, WinData());
                }
                WinData& w = winDataMap[h];
                w.setTitle = true;
                w.title = p.setting->windowTitle;
                w.preset = p.setting;
                //WindowFunctions::setWindowTitle(h, w.title);
                break;
            }
        }
    }

    // ----------------------------------------------------

    // Refresh all window titles
    foreach (HWND h, winDataMap.keys()) {
        if (!WindowFunctions::isWindow(h)) {
            // Window no longer exists. Remove from data structure
            winDataMap.remove(h);
        } else {
            // Refresh window title
            WinData& w = winDataMap[h];
            if (w.setTitle) {
                WindowFunctions::setWindowTitle(h, w.title);
            }
        }
    }
}

void MainWindow::updateActiveWinWidgets()
{
    if (winDataMap.contains(lastActiveWin)) {
        WinData& w = winDataMap[lastActiveWin];
        ui->lineEdit_title->setText(WindowFunctions::getWindowTitle(lastActiveWin));
        ui->lineEdit_title->setEnabled(true);
        ui->pushButton_changeTitle->setEnabled(true);
        ui->pushButton_unsetTitle->setEnabled(w.setTitle);
    } else {
        ui->lineEdit_title->clear();
        ui->lineEdit_title->setEnabled(false);
        ui->pushButton_changeTitle->setEnabled(false);
        ui->pushButton_unsetTitle->setEnabled(false);
    }
}

void MainWindow::on_pushButton_changeTitle_clicked()
{
    HWND h = lastActiveWin;
    if (!winDataMap.contains(h)) { return; }

    QString title = ui->lineEdit_title->text();

    if (!title.isEmpty()) {
        WinData& w = winDataMap[h];
        w.setTitle = true;
        w.title = title;
        WindowFunctions::setWindowTitle(h, w.title);

        updateActiveWinWidgets();
    }
}

void MainWindow::on_pushButton_unsetTitle_clicked()
{
    HWND h = lastActiveWin;
    if (!winDataMap.contains(h)) { return; }

    WinData& w = winDataMap[h];
    w.setTitle = false;

    updateActiveWinWidgets();
}

void MainWindow::on_lineEdit_title_returnPressed()
{
    // If text is selected, use selection. Otherwise, use all text
    if (!mSelectedText.isEmpty()) {
        on_toolButton_crop_clicked();
    } else {
        on_pushButton_changeTitle_clicked();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Simply hide the window but keep running in the system tray
    this->hide();
    event->ignore();
}

bool MainWindow::eventFilter(QObject* /*watched*/, QEvent *event)
{
    bool handled = false;
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent) {
            handled = true;
            switch (keyEvent->key()) {
            case Qt::Key_Escape:
                // Hide window when escape is pressed
                this->hide();
                break;
            default:
                handled = false;
            }
        }
    }
    return handled;
}

void MainWindow::quit()
{
    wantToQuit = true;
    qApp->quit();
}

void MainWindow::setupTrayIcon()
{
    trayIcon.setIcon(this->windowIcon());
    trayIcon.setToolTip("Set Window Title");
    connect(&trayIcon, &QSystemTrayIcon::activated,
            this, [=](QSystemTrayIcon::ActivationReason reason)
    {
        if (reason == QSystemTrayIcon::Context) { return; }
        this->setVisible(!this->isVisible());
    });

    QMenu* trayMenu = new QMenu(this);
    trayMenu->addAction("Quit", this, [=]()
    {
        quit();
    });
    trayIcon.setContextMenu(trayMenu);

    trayIcon.show();
}

void MainWindow::on_lineEdit_title_selectionChanged()
{
    // Store selected text for when crop button is pressed.
    // This is necessary as clicking the crop button changes focus away from
    // the line edit, clearing the selection.
    QString selected = ui->lineEdit_title->selectedText();
    if (!selected.isEmpty()) {
        mSelectedText = selected;
    }
}

void MainWindow::on_lineEdit_title_textChanged(const QString& /*arg1*/)
{
    // Clear the stored selected text so we don't have confusing lingering
    // selections from previous windows.
    mSelectedText.clear();
}

void MainWindow::on_toolButton_crop_clicked()
{
    if (mSelectedText.isEmpty()) { return; }

    ui->lineEdit_title->setText(mSelectedText);
    on_pushButton_changeTitle_clicked();
}

void MainWindow::on_lineEdit_regex_textChanged(const QString& /*arg1*/)
{
    QRegularExpression re(ui->lineEdit_regex->text());

    ui->listWidget->clear();

    QList<HWND> windows = WindowFunctions::getAllWindowHandles();
    foreach (HWND h, windows) {
        QString title = WindowFunctions::getWindowTitle(h);
        QRegularExpressionMatch match = re.match(title);
        if (match.hasMatch()) {
            ui->listWidget->addItem(title);
        }
    }
}

void MainWindow::on_pushButton_addPreset_clicked()
{
    Preset p;
    p.setting.reset(new Settings::Preset());
    p.setting->regexPattern = ui->lineEdit_regex->text();
    p.setting->windowTitle = ui->lineEdit_presetTitle->text();

    p.re.setPattern(p.setting->regexPattern);

    mSettings.presets.append(p.setting);
    mSettings.save();

    presets.append(p);
    ui->listWidget_presets->addItem(QString("re(%1) --> %2")
                                    .arg(p.setting->regexPattern)
                                    .arg(p.setting->windowTitle));
}

void MainWindow::on_toolButton_presets_remove_clicked()
{
    int index = ui->listWidget_presets->currentRow();
    if (index < 0) { return; }
    if (index >= presets.count()) { return; }

    QListWidgetItem* item = ui->listWidget_presets->item(index);
    if (!item) { return; }
    delete item;

    Preset& preset = presets[index];
    mSettings.presets.removeAll(preset.setting);

    presets.removeAt(index);

    // Unset all window titles using this preset
    foreach (HWND h, winDataMap.keys()) {
        WinData& win = winDataMap[h];
        if (win.preset == preset.setting) {
            win.setTitle = false;
            win.preset.reset();
        }
    }
}

