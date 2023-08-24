
#include "src/include/raylib.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MONSTROS 25                       // Define uma quantidade arbitrária de máximo de monstros.
#define VEL_DANO 1                            // Define o atraso entre cada instância de dano que o jogador pode levar + atraso base entre cada movimento dos monstros
#define VEL_JOGADOR  0.1                      // Define o atraso entre cada movimento do jogador.

#define TIRA_MONSTROS 0                       // Tira um numero de monstros da fase para testar quando o jogador é ruim

typedef struct posicao                        // Define a posição (x, y) de dado elemento no mapa.
{
    int x, y;
} POS;

typedef struct tipo_score                     // tipo_score será uma estrutura usada para salvar e ler pontuações
{
 char nome[20];                          // Nome do jogador
 int score;                              // Pontuação do jogador
} SCORE;

typedef struct Jogador                        // Jogador será uma estrutura com todas as informações sobre a sessão atual do jogador
{
    int vidas, pontos, niveis;           // Quantidades de vidas, pontos e níveis passados do jogador
    POS pos;                             // Posição do jogador no mapa
    char orient;                         // Orientação do jogador
    int dificuldade;                     // Dificuldade do jogo
} JOGADOR;

typedef struct Monstro                        // Monstro será uma estrutura para os monstros de cada nível
{
    POS pos;                             // Posição do monstro no mapa
    char orient;                         // Orientação do monstro
} MONSTRO;

// FUNÇÕES DE MAPA/ GERAIS
void carregaFase(char mapa[16][24], JOGADOR jogador, int *telaAtual);                                             // Carrega a próxima fase na pasta 'levels'
bool liberado(int deslocx, int deslocy, char mapa[16][24]);                                                       // Verifica se o terreno está livre para movimento
void inicializaEntidades(char mapa[16][24], JOGADOR *jogador, MONSTRO monstro[MAX_MONSTROS]);                     // Inicializa posições de jogador e monstros
void desenhaMapa(char mapa[16][24], Texture2D chao, Texture2D pedra);                                             // Desenha o background e os obstáculos
void desenhaHUD(JOGADOR jogador);                                                                                 // Desenha o HUD
bool levelClear(MONSTRO monstro[MAX_MONSTROS]);                                                                   // Verifica se o nível foi limpo (sem monstros)
void leNomeNaTela(JOGADOR *jogador, SCORE *score);                                                                // Recebe input do teclado para salvar o nome do jogador
void telaFinal(bool *finalIniciado, int *telaAtual, JOGADOR *jogador, SCORE *scoreAtual, SCORE scoreLeitura[10]); // Finaliza o jogo, resetando variáveis e retornando ao menu

// FUNÇÕES DE JOGADOR
void moveJogadorNaDirecao(char mapa[16][24], JOGADOR *jogador, char dir);                                         // Move o jogador em uma direção
void ataque(JOGADOR *jogador, POS AttPos[3], MONSTRO monstro[MAX_MONSTROS], Texture2D espadas[4], Sound hit);     // Desenha as espadas de ataque e testa se acertou um monstro
void desenhaJogador(JOGADOR jogador, Texture2D jogadorTex[4], float delta_t_dano);                                // Desenha o jogador na tela, se ele tomou dano, o desenha vermelho
void le_pontos(SCORE score[10]);                                                                                  // Lê o arquivo binário de pontuações
void salva_pontos(SCORE score[10], SCORE scoreAtual);                                                             // Sobrescreve o arquivo binário com as pontuações atualizadas

// FUNÇÕES DE MONSTRO
void moveMonstros(char mapa[16][24], MONSTRO monstro[MAX_MONSTROS]);                                              // Move todos os monstros aleatoriamente
void desenhaMonstros(MONSTRO monstro[MAX_MONSTROS], Texture2D monstroTex[4]);                                     // Desenha todos os monstros na tela
bool levaDano(JOGADOR jogador, MONSTRO monstro[MAX_MONSTROS]);                                                    // Verifica se o jogador recebeu dano

