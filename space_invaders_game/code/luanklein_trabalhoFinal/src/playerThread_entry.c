

#include "defines.h"
#include "gui/finalproject_resources.h"
#include "gui/finalproject_specifications.h"
#include "main_thread.h"

#include "playerThread.h"


extern void initialise_monitor_handles(void);  //Used for printf outputs

volatile int flag_move_right = 0;
volatile int flag_move_left = 0;
extern volatile int start_game;


int current_pos = 124;



void left_move_player_cb(external_irq_callback_args_t *p_args)
{
    flag_move_left = 1;
}

void right_move_player_cb(external_irq_callback_args_t *p_args)
{
    flag_move_right = 1;
}

/* Player Thread entry function */
void playerThread_entry(void)
{
    // Call the open function for IRQ
    g_external_irq1.p_api->open(g_external_irq1.p_ctrl, g_external_irq1.p_cfg);
    g_external_irq2.p_api->open(g_external_irq2.p_ctrl, g_external_irq2.p_cfg);
    initialise_monitor_handles();

    GX_TEXT_BUTTON * p_button = NULL;
    ssp_err_t err;


    /* Create a stop button. */
    while (!start_game)
    {
        tx_thread_sleep (10);
    }
    flag_move_right = 0;
    flag_move_left = 0;
    current_pos = 128;
    /* TODO: add your own code here */
    while (1)
    {

        // --------------- Begin Player Move ----------------------
        if (flag_move_right == 1)
        {
            if ( (current_pos + HALF_SIZE) < POS_MAX)
            {
                err = (ssp_err_t)gx_widget_find(&window2, (USHORT)ID_NAVE, GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET**)&p_button);
                if (TX_SUCCESS == err)
                {
                    gx_widget_shift(p_button, 5, 0, GX_TRUE);
                    gx_system_canvas_refresh();
                    current_pos += 5;
                }
            }
            flag_move_right = 0;
        }

        else if (flag_move_left == 1)
        {
            if ( (current_pos - HALF_SIZE) > 0)
            {
                err = (ssp_err_t)gx_widget_find(&window2, (USHORT)ID_NAVE, GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET**)&p_button);
                if (TX_SUCCESS == err)
                {
                    gx_widget_shift(p_button, -5, 0, GX_TRUE);
                    gx_system_canvas_refresh();
                    current_pos -= 5;
                }
                flag_move_left = 0;
            }
            flag_move_left = 0;
        }

        tx_thread_sleep (1);
    }
    // -------- End Player Move ---------------------
}
