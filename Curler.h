//
// Created by posix4e on 4/5/15.
//

#ifndef WEB_CRAWLER_CURLER_H
#define WEB_CRAWLER_CURLER_H

#include <string>
#include <set>
#include <vector>

class Curler {
    std::set<std::string> urlsVisited_;

public:
    Curler(char const *url, std::set<std::string> urlsVisited);
    std::vector <std::string> getURLS();
};


#endif //WEB_CRAWLER_CURLER_H
