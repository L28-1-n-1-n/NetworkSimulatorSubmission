#include <vector>
#include <random>

#include "AsyncSimulation.hpp"

void fillRingGraph(Graph &G, const uint32_t NodeSize)
{
    for (uint32_t i = 0; i < NodeSize; ++i)
    {
        auto descriptor = boost::add_vertex(Node{}, G);
        G[descriptor]._id = i;
        G[descriptor]._x = G[descriptor]._id;
        G[descriptor]._initiator = true;

        // std::cout << "adding vertex " << G[descriptor]._id << std::endl;

        // if (G[descriptor]._id == 2)
        // {
        //     G[descriptor]._initiator = true;
        // }
    }

    for (uint32_t i = 0; i < NodeSize - 1; ++i)
    {
        boost::add_edge(i, i + 1, G);
        // std::cout << "adding edge " << i << " - " << i + 1 << std::endl;
    }
    boost::add_edge(0, NodeSize - 1, G);
    // std::cout << "adding edge " << 0 << " - " << NodeSize - 1 << std::endl;

    // for (uint32_t i = 0; i < NodeSize; ++i)
    //    {
    //        auto neighbours = boost::adjacent_vertices(i, G);
    //        for (auto vd : boost::make_iterator_range(neighbours))
    //        {
    //            std::cout << "vertex " << i << " has adjacent vertex : " << G[vd]._id << "\n";
    //        }
    //    }
}

int main()
{
    const uint32_t NodeSize = 500;
    Graph G;
    fillRingGraph(G, NodeSize);

    using TimeType = std::uint32_t;
    // seed the random engine
    AsyncSimulation simulation{G, std::poisson_distribution<TimeType>{5}, std::default_random_engine{std::random_device{}()}};
    simulation.run();
}
