#pragma once

#include <algorithm>
#include <limits>
#include <tuple>
#include <utility>
#include <vector>

#include <sdsl/sd_vector.hpp>

/**
 * An implementation of a one-level PGM-index that uses binary search on the segments.
 */
class MyIndex {
    const std::vector<uint64_t> &data;
    sdsl::sd_vector<> eliasfano;
    sdsl::rank_support_sd<> ef_rank;

    const uint64_t base;
    const int n_samples = 1000000;
    const size_t block_size;

public:
    /** Constructs the index on the given sorted keys. */
    explicit MyIndex(const std::vector<uint64_t> &data) :
        data(data), base(data[0]), block_size(data.size() / n_samples) {

        std::vector<uint64_t> samples;
        samples.reserve(n_samples);

        // Sampling
        for (long i = 0; i < n_samples; ++i)
            samples.push_back(data[i * block_size] - base);
        samples.push_back(data[data.size() - 1]);

        eliasfano = sdsl::sd_vector<>(samples.begin(), samples.end());
        ef_rank = sdsl::rank_support_sd<>(&eliasfano);
    }

    /** Returns the element greater than or equal to a given key. */
    [[nodiscard]] uint64_t nextGEQ(uint64_t key) const { // This function is required for the challenge
//        auto start = std::chrono::high_resolution_clock::now();

        const size_t pos = ef_rank.rank(key - base);

//        auto stop = std::chrono::high_resolution_clock::now();
//        std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count() << std::endl;

        uint64_t res;

        if (pos == 0)
            res = *std::lower_bound(data.begin(), data.begin() + block_size, key);
        else if (pos == n_samples)
            res = *std::lower_bound(data.begin() + (pos - 1) * block_size, data.end(), key);
        else res = *std::lower_bound(data.begin() + (pos - 1) * block_size, data.begin() + pos * block_size, key);

        return res;
    }

    /** Returns the size of the index in bytes. */
    [[nodiscard]] size_t size_in_bytes() const { // This function is required for the challenge
        return sdsl::size_in_bytes(eliasfano) + sdsl::size_in_bytes(ef_rank);
    }
};
