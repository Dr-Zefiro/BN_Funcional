#include "lib/smile_license.h"
#include "MCIntegrator.h"
#include "networkLoader.h"

int main(int argc, char *argv[]) {
    std::cout << "--- Start ---\n\n";
//
//    const auto sampleSize = 1000;
//    const auto in_dim = 10000000;
//    const auto out_dim = 1;
//    auto generator = [](const auto &in, auto &out) {
//        for(const auto i : in) out[0] += i;
//        out[0] /= in.size();
////        out[0] = (in[0] * in[0] + in[1] * in[1]) < 1;
//    };
//
    Timer t{};
//    MCIntegrator(sampleSize, in_dim, out_dim, generator);
//    auto seqTime = t.getLap();
//    std::cout << "Samples per second: [millions]" << 1.0 * sampleSize / seqTime / 1000 << '\n';
//
//    t.reset();
//    ParallelMCIntegrator(sampleSize, in_dim, out_dim, generator);
//    auto parTime = t.getLap();
//    std::cout << "Samples per second: [millions]" << 1.0 * sampleSize / parTime / 1000 << '\n';
//
//    std::cout << "Speed up: " << 1.0 * seqTime / parTime << "\n\n";

    Sampler s;
    // t.reset();
    // S::vector<float> vec{0};
    auto randSamples = 1000 * 1000 * 1000;
    // for(auto i{0}; i < randSamples; i++) s.fill(vec);
    // double fillLap = t.getLap();
    // std::cout << "---Fill--- Samples per second: [millions]" << 1.0 * randSamples / fillLap / 1000 << '\n';

    volatile float data;
    t.reset();
    for(auto i{0}; i < randSamples; i++) data = s.next();

    double nextLap = t.getLap();
    std::cout << "---Next--- Samples per second: [millions]" << 1.0 * randSamples / nextLap / 1000 << '\n';

    return 0;
}

///home/zefiro/Documents/BN_Funcional/cmake-build-release/smileTest /home/zefiro/Documents/BN_Funcional/networks/VentureBN.xdsl
//4.00006 6926[ms]
//Samples per second: [millions]0.00144383
//4.0001 407[ms]
//Samples per second: [millions]0.02457
//Speed up: 17.0172
//---Fill--- Samples per second: [millions]5301[ms]
//188.644
//---Next--- Samples per second: [millions]5538[ms]
//180.571
//0.947738
//
//Process finished with exit code 0
