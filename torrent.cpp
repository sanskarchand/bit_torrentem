#include "include/torrentinfo.h"

Parser::Parser()
{

//    std::cout << "Token check\n";
//    std::vector<std::string> tokens = getProductionBodyTokens(productions.at(0));

//    for (std::string str : tokens) {
//        std::cout << "got " << str << " as a token\n";
//    }

    gram_map.emplace(std::make_pair("BDict", &Parser::BDict));
    getProductionHead(productions.at(0));


    FuncPtr fp = gram_map["BDict"];
    (this->*fp)();
}

std::vector<std::string> Parser::getProductions(std::string token)
{
    std::vector<std::string> prods;
    for (std::string prod : productions) {
        if ( getProductionHead(prod) == token ) {
            prods.push_back(prod);
        }
    }
    return prods;
}

std::string Parser::getProductionHead(std::string production)
{
    int index = production.find(" -> ");
    std::string head = production.substr(0, index);  // in this case, index should count as length
    return head;
}

std::vector<std::string> Parser::getProductionBodyTokens(std::string production)
{
    std::vector<std::string> tokens;

    int index = production.find(" -> ");
    std::string rest_string = production.substr(index+4);
   // std::cout << "FIRST_REST_STRING->" << rest_string << std::endl;

    while ( rest_string.size() != 0 ) {

        int index2 = rest_string.find(" ");
        std::string token = rest_string.substr(0, index2);
        tokens.push_back(token);

        // We've reached the end of the production
        if ( index2 == -1 ) {
            break;
        }

        rest_string = rest_string.substr(index2+1);
        //std::cout << "New string: " << rest_string << std::endl;
    }

    return tokens;
}

void  Parser::BDict()
{
    std::vector<std::string> bdict_prods = getProductions("BDict");
}

Torrent::Torrent(std::string torrent_data)
{

}
Torrent::~Torrent() {}
