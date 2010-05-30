#ifndef NETWORK_BOOST_H_
#define NETWORK_BOOST_H_

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

#include "network.h"

class network_boost_t: public network_t {
private:
	struct edge_s {
		double cost;
		double delay;
	};

	struct vertex_s {
		double x;
		double y;
	};

	typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
		vertex_s, edge_s, boost::no_property, boost::vecS> adjacency_list_t;

	typedef boost::graph_traits<adjacency_list_t>::edge_descriptor edge_desc;
	typedef boost::graph_traits<adjacency_list_t>::vertex_descriptor
		vertex_desc;

public:
	network_boost_t() {
	}

	virtual ~network_boost_t() {
	}

	virtual network_t* make_clone() {
		network_boost_t* result = new network_boost_t();
		result->adjacency_list = adjacency_list;
		return result;
	}

	virtual void dispose_clone(network_t* network) {
		delete dynamic_cast<network_boost_t*> (network);
	}

	virtual void set_cost(uint32_t from, uint32_t to, double new_cost) {
		edge_desc edge = get_edge_descriptor(from, to);
		adjacency_list[edge].cost = new_cost;
	}

	virtual void set_delay(uint32_t from, uint32_t to, double new_delay) {
		edge_desc edge = get_edge_descriptor(from, to);
		adjacency_list[edge].delay = new_delay;
	}

	virtual void set_link_desc(uint32_t from, uint32_t to, const link_desc& ld) {
		edge_desc edge = get_edge_descriptor(from, to);
		adjacency_list[edge].cost = ld.cost;
		adjacency_list[edge].delay = ld.delay;
	}

	virtual double get_cost(uint32_t from, uint32_t to) {
		edge_desc edge = get_edge_descriptor(from, to);
		return adjacency_list[edge].cost;
	}

	virtual double get_delay(uint32_t from, uint32_t to) {
		edge_desc edge = get_edge_descriptor(from, to);
		return adjacency_list[edge].delay;
	}

	virtual link_desc get_link_desc(uint32_t from, uint32_t to) {
		edge_desc edge = get_edge_descriptor(from, to);
		return link_desc(adjacency_list[edge].cost, adjacency_list[edge].delay);
	}

	virtual uint32_t get_nodes_count() {
		return boost::num_vertices(adjacency_list);
	}

	virtual bool has_link(const int_pair& pair) {
		return has_link(pair.first, pair.second);
	}

	virtual bool has_link(uint32_t from, uint32_t to) {
		return have_edge(from, to);
	}

private:
	void add_vertices_if_non_existent(uint32_t from, uint32_t to) {
		if (vertex_descriptors.size() <= from) {
			vertex_descriptors.resize(from + 1, NULL);
		}

		if (vertex_descriptors[from] == NULL) {
			vertex_s new_vertex_property = { 0.0, 0.0 };
			vertex_descriptors[from] = new vertex_desc;
			*(vertex_descriptors[from]) = boost::add_vertex(new_vertex_property,
				adjacency_list);
		}

		if (vertex_descriptors.size() <= to) {
			vertex_descriptors.resize(to + 1, NULL);
		}

		if (vertex_descriptors[to] == NULL) {
			vertex_s new_vertex_property = { 0.0, 0.0 };
			vertex_descriptors[to] = new vertex_desc;
			*(vertex_descriptors[to]) = boost::add_vertex(new_vertex_property,
				adjacency_list);
		}
	}

	bool have_edge(uint32_t from, uint32_t to) {
		add_vertices_if_non_existent(from, to);

		boost::graph_traits<adjacency_list_t>::adjacency_iterator i, j;
		for (boost::tie(i, j) = boost::adjacent_vertices(
			*(vertex_descriptors[from]), adjacency_list); i != j; ++i) {

			if (*i == *(vertex_descriptors[to])) {
				return true;
			}
		}

		return false;
	}

	edge_desc get_edge_descriptor(uint32_t from, uint32_t to) {
		add_vertices_if_non_existent(from, to);

		if (!have_edge(from, to)) {
			edge_s new_edge_property = { link_desc::default_cost,
					link_desc::default_delay };

			boost::add_edge(*(vertex_descriptors[from]), *(vertex_descriptors)[to],
				new_edge_property, adjacency_list);
		}

		edge_desc edge;
		bool exists;

		tie(edge, exists) = boost::edge(*(vertex_descriptors[from]),
			*(vertex_descriptors[to]), adjacency_list);

		if (!exists) {
			throw;
		}

		return edge;
	}

	adjacency_list_t adjacency_list;
	std::vector<vertex_desc*> vertex_descriptors;
};

#endif /* NETWORK_BOOST_H_ */
