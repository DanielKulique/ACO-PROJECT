#ifndef VISUALIZER_HPP
#define VISUALIZER_HPP

#include <cstddef>
#include <string>
#include <vector>

#include "raylib.h"

#include "ACO.hpp"
#include "Graph.hpp"

class Visualizer
{
private:
    const Graph& graph;
    ACO& aco;
    Font fonte;

    int largura;
    int altura;
    int larguraPainel;

    bool pausado;
    bool mostrarExploracao;

    float intervaloPasso;
    float tempoAteProximoPasso;
    
    std::size_t formigaSelecionada;

    // NOVO
    bool animandoMovimento;
    float progressoMovimento;
    float duracaoMovimento;

    std::vector<int> origemAnimacao;
    std::vector<int> destinoAnimacao;

    bool iniciarPassoAnimado();

    void atualizarAnimacao(float dt);

    Vector2 posicaoAnimadaFormiga(
        std::size_t indice,
        const std::vector<Vector2>& posicoes
    ) const;

    std::vector<Vector2> calcularPosicoes() const;

    void desenharFundo() const;
    void desenharArestas(const std::vector<Vector2>& posicoes) const;
    void desenharExploracao(const std::vector<Vector2>& posicoes) const;
    void desenharMelhorGlobal(const std::vector<Vector2>& posicoes) const;
    void desenharMelhorIteracao(const std::vector<Vector2>& posicoes) const;
    void desenharVertices(const std::vector<Vector2>& posicoes) const;
    void desenharFormigasAtuais(const std::vector<Vector2>& posicoes) const;
    void desenharPainel() const;

    void processarEntrada();
    bool executarUmPasso();
    void ajustarFormigaSelecionada();

    const std::vector<Ant>& formigasParaExibir() const;
    const DecisaoACO* decisaoDaFormiga(std::size_t indice) const;

    std::string formatarCaminho(
        const std::vector<int>& caminho,
        std::size_t limite = 10
    ) const;



public:
    Visualizer(
        const Graph& graph,
        ACO& aco,
        int largura = 1220,
        int altura = 760
    );

    void executar();
    
    void desenharTexto(
    const char* texto,
    float x,
    float y,
    float tamanho,
    Color cor
    ) const;
};

#endif
