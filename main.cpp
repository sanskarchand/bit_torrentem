#include "include/mainwindow.h"
#include "include/torrentinfo.h"
#include <QApplication>
#include <QLabel>

#include <string>
#include <iostream>
#include <assert.h>
#include <iterator>
#include <fstream>
void loadMetainfoFile(const char *file_name)
{
    std::ifstream torr_fs(file_name);
    std::string torr_info((std::istreambuf_iterator<char>(torr_fs)), std::istreambuf_iterator<char>());

    assert("Empty file" && torr_info.size() > 0);

}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow win_main;

    //loadMetainfoFile("tsukihime.torrent");
    QLabel *label = new QLabel(&win_main);
    label->setText("Print a torrent file to stdout\n");

    Parser *pr = new Parser();

    win_main.show();
    return app.exec();
}
