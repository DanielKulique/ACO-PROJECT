#include <iostream>

#include "ACO.hpp"
#include "Graph.hpp"
#include "Visualizer.hpp"
#include <random>

int main()
{
    const int linhas = 5;
    const int colunas = 5;

    const int origem = 20;
    const int destino = 14;

    std::random_device rd;
    std::mt19937 gerador(rd());

    // pesos inteiros de 1 a 20
    std::uniform_int_distribution<int> pesoAleatorio(1, 20);


    Graph graph(linhas * colunas); // 25 vertices

    for (int linha = 0; linha < linhas; linha++)
    {
        for (int coluna = 0; coluna < colunas; coluna++)
        {
            int atual = linha * colunas + coluna;

            // liga com o vertice da direita
            if (coluna < colunas - 1)
            {
                int direita = atual + 1;

                double peso = pesoAleatorio(gerador);

                graph.peso[atual][direita] = peso;
                graph.peso[direita][atual] = peso;
            }

            // liga com o vertice de baixo
            if (linha < linhas - 1)
            {
                int baixo = atual + colunas;

                double peso = pesoAleatorio(gerador);

                graph.peso[atual][baixo] = peso;
                graph.peso[baixo][atual] = peso;
            }
        }
    }


    ACO aco(
        graph,
        20,      // quantidade de formigas
        100,     // iteracoes
        1.0,     // alpha
        1.0,     // beta
        0.01,    // sigma
        10.0,    // Q
        0.1,     // feromonio inicial
        origem,
        destino
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