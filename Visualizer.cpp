#include "Visualizer.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <sstream>

namespace
{
constexpr float PI_F = 3.14159265358979323846f;

const Color COR_FUNDO = {248, 249, 252, 255};
const Color COR_PAINEL = {238, 241, 246, 255};
const Color COR_BORDA = {205, 211, 222, 255};

const Color COR_TEXTO = {31, 36, 46, 255};
const Color COR_TEXTO_SECUNDARIO = {91, 99, 113, 255};

const Color COR_VERTICE = {18, 93, 170, 255};

const Color COR_MELHOR_GLOBAL = {224, 52, 61, 255};
const Color COR_MELHOR_ITERACAO = {245, 166, 35, 255};

const Color COR_EXPLORACAO = {65, 132, 210, 255};
const Color COR_FEROMONIO = {72, 79, 91, 255};

const Color COR_FORMIGA = {245, 166, 35, 255};
const Color COR_FORMIGA_SELECIONADA = {255, 196, 0, 255};


float limitar(
    float valor,
    float minimo,
    float maximo
)
{
    return std::max(
        minimo,
        std::min(valor, maximo)
    );
}

}


// ============================================================
// CONSTRUTOR
// ============================================================

Visualizer::Visualizer(
    const Graph& graph,
    ACO& aco,
    int largura,
    int altura
)
    : graph(graph),
      aco(aco),
      largura(largura),
      altura(altura),
      larguraPainel(370),

      // inicia pausado
      pausado(true),

      mostrarExploracao(true),

      // tempo parado entre dois movimentos
      intervaloPasso(0.40f),

      tempoAteProximoPasso(0.0f),

      formigaSelecionada(0),

      // animação
      animandoMovimento(false),
      progressoMovimento(0.0f),

      // tempo para atravessar uma aresta
      duracaoMovimento(1.10f)
{
}


// ============================================================
// POSICOES DOS VERTICES
// ============================================================

std::vector<Vector2>
Visualizer::calcularPosicoes() const
{
    std::vector<Vector2> posicoes;

    posicoes.reserve(graph.size());

    if (graph.size() == 0)
    {
        return posicoes;
    }


    const float larguraGrafo =
        static_cast<float>(
            largura - larguraPainel
        );


    const Vector2 centro = {
        larguraGrafo / 2.0f,

        static_cast<float>(altura) /
            2.0f + 20.0f
    };


    const float raio =
        std::min(
            larguraGrafo,
            static_cast<float>(altura)
        ) * 0.34f;


    for (
        int i = 0;
        i < graph.size();
        ++i
    )
    {
        const float angulo =
            -PI_F / 2.0f
            +
            (
                2.0f *
                PI_F *
                static_cast<float>(i)
            )
            /
            static_cast<float>(
                graph.size()
            );


        posicoes.push_back({
            centro.x +
                raio *
                std::cos(angulo),

            centro.y +
                raio *
                std::sin(angulo)
        });
    }


    return posicoes;
}


// ============================================================
// TEXTO
// ============================================================

void Visualizer::desenharTexto(
    const char* texto,
    float x,
    float y,
    float tamanho,
    Color cor
) const
{
    DrawTextEx(
        fonte,
        texto,
        {x, y},
        tamanho,
        1.0f,
        cor
    );
}


// ============================================================
// FUNDO
// ============================================================

void Visualizer::desenharFundo() const
{
    ClearBackground(COR_FUNDO);


    desenharTexto(
        "Ant Colony Optimization - TSP",
        30,
        22,
        28,
        COR_TEXTO
    );


    desenharTexto(
        "Construcao das rotas passo a passo pelas formigas",
        29,
        55,
        17,
        COR_TEXTO_SECUNDARIO
    );
}


// ============================================================
// ARESTAS / FEROMONIO / PESOS
// ============================================================

