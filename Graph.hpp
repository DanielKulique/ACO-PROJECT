#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>


struct Graph
{
	std::vector<std::vector<double>> peso;	

	Graph(int size);

	int size() const;
	double getPeso(int origem, int destino) const;

};


#endif