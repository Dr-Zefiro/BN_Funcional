#include <experimental/type_traits>
#include <vector>

namespace detail {
    template<class F>
    using is_zero_callable_type = decltype(std::declval<F>()());

    template<class F>
    constexpr bool is_zero_callable_v = std::experimental::is_detected_v<is_zero_callable_type, F>;
}

template<class F>
struct curry_t {
    F f;

    static constexpr bool is_zero_callable = detail::is_zero_callable_v<F>;

    explicit constexpr curry_t(F fun) : f(fun) {}

    constexpr operator auto() { if constexpr (is_zero_callable) return f(); else return *this; }

    template<class... Args>
    constexpr auto operator()(const Args... args) {
        static_assert(sizeof...(args) == 0 || !is_zero_callable, "Attempting to apply too many arguments!");
        return curry([=](auto... rest) -> decltype(f(args..., rest...)) { return f(args..., rest...); });
    }
};

// Make curried functions out of lambdas, functions, callable objects in general
//
// example:
// auto add = [](int a, int b) { return a + b; };
// auto c_add = curry(add);
// c_add(1, 2) // 3
// c_add(1)(2) // 3
template<class F>
constexpr auto curry(const F func) { return curry_t<F>{func}; }


// Make curried functions out of member functions, binding them to the proper object
//
// example:
// struct X {
// X(int a) : a{a} {};
// int a;
// [[nodiscard]] int addToA(const int x, const int y) const { return a + x + y; }
// int operator()(int x, int y) { return addToA(x, y); };
// };
// auto c_add = curry(&X::addToA, x{1000});
// c_add(1, 2); // 1003
// c_add(1)(2); // 1003
template<typename C, typename R, typename... Args>
constexpr auto curry(R(C::*f)(Args...) const, const C &c) {
    return curry([f, &c](const Args &... args) { return (c.*f)(args...); });
}


template<class F>
struct flip_t {
    F f;

    explicit constexpr flip_t(F fun) : f(fun) {}

    template<typename Arg1, typename Arg2>
    constexpr auto operator()(Arg1 a1, Arg2 a2) { return f(a2, a1); };
};

// Flips the arguments of a function and automatically curries it,
// it works with lambdas, functions, callable objects in general
//
// example:
// auto flipped = flip([](int a, int b) { return a - b; })
// flipped(1, 0) // -1
// flipped(1)(0) // -1
template<class F>
constexpr auto flip(F f) { return curry(flip_t<F>(f)); }

template<typename C, typename R, typename... Args>
constexpr auto flip(R(C::*f)(Args...) const, const C &c) {
    return curry([f, &c](const Args &... args) { return (c.*f)(args...); });
}

template<typename Function, typename IN, typename OUT = std::decay_t<std::invoke_result_t<Function &, IN const &>>>
static auto map(Function &&fn, const std::vector<IN> &inputVector) {
    std::vector<OUT> res;
    res.reserve(inputVector.size());
    for (auto &&el : inputVector) res.push_back(fn(el));
    return res;
}

auto add_c = curry([](int a, int b) { return a + b; });
auto add_1 = add_c(1);