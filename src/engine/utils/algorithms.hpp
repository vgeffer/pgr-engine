#pragma once


#include <cstddef>
#include <string>
#include <vector>

namespace utils {
 
    /* Borrowed from https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c */
    inline std::vector<std::string> split(const std::string& str, const std::string& delimiter) {

        /* Since the string is modified, make a copy */
        std::string s = str;
        std::vector<std::string> tokens;
        size_t pos = 0;
        std::string token;
        while ((pos = s.find(delimiter)) != std::string::npos) {
            token = s.substr(0, pos);
            tokens.push_back(token);
            s.erase(0, pos + delimiter.length());
        }
        tokens.push_back(s);
    
        return tokens;
    }

    template <class Tp>
    const bool is_last_in_container(const Tp& container, const typename Tp::iterator& iter) {
        return (iter != container.end()) && (iter == --container.end());
    }

    /* Borrowed from https://helloacm.com/cc-function-to-compute-the-bilinear-interpolation/ */
    inline float bilinear_interpolation(float q11, float q12, float q21, float q22, float x1, float x2, float y1, float y2, float x, float y) {
        
        float x2x1 = x2 - x1;
        float y2y1 = y2 - y1;
        float x2x = x2 - x;
        float y2y = y2 - y;
        float yy1 = y - y1;
        float xx1 = x - x1;
        
        return 1.0f / (x2x1 * y2y1) * (
            q11 * x2x * y2y +
            q21 * xx1 * y2y +
            q12 * x2x * yy1 +
            q22 * xx1 * yy1
        );
    }
};
