#include <curl/curl.h>
#include <iostream>
#include <set>
#include <vector>
#include "Curler.h"

namespace {
    const size_t BUFFER_SIZE = 1024;
}

std::set<std::string> urlsVisited;

int main(int argc, char *argv[]) {
    std::string firstUrl = "http://www.google.com/";
    Curler curler(firstUrl.c_str(), urlsVisited);
    std::vector<std::string> urls = curler.getURLS();
    while (urls.size() > 0) {
        std::string url = urls.back();
        urls.pop_back();
        urlsVisited.insert(url);
        Curler curler(url.c_str(), urlsVisited);
        std::vector<std::string> foundUrls = curler.getURLS();
        for (auto url:foundUrls){
            if (urlsVisited.count(url) > 0){
                urls.push_back(url);
            }
        }
    }
}