#ifndef PARSER_HPP
#define PARSER_HPP

#include <stdio.h>
#include <assert.h>
#include <string>
#include <vector>
#include <map>

enum TOKEN_TYPE {
    T_BYTESTRING,
    T_INTEGER,
    T_LIST,
    T_DICTIONARY,
    T_INVALID,
    NUM_TYPES
};


/**
 * @brief The ParsedObject struct: A 'middle man' structure
 * Its purpose is to store the results of parsing in a comprehensible manner
 * po_objarray is to be used for both lists and
 * dicts. Obvious usage in lists.
 * In dictions, index 0 is the key, index 1 is the value
 *
 * The key idea is the Bytestrings and Integers are simple objects
 * and Lists and Dictionaries are complex/recursive objects.
 */
struct ParsedObject {
    //ParsedObject(): po_type(T_INVALID), po_objarray(nullptr) {}

    TOKEN_TYPE po_type;
    std::string po_stringval;
    std::string po_intval;

    std::vector<ParsedObject> po_listval;
    std::map<std::string, ParsedObject> po_dictval;
};


class Parser
{
public:
    Parser();
    ~Parser();
    TOKEN_TYPE identifyToken(std::string &torr_data, int index);

    int crunchByteString(std::string &torr_data, int index);
    int crunchInteger(std::string &torr_data, int index);
    int crunchList(std::string &torr_data, int index);
    int crunchDictionary(std::string &torr_data, int index);

    ParsedObject parseByteString (std::string &torr_data, int i );
    ParsedObject parseInteger(std::string &torr_data, int i);
    ParsedObject parseList(std::string &torr_data, int i);
    ParsedObject parseDictionary(std::string &torr_data, int i);

};

#endif // PARSER_HPP
