#pragma once

#include <iostream>

#include <random>
#include <unordered_map>
#include <queue>

#include <boost/graph/adjacency_list.hpp>

// #include <boost/graph/properties.hpp>
// #include <boost/property_map/property_map.hpp>
// #include <boost/graph/named_function_params.hpp>

#include <typeinfo>

#include "Node.hpp"

template <typename DelayDistribution>
class AsyncSimulation
{
public:
    using TimeType = typename DelayDistribution::result_type;
    using VertexDescriptor = boost::graph_traits<Graph>::vertex_descriptor;

    // AsyncSimulation(Graph &graph, DelayDistribution delay_distribution, std::uint64_t random_seed)
    //     : _graph{graph}, _delay_distribution{delay_distribution}, _random_engine{random_seed}
	    AsyncSimulation(Graph &graph, DelayDistribution delay_distribution, std::uint64_t random_seed, bool sync, bool verbose)
        : _graph{graph}, _delay_distribution{delay_distribution}, _random_engine{random_seed}, _sync{sync}, _verbose{verbose}    
{
        auto id_map = boost::get(&Node::_id, _graph);

        auto [begin, end] = boost::vertices(_graph);
        for (auto it = begin; it != end; ++it)
        {
            _node_map.try_emplace(id_map[*it], *it);
        }
    }

    std::uint32_t run()
    {
        // std::cout << "Verbose is initialized to " << _verbose << std::endl;
        // std::cout << "Sync is initialized to " << _sync << std::endl;
		
		std::unordered_map<std::uint32_t, bool> termination_map{};

        auto id_map = boost::get(&Node::_id, _graph);

        auto [begin, end] = boost::vertices(_graph);
        for (auto it = begin; it != end; ++it)
        {
            auto &node = _graph[*it];
            termination_map.emplace(node._id, false);
            if (node._initiator)
            {
                auto [adjacent_begin, adjacent_end] = boost::adjacent_vertices(*it, _graph);
                node.run_logic(id_map, adjacent_begin, adjacent_end, make_message_sender(node._id));
            }
        }

        do
        {
            if (_message_queue.empty())
            {
                throw std::runtime_error("Event queue is empty but the algorithm hasn't terminated.");
            }
            MessageWrapper message_wrapper = _message_queue.top();
            auto target_descriptor = _node_map.at(message_wrapper._target);
            _message_queue.pop();
            messageCount += 1;
            _current_time = message_wrapper._arrival_time;
            // std::cout << "current time updated to:" << message_wrapper._arrival_time << std::endl;

            auto &target_node = _graph[target_descriptor];
            target_node._incoming_messages.emplace(
                message_wrapper._source,
                message_wrapper._message);

            auto [begin, end] = boost::adjacent_vertices(target_descriptor, _graph);
            termination_map.at(target_node._id) = target_node.run_logic(
                id_map,
                begin,
                end,
                make_message_sender(target_node._id));

            // nodes use a callable for sending messages so that their logic stays the same
            // regardless of sync/async simulations and how the delay is decided
            // run_logic returns true if the node wants to terminate running
        } while (std::any_of(termination_map.begin(), termination_map.end(), [](const auto &pair)
                             { return !pair.second; }));
        std::cout << "Leader elected : " << _graph[*boost::vertices(_graph).first]._x
                  << std::endl
                  << "Termination time : " << _current_time
                  << std::endl
                  << "Message count : " << messageCount
                  << std::endl;
        return _graph[*boost::vertices(_graph).first]._x;
    }

private:
    Graph &_graph;
    DelayDistribution _delay_distribution;
    std::default_random_engine _random_engine;
	bool _verbose;
	bool _sync;
    std::uint64_t messageCount = 0;

    auto make_message_sender(std::uint32_t source)
    {
        return [this, source](std::uint32_t target, const Message &message)
        {
			std::uint32_t arrival_time;
			if (_sync == true)
				arrival_time = _current_time + 1;
			else
            	arrival_time = _current_time + _delay_distribution(_random_engine) + 1;
            
			MessageWrapper message_wrapper{
                arrival_time,
                source,
                target,
                message};
            _message_queue.emplace(message_wrapper);

			if (_verbose == true)
			{
				std::cout << "MESSAGE SENDER : " << std::endl;
	            std::cout << "    current_time: " << _current_time << std::endl;
	            std::cout << "    arrival_time: " << arrival_time << std::endl;
	            std::cout << "    source : " << source << std::endl;
	            std::cout << "    target : " << target << std::endl;
	            std::cout << "    message._x : " << message.x << std::endl;
	            std::cout << "    message._d : " << message.d << std::endl;
			}
        };
    }

    struct MessageWrapper
    {
        TimeType _arrival_time;
        std::uint32_t _source;
        std::uint32_t _target;
        Message _message;

        friend bool operator>(const MessageWrapper &a, const MessageWrapper &b)
        {
            return a._arrival_time > b._arrival_time;
        }
    };

    TimeType _current_time{0};
    std::unordered_map<std::uint32_t, VertexDescriptor> _node_map{};
    std::priority_queue<MessageWrapper, std::vector<MessageWrapper>, std::greater<MessageWrapper>> _message_queue{};
};
