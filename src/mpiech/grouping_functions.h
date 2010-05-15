#ifndef GROUPING_FUNCTIONS_H_
#define GROUPING_FUNCTIONS_H_

#include <algorithm>

namespace grouping {

class distribution_record {
public:
	distribution_record(size_t new_graph_id) :
		node_id(new_graph_id), neighbours_count(0) {
	}

	size_t get_node_id() const {
		return node_id;
	}

	size_t get_neighbours_count() const {
		return neighbours_count;
	}

	void add_neighbour() {
		++neighbours_count;
	}

	bool operator<(const distribution_record& other) const {
		return neighbours_count < other.neighbours_count;
	}

private:
	size_t node_id;
	size_t neighbours_count;
};

void high_degree(network_abstraction& network, int multicast_nodes,
	std::vector<size_t>& result) {

	size_t nodes_count = network.get_nodes_count();
	std::vector<distribution_record> distribution;

	for (size_t from = 0; from < nodes_count; ++from) {
		distribution_record new_record(from);
		// Zał. że graf jest nieskierowany!
		for (size_t to = (from + 1); to < nodes_count; ++to) {
			if (network.has_link(from, to)) {
				new_record.add_neighbour();
			}
		}
		distribution.push_back(new_record);
	}

	// To nie jest najlepsza decyzja projektowa, bo nie potrzeba sortować
	// całej tablicy. W najgorszym przypadku zbedny narzut może nie być
	// rozbrajająco wielki, ale jeśli tu się pojawi wąskie gardło to ten "sort"
	// jest do odstrzału na rzecz jakiegoś sprytnego wybierania.
	std::sort(distribution.begin(), distribution.end());

	result.clear();
	for (int i = 0; i < (multicast_nodes + 1); ++i) {
		result.push_back(distribution[i].get_node_id());
	}

}

}

#endif /* GROUPING_FUNCTIONS_H_ */
