#include <iostream>

#include "Graph.hpp"
#include "Ant.hpp"
#include "ACO.hpp"

int main()
{
    std::cout << "Hello Ant!\n";

    Graph graph(5);

    graph.peso = {
        { 0, 22, 50, 48, 29},
        {22,  0, 30, 34, 32},
        {50, 30,  0, 22, 23},
        {48, 34, 22,  0, 35},
        {29, 32, 23, 35,  0}
    };

    ACO aco(
        graph,
        10,     // quantidade de formigas
        100,    // quantidade de iteracoes
        1.0,    // alpha
        1.0,    // beta
        0.01    // evaporacao
    );

    // aco.executar();

    return 0;
}