#ifndef TORRENT_HPP
#define TORRENT_HPP

#include <string>
#include <vector>
#include <map>

typedef std::map<std::string, std::vector<std::string>> FilesDict;

struct TorrInfo {

    int ti_length;
    std::vector<FilesDict> ti_files;
    std::string ti_name;
    int piece_length;
    std::vector<char> pieces;
    //ignore pieces for now
};

class Torrent {
public:
    Torrent();
    ~Torrent();


    std::string t_announce;
    struct TorrInfo *t_info;

};

#endif // TORRENT_HPP
