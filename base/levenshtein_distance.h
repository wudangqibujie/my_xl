//
// Created by jay on 2023/1/2.
//

#ifndef MY_XL_LEVENSHTEIN_DISTANCE_H
#define MY_XL_LEVENSHTEIN_DISTANCE_H

#include <vector>
#include <string>

#include "../base/common.h"

class StrSimilar {
public:
    // Constructor and Destructor
    StrSimilar() { }
    ~StrSimilar() { }

    // Return true (false) if str is (not) in target string list.
    bool Find(const std::string& str,
              const std::vector<std::string>& list);

    // Find the most similar string from string list.
    // Return the minimal levenshtein distance.
    int FindSimilar(const std::string& str,
                    const std::vector<std::string>& list,
                    std::string& result);

private:
    // Levenshtein distance
    int ldistance(const std::string& source,
                  const std::string& target);

};

#endif //MY_XL_LEVENSHTEIN_DISTANCE_H
