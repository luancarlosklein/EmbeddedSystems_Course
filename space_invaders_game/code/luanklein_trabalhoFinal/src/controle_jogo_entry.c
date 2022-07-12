
#include "defines.h"
#include <stdio.h>
#include "controle_jogo.h"
#include "gui/finalproject_resources.h"
#include "gui/finalproject_specifications.h"
#include "main_thread.h"



//  ---------------------------  Variáveis Externas ---------------------------------- //
// Inimigos/Aliens
extern int aliens_type1_alive[3][7];
extern aliens_type[3][7];

// Posição do inimigo 1
extern int enemy1_posy;
extern int enemy1_posx;

// Posição do projétil do jogador
extern int ativo;
extern int projetil_posy;
extern int projetil_posx;

// Posição do chefão
extern int boss_active;
extern int pos_boss_x;
extern int pos_boss_y;

// Posição dos projéteis dos aliens
extern int posProjeteis[3][4];
extern int qtd_proj_active;

// Variavel que decide se o jogo inicio ou não
extern volatile int start_game;

// Posição X da nave do jogador
extern int current_pos;

extern GX_WINDOW_ROOT * p_window_root;
// ----------------------------------------------------------- //

// ----------------- Funções Externas ------------------------ //

// Função que movimenta um elemento para uma posição absoluta
extern void gx_widget_move(GX_WIDGET *widget, GX_VALUE x, GX_VALUE y);

// Função para reiniciar a animação dos inimgos e mudar a velocidade deles
extern void set_animation(int velocity);

// ------------------------------------------------------------------ //


// -------------------------  Variáveis Globais --------------------- //
// Variável para definir se o jogo deve ser reiniciado
int restartGame = 0;

// Variaveis para saber qual alien foi atingido
int pos_x_alien_hit = 0;
int pos_y_alien_hit = 0;

// Nível do jogo
int level_game = 1;

//Quantidade de aliens vivos
int qtd_aliens_alive = 21;

//Posição inicial do alien 1
int enemy1_posy_original;
int enemy1_posx_original;


int ranking[3] = {0,0,0};
//-----------------------------------------------------------------//

//-------------------------- FUNÇÕES ----------------------------//
int end_game(int score);
int check_collision_projetil_humano();
int check_collision_projetil_alien();
void update_score(uint8_t score_string[7]);
void restart_aliens();
static UINT show_window2(GX_WINDOW * p_new, GX_WIDGET * p_widget, bool detach_old);
int check_alien_arrive_down();
//-----------------------------------------------------------------//


