

#include "gui/finalproject_resources.h"
#include "gui/finalproject_specifications.h"

#include "main_thread.h"
#include "projetil_thread.h"

extern volatile int start_game;
extern int restartGame;
extern GX_WINDOW_ROOT * p_window_root;

static UINT show_window(GX_WINDOW * p_new, GX_WIDGET * p_widget, bool detach_old);

UINT window1_handler(GX_WINDOW *widget, GX_EVENT *event_ptr)
{

    UINT result = gx_window_event_process(widget, event_ptr);

    switch (event_ptr->gx_event_type)
        {
        case GX_SIGNAL(START_BUTTON, GX_EVENT_CLICKED):
            start_game = 1;
            show_window((GX_WINDOW*)&window2, (GX_WIDGET*)widget, true);

            break;
        default:
            gx_window_event_process(widget, event_ptr);
            break;
        }
    return result;
}



UINT window2_handler(GX_WINDOW *widget, GX_EVENT *event_ptr)
{

    UINT result = gx_window_event_process(widget, event_ptr);
    UINT status;
    switch (event_ptr->gx_event_type)
        {
        case GX_EVENT_PEN_UP:
            status =  tx_event_flags_set(&g_event_flags0, 0x1, TX_OR);
            break;
        default:
            gx_window_event_process(widget, event_ptr);
            break;
        }
    return result;
}



UINT window3_handler(GX_WINDOW *widget, GX_EVENT *event_ptr)
{

    UINT result = gx_window_event_process(widget, event_ptr);

    switch (event_ptr->gx_event_type)
        {
        case GX_SIGNAL(ID_RESTART, GX_EVENT_CLICKED):

            restartGame = 1;
            show_window((GX_WINDOW*)&window2, (GX_WIDGET*)widget, true);

            break;
        default:
            gx_window_event_process(widget, event_ptr);
            break;
        }
    return result;
}

static UINT show_window(GX_WINDOW * p_new, GX_WIDGET * p_widget, bool detach_old)
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
