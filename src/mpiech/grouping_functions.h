#ifndef GROUPING_FUNCTIONS_H_
#define GROUPING_FUNCTIONS_H_

#include <algorithm>

namespace grouping {

int high_degree(network_abstraction& network, int multicast_nodes, std::vector<size_t>& result) {

	size_t nodes_count = network.get_nodes_count();
	std::vector<size_t> distribution(nodes_count, 0);

	for(size_t from = 0; from < nodes_count; ++from) {
		// Zał. że graf jest nieskierowany!
		for(size_t to = (from+1); to < nodes_count; ++to) {
			if(network.has_link(from, to)) {
				++(distribution[from]);
			}
		}
	}

	// To nie jest najlepsza decyzja projektowa, bo nie potrzeba sortować
	// całej tablicy. W najgorszym przypadku zbedny narzut może nie być
	// rozbrajająco wielki, ale jeśli tu się pojawi wąskie gardło to ten "sort"
	// jest do odstrzału na rzecz jakiegoś sprytnego wybierania.
	std::sort(distribution.begin(), distribution.end());

	result.clear();
	for(size_t i = 0; i < (multicast_nodes + 1); ++i) {
		result.push_back(distribution[i]);
	}

}

}

#endif /* GROUPING_FUNCTIONS_H_ */
