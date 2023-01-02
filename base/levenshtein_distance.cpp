//
// Created by jay on 2023/1/2.
//

#include <algorithm>

#include "../base/levenshtein_distance.h"

#define  min(a,b) ((a<b)?a:b)

// Find str in string list.
// Return true if we can find str in target string list.
bool StrSimilar::Find(const std::string& str,
                      const std::vector<std::string>& list) {
    std::vector<std::string>::const_iterator it;
    it = std::find(list.begin(), list.end(), str);
    if (it != list.end()) {
        return true;
    }
    return false;
}

// Find the most similar string from string list.
// Return the minimal levenshtein distance.
int StrSimilar::FindSimilar(const std::string& str,
                            const std::vector<std::string>& list,
                            std::string& result) {
    int min_dis = kInt32Max;
    for (int i = 0; i < list.size(); ++i) {
        int dis = ldistance(str, list[i]);
        if (dis < min_dis) {
            min_dis = dis;
            result = list[i];
        }
    }
    return min_dis;
}

// Calculate Levenshtein distance by using
// dynamic programing (DP).
int StrSimilar::ldistance(const std::string& source,
                          const std::string& target) {
    //step 1
    int n = source.length();
    int m = target.length();
    if (m == 0) return n;
    if (n == 0) return m;
    //Construct a matrix
    typedef std::vector<std::vector<int> >  Tmatrix;
    Tmatrix matrix(n + 1);
    for (int i = 0; i <= n; i++)  { matrix[i].resize(m + 1); }
    //step 2 Initialize
    for (int i = 1; i <= n; i++) { matrix[i][0] = i; }
    for (int i = 1; i <= m; i++) { matrix[0][i] = i; }
    //step 3
    for (int i = 1; i <= n; i++) {
        const char si = source[i - 1];
        //step 4
        for (int j = 1; j <= m; j++) {
            const char dj = target[j - 1];
            //step 5
            int cost;
            if (si == dj) { cost = 0; }
            else { cost = 1; }
            //step 6
            const int above = matrix[i - 1][j] + 1;
            const int left = matrix[i][j - 1] + 1;
            const int diag = matrix[i - 1][j - 1] + cost;
            matrix[i][j] = min(above, min(left, diag));
        }
    }
    //step7
    return matrix[n][m];
}