int main()
{
    // TELA & MENU
    SetTraceLogLevel(LOG_ERROR); // Faz o console apenas escrever mensagens de erro.
    const int screenWidth = 1200;
    const int screenHeight = 860;
    InitWindow(screenWidth, screenHeight, "ZINF");
    SetTargetFPS(30);
    int telaAtual = 0;          // Define qual tela será exibida. {1 = Iniciar Jogo, 2 = Ver Pontuações, 3 = Dificuldade, 4 = Sair, 5 = GAME OVER, 6 = Pause, 7 = Vitória}
    int selection_hover = 1;    // Define qual das opções do Menu estará em destaque
    bool finalIniciado = false; // Diz se a o jogador pressionou SPACE para entrar na tela final.

    // AUDIO
    InitAudioDevice();                                  // Inicia o dispositivo de audio
    SetMasterVolume(70);                                // Coloca o volume em 70%

    // Carrega músicas e sons especiais usados no jogo
    Sound menu =     LoadSound("audio/Menu.mp3");
    Sound gameplay = LoadSound("audio/Gameplay.mp3");
    Sound gameOver = LoadSound("audio/GameOver.mp3");
    Sound venceu =   LoadSound("audio/LevelClear.wav");
    Sound dano =     LoadSound("audio/Dano.wav");
    Sound hit =      LoadSound("audio/Hit.wav");
    Sound ataqueS =  LoadSound("audio/Ataque.wav");

    // TEXTURAS
    // Carrega texturas do cenário
    Texture2D pedra = LoadTexture("sprites/Obstacle.png");
    Texture2D chao =  LoadTexture("sprites/Ground.png");

    // Carrega texturas de jogador.
    Texture2D jogadorTex[4];
    jogadorTex[0] = LoadTexture("sprites/Link_back.png");
    jogadorTex[1] = LoadTexture("sprites/Link_left.png");
    jogadorTex[2] = LoadTexture("sprites/Link_front.png");
    jogadorTex[3] = LoadTexture("sprites/Link_right.png");

    // Carrega texturas das espadas.
    Texture2D espadasTex[4];
    espadasTex[0] = LoadTexture("sprites/Attack_up.png");
    espadasTex[1] = LoadTexture("sprites/Attack_left.png");
    espadasTex[2] = LoadTexture("sprites/Attack_down.png");
    espadasTex[3] = LoadTexture("sprites/Attack_right.png");

    // Carrega texturas dos monstros.
    Texture2D monstroTex[4];
    monstroTex[0] = LoadTexture("sprites/Enemy_back.png");
    monstroTex[1] = LoadTexture("sprites/Enemy_left.png");
    monstroTex[2] = LoadTexture("sprites/Enemy_front.png");
    monstroTex[3] = LoadTexture("sprites/Enemy_right.png");

    // PONTUAÇÕES
    SCORE scoreAtual={};          // Variável que salvará o score da sessão de jogo atual
    SCORE scoreLeitura[10];       // Matriz que carregará todas as pontuações mostradas na tela de pontuações
    SCORE zero = {"NINGUEM", 0};  // Variavel usada para zerar os pontos
    le_pontos(scoreLeitura);

    // GAMEPLAY
    // Define a matriz do mapa no nível.
    char mapa[16][24];
    bool foiIniciado = false;

    // Inicializa alguns valores para os membros da variável jogador.
    JOGADOR jogador = {
        3, 0, 1, // Vidas, Pontos, Nível
        {0,0},   // Posição {x,y}
        's'      // Orientação
    };

    // Define a dificuldade do jogo (altera velocidade dos monstros e do jogador)
    int dificuldade = 1;

    // Define uma matriz para os monstros da fase
    MONSTRO monstro[MAX_MONSTROS];

    // Define uma matriz de [3] para o ataque pois serão 3 espadas em fileira, portanto, o primeiro índice dita qual das espadas, enquanto o segundo as posições X e Y.
    POS AttPos[3];

    // Define as variáveis de velocidade de movimento e ataque do jogador e dos monstros.
    SetRandomSeed((int)clock());                                                  // Inicializa a seed para da random, usado para a movimentação dos monstros.
    float delta_t_monstros, vel_monstros, t_anterior_monstros = 0;                // Variáveis usadas para definir a velocidade de movimento dos monstros.
    float delta_t_jogador, vel_jogador, t_anterior_jogador = 0;                   // Variáveis usadas para definir a velocidade de movimento do jogador.
    float delta_t_dano, t_anterior_dano = 0;                                      // Variáveis usadas para definir o atraso entre cada dano levado pelo jogador.

    while (!WindowShouldClose())
    {

        BeginDrawing();

            switch(telaAtual)
            {
                // ---------- TELA DE MENU ----------
                case 0:
                {
                    // Uma simples tela de menu: aqui o jogador poderá escolher diferentes opções dos menus.

                    ClearBackground(BLACK);                                                  // Cor de fundo do Menu.

                    if(IsSoundPlaying(menu)==false)                                          // Muda a música para a do Menu
                    {
                        StopSound(gameOver);
                        StopSound(venceu);
                        PlaySound(menu);
                    }

                    Vector2 titulo = {450,130};                                              // Posição do Título no Menu.
                    DrawTextureEx(espadasTex[1],titulo ,0.0, 5.0, WHITE);                    // Desenha a espada sob o título.
                    DrawText("ZINF",450,100,100,WHITE);                                      // Desenha o título do jogo.

                    // Desenha cada opção do menu em cinza
                    DrawText("Novo Jogo",100,400,70,GRAY);
                    DrawText("Dificuldade",100,500,70,GRAY);
                    DrawText("Pontuação",100,600,70,GRAY);
                    DrawText("Sair",100,700,70,GRAY);

                    // Instruções de como jogar desenhadas na tela
                    DrawRectangle(730,400,400,300, DARKGRAY);
                    DrawText("Como jogar:\nMovimento: W, A, S, D\nAtaque: K\nPause: P\nSair: ESC",750,420,35,WHITE);

                    // Desenha a opção em branco caso ela esteja em destaque para seleção.
                    switch(selection_hover)
                    {
                        case 1: DrawText("Novo Jogo",100,400,70,WHITE);break;
                        case 2: DrawText("Dificuldade",100,500,70,WHITE);break;
                        case 3: DrawText("Pontuação",100,600,70,WHITE);break;
                        case 4: DrawText("Sair",100,700,70,WHITE);break;
                    }

                    // Itera entre as opções do menu com W e S. Seleciona a opção com ENTER.
                    if(IsKeyPressed(KEY_S))
                        if(selection_hover<4)
                            selection_hover++;
                        else
                            selection_hover=1;

                    if(IsKeyPressed(KEY_W))
                        if(selection_hover>1)
                            selection_hover--;
                        else
                            selection_hover=4;

                    DrawText("Pressione W ou S para alternar e ENTER para selecionar", 50, 830, 30, WHITE);

                    if(IsKeyPressed(KEY_ENTER))
                        telaAtual = selection_hover;
                } break;

                // ------------------------------------ TELA DE GAMEPLAY ------------------------------------
                case 1:
                {
                    // A tela de gameplay irá carregar a fase com a função carregaFase e inicializar as entidades (monstros e jogador)
                    // com a função inicializaEntidades.
                    // A cada frame, primeiro os desenhos de cenário são atuallizados (pedras e chão), em seguida os desenhos de entidades
                    // são colocados (monstros e jogador). Os monstros irão se mover a cada determinada quantidade de tempo, definida por
                    // vel_monstros, utilizando as variáveis delta_t_monstros e t_anterior_monstros.
                    // O jogador se moverá de maneira similar, porém apenas com input do teclado, utilizando as variáveis vel_jogador, delta_t_jogador
                    // e t_anterior_jogador.
                    // O dano ao jogador será testado pela função levaDano. As variáveis delta_t_dano, t_anterior_dano e vel_monstro irão criar um
                    // atraso entre cada instância de dano.
                    // A função ataque irá checar se algum monstro foi pego pelas espadas do jogador e calcular a pontuação recebida baseado na
                    // dificuldade do jogo. Sempre que o jogador atacar, o jogo testa se a fase foi concluida com a função levelClear.

                    // Muda a música para a da gameplay
                    if(IsSoundPlaying(gameplay)==false)
                    {
                        StopSound(menu);
                        PlaySound(gameplay);
                    }

                    // INICIALIZA ENTIDADES
                    if(foiIniciado == false)
                    {
                        carregaFase(mapa, jogador, &telaAtual);       // Carrega objetos fixos do mapa (chão e pedras)
                        inicializaEntidades(mapa, &jogador, monstro); // Carrega entidades animadas no mapa (jogador e monstros)
                        jogador.dificuldade = dificuldade;            // Cada grau de dificuldade aumenta a quantidade de pontos recebida
                        vel_monstros = 0.4 - 0.075*dificuldade;       // Aumenta a velocidade dos monstros baseado na dificuldade
                        vel_jogador = 0.1 + 0.015*dificuldade;        // Reduz levemente a velocidade do jogador baseado na dificuldade
                        foiIniciado = true;
                    }

                    // DESENHA MAPA
                    desenhaMapa(mapa, chao, pedra);

                    // CONTROLES DE MOVIMENTO DO JOGADOR
                    delta_t_jogador = GetTime() - t_anterior_jogador;
                    if(delta_t_jogador > VEL_JOGADOR)
                    {
                        if(IsKeyDown(KEY_W))
                            moveJogadorNaDirecao(mapa, &jogador, 'w');
                        else if(IsKeyDown(KEY_A))
                            moveJogadorNaDirecao(mapa, &jogador, 'a');
                        else if(IsKeyDown(KEY_S))
                            moveJogadorNaDirecao(mapa, &jogador, 's');
                        else if(IsKeyDown(KEY_D))
                            moveJogadorNaDirecao(mapa, &jogador, 'd');

                        t_anterior_jogador=GetTime();
                        delta_t_jogador=0;
                    }

                    // ATAQUE DO JOGADOR
                    // A tecla K ataca 3 blocos na direção que o jogador está orientado
                    if(IsKeyDown(KEY_K))
                    {
                        if(IsSoundPlaying(ataqueS)==false)
                        {
                            PlaySound(ataqueS);
                            ataque(&jogador, AttPos, monstro, espadasTex, hit);
                        }

                        // A cada ataque, também já testa se o nível foi completado
                        if(levelClear(monstro))
                        {
                            jogador.niveis++;
                            foiIniciado = false;
                        }
                    }

                    // DESENHO DO JOGADOR NA TELA
                    // Se o jogador levou dano, sua textura fica avermelhada
                    desenhaJogador(jogador, jogadorTex, delta_t_dano);

                    // TESTA SE HÁ DANO DOS MONSTROS NO JOGADOR
                    delta_t_dano = GetTime() - t_anterior_dano;
                    if(delta_t_dano > VEL_DANO && levaDano(jogador, monstro))
                    {
                        jogador.vidas--;
                        t_anterior_dano=GetTime();
                        delta_t_dano=0;

                        if(IsSoundPlaying(dano)==false)
                            PlaySound(dano);
                    }

                    // CONTROLES DE MOVIMENTO DOS MONSTROS
                    delta_t_monstros = GetTime() - t_anterior_monstros; // Define delta_t para movimentação dos monstros.
                    if(delta_t_monstros > vel_monstros)
                    {
                        moveMonstros(mapa, monstro);
                        t_anterior_monstros = GetTime();
                        delta_t_monstros = 0;
                    }

                    // DESENHO DE MONSTROS NA TELA
                    desenhaMonstros(monstro, monstroTex);

                    // HUD
                    desenhaHUD(jogador);

                    // MORTE DO JOGADOR
                    if(jogador.vidas<1)
                        telaAtual = 5;

                    // BOTÃO DE PAUSE
                    if(IsKeyPressed(KEY_P))
                        telaAtual = 6;

                } break;

                // ------------------------------------ TELA DE DIFICULDADES ------------------------------------
                case 2:
                {
                    ClearBackground(BLACK);

                    // A dificuldade do jogo irá alterar o valor das seguintes variáveis: vel_monstro e vel_jogador
                    // fazendo com que os monstros demorem menos para se mover e o jogador demore mais, quanto maior
                    // o nível de dificuldade.

                    // Desenha os textos de diferentes opções de dificuldade na tela
                    DrawText("SELECIONE A DIFICULDADE", 100, 50, 50, WHITE);

                    DrawText("Fácil",120,200,70,GRAY);
                    DrawText("Médio",120,300,70,GRAY);
                    DrawText("Difícil",120,400,70,GRAY);
                    DrawText("Impossível",120,500,70,GRAY);
                    DrawText("ATENÇÃO: NÃO TESTADO",120,600,70,GRAY);

                    // Faz a opção selecionada ser desenhada em branco, ficando com highlight
                    switch(dificuldade)
                    {
                        case 1: DrawText("Fácil",120,200,70,WHITE);break;
                        case 2: DrawText("Médio",120,300,70,WHITE);break;
                        case 3: DrawText("Difícil",120,400,70,WHITE);break;
                        case 4: DrawText("Impossível",120,500,70,WHITE);break;
                        case 5: DrawText("ATENÇÃO: NÃO TESTADO",120,600,70,WHITE);break;
                    }

                    // Itera entre as opções do menu com W e S. Seleciona a opção com ENTER.
                    if(IsKeyPressed(KEY_S))
                        if(dificuldade<5)
                            dificuldade++;
                        else
                            dificuldade=1;

                    if(IsKeyPressed(KEY_W))
                        if(dificuldade>1)
                            dificuldade--;
                        else
                            dificuldade=5;

                    DrawText("Pressione W ou S para alternar e ENTER para escolher e voltar ao MENU", 50, 830, 27, WHITE);

                    if(IsKeyPressed(KEY_ENTER))
                        telaAtual=0;

                } break;

                // ------------------------------------ TELA DE PONTUAÇÕES ------------------------------------
                case 3:
                {
                    // A tela de pontuações irá sempre mostrar as pontuações carregadas na variável scoreLeitura pela função
                    // le_pontos.
                    // scoreLeitura será atualizada ao final de cada sessão de jogo com a função salva_pontos, que irá fazer
                    // uma varredura em scoreLeitura e identificar qual a posição do scoreAtual na lista, e atualiza-la de acordo

                    ClearBackground(BLACK);

                    DrawText("PONTUAÇÕES", 300, 50, 80, WHITE);

                    // Printa todas as 10 pontuações salvas
                    for(int i=0; i<10; i++)
                    {
                        DrawText(scoreLeitura[i].nome, 200, i*50+150, 50, WHITE);
                        char pontosTexto[6];
                        sprintf(pontosTexto, "%d",scoreLeitura[i].score);
                        DrawText(pontosTexto, 800, i*50+150, 50, WHITE);
                    }

                    // Opção de zerar os pontos do arquivo
                    if(IsKeyPressed(KEY_Z))
                    {
                        for(int i=0; i<10; i++)
                            scoreLeitura[i] = zero;
                        salva_pontos(scoreLeitura, scoreAtual);
                    }

                    DrawText("Pressione Z para zerar os pontos", 50, 800, 30, WHITE);
                    DrawText("Pressione ENTER para voltar ao MENU", 50, 830, 30, WHITE);

                    if(IsKeyPressed(KEY_ENTER))
                        telaAtual=0;

                };break;

                // ------------------------------------ FECHA O JOGO ------------------------------------
                case 4: EndDrawing(); CloseWindow(); CloseAudioDevice() ; break;

                // ------------------------------------ TELA DE GAME OVER ------------------------------------
                case 5:
                {
                    // A tela de Game Over irá aparecer quando jogador.vidas = 0. Esta tela irá pedir o nome
                    // do jogador e irá salvar seus pontos. Em seguida, irá reiniciar as variáveis da gameplay
                    // e retornar ao Menu

                    ClearBackground(BLACK);
                    if(finalIniciado==false)
                    {
                        // Inicia a tela de fim de jogo
                        DrawText("GAME OVER!", 300, 100, 100, RED);
                        DrawText("Pressione SPACE", 200, 400, 90, WHITE);
                        if(IsKeyPressed(KEY_SPACE))
                            finalIniciado=true;
                    }
                    else
                    {
                        Vector2 enemy = {450,300};                                                // Posição do Monstro no GAME OVER.
                        DrawTextureEx(monstroTex[2], enemy,0.0, 6.0, WHITE);                      // Desenha o Monstro com escala 8x.
                        DrawText("GAME OVER!", 300, 100, 100, RED);                                    // Escreve "GAME OVER".

                        // LE NOME NA TELA
                        leNomeNaTela(&jogador, &scoreAtual);

                        // Atualiza a música da tela
                        if(IsSoundPlaying(gameOver)==false)
                        {
                            StopSound(gameplay);
                            PlaySound(gameOver);
                        }

                        DrawText("Digite seu nome e pressione ENTER para voltar ao MENU", 50, 825, 30, WHITE);
                        if(IsKeyPressed(KEY_ENTER))
                        {
                            telaFinal(&finalIniciado, &telaAtual, &jogador, &scoreAtual, scoreLeitura);
                            foiIniciado=false;
                        }
                    }
                } break;

                // ------------------------------------ TELA DE PAUSE ------------------------------------
                case 6:
                {
                    // A tela de pause pode ser acessada a qualquer momento durante a gameplay
                    // apenas pressionando P.

                    DrawText("PAUSE",400,400,100,BLACK);      // Indica que o jogo está pausado.
                    if(IsKeyPressed(KEY_P))                   // Despausa o jogo.
                        telaAtual = 1;
                } break;

                // ------------------------------------ TELA DE VITÓRIA ------------------------------------
                case 7:
                {
                    // A tela de vitória funciona de maneira similar a tela de Game Over. Porém ela
                    // é acessada quando a função carregaFase não encontra mais arquivos .txt na
                    // pasta levels. Significando que o jogador finalizou o jogo.

                    ClearBackground(DARKGREEN);
                    if(finalIniciado==false)
                    {
                        // Inicia a tela de fim de jogo
                        DrawText("VOCE VENCEU!\nPressione SPACE", 200, 100, 100, WHITE);
                        DrawText(TextFormat("Bonus por (%d) vidas restantes: %d", jogador.vidas, jogador.vidas*dificuldade*25), 200, 400, 50, WHITE);
                        if(IsKeyPressed(KEY_SPACE))
                        {
                            // O jogador ganha alguns pontos baseado em quantas vidas sobraram e em qual dificuldade está
                            jogador.pontos+=jogador.vidas*dificuldade*100;  
                            finalIniciado=true;
                        }

                        // Atualiza a música da tela
                        if(IsSoundPlaying(venceu)==false)
                        {
                            StopSound(gameplay);
                            PlaySound(venceu);
                        }
                    }
                    else
                    {
                        Vector2 link = {450,300};                                                // Posição do Link na tela de vitória
                        DrawTextureEx(jogadorTex[2], link,0.0, 6.0, WHITE);                      // Desenha o Link com escala 6x
                        DrawText("VOCE VENCEU!",200,100,100,WHITE);

                        // LE NOME NA TELA
                        // Permite que o jogador escreva seu nome para leitura e gravamento no arquivo de pontuações
                        leNomeNaTela(&jogador, &scoreAtual);

                        // Quando o jogador pressiona ENTER, sua pontuação é salva e o jogo volta ao menu
                        DrawText("Digite seu nome e pressione ENTER para voltar ao MENU", 50, 825, 30, WHITE);
                        if(IsKeyPressed(KEY_ENTER))
                        {
                            telaFinal(&finalIniciado, &telaAtual, &jogador, &scoreAtual, scoreLeitura);
                            foiIniciado=false;
                        }
                    }
                }
            }
        EndDrawing();

    }

    CloseAudioDevice();   // Desliga o dispositivo de audio
    CloseWindow();        // Fecha a tela

    return 0;
}