void Visualizer::desenharArestas(
    const std::vector<Vector2>& posicoes
) const
{
    double menorFeromonio =
        std::numeric_limits<double>::infinity();

    double maiorFeromonio = 0.0;


    // encontra menor e maior quantidade de feromônio
    for (
        int i = 0;
        i < graph.size();
        ++i
    )
    {
        for (
            int j = i + 1;
            j < graph.size();
            ++j
        )
        {
            if (graph.getPeso(i, j) > 0.0)
            {
                const double tau =
                    aco.getFeromonio(i, j);

                menorFeromonio =
                    std::min(
                        menorFeromonio,
                        tau
                    );

                maiorFeromonio =
                    std::max(
                        maiorFeromonio,
                        tau
                    );
            }
        }
    }


    if (!std::isfinite(menorFeromonio))
    {
        menorFeromonio = 0.0;
    }


    const double faixa =
        maiorFeromonio -
        menorFeromonio;


    for (
        int i = 0;
        i < graph.size();
        ++i
    )
    {
        for (
            int j = i + 1;
            j < graph.size();
            ++j
        )
        {
            const double peso =
                graph.getPeso(i, j);


            if (peso <= 0.0)
            {
                continue;
            }


            // --------------------------------------------
            // intensidade visual do feromônio
            // --------------------------------------------

            double nivel = 0.18;


            if (faixa > 1e-12)
            {
                nivel =
                    (
                        aco.getFeromonio(i, j)
                        -
                        menorFeromonio
                    )
                    /
                    faixa;
            }


            nivel =
                std::clamp(
                    nivel,
                    0.0,
                    1.0
                );


            const float espessura =
                1.2f +
                static_cast<float>(nivel) *
                    6.5f;


            const float alphaCor =
                0.28f +
                static_cast<float>(nivel) *
                    0.55f;


            DrawLineEx(
                posicoes[i],
                posicoes[j],
                espessura,
                Fade(
                    COR_FEROMONIO,
                    alphaCor
                )
            );


            // --------------------------------------------
            // texto do peso
            // --------------------------------------------

            const Vector2 a =
                posicoes[i];

            const Vector2 b =
                posicoes[j];


            const float dx =
                b.x - a.x;

            const float dy =
                b.y - a.y;


            const float comprimento =
                std::sqrt(
                    dx * dx +
                    dy * dy
                );


            Vector2 normal = {
                0.0f,
                0.0f
            };


            if (comprimento > 0.0f)
            {
                normal = {
                    -dy / comprimento,
                    dx / comprimento
                };
            }


            const float sinal =
                ((i + j) % 2 == 0)
                    ? 1.0f
                    : -1.0f;


            const float deslocamento =
                13.0f * sinal;


            const Vector2 meio = {
                (a.x + b.x) / 2.0f +
                    normal.x *
                    deslocamento,

                (a.y + b.y) / 2.0f +
                    normal.y *
                    deslocamento
            };


            const char* texto =
                TextFormat(
                    "%.0f",
                    peso
                );


            const float tamanho =
                16.0f;


            const Vector2 medida =
                MeasureTextEx(
                    fonte,
                    texto,
                    tamanho,
                    1.0f
                );


            DrawRectangleRounded(
                {
                    meio.x -
                        medida.x / 2.0f -
                        5.0f,

                    meio.y -
                        9.0f,

                    medida.x +
                        10.0f,

                    20.0f
                },

                0.35f,

                6,

                Fade(
                    COR_FUNDO,
                    0.92f
                )
            );


            desenharTexto(
                texto,

                meio.x -
                    medida.x / 2.0f,

                meio.y -
                    8.0f,

                tamanho,

                COR_TEXTO_SECUNDARIO
            );
        }
    }
}


// ============================================================
// FORMIGAS QUE DEVEM SER MOSTRADAS
// ============================================================

const std::vector<Ant>&
Visualizer::formigasParaExibir() const
{
    if (!aco.getFormigasAtuais().empty())
    {
        return aco.getFormigasAtuais();
    }

    return aco.getUltimasFormigas();
}


// ============================================================
// POSICAO ANIMADA DE UMA FORMIGA
// ============================================================

Vector2 Visualizer::posicaoAnimadaFormiga(
    std::size_t indice,
    const std::vector<Vector2>& posicoes
) const
{
    const std::vector<Ant>& formigas =
        formigasParaExibir();


    if (indice >= formigas.size())
    {
        return {0.0f, 0.0f};
    }


    const int atual =
        formigas[indice].atual;


    if (
        !animandoMovimento ||
        indice >= origemAnimacao.size() ||
        indice >= destinoAnimacao.size()
    )
    {
        return posicoes[atual];
    }


    const int origem =
        origemAnimacao[indice];

    const int destino =
        destinoAnimacao[indice];


    if (
        origem < 0 ||
        destino < 0 ||
        origem >=
            static_cast<int>(posicoes.size()) ||
        destino >=
            static_cast<int>(posicoes.size()) ||
        origem == destino
    )
    {
        return posicoes[atual];
    }


    float t =
        std::clamp(
            progressoMovimento,
            0.0f,
            1.0f
        );


    // smoothstep:
    // começa devagar, acelera e desacelera no final
    t =
        t * t *
        (3.0f - 2.0f * t);


    const Vector2 inicio =
        posicoes[origem];

    const Vector2 fim =
        posicoes[destino];


    return {
        inicio.x +
            (fim.x - inicio.x) * t,

        inicio.y +
            (fim.y - inicio.y) * t
    };
}


