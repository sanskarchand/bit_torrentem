#include "include/bt_utils/urlencode.hpp"

namespace  BtUtils
{


/**
 * @brief getByteSequence:
 * @param inp
 *
 * @return
 */
std::string urlEncode(std::string inp)
{
    //src: https://tools.ietf.org/html/rfc3986#section-2.1

    //std::string reserved_chars =  "!*'();:@&=+$,/?#[]";
    std::string byte_seq;   // return value
    char temp_buf[13] = { 0 }; // max 4 bytes

    for (int i = 0; i < inp.size();  ) {

        uint8_t  c = static_cast<uint8_t>(inp.at(i));

        // unreserved characters: alphanumeric, hyphen, underscore, period, and tilde
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            byte_seq += c;
            i += 1;
            continue;
        }

        // all others must be percent-encoded
        // ASCII and UTF-8 first byte are equivalent
        int num_bytes = 0;

        if ((c & 0x80) ==  0x00) {  // first byte 0xxxxxxxx
            num_bytes = 1;
        } else if ((c & 0xE0) == 0XC0) {    // first byte 110xxxxx
            num_bytes = 2;
        } else if ((c & 0xF0) == 0xE0) {
            num_bytes = 3;
        } else if ((c & 0xF8) == 0xF0) {
            num_bytes = 4;
        } else {
            //printf("DIOS MIO, what an atrocity c=%0x i = %d\n",c,  i);
            //NOTABENE arbitrary byte case?
            num_bytes = 1;
        }

        for (int j = 0; j < num_bytes; j++) {
            char c_byte = inp.at(i + j);
            //NOTABENE: if not cast into unsigned int, gives ffffff
            sprintf(temp_buf + 3*j, "%%%X", static_cast<uint8_t>(c_byte));
        }

        //_CRUCIAL_
        i += num_bytes;

        byte_seq += std::string(temp_buf);

        //clear the buffer
        for (int k = 0; k < 13; k++) {
            *(temp_buf + k) = 0;
        }

    }


    return byte_seq;
}


} //END_NAMESPACE_BTUTILS
