#include <iostream>
#include <string>
#include <random>
#include <tuple>
#include <random>
#include <cstdlib>

#include "Node.hpp"
#include "GraphGen.hpp"
#include "Diameter.hpp"
#include "AsyncSimulation.hpp"

int main(int argc, char **argv)
{

	std::string topology;
	std::string synchrony;
	float time_delay;
	std::uint32_t num_nodes;
	std::string verbose;
	float initiator_prob;
	float edge_prob;
	std::string find_diameter;

	bool s = true;
	bool v = true;
	bool d = false;

	if (argc < 9)
	{
		std::cerr << "Usage : ./simulator <topology> <synchrony (s / n)> <time delay> <no. of nodes> <verbose (v / n> <initiator probability> <edge probability> <find diameter>";
		return 1;
	}

	topology = argv[1];
	synchrony = argv[2];
	time_delay = std::stof(argv[3]);
	num_nodes = std::stoul(argv[4]);
	verbose = argv[5];
	initiator_prob = std::stof(argv[6]);
	edge_prob = std::stof(argv[7]);
	find_diameter = argv[8];

	std::cout << "topology : " << topology << std::endl;
	std::cout << "synchrony : " << synchrony << std::endl;
	std::cout << "time_delay : " << time_delay << std::endl;
	std::cout << "num_nodes : " << num_nodes << std::endl;
	std::cout << "verbose : " << verbose << std::endl;
	std::cout << "initiator_prob : " << initiator_prob << std::endl;
	std::cout << "edge_prob : " << edge_prob << std::endl;
	std::cout << "find_diameter : " << find_diameter << std::endl;
	std::uint64_t random_seed = std::random_device{}();

	if (synchrony == "a")
		s = false;
	if (s == true)
		time_delay = 0;
	if (verbose == "n")
		v = false;
	if (find_diameter == "y")
		d = true;

	// std::uint64_t random_seed = 2786313363;
	std::cout << "Using random seed: " << random_seed << std::endl;

	std::default_random_engine random_gen{random_seed};

	Graph g;

	if (topology == "ring")
	{
		std::cout << "Generating Ring Graph " << std::endl
				  << "No. of nodes :" << num_nodes << std::endl
				  << "Initiator probability : " << initiator_prob << std::endl
				  << "Synchronous : " << s << std::endl
				  << "Mean time delay : " << time_delay << std::endl
				  << std::endl;
		g = generateRingGraph(num_nodes, initiator_prob, random_gen);
	}

	else if (topology == "random")
	{
		std::cout << "Generating Random Graph " << std::endl
				  << "No. of nodes :" << num_nodes << std::endl
				  << "Initiator probability : " << initiator_prob << std::endl
				  << "Edge probability : " << edge_prob << std::endl
				  << "Synchronous : " << s << std::endl
				  << "Mean time delay : " << time_delay << std::endl
				  << std::endl;
		g = generateRandomGraph(num_nodes, initiator_prob, edge_prob, random_gen);
	}
	else if (topology == "hypercube")
	{
		std::cout << "Generating Hypercube Graph " << std::endl
				  << "No. of nodes :" << num_nodes << std::endl
				  << "Initiator probability : " << initiator_prob << std::endl
				  << "Edge probability : " << edge_prob << std::endl
				  << "Synchronous : " << s << std::endl
				  << "Mean time delay : " << time_delay << std::endl
				  << std::endl;
		g = generateHyperCubeGraph(num_nodes, initiator_prob, random_gen);
	}

	if (d == true || topology == "random")
	{
		auto diameter = measureGraphDiameter(g);
		if (diameter.has_value())
		{
			std::cout << "Diameter: " << *diameter << std::endl;
		}
		else
		{
			throw std::runtime_error("The graph is not connected.");
		}
		std::cout << "Diameter : " << *diameter << std::endl;
	}

	using TimeType = std::uint32_t;
	AsyncSimulation simulation{g, std::poisson_distribution<TimeType>{time_delay}, random_gen(), s, v};
	simulation.run();

	if (d)
	{
		auto diameter = measureGraphDiameter(g);
		if (diameter.has_value())
		{
			std::cout << "Diameter: " << *diameter << std::endl;
		}
		else
		{
			throw std::runtime_error("The graph is not connected.");
		}
		std::cout << "Diameter : " << *diameter << std::endl;
	}
}