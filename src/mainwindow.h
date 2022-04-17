#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define WIN_WIDTH 640
#define WIN_HEIGHT 480

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif // MAINWINDOW_H
