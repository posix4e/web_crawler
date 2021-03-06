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
    std::string readBuffer_;

public:
    Curler(char const *url, std::set<std::string> urlsVisited);

    std::vector<std::string> getURLS();

    size_t nonStaticFunction(void *pVoid, size_t i, size_t i1, void *pVoid1);
};

#endif //WEB_CRAWLER_CURLER_H
