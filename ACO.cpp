#include "ACO.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>

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
      feromonio(
          graph.size(),
          std::vector<double>(graph.size(), 0.0)
      ),
      quantidadeFormigas(quantidadeFormigas),
      quantidadeIteracoes(quantidadeIteracoes),
      iteracoesConcluidas(0),
      passoAtual(0),
      alpha(alpha),
      beta(beta),
      sigma(sigma),
      Q(Q),
      feromonioInicial(feromonioInicial),
      gerador(std::random_device{}()),
      melhorCusto(std::numeric_limits<double>::infinity()),
      melhorCustoIteracao(std::numeric_limits<double>::infinity()),
      fase(FaseACO::PREPARANDO)
{
    inicializarFeromonio();

    if (graph.size() > 0 && quantidadeFormigas > 0 && quantidadeIteracoes > 0)
    {
        iniciarIteracao();
    }
    else
    {
        fase = FaseACO::FINALIZADO;
    }
}

void ACO::inicializarFeromonio()
{
    for (int i = 0; i < graph.size(); ++i)
    {
        for (int j = 0; j < graph.size(); ++j)
        {
            feromonio[i][j] =
                graph.getPeso(i, j) > 0.0
                    ? feromonioInicial
                    : 0.0;
        }
    }
}

double ACO::calcularHeuristica(int origem, int destino) const
{
    const double distancia = graph.getPeso(origem, destino);

    if (distancia <= 0.0)
    {
        return 0.0;
    }

    return 1.0 / distancia;
}

double ACO::calcularProbabilidade(const Ant& ant, int destino) const
{
    const int origem = ant.atual;

    if (
        destino < 0 ||
        destino >= graph.size() ||
        graph.getPeso(origem, destino) <= 0.0 ||
        ant.visitado[destino]
    )
    {
        return 0.0;
    }

    const double heuristicaDestino = calcularHeuristica(origem, destino);
    const double feromonioDestino = feromonio[origem][destino];

    const double numerador =
        std::pow(heuristicaDestino, alpha) *
        std::pow(feromonioDestino, beta);

    double denominador = 0.0;

    for (int candidato = 0; candidato < graph.size(); ++candidato)
    {
        if (
            graph.getPeso(origem, candidato) > 0.0 &&
            !ant.visitado[candidato]
        )
        {
            denominador +=
                std::pow(calcularHeuristica(origem, candidato), alpha) *
                std::pow(feromonio[origem][candidato], beta);
        }
    }

    if (denominador <= 0.0)
    {
        return 0.0;
    }

    return numerador / denominador;
}

DecisaoACO ACO::escolherProximoVertice(
    const Ant& ant,
    int indiceFormiga
)
{
    DecisaoACO decisao;
    decisao.indiceFormiga = indiceFormiga;
    decisao.origem = ant.atual;

    for (int candidato = 0; candidato < graph.size(); ++candidato)
    {
        if (
            graph.getPeso(ant.atual, candidato) > 0.0 &&
            !ant.visitado[candidato]
        )
        {
            decisao.candidatos.push_back({
                candidato,
                calcularProbabilidade(ant, candidato)
            });
        }
    }

    if (decisao.candidatos.empty())
    {
        return decisao;
    }

    std::uniform_real_distribution<double> distribuicao(0.0, 1.0);
    decisao.sorteio = distribuicao(gerador);

    double acumulada = 0.0;
    int ultimoValido = decisao.candidatos.back().vertice;

    for (const CandidatoACO& candidato : decisao.candidatos)
    {
        acumulada += candidato.probabilidade;

        if (decisao.sorteio <= acumulada)
        {
            decisao.destino = candidato.vertice;
            return decisao;
        }
    }

    // Protecao contra pequenas imprecisoes de ponto flutuante.
    decisao.destino = ultimoValido;
    return decisao;
}

void ACO::iniciarIteracao()
{
    formigasAtuais.clear();
    formigasAtuais.reserve(quantidadeFormigas);

    for (int k = 0; k < quantidadeFormigas; ++k)
    {
        const int inicio = k % graph.size();
        formigasAtuais.emplace_back(graph.size(), inicio);
    }

    melhorCaminhoIteracao.clear();
    melhorCustoIteracao = std::numeric_limits<double>::infinity();
    ultimasDecisoes.clear();

    passoAtual = 0;
    fase = FaseACO::CONSTRUINDO;
}

