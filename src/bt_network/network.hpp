#ifndef NETWORK_HPP
#define NETWORK_HPP

#include "../torrent.hpp"
#include "../bt_utils/utils.hpp"
#include "../bt_utils/general_utils.hpp"
#include "../bt_utils/urlencode.hpp"
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cerrno>
#include <map>


#define CLIENT_PORT "6881"
#define BACKLOG_SIZE 5

#define BT_NET_EVENT_STARTED "started"
#define BT_NET_EVENT_STOPPED "stopped"
#define BT_NET_EVENT_COMPLETED "completed"


namespace BtNetwork {

enum TrackerProtocol {
    P_HTTP,     // N.B.
    P_UDP
};

enum bt_NetStatus {
    NS_WAIT_TRACKER_CONNECT,
    NS_TRACKER_CONNECTED,
    NS_SENT_TRACKER_REQUEST,
    NS_WAIT_TRACKER_RESPONSE,
    NS_GOT_TRACKER_RESPONSE,
    NS_WAIT_PEER_CONNECT
};

/**
 * @brief The TrackerRequest struct
 * Fill this structure with the data needed to send a GET request to
 * the tracker.
 *
 * You can use <
 */
typedef struct {
    std::string info_hash;
    std::string peer_id;

    std::string port;
    uint64_t uploaded, downloaded, left;

    int compact;
    bool no_peer_id;

    std::string event;
} TrackerRequest;

class TorrentNetworkHandler
{
public:

    TorrentNetworkHandler(BtTorrent::Torrent *torr_obj);

    TrackerRequest generateTrackerRequest(std::string port_listen);
    int initialTrackerRequest(int tracker_idx=0);
    void networkMainLoop();

    BtTorrent::Torrent *m_torrent;
    bt_NetStatus m_status;
    int m_current_tracker;


    std::map<int, int> m_tracker_socket_map;
    std::map<int, std::string> m_tracker_addr_map;


};

} //END_NAMESPACE_BTNETWORK

#endif // NETWORK_HPP
