//
// Created by posix4e on 4/5/15.
//

#include <string>
#include <vector>
#include <boost/algorithm/string/predicate.hpp>
#include <regex>
#include <set>

#include "URLFinder.h"

/*
 * A class which gets all of the links out of a webpage
 */
URLFinder::URLFinder(std::string inputString,
                     std::set<std::string> &urlsAlreadyVisited) {
    this->input = inputString;
    this->urlsVisited = urlsAlreadyVisited;
}

void URLFinder::appendInput(const std::string newInput) {
    input.append(newInput);
}

std::vector<std::string> const URLFinder::getNewURLS() {
    std::vector<std::string> extractedURLS;
    std::smatch m;
    std::regex hrefRegex("<a\\s+(?:[^>]*?\\s+)?href=\"([^\"]*)\"");
    while (std::regex_search(this->input, m, hrefRegex)) {
        for (std::string x:m) {
            if (this->urlsVisited.count(x) == 0 &&
                boost::starts_with(x, "http")) {
                extractedURLS.push_back(x);
            }
            this->input = m.suffix().str();
        }
    }
    return extractedURLS;
}

