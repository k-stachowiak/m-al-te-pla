#include "network_helper.h"

#include <string>
#include <sstream>

/* private */
template<class T>
T str_to_(const std::string& str) {
	std::stringstream ss(str);
	T result;
	ss >> result;
	return result;
}

/* private */
template<class T>
T draw_from_stream(std::ifstream& stream) {
	std::string word;
	stream >> word;
	return str_to_<T> (word);
}

namespace network_helper {

void dfs_mark_visits(size_t current_node, network_abstraction& network,
	std::vector<bool>& visit_map) {

	for (size_t i = 0; i < network.get_nodes_count(); i++) {
		if (network.has_link(current_node, i) && !visit_map[i]) {

			visit_map[i] = true;
			dfs_mark_visits(i, network, visit_map);

		}
	}

}

bool is_connected(network_abstraction& network) {
	std::vector<bool> visits_map(network.get_nodes_count(), false);
	visits_map[0] = true;

	dfs_mark_visits(0, network, visits_map);

	for (std::vector<bool>::iterator i = visits_map.begin(); i
		!= visits_map.end(); ++i) {
		if (!(*i)) {
			return false;
		}
	}

	return true;
}

/* private */network_abstraction* current_network = NULL;
/* private */int current_load = -1;

/* private */void int_pair_presence_delegate(const int_pair_presence& ipp) {
	if (!ipp.second) {
		return;
	}

	size_t from = ipp.first.first, to = ipp.first.second;

	// Tu można by sprawdzać "has_link", ale wiemy, że to łącze w grafie
	// występuje, zatem sprawdzamy jedynie, czy jego "koszt" jest jeszcze
	// dodatni. "has_link" sprawdza też, czy hash mapa zawiera dany klucz.
	double cost = current_network->get_cost(from, to);
	if (cost > 0.0) {

		int band = int(10000.0 / cost);
		band = band - current_load;
		if (band <= 0) {
			current_network->set_cost(from, to, 0.0);

		} else {
			current_network->set_cost(from, to, int(10000.0 / band));

		}

	}
}

void increase_costs_due_to_transmission_load(network_abstraction& network,
	network_path& path, int load) {

	current_network = &network;
	current_load = load;
	path.for_each_link(int_pair_presence_delegate);
}

void load_network_from_file(std::ifstream& input, size_t num_edges,
	network_abstraction& network) {

	size_t id = draw_from_stream<size_t>(input);

	size_t id_edge, begin, end, cost;
	float delay;
	for (size_t edge = 0; edge < num_edges; ++edge) {

		id_edge = draw_from_stream<size_t>(input);
		begin = draw_from_stream<size_t>(input);
		end = draw_from_stream<size_t>(input);
		cost = draw_from_stream<size_t>(input);
		delay = draw_from_stream<float>(input);

		network.set_link_desc(begin, end, link_desc((int) cost, (int) delay));
	}
}

}
