#include "lib/smile_license.h"
#include "MCIntegrator.h"
#include "networkLoader.h"

#include <chrono>

int main(int argc, char *argv[]) {
    const auto sampleSize = 10000000;
    const auto in_dim = 100;
    const auto out_dim = 1;
    auto generator = [](auto &in, auto &out) {
        for(const auto i : in) out[0] += i;
//        out[0] = (in[0] * in[0] + in[1] * in[1]) < 1;
    };

    Timer t{};
    std::cout << MCIntegrator(sampleSize, in_dim, out_dim, generator)[0] * 8 << ' ';
    auto seqTime = t.getLap();
    std::cout << "Samples per second: [millions]" << 1.0 * sampleSize / seqTime / 1000 << '\n';

    t.reset();
    std::cout << ParallelMCIntegrator(sampleSize, in_dim, out_dim, generator)[0] * 8 << ' ';
    auto parTime = t.getLap();
    std::cout << "Samples per second: [millions]" << 1.0 * sampleSize / parTime / 1000 << '\n';

    std::cout << "Speed up: " << 1.0 * seqTime / parTime << '\n';

    return 0;
}