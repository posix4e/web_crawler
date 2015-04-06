#include <curl/curl.h>
#include <iostream>
#include <string>
#include <regex>
#include <boost/algorithm/string/predicate.hpp>
#include <set>

namespace {
    const size_t BUFFER_SIZE = 1024;
}

std::vector<std::string> const getURLSFromHRefs(std::string buffer);
std::set<std::string> urlsVisited;

static std::string readBuffer;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    readBuffer.append((char const *) contents, realsize);

    return realsize;
}

std::vector<std::string> const addUrlToWorkQueue(const char *url) {
    readBuffer.clear();
    std::cerr << "We are now downloading" << url << std::endl;
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    res = curl_easy_perform(curl);
    /* Check for errors */
    if (res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));

    /* always cleanup */
    curl_easy_cleanup(curl);
    return getURLSFromHRefs(readBuffer);
}

std::vector<std::string> const getURLSFromHRefs(const std::string buffer) {
    std::vector<std::string> extractedURLS;
    std::smatch m;
    std::regex hrefRegex("<a\\s+(?:[^>]*?\\s+)?href=\"([^\"]*)\"");
    while (std::regex_search(readBuffer, m, hrefRegex)) {
        for(std::string x:m){
            if (urlsVisited.count(x) == 0 && boost::starts_with(x, "http")){
                std::cout << "regex_search"  << x << std::endl;

                extractedURLS.push_back(x);
            }
            readBuffer = m.suffix().str();
        }
    }
    return extractedURLS;
}

int main(int argc, char *argv[]) {
    std::string firstUrl = "http://www.google.com/";
    std::vector<std::string> urls = addUrlToWorkQueue(firstUrl.c_str());
    while (urls.size() > 0) {
        std::string url = urls.back();
        urls.pop_back();
        urlsVisited.insert(url);
        std::cout << "foo" << std::endl;

        std::vector<std::string> foundUrls = addUrlToWorkQueue(url.c_str());
        for (auto url:foundUrls){
            if (urlsVisited.count(url) > 0){
                urls.push_back(url);
            }
        }
    }
}