#ifndef NETWORK_ARRAY_H_
#define NETWORK_ARRAY_H_

#include "network.h"

class network_array_t: public network_t {
public:
	network_array_t(uint32_t new_nodes_count) {
		nodes_count = new_nodes_count;
		array = new link_desc*[nodes_count];
		for (int32_t i = 0; i < nodes_count; ++i) {
			array[i] = new link_desc[nodes_count];
		}
	}

	virtual ~network_array_t() {
		for (int32_t i = 0; i < nodes_count; ++i) {
			delete[] array[i];
		}

		delete[] array;
	}

	network_t* make_clone() {
		network_t* result = new network_array_t(nodes_count);

		for (int32_t i = 0; i < nodes_count; ++i) {
			for (int32_t j = 0; j < nodes_count; ++j) {
				result->set_link_desc(i, j, array[i][j]);
			}
		}

		return result;
	}

	void dispose_clone(network_t* network) {
		delete dynamic_cast<network_array_t*> (network);
	}

	void set_cost(uint32_t from, uint32_t to, double new_cost) {
		array[from][to].cost = new_cost;
		array[to][from].cost = new_cost;
	}

	void set_delay(uint32_t from, uint32_t to, double new_delay) {
		array[from][to].delay = new_delay;
		array[to][from].delay = new_delay;
	}

	void set_link_desc(uint32_t from, uint32_t to, const link_desc& ld) {
		array[from][to] = ld;
		array[to][from] = ld;
	}

	double get_cost(uint32_t from, uint32_t to) {
		return array[from][to].cost;
	}

	double get_delay(uint32_t from, uint32_t to) {
		return array[from][to].delay;
	}

	link_desc get_link_desc(uint32_t from, uint32_t to) {
		return array[from][to];
	}

	uint32_t get_nodes_count() {
		return nodes_count;
	}

	void get_nodes_list(std::vector<size_t>& result, size_t limit) const {
		std::set<uint32_t> valid_ids;

		for (int32_t i = 0; i < nodes_count; ++i) {
			bool found_valid = false;
			for (int32_t j = 0; j < nodes_count; ++j) {
				if (!array[i][j].is_valid()) {
					continue;
				}

				valid_ids.insert(j);
				found_valid = true;
			}

			// Opóźniam wstawianie indeksu i, ponieważ takie wstawianie może być kosztowne.
			// W związku z tym w wewnętrznej pętli zapalam jedynie flagę która wskazuje,
			// że pojawiła się sytuacja, w której z "i" lub do "i" wychodzi/wchodzi
			// aktywna, czy też nieusunięta krawędź.
			if (found_valid) {
				valid_ids.insert(i);
			}
		}

		result.clear();
		for (std::set<uint32_t>::iterator i = valid_ids.begin(); i
			!= valid_ids.end(); ++i) {

			if (limit == 0) {
				return;
			}

			result.push_back(*i);

			--limit;
		}
	}

	bool has_link(const int_pair& pair) {
		return has_link(pair.first, pair.second);
	}

	bool has_link(uint32_t from, uint32_t to) {
		return array[from][to].is_valid() || array[to][from].is_valid();
	}

private:
	link_desc** array;
};

#endif /* NETWORK_ARRAY_H_ */