// ============================================================
// CAMINHOS DE EXPLORACAO
// ============================================================

void Visualizer::desenharExploracao(
    const std::vector<Vector2>& posicoes
) const
{
    if (!mostrarExploracao)
    {
        return;
    }


    const std::vector<Ant>& formigas =
        formigasParaExibir();


    for (
        std::size_t k = 0;
        k < formigas.size();
        ++k
    )
    {
        const Ant& ant =
            formigas[k];


        if (ant.caminho.size() < 2)
        {
            continue;
        }


        const bool animaUltimoSegmento =
            animandoMovimento &&
            k < origemAnimacao.size() &&
            k < destinoAnimacao.size() &&
            origemAnimacao[k] !=
                destinoAnimacao[k];


        std::size_t completos =
            ant.caminho.size() - 1;


        if (
            animaUltimoSegmento &&
            completos > 0
        )
        {
            completos--;
        }


        // caminhos que a formiga já concluiu
        for (
            std::size_t i = 0;
            i < completos;
            ++i
        )
        {
            const int origem =
                ant.caminho[i];

            const int destino =
                ant.caminho[i + 1];


            DrawLineEx(
                posicoes[origem],
                posicoes[destino],
                1.8f,
                Fade(
                    COR_EXPLORACAO,
                    0.18f
                )
            );
        }


        // segmento atualmente percorrido
        if (animaUltimoSegmento)
        {
            const int origem =
                origemAnimacao[k];


            const Vector2 atual =
                posicaoAnimadaFormiga(
                    k,
                    posicoes
                );


            DrawLineEx(
                posicoes[origem],
                atual,
                2.2f,
                Fade(
                    COR_EXPLORACAO,
                    0.32f
                )
            );
        }
    }
}


// ============================================================
// MELHOR CAMINHO GLOBAL
// ============================================================

void Visualizer::desenharMelhorGlobal(
    const std::vector<Vector2>& posicoes
) const
{
    const std::vector<int>& caminho =
        aco.getMelhorCaminho();


    for (
        std::size_t i = 0;
        i + 1 < caminho.size();
        ++i
    )
    {
        DrawLineEx(
            posicoes[caminho[i]],
            posicoes[caminho[i + 1]],
            6.0f,
            Fade(
                COR_MELHOR_GLOBAL,
                0.88f
            )
        );
    }
}


// ============================================================
// MELHOR CAMINHO DA ITERACAO
// ============================================================

void Visualizer::desenharMelhorIteracao(
    const std::vector<Vector2>& posicoes
) const
{
    const std::vector<int>& caminho =
        aco.getMelhorCaminhoIteracao();


    for (
        std::size_t i = 0;
        i + 1 < caminho.size();
        ++i
    )
    {
        DrawLineEx(
            posicoes[caminho[i]],
            posicoes[caminho[i + 1]],
            2.5f,
            COR_MELHOR_ITERACAO
        );
    }
}


// ============================================================
// VERTICES
// ============================================================

void Visualizer::desenharVertices(
    const std::vector<Vector2>& posicoes
) const
{
    for (
        int i = 0;
        i < graph.size();
        ++i
    )
    {
        DrawCircleV(
            posicoes[i],
            28.0f,
            COR_VERTICE
        );


        DrawCircleLines(
            static_cast<int>(
                posicoes[i].x
            ),

            static_cast<int>(
                posicoes[i].y
            ),

            28.0f,

            Fade(
                SKYBLUE,
                0.95f
            )
        );


        const char* rotulo =
            TextFormat(
                "%d",
                i
            );


        const float tamanho =
            21.0f;


        const Vector2 medida =
            MeasureTextEx(
                fonte,
                rotulo,
                tamanho,
                1.0f
            );


        desenharTexto(
            rotulo,

            posicoes[i].x -
                medida.x / 2.0f,

            posicoes[i].y -
                medida.y / 2.0f,

            tamanho,

            RAYWHITE
        );
    }
}


// ============================================================
// FORMIGAS
// ============================================================

