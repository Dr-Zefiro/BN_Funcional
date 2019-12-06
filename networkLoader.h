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
#include "boost/range/numeric.hpp"
#include "xmlUtils.h"

namespace B = boost;
namespace T = tinyxml2;
namespace S = std;

struct CumulativeCpt {
    S::vector<size_t> pRadix;
    size_t lineSize{0};
    S::vector<float> cumulativeCpt;

    static S::vector<size_t> getRadix(const S::vector<size_t> &parentStatusSizes, size_t total) {
        return partial_reduce(S::divides<>(), total, parentStatusSizes);
    };

    [[nodiscard]] S::vector<float> getCumulativeCPT(S::vector<float> cpt) const {
        for (auto line{cpt.begin()}; line < cpt.end(); line += lineSize)
            S::partial_sum(line, line + lineSize, line);
        return cpt;
    }

    size_t getState(const S::vector<size_t> &pStatus, const float sample) {
        const auto line{cumulativeCpt.begin() + B::inner_product(pStatus, pRadix, size_t{})};
        return S::upper_bound(line, line + lineSize, sample) - line;
    };

    CumulativeCpt() = default;

    CumulativeCpt(const S::vector<float> &cpt, const S::vector<size_t> &parentStatusSizes) :
            pRadix(getRadix(parentStatusSizes, cpt.size())),
            lineSize(pRadix.empty() ? cpt.size() : pRadix.back()),
            cumulativeCpt(getCumulativeCPT(cpt)) {}
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

struct BN_Network {
    S::vector<RawNode *> nodes;

    explicit BN_Network(const S::string &name) {
        nodes = map([](auto node) { return new RawNode{node}; }, getXmlNodes(name));
        setParents();
        setLayers();
        setCumulativeCPTs();
    }

    void setCumulativeCPTs() {
        for (const auto n : nodes)
            n->cumulativeCpt = CumulativeCpt(n->cpt, map([](auto p) { return p->stateIds.size(); }, n->parents));
    }

    void setParents() {
        S::map<S::string, RawNode *> nodeMap;
        for (const auto n : nodes)
            nodeMap.insert(S::pair<S::string, RawNode *>{n->id, n});

        for (const auto n : nodes)
            n->parents = map([&nodeMap](const auto p) { return nodeMap.find(p)->second; }, n->parentIdentifiers);
    }

    void setLayers() const {
        S::vector<S::pair<RawNode *, RawNode *>> edges{};
        for (const auto node : nodes)
            for (const auto parent : node->parents)
                edges.emplace_back(S::make_pair(parent, node));

        S::map<RawNode *, S::vector<RawNode *>> adjacencyMap{};
        for (const auto edge : edges) adjacencyMap[edge.first].push_back(edge.second);

        const auto roots{filter([](const auto node) { return node->parents.empty(); }, nodes)};
        setMaxDepth(roots, adjacencyMap);
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

    [[nodiscard]] auto sample() const {
        std::random_device rd;
        std::uniform_real_distribution<> dist{0.0, 1.0};

        auto nodeStates = S::vector<size_t>(nodes.size());
        auto nodeIterator = nodeStates.begin();
        const auto getPStates = [&](const auto p) { return nodeStates.at(p - nodes[0]); };

        for (const auto n : nodes)
            *nodeIterator++ = n->cumulativeCpt.getState(map(getPStates, n->parents), dist(rd));

        return nodeStates;
    }
};


















