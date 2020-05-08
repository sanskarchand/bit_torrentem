#ifndef TORRENT_HPP
#define TORRENT_HPP

#include <string>
#include <vector>
#include <map>

#include "parser.hpp"

#define S_ANNOUNCE      std::string("announce")
#define S_ANN_LIST      std::string("announce-list")
#define S_CREATION      std::string("creation date")
#define S_COMMENT       std::string("comment")
#define S_INFO          std::string("info")
#define S_FILES         std::string("files")
#define S_LENGTH        std::string("length")
#define S_PATH          std::string("path")
#define S_NAME          std::string("name")
#define S_PIECE_LEN     std::string("piece length")
#define S_PIECES        std::string("pieces")

//typedef std::map<std::string, std::vector<std::string>> FilesDict;

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
    struct TorrInfo *t_info;

    // info related to downloading
    std::string t_root_dir;
    int t_downlaoded_bytes = 0;


    //REFAC_NOTE: Maybe keeping both is redundant
    //should I make an unparser?
    std::string t_torrent_file; // the data in the .torrent file
    BtParser::ParsedObject *t_torrent_dict; // original dict

};

#endif // TORRENT_HPP