/* controle_jogo entry function */
void controle_jogo_entry(void)
{
    // ----- Variaveis locais ------ //
    // Vidas do jogador
    int lifes = 3;
    // Ids para mostrar as vidas na tela
    int ids_lifes[3] = {LIFE_3, LIFE_2, LIFE_1};
    // Pontuação do jogador
    int score = 0;
    // Variavel que ira mostrar o score na tela
    uint8_t score_string[7];
    // Pontuações por tipo de inimigo
    // A ultima posicao indica a pontuação do chefão
    int pont_inimigos[4] = {1, 3, 5, 8};
    // Váriaveis usadas para a identificação de itens na tela
    ssp_err_t err;
    GX_ICON_BUTTON * p_proj = NULL;
    // Indica se um alien foi atingido
    int status_alien_hit = 0;
    // Indica se o jogador foi atingido
    int status_user_hit = 0;
    //-----------------------------------------------------//

    enemy1_posy_original = enemy1_posy;
    enemy1_posx_original = enemy1_posx;

    // Fica em loop esperando o jogo iniciar
    while (!start_game)
    {
        tx_thread_sleep (10);
    }

    // Loop
    while (1)
    {
        // Reinicia as variáveis de verificação de colisão
        status_user_hit = 0;
        status_alien_hit = 0;

        // Verifica se o projétil do jogador está ativo
        if (ativo)
        {
            // Chama a função para verficar se um alien/chefao foi atingido
            status_alien_hit = check_collision_projetil_humano();
            // Verifica se atingiu um alien
            if (status_alien_hit == 1)
            {
                // Se sim, busca o alien atingido através das duas váriaveis: pos_y_alien_hit e pos_x_alien_hit
                err = (ssp_err_t)gx_widget_find(&window2, (USHORT)aliens_type1_alive[pos_y_alien_hit][pos_x_alien_hit], GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET**)&p_proj);
                // Apaga esse alien da tela
                gx_widget_hide(p_proj);
                // Tira ele da lista de aliens vivos
                aliens_type1_alive[pos_y_alien_hit][pos_x_alien_hit] = -1;
                // Diminui a quantidade de aliens vivos
                qtd_aliens_alive -= 1;
                // Atribui a pontuacao correta
                score += pont_inimigos[pos_y_alien_hit];
                // Atualiza o placar
                sprintf((char*)score_string, "%d", score);
                update_score(score_string);
                // Desativa o projetil do jogador
                ativo = 0;
            }
            // Verifica se o chefão foi atingido
            else if(status_alien_hit == 2)
            {
                // Pega o chefão
                err = (ssp_err_t)gx_widget_find(&window2, (USHORT)ID_CHEFAO, GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET**)&p_proj);
                // Apaga ele da tela
                gx_widget_hide(p_proj);

                // Atualiza a pontuação corretamente
                score += pont_inimigos[3];
                sprintf((char*)score_string, "%d", score);
                update_score(score_string);
                // Desativa o boss
                boss_active = 0;
                // Desativa o projetil
                ativo = 0;
            }
        }

        // Verifica se todos os aliens foram destruidos
        if(qtd_aliens_alive == 0)
        {
            // Se sim, reinicia todoseles
            restart_aliens();
        }


        // Verifica se algum projetil atingiu o jogador
        // Ve se tem algum projetil ativo
        if(qtd_proj_active > 0)
        {
            // Chama a função que faz a verificao
           status_user_hit = check_collision_projetil_alien();
           // Verifica se foi atingido
           if (status_user_hit)
           {
               // Se ele tiver mais que uma vida, apaga uma das vidas dele
               if (lifes > 1)
               {
                   err = (ssp_err_t)gx_widget_find(&window2, (USHORT)ids_lifes[lifes-1], GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET**)&p_proj);
                   gx_widget_hide(p_proj);
                   lifes -= 1;
               }
               // Senão, ele fica com vidas = 0;
               else
               {
                   lifes = 0;
               }

           }
        }

        // Verifica se o jogador ainda tem vidas e se os inimigos não charam até ele
        if (lifes <= 0 || check_alien_arrive_down())
        {
            // Se ele não tiver mais vidas, encerra o jogo
           // Encerra o jogo
            end_game(score);
            // Se voltar da chamada dessa função, é que o jogo será reiniciado
            // --------------- Restart -------------------------//
            level_game = 1;
            lifes = 3;
            score = 0;
            restart_aliens();
            ativo = 0;

            // Reinicia os projeteis dos inimigos
            for (int i = 0 ; i < 3; i++)
            {
                posProjeteis[i][0] = 0;
                posProjeteis[i][2] = 0;
                posProjeteis[i][3] = 0;
            }
            qtd_proj_active = 0;

            // Mostra as vidas de volta
            for (int i = 0; i< 3; i++)
            {
                err = (ssp_err_t)gx_widget_find(&window2, (USHORT)ids_lifes[i], GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET**)&p_proj);
                gx_widget_show(p_proj);
            }
        }
        gx_system_canvas_refresh();
        tx_thread_sleep (1);
    }
}

