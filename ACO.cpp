#include "ACO.hpp"
#include <cmath>
#include <cstdio>

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

double ACO::calcularHeuristica( //distancia inversa!
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
    //txy(t) alpha x nxy(t) beta
    int x = ant.atual;
    int y = destino;


    if (graph.getPeso(x, y) <= 0 || ant.visitado[y])
    {
        return 0.0;
    }

    double tal = calcularHeuristica(x, y);
    double eta = feromonio[x][y];

    double tal1 = std::pow(tal, alpha);
    double eta2 = std::pow(eta, beta);
    double numerador = tal1 * eta2;



    // calcula somatório
    double somatoria = 0.0;
    //vertice atual x y -> encontrar vizinhos 

    //vizinhos validos? x -> y > 0? && nao visitado?

    for (int l = 0; l < graph.size(); l++){

        if (graph.getPeso(x,l) > 0 && !ant.visitado[l]){
            
            //vertice disponível! 

            double tal_l = 
                std::pow(calcularHeuristica(x, l), alpha);
            double eta_l = 
                std::pow(feromonio[x][l], beta);

            somatoria += tal_l * eta_l;

        }
    }

    double denominador = somatoria;
    
    // calcula numerador / denominador

    double probabilidade = numerador/denominador;

    if (somatoria == 0.0)
    {
        return 0.0;     
    }

    return probabilidade;
}

int ACO::escolherProximoVertice(const Ant& ant)
{
    int x = ant.atual;

    for (int l = 0; l < graph.size(); l++)
    {
        if (graph.getPeso(x, l) > 0 && !ant.visitado[l])
        {
            double probabilidade =
                calcularProbabilidade(ant, l);

            printf(
                "Vizinho: %d | Peso: %.2f | Prob: %.4f\n",
                l,
                graph.getPeso(x, l),
                probabilidade
            );
        }
    }

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