#include "Ant.hpp"

Ant::Ant(int quantidadeVertices, int inicio)
    : atual(inicio),
      visitado(quantidadeVertices, false),
      custo(0.0)
{
    caminho.push_back(inicio);
    visitado[inicio] = true;
}

void Ant::caminhar(int destino, const Graph& graph)
{
    custo += graph.getPeso(atual, destino);
    atual = destino;
    caminho.push_back(destino);
    visitado[destino] = true;
}
