#include <set>

#include "network_abstraction.h"

link_desc::link_desc() :
	cost(default_cost), delay(default_delay) {
}

link_desc::link_desc(double new_cost, double new_delay) :
	cost(new_cost), delay(new_delay) {
}

bool link_desc::is_valid() {
	return cost > 0.0;
}

double link_desc::default_cost = -1;
double link_desc::default_delay = -1;

void network_path::add_link(size_t from, size_t to) {
	path[int_pair(from, to)] = true;
	path[int_pair(to, from)] = true;
}

bool network_path::has_link(const int_pair& pair) {
	path_hash_map::iterator i;

	i = path.find(pair);
	if (i != path.end()) {
		return path[pair];
	}

	// Opóźniona alokacja pary odwróconej. Możliwe, ze takowe będą się
	// pojawiac rzadziej i wtedy powyższy warunek stosunkowo często
	// "wyskoczy".
	int_pair reverse_pair(pair.second, pair.first);

	i = path.find(reverse_pair);
	if (i != path.end()) {
		return path[reverse_pair];
	}

	return false;
}

void network_path::for_each_link(int_pair_presence_functor func) {
	std::for_each(path.begin(), path.end(), func);
}

network_abstraction::network_abstraction() :
	nodes_count(-1) {
}

network_abstraction::network_abstraction(const network_abstraction& copy) :
	nodes_count(-1) {

	graph_hash_map::const_iterator i;
	for (i = copy.graph.begin(); i != copy.graph.end(); ++i) {
		set_link_desc(i->first.first, i->first.second, i->second);
	}
}

network_abstraction::~network_abstraction() {
}

void network_abstraction::set_cost(uint32_t from, uint32_t to, double new_cost) {
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

void network_abstraction::set_delay(uint32_t from, uint32_t to,
	double new_delay) {
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

void network_abstraction::set_link_desc(uint32_t from, uint32_t to,
	const link_desc& ld) {
	graph[int_pair(from, to)] = ld;
	graph[int_pair(to, from)] = ld;

	// Unieważniamy liczbę węzłów na wypadek gdyby tutaj uległa zmianie
	nodes_count = -1;

}

double network_abstraction::get_cost(uint32_t from, uint32_t to) {
	if (has_link(int_pair(from, to))) {
		return graph[int_pair(from, to)].cost;

	} else if (has_link(int_pair(to, from))) {
		return graph[int_pair(to, from)].cost;

	} else {
		throw;
	}
}

double network_abstraction::get_delay(uint32_t from, uint32_t to) {
	if (has_link(int_pair(from, to))) {
		return graph[int_pair(from, to)].delay;

	} else if (has_link(int_pair(from, to))) {
		return graph[int_pair(to, from)].delay;

	} else {
		throw;
	}
}

link_desc network_abstraction::get_link_desc(uint32_t from, uint32_t to) {
	if (has_link(int_pair(from, to))) {
		return graph[int_pair(from, to)];

	} else {
		throw;
	}
}

uint32_t network_abstraction::get_nodes_count() {
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

bool network_abstraction::has_link(const int_pair& pair) {
	graph_hash_map::iterator i = graph.find(pair);
	if (i == graph.end()) {
		return false;
	}

	if (!(*i).second.is_valid()) {
		return false;
	}

	return true;
}

bool network_abstraction::has_link(uint32_t from, uint32_t to) {
	return has_link(int_pair(from, to));
}
