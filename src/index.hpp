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
    sdsl::sd_vector<> eliasfanoL, eliasfanoR;
    sdsl::rank_support_sd<> ef_rankL, ef_rankR;
    sdsl::select_support_sd<> ef_selectL, ef_selectR;

    uint64_t const baseL, baseR;
    uint64_t lastL;

public:
    /** Constructs the index on the given sorted keys. */
    MyIndex(const std::vector<uint64_t> &data): baseL(data[0]), baseR(data[data.size() / 2]) {
        std::vector<uint64_t> uniqueL, uniqueR;
        uniqueL.reserve(data.size() / 2);
        uniqueR.reserve(data.size() / 2);

#pragma omp parallel sections
        {
#pragma omp section
            {
                auto last = baseL;
                uniqueL.push_back(0);
                for (size_t i = 1; i < data.size() / 2; ++i) {
                    if (data[i] != last) {
                        last = data[i];
                        uniqueL.push_back(data[i] - baseL);
                    }
                }
            }

#pragma omp section
            {
                auto last = baseR;
                uniqueR.push_back(0);
                for (size_t i = data.size() / 2 + 1; i < data.size(); ++i) {
                    if (data[i] != last) {
                        last = data[i];
                        uniqueR.push_back(data[i] - baseR);
                    }
                }
            }
        }

        lastL = data[data.size() / 2 - 1];
        //unique.reserve(data.size() / 2);
        /*auto last = base;
        unique.push_back(last);
        for (size_t i = 1; i < data.size(); ++i) {
            if (data[i] != last) {
                last = data[i];
                unique.push_back(data[i]);
            }
        }*/

        eliasfanoL = sdsl::sd_vector<>(uniqueL.begin(), uniqueL.end());
        ef_rankL = sdsl::rank_support_sd<>(&eliasfanoL);
        ef_selectL = sdsl::select_support_sd<>(&eliasfanoL);

        eliasfanoR = sdsl::sd_vector<>(uniqueR.begin(), uniqueR.end());
        ef_rankR = sdsl::rank_support_sd<>(&eliasfanoR);
        ef_selectR = sdsl::select_support_sd<>(&eliasfanoR);
    }

    /** Returns the element greater than or equal to a given key. */
    uint64_t nextGEQ(uint64_t key) const { // This function is required for the challenge
        if (key >= baseR) {
            return baseR + ef_selectR(ef_rankR(key - baseR) + 1);
        } else {
            if (key > lastL)
                return baseR;
            return baseL + ef_selectL(ef_rankL(key - baseL) + 1);
        }
    }

    /** Returns the size of the index in bytes. */
    size_t size_in_bytes() const { // This function is required for the challenge
        return sdsl::size_in_bytes(eliasfanoL) + sdsl::size_in_bytes(eliasfanoR);
    }
};
