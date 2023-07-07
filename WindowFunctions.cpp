#include "WindowFunctions.h"

QString WindowFunctions::getWindowTitle(HWND h)
{
    char title[256];
    GetWindowTextA(h, title, 256);
    QString t((const char*)title);
    return t;
}

void WindowFunctions::setWindowTitle(HWND h, const QString &text)
{
    SetWindowTextA(h, text.toLocal8Bit().constData());
}

bool WindowFunctions::isWindow(HWND h)
{
    return IsWindow(h);
}
