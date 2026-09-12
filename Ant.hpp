
#ifndef ANT_HPP
#define ANT_HPP

#include <vector>
#include "Graph.hpp"


struct Ant
{
	int atual;
	std::vector<int> caminho;
	std::vector<bool> visitado;
	double custo;



	Ant(int quantidade_vertices, int inicio);

	void caminhar(int destino, const Graph& graph);

};


#endif
