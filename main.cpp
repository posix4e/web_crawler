#include <curl/curl.h>
#include <iostream>
#include <set>
#include <vector>
#include <future>
#include <stack>
#include "Curler.h"

namespace {
    unsigned long NUMBER_OF_DOWNLOADERS = 10000;
}

bool stopped = false;
std::set<std::string> urlsVisited;
std::set<std::string> websitesToVisit;

std::string enqueLinks(std::string url) {
    if (urlsVisited.count(url) != 0) {
        return "";
    }
    for (auto newUrl:Curler(url.c_str(), urlsVisited).getURLS()) {
        if (urlsVisited.count(newUrl) == 0 || websitesToVisit.count(newUrl) == 0) {
            websitesToVisit.insert(newUrl);
        }
    }
    urlsVisited.insert(url);
    return "";
}

std::future<std::string> lookForNewLinks(std::string url) {
    return std::async(std::launch::async, enqueLinks, url);
}

int main(int argc, char *argv[]) {

    if (curl_global_init(CURL_GLOBAL_ALL)) {
        fprintf(stderr, "Could not init cURL\n");
        return 1;
    }
    // The watcher watches for new websites to download
    std::stack<std::future<std::string>> futures;

    std::string firstUrl = "http://www.google.com/";
    std::vector<std::string> firstLinks = Curler(firstUrl.c_str(), urlsVisited).getURLS();
    for (auto url:firstLinks) {
        futures.push(lookForNewLinks(url));
    }

    while (!websitesToVisit.empty() || futures.size() > 0) {
        if (!websitesToVisit.empty() && futures.size() < NUMBER_OF_DOWNLOADERS) {
            if (urlsVisited.count(*websitesToVisit.begin()) == 0) {
                futures.push(lookForNewLinks(*websitesToVisit.begin()));
            }
            websitesToVisit.erase(websitesToVisit.begin());
        } else if (futures.size() > 0) {
            futures.top().wait_for(std::chrono::seconds(1));
            futures.pop();
        } else {
            std::cout << "Doing nothing" << std::endl;
        }

    }
    stopped = true;
}
