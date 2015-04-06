//
// Created by posix4e on 4/5/15.
//

#ifndef WEB_CRAWLER_PARSER_H
#define WEB_CRAWLER_PARSER_H


#include <bits/stl_set.h>

class URLFinder {
    std::string input;
    std::set<std::string> urlsVisited;

public:
    URLFinder(std::string string, std::set<std::string> &set);

    std::vector<std::string> const getNewURLS();

    void appendInput(std::string const);
};


#endif //WEB_CRAWLER_PARSER_H
