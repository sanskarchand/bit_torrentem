#include "include/torrentinfo.h"

Parser::Parser()
{

    std::cout << "Token check\n";
    std::vector<std::string> tokens = getProductionBodyTokens(productions.at(0));

    for (std::string str : tokens) {
        std::cout << "got " << str << " as a token\n";
    }
    /*
    gram_map.push(std::pair<std::string, FuncPtr>("BDict", &Parser::BDict));
    gram_map.push(std::pair<std::string, FuncPtr>("BList", &Parser::BList));
    gram_map.push(std::pair<std::string, FuncPtr>("BX", &Parser::BX));
    */
    gram_map.emplace(std::make_pair("BDict", &Parser::BDict));
}

std::vector<std::string> Parser::getProductions(std::string token)
{
    std::vector<std::string> prods;
    for (std::string prod : productions) {
        if ( prod == token ) {
            prods.push_back(prod);
        }
    }
    return prods;
}

std::vector<std::string> Parser::getProductionBodyTokens(std::string production)
{
    std::vector<std::string> tokens;

    int index = production.find(" -> ");
    std::string rest_string = production.substr(index + 5);

    while ( rest_string.size() != 0 ) {

        int index2 = rest_string.find(" ");
        std::string token = rest_string.substr(0, index2);
        tokens.push_back(token);
        rest_string = rest_string.substr(index2+1);
        std::cout << "New string: " << rest_string << std::endl;
    }

    return tokens;

}

void  Parser::BDict()
{

}

Torrent::Torrent(std::string torrent_data)
{

}
Torrent::~Torrent() {}
