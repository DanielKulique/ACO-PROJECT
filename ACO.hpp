#ifndef ACO_HPP
#define ACO_HPP

#include <random>
#include <vector>

#include "Ant.hpp"
#include "Graph.hpp"

enum class FaseACO
{
    PREPARANDO,
    CONSTRUINDO,
    FECHANDO_CICLOS,
    ATUALIZANDO_FEROMONIO,
    FINALIZADO
};

struct CandidatoACO
{
    int vertice = -1;
    double probabilidade = 0.0;
};

struct DecisaoACO
{
    int indiceFormiga = -1;
    int origem = -1;
    int destino = -1;
    double sorteio = 0.0;
    std::vector<CandidatoACO> candidatos;
};

class ACO
{
private:
    const Graph& graph;

    std::vector<std::vector<double>> feromonio;

    int quantidadeFormigas;
    int quantidadeIteracoes;
    int iteracoesConcluidas;
    int passoAtual;

    double alpha;
    double beta;
    double sigma;
    double Q;
    double feromonioInicial;

    std::mt19937 gerador;

    std::vector<int> melhorCaminho;
    double melhorCusto;

    std::vector<int> melhorCaminhoIteracao;
    double melhorCustoIteracao;

    std::vector<Ant> formigasAtuais;
    std::vector<Ant> ultimasFormigas;
    std::vector<DecisaoACO> ultimasDecisoes;

    FaseACO fase;

    double calcularProbabilidade(const Ant& ant, int destino) const;
    double calcularHeuristica(int origem, int destino) const;
    DecisaoACO escolherProximoVertice(const Ant& ant, int indiceFormiga);

    void iniciarIteracao();
    void fecharCiclos();
    void evaporarFeromonio();
    void atualizarFeromonio(const std::vector<Ant>& formigas);
    void atualizarMelhorSolucao(const Ant& ant);
    bool rotaCompleta(const Ant& ant) const;
    void inicializarFeromonio();

public:
    ACO(
        const Graph& graph,
        int quantidadeFormigas,
        int quantidadeIteracoes,
        double alpha,
        double beta,
        double sigma,
        double Q,
        double feromonioInicial
    );

    // Executa uma etapa visual do ACO. Durante CONSTRUINDO, cada formiga
    // anda no maximo uma aresta por chamada.
    bool executarPasso();

    // Executa ate o fim, util para testes sem a interface grafica.
    void executar();

    void reiniciar();
    bool terminou() const;

    const std::vector<int>& getMelhorCaminho() const;
    double getMelhorCusto() const;

    const std::vector<int>& getMelhorCaminhoIteracao() const;
    double getMelhorCustoIteracao() const;

    const std::vector<Ant>& getFormigasAtuais() const;
    const std::vector<Ant>& getUltimasFormigas() const;
    const std::vector<DecisaoACO>& getUltimasDecisoes() const;

    double getFeromonio(int origem, int destino) const;

    int getIteracaoAtual() const;
    int getIteracoesConcluidas() const;
    int getQuantidadeIteracoes() const;
    int getQuantidadeFormigas() const;
    int getPassoAtual() const;
    int getPassosPorIteracao() const;

    double getAlpha() const;
    double getBeta() const;
    double getSigma() const;
    double getQ() const;

    FaseACO getFase() const;
    const char* getNomeFase() const;
};

#endif
