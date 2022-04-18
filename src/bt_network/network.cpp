#include "bt_network/network.hpp"

#include <unistd.h>
#include <iostream> //remove this
#include <stdio.h>  //remove this
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <cpr/cpr.h>


namespace BtNetwork {

TorrentNetworkHandler::TorrentNetworkHandler(BtTorrent::Torrent *torr_obj)
    : m_torrent(torr_obj), m_status(NS_WAIT_TRACKER_CONNECT), m_current_tracker(0)
{

}



TrackerRequest TorrentNetworkHandler::generateTrackerRequest(std::string port_listen)
{
    TrackerRequest req;

    std::string p_info_string = BtParser::getInfoSubString(
                m_torrent->t_torrent_file,
                &m_torrent->t_torrent_dict->po_dictval["info"]);

    p_info_string = BtUtils::calculateSHA1(p_info_string);
    //p_info_string = BtUtils::urlEncode(p_info_string);

    req.info_hash = p_info_string;
    req.peer_id = BtUtils::urlEncode(BtUtils::generatePeerID());
    req.port = port_listen;
    req.uploaded = m_torrent->t_uploaded_bytes;
    req.downloaded = m_torrent->t_downloaded_bytes;
    req.left = m_torrent->t_total_size - m_torrent->t_downloaded_bytes;
    req.compact = 0;
    req.no_peer_id = false;
    req.event = BT_NET_EVENT_STARTED;   // obsolete


    return req;
}

int TorrentNetworkHandler::initialTrackerRequest(int tracker_idx)
{
    std::string ann_url = m_torrent->t_announce_vec[tracker_idx];
    TrackerRequest tr = generateTrackerRequest(CLIENT_PORT);

    cpr::Parameters params = cpr::Parameters{{"info_hash", tr.info_hash},
            {"peer_id", tr.peer_id}, {"port", tr.port},
            {"uploaded", std::to_string(tr.uploaded)},
            {"downloaded", std::to_string(tr.downloaded)},
            {"left", std::to_string(tr.left)}, {"compact", "0"}, {"no_peer_id", "0"},
            {"event", tr.event}};

    std::cout << "sending GET/ to " << ann_url << std::endl;
    cpr::Response resp = cpr::Get(cpr::Url(ann_url), params);

    if (resp.status_code == 0) {
        std::cerr << "network.cpp: err: " << resp.error.message << std::endl;
        return -1;
    }
    std::cout << "Status code: " << resp.status_code << std::endl;
    std::cout << "Content type: " << resp.header["content-type"] << std::endl;
    std::cout << "Resplen " << resp.text.size() << std::endl;
    std::cout << "Tracker resp (decoded): \n";

    //Why do I get strange bytes in the middle of the data??
    for (int i = 0; i < resp.text.size(); i++) {
        std::cout << resp.text.at(i) << std::endl;
    }

    BtParser::ParsedObject po = BtParser::parseDictionary(resp.text, 0);
    BtParser::iteratePrintDict(&po);


    return 1;
}

void TorrentNetworkHandler::networkMainLoop()
{
    initialTrackerRequest(0);
}



} //END_NAMESPACE_BTNETWORK