// ===============================================
// =================== FUNÇÕES ===================
// ===============================================

// TESTE DE COLISÃO
// Identifica se a posição destino é uma pedra ou uma fronteira do mapa, se for, retorna false.
bool liberado(int deslocx, int deslocy, char mapa[16][24])
{
    bool valor = true;

    if
    (mapa[deslocy][deslocx] == 'P'||
    (deslocy > 15 || deslocy < 0)||
    (deslocx > 23 || deslocx < 0))
        valor = false;

    return valor;
}

// INICIALIZA ENTIDADES
// Esta função recebe a matriz mapa, a struct jogador e a struct monstro, atualizando os valores de posição dessas duas entidades.
void inicializaEntidades(char mapa[16][24], JOGADOR *jogador, MONSTRO monstro[MAX_MONSTROS])
{
    int i, j, k, l;

    for(i=0, k=0; i<16; i++)
        for(j=0; j<24; j++)
            switch(mapa[i][j])
            {
                case 'J': jogador->pos.x = j; jogador->pos.y = i; break;
                case 'M': monstro[k].pos.x = j; monstro[k].pos.y = i; monstro[k].orient = 's';k++; break;
            }

    for(k-=TIRA_MONSTROS; k<MAX_MONSTROS; k++)
        monstro[k].orient = 'M';

}

