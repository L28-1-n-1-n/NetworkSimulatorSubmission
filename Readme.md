# Network Simulator
This is a network simulator designed to model synchronous and asynchronous executions over a range of network topologies with various time delays and edge probabilities. Each node execute local logic separately. The local logic used here comes from Peleg's Time-optimal leader-election algorithm[1].

## Architecture
The network simulator is written using object-oriented programming in C++. It is encapsulated into three parts: 
1. The Network Simulator
![Network Simulator Engine](/image/netsim1.png)
![Annotations](/image/netsim2.png)
2. The Graph Generator
3. Nodes that run local logic
![Logic at each node](/image/local_logic.png)

## Compilation
```
g++ -std=c++17 -I ./NetworkSimulator/Eigen/ ./NetworkSimulator/Demo.cpp -o simulator -L ./BOOST/libboost_graph-mt.a
``` 

## Usage

```
./simulator <topology(ring/random/hypercube>)> <synchrony (s / n)> <time delay> <no. of nodes> <verbose (v / n> <initiator probability> <edge probability> <find diameter(y/n)>
```

### Examples
To run an asynchronous execution on a ring topology of 50 nodes with a mean time delay of 2 cycles, with verbose output of messages, at an initiator probability of 0.7 and edge probability of 0.5, without running the diameter finder
```
./simulator ring a 2 50 v 0.7 0.5 n
```

To run a synchronous execution on a random graph topology of 100 nodes with a mean time delay of 5 cycles, no verbose output of messages, at an initiator probability of 0.3 and edge probabilty of 0.8, diameter finder will be run regardless of the last input since it is required to check the connectedness of the randomly-generated graph 
```
./simulator random s 5 100 n 0.3 0.8 n
```

To run an asynchronous execution on a hypercube graph of 32 nodes with a mean time delay of 3 cycles, no verbose output of messages, at an initiator probability of 0.6 and an edge probability of 0.5, and output the diameter
```
./simulator hypercube a 3 32 n 0.6 0.5 y
```
## Parameters
### Topologies
The following 3 topologies are implemented:
1. Ring Graphs - Equal number of nodes and edges
2. Hypercube Graphs - 2^n vertices, n * 2^(n-1) edges, n diameter
3. Random Graphs - graph generated with user's input edge probability, then checked for connectedness. Unconnected random graphs will halt execution. 

Execution will also be terminated when the generated graph has no initiator.

### Time delay
Time delay in global cycles for each message to arrive at destination node in asynchronous executions, modeled by Poisson distribution

[1] D. Peleg , Time-optimal leader election in general net- works, Journal of Parallel and Distributed Computing, Vol 8, Issue 1, pp.96-99, 1990.