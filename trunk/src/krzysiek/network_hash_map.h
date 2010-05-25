#ifndef NETWORK_HASH_MAP_H_
#define NETWORK_HASH_MAP_H_

#include "network_abstraction.h"

typedef hash_map<int_pair, link_desc, int_pair_hash, int_pair_compare>
	graph_hash_map;

class network_hash_map_t: public network_t {
public:
	network_hash_map_t() {
	}

	virtual ~network_hash_map_t() {
	}

	network_t* make_clone() {
		network_t* result = new network_hash_map_t;

		graph_hash_map::iterator i;
		for (i = graph.begin(); i != graph.end(); ++i) {
			result->set_link_desc(i->first.first, i->first.second, i->second);
		}

		return result;
	}

	void dispose_clone(network_t* network) {
		delete dynamic_cast<network_hash_map_t*> (network);
	}

	void set_cost(uint32_t from, uint32_t to, double new_cost) {
		if (has_link(int_pair(from, to))) {
			graph[int_pair(from, to)].cost = new_cost;
			graph[int_pair(to, from)].cost = new_cost;

		} else {
			graph[int_pair(from, to)] = link_desc(new_cost,
				link_desc::default_delay);
			graph[int_pair(to, from)] = link_desc(new_cost,
				link_desc::default_delay);

		}

		// Unieważniamy liczbę węzłów na wypadek gdyby tutaj uległa zmianie
		nodes_count = -1;
	}

	void set_delay(uint32_t from, uint32_t to, double new_delay) {
		if (has_link(int_pair(from, to))) {
			graph[int_pair(from, to)].delay = new_delay;
			graph[int_pair(to, from)].delay = new_delay;

		} else {
			graph[int_pair(from, to)] = link_desc(link_desc::default_cost,
				new_delay);
			graph[int_pair(to, from)] = link_desc(link_desc::default_cost,
				new_delay);

		}

		// Unieważniamy liczbę węzłów na wypadek gdyby tutaj uległa zmianie
		nodes_count = -1;
	}

	void set_link_desc(uint32_t from, uint32_t to, const link_desc& ld) {
		graph[int_pair(from, to)] = ld;
		graph[int_pair(to, from)] = ld;

		// Unieważniamy liczbę węzłów na wypadek gdyby tutaj uległa zmianie
		nodes_count = -1;

	}

	double get_cost(uint32_t from, uint32_t to) {
		if (has_link(int_pair(from, to))) {
			return graph[int_pair(from, to)].cost;

		} else if (has_link(int_pair(to, from))) {
			return graph[int_pair(to, from)].cost;

		} else {
			throw;
		}
	}

	double get_delay(uint32_t from, uint32_t to) {
		if (has_link(int_pair(from, to))) {
			return graph[int_pair(from, to)].delay;

		} else if (has_link(int_pair(from, to))) {
			return graph[int_pair(to, from)].delay;

		} else {
			throw;
		}
	}

	link_desc get_link_desc(uint32_t from, uint32_t to) {
		if (has_link(int_pair(from, to))) {
			return graph[int_pair(from, to)];

		} else {
			throw;
		}
	}

	uint32_t get_nodes_count() {
		if (nodes_count == -1) {
			std::set<size_t> unique_ids;
			graph_hash_map::iterator i;
			for (i = graph.begin(); i != graph.end(); ++i) {
				unique_ids.insert(i->first.first);
				unique_ids.insert(i->first.second);
			}

			nodes_count = unique_ids.size();
		}

		return (uint32_t) nodes_count;
	}

	void get_nodes_list(std::vector<size_t>& result, size_t limit) const {

		std::set<size_t> unique_ids;
		graph_hash_map::const_iterator i;
		for (i = graph.begin(); i != graph.end(); ++i) {
			unique_ids.insert(i->first.first);
			unique_ids.insert(i->first.second);
		}

		result.clear();
		result.reserve(unique_ids.size());
		std::set<size_t>::iterator j;
		for (j = unique_ids.begin(); j != unique_ids.end(); ++j) {
			if(limit == 0) {
				return;
			}

			result.push_back(*j);

			--limit;
		}
	}

	bool has_link(const int_pair& pair) {
		graph_hash_map::iterator i = graph.find(pair);
		if (i == graph.end()) {
			return false;
		}

		if (!(*i).second.is_valid()) {
			return false;
		}

		return true;
	}

	bool has_link(uint32_t from, uint32_t to) {
		return has_link(int_pair(from, to));
	}

private:
	graph_hash_map graph;
};

#endif /* NETWORK_HASH_MAP_H_ */
