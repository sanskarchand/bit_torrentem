#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>
#include <string>

namespace BtUtils {
    /**
     * @brief The BtUint160 struct
     * caveat emptor: Hastily thrown together
     * smallest 32-bit int should be in uint_parts[4],
     * and the largest should be in uint_parts[0]
     */
    struct BtUint160 {
        BtUint160(): uint_parts{0, 0, 0, 0, 0} {};
        uint32_t uint_parts[5];
    };

    std::string calculateSHA1(std::string input);
    uint32_t leftRotate32(uint32_t num, int n);

    struct BtUint160 leftShiftUint160(struct BtUint160 num, int num_bits);
    struct BtUint160 bitwiseOrUint160(struct BtUint160 num1, struct BtUint160 num2);
    std::string getStringFromBtUint160(struct BtUint160 num);
} //END_NAMESPACE_BTUTILS

#endif // UTILS_HPP
