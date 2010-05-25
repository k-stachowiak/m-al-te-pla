#include "../krzysiek/network.h"

//------------------------- funkcja wyznacza min. sciezke algorytmem Dijkstry ----------------------

int DijkstraKPP_HM(int n, int *Path, int s, int t, int &pathdelay,
	network_t& network) {

	int i, j, v, x, u, endcost, recent, newcost, mincost = 999999999;
	int k = 1;

	int *dist;
	dist = new int[n];
	for (i = 0; i < n; i++) {
		dist[i] = 999999999;
	}
	int *final;
	final = new int[n];
	for (i = 0; i < n; i++) {
		final[i] = 0;
	}
	int *pred;
	pred = new int[n];
	for (i = 0; i < n; i++) {
		pred[i] = -1;
	}
	for (i = 1; i < n; i++) {
		Path[i] = -1;
	}
	dist[s] = 0;
	final[s] = 1;
	recent = s;
	Path[0] = s;

	while (final[t] == 0) {

		for (i = 0; i < n; i++) {
			if (network.has_link(recent, i)) {
				v = i; // Po co?
				if (final[v] == 0) {
					newcost = dist[recent] + network.get_cost(recent, v);
					if (newcost < dist[v]) {
						dist[v] = newcost;
						pred[v] = recent;
					}
				}
			}
		}

		mincost = 999999999;
		for (j = 0; j < n; j++) {
			if ((final[j] == 0) && (dist[j] < mincost)) {
				x = j;
				mincost = dist[j];
			}
		}

		final[x] = 1;
		recent = x;

	}

	v = t;
	k = 0;
	pathdelay = 0;

	while (v != s) { // petla pozwala odtworzyc droge (wezly) o najmniejszym koszcie (opoznieniu)
		k++;
		Path[k - 1] = v;

		if (network.has_link(v, pred[v])) {
			pathdelay += network.get_delay(v, pred[v]);
		}

		v = pred[v];
	}

	k++;
	Path[k - 1] = s;

	endcost = dist[t];

	delete[] pred;
	delete[] final;
	delete[] dist;

	return endcost;
}

//--------------------------------------------------------------------------------------------------

//------------------------- funkcja wyznacza min. drzewo algorytmem Prima-Dijkstry -----------------

