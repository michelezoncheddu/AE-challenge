#pragma once

#include <algorithm>
#include <limits>
#include <tuple>
#include <utility>
#include <vector>

#include <sdsl/sd_vector.hpp>

/**
 * An implementation of an index.
 */
class MyIndex {
    const std::vector<uint64_t> &data;

    sdsl::sd_vector<> eliasfano;
    sdsl::rank_support_sd<> ef_rank;

    static constexpr int n_samples = 1000000;

    const uint64_t base;
    const size_t block_size;

public:
    /** Constructs the index on the given sorted keys. */
    explicit MyIndex(const std::vector<uint64_t> &data) :
        data(data), base(data[0]), block_size(data.size() / n_samples) {

        std::vector<uint64_t> samples;
        samples.reserve(n_samples + 1);

        // Sampling
        for (int i = 0; i < n_samples; ++i)
            samples.push_back(data[i * block_size] - base);
        samples.push_back(data[data.size() - 1] - base);

        eliasfano = sdsl::sd_vector<>(samples.begin(), samples.end());
        ef_rank = sdsl::rank_support_sd<>(&eliasfano);
    }

    /** Returns the element greater than or equal to a given key. */
    [[nodiscard]] uint64_t nextGEQ(const uint64_t key) const {
//        auto start = std::chrono::high_resolution_clock::now();

        const size_t pos = ef_rank(key - base);

//        auto stop = std::chrono::high_resolution_clock::now();
//        std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count() << std::endl;

        if (pos == 0)
            return *std::lower_bound(data.begin(), data.begin() + block_size, key);
        if (pos == n_samples)
            return *std::lower_bound(data.begin() + (pos - 1) * block_size, data.end(), key);
        return *std::lower_bound(data.begin() + (pos - 1) * block_size, data.begin() + pos * block_size, key);
    }

    /** Returns the size of the index in bytes. */
    [[nodiscard]] size_t size_in_bytes() const {
        return sdsl::size_in_bytes(eliasfano) + sdsl::size_in_bytes(ef_rank);
    }
};
