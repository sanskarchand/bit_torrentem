#ifndef GENERAL_UTILS_HPP
#define GENERAL_UTILS_HPP

#include <string>
#include <cstdlib>
#include <ctime>

namespace BtUtils {

// conversion functions
std::string getPortFromAnnounce(std::string announce_url);
std::string getURLFromAnnounce(std::string announce_url);
std::string getSchemaFromAnnounce(std::string announce_url);
std::string generatePeerID();

} //END_NAMESPACE_BTUTILS


#endif // GENERAL_UTILS_HPP
