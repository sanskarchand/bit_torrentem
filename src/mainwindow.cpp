#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("BitTorrentem");
    setMinimumSize(WIN_WIDTH, WIN_HEIGHT);
}

MainWindow::~MainWindow()
{
}