bool ACO::rotaCompleta(const Ant& ant) const
{
    return
        ant.caminho.size() ==
            static_cast<std::size_t>(graph.size() + 1) &&
        ant.caminho.front() == ant.caminho.back();
}

void ACO::fecharCiclos()
{
    melhorCaminhoIteracao.clear();
    melhorCustoIteracao = std::numeric_limits<double>::infinity();

    for (Ant& ant : formigasAtuais)
    {
        if (
            ant.caminho.size() == static_cast<std::size_t>(graph.size()) &&
            !ant.caminho.empty()
        )
        {
            const int inicio = ant.caminho.front();

            if (graph.getPeso(ant.atual, inicio) > 0.0)
            {
                ant.caminhar(inicio, graph);
            }
        }

        if (!rotaCompleta(ant))
        {
            continue;
        }

        if (ant.custo < melhorCustoIteracao)
        {
            melhorCustoIteracao = ant.custo;
            melhorCaminhoIteracao = ant.caminho;
        }

        atualizarMelhorSolucao(ant);
    }

    // Mantem uma copia da rodada completa para a visualizacao.
    ultimasFormigas = formigasAtuais;
}

void ACO::evaporarFeromonio()
{
    const double fator = std::clamp(1.0 - sigma, 0.0, 1.0);

    for (int i = 0; i < graph.size(); ++i)
    {
        for (int j = 0; j < graph.size(); ++j)
        {
            if (graph.getPeso(i, j) > 0.0)
            {
                feromonio[i][j] *= fator;
            }
        }
    }
}

void ACO::atualizarFeromonio(const std::vector<Ant>& formigas)
{
    for (const Ant& ant : formigas)
    {
        if (!rotaCompleta(ant) || ant.custo <= 0.0)
        {
            continue;
        }

        const double delta = Q / ant.custo;

        for (std::size_t i = 0; i + 1 < ant.caminho.size(); ++i)
        {
            const int origem = ant.caminho[i];
            const int destino = ant.caminho[i + 1];

            feromonio[origem][destino] += delta;

            // Grafo nao direcionado: atualiza tambem o sentido inverso.
            feromonio[destino][origem] += delta;
        }
    }
}

void ACO::atualizarMelhorSolucao(const Ant& ant)
{
    if (!rotaCompleta(ant))
    {
        return;
    }

    if (ant.custo < melhorCusto)
    {
        melhorCusto = ant.custo;
        melhorCaminho = ant.caminho;
    }
}

bool ACO::executarPasso()
{
    if (fase == FaseACO::FINALIZADO)
    {
        return false;
    }

    // A fase PREPARANDO existe entre duas iteracoes. Na mesma chamada,
    // cria a nova colonia e ja permite o primeiro movimento.
    if (fase == FaseACO::PREPARANDO)
    {
        iniciarIteracao();
    }

    if (fase == FaseACO::CONSTRUINDO)
    {
        ultimasDecisoes.clear();
        bool algumaMoveu = false;

        for (std::size_t k = 0; k < formigasAtuais.size(); ++k)
        {
            Ant& ant = formigasAtuais[k];

            if (
                ant.caminho.size() >=
                static_cast<std::size_t>(graph.size())
            )
            {
                continue;
            }

            DecisaoACO decisao = escolherProximoVertice(
                ant,
                static_cast<int>(k)
            );

            ultimasDecisoes.push_back(decisao);

            if (decisao.destino != -1)
            {
                ant.caminhar(decisao.destino, graph);
                algumaMoveu = true;
            }
        }

        ++passoAtual;

        bool todasConstruidas = true;

        for (const Ant& ant : formigasAtuais)
        {
            if (
                ant.caminho.size() <
                static_cast<std::size_t>(graph.size())
            )
            {
                todasConstruidas = false;
                break;
            }
        }

        // Em um grafo incompleto, !algumaMoveu tambem evita loop infinito.
        if (todasConstruidas || !algumaMoveu)
        {
            fase = FaseACO::FECHANDO_CICLOS;
        }

        return true;
    }

    if (fase == FaseACO::FECHANDO_CICLOS)
    {
        fecharCiclos();
        fase = FaseACO::ATUALIZANDO_FEROMONIO;
        return true;
    }

    if (fase == FaseACO::ATUALIZANDO_FEROMONIO)
    {
        evaporarFeromonio();
        atualizarFeromonio(formigasAtuais);

        ++iteracoesConcluidas;

        if (iteracoesConcluidas >= quantidadeIteracoes)
        {
            fase = FaseACO::FINALIZADO;
        }
        else
        {
            fase = FaseACO::PREPARANDO;
        }

        return true;
    }

    return false;
}

