#include "../krzysiek/network_abstraction.h"

//------------------------- funkcja wyznacza min. drzewo algorytmem Prima-Dijkstry -----------------

int PrimPPH_HM(network_abstraction& network, int *edge1, int *edge2, int n, int s, int nt) {

	int totalcost, totalcount, min, u, i, j;
	int *nearest;
	nearest = new int[n];
	for (i = 0; i < n; i++) {
		nearest[i] = 0;
	}
	nearest[s] = -1;
	int *dist;
	dist = new int[n];
	for (i = 0; i < n; i++) {
		if(network.has_link(s, i)) {
			dist[i] = network.get_cost(s, i);

		} else {
			dist[i] = 0; // Czy -1 ?

		}
	}

	totalcount = 0;
	totalcost = 0;

	while (totalcount < (nt - 1)) {

		min = 999999999;

		for (i = 0; i < n; i++) {
			if (nearest[i] != -1) {
				if (dist[i] < min && dist[i] > 0) {
					min = dist[i];
					u = i;
				}
			}
		}

		totalcost = totalcost + dist[u];
		edge1[totalcount] = nearest[u];
		edge2[totalcount] = u;

		nearest[u] = -1;

		for (i = 0; i < n; i++) {
			if (nearest[i] != -1) { // zeby nie bylo cyklu
				if(network.has_link(u, i) && (dist[i] > network.get_cost(u, i) || dist[i] == 0)) {
					dist[i] = network.get_cost(u, i);
					nearest[i] = u;
				}

			}
		}

		totalcount++;

	}

	delete[] nearest;
	delete[] dist;

	return totalcost;
}

//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------


int PPH_HM(network_abstraction& network, std::vector<size_t>& M, int delta, network_path& path) {

	int n = network.get_nodes_count();
	int m = M.size();

	int s = M[0];
	int i, j, k, v, vv, l, p, nt, last_seed, total = 0, sim, temp1, temp2,
		hop_diam, length_diam, licz, clust;
	int mincost, pathdelay, all, av_path, constr, cst, seria, costPPH;
	int sredni_koszt, exist, cut;
	float calkowity_koszt, pu, odchylenie;
	long start, stop;
	float cc, neighbour;

	int *edge1;
	edge1 = new int[n - 1];
	int *edge2;
	edge2 = new int[n - 1];
	int *links;
	links = new int[n]; // ile laczy od kazdego wezla

	for (j = 0; j < n - 1; j++)
		edge1[j] = -1;
	for (j = 0; j < n - 1; j++)
		edge2[j] = -1;
	for (i = 0; i < n; i++)
		links[i] = 0;

	av_path = 0;
	l = 1;
	p = 0;
	pathdelay = 0;
	nt = 0;
	v = 0;
	vv = 0;
	constr = 0;

	m++;

	for (i = 0; i < n; i++) { //jesli graf niespojny to liczy ile wierzch wchodzi do drzewa
		k = 0;
		for (j = 0; j < n; j++) {
			if (network.has_link(i, j)) {
				k++;
			}
		}
		if (k > 0)
			nt++;
	}

	costPPH = PrimPPH_HM(network, edge1, edge2, n, s, nt);

	v = 0;
	for (i = 0; i < nt - 1; i++) { // sprawdza czy wszystkie wezly multicast sa w drzewie
		for (k = 1; k < m; k++) {
			if (M[k] == edge2[i]) {
				v++;
			}
		}
	}

	if ((vv != 0) || (v < m - 1)) {
		delete[] edge1;
		delete[] edge2;
		delete[] links;

		return 999999999;

	} else {

		do {
			cut = 0;
			for (i = 0; i < n; i++)
				links[i] = 0;

			for (i = 0; i < nt - 1; i++) {
				if (edge1[i] != -1) {
					links[edge1[i]]++;
					links[edge2[i]]++;
				}

			}

			for (i = 0; i < n - 1; i++) {
				if (links[i] == 1) {
					exist = 1;
					for (j = 0; j < m; j++) {
						if (M[j] == i) {
							exist = 0;
						}
					}
					if (exist == 1) {
						cut = 1;
						for (k = 0; k < nt - 1; k++) {
							if (edge1[k] == i) {
								edge1[k] = -1;
								edge2[k] = -1;
							}
						}
						for (k = 0; k < nt - 1; k++) {
							if (edge2[k] == i) {
								edge1[k] = -1;
								edge2[k] = -1;
							}
						}
					}
				}
			}
		} while (cut == 1);

		costPPH = 0;
		for (k = 0; k < nt - 1; k++) {
			if (edge1[k] != -1) {
				double c;

				if(network.has_link(edge1[k], edge2[k])) {
					c = network.get_cost(edge1[k], edge2[k]);

				} else {
					c = 0;

				}

				costPPH += c;
				path.add_link(edge1[k], edge2[k]);
			}
		}

		delete[] edge1;
		delete[] edge2;
		delete[] links;

		return costPPH;

	}

}