// Função para verificar colisão entre o projétil humano e os aliens (inimigos e chefão)
int check_collision_projetil_humano()
{
    // Verifica se acertou o chefão
    if (boss_active)
    {
        // Verifica no eixo Y
        if(projetil_posy <= (pos_boss_y+SIZE_BOSS_Y) && pos_boss_y >= pos_boss_y)
        {
            // Verifica no eixo X
            if (projetil_posx >= pos_boss_x && projetil_posx <= (pos_boss_x+SIZE_BOSS_X)  )
            {
                return 2;
            }
        }
    }

    // Definições
    int pos_aux_y = enemy1_posy;
    int pos_aux_x = enemy1_posx;
    int colision = 0;

    // Loop para percorrer a lista de inimigos
    // Percorre as linhas
    for (int i = 0; i < QTD_ENEMIES_LINHAS && colision == 0; i++)
    {
        // Começa sempre percorrendo da esquerda para a direita
        pos_aux_x = enemy1_posx;
        // Soma i-1 para pegar a parte debaixo dos aliens
        pos_aux_y = enemy1_posy -(i-1)*SIZE_ALIEN;
        // Percorre as colunas
        for (int j = 0; j < QTD_ENEMIES_COLUNAS && colision == 0; j++)
        {
            // Vai aumentado para pegar a posição em x dos aliens
            pos_aux_x = enemy1_posx + (j)*(SIZE_ALIEN + MARGIN_VISUAL);
            // Verifica se o alien está ativo (vivo)
            if (aliens_type1_alive[i][j] != -1)
            {
                // Verifica se haveria uma colisão no eixo Y
                if(projetil_posy <= pos_aux_y && projetil_posy >= (pos_aux_y-SIZE_ALIEN))
                {
                    // Verifica se haveria colisao no eixo X
                    if (( projetil_posx >= (pos_aux_x) && projetil_posx <= (pos_aux_x + SIZE_ALIEN)) ||
                       ((projetil_posx+SIZE_PROJETIL_X) >= (pos_aux_x) && (projetil_posx+SIZE_PROJETIL_X) <= (pos_aux_x + SIZE_ALIEN)))
                    {
                        // Se sim, avisa que houve uma colisão
                        colision = 1;
                        pos_x_alien_hit = j;
                        pos_y_alien_hit = i;
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

// Função para a verificação de colisão de um projetil de alien com o jogador
int check_collision_projetil_alien()
{
    ssp_err_t err;
    GX_ICON_BUTTON * p_proj_ini = NULL;
    // Percorre a lista de projeteis
    for (int i = 0; i < 3; i++)
    {
        // Verifica se é um projétil ativo
        if (posProjeteis[i][0] !=  0)
        {
            // Faz a comparação no eixo Y
            if (POS_USER_Y <= posProjeteis[i][3])
            {
                // Faz a comparação no eixo X
                if ( (current_pos - HALF_SIZE) <= posProjeteis[i][2] && (current_pos + HALF_SIZE) >= posProjeteis[i][2])
                {
                    // Apaga o projetil, e retorna 1
                    err = (ssp_err_t)gx_widget_find(&window2, (USHORT)posProjeteis[i][1], GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET**)&p_proj_ini);
                    gx_widget_hide(p_proj_ini);
                    posProjeteis[i][0] = 0;
                    posProjeteis[i][2] = 0;
                    posProjeteis[i][3] = 0;
                    qtd_proj_active -= 1;
                    return 1;
                }

            }

        }
    }
    return 0;
}


// Função para atualizar a pontuação do jogador na tela
void update_score(uint8_t score_string[7])
{
    GX_PROMPT *p_score = NULL;
    ssp_err_t err;
    //-------------------- Atualizar placar -----------------
    err = (ssp_err_t)gx_widget_find(&window2, (USHORT)SCORE, GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET**)&p_score);
    gx_prompt_text_set(p_score, (GX_CHAR*)score_string);
    gx_system_canvas_refresh();
    //------------------------------------------------------------
}


static UINT show_window2(GX_WINDOW * p_new, GX_WIDGET * p_widget, bool detach_old)
{
    UINT err = GX_SUCCESS;

    if (!p_new->gx_widget_parent)
    {
        err = gx_widget_attach(p_window_root, p_new);
    }
    else
    {
        err = gx_widget_show(p_new);
    }

    gx_system_focus_claim(p_new);

    GX_WIDGET * p_old = p_widget;
    if (p_old && detach_old)
    {
        if (p_old != (GX_WIDGET*)p_new)
        {
            gx_widget_detach(p_old);
        }
    }

    return err;
}


int end_game(int score)
{
    int score_copy = score;
    int aux;
    for (int i = 0; i < 3; i++)
    {
        if(ranking[i] < score_copy)
        {
            aux = ranking[i];
            ranking[i] = score_copy;
            score_copy = aux;
        }
    }

    ssp_err_t err;
    GX_BUTTON * p_btn = NULL;

    uint8_t score_string[7];
    err = (ssp_err_t)gx_widget_find(&window3, (USHORT)FINAL_SCORE, GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET**)&p_btn);
    show_window2((GX_WINDOW*)&window3, (GX_WIDGET*)p_btn, true);
    //gx_system_canvas_refresh();

    GX_PROMPT *p_score = NULL;
    sprintf((char*)score_string, "%d", score);

    err = (ssp_err_t)gx_widget_find(&window3, (USHORT)FINAL_SCORE, GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET**)&p_score);
    gx_prompt_text_set(p_score, (GX_CHAR*)score_string);

    GX_PROMPT *p_places = NULL;
    uint8_t score_string2[7];
    sprintf((char*)score_string2, "%d", ranking[0]);
    (ssp_err_t)gx_widget_find(&window3, (USHORT)FIRST_PLACE, GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET**)&p_places);
    gx_prompt_text_set(p_places, (GX_CHAR*)score_string2);
    gx_system_canvas_refresh();

    uint8_t score_string3[7];
    sprintf((char*)score_string3, "%d", ranking[1]);
    (ssp_err_t)gx_widget_find(&window3, (USHORT)SECOND_PLACE, GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET**)&p_places);
    gx_prompt_text_set(p_places, (GX_CHAR*)score_string3);
    gx_system_canvas_refresh();

    uint8_t score_string4[7];
    sprintf((char*)score_string4, "%d", ranking[2]);
    (ssp_err_t)gx_widget_find(&window3, (USHORT)THIRD_PLACE, GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET**)&p_places);
    gx_prompt_text_set(p_places, (GX_CHAR*)score_string4);
    gx_system_canvas_refresh();

    uint8_t score_string5[7];
    sprintf((char*)score_string5, "%d", score);
    (ssp_err_t)gx_widget_find(&window3, (USHORT)TOTAL, GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET**)&p_places);
    gx_prompt_text_set(p_places, (GX_CHAR*)score_string5);
    gx_system_canvas_refresh();


    while (!restartGame)
        {
        tx_thread_sleep (1);

        }

    restartGame = 5;
    return 1;
}


