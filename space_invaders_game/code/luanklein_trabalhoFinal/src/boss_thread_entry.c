#include "gui/finalproject_resources.h"
#include "gui/finalproject_specifications.h"
#include "boss_thread.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

// Flag para verificar o inicio do jogo
extern volatile int start_game;
// Função que movimenta um elemento para uma posição absoluta
extern void gx_widget_move(GX_WIDGET *widget, GX_VALUE x, GX_VALUE y);

// Váriaveis de controle do Boss
int boss_active = 0;
int pos_boss_x = 0;
int pos_boss_y = 0;

/* Boss Thread entry function */
void boss_thread_entry(void)
{

    GX_ICON_BUTTON * p_boss = NULL;
    int prob;
    //Gera a semente para geração aleatória de valores
    srand(time(NULL));

    // Espera iniciar o jogo
    while (!start_game)
    {
        tx_thread_sleep (10);
    }

    // Loop infinito
    while (1)
    {
        // Gera um número aleatório
        prob = rand() % 100;
        // Se for acime de 80 (20% de chance), inicia o chefão
        if (prob > 80)
        {
            // Ativa o chefão
            boss_active = 1;
            // Seta a posição inicial dele
            pos_boss_y = 57;
            pos_boss_x = 290;
            ssp_err_t err; (ssp_err_t)gx_widget_find(&window2, (USHORT)ID_CHEFAO, GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET**)&p_boss);
            // Move ele para a posição inicial
            gx_widget_move(p_boss, pos_boss_x, pos_boss_y);
            // Torna ele visivel
            gx_widget_show(p_boss);
            gx_system_canvas_refresh();

            // Movimentação do Boss (enquanto não for atingido por um projétil) e não chegar no fim da tela
            while (boss_active && pos_boss_x > -50)
            {
                // Movimenta 1 pixel por vez
                pos_boss_x -= 1;
                gx_widget_shift(p_boss, -1, 0, GX_TRUE);
                tx_thread_sleep (3);
            }
            // Torna o boss invisivel
            gx_widget_hide(p_boss);
            gx_system_canvas_refresh();
            // Desativa ele
            boss_active = 0;
        }
        // Espera 3,5 segundos para realizar esse processo novamente
        tx_thread_sleep (350);
    }
}
