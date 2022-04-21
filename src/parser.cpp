#include "parser.hpp"
#include <iostream>
#include <algorithm>

namespace BtParser
{

/**
 * @brief identifyToken
 * @param torr_data =>  buffer containing torrent data
 * @param index => offset to presumed start of element/token
 * @return
 */
TOKEN_TYPE identifyToken(std::string &torr_data, int index)
{
    char f = torr_data.at(index);

    if (f >= '0' && f <= '9') {
        return T_BYTESTRING;
    } else if (f == 'i') {
        return T_INTEGER;
    } else if (f == 'd') {
        return T_DICTIONARY;
    } else if (f == 'l') {
        return T_LIST;
    }

    return T_INVALID;
}

int crunchByteString(std::string &torr_data, int index)
{
    int col_index = torr_data.find(":", index);
    int string_length = std::stoi(torr_data.substr(index, col_index - index));

    return col_index + string_length + 1;
}

int crunchInteger(std::string &torr_data, int index)
{
    int end_index = torr_data.find("e", index);
    return end_index + 1;
}

// recursive structures
int crunchList(std::string &torr_data, int index)
{
    int final_index = index + 1;        // index of 'l' + 1
    char check_c = torr_data.at(final_index);

    while (check_c != 'e') {
        TOKEN_TYPE typ = identifyToken(torr_data, final_index);

        switch (typ) {

            case T_BYTESTRING:
                final_index = crunchByteString(torr_data, final_index);
                break;
            case T_INTEGER:
                final_index = crunchInteger(torr_data, final_index);
                break;
            case T_LIST:
                final_index = crunchList(torr_data, final_index);
                break;
            case T_DICTIONARY:
                final_index = crunchDictionary(torr_data, final_index);
                break;
            default:
                fprintf(stderr, "crunchList: invalid value in list\n");
                break;
        }

        check_c = torr_data.at(final_index);
    }

    return final_index + 1;
}


int crunchDictionary(std::string &torr_data, int index)
{
    assert ("crunchDict: wrong starting index" && torr_data.at(index) == 'd');

    int final_index = index + 1;
    char check_c = torr_data.at(final_index);

    while (check_c != 'e') {

        assert("Error: key is not string" &&
               identifyToken(torr_data, final_index) == T_BYTESTRING);

        final_index = crunchByteString(torr_data, final_index);
        TOKEN_TYPE typ = identifyToken(torr_data, final_index);

        switch (typ) {
            case T_BYTESTRING:
                final_index = crunchByteString(torr_data, final_index);
                break;
            case T_INTEGER:
                final_index = crunchInteger(torr_data, final_index);
                break;
            case T_LIST:
                final_index = crunchList(torr_data, final_index);
                break;
            case T_DICTIONARY:
                final_index = crunchDictionary(torr_data, final_index);
                break;
            default:
                fprintf(stderr, "crunchDictionary: invalid value in dictionary\n");
                break;
        }
        check_c = torr_data.at(final_index);
    }

    return final_index + 1;
}

/**
 * @brief parseByteString
 * @param torr_data =>  buffer containing torrent data
 * @param i => offset; position in buffer where ByteString begins
 */
ParsedObject parseByteString(std::string &torr_data, int i)
{
    ParsedObject ret;
    // len : string
    // len is encoded in base ten ASCII; string is not specified.
    ret.po_type = T_BYTESTRING;
    ret.po_index = i;
    int index = torr_data.find(":", i);
    int bstring_len = std::stoi(torr_data.substr(i, index - i));


    ret.po_stringval = torr_data.substr(index+1, bstring_len);
    return ret;
}

ParsedObject parseInteger(std::string &torr_data, int i)
{
    ParsedObject ret;
    // 'i' is at index i
    ret.po_type = T_INTEGER;
    ret.po_index = i;

    int f_index = torr_data.find("e", i);
    ret.po_intval = std::stoi(torr_data.substr(i+1, f_index-i));
    return ret;
}

ParsedObject parseList(std::string &torr_data, int i)
{
    // d<string><elem>....<string><elem>e
    ParsedObject list_obj;
    list_obj.po_type = T_LIST;
    list_obj.po_index = i;

    int r_index = i + 1;                // running index
    char check_c = torr_data.at(r_index);

    while (check_c != 'e') {

        ParsedObject p_item;
        p_item.po_type = T_INVALID;

        TOKEN_TYPE typ = identifyToken(torr_data, r_index);

        switch (typ) {
            case T_BYTESTRING:
                p_item = parseByteString(torr_data, r_index);
                r_index = crunchByteString(torr_data, r_index);
                break;
            case T_INTEGER:
                p_item = parseInteger(torr_data, r_index);
                r_index = crunchInteger(torr_data, r_index);
                break;
            case T_LIST:
                p_item = parseList(torr_data, r_index);
                r_index = crunchList(torr_data, r_index);
                break;
            case T_DICTIONARY:
                p_item = parseDictionary(torr_data, r_index);
                r_index = crunchDictionary(torr_data, r_index);
                break;
            default:
                fprintf(stderr, "parseList: invalid value in list\n");
                break;
        }
        list_obj.po_listval.push_back(p_item);
        check_c = torr_data.at(r_index);
    }

    return list_obj;
}

/**
 * @brief parseDictionay
 * @param torr_data =>  buffer containing torrent data
 * @param i => offset; position of 'd'
 * @return
 */
ParsedObject parseDictionary(std::string &torr_data, int i)
{
    // d<string><elem>....<string><elem>e
    assert ("parseDict: wrong starting index" && torr_data.at(i) == 'd');

    ParsedObject dict_obj;
    dict_obj.po_type = T_DICTIONARY;
    dict_obj.po_index = i;

    int r_index = i + 1;                // running index
    char check_c = torr_data.at(r_index);

    while (check_c != 'e') {
        std::cout << "r_index = " << r_index << std::endl;
        assert("Error: key not string" && identifyToken(torr_data, r_index) == T_BYTESTRING);

        std::string dict_key = parseByteString(torr_data, r_index).po_stringval;

        r_index = crunchByteString(torr_data, r_index);

        ParsedObject dict_val;
        dict_val.po_type = T_INVALID;

        TOKEN_TYPE typ = identifyToken(torr_data, r_index);

        switch (typ) {
            case T_BYTESTRING:
                dict_val = parseByteString(torr_data, r_index);
                r_index = crunchByteString(torr_data, r_index);
                break;
            case T_INTEGER:
                dict_val = parseInteger(torr_data, r_index);
                r_index = crunchInteger(torr_data, r_index);
                break;
            case T_LIST:
                dict_val = parseList(torr_data, r_index);
                r_index = crunchList(torr_data, r_index);
                break;
            case T_DICTIONARY:
                dict_val = parseDictionary(torr_data, r_index);
                r_index = crunchDictionary(torr_data, r_index);
                break;
            default:
                fprintf(stderr, "parseDictionary: invalid value in dict \n");
                break;
        }
        dict_obj.po_dictval.insert(std::pair<std::string, ParsedObject>(dict_key, dict_val));
        check_c = torr_data.at(r_index);

    }

    return dict_obj;
}


std::string getInfoSubString(std::string &torr_data, ParsedObject *dict_obj)
{
    int start_ind = dict_obj->po_index;
    int end_ind = crunchDictionary(torr_data, start_ind);

    return torr_data.substr(start_ind, end_ind - start_ind);
}

void iteratePrintList(ParsedObject *p_list, bool replace_unprintable, int indent_level)
{
    assert("iteratePrintList: not a list" && p_list->po_type == T_LIST);

    for (int i = 0; i < p_list->po_listval.size(); i++ ) {
        ParsedObject obj = p_list->po_listval.at(i);

        int indent = indent_level * 2;

        // using "%*s" to apply indents
        if (obj.po_type == T_BYTESTRING) {
            std::string text = obj.po_stringval;
            if (replace_unprintable) {
                std::replace_if(text.begin(), text.end(),
                                std::not_fn([](char c){ return isalnum(c) || c == ':'; }), REPL_CHAR);
            }
            printf("%*sV_STR=> %s\n", indent, "", text.c_str());
        } else if (obj.po_type == T_INTEGER) {
            printf("%*s%d\n", indent, "", obj.po_intval);
        } else if (obj.po_type == T_LIST) {
            printf("%*s<LIST>\n", indent, "");
            iteratePrintList(&obj, indent_level + 1);
            printf("%*s</LIST>\n", indent, "");
        } else if (obj.po_type == T_DICTIONARY) {
            printf("%*s<DICT>\n", indent, "");
            iteratePrintDict(&obj, indent_level + 1);
            printf("%*s</DICT>\n", indent, "");

        }

    }

}

void iteratePrintDict(ParsedObject *p_dict, bool replace_unprintable, int indent_level)
{
    assert("iteratePrintdict: not a dict" && p_dict->po_type == T_DICTIONARY);

    std::map<std::string, ParsedObject>::iterator it;

    for (it = p_dict->po_dictval.begin(); it != p_dict->po_dictval.end(); it++ ) {

        ParsedObject obj = it->second;
        std::string key = it->first;

        int indent = indent_level * 2;

        printf("%*sK:(%s) -> ", indent, "", key.c_str());
        if (obj.po_type != T_BYTESTRING && obj.po_type != T_INTEGER) {
            printf("\n");
        }
        if (obj.po_type == T_BYTESTRING) {
            std::string text = obj.po_stringval;
            if (replace_unprintable) {
                std::replace_if(text.begin(), text.end(),
                                std::not_fn([](char c){ return isalnum(c) || c == ':'; }), REPL_CHAR);
            }
            printf("%*sV_STR=> %s\n", indent, "", text.c_str());
        } else if (obj.po_type == T_INTEGER) {
            printf("%*sV_INT=> %d\n", indent, "", obj.po_intval);
        } else if (obj.po_type == T_LIST) {
            printf("%*s<LIST>\n", indent, "");
            iteratePrintList(&obj, indent_level + 1);
            printf("%*s</LIST>\n", indent, "");
        } else if (obj.po_type == T_DICTIONARY) {
            printf("%*s<DICT>\n", indent, "");
            iteratePrintDict(&obj, indent_level + 1);
            printf("%*s</DICT>\n", indent, "");

        }

    }

}


} //END_NAMESPACE_BTPARSER