void Visualizer::desenharFormigasAtuais(
    const std::vector<Vector2>& posicoes
) const
{
    const std::vector<Ant>& formigas =
        formigasParaExibir();


    for (
        std::size_t k = 0;
        k < formigas.size();
        ++k
    )
    {
        const bool estaMovendo =
            animandoMovimento &&
            k < origemAnimacao.size() &&
            k < destinoAnimacao.size() &&
            origemAnimacao[k] !=
                destinoAnimacao[k];


        Vector2 pos;


        // durante a animação a formiga fica na aresta
        if (estaMovendo)
        {
            pos =
                posicaoAnimadaFormiga(
                    k,
                    posicoes
                );
        }

        // parada: mostra perto do vértice
        else
        {
            const int vertice =
                formigas[k].atual;


            if (
                vertice < 0 ||
                vertice >=
                    static_cast<int>(
                        posicoes.size()
                    )
            )
            {
                continue;
            }


            int ordem = 0;
            int totalNoVertice = 0;


            for (
                std::size_t j = 0;
                j < formigas.size();
                ++j
            )
            {
                if (
                    formigas[j].atual ==
                    vertice
                )
                {
                    if (j < k)
                    {
                        ++ordem;
                    }

                    ++totalNoVertice;
                }
            }


            const float angulo =
                totalNoVertice > 0
                    ?
                    (
                        2.0f *
                        PI_F *
                        static_cast<float>(
                            ordem
                        )
                    )
                    /
                    static_cast<float>(
                        totalNoVertice
                    )
                    :
                    0.0f;


            const float raioOffset =
                38.0f;


            pos = {
                posicoes[vertice].x +
                    std::cos(angulo) *
                    raioOffset,

                posicoes[vertice].y +
                    std::sin(angulo) *
                    raioOffset
            };
        }


        const bool selecionada =
            k ==
            formigaSelecionada;


        const Color cor =
            selecionada
                ?
                COR_FORMIGA_SELECIONADA
                :
                COR_FORMIGA;


        const float raio =
            selecionada
                ?
                9.0f
                :
                7.0f;


        // halo da formiga selecionada
        if (selecionada)
        {
            DrawCircleV(
                pos,
                13.0f,
                Fade(
                    COR_FORMIGA_SELECIONADA,
                    0.18f
                )
            );
        }


        DrawCircleV(
            pos,
            raio,
            cor
        );


        DrawCircleLines(
            static_cast<int>(pos.x),
            static_cast<int>(pos.y),
            raio,
            selecionada
                ?
                MAROON
                :
                BROWN
        );


        const char* indice =
            TextFormat(
                "%zu",
                k
            );


        desenharTexto(
            indice,
            pos.x + 10.0f,
            pos.y - 8.0f,
            12.0f,
            COR_TEXTO_SECUNDARIO
        );
    }
}


// ============================================================
// FORMATAR CAMINHO
// ============================================================

std::string Visualizer::formatarCaminho(
    const std::vector<int>& caminho,
    std::size_t limite
) const
{
    if (caminho.empty())
    {
        return "--";
    }


    std::ostringstream saida;


    if (caminho.size() <= limite)
    {
        for (
            std::size_t i = 0;
            i < caminho.size();
            ++i
        )
        {
            if (i > 0)
            {
                saida << " -> ";
            }

            saida << caminho[i];
        }

        return saida.str();
    }


    for (
        std::size_t i = 0;
        i < limite - 2;
        ++i
    )
    {
        if (i > 0)
        {
            saida << " -> ";
        }

        saida << caminho[i];
    }


    saida
        << " -> ... -> "
        << caminho.back();


    return saida.str();
}


// ============================================================
// DECISAO DA FORMIGA
// ============================================================

const DecisaoACO*
Visualizer::decisaoDaFormiga(
    std::size_t indice
) const
{
    for (
        const DecisaoACO& decisao :
        aco.getUltimasDecisoes()
    )
    {
        if (
            decisao.indiceFormiga ==
            static_cast<int>(indice)
        )
        {
            return &decisao;
        }
    }

    return nullptr;
}


// ============================================================
// PAINEL
// ============================================================

