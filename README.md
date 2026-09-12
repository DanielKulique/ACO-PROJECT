# ACO Project

Implementação do algoritmo **Ant Colony Optimization (ACO)** em C++ para resolução do **Problema do Caixeiro Viajante (Traveling Salesman Problem - TSP)**.

O projeto está sendo desenvolvido com foco no estudo do funcionamento do ACO, incluindo seleção probabilística de caminhos, evaporação e depósito de feromônio.

## Objetivo

Encontrar um caminho de baixo custo entre todos os vértices de um grafo utilizando o comportamento coletivo de formigas artificiais.

Cada formiga constrói uma solução percorrendo o grafo e escolhendo o próximo vértice com base em:

- distância entre os vértices;
- quantidade de feromônio presente na aresta;
- influência da heurística (`alpha`);
- influência do feromônio (`beta`).

Após cada iteração, ocorre atualização da matriz de feromônio.

## Estrutura do projeto

```text
.
├── ACO.cpp
├── ACO.hpp
├── Ant.cpp
├── Ant.hpp
├── Graph.cpp
├── Graph.hpp
├── main.cpp
└── README.md
