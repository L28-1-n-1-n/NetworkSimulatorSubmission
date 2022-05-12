#include <random>

#include <iostream>

#include "Node.hpp"
#include "GraphGen.hpp"
#include "Diameter.hpp"
#include "AsyncSimulation.hpp"

int main()
{
    std::uint64_t random_seed = std::random_device{}();
    // std::uint64_t random_seed = 2786313363;
    std::cout << "Using random seed: " << random_seed << std::endl;
    
    std::default_random_engine random_gen{random_seed};
    
    const uint32_t num_nodes = 10;
    Graph g = generateRandomGraph(num_nodes, 0.1, 0.4, random_gen);
    // recordNodeIDs(G);
    // utils::saveGraphToDot(G, "GraphDotFile");
    // utils::convertDotToSvg("GraphDotFile", "GraphSVG");
	auto diameter = measureGraphDiameter(g);
    if (diameter.has_value()) {
        std::cout << "Diameter: " << *diameter << std::endl;
    }
    else {
        throw std::runtime_error("The graph is not connected.");
    }

    using TimeType = std::uint32_t;
    AsyncSimulation simulation{g, std::poisson_distribution<TimeType>{5.0}, random_gen()};
    simulation.run();
	std::cout << "Diameter : " << *diameter << std::endl;

}
