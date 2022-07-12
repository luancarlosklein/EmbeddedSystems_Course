
#include "defines.h"

#include "gui/finalproject_resources.h"
#include "gui/finalproject_specifications.h"
#include "main_thread.h"
#include "aliens_thread.h"

// Flag que define a execução do jogo
volatile int start_game = 0;

// Define os aliens
int aliens_type[3][7] = {{ID_ALIEN1, ID_ALIEN2, ID_ALIEN3, ID_ALIEN4, ID_ALIEN5, ID_ALIEN6, ID_ALIEN7},
                       {ID_ALIEN21, ID_ALIEN22, ID_ALIEN23, ID_ALIEN24, ID_ALIEN25, ID_ALIEN26, ID_ALIEN27},
                       {ID_ALIEN31, ID_ALIEN32, ID_ALIEN33, ID_ALIEN34, ID_ALIEN35, ID_ALIEN36, ID_ALIEN37}};

int aliens_type1_alive[3][7] = {{ID_ALIEN1, ID_ALIEN2, ID_ALIEN3, ID_ALIEN4, ID_ALIEN5, ID_ALIEN6, ID_ALIEN7},
                              {ID_ALIEN21, ID_ALIEN22, ID_ALIEN23, ID_ALIEN24, ID_ALIEN25, ID_ALIEN26, ID_ALIEN27},
                              {ID_ALIEN31, ID_ALIEN32, ID_ALIEN33, ID_ALIEN34, ID_ALIEN35, ID_ALIEN36, ID_ALIEN37}};

// Variaveis globais
int enemy1_posy = 150;
int enemy1_posx = 10;
int direction_animation = 1; // 0-> Left; 1 -> Right
int posFirst = 10;
int posLast = SIZE_ALIEN * 7 + 14;
int moves_size_horizontal = 3;
int moves_size_vertical = 3;
int moves_size_horizontal_aux = 0;


void set_animation(int velocity);

/* aliens Thread entry function */
void aliens_thread_entry(void)
{
    initialise_monitor_handles();
    /* TODO: add your own code here */
    GX_ICON * p_icon = NULL;
    ssp_err_t err;

    while (!start_game)
    {
        tx_thread_sleep (10);
    }
    tx_thread_sleep (25);

    while (1)
    {
        // Faz as verificações para a verificação nos limites das bordas
        if ( (posLast + moves_size_horizontal + SECURITY_MARGIN) > POS_MAX)
        {
            moves_size_horizontal = (-1)*moves_size_horizontal;
            moves_size_horizontal_aux = moves_size_vertical;
         }
         else if ((posFirst+moves_size_horizontal - SECURITY_MARGIN) < 0)
         {
             moves_size_horizontal = (-1)*moves_size_horizontal;
             moves_size_horizontal_aux = moves_size_vertical;
         }
         ////------------------------------------------------------------//
         // Animação dos aliens - Vertical
         if (moves_size_horizontal_aux != 0)
         {
             for (int i = 0; i < QTD_ENEMIES_LINHAS; i++)
             {
                 for (int j = 0; j < QTD_ENEMIES_COLUNAS; j++)
                 {
                     err = (ssp_err_t)gx_widget_find(&window2, (USHORT)aliens_type1_alive[i][j], GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET**)&p_icon);
                     gx_widget_shift(p_icon, 0, moves_size_horizontal_aux, GX_TRUE);

                  }
              }
              enemy1_posy += moves_size_horizontal_aux;
              moves_size_horizontal_aux = 0;
              gx_system_canvas_refresh();
              tx_thread_sleep (40);
         }
         // Animação dos aliens, na horizontal
         for (int i = 0; i < QTD_ENEMIES_LINHAS; i++)
         {
             for (int j = 0; j < QTD_ENEMIES_COLUNAS; j++)
             {
                 err = (ssp_err_t)gx_widget_find(&window2,  (USHORT)aliens_type1_alive[i][j], GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET**)&p_icon);
                 gx_widget_shift(p_icon, moves_size_horizontal, 0, GX_TRUE);

             }
          }
          enemy1_posx += moves_size_horizontal;
          posFirst += moves_size_horizontal;
          posLast += moves_size_horizontal;
          gx_system_canvas_refresh();
          tx_thread_sleep (40);
    }
}

// Define os parametros da animação
void set_animation(int velocity)
{
    direction_animation = 1; // 0-> Left; 1 -> Right
    posFirst = 10;
    posLast = SIZE_ALIEN * 7 + 14;
    moves_size_horizontal = velocity;
    moves_size_vertical = velocity;
    moves_size_horizontal_aux = 0;
}
