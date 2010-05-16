#include <iostream>
#include <fstream>

#include <FastDelegate.h>

#include "krzysiek/results_container.h"
#include "krzysiek/network_abstraction.h"
#include "krzysiek/network_helper.h"

typedef fastdelegate::FastDelegate3<network_abstraction&, int, std::vector<
	size_t>& > grouping_func_t;

typedef fastdelegate::FastDelegate4<network_abstraction&, std::vector<size_t>&,
	int, network_path&, int> algorithm_func_t;

#include "mpiech/kpp_hash_map.h"
#include "mpiech/pph_hash_map.h"
#include "mpiech/grouping_functions.h"

size_t get_trees_count(network_abstraction& network, int delta, int load,
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
	results_container results;

	std::ifstream input;
	input.open("networks.txt");

	size_t networks_count = 5;
	size_t repetitions_per_network = 10;
	size_t edges_count = 20576;

	int delta = 2000000;
	int load = 10;

	for (size_t i = 0; i < networks_count; ++i) {
		network_abstraction network;
		network_helper::load_network_from_file(input, edges_count, network);

		for (size_t j = 0; j < repetitions_per_network; ++j) {
			network_abstraction network_copy(network);
			size_t trees = get_trees_count(network_copy, delta, load,
				grouping_func_t(&grouping::high_degree),
				algorithm_func_t(&PPH_HM));
			results.insert_result("PPH", i, trees);
		}
	}

	return 0;
}
