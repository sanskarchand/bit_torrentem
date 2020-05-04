#include "../include/bt_network/network.hpp"

TorrentNetworkHandler::TorrentNetworkHandler(Torrent *torr_obj)
{
    m_torrent = torr_obj;

}

/**
 * @brief TorrentNetworkHandler::connectToTracker
 * @return Socket file descriptor, or -1 on error
 */
int TorrentNetworkHandler::connectToTracker(int tracker_index)
{
    // step 1: get url
    //std::string url = m_torrent->t_announce;
    std::string url = "127.0.0.1";

    int sock_fd;
    int status;
    struct addrinfo hints, *results;
    struct addrinfo *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;           //auto-fill IP address

    if ((status = getaddrinfo(url.c_str(), "8000", &hints, &results)) != 0) {
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
        printf("Address: %s\n", address_dat);
    }
    // first res; will be IPv6 for localhost testing, but definitely IPv4 for actual trackers
    sock_fd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
    connect(sock_fd, results->ai_addr, results->ai_addrlen);

    //N.B.: C++11, map insertion with list initialization syntax
    //m_tracker_socket_map.insert({tracker_index, sock_fd});

    // prefer over-write for sockets; makes sense
    m_tracker_socket_map[tracker_index] = sock_fd;

    return sock_fd;

}
