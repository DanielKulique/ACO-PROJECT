ACO Project

Implementação do algoritmo Ant Colony Optimization (ACO) em C++ para resolução do Problema do Caixeiro Viajante (TSP), com visualização interativa em raylib.

Dependências

O projeto requer:

C++17

raylib 6.x

Git

CMake

pkg-config

bibliotecas gráficas do sistema

Linux — Ubuntu / Linux Mint / Debian

1. Instalar compilador e dependências

sudo apt update

sudo apt install -y \
    build-essential \
    git \
    cmake \
    pkg-config \
    libasound2-dev \
    libudev-dev \
    libx11-dev \
    libxrandr-dev \
    libxi-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libxcursor-dev \
    libxinerama-dev

Confirme:

g++ --version

2. Instalar a raylib

cd ~
git clone https://github.com/raysan5/raylib.git
cd raylib

Configure:

cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_EXAMPLES=OFF

Compile:

cmake --build build -j$(nproc)

Instale:

sudo cmake --install build
sudo ldconfig

Verifique:

pkg-config --modversion raylib
pkg-config --cflags --libs raylib
ls /usr/local/include/raylib.h

3. Compilar o projeto

Com o Makefile:

make clean
make
./main

Ou manualmente:

g++ -std=c++17 -Wall -Wextra \
    main.cpp Graph.cpp Ant.cpp ACO.cpp Visualizer.cpp \
    -o main \
    -I/usr/local/include \
    -L/usr/local/lib \
    -lraylib \
    -lGL \
    -lm \
    -lpthread \
    -ldl \
    -lrt \
    -lX11 \
    -lXrandr \
    -lXi \
    -lXcursor \
    -lXinerama

Windows — MSYS2 / MinGW-w64

A forma recomendada para este projeto é usar MSYS2 UCRT64.

1. Instalar o MSYS2

Instale o MSYS2 e abra o terminal:

MSYS2 UCRT64

Use o terminal UCRT64, não o terminal MSYS comum.

2. Atualizar os pacotes

pacman -Syu

Se o terminal pedir para fechar, abra novamente o MSYS2 UCRT64 e execute:

pacman -Syu

3. Instalar compilador, ferramentas e raylib

pacman -S --needed \
    mingw-w64-ucrt-x86_64-gcc \
    mingw-w64-ucrt-x86_64-make \
    mingw-w64-ucrt-x86_64-pkgconf \
    mingw-w64-ucrt-x86_64-raylib \
    git

Verifique:

g++ --version
pkg-config --modversion raylib
pkg-config --cflags --libs raylib

4. Compilar no Windows

No terminal MSYS2 UCRT64:

g++ -std=c++17 -Wall -Wextra \
    main.cpp Graph.cpp Ant.cpp ACO.cpp Visualizer.cpp \
    -o main.exe \
    $(pkg-config --cflags --libs raylib)

Execute:

./main.exe

Se preferir link manual:

g++ -std=c++17 -Wall -Wextra \
    main.cpp Graph.cpp Ant.cpp ACO.cpp Visualizer.cpp \
    -o main.exe \
    -lraylib \
    -lopengl32 \
    -lgdi32 \
    -lwinmm

Controles da visualização

Tecla

Função

SPACE

Pausar / continuar

N

Próximo passo

R

Reiniciar

E

Mostrar / ocultar exploração

LEFT / RIGHT

Trocar formiga selecionada

UP

Aumentar velocidade

DOWN

Diminuir velocidade

Funcionamento

Cada formiga escolhe o próximo vértice considerando:

distância;

feromônio;

alpha;

beta.

Heurística:

η(i,j) = 1 / d(i,j)

Probabilidade proporcional a:

[η(i,j)]^alpha × [τ(i,j)]^beta

Após uma iteração:

evaporação;

depósito de feromônio;

atualização do melhor caminho da iteração;

atualização do melhor caminho global.

Depósito:

Δτ = Q / custo_da_rota

Exemplo atual

graph.peso = {
    { 0, 22, 50, 48, 29},
    {22,  0, 30, 34, 32},
    {50, 30,  0, 22, 23},
    {48, 34, 22,  0, 35},
    {29, 32, 23, 35,  0}
};

Solução de problemas

Linux: raylib.h não encontrado

ls /usr/local/include/raylib.h

Package 'raylib' not found

export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH
pkg-config --modversion raylib

Linux: undefined reference to X...

Use o Makefile do projeto ou inclua:

-lGL -lm -lpthread -ldl -lrt -lX11 -lXrandr -lXi -lXcursor -lXinerama

Windows: raylib.dll was not found

Execute pelo terminal MSYS2 UCRT64 ou adicione ao PATH:

C:\msys64\ucrt64\bin
