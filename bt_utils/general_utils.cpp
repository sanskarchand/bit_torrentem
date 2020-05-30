#include "include/bt_utils/general_utils.hpp"
namespace BtUtils {

std::string getPortFromAnnounce(std::string announce_url)
{
    int ind_colon = announce_url.rfind(':');
    int ind_slash = announce_url.find("/announce");

    return announce_url.substr(ind_colon+1, ind_slash-ind_colon-1);
}

std::string getURLFromAnnounce(std::string announce_url)
{
    // strip the ":[port_no]/announce" part
    // and the schema
    int ind_colon = announce_url.rfind(':');
    int ind_first_colon = announce_url.find(':');
    int index = ind_first_colon + 3;

    std::string url = announce_url.substr(
                index, ind_colon - index);

    return url;
}

std::string getSchemaFromAnnounce(std::string announce_url)
{
    int ind_colon = announce_url.find(':');
    return announce_url.substr(0, ind_colon);
}

std::string generatePeerID()
{
    //Azureus-style
    std::string id = "-BM0001-";

    //12 random digits
    srand(time(NULL));
    for (int i = 0; i < 12; i++) {
        int rand_dig = rand() % 10;
        id += static_cast<char>(rand_dig + '0');
    }


    return id;
}

} //END_NAMESPACE_BTUTILS