// DESENHA MAPA
// Desenha o chão e as pedras do mapa.
void desenhaMapa(char mapa[16][24], Texture2D chao, Texture2D pedra)
{
    int i, j;

    Rectangle rec = {1200,860,1200,800};
    Vector2 vec = {0,60};

    ClearBackground(WHITE);
    DrawTextureRec(chao, rec, vec, DARKGREEN);

    for(i=0; i<16; i++)
        for(j=0; j<24; j++)
            if(mapa[i][j]=='P')
                DrawTexture(pedra, j*50, i*50+60, DARKGREEN);
}

// HUD
// Desenha o Heads-Up Display: Onde são mostradas as informações importantes da sessão (Vidas, Níveis, Pontos).
void desenhaHUD(JOGADOR jogador)
{
    // É necessário converter para char para que o raylib use a função DrawText().
    char vidasTexto[2], niveisTexto[2], pontosTexto[6];

    sprintf(vidasTexto,  "%d",jogador.vidas);
    sprintf(pontosTexto, "%d",jogador.pontos);
    sprintf(niveisTexto, "%d",jogador.niveis);

    DrawRectangle(0, 0, 1200, 60, BLACK);
    DrawText("Vidas:", 50, 10, 45, WHITE);
    DrawText(vidasTexto, 200, 10, 45, WHITE);
    DrawText("Pontos:", 300, 10, 45, WHITE);
    DrawText(pontosTexto, 470, 10, 45, WHITE);
    DrawText("Nivel:", 600, 10, 45, WHITE);
    DrawText(niveisTexto, 740, 10, 45, WHITE);
}