void Visualizer::desenharPainel() const
{
    const int x =
        largura -
        larguraPainel;


    DrawRectangle(
        x,
        0,
        larguraPainel,
        altura,
        COR_PAINEL
    );


    DrawLine(
        x,
        0,
        x,
        altura,
        COR_BORDA
    );


    desenharTexto(
        "ACO - TSP",
        x + 24,
        20,
        29,
        COR_TEXTO
    );


    // ========================================================
    // STATUS
    // ========================================================

    const char* status;
    Color corStatus;


    if (aco.terminou())
    {
        status = "CONCLUIDO";
        corStatus = DARKGREEN;
    }
    else if (animandoMovimento)
    {
        status = "ANIMANDO";
        corStatus = COR_VERTICE;
    }
    else if (pausado)
    {
        status = "PAUSADO";
        corStatus = COR_MELHOR_ITERACAO;
    }
    else
    {
        status = "EXECUTANDO";
        corStatus = COR_VERTICE;
    }


    DrawRectangleRounded(
        {
            static_cast<float>(x + 24),
            59.0f,
            128.0f,
            27.0f
        },
        0.45f,
        8,
        Fade(
            corStatus,
            0.16f
        )
    );


    desenharTexto(
        status,
        x + 35,
        65,
        15,
        corStatus
    );


    desenharTexto(
        TextFormat(
            "Fase: %s",
            aco.getNomeFase()
        ),
        x + 170,
        65,
        14,
        COR_TEXTO_SECUNDARIO
    );


    // ========================================================
    // PROGRESSO
    // ========================================================

    desenharTexto(
        "Progresso",
        x + 24,
        105,
        18,
        COR_TEXTO_SECUNDARIO
    );


    desenharTexto(
        TextFormat(
            "Iteracao %d / %d",
            aco.getIteracaoAtual(),
            aco.getQuantidadeIteracoes()
        ),
        x + 24,
        130,
        20,
        COR_TEXTO
    );


    desenharTexto(
        TextFormat(
            "Passo %d / %d",
            std::min(
                aco.getPassoAtual(),
                aco.getPassosPorIteracao()
            ),
            aco.getPassosPorIteracao()
        ),
        x + 205,
        132,
        16,
        COR_TEXTO_SECUNDARIO
    );


    const float progresso =
        aco.getQuantidadeIteracoes() > 0
            ?
            static_cast<float>(
                aco.getIteracoesConcluidas()
            )
            /
            static_cast<float>(
                aco.getQuantidadeIteracoes()
            )
            :
            0.0f;


    DrawRectangleRounded(
        {
            static_cast<float>(x + 24),
            161.0f,
            322.0f,
            10.0f
        },
        0.5f,
        8,
        Fade(
            GRAY,
            0.20f
        )
    );


    DrawRectangleRounded(
        {
            static_cast<float>(x + 24),
            161.0f,

            322.0f *
                limitar(
                    progresso,
                    0.0f,
                    1.0f
                ),

            10.0f
        },
        0.5f,
        8,
        COR_VERTICE
    );


    desenharTexto(
        TextFormat(
            "Formigas: %d",
            aco.getQuantidadeFormigas()
        ),
        x + 24,
        183,
        16,
        COR_TEXTO_SECUNDARIO
    );


    DrawLine(
        x + 24,
        211,
        largura - 24,
        211,
        COR_BORDA
    );


    // ========================================================
    // RESULTADOS
    // ========================================================

    desenharTexto(
        "Resultados",
        x + 24,
        226,
        19,
        COR_TEXTO
    );


    if (
        std::isfinite(
            aco.getMelhorCustoIteracao()
        )
    )
    {
        desenharTexto(
            TextFormat(
                "Melhor da iteracao: %.2f",
                aco.getMelhorCustoIteracao()
            ),
            x + 24,
            255,
            17,
            COR_MELHOR_ITERACAO
        );
    }
    else
    {
        desenharTexto(
            "Melhor da iteracao: --",
            x + 24,
            255,
            17,
            COR_TEXTO_SECUNDARIO
        );
    }


    if (
        std::isfinite(
            aco.getMelhorCusto()
        )
    )
    {
        desenharTexto(
            TextFormat(
                "Melhor global: %.2f",
                aco.getMelhorCusto()
            ),
            x + 24,
            282,
            18,
            COR_MELHOR_GLOBAL
        );
    }
    else
    {
        desenharTexto(
            "Melhor global: --",
            x + 24,
            282,
            18,
            COR_TEXTO_SECUNDARIO
        );
    }


    desenharTexto(
        "Rota global",
        x + 24,
        312,
        15,
        COR_TEXTO_SECUNDARIO
    );


    const std::string rotaGlobal =
        formatarCaminho(
            aco.getMelhorCaminho(),
            8
        );


    desenharTexto(
        rotaGlobal.c_str(),
        x + 24,
        334,
        15,
        COR_TEXTO
    );


    DrawLine(
        x + 24,
        363,
        largura - 24,
        363,
        COR_BORDA
    );


    // ========================================================
    // FORMIGA SELECIONADA
    // ========================================================

    desenharTexto(
        "Formiga selecionada",
        x + 24,
        378,
        19,
        COR_TEXTO
    );


    const std::vector<Ant>& formigas =
        formigasParaExibir();


    if (
        !formigas.empty() &&
        formigaSelecionada <
            formigas.size()
    )
    {
        const Ant& ant =
            formigas[
                formigaSelecionada
            ];


        const bool selecionadaMovendo =
            animandoMovimento &&
            formigaSelecionada <
                origemAnimacao.size() &&
            formigaSelecionada <
                destinoAnimacao.size() &&
            origemAnimacao[
                formigaSelecionada
            ]
            !=
            destinoAnimacao[
                formigaSelecionada
            ];


        if (selecionadaMovendo)
        {
            desenharTexto(
                TextFormat(
                    "#%zu   movimento: %d -> %d   custo: %.1f",
                    formigaSelecionada,
                    origemAnimacao[
                        formigaSelecionada
                    ],
                    destinoAnimacao[
                        formigaSelecionada
                    ],
                    ant.custo
                ),
                x + 24,
                408,
                14,
                COR_MELHOR_ITERACAO
            );
        }
        else
        {
            desenharTexto(
                TextFormat(
                    "#%zu   atual: %d   custo parcial: %.1f",
                    formigaSelecionada,
                    ant.atual,
                    ant.custo
                ),
                x + 24,
                408,
                14,
                COR_TEXTO_SECUNDARIO
            );
        }


        const std::string rotaAtual =
            formatarCaminho(
                ant.caminho,
                7
            );


        desenharTexto(
            rotaAtual.c_str(),
            x + 24,
            431,
            15,
            COR_TEXTO
        );


        const DecisaoACO* decisao =
            decisaoDaFormiga(
                formigaSelecionada
            );


        if (
            decisao != nullptr &&
            !decisao->candidatos.empty()
        )
        {
            desenharTexto(
                TextFormat(
                    "Sorteio: %.3f   escolhido: %d",
                    decisao->sorteio,
                    decisao->destino
                ),
                x + 24,
                458,
                15,
                COR_TEXTO_SECUNDARIO
            );


            desenharTexto(
                "Candidatos",
                x + 24,
                485,
                16,
                COR_TEXTO
            );


            const std::size_t maxMostrar =
                std::min<std::size_t>(
                    5,
                    decisao
                        ->candidatos
                        .size()
                );


            for (
                std::size_t i = 0;
                i < maxMostrar;
                ++i
            )
            {
                const CandidatoACO& candidato =
                    decisao
                        ->candidatos[i];


                const bool escolhido =
                    candidato.vertice ==
                    decisao->destino;


                desenharTexto(
                    TextFormat(
                        "%s %d   P = %.4f",
                        escolhido
                            ? ">"
                            : " ",
                        candidato.vertice,
                        candidato.probabilidade
                    ),

                    x + 34,

                    509 +
                        static_cast<int>(
                            i
                        ) * 21,

                    14,

                    escolhido
                        ?
                        COR_MELHOR_ITERACAO
                        :
                        COR_TEXTO_SECUNDARIO
                );
            }
        }
        else
        {
            desenharTexto(
                "Aguardando proxima decisao...",
                x + 24,
                461,
                15,
                COR_TEXTO_SECUNDARIO
            );
        }
    }
    else
    {
        desenharTexto(
            "Nenhuma formiga disponivel.",
            x + 24,
            408,
            15,
            COR_TEXTO_SECUNDARIO
        );
    }


    DrawLine(
        x + 24,
        625,
        largura - 24,
        625,
        COR_BORDA
    );


    // ========================================================
    // PARAMETROS
    // ========================================================

    desenharTexto(
        TextFormat(
            "a %.1f  b %.1f  sigma %.2f  Q %.1f",
            aco.getAlpha(),
            aco.getBeta(),
            aco.getSigma(),
            aco.getQ()
        ),
        x + 24,
        637,
        14,
        COR_TEXTO_SECUNDARIO
    );


    desenharTexto(
        TextFormat(
            "movimento %.2fs   pausa %.2fs",
            duracaoMovimento,
            intervaloPasso
        ),
        x + 24,
        658,
        13,
        COR_TEXTO_SECUNDARIO
    );


    // ========================================================
    // CONTROLES
    // ========================================================

    desenharTexto(
        "Controles",
        x + 24,
        685,
        18,
        COR_TEXTO
    );


    desenharTexto(
        "SPACE  pausar / continuar",
        x + 24,
        710,
        14,
        COR_TEXTO_SECUNDARIO
    );


    desenharTexto(
        "N      proximo passo",
        x + 24,
        730,
        14,
        COR_TEXTO_SECUNDARIO
    );


    desenharTexto(
        "LEFT/RIGHT  trocar formiga",
        x + 24,
        750,
        14,
        COR_TEXTO_SECUNDARIO
    );


    desenharTexto(
        "UP/DOWN     velocidade",
        x + 24,
        770,
        14,
        COR_TEXTO_SECUNDARIO
    );


    // ========================================================
    // LEGENDA
    // ========================================================

    const int legendaY =
        altura - 37;


    DrawLineEx(
        {
            28.0f,
            static_cast<float>(legendaY)
        },
        {
            78.0f,
            static_cast<float>(legendaY)
        },
        6.0f,
        COR_MELHOR_GLOBAL
    );


    desenharTexto(
        "melhor global",
        87,
        legendaY - 9,
        14,
        COR_TEXTO_SECUNDARIO
    );


    DrawLineEx(
        {
            212.0f,
            static_cast<float>(legendaY)
        },
        {
            262.0f,
            static_cast<float>(legendaY)
        },
        2.5f,
        COR_MELHOR_ITERACAO
    );


    desenharTexto(
        "melhor da iteracao",
        271,
        legendaY - 9,
        14,
        COR_TEXTO_SECUNDARIO
    );


    DrawLineEx(
        {
            445.0f,
            static_cast<float>(legendaY)
        },
        {
            495.0f,
            static_cast<float>(legendaY)
        },
        4.5f,
        Fade(
            COR_FEROMONIO,
            0.78f
        )
    );


    desenharTexto(
        "feromonio",
        504,
        legendaY - 9,
        14,
        COR_TEXTO_SECUNDARIO
    );
}


