#ifndef NETWORK_HELPER_H
#define NETWORK_HELPER_H

#include <fstream>

#include "network_abstraction.h"

namespace network_helper {

void dfs_mark_visits(size_t current_node, network_abstraction& network,
	std::vector<bool>& visit_map);

bool is_connected(network_abstraction& network);

void increase_costs_due_to_transmission_load(network_abstraction& network,
	network_path& path, int load);

void load_network_from_file(std::ifstream& input, size_t num_edges,
	network_abstraction& network);

}

#endif /* NETWORK_HELPER_H */