int PrimKPP_HM(int **G, std::vector<size_t>& M, int *edge1, int *edge2, int m, int s) {

	int totalcost, totalcount, min, u, i, j;
	int *nearest;
	nearest = new int[m];
	for (i = 0; i < m; i++) {
		nearest[i] = s;
	}
	nearest[s] = -1;
	int *dist;
	dist = new int[m];
	for (i = 0; i < m; i++) {
		dist[i] = G[s][i];
	}

	totalcount = 0;
	totalcost = 0;

	while (totalcount < (m - 1)) {
		min = 999999999;

		for (i = 0; i < m; i++) { //znajdz najtanszy koszt do sasiada
			if (nearest[i] != -1) {
				if (dist[i] < min && dist[i] > 0) {
					min = dist[i];
					u = i;
				}
			}
		}

		totalcost = totalcost + dist[u];
		if (M[nearest[u]] < M[u]) {
			edge1[totalcount] = M[nearest[u]];
			edge2[totalcount] = M[u];
		} else {
			edge1[totalcount] = M[u];
			edge2[totalcount] = M[nearest[u]];
		}

		nearest[u] = -1;

		for (i = 0; i < m; i++) {
			if (nearest[i] != -1) { // zeby nie bylo cyklu
				if ((dist[i] > G[u][i] || dist[i] == 0) && G[u][i] != 0) {
					dist[i] = G[u][i];
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

//------------------------- funkcja wyznacza min. drzewo algorytmem Prima-Dijkstry (2) -------------

int PrimFinalKPP_HM(int **T, int *edge3, int *edge4, int n, int s, int nt) {

	int totalcost, totalcount, min, u, i, j;
	int *nearest;
	nearest = new int[n];
	for (i = 0; i < n; i++) {
		nearest[i] = s;
	}
	nearest[s] = -1;
	int *dist;
	dist = new int[n];
	for (i = 0; i < n; i++) {
		dist[i] = T[s][i];
	}

	totalcount = 0;
	totalcost = 0;

	while (totalcount < (nt - 1)) {
		min = 999999999;

		for (i = 0; i < n; i++) { //znajdz najtanszy koszt do sasiada
			if (nearest[i] != -1) {
				if (dist[i] < min && dist[i] > 0) {
					min = dist[i];
					u = i;
				}
			}
		}

		totalcost = totalcost + dist[u];
		edge3[totalcount] = nearest[u];
		edge4[totalcount] = u;

		nearest[u] = -1;

		for (i = 0; i < n; i++) {
			if (nearest[i] != -1) { // zeby nie bylo cyklu
				if ((dist[i] > T[u][i] || dist[i] == 0) && T[u][i] != 0) {
					dist[i] = T[u][i];
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


int KPP_HM(network_t& network, std::vector<size_t>& M, int delta,
	network_path& path) {

	int n = network.get_nodes_count();
	int m = M.size();

	int i, j, k, v, l, p, nt, last_seed, total = 0, sim, temp1, temp2;
	int all, av_path, niespojny, Gsum, seria;
	int s = M[0];
	int mincost, pathdelay, costKPP, exist, cut;
	float calkowity_koszt, pu, odchylenie;
	float cc, neighbour;

	int **G;
	G = new int*[m + 1]; // macierz bezposrednich polaczen miedzy czlonkami multicast
	for (i = 0; i < m + 1; i++) {
		G[i] = new int[m + 1];
	}

	int **T;
	T = new int*[n]; // macierz zawierajaca polaczenia miedzy czlonkami multicast
	for (i = 0; i < n; i++) {
		T[i] = new int[n];
	}

	int **Permute; // macierz zawierajaca sciezki miedzy wezlami multicast
	Permute = new int*[(m * (m + 1)) / 2];
	for (i = 0; i < (m * (m + 1) / 2); i++) {
		Permute[i] = new int[n + 2];
	}

	int *Path;
	Path = new int[n];
	int *edge1;
	edge1 = new int[m]; //bo m-1 krawedzi w drzewie
	int *edge2;
	edge2 = new int[m];
	int *edge3;
	edge3 = new int[n - 1];
	int *edge4;
	edge4 = new int[n - 1];
	int *links;
	links = new int[n]; // ile laczy od kazdego wezla

	for (i = 0; i < m + 1; i++)
		for (j = 0; j < m + 1; j++)
			G[i][j] = 0;
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			T[i][j] = 0;
	for (i = 0; i < n; i++) {
		Path[i] = -1;
	}
	for (i = 0; i < (m * (m + 1) / 2); i++)
		for (j = 0; j < (n + 2); j++)
			Permute[i][j] = -1;
	for (i = 0; i < m; i++)
		edge1[i] = -1;
	for (i = 0; i < m; i++)
		edge2[i] = -1;
	for (i = 0; i < n; i++)
		links[i] = 0;

	l = 1;
	av_path = 0;
	p = 0;
	nt = 0;
	all = 0;
	costKPP = 0;

	m++;

	for (k = 0; k < m; k++) { // permutacja - kazdy z kazdym wsrod wierzch multicast ((m*(m-1))/2
		for (j = l; j < m; j++) {
			mincost = DijkstraKPP_HM(n, Path, M[k], M[j], pathdelay, network);
			av_path += pathdelay;
			if (pathdelay <= delta) {
				G[k][j] = mincost;
				G[j][k] = G[k][j];

				Permute[p][0] = M[k];
				Permute[p][1] = M[j];
				for (i = 0; i < n; i++) {
					Permute[p][i + 2] = Path[i];
				}
				p++;
			}
			all++;
		}
		l++;
	}

	niespojny = 0;
	for (k = 0; k < m; k++) { // spr czy podgraf G jest spojny
		Gsum = 0;
		for (j = 0; j < m; j++) {
			Gsum += G[k][j];
		}
		if (Gsum == 0) {
			niespojny = 1;
		}
	}

	if (p >= (m - 1) && niespojny == 0) { // licz dalej jesli jest wiecej niz m-1 krawedzi w podgrafie kazdy-z-kazdym

		PrimKPP_HM(G, M, edge1, edge2, m, 0);

		for (i = 0; i < (m - 1); i++) { // na podstawie tablic edge i tablicy Permute odbuduj drzewo multicast
			for (j = 0; j < (m * (m - 1) / 2); j++) {
				if ((edge1[i] == Permute[j][0] && edge2[i] == Permute[j][1])
					|| (edge1[i] == Permute[j][1] && edge2[i] == Permute[j][0])) {
					k = 0;
					while (Permute[j][k + 3] != -1 && k <= (n + 2)) {

						if (network.has_link(Permute[j][k + 2], Permute[j][k
							+ 3])) {
							T[Permute[j][k + 2]][Permute[j][k + 3]]
								= network.get_cost(Permute[j][k + 2],
									Permute[j][k + 3]);
						} else {
							T[Permute[j][k + 2]][Permute[j][k + 3]] = 0;
						}

						if (network.has_link(Permute[j][k + 3], Permute[j][k
							+ 2])) {
							T[Permute[j][k + 3]][Permute[j][k + 2]]
								= network.get_cost(Permute[j][k + 3],
									Permute[j][k + 2]);
						} else {
							T[Permute[j][k + 3]][Permute[j][k + 2]] = 0;
						}

						k++;
					}
				}
			}
		}

		for (i = 0; i < n; i++) { //jesli graf niespojny to liczy ile wierzch wchodzi do drzewa
			k = 0;
			for (j = 0; j < n; j++) {
				if (T[i][j] != 0) {
					k++;
				}
			}
			if (k > 0)
				nt++;
		}

		costKPP = PrimFinalKPP_HM(T, edge3, edge4, n, s, nt);

		v = 0;
		for (i = 0; i < nt - 1; i++) { // sprawdza czy wszystkie wezly multicast sa w drzewie
			for (k = 1; k < m; k++) {
				if (M[k] == edge4[i]) {
					v++;
				}
			}
		}

		if (v < m - 1) {

			for (i = 0; i < m; i++) {
				delete[] G[i];
			}
			delete[] G;
			for (i = 0; i < n; i++) {
				delete[] T[i];
			}
			delete[] T;
			for (i = 0; i < (m * (m - 1) / 2); i++) {
				delete[] Permute[i];
			}
			delete[] Permute;
			delete[] Path;
			delete[] edge1;
			delete[] edge2;
			delete[] edge3;
			delete[] edge4;
			delete[] links;

			return 999999999;
		}

		costKPP = 0;
		for (k = 0; k < nt - 1; k++) {
			if (edge3[k] != -1) {
				double c;
				if (network.has_link(edge3[k], edge4[k])) {
					c = network.get_cost(edge3[k], edge4[k]);

				} else {
					c = 0.0;

				}

				costKPP += c;
				path.add_link(edge3[k], edge4[k]);
			}
		}

		for (i = 0; i < m; i++) {
			delete[] G[i];
		}
		delete[] G;
		for (i = 0; i < n; i++) {
			delete[] T[i];
		}
		delete[] T;
		for (i = 0; i < (m * (m - 1) / 2); i++) {
			delete[] Permute[i];
		}
		delete[] Permute;
		delete[] Path;
		delete[] edge1;
		delete[] edge2;
		delete[] edge3;
		delete[] edge4;
		delete[] links;

		return costKPP; //zwroc koszt drzewa multicast

	} else {

		for (i = 0; i < m; i++) {
			delete[] G[i];
		}
		delete[] G;
		for (i = 0; i < n; i++) {
			delete[] T[i];
		}
		delete[] T;
		for (i = 0; i < (m * (m - 1) / 2); i++) {
			delete[] Permute[i];
		}
		delete[] Permute;
		delete[] Path;
		delete[] edge1;
		delete[] edge2;
		delete[] edge3;
		delete[] edge4;
		delete[] links;

		return 999999999;

	} // jesli macierz G obejmuje wszystkie z M to licz dalej drzewo multicast

}