// ============================================================
// EXECUTAR PASSO NORMAL
// ============================================================

bool Visualizer::executarUmPasso()
{
    if (aco.terminou())
    {
        return false;
    }

    return aco.executarPasso();
}


// ============================================================
// INICIAR PASSO COM ANIMACAO
// ============================================================

bool Visualizer::iniciarPassoAnimado()
{
    if (
        animandoMovimento ||
        aco.terminou()
    )
    {
        return false;
    }


    // guarda o tamanho anterior dos caminhos
    const std::vector<Ant>& antes =
        aco.getFormigasAtuais();


    std::vector<std::size_t>
        tamanhosAntes;


    tamanhosAntes.reserve(
        antes.size()
    );


    for (const Ant& ant : antes)
    {
        tamanhosAntes.push_back(
            ant.caminho.size()
        );
    }


    // algoritmo toma as decisões
    if (!aco.executarPasso())
    {
        return false;
    }


    const std::vector<Ant>& depois =
        formigasParaExibir();


    origemAnimacao.assign(
        depois.size(),
        -1
    );


    destinoAnimacao.assign(
        depois.size(),
        -1
    );


    bool houveMovimento =
        false;


    for (
        std::size_t k = 0;
        k < depois.size();
        ++k
    )
    {
        const Ant& ant =
            depois[k];


        bool caminhoCresceu =
            false;


        if (
            k <
            tamanhosAntes.size()
        )
        {
            caminhoCresceu =
                ant.caminho.size() >
                tamanhosAntes[k];
        }
        else
        {
            // primeira movimentação
            // da nova iteração
            caminhoCresceu =
                ant.caminho.size() >= 2;
        }


        if (
            caminhoCresceu &&
            ant.caminho.size() >= 2
        )
        {
            origemAnimacao[k] =
                ant.caminho[
                    ant.caminho.size() - 2
                ];


            destinoAnimacao[k] =
                ant.caminho.back();


            if (
                origemAnimacao[k] !=
                destinoAnimacao[k]
            )
            {
                houveMovimento =
                    true;
            }
        }
        else
        {
            origemAnimacao[k] =
                ant.atual;

            destinoAnimacao[k] =
                ant.atual;
        }
    }


    progressoMovimento =
        0.0f;


    animandoMovimento =
        houveMovimento;


    tempoAteProximoPasso =
        0.0f;


    return true;
}


