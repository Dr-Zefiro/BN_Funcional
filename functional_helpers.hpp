#include <vector>
#include <functional>

template<typename Function, typename IN, typename OUT = std::decay_t<std::invoke_result_t<Function &, IN const &>>>
static auto map(Function &&fn, const std::vector<IN> &inputVector) {
    std::vector<OUT> res;
    res.reserve(inputVector.size());
    for (auto &&el : inputVector) res.push_back(fn(el));
    return res;
}


template<typename Function, typename T>
static std::vector<T> filter(const Function &fn, const std::vector<T> &inputVector) {
    std::vector<T> res;
    res.reserve(inputVector.size());
    for (auto &&el : inputVector) if(fn(el)) res.push_back(el);
    return res;
}
