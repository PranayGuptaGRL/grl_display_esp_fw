#include "DisplayApp.h"
//Dont disturb this sequence as below lcd variable is being used in touch.h file so it expects lcd variable to be declared before using .h
LGFX lcd;
#include "touch.h"
#include "grl_timers.h"
#include "callbacks.h"
#include "grl_apis.h"
int dbg = 0;
// lv_obj_t * GrlMainTab;
// lv_obj_t * OverViewTab;
// lv_obj_t * AnalysisTab;
#if !LV_USE_DEMO_WIDGETS
bool g_button_pressed = false;
bool set_dd_pressed = false;
bool get_dd_pressed = false;
char selected_option[64] = {0};

int grl_uart_write(uart_port_t uart_num, const void* src, size_t size)
{
    int bytes_written = 0;
    // Take the mutex
    xSemaphoreTake(uart_write_mutex, portMAX_DELAY);

    bytes_written = uart_write_bytes(uart_num, src, size);
    uart_wait_tx_done(uart_num, portMAX_DELAY);    // Wait for TX buffer to empty
    Serial.flush(); // Wait until TX buffer is empty
    // Release the mutex
    xSemaphoreGive(uart_write_mutex);
    return bytes_written;
}


static void color_changer_create(lv_obj_t * parent)
{
    static lv_palette_t palette[] = {
        LV_PALETTE_BLUE, LV_PALETTE_GREEN, LV_PALETTE_BLUE_GREY,  LV_PALETTE_ORANGE,
        LV_PALETTE_RED, LV_PALETTE_PURPLE, LV_PALETTE_TEAL, _LV_PALETTE_LAST
    };

    lv_obj_t * color_cont = lv_obj_create(parent);
    lv_obj_remove_style_all(color_cont);
    lv_obj_set_flex_flow(color_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(color_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_flag(color_cont, LV_OBJ_FLAG_FLOATING);

    lv_obj_set_style_bg_color(color_cont, lv_color_white(), 0);
    lv_obj_set_style_pad_right(color_cont, disp_size == DISP_SMALL ? LV_DPX(47) : LV_DPX(55), 0);
    lv_obj_set_style_bg_opa(color_cont, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(color_cont, LV_RADIUS_CIRCLE, 0);

    if(disp_size == DISP_SMALL) lv_obj_set_size(color_cont, LV_DPX(52), LV_DPX(52));
    else lv_obj_set_size(color_cont, LV_DPX(60), LV_DPX(60));

    lv_obj_align(color_cont, LV_ALIGN_BOTTOM_RIGHT, - LV_DPX(10),  - LV_DPX(10));

    uint32_t i;
    for(i = 0; palette[i] != _LV_PALETTE_LAST; i++) {
        lv_obj_t * c = lv_btn_create(color_cont);
        lv_obj_set_style_bg_color(c, lv_palette_main(palette[i]), 0);
        lv_obj_set_style_radius(c, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_opa(c, LV_OPA_TRANSP, 0);
        lv_obj_set_size(c, 20, 20);
        lv_obj_add_event_cb(c, color_event_cb, LV_EVENT_ALL, &palette[i]);
        lv_obj_clear_flag(c, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    }

    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_FLOATING | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(btn, lv_color_white(), LV_STATE_CHECKED);
    lv_obj_set_style_pad_all(btn, 10, 0);
    lv_obj_set_style_radius(btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_add_event_cb(btn, color_changer_event_cb, LV_EVENT_ALL, color_cont);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_set_style_bg_img_src(btn, LV_SYMBOL_TINT, 0);

    if(disp_size == DISP_SMALL) {
        lv_obj_set_size(btn, LV_DPX(42), LV_DPX(42));
        lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, -LV_DPX(15), -LV_DPX(15));
    }
    else {
        lv_obj_set_size(btn, LV_DPX(50), LV_DPX(50));
        lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, -LV_DPX(15), -LV_DPX(15));
    }
}

static void setup_grllogo_panel(lv_obj_t * parent){

    lv_obj_t * grl_logo_panel = lv_obj_create(parent);
    lv_obj_set_height(grl_logo_panel, LV_SIZE_CONTENT);

    LV_IMG_DECLARE(grl_logo_circle);
    lv_obj_t * avatar = lv_img_create(grl_logo_panel);
    lv_img_set_src(avatar, &grl_logo_circle);

    lv_obj_t * name = lv_label_create(grl_logo_panel);
    lv_label_set_text(name, "GRL-V-DPWR");
    lv_obj_add_style(name, &style_title, 0);

    lv_obj_t * dsc = lv_label_create(grl_logo_panel);
    lv_obj_add_style(dsc, &style_text_muted, 0);
    lv_label_set_text(dsc, "USB Power Delivery Dual Role Power & Data Loop Back Tester");
    lv_label_set_long_mode(dsc, LV_LABEL_LONG_WRAP);

    lv_obj_t * email_icn = lv_label_create(grl_logo_panel);
    lv_obj_add_style(email_icn, &style_icon, 0);
    lv_label_set_text(email_icn, LV_SYMBOL_ENVELOPE);

    lv_obj_t * email_label = lv_label_create(grl_logo_panel);
    lv_label_set_text(email_label, "www.graniteriverlabs.com");
#ifdef CALL_ICON
    lv_obj_t * call_icn = lv_label_create(panel1);
    lv_obj_add_style(call_icn, &style_icon, 0);
    lv_label_set_text(call_icn, LV_SYMBOL_CALL);

    lv_obj_t * call_label = lv_label_create(panel1);
    lv_label_set_text(call_label, "+79 246 123 4567");
#endif
  if(disp_size == DISP_LARGE) {
    static lv_coord_t grid_main_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_main_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};

    /*Create the top panel*/
    static lv_coord_t grid_1_col_dsc[] = {LV_GRID_CONTENT, 5, LV_GRID_CONTENT, LV_GRID_FR(2), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_1_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, 1, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    
    lv_obj_set_grid_dsc_array(parent, grid_main_col_dsc, grid_main_row_dsc);
    lv_obj_set_grid_cell(grl_logo_panel, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);

    lv_obj_set_grid_dsc_array(grl_logo_panel, grid_1_col_dsc, grid_1_row_dsc);
    lv_obj_set_grid_cell(avatar, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 5);
    lv_obj_set_grid_cell(name, LV_GRID_ALIGN_START, 2, 2, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(dsc, LV_GRID_ALIGN_STRETCH, 2, 4, LV_GRID_ALIGN_START, 1, 1);
    lv_obj_set_grid_cell(email_icn, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_grid_cell(email_label, LV_GRID_ALIGN_START, 3, 1, LV_GRID_ALIGN_CENTER, 3, 1);
#ifdef CALL_ICON
    lv_obj_set_grid_cell(call_icn, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 4, 1);
    lv_obj_set_grid_cell(call_label, LV_GRID_ALIGN_START, 3, 1, LV_GRID_ALIGN_CENTER, 4, 1);
#endif
    }
  else if(disp_size == DISP_MEDIUM) {
    static lv_coord_t grid_main_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_main_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};

    /*Create the top panel*/
    static lv_coord_t grid_1_col_dsc[] = {LV_GRID_CONTENT, 1, LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_1_row_dsc[] = {
        LV_GRID_CONTENT, /*Name*/
        LV_GRID_CONTENT, /*Description*/
        LV_GRID_CONTENT, /*Email*/
        -20,
        LV_GRID_CONTENT, /*Phone*/
        LV_GRID_CONTENT, /*Buttons*/
        LV_GRID_TEMPLATE_LAST
    };

    lv_obj_set_grid_dsc_array(parent, grid_main_col_dsc, grid_main_row_dsc);

    lv_obj_set_grid_dsc_array(parent, grid_main_col_dsc, grid_main_row_dsc);
    lv_obj_set_grid_cell(grl_logo_panel, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_dsc_array(grl_logo_panel, grid_1_col_dsc, grid_1_row_dsc);
    lv_obj_set_grid_cell(avatar, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 0, 4);
    lv_obj_set_grid_cell(name, LV_GRID_ALIGN_START, 2, 2, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(dsc, LV_GRID_ALIGN_STRETCH, 2, 2, LV_GRID_ALIGN_START, 1, 1);
    lv_obj_set_grid_cell(email_label, LV_GRID_ALIGN_START, 3, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell(email_icn, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 2, 1);
  }
  else if(disp_size == DISP_SMALL){
        static lv_coord_t grid_main_col_dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static lv_coord_t grid_main_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
        lv_obj_set_grid_dsc_array(parent, grid_main_col_dsc, grid_main_row_dsc);


        /*Create the top panel*/
        static lv_coord_t grid_1_col_dsc[] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static lv_coord_t grid_1_row_dsc[] = {LV_GRID_CONTENT, /*Avatar*/
                                              LV_GRID_CONTENT, /*Name*/
                                              LV_GRID_CONTENT, /*Description*/
                                              LV_GRID_CONTENT, /*Email*/
                                              LV_GRID_CONTENT, /*Phone number*/
                                              LV_GRID_CONTENT, /*Button1*/
                                              LV_GRID_CONTENT, /*Button2*/
                                              LV_GRID_TEMPLATE_LAST
                                             };

        lv_obj_set_grid_dsc_array(grl_logo_panel, grid_1_col_dsc, grid_1_row_dsc);

        lv_obj_set_grid_cell(grl_logo_panel, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);

        lv_obj_set_style_text_align(dsc, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_set_grid_cell(avatar, LV_GRID_ALIGN_CENTER, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);
        lv_obj_set_grid_cell(name, LV_GRID_ALIGN_CENTER, 0, 2, LV_GRID_ALIGN_CENTER, 1, 1);
        lv_obj_set_grid_cell(dsc, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(email_icn, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(email_label, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);
#ifdef CALL_ICON
        lv_obj_set_grid_cell(call_icn, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 4, 1);
        lv_obj_set_grid_cell(call_label, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 4, 1);
#endif
  }
#ifdef STR_USE
  panel_obj_p->grl_logo_panel = grl_logo_panel;
#endif
}

static void setup_systeminfo_panel(lv_obj_t * parent){
    /*Create the system_info panel*/
    lv_obj_t * system_info_panel = lv_obj_create(parent);
    lv_obj_set_height(system_info_panel, LV_SIZE_CONTENT);
    
    lv_obj_t * system_info_tittle_label = lv_label_create(system_info_panel);
    lv_label_set_text(system_info_tittle_label, "System Info");
    lv_obj_add_style(system_info_tittle_label, &style_title, 0);

    lv_obj_t * fw_ver_text_label = lv_label_create(system_info_panel);
    lv_label_set_text(fw_ver_text_label, "FW Version");
    lv_obj_add_style(fw_ver_text_label, &style_text_muted, 0);

    lv_obj_t * fw_ver_divider = lv_label_create(system_info_panel);
    lv_label_set_text(fw_ver_divider, "-");
    lv_obj_add_style(fw_ver_divider, &style_text_muted, 0);

    lv_obj_t * fw_ver_num_label = lv_label_create(system_info_panel);
    lv_label_set_text(fw_ver_num_label, fw_version);
    lv_obj_add_style(fw_ver_num_label, &style_text_muted, 0);

    lv_obj_t * ip_info_text_label = lv_label_create(system_info_panel);
    lv_label_set_text(ip_info_text_label, "IP address");
    lv_obj_add_style(ip_info_text_label, &style_text_muted, 0);

    lv_obj_t * ip_add_divider = lv_label_create(system_info_panel);
    lv_label_set_text(ip_add_divider, "-");
    lv_obj_add_style(ip_add_divider, &style_text_muted, 0);

    lv_obj_t * ip_add_label = lv_label_create(system_info_panel);
    lv_label_set_text(ip_add_label, "192.168.255.1");
    lv_obj_add_style(ip_add_label, &style_text_muted, 0);

    lv_obj_t * eload_ver_text_label = lv_label_create(system_info_panel);
    lv_label_set_text(eload_ver_text_label, "E-Load Version");
    lv_obj_add_style(eload_ver_text_label, &style_text_muted, 0);

    lv_obj_t * eload_ver_divider = lv_label_create(system_info_panel);
    lv_label_set_text(eload_ver_divider, "-");
    lv_obj_add_style(eload_ver_divider, &style_text_muted, 0);

    lv_obj_t * eload_ver_num_label = lv_label_create(system_info_panel);
    lv_label_set_text(eload_ver_num_label, "x.y.z");
    lv_obj_add_style(eload_ver_num_label, &style_text_muted, 0);

    if(disp_size == DISP_LARGE) {
        // static lv_coord_t grid_2_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        // static lv_coord_t grid_2_row_dsc[] = {
        //     LV_GRID_CONTENT,  /*Title*/
        //     5,                /*Separator*/
        //     LV_GRID_CONTENT,  /*Box title*/
        //     30,               /*Boxes*/
        //     5,                /*Separator*/
        //     LV_GRID_CONTENT,  /*Box title*/
        //     30,               /*Boxes*/
        //     LV_GRID_TEMPLATE_LAST
        // };

        static lv_coord_t sys_info_col_dsc[] = {LV_GRID_CONTENT, 1, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
        static lv_coord_t sys_info_row_dsc[] = {
            LV_GRID_CONTENT,  /*Title*/
            1,                /*Separator*/
            LV_GRID_CONTENT,  /*FW version*/
            LV_GRID_CONTENT,  /*Eload version*/
            LV_GRID_CONTENT,  /*IP Address*/
            LV_GRID_CONTENT,  /*IP Address*/
            LV_GRID_TEMPLATE_LAST
        };

        lv_obj_set_grid_cell(system_info_panel, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_SPACE_BETWEEN, 1, 1);
        lv_obj_set_grid_dsc_array(system_info_panel, sys_info_col_dsc, sys_info_row_dsc);
        lv_obj_set_grid_cell(system_info_tittle_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
        lv_obj_set_grid_cell(fw_ver_text_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
        lv_obj_set_grid_cell(fw_ver_divider, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);
        lv_obj_set_grid_cell(fw_ver_num_label, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 2, 1);
        lv_obj_set_grid_cell(eload_ver_text_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(eload_ver_divider, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(eload_ver_num_label, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(ip_info_text_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 4, 1);
        lv_obj_set_grid_cell(ip_add_divider, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 4, 1);
        lv_obj_set_grid_cell(ip_add_label, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 4, 1);

    }
    else if(disp_size == DISP_MEDIUM) {
        static lv_coord_t grid_2_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static lv_coord_t grid_2_row_dsc[] = {
            LV_GRID_CONTENT,  /*Title*/
            5,                /*Separator*/
            LV_GRID_CONTENT,  /*Box title*/
            40,               /*Box*/
            LV_GRID_CONTENT,  /*Box title*/
            40,               /*Box*/
            LV_GRID_CONTENT,  /*Box title*/
            40,               /*Box*/
            LV_GRID_CONTENT,  /*Box title*/
            40,               /*Box*/
            LV_GRID_TEMPLATE_LAST
        };
    }
    else if(disp_size == DISP_SMALL) {

        static lv_coord_t grid_2_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static lv_coord_t grid_2_row_dsc[] = {
            LV_GRID_CONTENT,  /*Title*/
            5,                /*Separator*/
            LV_GRID_CONTENT,  /*Box title*/
            40,               /*Box*/
            LV_GRID_CONTENT,  /*Box title*/
            40,               /*Box*/
            LV_GRID_CONTENT,  /*Box title*/
            40,               /*Box*/
            LV_GRID_CONTENT,  /*Box title*/
            40, LV_GRID_TEMPLATE_LAST               /*Box*/
        };
    }
#ifdef STR_USE
    panel_obj_p->over_view_tab_s.sys_info_panel->sys_info_panel_obj = system_info_panel;
    panel_obj_p->over_view_tab_s.sys_info_panel->fw_ver_num_label = fw_ver_num_label;
    panel_obj_p->over_view_tab_s.sys_info_panel->ip_add_label = ip_add_label;
    panel_obj_p->over_view_tab_s.sys_info_panel->eload_ver_num_label = eload_ver_num_label;
#endif
}
void setup_portstatus_Panel(lv_obj_t * parent)
{
    lv_obj_t * port_status_panel = lv_obj_create(parent);
    lv_obj_set_height(port_status_panel, LV_SIZE_CONTENT);

    lv_obj_t * tap3_panel1_tittle = lv_label_create(port_status_panel);
    lv_label_set_text(tap3_panel1_tittle, "Port Status");
    lv_obj_add_style(tap3_panel1_tittle, &style_title, 0);

    lv_obj_t * tester_port_role_label = lv_label_create(port_status_panel);
    lv_label_set_text(tester_port_role_label, "Tester Port Role");
    lv_obj_add_style(tester_port_role_label, &style_text_muted, 0);

    lv_obj_t * separator1 = lv_label_create(port_status_panel);
    lv_label_set_text(separator1, ":");
    lv_obj_add_style(separator1, &style_text_muted, 0);

    lv_obj_t * tester_port_role = lv_label_create(port_status_panel);
    lv_label_set_text(tester_port_role, "Source/DFP");
    lv_obj_add_style(tester_port_role, &style_text_muted, 0);

    lv_obj_t * dut_port_role_label = lv_label_create(port_status_panel);
    lv_label_set_text(dut_port_role_label, "DUT Port Role");
    lv_obj_add_style(dut_port_role_label, &style_text_muted, 0);

    lv_obj_t * separator2 = lv_label_create(port_status_panel);
    lv_label_set_text(separator2, ":");
    lv_obj_add_style(separator2, &style_text_muted, 0);

    lv_obj_t * dut_port_role = lv_label_create(port_status_panel);
    lv_label_set_text(dut_port_role, "Sink/UFP");
    lv_obj_add_style(dut_port_role, &style_text_muted, 0);

    lv_obj_t * vconn_provider_label = lv_label_create(port_status_panel);
    lv_label_set_text(vconn_provider_label, "Vconn Provider");
    lv_obj_add_style(vconn_provider_label, &style_text_muted, 0);

    lv_obj_t * separator3 = lv_label_create(port_status_panel);
    lv_label_set_text(separator3, ":");
    lv_obj_add_style(separator3, &style_text_muted, 0);

    lv_obj_t * vconn_provider = lv_label_create(port_status_panel);
    lv_label_set_text(vconn_provider, "Tester");
    lv_obj_add_style(vconn_provider, &style_text_muted, 0);

    lv_obj_t * cc_pol_label = lv_label_create(port_status_panel);
    lv_label_set_text(cc_pol_label, "CC Polarity");
    lv_obj_add_style(cc_pol_label, &style_text_muted, 0);

    lv_obj_t * separator4 = lv_label_create(port_status_panel);
    lv_label_set_text(separator4, ":");
    lv_obj_add_style(separator4, &style_text_muted, 0);

    lv_obj_t * cc_pol = lv_label_create(port_status_panel);
    lv_label_set_text(cc_pol, "CC1");
    lv_obj_add_style(cc_pol, &style_text_muted, 0);

    lv_obj_t * vbus_vol_lable = lv_label_create(port_status_panel);
    lv_label_set_text(vbus_vol_lable, "Vbus Voltage");
    lv_obj_add_style(vbus_vol_lable, &style_text_muted, 0);

    lv_obj_t * separator5 = lv_label_create(port_status_panel);
    lv_label_set_text(separator5, ":");
    lv_obj_add_style(separator5, &style_text_muted, 0);

    lv_obj_t * vbus_vol = lv_label_create(port_status_panel);
    lv_label_set_text(vbus_vol, "5.03V");
    lv_obj_add_style(vbus_vol, &style_text_muted, 0);

    lv_obj_t * vbus_curr_lable = lv_label_create(port_status_panel);
    lv_label_set_text(vbus_curr_lable, "Vbus Current");
    lv_obj_add_style(vbus_curr_lable, &style_text_muted, 0);

    lv_obj_t * separator6 = lv_label_create(port_status_panel);
    lv_label_set_text(separator6, ":");
    lv_obj_add_style(separator6, &style_text_muted, 0);

    lv_obj_t * vbus_curr = lv_label_create(port_status_panel);
    lv_label_set_text(vbus_curr, "2400mA");
    lv_obj_add_style(vbus_curr, &style_text_muted, 0);


    // lv_obj_set_grid_cell(parent, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 1);

    static lv_coord_t grid_1_col_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_1_row_dsc[] = {
            LV_GRID_CONTENT,  /*Title*/
            LV_GRID_CONTENT,                /*Separator*/
            LV_GRID_CONTENT,  /*Box title*/
            LV_GRID_CONTENT,               /*Boxes*/
            LV_GRID_CONTENT,                /*Separator*/
            LV_GRID_CONTENT,  /*Box title*/
            LV_GRID_CONTENT,               /*Boxes*/
            LV_GRID_TEMPLATE_LAST
    };
    lv_obj_set_grid_cell(port_status_panel, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_START, 1, 1);

    lv_obj_set_grid_dsc_array(port_status_panel, grid_1_col_dsc, grid_1_row_dsc);

    lv_obj_set_grid_cell(tap3_panel1_tittle, LV_GRID_ALIGN_START, 0, 3, LV_GRID_ALIGN_CENTER, 0, 1);

    lv_obj_set_grid_cell(tester_port_role_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell(separator1, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell(tester_port_role, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    lv_obj_set_grid_cell(dut_port_role_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell(separator2, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell(dut_port_role, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 2, 1);

    lv_obj_set_grid_cell(vconn_provider_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_grid_cell(separator3, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_grid_cell(vconn_provider, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 3, 1);

    lv_obj_set_grid_cell(cc_pol_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 4, 1);
    lv_obj_set_grid_cell(separator4, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 4, 1);
    lv_obj_set_grid_cell(cc_pol, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 4, 1);

    lv_obj_set_grid_cell(vbus_vol_lable, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 5, 1);
    lv_obj_set_grid_cell(separator5, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 5, 1);
    lv_obj_set_grid_cell(vbus_vol, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 5, 1);

    lv_obj_set_grid_cell(vbus_curr_lable, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 6, 1);
    lv_obj_set_grid_cell(separator6, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 6, 1);
    lv_obj_set_grid_cell(vbus_curr, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 6, 1);
#ifdef STR_USE
    panel_obj_p->over_view_tab_s.port_sts_panel->port_sts_panel_obj = port_status_panel;
    panel_obj_p->over_view_tab_s.port_sts_panel->dut_port_role = dut_port_role;
    panel_obj_p->over_view_tab_s.port_sts_panel->vconn_provider = vconn_provider;
    panel_obj_p->over_view_tab_s.port_sts_panel->cc_pol = cc_pol;
    panel_obj_p->over_view_tab_s.port_sts_panel->vbus_vol = vbus_vol;
    panel_obj_p->over_view_tab_s.port_sts_panel->vbus_curr = vbus_curr; 
#endif
}

static void overview_profile_create(lv_obj_t * overviewparent)
{
    setup_grllogo_panel(overviewparent);
    setup_systeminfo_panel(overviewparent);
    setup_portstatus_Panel(overviewparent);
}

static void chart_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if(code == LV_EVENT_PRESSED || code == LV_EVENT_RELEASED) {
        lv_obj_invalidate(obj); /*To make the value boxes visible*/
    }
    else if(code == LV_EVENT_DRAW_PART_BEGIN) {
        lv_obj_draw_part_dsc_t * dsc = (lv_obj_draw_part_dsc_t * )lv_event_get_param(e);
        /*Set the markers' text*/
        if(dsc->part == LV_PART_TICKS && dsc->id == LV_CHART_AXIS_PRIMARY_X) {
            if(lv_chart_get_type(obj) == LV_CHART_TYPE_BAR) {
                const char * month[] = {"I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X", "XI", "XII"};
                lv_snprintf(dsc->text, dsc->text_length, "%s", month[dsc->value]);
            }
            else {
                const char * month[] = {"0s", "1s", "2s", "3s", "4s", "5s", "6s", "7s", "8s", "9s", "10s", "11s"};
                lv_snprintf(dsc->text, dsc->text_length, "%s", month[dsc->value]);
            }
        }

        /*Add the faded area before the lines are drawn */
        else if(dsc->part == LV_PART_ITEMS) {
#if LV_DRAW_COMPLEX
            /*Add  a line mask that keeps the area below the line*/
            if(dsc->p1 && dsc->p2) {
                lv_draw_mask_line_param_t line_mask_param;
                lv_draw_mask_line_points_init(&line_mask_param, dsc->p1->x, dsc->p1->y, dsc->p2->x, dsc->p2->y,
                                              LV_DRAW_MASK_LINE_SIDE_BOTTOM);
                int16_t line_mask_id = lv_draw_mask_add(&line_mask_param, NULL);

                /*Add a fade effect: transparent bottom covering top*/
                lv_coord_t h = lv_obj_get_height(obj);
                lv_draw_mask_fade_param_t fade_mask_param;
                lv_draw_mask_fade_init(&fade_mask_param, &obj->coords, LV_OPA_COVER, obj->coords.y1 + h / 8, LV_OPA_TRANSP,
                                       obj->coords.y2);
                int16_t fade_mask_id = lv_draw_mask_add(&fade_mask_param, NULL);

                /*Draw a rectangle that will be affected by the mask*/
                lv_draw_rect_dsc_t draw_rect_dsc;
                lv_draw_rect_dsc_init(&draw_rect_dsc);
                draw_rect_dsc.bg_opa = LV_OPA_50;
                draw_rect_dsc.bg_color = dsc->line_dsc->color;

                lv_area_t obj_clip_area;
                _lv_area_intersect(&obj_clip_area, dsc->draw_ctx->clip_area, &obj->coords);
                const lv_area_t * clip_area_ori = dsc->draw_ctx->clip_area;
                dsc->draw_ctx->clip_area = &obj_clip_area;
                lv_area_t a;
                a.x1 = dsc->p1->x;
                a.x2 = dsc->p2->x - 1;
                a.y1 = LV_MIN(dsc->p1->y, dsc->p2->y);
                a.y2 = obj->coords.y2;
                lv_draw_rect(dsc->draw_ctx, &draw_rect_dsc, &a);
                dsc->draw_ctx->clip_area = clip_area_ori;
                /*Remove the masks*/
                lv_draw_mask_remove_id(line_mask_id);
                lv_draw_mask_remove_id(fade_mask_id);
            }
#endif
            const lv_chart_series_t * ser = (lv_chart_series_t *)dsc->sub_part_ptr;

            if(lv_chart_get_pressed_point(obj) == dsc->id) {
                if(lv_chart_get_type(obj) == LV_CHART_TYPE_LINE) {
                    dsc->rect_dsc->outline_color = lv_color_white();
                    dsc->rect_dsc->outline_width = 2;
                }
                else {
                    dsc->rect_dsc->shadow_color = ser->color;
                    dsc->rect_dsc->shadow_width = 15;
                    dsc->rect_dsc->shadow_spread = 0;
                }

                char buf[8];
                lv_snprintf(buf, sizeof(buf), "%"LV_PRIu32, dsc->value);

                lv_point_t text_size;
                lv_txt_get_size(&text_size, buf, font_normal, 0, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);

                lv_area_t txt_area;
                if(lv_chart_get_type(obj) == LV_CHART_TYPE_BAR) {
                    txt_area.y2 = dsc->draw_area->y1 - LV_DPX(15);
                    txt_area.y1 = txt_area.y2 - text_size.y;
                    if(ser == lv_chart_get_series_next(obj, NULL)) {
                        txt_area.x1 = dsc->draw_area->x1 + lv_area_get_width(dsc->draw_area) / 2;
                        txt_area.x2 = txt_area.x1 + text_size.x;
                    }
                    else {
                        txt_area.x2 = dsc->draw_area->x1 + lv_area_get_width(dsc->draw_area) / 2;
                        txt_area.x1 = txt_area.x2 - text_size.x;
                    }
                }
                else {
                    txt_area.x1 = dsc->draw_area->x1 + lv_area_get_width(dsc->draw_area) / 2 - text_size.x / 2;
                    txt_area.x2 = txt_area.x1 + text_size.x;
                    txt_area.y2 = dsc->draw_area->y1 - LV_DPX(15);
                    txt_area.y1 = txt_area.y2 - text_size.y;
                }

                lv_area_t bg_area;
                bg_area.x1 = txt_area.x1 - LV_DPX(8);
                bg_area.x2 = txt_area.x2 + LV_DPX(8);
                bg_area.y1 = txt_area.y1 - LV_DPX(8);
                bg_area.y2 = txt_area.y2 + LV_DPX(8);

                lv_draw_rect_dsc_t rect_dsc;
                lv_draw_rect_dsc_init(&rect_dsc);
                rect_dsc.bg_color = ser->color;
                rect_dsc.radius = LV_DPX(5);
                lv_draw_rect(dsc->draw_ctx, &rect_dsc, &bg_area);

                lv_draw_label_dsc_t label_dsc;
                lv_draw_label_dsc_init(&label_dsc);
                label_dsc.color = lv_color_white();
                label_dsc.font = font_normal;
                lv_draw_label(dsc->draw_ctx, &label_dsc, &txt_area,  buf, NULL);
            }
            else {
                dsc->rect_dsc->outline_width = 0;
                dsc->rect_dsc->shadow_width = 0;
            }
        }
    }
}
void voltage_chart_create(lv_obj_t * parent)
{
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW_WRAP);

    static lv_coord_t grid_chart_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_FR(1), 10, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_chart_col_dsc[] = {20, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    lv_obj_t * volt_plot = lv_obj_create(parent);
    lv_obj_set_flex_grow(volt_plot, 1);
    lv_obj_set_grid_dsc_array(volt_plot, grid_chart_col_dsc, grid_chart_row_dsc);

    lv_obj_set_height(volt_plot, LV_PCT(100));
    lv_obj_set_style_max_height(volt_plot, 300, 0);

    lv_obj_t * title = lv_label_create(volt_plot);
    lv_label_set_text(title, "Voltage Plot in Volts");
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_set_grid_cell(title, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 0, 1);

    voltage_chart = lv_chart_create(volt_plot);
    lv_group_add_obj(lv_group_get_default(), voltage_chart);
    lv_obj_add_flag(voltage_chart, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_set_grid_cell(voltage_chart, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_chart_set_axis_tick(voltage_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 0, 5, 1, true, 80);
    lv_chart_set_axis_tick(voltage_chart, LV_CHART_AXIS_PRIMARY_X, 0, 0, 12, 1, true, 50);
    lv_chart_set_div_line_count(voltage_chart, 0, 25);
    lv_chart_set_point_count(voltage_chart, 25);
    lv_obj_add_event_cb(voltage_chart, chart_event_cb, LV_EVENT_ALL, NULL);
    if(disp_size == DISP_SMALL) lv_chart_set_zoom_x(voltage_chart, 256 * 3);
    else if(disp_size == DISP_MEDIUM) lv_chart_set_zoom_x(voltage_chart, 256 * 2);

    lv_obj_set_style_border_side(voltage_chart, LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_radius(voltage_chart, 0, 0);

    volt_axis = lv_chart_add_series(voltage_chart, lv_theme_get_color_primary(voltage_chart), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_next_value(voltage_chart, volt_axis, lv_rand(10, 80));
    lv_chart_set_next_value(voltage_chart, volt_axis, lv_rand(10, 80));
    lv_chart_set_next_value(voltage_chart, volt_axis, lv_rand(10, 80));
    lv_chart_set_next_value(voltage_chart, volt_axis, lv_rand(10, 80));
    lv_chart_set_next_value(voltage_chart, volt_axis, lv_rand(10, 80));
    lv_chart_set_next_value(voltage_chart, volt_axis, lv_rand(10, 80));
    lv_chart_set_next_value(voltage_chart, volt_axis, lv_rand(10, 80));
    lv_chart_set_next_value(voltage_chart, volt_axis, lv_rand(10, 80));
    lv_chart_set_next_value(voltage_chart, volt_axis, lv_rand(10, 80));
    lv_chart_set_next_value(voltage_chart, volt_axis, lv_rand(10, 80));
    lv_chart_set_next_value(voltage_chart, volt_axis, lv_rand(10, 80));
    lv_chart_set_next_value(voltage_chart, volt_axis, lv_rand(10, 80));
    lv_chart_set_next_value(voltage_chart, volt_axis, lv_rand(10, 80));
}
void current_chart_create(lv_obj_t * parent)
{
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW_WRAP);

    static lv_coord_t grid_chart_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_FR(1), 10, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_chart_col_dsc[] = {20, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    lv_obj_t * chart2_cont = lv_obj_create(parent);
    lv_obj_add_flag(chart2_cont, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    lv_obj_set_flex_grow(chart2_cont, 1);

    lv_obj_set_height(chart2_cont, LV_PCT(100));
    lv_obj_set_style_max_height(chart2_cont, 300, 0);

    lv_obj_set_grid_dsc_array(chart2_cont, grid_chart_col_dsc, grid_chart_row_dsc);

    lv_obj_t * title = lv_label_create(chart2_cont);
    lv_label_set_text(title, "Current Plot in mA");
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_set_grid_cell(title, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 0, 1);

    current_chart = lv_chart_create(chart2_cont);
    lv_group_add_obj(lv_group_get_default(), current_chart);
    lv_obj_add_flag(current_chart, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    lv_obj_set_grid_cell(current_chart, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_chart_set_axis_tick(current_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 0, 5, 1, true, 80);
    lv_chart_set_axis_tick(current_chart, LV_CHART_AXIS_PRIMARY_X, 0, 0, 12, 1, true, 50);
    lv_obj_set_size(current_chart, LV_PCT(100), LV_PCT(100));
    lv_chart_set_type(current_chart, LV_CHART_TYPE_LINE);
    lv_chart_set_div_line_count(current_chart, 6, 0);
    lv_chart_set_point_count(current_chart, 12);
    lv_obj_add_event_cb(current_chart, chart_event_cb, LV_EVENT_ALL, NULL);
    lv_chart_set_zoom_x(current_chart, 256 * 2);
    lv_obj_set_style_border_side(current_chart, LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_radius(current_chart, 0, 0);

    if(disp_size == DISP_SMALL) {
        lv_obj_set_style_pad_gap(current_chart, 0, LV_PART_ITEMS);
        lv_obj_set_style_pad_gap(current_chart, 2, LV_PART_MAIN);
    }
    else if(disp_size == DISP_LARGE) {
        lv_obj_set_style_pad_gap(current_chart, 16, 0);
    }
    pow_axis = lv_chart_add_series(current_chart, lv_theme_get_color_primary(voltage_chart), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_next_value(current_chart, pow_axis, lv_rand(10, 80));
    lv_chart_set_next_value(current_chart, pow_axis, lv_rand(10, 80));
    lv_chart_set_next_value(current_chart, pow_axis, lv_rand(10, 80));
    lv_chart_set_next_value(current_chart, pow_axis, lv_rand(10, 80));
    lv_chart_set_next_value(current_chart, pow_axis, lv_rand(10, 80));
    lv_chart_set_next_value(current_chart, pow_axis, lv_rand(10, 80));
    lv_chart_set_next_value(current_chart, pow_axis, lv_rand(10, 80));
    lv_chart_set_next_value(current_chart, pow_axis, lv_rand(10, 80));
    lv_chart_set_next_value(current_chart, pow_axis, lv_rand(10, 80));
    lv_chart_set_next_value(current_chart, pow_axis, lv_rand(10, 80));
    lv_chart_set_next_value(current_chart, pow_axis, lv_rand(10, 80));
    lv_chart_set_next_value(current_chart, pow_axis, lv_rand(10, 80));
    lv_chart_set_next_value(current_chart, pow_axis, lv_rand(10, 80));
}
static void analytics_profile_create(lv_obj_t * parent)
{
    voltage_chart_create(parent);
    current_chart_create(parent);
}

void port_status_info(lv_obj_t * parent)
{
    lv_obj_t * tap3_panel1_tittle = lv_label_create(parent);
    lv_label_set_text(tap3_panel1_tittle, "Port Status");
    lv_obj_add_style(tap3_panel1_tittle, &style_title, 0);
#if 0
    lv_obj_t * tester_port_role_label = lv_label_create(parent);
    lv_label_set_text(tester_port_role_label, "Tester Port Role");
    lv_obj_add_style(tester_port_role_label, &style_text_muted, 0);

    lv_obj_t * separator1 = lv_label_create(parent);
    lv_label_set_text(separator1, ":");
    lv_obj_add_style(separator1, &style_text_muted, 0);

    lv_obj_t * tester_port_role = lv_label_create(parent);
    lv_label_set_text(tester_port_role, "Source/DFP");
    lv_obj_add_style(tester_port_role, &style_text_muted, 0);

    lv_obj_t * dut_port_role_label = lv_label_create(parent);
    lv_label_set_text(dut_port_role_label, "DUT Port Role");
    lv_obj_add_style(dut_port_role_label, &style_text_muted, 0);

    lv_obj_t * separator2 = lv_label_create(parent);
    lv_label_set_text(separator2, ":");
    lv_obj_add_style(separator2, &style_text_muted, 0);

    lv_obj_t * dut_port_role = lv_label_create(parent);
    lv_label_set_text(dut_port_role, "Sink/UFP");
    lv_obj_add_style(dut_port_role, &style_text_muted, 0);

    lv_obj_t * vconn_provider_label = lv_label_create(parent);
    lv_label_set_text(vconn_provider_label, "Vconn Provider");
    lv_obj_add_style(vconn_provider_label, &style_text_muted, 0);

    lv_obj_t * separator3 = lv_label_create(parent);
    lv_label_set_text(separator3, ":");
    lv_obj_add_style(separator3, &style_text_muted, 0);

    lv_obj_t * vconn_provider = lv_label_create(parent);
    lv_label_set_text(vconn_provider, "Tester");
    lv_obj_add_style(vconn_provider, &style_text_muted, 0);

    lv_obj_t * cc_pol_label = lv_label_create(parent);
    lv_label_set_text(cc_pol_label, "CC Polarity");
    lv_obj_add_style(cc_pol_label, &style_text_muted, 0);

    lv_obj_t * separator4 = lv_label_create(parent);
    lv_label_set_text(separator4, ":");
    lv_obj_add_style(separator4, &style_text_muted, 0);

    lv_obj_t * cc_pol = lv_label_create(parent);
    lv_label_set_text(cc_pol, "CC1");
    lv_obj_add_style(cc_pol, &style_text_muted, 0);

    lv_obj_t * vbus_vol_lable = lv_label_create(parent);
    lv_label_set_text(vbus_vol_lable, "Vbus Voltage");
    lv_obj_add_style(vbus_vol_lable, &style_text_muted, 0);

    lv_obj_t * separator5 = lv_label_create(parent);
    lv_label_set_text(separator5, ":");
    lv_obj_add_style(separator5, &style_text_muted, 0);

    lv_obj_t * vbus_vol = lv_label_create(parent);
    lv_label_set_text(vbus_vol, "5.03V");
    lv_obj_add_style(vbus_vol, &style_text_muted, 0);

    lv_obj_t * vbus_curr_lable = lv_label_create(parent);
    lv_label_set_text(vbus_curr_lable, "Vbus Current");
    lv_obj_add_style(vbus_curr_lable, &style_text_muted, 0);

    lv_obj_t * separator6 = lv_label_create(parent);
    lv_label_set_text(separator6, ":");
    lv_obj_add_style(separator6, &style_text_muted, 0);

    lv_obj_t * vbus_curr= lv_label_create(parent);
    lv_label_set_text(vbus_curr, "2400mA");
    lv_obj_add_style(vbus_curr, &style_text_muted, 0);

#endif
    // lv_obj_set_grid_cell(parent, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 1);

    static lv_coord_t grid_1_col_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_1_row_dsc[] = {
            LV_GRID_CONTENT,  /*Title*/
            LV_GRID_CONTENT,                /*Separator*/
            LV_GRID_CONTENT,  /*Box title*/
            LV_GRID_CONTENT,               /*Boxes*/
            LV_GRID_CONTENT,                /*Separator*/
            LV_GRID_CONTENT,  /*Box title*/
            LV_GRID_CONTENT,               /*Boxes*/
            LV_GRID_TEMPLATE_LAST
    };

    lv_obj_set_grid_dsc_array(parent, grid_1_col_dsc, grid_1_row_dsc);

    lv_obj_set_grid_cell(tap3_panel1_tittle, LV_GRID_ALIGN_START, 0, 3, LV_GRID_ALIGN_CENTER, 0, 1);
#if 0
    lv_obj_set_grid_cell(tester_port_role_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell(separator1, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell(tester_port_role, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    lv_obj_set_grid_cell(dut_port_role_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell(separator2, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell(dut_port_role, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 2, 1);

    lv_obj_set_grid_cell(vconn_provider_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_grid_cell(separator3, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_grid_cell(vconn_provider, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 3, 1);

    lv_obj_set_grid_cell(cc_pol_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 4, 1);
    lv_obj_set_grid_cell(separator4, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 4, 1);
    lv_obj_set_grid_cell(cc_pol, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 4, 1);

    lv_obj_set_grid_cell(vbus_vol_lable, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 5, 1);
    lv_obj_set_grid_cell(separator5, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 5, 1);
    lv_obj_set_grid_cell(vbus_vol, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 5, 1);

    lv_obj_set_grid_cell(vbus_curr_lable, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 6, 1);
    lv_obj_set_grid_cell(separator6, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 6, 1);
    lv_obj_set_grid_cell(vbus_curr, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 6, 1);
#endif
}

void set_dd_config_handler(lv_obj_t * parent){
   // Create the dropdown in the first row
    lv_obj_t *set_dropdown = lv_dropdown_create(parent);
    /*Set a fixed text to display on the button of the drop-down list*/
    lv_dropdown_set_text(set_dropdown, "Set Func.");
    lv_dropdown_set_options_static(set_dropdown, SET_DD_OPTIONS);
    lv_obj_add_event_cb(set_dropdown, set_dd_cb_handler, LV_EVENT_ALL, NULL);
    lv_obj_set_grid_cell(set_dropdown, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);

}
void get_dd_config_handler(lv_obj_t * parent){
   // Create the dropdown in the second row
    lv_obj_t *get_dropdown = lv_dropdown_create(parent);
    /*Set a fixed text to display on the button of the drop-down list*/
    lv_dropdown_set_text(get_dropdown, "Get Func.");
    lv_dropdown_set_options_static(get_dropdown, GET_DD_OPTIONS);
    lv_obj_add_event_cb(get_dropdown, get_dd_cb_handler, LV_EVENT_ALL, NULL);
    lv_obj_set_grid_cell(get_dropdown, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);

}
void packet_select_info(lv_obj_t * parent)
{
    lv_obj_t * pd_msg_sel_label = lv_label_create(parent);
    lv_label_set_text(pd_msg_sel_label, "PD Message Selection");
    lv_obj_add_style(pd_msg_sel_label, &style_title, 0);
    static lv_coord_t grid_col_dsc[] = {LV_GRID_FR(1),LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_row_dsc[] = {
            LV_GRID_CONTENT,  /*Title*/
            LV_GRID_CONTENT,
            LV_GRID_CONTENT,
            LV_GRID_TEMPLATE_LAST
    };
    lv_obj_set_grid_dsc_array(parent, grid_col_dsc, grid_row_dsc);
    lv_obj_set_grid_cell(pd_msg_sel_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    
    /***creating dropdown for set commands */
    set_dd_config_handler(parent);

    /**Creating dropdwown for get commands */
    get_dd_config_handler(parent);
}

void port_control_tab(lv_obj_t * parent)
{
    static lv_coord_t grid_main_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_main_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(parent, grid_main_col_dsc, grid_main_row_dsc);

    lv_obj_t * packet_select_panel = lv_obj_create(parent);
    lv_obj_set_height(packet_select_panel, LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(packet_select_panel, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_START, 0, 1);
    packet_select_info(packet_select_panel);

    lv_obj_t * port_status_panel = lv_obj_create(parent);
    lv_obj_set_height(port_status_panel, LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(port_status_panel, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_START, 0, 1);
    port_status_info(port_status_panel);
}

void log_features_info(lv_obj_t * parent)
{

    lv_obj_t * fw_logging_label = lv_label_create(parent);
    lv_label_set_text(fw_logging_label, "FW_LOGGING");
    lv_obj_add_style(fw_logging_label, &style_title, 0);

    // lv_obj_t * key_features = lv_label_create(parent);
    // lv_obj_add_style(key_features, &style_text_muted, 0);
    // lv_label_set_text(key_features, "Supports the following specifications:\n\t\t1. USB PD 2.0 & PD 3.1 Programmable Power Supply (PPS) negotiation\n\t\t2. USB 2.0 and USB 3.2 data loopback testing\n\t\t3. Up to 100W of power loading and sourcing\n\t\t4. Up to 7.5W per port for VCONN power loading\n\t\t5. IR drop compensation through VBUS voltage sense line within the GRL Tester cable offers");
    // lv_label_set_long_mode(key_features, LV_LABEL_LONG_WRAP);

    static lv_coord_t grid_col_dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_row_dsc[] = {
            LV_GRID_CONTENT,  /*Title*/
            LV_GRID_CONTENT,
            LV_GRID_TEMPLATE_LAST
    };

    lv_obj_set_grid_dsc_array(parent, grid_col_dsc, grid_row_dsc);

    lv_obj_set_grid_cell(fw_logging_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 1);
    // lv_obj_set_grid_cell(key_features, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_START, 1, 1);
    
    panel_obj_p->log_tab_s.log_panel->fw_logging_label = fw_logging_label;

}
void setup_buttons_panel(lv_obj_t * panelparent){

    lv_obj_t * buttons_panel = lv_obj_create(panelparent);
    lv_obj_set_height(buttons_panel, LV_SIZE_CONTENT);
    lv_obj_set_width(buttons_panel,LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(buttons_panel, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_START, 0, 1);

    lv_obj_t * tap3_panel2_tittle = lv_label_create(panelparent);
    lv_label_set_text(tap3_panel2_tittle, "PD Message \nSelection");
    lv_obj_add_style(tap3_panel2_tittle, &style_title, 0);

    lv_obj_t * btn1 = lv_btn_create(panelparent);
    lv_obj_set_height(btn1, LV_SIZE_CONTENT);
    lv_obj_t * btn1_label = lv_label_create(btn1);
    lv_obj_add_event_cb(btn1, button1_event_cb, LV_EVENT_CLICKED, NULL);
    lv_label_set_text(btn1_label, "Get Source \nCapability");
    lv_label_set_long_mode(btn1_label, LV_LABEL_LONG_WRAP);
    lv_obj_center(btn1_label);
            
    static lv_coord_t sys_info_col_dsc[] = {LV_GRID_FR(1),LV_GRID_TEMPLATE_LAST};
    static lv_coord_t sys_info_row_dsc[] = {
        LV_GRID_CONTENT,
        LV_GRID_CONTENT,           
        // LV_GRID_CONTENT,           
        LV_GRID_TEMPLATE_LAST
    };
    lv_obj_set_grid_dsc_array(buttons_panel, sys_info_col_dsc, sys_info_row_dsc);
    lv_obj_set_grid_cell(tap3_panel2_tittle, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell(btn1, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);

}
void log_tab_create(lv_obj_t * parent)
{
    static lv_coord_t grid_main_col_dsc[] = {LV_GRID_FR(1),LV_GRID_FR(1),LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_main_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(parent, grid_main_col_dsc, grid_main_row_dsc);

    setup_buttons_panel(parent);

    panel_obj_p->log_tab_s.log_panel->log_panel_obj = lv_obj_create(parent);
    lv_obj_set_height(panel_obj_p->log_tab_s.log_panel->log_panel_obj, LV_SIZE_CONTENT);
    // lv_obj_set_width(panel_obj_p->log_tab_s.log_panel->log_panel_obj,LV_SIZE_CONTENT);

    // lv_obj_set_grid_dsc_array(panel_obj_p->log_tab_s.log_panel->log_panel_obj, grid_main_col_dsc, grid_main_row_dsc);
    lv_obj_set_grid_cell(panel_obj_p->log_tab_s.log_panel->log_panel_obj, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_START, 0, 1);

    log_features_info(panel_obj_p->log_tab_s.log_panel->log_panel_obj);
}
void grl_create_widgets(void){

  if(LV_HOR_RES <= 320) 
    disp_size = DISP_SMALL;
  else if(LV_HOR_RES < 720) 
    disp_size = DISP_MEDIUM;
  else 
    disp_size = DISP_LARGE;

  font_large = LV_FONT_DEFAULT;
  font_normal = LV_FONT_DEFAULT;
  lv_coord_t tab_h;
  if(disp_size == DISP_LARGE) {
    tab_h = 70;
    font_large     = &lv_font_montserrat_24;
    font_normal    = &lv_font_montserrat_16;
  }
  else if(disp_size == DISP_MEDIUM) {
    tab_h = 45;
    font_large     = &lv_font_montserrat_20;
    font_normal    = &lv_font_montserrat_14;
  }
  else{ /* disp_size == DISP_SMALL */
    tab_h = 45;
    font_large     = &lv_font_montserrat_18;
    font_normal    = &lv_font_montserrat_12;
  }

  lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK,
                          font_normal);
  lv_style_init(&style_text_muted);
  lv_style_set_text_opa(&style_text_muted, LV_OPA_COVER);
  lv_style_init(&style_title);
  lv_style_set_text_font(&style_title, font_large);

  lv_style_init(&style_icon);
  lv_style_set_text_color(&style_icon, lv_theme_get_color_primary(NULL));
  lv_style_set_text_font(&style_icon, font_large);

  lv_style_init(&style_bullet);
  lv_style_set_border_width(&style_bullet, 0);
  lv_style_set_radius(&style_bullet, LV_RADIUS_CIRCLE);

  panel_obj_p->grl_main_panel = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, tab_h);

  lv_obj_set_style_text_font(lv_scr_act(), font_normal, 0);

  if(disp_size == DISP_LARGE) {
      lv_obj_t * tab_btns = lv_tabview_get_tab_btns(panel_obj_p->grl_main_panel);
      lv_obj_set_style_pad_left(tab_btns, LV_HOR_RES / 2, 0);
      lv_obj_t * logo = lv_img_create(tab_btns);
      LV_IMG_DECLARE(grl_image);
      lv_img_set_src(logo, &grl_image);
      lv_obj_align(logo, LV_ALIGN_LEFT_MID, -LV_HOR_RES / 2 + 25, 0);

      lv_obj_t * label = lv_label_create(tab_btns);
      lv_obj_add_style(label, &style_title, 0);
      // lv_label_set_text(label, "LVGL v8");
      lv_label_set_text_fmt(label, "GRL-V-DPWR");
      lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
  }

  panel_obj_p->over_view_tab_s.over_view_tab = lv_tabview_add_tab(panel_obj_p->grl_main_panel, "Overview");
  overview_profile_create(panel_obj_p->over_view_tab_s.over_view_tab);
  
 
  panel_obj_p->analysis_tab_s.analysis_tab = lv_tabview_add_tab(panel_obj_p->grl_main_panel, "Analysis");
  analytics_profile_create(panel_obj_p->analysis_tab_s.analysis_tab);
  
//   lv_obj_t * lbl;
  panel_obj_p->port_ctrl_tab_s.prt_ctrl_tab = lv_tabview_add_tab(panel_obj_p->grl_main_panel, "Port Control");
  port_control_tab(panel_obj_p->port_ctrl_tab_s.prt_ctrl_tab);
  
  panel_obj_p->log_tab_s.log_tab = lv_tabview_add_tab(panel_obj_p->grl_main_panel, "Log");
  log_tab_create(panel_obj_p->log_tab_s.log_tab);

  color_changer_create(panel_obj_p->grl_main_panel);
}
#endif/*!LV_USE_DEMO_WIDGETS*/

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{

  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  //lcd.fillScreen(TFT_WHITE);
#if (LV_COLOR_16_SWAP != 0)
 lcd.pushImageDMA(area->x1, area->y1, w, h,(lgfx::rgb565_t*)&color_p->full);
#else
  lcd.pushImageDMA(area->x1, area->y1, w, h,(lgfx::rgb565_t*)&color_p->full);//
#endif

  lv_disp_flush_ready(disp);

}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  if (touch_has_signal())
  {
    if (touch_touched())
    {
      data->state = LV_INDEV_STATE_PR;

      /*Set the coordinates*/
      data->point.x = touch_last_x;
      data->point.y = touch_last_y;
      // Serial.print( "Data x :" );
    //   Serial.println( touch_last_x );

      // Serial.print( "Data y :" );
    //   Serial.println( touch_last_y );
    }
    else if (touch_released())
    {
      data->state = LV_INDEV_STATE_REL;
    }
  }
  else
  {
    data->state = LV_INDEV_STATE_REL;
  }
  delay(15);
}

void update()
{
    uint32_t i;
    memcpy(&debug[0],&debug[1],(4 * 24));
    debug[24] = lv_rand(10, 80);

    for(i = 0; i < 25; i++) {
        lv_chart_set_next_value(voltage_chart, volt_axis, debug[i]);
    }

    for(i = 0; i < 12; i++) {
        // lv_chart_set_next_value(current_chart, curr_axis, lv_rand(10, 80));
        lv_chart_set_next_value(current_chart, pow_axis, debug[i]);
    }
}
void pop_ui_log(_rx_queue_struct_t rx_struct){
// #ifdef PUSH_TO_UI

    String input = " ";
    // Convert the binary data into a human-readable string just to push it to UI
    // this loop is only for pushing data to UI for logging purpose
    for (int i = 0; i < rx_struct.rx_buf_size; i++) {
    // if(i>0)
        input+=" 0x";// Add space between bytes
        input += String(rx_struct.rx_buffer[i], HEX); // Convert to hex and append
    // pos += snprintf(&input[pos], sizeof(input) - pos, "%02X ", data[i]);
    // if (pos >= sizeof(input)) break; // Prevent buffer overflow
    }
    input += "\n";
    // Read the incoming byte from Serial (UART0)
    // incomingByte = Serial.read();
    // input += incomingByte;
    // inp_arr[i++] = incomingByte;
    // Print the received data to the Serial Monitor
    // if(incomingByte == '\n' || incomingByte == '\r') 
    // data_rx=1;
    // i=0;
    // delay(2);
    lv_label_set_text(panel_obj_p->log_tab_s.log_panel->fw_logging_label, (input).c_str());
    delay(2);
    lv_obj_invalidate(panel_obj_p->log_tab_s.log_panel->log_panel_obj);  // Invalidate the button to refresh the UI

// #endif/**PUSH_TO_UI*/
}
#define SOP_INDEX	0
#define PAYLOAD_INDEX	2
#define RX_CHUNK_SIZE 32
#define MAX_MESSAGE_SIZE 255
#define MESSAGE_TIMEOUT 1    // Timeout in seconds
#define MESSAGE_TIMEOUT_mS 250    // Timeout in milli seconds
#define SOP ';'             // Start of Packet marker
#define EOP ';'

typedef struct {
    uint8_t rxbuffer[MAX_MESSAGE_SIZE];
    uint8_t buffer_pos;
    uint8_t api_length;
    bool is_receiving_message;
    time_t last_received_time; // Timestamp for timeout tracking in Sec
    long long last_recv_time_ms;//Timestamp for last received time in mS
} rx_msg_struct_t;

void init_message_buffer(rx_msg_struct_t *rx_msg_buf) {
    memset(rx_msg_buf->rxbuffer, 0, sizeof(rx_msg_buf->rxbuffer)); // Clear the buffer
    rx_msg_buf->buffer_pos = 0;
    rx_msg_buf->api_length = 0;
    rx_msg_buf->is_receiving_message = false;
    rx_msg_buf->last_received_time = time(NULL);
}
long long current_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (long long)(ts.tv_sec) * 1000 + (ts.tv_nsec) / 1000000;
}
void uart_read()
{
    int index = 0;
#if 0
    rx_msg_struct_t msg_buf;
	init_message_buffer(&msg_buf);
	uint8_t chunk_buffer[RX_CHUNK_SIZE];
    while (Serial.available())
    {
        int bytes_read = uart_read_bytes(UART_NUM_0, chunk_buffer, RX_CHUNK_SIZE, portMAX_DELAY);
		if (bytes_read > 0) {
            
            for (int i = 0; i < bytes_read; i++) {   	
			int byte = chunk_buffer[i];
            
            // Handle Start of Packet (SOP)
            if (!msg_buf.is_receiving_message && byte == (int)SOP) {
            //printf("SOP validate\n");
            init_message_buffer(&msg_buf); // Reset buffer for new message
            msg_buf.is_receiving_message = true;
            msg_buf.rxbuffer[msg_buf.buffer_pos++] = (char)chunk_buffer[SOP_INDEX]; // Store SOP
            msg_buf.last_received_time = time(NULL);
            msg_buf.last_recv_time_ms = current_time_ms();
            //printf("Current time in mS: %lld\n", msg_buf.last_recv_time_ms);
            msg_buf.api_length = chunk_buffer[PAYLOAD_INDEX]; // Store payload length

            // Validate Length
            if (msg_buf.api_length == 0 || msg_buf.api_length > MAX_MESSAGE_SIZE - 4) {// -4 for Header (3bytes -> SOP|Cmd_Type|Length) and EOP
                    printf("Invalid length header: %d\n", msg_buf.api_length);
                    init_message_buffer(&msg_buf);
                    break; // Skip processing this chunk
                }
            continue;//go to next iteration
            }
        	// Receiving message
            if (msg_buf.is_receiving_message) 
			{	
                // Append Data to Buffer
                if (msg_buf.buffer_pos < MAX_MESSAGE_SIZE) 
				{
                    msg_buf.rxbuffer[msg_buf.buffer_pos++] = byte;
                    // Check for End of Packet (EOP)
                    if (msg_buf.buffer_pos == msg_buf.api_length + 4) { // +4 for SOP,CMDtype, Length, and EOP
                        if (msg_buf.rxbuffer[msg_buf.buffer_pos - 1] != (int)EOP) {
                            printf("EOP error, discarding message\n");
                            init_message_buffer(&msg_buf);
                            break;
                        }
                        // Process Complete Message
                        printf("Received complete message (%d bytes): ", msg_buf.api_length);
                        for (int j = 0; j < msg_buf.api_length + 4; j++) { // Skip SOP and Length, +4 for SOP|cmd type|lenght|EOP|
                            printf("0x%02x ",msg_buf.rxbuffer[j]);
                        }
                        printf("\n--------------------------\n");
                        fflush(stdout);
                        
                        memcpy(uart_rx_buf,&msg_buf.rxbuffer[1],msg_buf.api_length + 4);
                        _rx_queue_struct_t  rx_queue_struct;
                        rx_queue_struct.rx_buffer = uart_rx_buf;
                        rx_queue_struct.rx_buf_size = index;
                        BaseType_t high_task_awoken = pdFALSE;
                        xQueueSendFromISR(_rx_queue, &rx_queue_struct, &high_task_awoken);
                        // Reset Buffer
                        init_message_buffer(&msg_buf);
                    }
                }
            }//if
            
        }//for
        
      }//if bytes read
        else if (bytes_read < 0 && errno != EAGAIN) {
            perror("UART read error");
            break;
        }
        long long l_last_recv_time_ms = current_time_ms();

        // Check for timeout in mS
        if (msg_buf.is_receiving_message && l_last_recv_time_ms - msg_buf.last_recv_time_ms > MESSAGE_TIMEOUT_mS) {
            printf("Message timeout, discarding incomplete message\n");
            //printf("timeout check : %lld\n", l_last_recv_time_ms);
            //printf("timeout diff : %lld\n", (l_last_recv_time_ms - msg_buf.last_recv_time_ms));
            init_message_buffer(&msg_buf);
        }

        // Check for timeout in Sec
        /*if (msg_buf.is_receiving_message && time(NULL) - msg_buf.last_received_time > MESSAGE_TIMEOUT) {
            //printf("Message timeout, discarding incomplete message\n");
            //printf("timeout check : %lld\n", l_last_recv_time_ms);
            printf("timeout diff : %lld\n", (l_last_recv_time_ms - msg_buf.last_recv_time_ms));
            init_message_buffer(&msg_buf);
        }*/
        usleep(100);
    }//while 1
#else
    while (Serial.available()) {
        // Read all available bytes
        while (Serial.available() > 0 && index < sizeof(uart_rx_buf)) {
            uart_rx_buf[index++] = Serial.read();
        }
        
        // index = uart_read_bytes(UART_NUM_0, chunk_buffer, RX_CHUNK_SIZE, portMAX_DELAY);

        bytex_rx = index;
        //If data is available
        if(index > 0){
            _rx_queue_struct_t  rx_queue_struct;
            rx_queue_struct.rx_buffer = uart_rx_buf;
            rx_queue_struct.rx_buf_size = index;
            BaseType_t high_task_awoken = pdFALSE;
            xQueueSendFromISR(_rx_queue, &rx_queue_struct, &high_task_awoken);
        }else{
        }
        index = 0;
    }
#endif
}
void uart_tx_handler(_tx_queue_struct_t tx_struct)
{
#ifdef GRL_DBG_LEVEL_1
    Serial.println(__FUNCTION__);
#endif
    switch(tx_struct.cmd_type){
        case SET_CMD:
            set_cmds_fp[tx_struct.selected_id]((set_dd_button_e)tx_struct.selected_id);
        break;
        case GET_CMD:
            get_cmds_fp[tx_struct.selected_id]((get_dd_button_e)tx_struct.selected_id);
        break;
        case PGM_CMD:
        break;
        case POLL_CMD:
            polling_cmd_init(tx_struct);
        break;
        default:
            Serial.println("Invalid API Queued");
        break;
    }
}
void process_polling_cmd(){

}

void process_app_cmd(_rx_queue_struct_t rx_struct)
{
#ifdef GRL_DBG_LEVEL_1
    Serial.println(__FUNCTION__);
#endif
    pop_ui_log(rx_struct);
    switch( (cmd_type_e) rx_struct.rx_buffer[0] & 0x0F){
        case POLL_CMD:
            process_polling_cmd();
        break;
        case SET_CMD:
        break;
        case GET_CMD:
        break;
        case PGM_CMD:
        break;
        default:
            Serial.println("Invalid API received");
        break;
    }
}

void uart_read_task(void *pvParameters)
{
    for (;;)
    {
        lv_timer_handler(); /* let the GUI do its work */
        if(dbg++ == 25){
            dbg = 0;
            update();
        }
        uart_read();
        // delay( 10 );
    }
}
void uart_write_task(void *pvParameters)
{
    _tx_queue_struct_t tx_struct;
    for (;;)
    {
        if (xQueueReceive(_tx_queue, &tx_struct, pdMS_TO_TICKS(20))) {
            uart_tx_handler(tx_struct);
        }else {
            // Serial.println( "write else" );
        }
        // delay( 10 );
    }
}

void process_task(void *pvParameters)
{
    grl_create_widgets();
    xTimerStart(timer_1, pdMS_TO_TICKS(500));
    _rx_queue_struct_t rx_struct;
    for (;;)
    {
        if (xQueueReceive(_rx_queue, &rx_struct, pdMS_TO_TICKS(20))){
            process_app_cmd(rx_struct);
        }else{
            // Serial.println( "process else" );
        }
        // delay( 10 );
    }
}


void grl_display_init(void){

  // Init Display
  lcd.begin();
  lcd.fillScreen(TFT_BLACK);
  // lcd.setTextSize(2);
  delay(200);
  lv_init();
  delay(100);
  touch_init();

  screenWidth = lcd.width();
  screenHeight = lcd.height();

  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, screenWidth * screenHeight / 10);
  //  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, 480 * 272 / 10);
  /* Initialize the display */
  lv_disp_drv_init(&disp_drv);
  /* Change the following line to your display resolution */
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /* Initialize the (dummy) input device driver */
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

}
