#include "Curler.h"
#include <curl/curl.h>
#include <iostream>
#include "URLFinder.h"
#include "gumbo.h"

/**
 * This class is responsible for actually downloading web pages. It uses a class URLFinder below it
 * so that way don't need to worry about parsing webpges
 *
 */

static size_t staticFunction(void *buffer, size_t size, size_t nmemb, void *f) {
    return static_cast<Curler *>(f)->nonStaticFunction(buffer, size, nmemb, f);
}

Curler::Curler(const char *url, std::set<std::string> urlsVisited) :
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
    return urlFinder.newUrls();
}

size_t Curler::nonStaticFunction(void *pVoid, size_t i, size_t i1, void *pVoid1) {
    readBuffer_.append((char const *) pVoid, i * i1);
    return i * i1;
}

/*
 * A class which gets all of the links out of a webpage
 */
URLFinder::URLFinder(std::string inputString,
                     std::set<std::string> &urlsAlreadyVisited) {
    this->input = inputString;
    this->urlsVisited = urlsAlreadyVisited;
}

static std::vector<std::string> search_for_links(GumboNode *node) {
    std::vector<std::string> links;
    if (node->type != GUMBO_NODE_ELEMENT) {
        return links;
    }
    GumboAttribute *href;
    if (node->v.element.tag == GUMBO_TAG_A &&
        (href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
        links.push_back(href->value);
    }

    GumboVector *children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i) {
        std::vector<std::string> childrenLinks = search_for_links(static_cast<GumboNode *>(children->data[i]));
        for (auto url:childrenLinks) {
            if (url.find("http") == 0) {
                links.push_back(url);
            }
        }
    }
    return links;
}

std::vector<std::string> const URLFinder::newUrls() {
    GumboOutput *output = gumbo_parse(this->input.c_str());
    std::vector<std::string> links = search_for_links(output->root);
    gumbo_destroy_output(&kGumboDefaultOptions, output);
    return links;
}

