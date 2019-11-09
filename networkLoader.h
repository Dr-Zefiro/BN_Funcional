#pragma once

#include <string>
#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <queue>
#include "functional_helpers.hpp"
#include "tinyxml2/tinyxml2.h"
#include "boost/lexical_cast.hpp"
#include "boost/range/algorithm/transform.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/algorithm.hpp"

namespace B = boost;
namespace T = tinyxml2;
namespace S = std;

static auto splitString(S::string raw, const S::string &delimiter = " ") {
    S::vector<S::string> tokens;
    B::trim(raw);
    return raw.empty() ? S::vector<S::string>{} : B::split(tokens, raw, B::is_any_of(delimiter));
}

auto getAttrId = [](T::XMLElement *s) { return S::string{s->Attribute("id")}; };

const auto getToken = [](const char *field, T::XMLElement *node, bool nullable = false) {
    auto p = node->FirstChildElement(field);
    return (nullable && !p) ? S::vector<S::string>{} : splitString(p->GetText());
};

auto toVector(T::XMLElement *rootElement, const char *name = nullptr) {
    S::vector<T::XMLElement *> elements{};
    for (auto el = rootElement->FirstChildElement(name); el; el = el->NextSiblingElement(name))
        elements.emplace_back(el);
    return elements;
}

struct RawNode {
    S::string id;
    S::vector<S::string> stateIds;
    S::vector<S::string> parentIds;
    S::vector<RawNode *> parents;

    S::vector<float> cpt;
    S::vector<float> cumulativeCpt;

    explicit RawNode(T::XMLElement *node) :
            id(getAttrId(node)),
            cpt(map(B::lexical_cast<float, S::string>, getToken("probabilities", node))),
            stateIds(map(getAttrId, toVector(node, "state"))),
            parentIds(getToken("parents", node, true)) {}
};

auto getXmlNodes(const S::string &name) {
    auto *doc = new T::XMLDocument{};
    doc->LoadFile(name.c_str());
    auto xmlNodes = doc->FirstChildElement("smile")->FirstChildElement("nodes");

    return toVector(xmlNodes, "cpt");
}

void setParents(S::vector<RawNode *> &nodes) {
    S::map<S::string, RawNode *> nodeMap;
    for (const auto n : nodes)
        nodeMap.insert(S::pair<S::string, RawNode *>{n->id, n});

    for (const auto n : nodes)
        n->parents = map([&nodeMap](const auto p) { return nodeMap.find(p)->second; }, n->parentIds);
}

auto setLayers(S::vector<RawNode *> &nodes) {
    auto incomingEdgesCount = map([](auto const n) { return n->parents.size(); }, nodes);
    S::queue<RawNode *> q;
}

void importBN(const S::string &name) {
    auto rawNodes = map([](auto node) { return new RawNode{node}; }, getXmlNodes(name));
    setParents(rawNodes);
    setLayers(rawNodes);
}

