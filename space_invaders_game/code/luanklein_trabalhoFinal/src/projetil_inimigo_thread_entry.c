
#include "defines.h"
#include <projetil_inimigo_thread.h>
#include "gui/finalproject_resources.h"
#include "gui/finalproject_specifications.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

extern int aliens_type1_alive[3][7];
extern volatile int start_game;

// Posição do inimigo 1
extern int enemy1_posy;
extern int enemy1_posx;

// Vetor dos projeteis
//[0]-> Ativo; [1]-> ID; [2] -> PosX; [3] -> PosY
int posProjeteis[3][4] = {{0, ID_PROJETIL_ALIEN_1, 0, 0}, {0, ID_PROJETIL_ALIEN_2, 0, 0}, {0, ID_PROJETIL_ALIEN_3, 0, 0}};
int qtd_proj_active = 0;


void createProj(int posProj);
void deleteProj(int posProj);

/* ProjetilInimigo Thread entry function */
void projetil_inimigo_thread_entry(void)
{
    int vel_proj = 2;
    srand(time(NULL));

    GX_ICON_BUTTON * p_proj = NULL;
    ssp_err_t err;
    /* TODO: add your own code here */

    while (!start_game)
    {
        tx_thread_sleep (10);
    }
    int prob ;
    while (1)
    {
        // Geração de Projéteis dos inimigos
        // Verifica se tem menos de 3 projéteis ativos
        if (qtd_proj_active < 3)
        {
            // Gera uma porcentagem com 75%
            prob = rand() % 100;
            if (prob > 75)
            {
                // Percorre a lista dos projeteis
                for (int i = 0; i < 3; i++)
                {
                    // Se encontrar um espaco vazio
                    if(posProjeteis[i][0] == 0)
                    {
                        // Pede para criar o projetil ali
                        createProj(i);
                        // Verifica se criou mesmo
                        if (posProjeteis[i][0] == 1)
                        {
                            // Coloca o projetil no lugar correto para ser disparado e mostra ele na tela
                            err = (ssp_err_t)gx_widget_find(&window2, (USHORT)posProjeteis[i][1], GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET**)&p_proj);
                            gx_widget_move(p_proj, posProjeteis[i][2], posProjeteis[i][3]);
                            gx_widget_show(p_proj);
                        }
                    }
                }
            }
        }

        // Mostrando os projéteis
        // Percorre a lista de projeteis
        for (int i = 0; i < 3; i++)
        {
            // Verifica se encontrou um projetil ativo
            if(posProjeteis[i][0] != 0)
            {
                // Se sim, move ele para baixo
                err = (ssp_err_t)gx_widget_find(&window2, (USHORT)posProjeteis[i][1], GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET**)&p_proj);
                gx_widget_shift(p_proj, 0, vel_proj, GX_TRUE);
                posProjeteis[i][3] += vel_proj;
                // Verifica se esse projetil já chegou no fim da pagina.
                if (posProjeteis[i][3] >= 320)
                {
                    //Se sim, exclui ele
                    deleteProj(i);
                    gx_widget_hide(p_proj);
                }
            }
        }
        tx_thread_sleep (10);
    }
}

// Funcao para criar um projetil
void createProj(int posProj)
{

    // Sorteia um dos aliens
    int alienX = rand() % 7;
    int alienY = rand() % 3;
    // Verifica se esse alien está vivo
    if (aliens_type1_alive[alienY][alienX] != -1)
    {
        qtd_proj_active += 1;
        // Torna ele ativo
        posProjeteis[posProj][0] = 1;
        // Seta a posição X do projétil
        posProjeteis[posProj][2] = enemy1_posx + (alienX-1)*(SIZE_ALIEN + MARGIN_VISUAL);
        // Seta a posição Y do projétil
        posProjeteis[posProj][3] = enemy1_posy + (alienY-1)*(SIZE_ALIEN);
    }
}

// Funcao para deletar um projetil
void deleteProj(int posProj)
{
    // Verifica se realmente era um projetil ativo
    if (posProjeteis[posProj][0] == 1)
    {
        qtd_proj_active -= 1;
    }
    // Zera todas as variaveis dele
    posProjeteis[posProj][0] = 0;
    posProjeteis[posProj][2] = 0;
    posProjeteis[posProj][3] = 0;
}
