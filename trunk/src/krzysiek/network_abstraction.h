#ifndef NETWORK_ABSTRACTION_H
#define NETWORK_ABSTRACTION_H

#include <utility>
#include <hash_map>
#include <stdint.h>

using namespace __gnu_cxx;

typedef std::pair<uint32_t, uint32_t> int_pair;

// Deskryptor łącza - tu można dodawać kryteria
class link_desc {
public:
	link_desc();
	link_desc(double new_cost, double new_delay);
	bool is_valid();

	static double default_cost;
	static double default_delay;

	double cost;
	double delay;
};

// Generator hashu - tu można pokombinować w razie poważnego spowolnienia przeszukiwania
struct int_pair_hash {
	size_t operator()(const int_pair& pair) const {
		return pair.first ^ pair.second;
	}
};

// Komparator, uwaga, zakładamy, ze graf jest nieskierowany
struct int_pair_compare {
	bool operator()(const int_pair& a, const int_pair& b) const {
		return (a.first == b.first && a.second == b.second) || (a.first
			== b.second && a.second == b.first);
	}
};

typedef hash_map<int_pair, bool, int_pair_hash, int_pair_compare> path_hash_map;
typedef std::pair<int_pair, bool> int_pair_presence;
typedef void(*int_pair_presence_functor)(const int_pair_presence&);

class network_path {
public:
	void add_link(size_t from, size_t to);
	bool has_link(const int_pair& pair);
	void for_each_link(int_pair_presence_functor func);

private:
	path_hash_map path;
};

typedef hash_map<int_pair, link_desc, int_pair_hash, int_pair_compare>
	graph_hash_map;

class network_abstraction {
public:
	network_abstraction();
	network_abstraction(const network_abstraction& copy);
	virtual ~network_abstraction();

	void set_cost(uint32_t from, uint32_t to, double new_cost);
	void set_delay(uint32_t from, uint32_t to, double new_delay);

	void set_link_desc(uint32_t from, uint32_t to, const link_desc& ld);

	double get_cost(uint32_t from, uint32_t to);
	double get_delay(uint32_t from, uint32_t to);

	link_desc get_link_desc(uint32_t from, uint32_t to);

	uint32_t get_nodes_count();

	bool has_link(const int_pair& pair);
	bool has_link(uint32_t from, uint32_t to);

private:
	int64_t nodes_count;
	graph_hash_map graph;
};

#endif /* NETWORK_ABSTRACTION_H */
