#include <iostream>
#include <fstream>

#include <FastDelegate.h>

#include "krzysiek/results_container.h"
#include "krzysiek/network_array.h"
#include "krzysiek/network_hash_map.h"
#include "krzysiek/network_helper.h"

typedef fastdelegate::FastDelegate3<network_t&, int, std::vector<
	size_t>& > grouping_func_t;

typedef fastdelegate::FastDelegate4<network_t&, std::vector<size_t>&,
	int, network_path&, int> algorithm_func_t;

#include "mpiech/kpp_hash_map.h"
#include "mpiech/pph_hash_map.h"
#include "mpiech/grouping_functions.h"

size_t get_trees_count(network_t& network, int delta, int load,
	grouping_func_t grouping_func, algorithm_func_t algorithm_func) {

	size_t trees = 0;
	while (network_helper::is_connected(network)) {

		size_t multicast_nodes_count = 80;
		std::vector<size_t> multicast_nodes;
		grouping_func(network, multicast_nodes_count, multicast_nodes);

		network_path path;
		int cost = algorithm_func(network, multicast_nodes, delta, path);

		if (cost < 999999999) {
			network_helper::increase_costs_due_to_transmission_load(network,
				path, load);

		} else {
			break;

		}

		trees++;
	}

	return trees;
}

int main() {
	int delta = 2000000;
	int load = 10;

	size_t networks_count = 2;
	size_t repetitions_per_network = 1;
	size_t nodes_count = 10000;
	size_t edges_count = 20576;

	std::ifstream input;
	input.open("networks.txt");

	results_container results;

	for (size_t i = 0; i < networks_count; ++i) {
		network_t* network = new network_array_t(nodes_count);
		network_helper::load_network_from_file(input, edges_count, *network);

		for (size_t j = 0; j < repetitions_per_network; ++j) {

			{
				network_t* copy = network->make_clone();

				size_t trees = get_trees_count(*copy, delta, load,
					grouping_func_t(&grouping::high_degree),
					algorithm_func_t(&PPH_HM));

				results.insert_result("KPP", i, trees);

				network->dispose_clone(copy);
			}

		}

		delete network;
	}

	results.print();

	return 0;
}
