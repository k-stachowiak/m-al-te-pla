#ifndef NETWORK_HELPER_H
#define NETWORK_HELPER_H

#include <fstream>

#include "network.h"

namespace network_helper {

void dfs_mark_visits(size_t current_node, network_t& network,
	std::vector<bool>& visit_map);

bool is_connected(network_t& network);

void increase_costs_due_to_transmission_load(network_t& network,
	network_path& path, int load);

void load_network_from_file(std::ifstream& input, size_t num_edges,
	network_t& network);

}

#endif /* NETWORK_HELPER_H */
