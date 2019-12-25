#pragma once

#include <vector>
#include <functional>
#include <string>
#include <iostream>
#include <utility>
#include <map>
#include <queue>
#include <numeric>
#include <chrono>

#include "tinyxml2/tinyxml2.h"
#include "boost/lexical_cast.hpp"
#include "boost/range/algorithm/transform.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/algorithm.hpp"
#include "boost/iterator/zip_iterator.hpp"

namespace B = boost;
namespace T = tinyxml2;
namespace S = std;

template<typename Function, typename IN, typename OUT = S::decay_t<S::invoke_result_t<Function &, IN const &>>>
constexpr auto map(Function &&fn, const S::vector<IN> &inputVector) {
    S::vector<OUT> res;
    res.reserve(inputVector.size());
    for (auto &&el : inputVector) res.push_back(fn(el));
    return res;
}

template<typename Function, typename IN, typename OUT = S::decay_t<S::invoke_result_t<Function &, IN const &>>>
constexpr auto partial_reduce(Function &&fn, OUT initVal, const S::vector<IN> &inputVector) {
    S::vector<OUT> res;
    res.reserve(inputVector.size());
    for (auto &&el : inputVector) {
        initVal = fn(initVal, el);
        res.push_back(initVal);
    }
    return res;
}

template<typename Function, typename T>
constexpr static S::vector<T> filter(const Function &fn, const S::vector<T> &inputVector) {
    S::vector<T> res;
    res.reserve(inputVector.size());
    for (auto &&el : inputVector) if (fn(el)) res.push_back(el);
    return res;
}

static auto splitString(S::string raw, const S::string &delimiter = " ") {
    using namespace boost::algorithm;
    S::vector<S::string> tokens;
    B::algorithm::trim(raw);
    return raw.empty()
           ? S::vector<S::string>{}
           : split(tokens, raw, is_any_of(delimiter));
}

template<typename C1, typename C2>
class zip_container {
    C1 *c1;
    C2 *c2;

    typedef B::tuple<decltype(std::begin(*c1)), decltype(std::begin(*c2))> tuple;

public:
    zip_container(C1 &c1, C2 &c2) : c1(&c1), c2(&c2) {}

    typedef B::zip_iterator<tuple> iterator;

    iterator begin() const { return iterator(std::begin(*c1), std::begin(*c2)); }

    iterator end() const { return iterator(std::end(*c1), std::end(*c2)); }
};

template<typename C1, typename C2>
zip_container<C1, C2> zip(C1 &c1, C2 &c2) {
    return zip_container<C1, C2>(c1, c2);
}

template<typename T>
static auto printVector(S::vector<T> v) {
    for (auto el: v) S::cout << el << ' ';
    S::cout << '\n';
}

struct Timer {
    std::chrono::time_point<std::chrono::system_clock> start;

    Timer() {
        start = std::chrono::high_resolution_clock::now();
    }

    auto getLap() {
        using namespace std::chrono;
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        start = high_resolution_clock::now();
        std::cout << duration.count() << "[ms]\n";
        return duration.count();
    }

    auto reset() {
        using namespace std::chrono;
        start = high_resolution_clock::now();
    }
};