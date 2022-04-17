#include "bt_utils/utils.hpp"

namespace BtUtils {

/**
 * @brief calculateSHA1
 * @param input_message: patent
 * @return patent
 */
std::string calculateSHA1(std::string input_message)
{
    static uint32_t h0 = 0x67452301;
    static uint32_t h1 = 0xEFCDAB89;
    static uint32_t h2 = 0x98BADCFE;
    static uint32_t h3 = 0x10325476;
    static uint32_t h4 = 0xC3D2E1F0;

    uint32_t a, b, c, d, e;

    // in bits
    uint64_t m_len = input_message.size() * 8;

    /* BEGIN Preprocessing */
    //pad to obtain a multiple of 512 bits
    if (m_len % 8 == 0) {
        input_message += 0x80;
    }
    // have added the bit '1' and also seven '0' bits above;
    // anyway just have to make sure 64 bits = 8 bytes are left over

    for (int k = 0; k < 64; k++) {

        // if only 8 bytes are left over to get a multiple of 512, stop
        if (input_message.size() * 8 % 512 == 448) {
            break;
        }
        input_message += static_cast<char>(0x00);
    }

    // add the original message length as a 64-bit big-endian integer
    // start from the last (biggest)  byte, so that we can add it to the
    // message string in big-endian order

    for (int i = 7; i >= 0; i--) {
        char byte = static_cast<char>( ((m_len >> 8 * i) & static_cast<uint64_t>(0xFF)) );
        input_message += byte;
    }

    /* END Preprocessing */

    // process in terms of 512-bit chunks
    int num_chunks = (input_message.size() * 8)/512;

    for (int i = 0; i < num_chunks; i++){
        // 512 bits = 64 bytes
        std::string curr_chunk = input_message.substr(i * 64, 64);

        // break chunk into 16 32-bit big-endian words
        uint32_t words_big_endian[80];
        for (int j = 0; j < 16; j++) {
            std::string byte_stream = curr_chunk.substr(j * 4, 4);

            // ensuring that the higher ordered bytes are zeroed-out, though
            //  this must be unnecessary. This precaution is not taken after this
            uint32_t b0 = byte_stream.at(0) & 0x000000FF;
            uint32_t b1 = byte_stream.at(1) & 0x000000FF;
            uint32_t b2 = byte_stream.at(2) & 0x000000FF;
            uint32_t b3 = byte_stream.at(3) & 0x000000FF;
            // just need to 'glue together' the four chars

            words_big_endian[j] = (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
        }


        for (int j = 16; j < 80; j++) {
                uint32_t temp_xor = (words_big_endian[j-3] ^ words_big_endian[j-8] ^
                    words_big_endian[j-14] ^ words_big_endian[j-16]);

                words_big_endian[j] = leftRotate32(temp_xor, 1);
        }


        // initialize has values for this chunk
        a = h0; b = h1; c = h2; d = h3; e = h4;
        uint32_t k, f;             // compilercheck; will it catch this case?

        for (int j = 0; j < 80; j++) {

            if (j < 20) {
                f = (b & c) | ((~b) & d);
                k = 0x5A827999;
            } else if (j < 40) {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1;
            } else if (j < 60) {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDC;

            } else if (j < 80) {
                f = b ^ c ^ d;
                k = 0xCA62C1D6;
                // AAAAAAAAAAAAAAAAAAAH I SPENT 5 HOURS DEBUGGING BECAUSE I PUT A 70 IN THE COND INSTEAD OF 80
                //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
            }

            uint32_t temp = leftRotate32(a, 5) + f + e + k + words_big_endian[j];
            e = d;
            d = c;
            c = leftRotate32(b, 30);
            b = a;
            a = temp;

        }
        // add this chunk's hash to the result so far
        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
        h4 += e;
        /*
        std::cout << "debug addvars: " << std::endl;
        printf("%x\n", h0);
        printf("%x\n", h1);
        printf("%x\n", h2);
        printf("%x\n", h3);
        printf("%x\n", h4);
        */
    }

    // produce the final hash value
    // 160-bit number; use custom structure
    struct BtUint160 h0_l, h1_l, h2_l, h3_l, h4_l, res_prelim;
    h0_l.uint_parts[4] = h0;
    h1_l.uint_parts[4] = h1;
    h2_l.uint_parts[4] = h2;
    h3_l.uint_parts[4] = h3;
    h4_l.uint_parts[4] = h4;

    h0_l = leftShiftUint160(h0_l, 128);
    h1_l = leftShiftUint160(h1_l, 96);
    h2_l = leftShiftUint160(h2_l, 64);
    h3_l = leftShiftUint160(h3_l, 32);

    res_prelim = bitwiseOrUint160(h0_l, h1_l);
    res_prelim = bitwiseOrUint160(res_prelim, h2_l);
    res_prelim = bitwiseOrUint160(res_prelim, h3_l);
    res_prelim = bitwiseOrUint160(res_prelim, h4_l);

    return getStringFromBtUint160(res_prelim);
}


uint32_t leftRotate32(uint32_t num, int n)
{
    // left-shift, and then push the 'cast-off' bits to the places at the right side
    // where they are needed
    return (num << n) | (num >> (32 - n));
}

struct BtUint160 leftShiftUint160(struct BtUint160 num, int num_bits)
{
    struct BtUint160 result;
    int glob_pos = 0;

    // new starting point of each 32-bit part after left shift
    int lost_splice_pos[5] = {0};       // position from index 0

    // first loop: decide what the final bits will be
    for (int j = 4; j >= 0; j--) {
        // save the bits that get 'cast off'
        //lshift_lost[j] = num.uint_parts[j] >> (32 - bits_to_shift);
        lost_splice_pos[j] = 128 - ( (4-j) * 32 + num_bits);
    }

    auto pos_actual_ind_lambda = [](int pos) { return pos/32; };
    auto pos_rel_ind_lambda = [](int pos) { return pos % 32; };
    auto right_pos_lambda = [](int pos) { return 31 - pos; };

    // second loop; write the bits;
    // start from the biggest bite, since the lower bytes must be able to
    // override their values
    for (int j = 0; j < 5; j++) {

        int start_ind = lost_splice_pos[j];

        // get ith bit
        for (int x = 0; x < 32; x++) {
            glob_pos = x + start_ind;
            if (x + start_ind < 0) continue;

            // get xth bit from the right for the current part
            // OP_READ
            int shift_n = right_pos_lambda(x);
            uint32_t bit_to_write  = (num.uint_parts[j] >> shift_n) & static_cast<uint32_t>(1);

            // now, we just need to write this bit to the appropriate
            // bit position in the appropriate part in the result 160-bit struct

            // find out which 'part' glob pos corressponds to, and the appropriate
            // relative index of that part
            int part_ind = pos_actual_ind_lambda(glob_pos);
            int rel_ind = pos_rel_ind_lambda(glob_pos);
            // reuse
            shift_n = right_pos_lambda(rel_ind);

            // OP_WRITE
            result.uint_parts[part_ind] |= bit_to_write << shift_n;
        }

    }

    return result;
}

struct BtUint160 bitwiseOrUint160(struct BtUint160 num1, struct BtUint160 num2)
{
    struct BtUint160 result;

    for (int i = 0; i < 5; i++) {
        uint32_t res_part = num1.uint_parts[i] | num2.uint_parts[i];
        result.uint_parts[i] = res_part;
    }

    return result;
}

std::string getStringFromBtUint160(struct BtUint160 num)
{
    std::string ret_string = "";
    for (int i = 0; i < 5; i++) {
        uint32_t part = num.uint_parts[i];

        // break it into four 8-bit chars
        int j = 3;
        while (j >= 0) {
            uint8_t byte = static_cast<uint8_t>( ((part >> 8 * j) & static_cast<uint32_t>(0xFF)) );
            ret_string += static_cast<char>(byte);
            j--;
        }
    }
    return ret_string;
}

}
