#include <vector>
#include <functional>
#include <string>
#include <iostream>
#include <utility>
#include <map>
#include <queue>
#include <numeric>
#include "tinyxml2/tinyxml2.h"
#include "boost/lexical_cast.hpp"
#include "boost/range/algorithm/transform.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/algorithm.hpp"

namespace B = boost;
namespace T = tinyxml2;
namespace S = std;

template<typename Function, typename IN, typename OUT = S::decay_t<S::invoke_result_t<Function &, IN const &>>>
static auto map(Function &&fn, const S::vector<IN> &inputVector) {
    S::vector<OUT> res;
    res.reserve(inputVector.size());
    for (auto &&el : inputVector) res.push_back(fn(el));
    return res;
}

template<typename Function, typename T>
static S::vector<T> filter(const Function &fn, const S::vector<T> &inputVector) {
    S::vector<T> res;
    res.reserve(inputVector.size());
    for (auto &&el : inputVector) if (fn(el)) res.push_back(el);
    return res;
}

template<typename T>
static auto printVector(S::vector<T> v){
    for(auto el: v) S::cout << el << ' ';
    S::cout << '\n';
}

static auto splitString(S::string raw, const S::string &delimiter = " ") {
    using namespace boost::algorithm;
    S::vector<S::string> tokens;
    B::algorithm::trim(raw);
    return raw.empty()
           ? S::vector<S::string>{}
           : split(tokens, raw, is_any_of(delimiter));
}