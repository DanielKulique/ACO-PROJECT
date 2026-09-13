# ACO Project - TSP com Raylib

Implementacao didatica de **Ant Colony Optimization (ACO)** em C++ para o **Problema do Caixeiro Viajante (TSP)**, com visualizacao em tempo real usando raylib.

## O que mudou nesta versao

A interface nao recebe mais uma iteracao inteira pronta. Durante a fase `CONSTRUINDO`, cada chamada de `executarPasso()` faz **cada formiga andar no maximo uma aresta**. Assim e possivel observar a construcao gradual das rotas.

Fluxo de uma iteracao:

```text
CONSTRUINDO
  -> passo 1 das formigas
  -> passo 2 das formigas
  -> ...
FECHANDO_CICLOS
  -> retorno ao vertice inicial
ATUALIZANDO_FEROMONIO
  -> evaporacao
  -> deposito
proxima iteracao
```

A interface tambem mostra, para uma formiga selecionada, o sorteio e as probabilidades dos vertices candidatos da ultima decisao.

## Arquivos

```text
.
├── ACO.cpp
├── ACO.hpp
├── Ant.cpp
├── Ant.hpp
├── Graph.cpp
├── Graph.hpp
├── Visualizer.cpp
├── Visualizer.hpp
├── main.cpp
├── Makefile
└── README.md
```

## Controles

- `SPACE`: pausar / continuar
- `N`: executar um passo manualmente
- `R`: reiniciar o ACO
- `E`: mostrar / esconder caminhos de exploracao
- `LEFT` / `RIGHT`: trocar a formiga selecionada
- `UP`: acelerar
- `DOWN`: desacelerar

A aplicacao inicia **pausada** para facilitar o estudo. Pressione `N` para acompanhar passo a passo ou `SPACE` para executar automaticamente.

## Compilacao

Com raylib instalado em `/usr/local`:

```bash
make clean
make
./main
```

ou:

```bash
make run
```

## Parametros atuais

No `main.cpp`:

- 5 vertices
- 5 formigas (uma por vertice, para facilitar a visualizacao)
- 100 iteracoes
- alpha = 1.0
- beta = 1.0
- sigma = 0.01
- Q = 10.0
- feromonio inicial = 0.1

Com apenas 5 vertices, ainda e perfeitamente possivel encontrar a rota otima na primeira iteracao. Isso nao e um erro: o espaco de busca e pequeno. A diferenca desta versao e que agora e possivel ver **como** cada rota foi construida antes de o melhor global aparecer.
