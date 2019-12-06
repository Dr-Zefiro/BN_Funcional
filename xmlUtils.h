#pragma once

#include <string>
#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <queue>
#include <numeric>
#include <random>
#include "functional_helpers.hpp"
#include "tinyxml2/tinyxml2.h"
#include "boost/lexical_cast.hpp"
#include "boost/range/algorithm/transform.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/algorithm.hpp"

namespace B = boost;
namespace T = tinyxml2;
namespace S = std;

const auto getAttrId = [](T::XMLElement *s) { return S::string{s->Attribute("id")}; };

const auto getToken = [](const char *field, T::XMLElement *node, bool nullable = false) {
    const auto p{node->FirstChildElement(field)};
    return (nullable && !p) ? std::vector<S::string>{} : splitString(p->GetText());
};

auto toVector(T::XMLElement *rootElement, const char *name = nullptr) {
    S::vector<T::XMLElement *> elements{};
    for (auto el{rootElement->FirstChildElement(name)}; el; el = el->NextSiblingElement(name))


        elements.emplace_back(el);
    return elements;
}

auto getXmlNodes(const S::string &name) {
    auto *doc{new T::XMLDocument{}};
    doc->LoadFile(name.c_str());
    const auto xmlNodes{doc->FirstChildElement("smile")->FirstChildElement("nodes")};

    return toVector(xmlNodes, "cpt");
}