#include "include/mainwindow.h"
#include "include/parser.hpp"
#include "include/torrent.hpp"
#include "include/bt_network/network.hpp"
#include "include/bt_utils/utils.hpp"
#include "include/bt_utils/urlencode.hpp"
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

    BtParser::ParsedObject po = BtParser::parseDictionary(torr_data, 0);
    BtParser::iteratePrintDict(&po);
    BtTorrent::Torrent torr_obj(&po, torr_data);

    //BtParser::ParsedObject info_obj = po.po_dictval["info"];
    // std::string info = BtParser::getInfoSubString(torr_data, &po.po_dictval["info"]);

    //std::cout << info  << std::endl;
    BtNetwork::TorrentNetworkHandler tnh = BtNetwork::TorrentNetworkHandler(&torr_obj);
    tnh.networkMainLoop();

    //tnh.connectToTracker(0);
    //tnh.connectToTracker();


    //NOTABENE APR13,no GUI for now
    //win_main.show();
    //return app.exec();
}
