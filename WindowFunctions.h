#ifndef WINDOWFUNCTIONS_H
#define WINDOWFUNCTIONS_H

#include <QString>

#include <windows.h>
#include <winuser.h>

class WindowFunctions
{
public:

    static QString getWindowTitle(HWND h);
    static void setWindowTitle(HWND h, const QString &text);
    static bool isWindow(HWND h);
};

#endif // WINDOWFUNCTIONS_H
