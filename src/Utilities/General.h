#ifndef _general_h_
#define _general_h_
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

namespace General {
using namespace std;

    std::vector<std::string> splitString(std::string st){
            std::istringstream stream(st);
            std::vector<std::string> result((std::istream_iterator<std::string>(stream)),
                                                std::istream_iterator<std::string>());

            return result;
    }
}

#endif