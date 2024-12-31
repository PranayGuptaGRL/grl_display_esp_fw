#ifndef STRUCTS_H
#define STRUCTS_H

#include"Includes.h"
#include"Defines.h"
#include"Structs.h"


class LGFX : public lgfx::LGFX_Device
{
public:
    lgfx::Bus_RGB _bus_instance;   // Bus instance
    lgfx::Panel_RGB _panel_instance;  // Panel instance

    LGFX(void);  // Constructor declaration
};

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    DISP_SMALL,
    DISP_MEDIUM,
    DISP_LARGE,
} disp_size_t;

typedef struct sys_info_panel_struct{
    lv_obj_t * sys_info_panel_obj;
    lv_obj_t * fw_ver_num_label;
    lv_obj_t * ip_add_label;
    lv_obj_t * eload_ver_num_label;
}sys_info_panel_t;

typedef struct port_sts_panel_struct{
    lv_obj_t * port_sts_panel_obj;
    lv_obj_t * tester_port_role;
    lv_obj_t * dut_port_role;
    lv_obj_t * vconn_provider;
    lv_obj_t * cc_pol;
    lv_obj_t * vbus_vol;
    lv_obj_t * vbus_curr;
}port_sts_panel_t;

typedef struct log_panel_struct{
    lv_obj_t * log_panel_obj;
    lv_obj_t * fw_logging_label;
}log_panel_t;


typedef struct dynamic_panel_objects{
    
    lv_obj_t * grl_main_panel;
    lv_obj_t * grl_logo_panel;
    
    struct{
        lv_obj_t * over_view_tab;
        sys_info_panel_t * sys_info_panel;
        port_sts_panel_t * port_sts_panel; 
    }over_view_tab_s;

    struct{
        lv_obj_t * analysis_tab;
    }analysis_tab_s;
    
    struct{
        lv_obj_t * prt_ctrl_tab;
    }port_ctrl_tab_s;

    struct{
        lv_obj_t * log_tab;
        log_panel_t * log_panel;
    }log_tab_s;

}panel_obj_t;

typedef enum{
    DD_ATTACH_SET = 0,
    DD_DETACH_SET = 1,
    DD_PORT_ROLE_SET = 2,
}set_dd_button_e;

typedef enum{
    DD_GET_SRC_CAPS = 0,
    DD_GET_FW_VESION = 1,
    DD_GET_PDC_DETAILS = 2,
    DD_GET_VBUS_DATA   =3,
    DD_GET_VCONN_DATA   =4,
}get_dd_button_e;

typedef enum{
    SET_CMD = 1,
    PGM_CMD = 2,
    POLL_CMD = 3,
    GET_CMD = 7,
}cmd_type_e;

typedef struct{
  uint8_t *tx_buffer;
  uint8_t tx_buf_size;
  cmd_type_e cmd_type;
  uint8_t selected_id;
}_tx_queue_struct_t;

typedef struct{
  uint8_t *rx_buffer;
  uint8_t rx_buf_size;
}_rx_queue_struct_t;


extern LGFX lcd;
extern sys_info_panel_t sys_info_panel_instance;
extern port_sts_panel_t port_sts_panel_instance;
extern log_panel_t log_panel_instance;

extern panel_obj_t *panel_obj_p;
extern uint8_t uart_rx_buf[512];
extern uint8_t bytex_rx;
extern SemaphoreHandle_t uart_write_mutex;
extern QueueHandle_t _tx_queue;
extern QueueHandle_t _rx_queue;


#endif // STRUCTS_H
