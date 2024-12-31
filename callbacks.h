#ifndef CALLBACKS_H
#define CALLBACKS_H

#include "Includes.h"
void get_dd_cb_handler(lv_event_t *e);
void set_dd_cb_handler(lv_event_t *e);
static void color_changer_anim_cb(void * var, int32_t v);
void color_changer_event_cb(lv_event_t * e);
void color_event_cb(lv_event_t * e);
void button1_event_cb(lv_event_t *e);


#endif // CALLBACKS_H