void ACO::executar()
{
    int iteracaoAnterior = iteracoesConcluidas;

    while (!terminou())
    {
        executarPasso();

        if (iteracoesConcluidas != iteracaoAnterior)
        {
            std::cout
                << "Iteracao " << iteracoesConcluidas
                << " | Melhor da iteracao: ";

            if (std::isfinite(melhorCustoIteracao))
            {
                std::cout << melhorCustoIteracao;
            }
            else
            {
                std::cout << "sem solucao";
            }

            std::cout << " | Melhor global: ";

            if (std::isfinite(melhorCusto))
            {
                std::cout << melhorCusto;
            }
            else
            {
                std::cout << "sem solucao";
            }

            std::cout << '\n';
            iteracaoAnterior = iteracoesConcluidas;
        }
    }
}

void ACO::reiniciar()
{
    iteracoesConcluidas = 0;
    passoAtual = 0;

    melhorCaminho.clear();
    melhorCaminhoIteracao.clear();
    formigasAtuais.clear();
    ultimasFormigas.clear();
    ultimasDecisoes.clear();

    melhorCusto = std::numeric_limits<double>::infinity();
    melhorCustoIteracao = std::numeric_limits<double>::infinity();

    inicializarFeromonio();
    gerador.seed(std::random_device{}());

    if (graph.size() > 0 && quantidadeFormigas > 0 && quantidadeIteracoes > 0)
    {
        fase = FaseACO::PREPARANDO;
        iniciarIteracao();
    }
    else
    {
        fase = FaseACO::FINALIZADO;
    }
}

bool ACO::terminou() const
{
    return fase == FaseACO::FINALIZADO;
}

const std::vector<int>& ACO::getMelhorCaminho() const
{
    return melhorCaminho;
}

double ACO::getMelhorCusto() const
{
    return melhorCusto;
}

const std::vector<int>& ACO::getMelhorCaminhoIteracao() const
{
    return melhorCaminhoIteracao;
}

double ACO::getMelhorCustoIteracao() const
{
    return melhorCustoIteracao;
}

const std::vector<Ant>& ACO::getFormigasAtuais() const
{
    return formigasAtuais;
}

const std::vector<Ant>& ACO::getUltimasFormigas() const
{
    return ultimasFormigas;
}

const std::vector<DecisaoACO>& ACO::getUltimasDecisoes() const
{
    return ultimasDecisoes;
}

double ACO::getFeromonio(int origem, int destino) const
{
    return feromonio[origem][destino];
}

int ACO::getIteracaoAtual() const
{
    if (quantidadeIteracoes <= 0)
    {
        return 0;
    }

    if (terminou())
    {
        return quantidadeIteracoes;
    }

    return std::min(iteracoesConcluidas + 1, quantidadeIteracoes);
}

int ACO::getIteracoesConcluidas() const
{
    return iteracoesConcluidas;
}

int ACO::getQuantidadeIteracoes() const
{
    return quantidadeIteracoes;
}

int ACO::getQuantidadeFormigas() const
{
    return quantidadeFormigas;
}

int ACO::getPassoAtual() const
{
    return passoAtual;
}

int ACO::getPassosPorIteracao() const
{
    return std::max(0, graph.size() - 1);
}

double ACO::getAlpha() const
{
    return alpha;
}

double ACO::getBeta() const
{
    return beta;
}

double ACO::getSigma() const
{
    return sigma;
}

double ACO::getQ() const
{
    return Q;
}

FaseACO ACO::getFase() const
{
    return fase;
}

const char* ACO::getNomeFase() const
{
    switch (fase)
    {
        case FaseACO::PREPARANDO:
            return "PREPARANDO";
        case FaseACO::CONSTRUINDO:
            return "CONSTRUINDO ROTAS";
        case FaseACO::FECHANDO_CICLOS:
            return "FECHANDO CICLOS";
        case FaseACO::ATUALIZANDO_FEROMONIO:
            return "ATUALIZANDO FEROMONIO";
        case FaseACO::FINALIZADO:
            return "FINALIZADO";
    }

    return "--";
}
