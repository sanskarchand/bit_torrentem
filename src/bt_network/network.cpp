#include "bt_network/network.hpp"

#include <unistd.h>
#include <iostream> //remove this

namespace BtNetwork {

TorrentNetworkHandler::TorrentNetworkHandler(BtTorrent::Torrent *torr_obj)
    : m_torrent(torr_obj), m_status(NS_WAIT_TRACKER_CONNECT), m_current_tracker(0)
{

}

/**
 * @brief TorrentNetworkHandler::connectToTracker
 * @return Socket file descriptor, or -1 on error
 */
int TorrentNetworkHandler::connectToTracker(int tracker_index)
{
    // step 1: get url
    //std::string url = m_torrent->t_announce;
    //NOTABENE: currently works only for tracker_index=0
    std::string url = m_torrent->t_announce_vec[tracker_index];
    std::string port_no = BtUtils::getPortFromAnnounce(url);
    std::string schema = BtUtils::getSchemaFromAnnounce(url);
    url = BtUtils::getURLFromAnnounce(url);

    printf("URL %s and schema %s\n", url.c_str(), schema.c_str());
    int sock_fd;
    int status;
    struct addrinfo hints, *results;
    struct addrinfo *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;           //auto-fill IP address

    if ((status = getaddrinfo(url.c_str(), port_no.c_str(), &hints, &results)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    }

    for (p = results; p != NULL; p = p->ai_next) {


        void *addr;
        char address_dat[24];
        if (p->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in*)p->ai_addr;
            addr = &(ipv4->sin_addr);
        } else {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6*)p->ai_addr;
            addr = &(ipv6->sin6_addr);
        }
        inet_ntop(p->ai_family, addr, address_dat, sizeof address_dat);
        //NOTABENE last address
        m_tracker_addr_map[tracker_index] = std::string(address_dat);
        printf("Address: %s\n", address_dat);
    }
    // first res; will be IPv6 for localhost testing, but definitely IPv4 for actual trackers
    sock_fd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);

    int bind(int sockfd, struct sockaddr *my_addr, int addrlen);

    connect(sock_fd, results->ai_addr, results->ai_addrlen);
    m_status = NS_TRACKER_CONNECTED;

    // hang around for a while
    //unsigned int microsecs = 20 * 1000 * 1000;
    //usleep(microsecs);
    //N.B.: C++11, map insertion with list initialization syntax
    //m_tracker_socket_map.insert({tracker_index, sock_fd});


    // prefer over-write for sockets; makes sense
    m_tracker_socket_map[tracker_index] = sock_fd;

    freeaddrinfo(results);
    return sock_fd;
}




/*
 * struct TrackerRequest {
    std::string info_hash;
    std::string peer_id;

    int port;
    int uploaded, downloaded, left;

    int compact;
    bool no_peer_id;

    std::string event;
};
*/

struct TrackerRequest TorrentNetworkHandler::generateTrackerRequest
(std::string port_listen)
{
    TrackerRequest req;

    std::string p_info_string = BtParser::getInfoSubString(
                m_torrent->t_torrent_file,
                &m_torrent->t_torrent_dict->po_dictval["info"]);

    p_info_string = BtUtils::calculateSHA1(p_info_string);
    std::cout << "Hashed: " << std::endl;
    for (int i = 0; i < 20; i++) {
        printf("%0x ", (unsigned char)p_info_string.at(i));
    }
    printf("\n");
    std::cout << p_info_string << std::endl;
    p_info_string = BtUtils::urlEncode(p_info_string);


    req.info_hash = p_info_string;
    req.peer_id = BtUtils::urlEncode(BtUtils::generatePeerID());
    req.port = port_listen;
    req.uploaded = m_torrent->t_uploaded_bytes;
    req.downloaded = m_torrent->t_downloaded_bytes;
    req.left = m_torrent->t_total_size - m_torrent->t_downloaded_bytes;
    req.compact = 0;
    req.no_peer_id = false;
    req.event = BT_NET_EVENT_STARTED;


    return req;
}

