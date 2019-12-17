#pragma once

#include <vector>
#include <numeric>
#include <random>
#include <thread>

#include "pcg-cpp/include/pcg_random.hpp"
#include "pcg-cpp/include/pcg_extras.hpp"
#include "pcg-cpp/include/pcg_uint128.hpp"
#include "functional_helpers.hpp"

namespace S = std;

template<typename T, typename V>
void addVectorsInPlace(S::vector<T> &result, const S::vector<V> &v) {
    S::transform(v.begin(), v.end(), result.begin(), result.begin(), S::plus<>());
};

typedef void GENERATOR(const S::vector<float> &, S::vector<float> &);

struct Sampler {
    pcg32 rng{pcg_extras::seed_seq_from<std::random_device>{} };
    S::uniform_real_distribution<> dist{0.0, 1.0};

    inline void fill(S::vector<float> &s) { for(auto& el : s) el = dist(rng); }
};

auto MCIntegrator(size_t samples, size_t in_dimension, size_t out_dimension, GENERATOR generator) {
    thread_local auto s = Sampler();

    thread_local auto input = S::vector<float>(in_dimension);
    thread_local auto sample = S::vector<float>(out_dimension);
    thread_local auto result = S::vector<double>(out_dimension);

    for (size_t i{0}; i < samples; i++) {
        s.fill(input);
        generator(input, sample);
        addVectorsInPlace(result, sample);
    }

    return map([=](auto v) { return v / samples; }, result);
}

auto ParallelMCIntegrator(size_t samples, size_t in_dimension, size_t out_dimension, GENERATOR generator) {
    auto concurrency = std::thread::hardware_concurrency();
    auto results = S::vector<S::vector<double>>(concurrency);
    S::vector<S::thread> threads;

    for (auto i{0}; i < concurrency; i++){
        auto threadRes = &results[i];
        threads.emplace_back(std::thread([=]() {
            *threadRes = MCIntegrator(samples / concurrency, in_dimension, out_dimension, generator);
        }));
    }

    for (auto &t : threads) t.join();

    auto result = S::vector<double>(out_dimension);

    for (const auto &r : results) addVectorsInPlace(result, r);
    return map([=](auto el) { return el / concurrency; }, result);
}
