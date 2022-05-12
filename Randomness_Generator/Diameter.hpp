#pragma once

#include <vector>

#include "Eigen/Dense"

#include "Node.hpp"

std::optional<std::uint64_t> measureGraphDiameter(const Graph& g)
{
    using AdjacencyMatrix = Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic>;

    const auto num_vertices = boost::num_vertices(g);
    std::vector<AdjacencyMatrix> memoized_matrices;

    // Build the adjacency matrix
    memoized_matrices.emplace_back(AdjacencyMatrix::Identity(num_vertices, num_vertices));

    auto [vertices_begin, vertices_end] = boost::vertices(g);
    for (auto it = vertices_begin; it != vertices_end; ++it)
    {
        auto vertex_descriptor = *it;
        auto [out_begin, out_end] = boost::adjacent_vertices(vertex_descriptor, g);

        for (auto other_it = out_begin; other_it != out_end; ++other_it)
        {
            memoized_matrices[0](vertex_descriptor, *other_it) = 1;
        }
    }

    if (memoized_matrices.back().all()) {
        // Complete graph
        return 1;
    }

    // Calculate adjacency_matrix adj^(2^k) where adj is the adjacency matrix
    // and k is high enough that adj becomes all 1s
    while (!memoized_matrices.back().all()) {
        const auto& new_mat = memoized_matrices.emplace_back(memoized_matrices.back() * memoized_matrices.back());

        auto vec_end = memoized_matrices.end();
        if (*(vec_end - 1) == *(vec_end - 2)) {
            // The matrix reached a steady state but it's not all 1s
            // this means the graph is not connected
            return std::nullopt;
        }
    }

    std::uint64_t log_diameter_upper_bound = memoized_matrices.size() - 1;
    std::uint64_t diameter_upper_bound = 1 << log_diameter_upper_bound;
    std::uint64_t diameter_lower_bound = (1 << (log_diameter_upper_bound - 1)) + 1;
    // Matrix of all 1s not needed anymore
    memoized_matrices.pop_back();

    // Binary search for the first exponent that makes the matrix all 1s
    // between 2^(k - 1) and 2^k
    for (std::uint64_t i = log_diameter_upper_bound - 1; i-- > 0;)
    {
        if (diameter_lower_bound >= diameter_upper_bound) {
            throw std::runtime_error("Bounds met too quickly.");
        }

        AdjacencyMatrix mat = memoized_matrices.back() * memoized_matrices[i];

        if (!mat.all()) {
            // the current exponent is not high enough to make adj all 1s, keep the 2^(k - i) factor
            memoized_matrices.back() = mat;
            diameter_lower_bound += (1 << i);
        }
        else {
            diameter_upper_bound -= (1 << i);
        }
    }

    if (diameter_lower_bound != diameter_upper_bound) {
        std::cerr << diameter_lower_bound << ", " << diameter_upper_bound << std::endl;
        throw std::runtime_error("Bounds didn't meet.");
    }

    return diameter_lower_bound;
}