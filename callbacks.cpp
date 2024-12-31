
#include "callbacks.h"
#include "DisplayApp.h"
#include"grl_apis.h"
// Function triggered when dropdown value changes
void get_dd_cb_handler(lv_event_t *e) {
    if (e == NULL){
        printf("e null\n");
        return; // Defensive check
    }
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *dropdown = lv_event_get_target(e);  // Get the dropdown object
    if (dropdown == NULL) {
        printf("DD NUll\n");
        return; // Ensure dropdown is valid
    }
    get_dd_button_e selected_id = (get_dd_button_e)lv_dropdown_get_selected(dropdown);

    // Fetch selected text
    if(code == LV_EVENT_VALUE_CHANGED) {
        get_dd_pressed = true;
        lv_dropdown_get_selected_str(dropdown, selected_option, sizeof(selected_option));
    }
    else{
        return;
    }

    _tx_queue_struct_t tx_struct;
    tx_struct.cmd_type = GET_CMD;
    tx_struct.selected_id = selected_id;
    BaseType_t high_task_awoken = pdFALSE;
    xQueueSendFromISR(_tx_queue, &tx_struct, &high_task_awoken);
#if 0
    // Add your custom functionality based on the selected ID
    switch ((get_dd_button_e)selected_id) {
        case DD_GET_SRC_CAPS:
            dd_get_src_caps_init(selected_id);
            break;
        case DD_GET_FW_VESION:
            dd_get_fw_v_init(selected_id);
            break;
        case DD_GET_PDC_DETAILS:
            dd_get_pdc_dets_init(selected_id);
            break;
        case DD_GET_VBUS_DATA:
            dd_get_vbus_data_init(selected_id);
        break;
        case DD_GET_VCONN_DATA:
            dd_get_vconn_data_init(selected_id);
        break;
        default:
            // Serial.println("Unknown functionality.");
            break;
    }
#endif
}

// Function triggered when dropdown value changes
void set_dd_cb_handler(lv_event_t *e) {
    if (e == NULL){
        printf("Err:E null\n");
        return; // Defensive check
    }
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *dropdown = lv_event_get_target(e);  // Get the dropdown object
    if (dropdown == NULL) {
        printf("Err: DD NUll\n");
        return; // Ensure dropdown is valid
    }
    set_dd_button_e selected_id = (set_dd_button_e)lv_dropdown_get_selected(dropdown);
    
    // Fetch selected text
    // char selected_option[64];
    if(code == LV_EVENT_VALUE_CHANGED) {
        set_dd_pressed = true;
        lv_dropdown_get_selected_str(dropdown, selected_option, sizeof(selected_option));
    }
    else{
        return;
    }

    _tx_queue_struct_t tx_struct;
    tx_struct.cmd_type = SET_CMD;
    tx_struct.selected_id = selected_id;
    BaseType_t high_task_awoken = pdFALSE;
    xQueueSendFromISR(_tx_queue, &tx_struct, &high_task_awoken);
#if 0
    // Add your custom functionality based on the selected ID
    switch ((set_dd_button_e)selected_id) {
        case DD_ATTACH_SET:
            dd_attach_api_init(selected_id);
            break;
        case DD_DETACH_SET:
            dd_detach_api_init(selected_id);
            break;
        case DD_PORT_ROLE_SET:
            dd_port_role_set_init(selected_id);
            break;
        default:
            
            break;
    }
#endif
}

void color_changer_anim_cb(void * var, int32_t v)
{
    lv_obj_t * obj = (lv_obj_t *) var;
    lv_coord_t max_w = lv_obj_get_width(lv_obj_get_parent(obj)) - LV_DPX(20);
    lv_coord_t w;

    if(disp_size == DISP_SMALL) {
        w = lv_map(v, 0, 256, LV_DPX(52), max_w);
        lv_obj_set_width(obj, w);
        lv_obj_align(obj, LV_ALIGN_BOTTOM_RIGHT, - LV_DPX(10),  - LV_DPX(10));
    }
    else {
        w = lv_map(v, 0, 256, LV_DPX(60), max_w);
        lv_obj_set_width(obj, w);
        lv_obj_align(obj, LV_ALIGN_BOTTOM_RIGHT, - LV_DPX(10),  - LV_DPX(10));
    }

    if(v > LV_OPA_COVER) v = LV_OPA_COVER;

    uint32_t i;
    for(i = 0; i < lv_obj_get_child_cnt(obj); i++) {
        lv_obj_set_style_opa(lv_obj_get_child(obj, i), v, 0);
    }
}
void color_changer_event_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_obj_t * color_cont = (lv_obj_t * )lv_event_get_user_data(e);
        if(lv_obj_get_width(color_cont) < LV_HOR_RES / 2) {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, color_cont);
            lv_anim_set_exec_cb(&a, color_changer_anim_cb);
            lv_anim_set_values(&a, 0, 256);
            lv_anim_set_time(&a, 200);
            lv_anim_start(&a);
        }
        else {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, color_cont);
            lv_anim_set_exec_cb(&a, color_changer_anim_cb);
            lv_anim_set_values(&a, 256, 0);
            lv_anim_set_time(&a, 200);
            lv_anim_start(&a);
        }
    }
}

void color_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if(code == LV_EVENT_FOCUSED) {
        lv_obj_t * color_cont = lv_obj_get_parent(obj);
        if(lv_obj_get_width(color_cont) < LV_HOR_RES / 2) {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, color_cont);
            lv_anim_set_exec_cb(&a, color_changer_anim_cb);
            lv_anim_set_values(&a, 0, 256);
            lv_anim_set_time(&a, 200);
            lv_anim_start(&a);
        }
    }
    else if(code == LV_EVENT_CLICKED) {
        lv_palette_t * palette_primary = (lv_palette_t * )lv_event_get_user_data(e);
        lv_palette_t palette_secondary = (lv_palette_t) ((*palette_primary) + 3); /*Use another palette as secondary*/
        if(palette_secondary >= _LV_PALETTE_LAST) palette_secondary = (lv_palette_t)0;
#if LV_USE_THEME_DEFAULT
        lv_theme_default_init(NULL, lv_palette_main(*palette_primary), lv_palette_main(palette_secondary),
                              LV_THEME_DEFAULT_DARK, font_normal);
#endif
        lv_color_t color = lv_palette_main(*palette_primary);
        lv_style_set_text_color(&style_icon, color);
        lv_chart_set_series_color(voltage_chart, volt_axis, color);
        lv_chart_set_series_color(current_chart, pow_axis, color);
    }
}
// Event callback for Button 1
void button1_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        g_button_pressed = true;
        // Serial.println("Button 1 Pressed");
        // Perform actions for Button 1
    }
}