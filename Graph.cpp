#include "Graph.hpp"

Graph::Graph(int size)
    : peso(size, std::vector<double>(size, 0.0))
{
}

int Graph::size() const
{
    return static_cast<int>(peso.size());
}

double Graph::getPeso(int origem, int destino) const
{
    return peso[origem][destino];
}
