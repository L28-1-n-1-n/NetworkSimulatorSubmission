#include "Diameter.hpp"

#include "GraphGen.hpp"

#include <gtest/gtest.h>

class DiameterTest : public ::testing::TestWithParam<std::tuple<std::optional<std::uint64_t>, Graph, std::string>> {};

TEST_P(DiameterTest, ComputeDiameter) {
    auto [diameter, graph, name] = GetParam();
    ASSERT_EQ(measureGraphDiameter(graph), diameter);
}

INSTANTIATE_TEST_SUITE_P(
    DiameterTests,
    DiameterTest,
    ::testing::Values(
        std::tuple{1, generateLineGraph(2), "line_graph_2"},
        std::tuple{4, generateLineGraph(5), "line_graph_5"},
        std::tuple{9, generateLineGraph(10), "line_graph_10"},
        std::tuple{4, generateRingGraph(8), "ring_graph_8"},
        std::tuple{4, generateRingGraph(9), "ring_graph_9"},
        std::tuple{5, generateRingGraph(10), "ring_graph_10"},
        std::tuple{53, generateRingGraph(106), "ring_graph_106"},
        std::tuple{53, generateRingGraph(107), "ring_graph_107"},
        std::tuple{54, generateRingGraph(108), "ring_graph_108"},
        std::tuple{2 + 3 + 1, generateConnectedRingsGraph(5, 7), "rings_graph_5_7"},
        std::tuple{2 + 4 + 1, generateConnectedRingsGraph(4, 9), "rings_graph_4_9"},
        std::tuple{10 + 13 + 1, generateConnectedRingsGraph(20, 27), "rings_graph_20_27"},
        std::tuple{10 + 9 + 1, generateConnectedRingsGraph(20, 18), "rings_graph_20_18"}
    ),
    [](const ::testing::TestParamInfo<DiameterTest::ParamType>& info) {
        return std::get<2>(info.param);
    }
);