// CONTROLES DE MOVIMENTO DO JOGADOR
// Direcionais W, A, S, D controlam a orientação e movimento. Enquanto SPACE estiver pressionada, apenas a orientação muda.
void moveJogadorNaDirecao(char mapa[16][24], JOGADOR *jogador, char dir)
{
    jogador->orient = dir;
    if(IsKeyUp(KEY_SPACE))
    {
        if(dir=='w')
            if(liberado(jogador->pos.x, jogador->pos.y-1, mapa))
                jogador->pos.y--;
        if(dir=='a')
            if(liberado(jogador->pos.x-1, jogador->pos.y, mapa))
                jogador->pos.x--;
        if(dir=='s')
            if(liberado(jogador->pos.x, jogador->pos.y+1, mapa))
                jogador->pos.y++;
        if(dir=='d')
            if(liberado(jogador->pos.x+1, jogador->pos.y, mapa))
                jogador->pos.x++;
    }
}

// ATAQUE DO JOGADOR
// Desenha as espadas de ataque do jogador, além de testar se acertaram algum monstro
void ataque(JOGADOR *jogador, POS AttPos[3], MONSTRO monstro[MAX_MONSTROS], Texture2D espadasTex[4], Sound hit)
{
    int i, j;
    Texture2D espada;
    switch(jogador->orient)
    {
        case 'w' : espada = espadasTex[0];
        AttPos[0].x=jogador->pos.x; AttPos[0].y=jogador->pos.y-1; // Primeira espada
        AttPos[1].x=jogador->pos.x; AttPos[1].y=jogador->pos.y-2; // Segunda espada
        AttPos[2].x=jogador->pos.x; AttPos[2].y=jogador->pos.y-3; // Terceira espada
        break;
        case 'a' : espada = espadasTex[1];
        AttPos[0].x=jogador->pos.x-1; AttPos[0].y=jogador->pos.y;
        AttPos[1].x=jogador->pos.x-2; AttPos[1].y=jogador->pos.y;
        AttPos[2].x=jogador->pos.x-3; AttPos[2].y=jogador->pos.y;
        break;
        case 's' : espada = espadasTex[2];
        AttPos[0].x=jogador->pos.x; AttPos[0].y=jogador->pos.y+1;
        AttPos[1].x=jogador->pos.x; AttPos[1].y=jogador->pos.y+2;
        AttPos[2].x=jogador->pos.x; AttPos[2].y=jogador->pos.y+3;
        break;
        case 'd' : espada = espadasTex[3];
        AttPos[0].x=jogador->pos.x+1; AttPos[0].y=jogador->pos.y;
        AttPos[1].x=jogador->pos.x+2; AttPos[1].y=jogador->pos.y;
        AttPos[2].x=jogador->pos.x+3; AttPos[2].y=jogador->pos.y;
        break;
    }

    DrawTexture(espada, AttPos[0].x*50, AttPos[0].y*50+60, WHITE);
    DrawTexture(espada, AttPos[1].x*50, AttPos[1].y*50+60, WHITE);
    DrawTexture(espada, AttPos[2].x*50, AttPos[2].y*50+60, WHITE);

    // Testa se os ataques acertaram algum monstro, se sim, ele morre e o jogador ganha pontos
    for(i=0; i<3; i++)
        for(j=0; j<MAX_MONSTROS; j++)
            if(monstro[j].pos.x==AttPos[i].x && monstro[j].pos.y==AttPos[i].y&&monstro[j].orient!='M')
                {
                    monstro[j].orient='M';
                    monstro[j].pos.x = -50;
                    monstro[j].pos.y = -50;
                    jogador->pontos+=80 + 25*jogador->dificuldade;
                    if(IsSoundPlaying(hit)==false)
                            PlaySound(hit);
                }
}

