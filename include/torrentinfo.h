#ifndef TORRENTINFO_H
#define TORRENTINFO_H

#include <iostream>
#include <vector>
#include <string>
#include <map>

// BDict -> d<BString><BElem>e        //BString maps to BElem
// BList-> l<BX>e
// BX-> BElem<BX> | E // epsilon
// BElem-> <BString> | <BInteger>
// BInteger-> i[Number]e  // base-10 interpretation of ASCII
// BString-> [Number]:[String]

class Parser;
typedef void(Parser::*FuncPtr)(void);   // function pointer type
//using PF = void(*)();

// Simple top-down parser
class Parser {

private:
    std::vector<std::string> non_terms = { "BDict", "Blist", "BX", "BElem", "BString", "BInteger" };
    std::vector<std::string> productions = {
            "BDict -> d BString BElem e",
            "BList -> l BX e",
            "BX -> BElem BX",
            "BX -> <epsilon>",
            "BElem -> BString",
            "BElem -> BInteger",
            "BInteger -> i [Number] e",
            "BString -> [Number] : [String]"
    };

    //[Number] and [String] would be the terminals

    std::map<std::string, FuncPtr> gram_map;
public:

    Parser();

    bool isNonTerminal(std::string token);
    std::vector<std::string> getProductions(std::string token);
    std::vector<std::string> getProductionBodyTokens(std::string token);
    // one fuction for each nonterm
    void BDict();
    void BList();
    void BX();
    void BElem();
    void BString();
    void BInteger();

};

class Torrent {

public:
    Torrent(std::string torrent_data);
    ~Torrent();

};


#endif // TORRENTINFO_H
