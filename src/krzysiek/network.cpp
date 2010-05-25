#include "network.h"

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