// DESENHO DO JOGADOR NA TELA
// A variável de orientação do jogador define a textura a ser carregada
void desenhaJogador(JOGADOR jogador, Texture2D jogadorTex[4], float delta_t_dano)
{
    Texture2D textura;

    switch(jogador.orient)
    {
        case 'w' : textura = jogadorTex[0];break;
        case 'a' : textura = jogadorTex[1];break;
        case 's' : textura = jogadorTex[2];break;
        case 'd' : textura = jogadorTex[3];break;
    }

    // Se o cooldown de dano está ativo, o jogador fica vermelho, indicando que está invulnerável temporariamente
    if(delta_t_dano > VEL_DANO)
        DrawTexture(textura, (jogador.pos.x*50), (jogador.pos.y*50)+60, WHITE);
    else
        DrawTexture(textura, (jogador.pos.x*50), (jogador.pos.y*50)+60, RED);
}

// CONTROLES DE MOVIMENTO DOS MONSTROS
// Move as posições dos monstros aleatoriamente, além de alterar suas orientações
void moveMonstros(char mapa[16][24], MONSTRO monstro[MAX_MONSTROS])
{
    for(int i=0; i<MAX_MONSTROS; i++)
        if(monstro[i].orient != 'M')
            switch(GetRandomValue(0, 3))
            {
                case 0: monstro[i].orient = 'w';
                if(liberado(monstro[i].pos.x, monstro[i].pos.y-1, mapa))
                    monstro[i].pos.y--;
                break;
                case 1: monstro[i].orient = 'a';
                if(liberado(monstro[i].pos.x-1, monstro[i].pos.y, mapa))
                    monstro[i].pos.x--;
                break;
                case 2: monstro[i].orient = 's';
                if(liberado(monstro[i].pos.x, monstro[i].pos.y+1, mapa))
                    monstro[i].pos.y++;
                break;
                case 3: monstro[i].orient = 'd';
                if(liberado(monstro[i].pos.x+1, monstro[i].pos.y, mapa))
                    monstro[i].pos.x++;
                break;
            }
}

