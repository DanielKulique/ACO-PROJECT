#include <iostream>

#include "ACO.hpp"
#include "Graph.hpp"
#include "Visualizer.hpp"

int main()
{
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
        5,       // uma formiga por vertice: mais didatico para visualizar
        100,     // quantidade de iteracoes
        1.0,     // alpha: peso da heuristica 1/distancia
        1.0,     // beta: peso do feromonio
        0.01,    // sigma: evaporacao
        10.0,    // Q
        0.1      // feromonio inicial
    );

    Visualizer visualizer(graph, aco);
    visualizer.executar();

    const std::vector<int>& caminho = aco.getMelhorCaminho();

    if (!caminho.empty())
    {
        std::cout << "\nMelhor caminho: ";

        for (std::size_t i = 0; i < caminho.size(); ++i)
        {
            std::cout << caminho[i];

            if (i + 1 < caminho.size())
            {
                std::cout << " -> ";
            }
        }

        std::cout
            << "\nMelhor custo: "
            << aco.getMelhorCusto()
            << '\n';
    }

    return 0;
}
