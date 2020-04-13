#include "include/parser.hpp"

Parser::Parser()
{

}

Parser::~Parser()
{

}
/**
 * @brief Parser::parseByteString
 * @param torr_string =>  buffer containing torrent data
 * @param i => position in buffer where ByteString begins/offset
 */
std::string Parser::parseByteString(std::string &torr_data, int i )
{
    // len : string
    // len is encoded in base ten ASCII; string is not specified.
    int index = torr_data.find(":", i);
    int bstring_len = std::stoi(torr_data.substr(i, index - i));

    return torr_data.substr(index+1, bstring_len);

}

// parse dict
