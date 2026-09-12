#include "ACO.hpp"

ACO::ACO(
    const Graph& graph,
    int quantidadeFormigas,
    int quantidadeIteracoes,
    double alpha,
    double beta,
    double sigma,
    double Q,
    double feromonioInicial
)
    : graph(graph),
      quantidadeFormigas(quantidadeFormigas),
      quantidadeIteracoes(quantidadeIteracoes),
      alpha(alpha),
      beta(beta),
      sigma(sigma),
      Q(Q),
      feromonioInicial(feromonioInicial),
      feromonio(
          graph.size(),
          std::vector<double>(
              graph.size(),
              feromonioInicial
          )
      )
{
}

double ACO::calcularHeuristica(
    int origem,
    int destino
) const
{
    return 1.0 / graph.getPeso(origem, destino);
}

double ACO::calcularProbabilidade(
    const Ant& ant,
    int destino
)
{
    // calcula numerador
    // calcula somatório
    // calcula numerador / denominador

    return 0.0;
}

int ACO::escolherProximoVertice(
    const Ant& ant
)
{
    // calcula probabilidades
    // faz escolha probabilística

    return -1;
}

void ACO::evaporarFeromonio()
{
}

void ACO::atualizarFeromonio(
    const std::vector<Ant>& formigas
)
{
}

void ACO::executar()
{
}