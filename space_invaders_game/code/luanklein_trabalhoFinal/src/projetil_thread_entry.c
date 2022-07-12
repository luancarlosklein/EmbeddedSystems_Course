#include "projetil_thread.h"
#include "gui/finalproject_resources.h"
#include "gui/finalproject_specifications.h"
#include "main_thread.h"

void gx_widget_move(GX_WIDGET *widget, GX_VALUE x, GX_VALUE y);




extern int current_pos;

int ativo = 0;
int projetil_posy = 0;
int projetil_posx = 0;

/* projetil_thread entry function */
void projetil_thread_entry(void)
{

    GX_ICON_BUTTON * p_proj = NULL;


    ssp_err_t err;
    UINT    status;
    ULONG   actual_flags;
    int posY = 250;

    /* TODO: add your own code here */
    while (1)
    {
        posY = 250;
        /* Wait for event flag 0.  */
        status =  tx_event_flags_get(&g_event_flags0, 0x1, TX_OR_CLEAR,
                                                 &actual_flags, TX_WAIT_FOREVER);
        //
        err = (ssp_err_t)gx_widget_find(&window2, (USHORT)ID_PROJETIL_USER, GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET**)&p_proj);

        ativo = 1;
        //----------------- Seta a posição inicial do projétil ----------------------------
        projetil_posx = current_pos;
        projetil_posy = 275;
        gx_widget_move(p_proj, projetil_posx, projetil_posy);
        gx_widget_show(p_proj);
        //------------------------------------------------------------------------------------
        gx_system_canvas_refresh();

        // -------- Faz a animação do projétil ------------------
        while(posY > 20 && ativo)
        {
           // Move o projétil para cima
            projetil_posy -= 3;
            gx_widget_shift(p_proj, 0, -3, GX_TRUE);
            gx_system_canvas_refresh();
            tx_thread_sleep (3);
            posY-= 3;
        }
        ativo = 0;
        //---------------------------------------------------------
        // Apaga o projétil depois que chega no final
        gx_widget_hide(p_proj);
        gx_system_canvas_refresh();
        // Desativa a flag, caso tenha sido apertado para lançar enquanto estava na animação
        status =  tx_event_flags_get(&g_event_flags0, 0x1, TX_OR_CLEAR, &actual_flags, TX_NO_WAIT);
        tx_thread_sleep (1);
    }
}

void gx_widget_move(GX_WIDGET *widget, GX_VALUE x, GX_VALUE y)
{
  gx_system_dirty_partial_add(widget->gx_widget_parent, &widget->gx_widget_size);
  widget->gx_widget_size.gx_rectangle_bottom = widget->gx_widget_size.gx_rectangle_bottom - widget->gx_widget_size.gx_rectangle_top + y;
  widget->gx_widget_size.gx_rectangle_right = widget->gx_widget_size.gx_rectangle_right - widget->gx_widget_size.gx_rectangle_left + x;
  widget->gx_widget_size.gx_rectangle_top = y;
  widget->gx_widget_size.gx_rectangle_left = x;
  widget->gx_widget_clip = widget->gx_widget_size;
  gx_system_dirty_mark(widget);

}
