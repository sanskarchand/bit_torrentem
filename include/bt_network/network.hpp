#ifndef NETWORK_HPP
#define NETWORK_HPP

#include "../torrent.hpp"
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cerrno>
#include <map>


#define CLIENT_PORT "6881"

enum TrackerProtocol {
    P_HTTP,     // N.B.
    P_UDP
};

/**
 * @brief The TrackerRequest struct
 * Fill this structure with the data needed to send a GET request to
 * the tracker.
 *
 * You can use <
 */
struct TrackerRequest {

};

struct TrackerResponse {

};

// One of these for a single torrent
// NOTABENE: assuming we use only a single tracker for now
// JANNYNOTES: must rewrite later to utilize multiple trackers
class TorrentNetworkHandler
{
public:
    TorrentNetworkHandler(Torrent *torr_obj);

    int connectToTracker(int tracker_index=0);
    void sendRequest();


    Torrent *m_torrent;

    int m_current_tracker;
    std::map<int, int> m_tracker_socket_map;


};


 //Utilities: urlencode defined here

#endif // NETWORK_HPP
