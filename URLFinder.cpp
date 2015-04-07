//
// Created by posix4e on 4/5/15.
//

#include <string>
#include <vector>
#include <boost/algorithm/string/predicate.hpp>
#include <regex>
#include <set>
#include "gumbo.h"

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

static std::vector<std::string> search_for_links(GumboNode* node) {
    std::vector<std::string> links;
    if (node->type != GUMBO_NODE_ELEMENT) {
        return links;
    }
    GumboAttribute* href;
    if (node->v.element.tag == GUMBO_TAG_A &&
        (href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
        links.push_back(href->value);
    }

    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i) {
        std::vector<std::string> childrenLinks = search_for_links(static_cast<GumboNode*>(children->data[i]));
        for (auto url:childrenLinks){
            if (url.find("http") == 0) {
                links.push_back(url);
            }
        }
    }
    return links;
}

std::vector<std::string> const URLFinder::getNewURLS() {
    GumboOutput* output = gumbo_parse(this->input.c_str());

    std::vector<std::string> links = search_for_links(output->root);
    gumbo_destroy_output(&kGumboDefaultOptions, output);
    return links;
}

