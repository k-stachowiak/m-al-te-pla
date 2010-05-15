#ifndef RESULTS_CONTAINER_H
#define RESULTS_CONTAINER_H

#include <stdint.h>
#include <vector>
#include <map>
#include <string>

class results_container {
public:
	typedef std::vector<uint32_t> results_list_t;
	typedef std::map<size_t, results_list_t*> graph_results_t;
	typedef std::map<std::string, graph_results_t*> algorithm_results_t;

	~results_container() {
		algorithm_results_t::iterator i;
		for(i = algorithm_results.begin(); i != algorithm_results.end(); ++i) {

			graph_results_t* graph_results = i->second;
			graph_results_t::iterator j;
			for(j = graph_results->begin(); j != graph_results->end(); ++j) {
				results_list_t* results_list = j->second;
				delete results_list;

			}
			delete graph_results;

		}
	}

	void insert_result(std::string algorithm_name, size_t graph_id,
		uint32_t result) {

		algorithm_results_t::iterator i =
			algorithm_results.find(algorithm_name);

		if (i == algorithm_results.end()) {
			algorithm_results[algorithm_name] = new graph_results_t;
		}

		graph_results_t* graph_result = algorithm_results[algorithm_name];

		graph_results_t::iterator j = graph_result->find(graph_id);
		if (j == graph_result->end()) {
			(*graph_result)[graph_id] = new results_list_t;
		}

		(*graph_result)[graph_id]->push_back(result);
	}

private:
	algorithm_results_t algorithm_results;
};

#endif /* RESULTS_CONTAINER_H */