// ============================================================
// ATUALIZAR ANIMACAO
// ============================================================

void Visualizer::atualizarAnimacao(
    float dt
)
{
    if (!animandoMovimento)
    {
        return;
    }


    progressoMovimento +=
        dt /
        duracaoMovimento;


    if (
        progressoMovimento >=
        1.0f
    )
    {
        progressoMovimento =
            1.0f;

        animandoMovimento =
            false;

        tempoAteProximoPasso =
            0.0f;
    }
}


// ============================================================
// FORMIGA SELECIONADA
// ============================================================

void Visualizer::ajustarFormigaSelecionada()
{
    const std::vector<Ant>& formigas =
        formigasParaExibir();


    if (formigas.empty())
    {
        formigaSelecionada = 0;
        return;
    }


    if (
        formigaSelecionada >=
        formigas.size()
    )
    {
        formigaSelecionada =
            formigas.size() - 1;
    }
}


// ============================================================
// ENTRADA
// ============================================================

void Visualizer::processarEntrada()
{
    // pausa / continua
    if (
        IsKeyPressed(
            KEY_SPACE
        )
    )
    {
        pausado =
            !pausado;

        tempoAteProximoPasso =
            0.0f;
    }


    // próximo passo manual
    if (
        IsKeyPressed(
            KEY_N
        )
    )
    {
        if (!animandoMovimento)
        {
            iniciarPassoAnimado();
        }

        pausado = true;

        tempoAteProximoPasso =
            0.0f;
    }


    // reiniciar
    if (
        IsKeyPressed(
            KEY_R
        )
    )
    {
        aco.reiniciar();

        pausado = true;

        tempoAteProximoPasso =
            0.0f;

        formigaSelecionada =
            0;

        animandoMovimento =
            false;

        progressoMovimento =
            0.0f;

        origemAnimacao.clear();

        destinoAnimacao.clear();
    }


    // mostrar/esconder exploração
    if (
        IsKeyPressed(
            KEY_E
        )
    )
    {
        mostrarExploracao =
            !mostrarExploracao;
    }


    // ========================================================
    // VELOCIDADE
    // ========================================================

    // mais rápido
    if (
        IsKeyPressed(
            KEY_UP
        )
    )
    {
        duracaoMovimento =
            limitar(
                duracaoMovimento -
                    0.15f,
                0.20f,
                4.0f
            );


        intervaloPasso =
            limitar(
                intervaloPasso -
                    0.10f,
                0.10f,
                2.0f
            );
    }


    // mais lento
    if (
        IsKeyPressed(
            KEY_DOWN
        )
    )
    {
        duracaoMovimento =
            limitar(
                duracaoMovimento +
                    0.15f,
                0.20f,
                4.0f
            );


        intervaloPasso =
            limitar(
                intervaloPasso +
                    0.10f,
                0.10f,
                2.0f
            );
    }


    // ========================================================
    // TROCAR FORMIGA
    // ========================================================

    const std::vector<Ant>& formigas =
        formigasParaExibir();


    if (
        !formigas.empty() &&
        IsKeyPressed(
            KEY_RIGHT
        )
    )
    {
        formigaSelecionada =
            (
                formigaSelecionada +
                1
            )
            %
            formigas.size();
    }


    if (
        !formigas.empty() &&
        IsKeyPressed(
            KEY_LEFT
        )
    )
    {
        formigaSelecionada =
            (
                formigaSelecionada +
                formigas.size() -
                1
            )
            %
            formigas.size();
    }
}