// DESENHO DE MONSTROS NA TELA
// A variável de orientação do monstro define a textura a ser carregada
void desenhaMonstros(MONSTRO monstro[MAX_MONSTROS], Texture2D monstroTex[4])
{
    Texture2D textura;

    for(int i=0; i<MAX_MONSTROS; i++)
        if(monstro[i].orient!='M')
        {
            switch(monstro[i].orient)
            {
                case 'w': textura = monstroTex[0]; break;
                case 'a': textura = monstroTex[1]; break;
                case 's': textura = monstroTex[2]; break;
                case 'd': textura = monstroTex[3]; break;
            }
            DrawTexture(textura, monstro[i].pos.x*50, monstro[i].pos.y*50+60, WHITE);
        }
}

// TESTA SE HÁ DANO DOS MONSTROS NO JOGADOR
// Se o jogador pisou na mesma posição de um monstro não morto, retorna true
bool levaDano(JOGADOR jogador, MONSTRO monstro[MAX_MONSTROS])
{
    bool levaDano = false;
    int i;
    for(i=0; i<MAX_MONSTROS; i++)
        // Se o jogador estiver na mesma posição que o monstro, ele leva dano.
        if((monstro[i].pos.x==jogador.pos.x&&monstro[i].pos.y==jogador.pos.y)&&monstro[i].orient!='M')
            levaDano = true;
    return levaDano;
}

