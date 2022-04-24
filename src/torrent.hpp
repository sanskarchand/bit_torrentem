#ifndef TORRENT_HPP
#define TORRENT_HPP

#include <string>
#include <vector>
#include <map>

#include "parser.hpp"

//typedef std::map<std::string, std::vector<std::string>> FilesDict;

namespace BtTorrent {


struct TorrInfo {

    TorrInfo(): ti_single_file(false),ti_creation_date(-1),
        ti_comment(""), ti_encoding("") { }

    bool ti_single_file;
    int ti_length;              // if only a single file
    std::vector<int> ti_file_lengths;
    std::vector<std::vector<std::string>> ti_file_paths_list;
    std::string ti_name;
    int ti_piece_length;
    std::vector<char> ti_pieces;

    // other info
    int ti_creation_date;       // maybe use std::time_t? same thing?
    std::string ti_comment;
    std::string ti_encoding;

};

class Torrent {
public:
    Torrent(BtParser::ParsedObject *parsed_dict, std::string torrent_file_path);  // adding initially
    Torrent(std::string filename); // de-serialize from file
   ~Torrent();

    // torrent and tracker-related info
    std::string t_announce;
    std::vector<std::string> t_announce_vec;
    struct TorrInfo *t_info;

    // info related to downloading
    std::string t_root_dir;
    uint64_t t_total_size = 0;
    uint64_t t_downloaded_bytes = 0;
    uint64_t t_uploaded_bytes = 0;
    //order identical with that of the metainfo file
    std::vector<int> t_per_file_downloaded_bytes;

    bool t_error;
    std::string t_err_string;


    //REFAC_NOTE: Maybe keeping both is redundant
    std::string t_torrent_file; // the data in the .torrent file\
    //NOTE: serialize BtParser ParsedObject
    BtParser::ParsedObject *t_torrent_dict; // original dict

};

} //END_NAMESPACE_BTTORRENT
#endif // TORRENT_HPP
