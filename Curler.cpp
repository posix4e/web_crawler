//
// Created by posix4e on 4/5/15.
//

#include "Curler.h"
#include <string>
#include <vector>
#include <curl/curl.h>
#include <iostream>
#include "URLFinder.h"

static std::string readBuffer;
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

/**
 * This class is responsible for actually downloading web pages. It uses a URLFinder
 * so that way don't need to return the actual webpage content.
 * //TODO we could move out the urlsVisited and getURLS behavior out.
 *
 */
Curler::Curler(char const *url, std::set<std::string> urlsVisited) :
 urlsVisited_(urlsVisited) {
    readBuffer.clear();
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    std::cerr << "We are now downloading" << url << std::endl;
    res = curl_easy_perform(curl);
    /* Check for errors */
    if (res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));

    /* always cleanup */
    curl_easy_cleanup(curl);
}

std::vector<std::string> Curler::getURLS(){
    URLFinder urlFinder(readBuffer, this->urlsVisited_);
    return urlFinder.getNewURLS();
}

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    readBuffer.append((char const *) contents, realsize);
    return realsize;
}