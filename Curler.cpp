//
// Created by posix4e on 4/5/15.
//

#include "Curler.h"
#include <curl/curl.h>
#include <iostream>
#include "URLFinder.h"


/**
 * This class is responsible for actually downloading web pages. It uses a URLFinder
 * so that way don't need to return the actual webpage content.
 * //TODO we could move out the urlsVisited and getURLS behavior out.
 *
 */

static size_t staticFunction(void *buffer, size_t size, size_t nmemb, void *f) {
    return static_cast<Curler *>(f)->nonStaticFunction(buffer, size, nmemb, f);
}

Curler::Curler(const char *url,
               std::set<std::string> urlsVisited) :
        urlsVisited_(urlsVisited) {
    std::cerr << "<";
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, staticFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    res = curl_easy_perform(curl);
    /* Check for errors */
    if (res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));

    /* always cleanup */
    curl_easy_cleanup(curl);
    std::cerr << ">";

}

std::vector<std::string> Curler::getURLS() {
    URLFinder urlFinder(this->readBuffer_, this->urlsVisited_);
    return urlFinder.getNewURLS();
}

size_t Curler::nonStaticFunction(void *pVoid, size_t i, size_t i1, void *pVoid1) {
    readBuffer_.append((char const *) pVoid, i * i1);
    return i * i1;
}
