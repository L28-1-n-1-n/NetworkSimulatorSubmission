#pragma once

#include <vector>
#include <algorithm>
#include <random>

#include <iostream>
#include <fstream>

#include "Node.hpp"

template<typename T>
class node_pair_iterator {
    static_assert(std::is_integral_v<T>);
    std::pair<T, T> _pair;
    T _mod;

public:
    node_pair_iterator(std::pair<T, T> pair, T mod) : _pair{pair}, _mod{mod} {}

    bool operator==(const node_pair_iterator<T> &other) const
    {
        return _pair == other._pair && _mod == other._mod;
    }

    bool operator!=(const node_pair_iterator<T> &other) const
    {
        return !(*this == other);
    }

    node_pair_iterator& operator++() {
        if ((_pair.second + 1) / _mod >= 1) {
            ++_pair.first;
            _pair.second = _pair.first + 1;
        }
        else {
            ++_pair.second;
        }

        return *this;
    }

    std::pair<T, T> operator*() { return _pair; }
};

namespace std
{
template <typename T>
struct iterator_traits<node_pair_iterator<T>> {
    using value_type = std::pair<T, T>;
    using reference = const value_type&;
    using difference_type = std::ptrdiff_t;
    using pointer = const value_type*;
    using iterator_category = std::forward_iterator_tag;
};
}

Graph generateRandomGraph(std::uint32_t num_nodes, float initiator_probability, float edge_probability, std::default_random_engine& random_gen)
{
    std::ofstream myfile;
    myfile.open("./test.edgelist");
	// std::ofstream myfile("test.edgelist");

	Graph g;

    bool any_initiators = false;
    std::bernoulli_distribution initiator_dist{initiator_probability};
    for (uint32_t i = 0; i < num_nodes; ++i)
    {
        auto descriptor = boost::add_vertex(Node{}, g);
        g[descriptor]._id = descriptor;
		g[descriptor]._x = descriptor;

        if (initiator_dist(random_gen)) {
            std::cout << "Node " << i << " is an initiator" << std::endl;
            g[descriptor]._initiator = true;
            any_initiators = true;
        }
    }

    if (!any_initiators) {
        throw std::runtime_error("No initiators.");
    }

    const std::uint64_t max_edges = num_nodes * (num_nodes - 1) / 2;

    using NumEdgesDistribution = std::binomial_distribution<std::uint64_t>;
    std::uint64_t num_edges = NumEdgesDistribution{max_edges, edge_probability}(random_gen);

    std::cout << "Generating " << num_edges << " edges" << std::endl;
    std::vector<std::pair<std::uint32_t, std::uint32_t>> sampled_edges;
    std::sample(
        node_pair_iterator<std::uint32_t>{{0, 1}, num_nodes},
        node_pair_iterator<std::uint32_t>{{num_nodes - 1, num_nodes}, num_nodes},
        std::back_inserter(sampled_edges),
        num_edges,
        random_gen
    );

    for (const auto &pair : sampled_edges)
    {
        std::cout << "Adding edge: (" << pair.first << ", " << pair.second << ")" << std::endl;
        boost::add_edge(pair.first, pair.second, g);
		myfile << pair.first << " " << pair.second << "\n";
		std::cout << "Written to myfile : " << pair.first << " " << pair.second << std::endl;
    }
	
	myfile.close();
    return g;
}

Graph generateLineGraph(std::uint32_t num_nodes)
{
    Graph g;
    std::optional<boost::graph_traits<Graph>::vertex_descriptor> first_descriptor, last_descriptor;

    for (std::uint32_t i = 0; i < num_nodes; ++i)
    {
        auto descriptor = boost::add_vertex(Node{}, g);
        g[descriptor]._id = descriptor;
        g[descriptor]._x = descriptor;

        if (last_descriptor.has_value()) {
            boost::add_edge(*last_descriptor, descriptor, g);
        }

        if (!first_descriptor.has_value()) {
            first_descriptor = descriptor;
        }
        last_descriptor = descriptor;
    }
    
    return g;
}

Graph generateRingGraph(std::uint32_t num_nodes)
{
    Graph g;
    std::optional<boost::graph_traits<Graph>::vertex_descriptor> first_descriptor, last_descriptor;

    for (std::uint32_t i = 0; i < num_nodes; ++i)
    {
        auto descriptor = boost::add_vertex(Node{}, g);
        g[descriptor]._id = descriptor;
        g[descriptor]._x = descriptor;

        if (last_descriptor.has_value()) {
            boost::add_edge(*last_descriptor, descriptor, g);
        }

        if (!first_descriptor.has_value()) {
            first_descriptor = descriptor;
        }
        last_descriptor = descriptor;
    }
    
    boost::add_edge(first_descriptor.value(), last_descriptor.value(), g);

	for (uint32_t i = 0; i < num_nodes; ++i)
    {
        auto neighbours = boost::adjacent_vertices(i, g);
        for (auto vd : boost::make_iterator_range(neighbours))
        {
            std::cout << "vertex " << i << " has adjacent vertex : " << g[vd]._id << "\n";
        }
    }
    return g;
}

Graph generateConnectedRingsGraph(std::uint32_t num_nodes_a, std::uint32_t num_nodes_b)
{
    Graph g;
    std::vector<boost::graph_traits<Graph>::vertex_descriptor> first_descriptors;

    for (std::uint32_t ring_i = 0; ring_i < 2; ++ring_i) {
        std::optional<boost::graph_traits<Graph>::vertex_descriptor> last_descriptor;

        for (std::uint32_t i = 0; i < (ring_i == 0 ? num_nodes_a : num_nodes_b); ++i)
        {
            auto descriptor = boost::add_vertex(Node{}, g);
            g[descriptor]._id = descriptor;
            g[descriptor]._x = descriptor;

            if (last_descriptor.has_value()) {
                boost::add_edge(*last_descriptor, descriptor, g);
            }

            if (first_descriptors.size() < ring_i + 1) {
                first_descriptors.push_back(descriptor);
            }
            last_descriptor = descriptor;
        }

        boost::add_edge(first_descriptors[ring_i], last_descriptor.value(), g);
    }
    
    boost::add_edge(first_descriptors[0], first_descriptors[1], g);
    
    return g;
}