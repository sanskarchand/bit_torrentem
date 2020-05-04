#include "include/parser.hpp"

Parser::Parser()
{

}

Parser::~Parser()
{

}
/**
 * @brief Parser::identifyToken
 * @param torr_data =>  buffer containing torrent data
 * @param index => offset to presumed start of element/token
 * @return
 */
TOKEN_TYPE Parser::identifyToken(std::string &torr_data, int index)
{
    char f = torr_data.at(index);

    if (f >= '1' && f <= '9') {
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

int Parser::crunchByteString(std::string &torr_data, int index)
{
    int col_index = torr_data.find(":", index);
    int string_length = std::stoi(torr_data.substr(index, col_index - index));

    return col_index + string_length + 1;
}

int Parser::crunchInteger(std::string &torr_data, int index)
{
    int end_index = torr_data.find("e", index);
    return end_index + 1;
}

// recursive structures
int Parser::crunchList(std::string &torr_data, int index)
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
                printf("ERROR ERROR ERROR crunchList error error ABORT ABORT\n");
                break;
        }

        check_c = torr_data.at(final_index);
    }

    return final_index + 1;
}


int Parser::crunchDictionary(std::string &torr_data, int index)
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
                printf("ERROR ERROR ERROR crunchList error error ABORT ABORT\n");
                break;
        }
        check_c = torr_data.at(final_index);
    }

    return final_index + 1;
}

/**
 * @brief Parser::parseByteString
 * @param torr_data =>  buffer containing torrent data
 * @param i => offset; position in buffer where ByteString begins
 */
ParsedObject Parser::parseByteString(std::string &torr_data, int i)
{
    ParsedObject ret;
    // len : string
    // len is encoded in base ten ASCII; string is not specified.
    int index = torr_data.find(":", i);
    int bstring_len = std::stoi(torr_data.substr(i, index - i));


    ret.po_type = T_BYTESTRING;
    ret.po_stringval = torr_data.substr(index+1, bstring_len);
    return ret;
}

ParsedObject Parser::parseInteger(std::string &torr_data, int i)
{
    ParsedObject ret;
    // 'i' is at index i
    int f_index = torr_data.find("e", i);

    ret.po_type = T_INTEGER;
    ret.po_intval = std::stoi(torr_data.substr(i+1, f_index-i));
    return ret;
}

ParsedObject Parser::parseList(std::string &torr_data, int i)
{
    // d<string><elem>....<string><elem>e
    ParsedObject list_obj;
    list_obj.po_type = T_LIST;

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
                printf("ERROR ERROR ERROR crunchList error error ABORT ABORT\n");
                break;
        }
        list_obj.po_listval.push_back(p_item);
        check_c = torr_data.at(r_index);
    }

    return list_obj;
}

/**
 * @brief Parser::parseDictionay
 * @param torr_data =>  buffer containing torrent data
 * @param i => offset; position of 'd'
 * @return
 */
ParsedObject Parser::parseDictionary(std::string &torr_data, int i)
{
    // d<string><elem>....<string><elem>e
     assert ("parseDict: wrong starting index" && torr_data.at(i) == 'd');

    ParsedObject dict_obj;
    dict_obj.po_type = T_DICTIONARY;

    int r_index = i + 1;                // running index
    char check_c = torr_data.at(r_index);

    while (check_c != 'e') {
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
                printf("ERROR ERROR ERROR crunchList error error ABORT ABORT\n");
                break;
        }
        dict_obj.po_dictval.insert(std::pair<std::string, ParsedObject>(dict_key, dict_val));
        check_c = torr_data.at(r_index);

    }

    return dict_obj;
}


void iteratePrintList(ParsedObject *p_list, int indent_level)
{
    assert("iteratePrintList: not a list" && p_list->po_type == T_LIST);

    for (int i = 0; i < p_list->po_listval.size(); i++ ) {
        ParsedObject obj = p_list->po_listval.at(i);

        for (int i = 0; i < indent_level; i++) {
            printf("  ");
        }
        printf("(%d)", indent_level);

        if (obj.po_type == T_BYTESTRING) {
            printf("%s\n", obj.po_stringval.c_str());
        } else if (obj.po_type == T_INTEGER) {
            printf("%d\n", obj.po_intval);
        } else if (obj.po_type == T_LIST) {
            printf("<<LIST BEGIN>>\n");
            iteratePrintList(&obj, indent_level + 1);
            printf("<<LIST END>>\n");
        } else if (obj.po_type == T_DICTIONARY) {
            printf("<<DICT BEGIN>>\n");
            iteratePrintDict(&obj, indent_level + 1);
            printf("<<DICT END>>\n");

        }

    }

}

void iteratePrintDict(ParsedObject *p_dict, int indent_level)
{
    assert("iteratePrintdict: not a dict" && p_dict->po_type == T_DICTIONARY);

    std::map<std::string, ParsedObject>::iterator it;

    for (it = p_dict->po_dictval.begin(); it != p_dict->po_dictval.end(); it++ ) {

        ParsedObject obj = it->second;
        std::string key = it->first;

        for (int i = 0; i < indent_level; i++) {
            printf("  ");
        }
        printf("(%d)", indent_level);

        printf("K:(%s) -> ", key.c_str());
        if (obj.po_type != T_BYTESTRING && obj.po_type != T_INTEGER) {
            printf("\n");
        }

        if (obj.po_type == T_BYTESTRING) {
            printf("VAL BYTESTRING: %s\n", obj.po_stringval.c_str());
        } else if (obj.po_type == T_INTEGER) {
            printf("VAL INT: %d\n", obj.po_intval);
        } else if (obj.po_type == T_LIST) {
            printf("<<LIST BEGIN>>\n");
            iteratePrintList(&obj, indent_level + 1);
            printf("<<LIST END>>\n");
        } else if (obj.po_type == T_DICTIONARY) {
            printf("<<DICT BEGIN>>\n");
            iteratePrintDict(&obj, indent_level + 1);
            printf("<<DICT END>>\n");

        }

    }

}
