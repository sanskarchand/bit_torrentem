#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
class Parser
{
public:
    Parser();
    ~Parser();
    std::string parseByteString (std::string &torr_data, int i );
};

#endif // PARSER_HPP
