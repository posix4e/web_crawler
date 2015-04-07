#include <curl/curl.h>
#include <iostream>
#include <set>
#include <vector>
#include <future>
#include <boost/lockfree/stack.hpp>
#include "Curler.h"

namespace {
    unsigned long NUMBER_OF_DOWNLOADERS = 1000;
}

long numberOfDownloaders = 0;
bool stopped = false;
std::set<std::string> urlsVisited;
std::set<std::string> websitesToVisit;
boost::lockfree::stack<std::string *> websitesToVisitStack(10240);

std::string enqueLinks(std::string url) {
    if (urlsVisited.count(url) != 0) {
        return "";
    }
    for (auto newUrl:Curler(url.c_str(), urlsVisited).getURLS()) {
        if (newUrl.compare(0, 5, "http:") == 0) {
            if (urlsVisited.count(newUrl) == 0
                || websitesToVisit.count(newUrl) == 0) {
                websitesToVisit.insert(newUrl);
                while (!websitesToVisitStack.push(new std::string(newUrl))) {
                    std::cout << "failed to push" << newUrl << std::endl;
                }
            }
        }
    }
    urlsVisited.insert(url);
    numberOfDownloaders--;
    return "";
}

std::future<std::string> lookForNewLinks(std::string url) {
    numberOfDownloaders++;
    return std::async(std::launch::async, enqueLinks, url);
}

int main(int argc, char *argv[]) {

    if (curl_global_init(CURL_GLOBAL_ALL)) {
        fprintf(stderr, "Could not init cURL\n");
        return 1;
    }
    std::vector<std::future<std::string>> futures;

    std::string firstUrl = "http://www.google.com/";
    std::vector<std::string> firstLinks = Curler(firstUrl.c_str(), urlsVisited).getURLS();
    for (auto url:firstLinks) {
        futures.push_back(lookForNewLinks(url));
    }

    while (!websitesToVisitStack.empty() || futures.size() > 0) {
        if (!websitesToVisitStack.empty() && futures.size() < NUMBER_OF_DOWNLOADERS) {
            std::string *newUrl;
            if (websitesToVisitStack.pop(newUrl)) {
                std::string newUrlCopy = *newUrl;
                delete(newUrl);
                if (urlsVisited.count(newUrlCopy) == 0 || websitesToVisit.count(newUrlCopy) == 0) {
                    futures.push_back(lookForNewLinks(newUrlCopy));
                }
            } else {
                std::cout << "nothing to pop" << std::endl;
            }
        } else if (futures.size() > 0) {
            while (numberOfDownloaders > NUMBER_OF_DOWNLOADERS) {
                std::cout << "Waiting for downloaders to catch up" << numberOfDownloaders << std::endl;
            }
            std::vector<std::future<std::string>> futuresToProcess;
            futures.swap(futuresToProcess);
        } else {
            std::cout << "Doing nothing" << std::endl;
        }

    }
    stopped = true;
}
