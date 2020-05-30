#include "include/torrent.hpp"

namespace BtTorrent {


Torrent::Torrent(BtParser::ParsedObject *parsed_dict, std::string torrent_raw_file)
{
    this->t_torrent_file = torrent_raw_file;
    this->t_torrent_dict = parsed_dict;

    BtParser::ParsedObject ann_string = parsed_dict->po_dictval[S_ANNOUNCE];

    //NOTABENE for announce lists: have not added this S_ANN_LIST
    //just add as a first element for now
    t_announce_vec.push_back(ann_string.po_stringval);
    t_announce = ann_string.po_stringval;
    t_info = new TorrInfo;

    BtParser::ParsedObject *info_dict = &parsed_dict->po_dictval[S_INFO];

    // handle single or multiple files
    if (info_dict->po_dictval.find(S_FILES) == info_dict->po_dictval.end()) {
        t_info->ti_single_file = true;
        printf("[ SINGLE FILE | PLACEHOLDER STRING]");
    } else {
        BtParser::ParsedObject *files_list = &info_dict->po_dictval[S_FILES];
        for (int i = 0; i < files_list->po_listval.size(); i++) {
            BtParser::ParsedObject *dict = &files_list->po_listval.at(i);

            // ti_file_lengths item GET
            int val_length = dict->po_dictval[S_LENGTH].po_intval;
            t_total_size += val_length;


            // ti_file_paths item GET
            std::vector<std::string> paths; // list of pahs
            BtParser::ParsedObject *list = &dict->po_dictval[S_PATH];
            for (int j = 0; j < list->po_listval.size(); j++) {
                paths.push_back(list->po_listval.at(j).po_stringval);
            }

           t_info->ti_file_lengths.push_back(val_length);
           t_info->ti_file_paths_list.push_back(paths);
        }
    }

    t_info->ti_name = info_dict->po_dictval[S_NAME].po_stringval;
    t_info->ti_piece_length = info_dict->po_dictval[S_PIECE_LEN].po_intval;

    //read the pieces in
    std::string *pieces = &info_dict->po_dictval[S_PIECES].po_stringval;

    t_info->ti_pieces = std::vector<char>(pieces->begin(), pieces->end());

    // extra stuff like comments and dates
}

Torrent::~Torrent()
{
    delete t_info;
}

} //END_NAMESPACE_BTTORRENT
