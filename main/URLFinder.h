//
// Created by posix4e on 4/5/15.
//

#ifndef WEB_CRAWLER_PARSER_H
#define WEB_CRAWLER_PARSER_H

class URLFinder {
    std::string input;
    std::set<std::string> urlsVisited;

public:
    URLFinder(std::string, std::set<std::string> &);
    std::vector<std::string> const newUrls();

};


#endif //WEB_CRAWLER_PARSER_H
