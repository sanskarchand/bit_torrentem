#include "bt_network/network.hpp"

#include <unistd.h>
#include <iostream> //remove this
#include <stdio.h>  //remove this
#include <errno.h>
#include <string.h>
#include <stdlib.h>


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

    if (connect(sock_fd, results->ai_addr, results->ai_addrlen) == -1) {
        close(sock_fd);
        fprintf(stderr, "Could not connect to tracker\n");
    }


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

    fprintf(stderr, "Sockfd is %d\n", sockfd);
    fflush(stderr);

    int len = request_string.size();

    if (send(sockfd, static_cast<const void*>(request_string.c_str()), len, 0) != len) {
        fprintf(stderr, "Not fully sent\n");
    } else {
        std::cout << "Trackert GET req. sent: \n";
        std::cout << request_string << std::endl;
    }

    m_status = NS_SENT_TRACKER_REQUEST;

    char buf[512], samebuf[512];

    int recvlen = recv(sockfd, (void *)buf, 512, 0);
    std::cout << "Recvlen = " << recvlen << std::endl;
    if (recvlen == -1) {
        std::cerr << "recv error" << std::endl;
        std::cerr << gai_strerror(errno) << std::endl;
        return -1;
    }

    memcpy(samebuf, buf, 512);
    strtok(samebuf, "\r\n");
    strtok(NULL, "\r\n");
    printf("Tracker resp: %s\n", strtok(NULL, "\r\n"));



    // obtain and parse the bencoded data from the tracker's HTTP response
    // first, check if we got all the necessary bytes using recv()
//    strtok(samebuf, "\r\n");
//    char *con_len = strtok(NULL, "\r\n");
//    con_len = strtok(con_len, ": ");
//    printf("Conlen so far %s\n", con_len);


//    while (*con_len++ != '\r') {
//        ;
//    }
//    *con_len = 0;
//    printf("Con len %s\n", con_len);


/*
    const char *actual_resp = strtok(NULL, "\r\n");
    std::string parse_data = std::string(actual_resp);
    std::cout << parse_data << std::endl;
    BtParser::ParsedObject po = BtParser::parseDictionary(parse_data, 0);
    BtParser::iteratePrintDict(&po);
*/
    return 1;
}

std::string TorrentNetworkHandler::getTrackerResponse()
{
}


//NOTABENE: only using one tracker right now
void TorrentNetworkHandler::networkMainLoop()
{
    bool quit = false;

    // initial tracker-req-resp
    int sock_fd = connectToTracker(m_current_tracker);
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