int TorrentNetworkHandler::sendTrackerRequest(std::string m_tracker_http_req)
{

    std::string request_string = "GET /announce?" + m_tracker_http_req
            + " HTTP/1.1\r\n";
    std::string port = BtUtils::getPortFromAnnounce(m_torrent->t_announce_vec[m_current_tracker]);

    request_string += "Host: " + m_tracker_addr_map[m_current_tracker]
                + ":" + port + "\r\n";
    request_string += "Accept: text/plain,*/*\r\n";
    request_string += "Connection: keep-alive\r\n";
    request_string += "\r\n\r\n";

    int sockfd = m_tracker_socket_map[m_current_tracker];
    int len = request_string.size();

    if (send(sockfd, static_cast<const void*>(request_string.c_str()), len, 0) != len) {
        printf("Not fully sent\n");
    } else {
        std::cout << "SENT: \n";
        std::cout << request_string << std::endl;
    }

    m_status = NS_SENT_TRACKER_REQUEST;

    char buf[512];
    int recvlen = recv(sockfd, (void *)buf, 512, 0);
    printf("rlen:%d RECV:%s\n", recvlen, buf);
    return 1;
}

std::string TorrentNetworkHandler::getTrackerResponse()
{
    struct addrinfo hints, *results;
    struct sockaddr_storage incoming_addr;
    socklen_t addr_size;
    int status, socket_fd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(NULL,  CLIENT_PORT, &hints, &results)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    }

    socket_fd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
    if (bind(socket_fd, results->ai_addr, results->ai_addrlen) == -1) {
        fprintf(stderr, "network.cpp: listenForTracker(): bind() failed\n");
    }

    if (listen(socket_fd, BACKLOG_SIZE) == -1) {
        fprintf(stderr, "network.cpp: listenForTracker(): listen(): %s\n", strerror(errno));
    }

    addr_size = sizeof incoming_addr;
    m_status = NS_WAIT_TRACKER_RESPONSE;
    printf("Started to wait for a response\n");

    int new_sockfd;
    while (1) {
        new_sockfd = accept(socket_fd, (struct sockaddr *)&incoming_addr, &addr_size);
        if (new_sockfd == -1) {
            continue;
        }
        printf("GOt a conn!\n");
        break;

    }
    char listen_buf[512];
    //NOTABENE: checking length
    int recvlen = recv(new_sockfd, static_cast<void*>(listen_buf), 512, 0);

    m_status = NS_GOT_TRACKER_RESPONSE;
    return std::string(listen_buf);
}


//NOTABENE: only using one tracker right now
void TorrentNetworkHandler::networkMainLoop()
{
    bool quit = false;

    printf("Size of sockaddr %u \n", sizeof(struct sockaddr));
    // initial tracker-req-resp
    connectToTracker(m_current_tracker);
    struct TrackerRequest tra_req = generateTrackerRequest(std::string(CLIENT_PORT));
    std::string param_string = buildParamString(tra_req);
    sendTrackerRequest(param_string);

    //NOTABENE: blocking call
    //std::string resp = getTrackerResponse();

    // peer-related stuff

}


std::string buildParamString(struct TrackerRequest track_req)
{
    std::string param_string = "";

    param_string += "info_hash=" + track_req.info_hash;
    param_string += "&peer_id=" + track_req.peer_id;
    param_string += "&port=" + track_req.port;
    param_string += "&uploaded=" + std::to_string(track_req.uploaded);
    param_string += "&downloaded=" + std::to_string(track_req.downloaded);
    param_string += "&left=" + std::to_string(track_req.left);
    param_string += "&compact=0";
    param_string += "&no_peer_id=0";
    param_string += "&event=" + track_req.event;
    // NOTABENE: ignored optional fields

    return param_string;
}


} //END_NAMESPACE_BTNETWORK
