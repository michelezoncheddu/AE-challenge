#pragma once

#include <algorithm>
#include <cmath>
#include <vector>

using type = int32_t;

/**
 * An implementation of an index.
 */
class MyIndex {
    const std::vector<uint64_t> &data;

    static constexpr float percentage = 1.0 / 100; // 0 < percentage <= 1
    static constexpr type nil = -1;

    const int n_samples;
    const uint64_t base;

    size_t range_size;
    std::vector<type> high, low; // Start and end positions of the buckets.

public:
    /** Constructs the index on the given sorted keys. */
    explicit MyIndex(const std::vector<uint64_t> &data)
    : data{data}, n_samples{(int)(data.size() * percentage)}, base{data[0]} {

        high = std::vector<type>(n_samples, nil);
        low  = std::vector<type>(n_samples, nil);

        range_size = ceil((double)(data[data.size() - 1] - base + 1) / n_samples);
        auto bucket = 0;
        low[bucket] = 0;
        low[n_samples - 1] = high[n_samples - 1] = data.size() - 1;

        size_t i;
        for (i = 1; i < data.size(); ++i) {
            if ((data[i] - base) / range_size == bucket) continue;
            high[bucket] = i - 1;
            bucket = (data[i] - base) / range_size;
            low[bucket] = i;
        }
        high[bucket] = i; // For setting the end of the last bucket.

        // For skipping the empty buckets. Useful for NextGEQ.
        auto jump_to = low[low.size() - 1]; // Last valid start.
        for (i = low.size() - 2; i > 0; --i) {
            if (low[i] == nil)
                low[i] = high[i] = jump_to;
            else
                jump_to = low[i];
        }
    }

    /** Returns the element greater than or equal to a given key. */
    [[nodiscard]] uint64_t nextGEQ(const uint64_t key) const {
        auto pos = (key - base) / range_size;

        auto res = *std::lower_bound(data.begin() + low[pos], data.begin() + high[pos], key);
        return res < key ? data[low[pos + 1]] : res;
    }

    /** Returns the size of the index in bytes. */
    [[nodiscard]] size_t size_in_bytes() const {
        return sizeof(*this) + (low.capacity() + high.capacity()) * sizeof(type);
    }
};