// CARREGAMENTO DE FASE
// Carrega a próxima fase a ser jogada, baseado em jogador.nivel
void carregaFase(char mapa[16][24], JOGADOR jogador, int *telaAtual)
{
    FILE* fptr;
    char ch, nivel[20]="niveis/nivel@.txt";
    int i, j;

    // A variável nivel recebe sempre o texto "levels/level@.txt" com @ sendo o nível atual.
    nivel[12] = jogador.niveis+'0'; // Transforma jogador.niveis em string

    fptr = fopen(nivel, "r");

    // Se não existirem mais níveis, o jogador vence o jogo.
    if(fptr != NULL)
        for(i=0; i<16; i++)
            for(j=0; j<24; j++)
            {
                ch = fgetc(fptr);
                if(ch=='J' || ch=='M' || ch=='B' || ch=='P') // Apenas lê caractéres válidos
                    mapa[i][j]=ch;
                else
                    j--; // Se não for válido (por exemplo " " ou "\n") decrementa o índice
            }
    else
        // É levado a tela de vitória.
        *telaAtual = 7;

    fclose(fptr);
}

// LEVEL CLEAR
// Verifica se não existem mais monstros vivos no nível
bool levelClear(MONSTRO monstro[MAX_MONSTROS])
{
    bool clear = true;
    int i;
    for(i=0; i<MAX_MONSTROS; i++)
        if(monstro[i].orient != 'M')
        {
            clear = false;
            break;
        }
    return clear;
}

// LE NOME NA TELA
// Permite que o jogador digite seu nome e salve em Score. Caso pressione BACKSPACE, o nome será resetado
void leNomeNaTela(JOGADOR *jogador, SCORE *score)
{
    char ch;
    char pontosTexto[6];

    // Verifica sempre que uma tecla é pressionada
    if(IsKeyUp(KEY_SPACE))
    {
        ch=GetCharPressed();
        strncat(score->nome, &ch, 20);
    }

    // Verifica se BACKSPACE foi pressionada e apaga o nome, caso seja
    if(IsKeyPressed(KEY_BACKSPACE))
    {
        strcpy(score->nome, "");
    }

    // Desenha os textos na tela
    sprintf(pontosTexto, "%d",jogador->pontos);
    DrawText("Pontos:", 50, 750, 70, WHITE);
    DrawText(pontosTexto, 350, 750, 70, WHITE);
    DrawText("Seu nome:", 50, 680, 70, WHITE);
    DrawText(score->nome, 410, 680, 70, WHITE);
}

// SALVA PONTUAÇÕES
// Salva o arquivo de pontuações em ordem decrescente de pontos, além de acrescentar alguma pontuação nova se for maior
void salva_pontos(SCORE score[10], SCORE scoreAtual)
{
    // Aqui verificamos em qual posição o novo score é salvo
    int posicao = 11;

    // Checa em qual posição a pontuação atual será salva
    for(int i=0; i<10; i++)
        if(scoreAtual.score>score[i].score)
        {
            posicao=i;
            break;
        }

    // Movimenta todas as pontuações e insere a nova
    if(posicao<11)
    {
        for(int i=9; i>=posicao; i--)
        {
            score[i+1]=score[i];
        }
        score[posicao]=scoreAtual;
    }

    // Aqui salvamos a lista de pontos no arquivo.
    int i;
    SCORE tmp;
    FILE *fptr;

    fptr = fopen("ranking.bin", "wb");

    for(i=0; i<10; i++)
    {
        tmp = score[i];
        fwrite(&tmp, sizeof(SCORE), 1, fptr);
    }

    fclose(fptr);
}

// LÊ PONTUAÇÕES
// Abre o arquivo para ler as pontuações.
void le_pontos(SCORE score[10])
{
    int i;
    SCORE tmp;
    FILE *fptr;

    fptr = fopen("ranking.bin", "rb");

    if (fptr != NULL)
        for(i=0; i<10; i++)
            {
                fread(&tmp, sizeof(SCORE), 1, fptr);
                score[i]=tmp;
            }

    fclose(fptr);
}

// TELA FINAL
// Salva os pontos e reinicia as variáveis de jogador e score.
void telaFinal(bool *finalIniciado, int *telaAtual, JOGADOR *jogador, SCORE *scoreAtual, SCORE scoreLeitura[10])
{
    scoreAtual->score = jogador->pontos;
    salva_pontos(scoreLeitura, *scoreAtual);
    SCORE novo={};
    *scoreAtual = novo;
    jogador->vidas = 3;
    jogador->pontos = 0;
    jogador->niveis = 1;
    *finalIniciado = false;
    *telaAtual = 0;
}