#include <fstream>

#include "krzysiek/results_container.h"
#include "krzysiek/network_abstraction.h"
#include "krzysiek/network_helper.h"

#include "mpiech/kpp_hash_map.h"
#include "mpiech/pph_hash_map.h"
#include "mpiech/grouping_functions.h"

size_t get_trees_pph(network_abstraction& network, int multicast_nodes_count,
	std::vector<size_t>& multicast_nodes, int delta, int load) {

	size_t trees = 0;
	while (network_helper::is_connected(network)) {

		grouping::high_degree(network, multicast_nodes_count, multicast_nodes);

		network_path path;
		int cost = PPH_HM(network, multicast_nodes, network.get_nodes_count(),
			multicast_nodes.size(), delta, path);

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
	size_t multicast_nodes_count = 80;

	int delta = 2000000;
	int load = 10;

	std::vector<size_t> multicast_nodes;

	for (size_t i = 0; i < networks_count; ++i) {
		network_abstraction network;
		network_helper::load_network_from_file(input, edges_count, network);

		for (size_t j = 0; j < repetitions_per_network; ++j) {
			// Uwaga, konstruktor kopiujący został zdefiniowany!
			network_abstraction network_copy;
			network_copy = network;

			size_t trees = get_trees_pph(network_copy, multicast_nodes_count,
				multicast_nodes, delta, load);

			results.insert_result("PPH", i, trees);
		}
	}

	return 0;
}
