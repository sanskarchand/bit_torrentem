#include "include/mainwindow.h"
#include "include/parser.hpp"
#include "include/torrent.hpp"
#include "include/bt_network/network.hpp"
#include "include/bt_utils/utils.hpp"
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
    const char *test_torrent_file="../[Natsu] Yuru Camp  △ [BD][1080P][AAC][HEVC].torrent";

    QLabel *label = new QLabel(&win_main);
    label->setText("Print a torrent file to stdout");


    std::string torr_data = loadMetainfoFile(test_torrent_file);
    Parser p;
    ParsedObject po = p.parseDictionary(torr_data, 0);
    //iteratePrintDict(&po);
    Torrent torr_obj(&po);

    std::string test_string = "The quick brown fox jumps over the lazy dog";
    std::cout << "SHA1 test | string = " << test_string << std::endl;
    std::cout << "Hash: " << std::endl;
    std::string hash_str = calculateSHA1(test_string);

    // beware of sign extension
    for (unsigned char c: hash_str) {
        printf("%x", c);
    }
    printf("\n");


    //TorrentNetworkHandler tnh = TorrentNetworkHandler(&torr_obj);
    //tnh.connectToTracker();


    //NOTABENE APR13,no GUI for now
    //win_main.show();
    //return app.exec();
}
