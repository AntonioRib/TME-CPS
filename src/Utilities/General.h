#ifndef _GENERAL_H_
#define _GENERAL_H_
#include <time.h>
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

    template <typename M, typename V>
    void mapToVec(const M& m, V& v) {
        for (typename M::const_iterator it = m.begin(); it != m.end(); ++it) {
            v.push_back(it->second);
        }
    }

    int random_0_to_n(int min, int max) {
        srand(time(NULL));
        return min + (rand() % (int)(max - min + 1));
    }
}

#endif