// ============================================================
// LOOP PRINCIPAL
// ============================================================

void Visualizer::executar()
{
    // precisa ser chamado antes de InitWindow
    SetConfigFlags(
        FLAG_MSAA_4X_HINT |
        FLAG_WINDOW_HIGHDPI
    );


    InitWindow(
        largura,
        altura,
        "ACO - Ant Colony Optimization"
    );


    fonte =
        LoadFontEx(
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            32,
            nullptr,
            0
        );


    SetTextureFilter(
        fonte.texture,
        TEXTURE_FILTER_BILINEAR
    );


    SetTargetFPS(60);


    const std::vector<Vector2> posicoes =
        calcularPosicoes();


    while (!WindowShouldClose())
    {
        const float dt =
            GetFrameTime();


        // --------------------------------------------
        // input
        // --------------------------------------------

        processarEntrada();

        ajustarFormigaSelecionada();


        // --------------------------------------------
        // animação atual continua mesmo pausada
        // --------------------------------------------

        atualizarAnimacao(dt);


        // --------------------------------------------
        // execução automática
        // --------------------------------------------

        if (
            !pausado &&
            !aco.terminou() &&
            !animandoMovimento
        )
        {
            tempoAteProximoPasso +=
                dt;


            if (
                tempoAteProximoPasso >=
                intervaloPasso
            )
            {
                iniciarPassoAnimado();

                tempoAteProximoPasso =
                    0.0f;
            }
        }


        if (aco.terminou())
        {
            pausado = true;
        }


        // --------------------------------------------
        // desenho
        // --------------------------------------------

        BeginDrawing();


        desenharFundo();

        desenharArestas(
            posicoes
        );

        desenharExploracao(
            posicoes
        );

        desenharMelhorGlobal(
            posicoes
        );

        desenharMelhorIteracao(
            posicoes
        );

        desenharVertices(
            posicoes
        );

        desenharFormigasAtuais(
            posicoes
        );

        desenharPainel();


        EndDrawing();
    }


    UnloadFont(fonte);

    CloseWindow();
}