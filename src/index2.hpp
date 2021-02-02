#pragma once

#include <algorithm>
#include <limits>
#include <tuple>
#include <utility>
#include <vector>

/**
 * An implementation of an index.
 */
class MyIndex {
    const std::vector<uint64_t> &data;

    //static constexpr int n_samples = 1000000;

    const int n_samples;
    const uint64_t base;

    size_t range_size;
    std::vector<long> high, low;

public:
    /** Constructs the index on the given sorted keys. */
    explicit MyIndex(const std::vector<uint64_t> &data) : data{data}, n_samples(data.size() / 500), base{data[0]} {
        high = std::vector<long>(n_samples, -1);
        low  = std::vector<long>(n_samples, -1);

        range_size = (data[data.size() - 1] - base + 1) / n_samples + 1; // ceiling, not + 1
        int block = 0;
        low[block] = 0;
        low[n_samples - 1] = high[n_samples - 1] = data.size() - 1;

        for (size_t i = 1; i < data.size(); ++i) {
            if ((data[i] - base) / range_size == block) continue;
            high[block] = i - 1;
            block = (data[i] - base) / range_size;
            low[block] = i;
        }

        auto last = low[low.size() - 1];
        for (size_t i = low.size() - 2; i > 0; --i) {
            if (low[i] == -1)
                low[i] = high[i] = last;
            else
                last = low[i];
        }

        for (size_t i = 0; i < high.size(); ++i)
            if (high[i] == -1)
                high[i] = data.size() - 1;
    }

    /** Returns the element greater than or equal to a given key. */
    [[nodiscard]] uint64_t nextGEQ(const uint64_t key) const {
        auto pos = (key - base) / range_size;

        auto res = *std::lower_bound(data.begin() + low[pos], data.begin() + high[pos], key);
        if (res < key)
            return data[low[pos + 1]];
        else
            return res;
    }

    /** Returns the size of the index in bytes. */
    [[nodiscard]] size_t size_in_bytes() const {
        return (low.size() + high.size()) * sizeof(long) + size(low) + size(high);
    }
};
