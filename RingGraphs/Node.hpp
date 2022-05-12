#pragma once

#include <cstdint>
#include <optional>
#include <typeinfo>
#include <map>
#include <algorithm>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/named_function_params.hpp>

struct Message
{
    std::uint32_t x;
    std::int32_t d;
};

using MessageBuffer = std::multimap<std::uint32_t, Message>;

class Node
{
public:
    std::uint32_t _id;
    bool _initiator = false;

    bool _awake = false;
    std::uint32_t _c = 0;
    std::int32_t _d = 0;
    std::int32_t _b = 1;
    std::uint32_t _pulse = 0;
    std::uint32_t _x = _id;
    MessageBuffer _incoming_messages{};

    // id_map is const because we're not supposed to push the messages in the neighbors' buffers manually
    // we should use message_sender for that
    template <typename PropertyMap, typename Iterator, typename MessageSender>
    bool run_logic(const PropertyMap &id_map, const Iterator &adjacent_begin, const Iterator &adjacent_end, const MessageSender &message_sender)
    {
        // std::cout << "run_logic from Node " << _id << " called " << std::endl;

        if (_d == -1)
        {
            return true;
        }

        if (!_awake && (_initiator || _incoming_messages.size() > 0))
        {
            // std::cout << "this node is awaken" << std::endl;
            _awake = true;
            broadcast(id_map, adjacent_begin, adjacent_end, message_sender);
        }

        bool stopping_condition = false;
        if (all_messages_received(id_map, adjacent_begin, adjacent_end))
        {
            stopping_condition = run_pulse(id_map, adjacent_begin, adjacent_end, message_sender);
        }

        return stopping_condition;
    }

    template <typename PropertyMap, typename Iterator, typename MessageSender>
    void broadcast(const PropertyMap &id_map, const Iterator &adjacent_begin, const Iterator &adjacent_end, const MessageSender &message_sender) const
    {
        Message message{_x, _d};

        for (auto it = adjacent_begin; it != adjacent_end; ++it)
        {
            message_sender(id_map[*it], message);
        }
    }

private:
    template <typename PropertyMap, typename Iterator, typename MessageSender>
    bool run_pulse(const PropertyMap &id_map, const Iterator &adjacent_begin, const Iterator &adjacent_end, const MessageSender &message_sender)
    {
        // std::cout << "run_pulse : " << _pulse << std::endl;

        ++_pulse;

        // Extract oldest message for each neighbor
        std::vector<Message> to_process;
        std::transform(
            adjacent_begin,
            adjacent_end,
            std::back_inserter(to_process),
            [this, id_map](auto neighbor_descriptor)
            {
                auto [begin, end] = _incoming_messages.equal_range(id_map[neighbor_descriptor]);

                Message ret = begin->second;
                _incoming_messages.erase(begin);

                return ret;
            });

        // Completion signal received
        if (std::any_of(to_process.begin(), to_process.end(), [](const auto &message)
                        { return message.d == -1; }))
        {
            // std::cout << "completion signal received" << std::endl;
            _d = -1;
            broadcast(id_map, adjacent_begin, adjacent_end, message_sender);
            return true;
        }

        // Highest node id this node has heard of
        std::optional<std::uint32_t> y = std::nullopt;

        for (const auto &message : to_process)
        {
            if (!y.has_value() || *y < message.x)
            {
                y = message.x;
            }
        }

        // node hears of a new candidate
        // std::optional<>::value() throws an exception if y hasn't been set
        // std::optional<>::operator*() doesn't
        if (y.value() > _x)
        {
            _b = 0;
            _x = *y;
            _d = _pulse;
        }

        if (_b != 0)
        {
            if (*y < _x)
            {
                _c = 1;
            }
            else
            {
                std::optional<std::uint32_t> z = std::nullopt;

                // Longest distance to a known node
                for (const auto &message : to_process)
                {
                    if (!z.has_value() || *z < message.d)
                    {
                        z = message.d;
                    }
                }

                if (z.value() > _d)
                {
                    _d = *z;
                    _c = 0;
                }
                else
                {
                    ++_c;
                }

                if (_c == 2)
                {
                    // Completion, current node is the leader
                    _d = -1;
                    broadcast(id_map, adjacent_begin, adjacent_end, message_sender);
                    return true;
                }
            }
        }

        broadcast(id_map, adjacent_begin, adjacent_end, message_sender);
        return false;
    }

    template <typename PropertyMap, typename Iterator>
    bool all_messages_received(const PropertyMap &id_map, const Iterator &adjacent_begin, const Iterator &adjacent_end) const
    {
        return std::all_of(
            adjacent_begin,
            adjacent_end,
            [this, id_map](auto descriptor)
            {
                std::uint32_t neighbor_id = id_map[descriptor];
                return _incoming_messages.find(neighbor_id) != _incoming_messages.end(); // if neighbor_id is not in _incoming_messages
            });                                                                          // std::all_of returns true if all neighbor_id, i.e. id_map[descriptor] found in message buffer
    }
};

using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, Node, boost::no_property>;