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

typedef size_t nodeState;

const auto getAttrId = [](T::XMLElement *s) { return S::string{s->Attribute("id")}; };

const auto getToken = [](const char *field, T::XMLElement *node, bool nullable = false) {
    const auto p{node->FirstChildElement(field)};
    return (nullable && !p) ? S::vector<S::string>{} : splitString(p->GetText());
};

auto toVector(T::XMLElement *rootElement, const char *name = nullptr) {
    S::vector<T::XMLElement *> elements{};
    for (auto el{rootElement->FirstChildElement(name)}; el; el = el->NextSiblingElement(name))
        elements.emplace_back(el);
    return elements;
}

struct CumulativeCpt {
    S::vector<size_t> pRadix;
    size_t lineSize{0};
    S::vector<float> cumulativeCpt;

    CumulativeCpt() = default;

    CumulativeCpt(const S::vector<float> &cpt, const S::vector<size_t> &parentStatusSizes) :
            pRadix(getRadix(parentStatusSizes, cpt.size())),
            lineSize(pRadix.empty() ? cpt.size() : pRadix.back()),
            cumulativeCpt(getCumulativeCPT(cpt, lineSize)) {}

    static S::vector<size_t> getRadix(const S::vector<size_t> &parentStatusSizes, size_t total) {
        S::vector<size_t> res{};
        for (const auto p : parentStatusSizes) {
            total /= p;
            res.emplace_back(total);
        }
        return res;
    };

    static std::vector<float> getCumulativeCPT(S::vector<float> cpt, const size_t lineSize) {
        for (auto line{cpt.begin()}; line < cpt.end(); line += lineSize)
            S::partial_sum(line, line + lineSize, line);
        return cpt;
    }

    size_t getState(const S::vector<size_t> &pStatus, const float sample) {
        const auto offset{S::inner_product(pStatus.begin(), pStatus.end(), pRadix.begin(), size_t{})};
        const auto line{cumulativeCpt.begin() + offset};
        return S::upper_bound(line, line + lineSize, sample) - line;
    };
};

struct RawNode {
    S::string id;
    S::vector<S::string> stateIds;
    S::vector<S::string> parentIdentifiers;
    S::vector<RawNode *> parents;

    S::vector<float> cpt;
    CumulativeCpt cumulativeCpt;

    size_t depth{0};

    explicit RawNode(T::XMLElement *node) :
            id(getAttrId(node)),
            cpt(map(B::lexical_cast<float, S::string>, getToken("probabilities", node))),
            stateIds(map(getAttrId, toVector(node, "state"))),
            parentIdentifiers(getToken("parents", node, true)) {}
};

struct BN_Network{
    S::vector<RawNode *> &nodes;

    static auto getXmlNodes(const S::string &name) {
        auto *doc{new T::XMLDocument{}};
        doc->LoadFile(name.c_str());
        const auto xmlNodes{doc->FirstChildElement("smile")->FirstChildElement("nodes")};

        return toVector(xmlNodes, "cpt");
    }

    void setCumulativeCPTs() const {
        for (const auto n : nodes)
            n->cumulativeCpt = CumulativeCpt(n->cpt, map([](auto p) { return p->stateIds.size(); }, n->parents));
    }

    static void setMaxDepth(const S::vector<RawNode *> &roots,
                     const S::map<RawNode *, S::vector<RawNode *>> &adjacencyMap,
                     const size_t depth = 0) {
        assert(adjacencyMap.size() >= depth);
        for (const auto root : roots) {
            root->depth = S::max(depth, root->depth);
            const auto edges{adjacencyMap.find(root)};
            if (edges == adjacencyMap.end()) continue;
            setMaxDepth(edges->second, adjacencyMap, depth + 1);
        }
    }

    static void setParents(S::vector<RawNode *> &nodes) {
        S::map<S::string, RawNode *> nodeMap;
        for (const auto n : nodes)
            nodeMap.insert(S::pair<S::string, RawNode *>{n->id, n});

        for (const auto n : nodes)
            n->parents = map([&nodeMap](const auto p) { return nodeMap.find(p)->second; }, n->parentIdentifiers);
    }

    auto setLayers() const {
        S::vector<S::pair<RawNode *, RawNode *>> edges{};
        for (const auto node : nodes)
            for (const auto parent : node->parents)
                edges.emplace_back(S::make_pair(parent, node));

        S::map<RawNode *, S::vector<RawNode *>> adjacencyMap{};
        for (const auto edge : edges) adjacencyMap[edge.first].push_back(edge.second);

        const auto roots{filter([](const auto node) { return node->parents.empty(); }, nodes)};
        setMaxDepth(roots, adjacencyMap);
    }

    static S::vector<size_t> sample(){
        std::random_device rd;
        std::uniform_real_distribution<> dist{0.0, 1.0};

        auto nodeStates = S::vector<size_t>(nodes.size());
        auto nodeIterator = nodeStates.begin();

        for(const auto n : nodes){
            const auto val = dist(rd);
            const auto pStates = map([&](const auto p) -> size_t {return nodeStates.at(p - nodes[0]); }, n->parents);
            const auto state = n->cumulativeCpt.getState(pStates, val);
            *nodeIterator = state;
            nodeIterator++;
        }
        return nodeStates;
    }

    void importBN(const S::string &name) {
        auto rawNodes{map([](auto node) { return new RawNode{node}; }, getXmlNodes(name))};
        setParents(rawNodes);
        setLayers(rawNodes);
        setCumulativeCPTs(rawNodes);
        for(auto i{0}; i < 10; i++)
            printVector(sample(rawNodes));
    }
};


