int check_alien_arrive_down()
{
    int posY = enemy1_posy;
    for (int i = 0; i < 3; i++)
    {
        posY = enemy1_posy + i*(SIZE_ALIEN);
        for(int j = 0; j < 7; j++)
        {

            if(aliens_type1_alive[i][j] != -1)
            {
                if (posY > 230)
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}

void restart_aliens()
{

    ssp_err_t err;
    GX_ICON_BUTTON * p_proj = NULL;
    qtd_aliens_alive = 21;
    enemy1_posy = enemy1_posy_original;
    enemy1_posx = enemy1_posx_original;
    int auxX = enemy1_posx_original;
    int auxY = enemy1_posy_original;

    for (int i = 0; i < QTD_ENEMIES_LINHAS; i++)
    {
          auxY = enemy1_posy_original - (i)*(SIZE_ALIEN) ;
                    for (int j = 0; j < QTD_ENEMIES_COLUNAS; j++)
                    {
                        auxX = enemy1_posx_original + (j)*(SIZE_ALIEN + MARGIN_VISUAL);
                        // Reinicia a matriz de aliens vivos
                        aliens_type1_alive[i][j] = aliens_type[i][j];

                        err = (ssp_err_t)gx_widget_find(&window2, (USHORT)aliens_type[i][j], GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET**)&p_proj);
                        gx_widget_move(p_proj, auxX, auxY);
                        gx_widget_show(p_proj);
                        set_animation(level_game*2);
                      }
                 }
}
