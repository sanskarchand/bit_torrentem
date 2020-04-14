#include "include/mainwindow.h"
#include "include/parser.hpp"
#include <QApplication>
#include <QLabel>

#include <string>
#include <iostream>
#include <assert.h>
#include <iterator>
#include <fstream>
std::string loadMetainfoFile(const char *file_name)
{
    // NB: apparently inefficient for large files
    std::ifstream torr_fs(file_name);
    std::string torr_info((std::istreambuf_iterator<char>(torr_fs)), std::istreambuf_iterator<char>());

    assert("Empty file" && torr_info.size() > 0);

    return torr_info;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow win_main;

    // C should allow this
    const char *test_torrent_file="/home/drakinosh/Downloads/"
        "[BakaBT.164462v1] Carnival Phantasm [UTW] [h264 1080p FLAC].torrent";

    QLabel *label = new QLabel(&win_main);
    label->setText("Print a torrent file to stdout");


    //Parser *pr = new Parser();
    std::string torr_data = loadMetainfoFile(test_torrent_file);

    // TEST:get announce string
    Parser p;
    //std::cout << p.parseByteString(torr_data, 11) << std::endl;
    ParsedObject po = p.parseDictionary(torr_data, 0);
    //NOTABENE APR13,no GUI for now
    //win_main.show();
    //return app.exec();
}
