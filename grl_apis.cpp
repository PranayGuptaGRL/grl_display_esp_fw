
#include"grl_apis.h"
#include"DisplayApp.h"

int (*set_cmds_fp[NUM_SET_APIS_DD])(set_dd_button_e selected_id) = {NULL};

int (*get_cmds_fp[NUM_GET_APIS_DD])(get_dd_button_e selected_id) = {NULL};

int polling_cmd_init(_tx_queue_struct_t tx_struct){
#ifdef GRL_DBG_LEVEL_1
    Serial.println(__FUNCTION__);
#endif
    return grl_uart_write(UART_NUM_0, tx_struct.tx_buffer,tx_struct.tx_buf_size);
}

/***SET APIS FRAMING and INIT */
int dd_attach_api_init(set_dd_button_e abutton_sel){
#ifdef GRL_DBG_LEVEL_1
    Serial.println(__FUNCTION__);
#endif
    return grl_uart_write(UART_NUM_0, dd_set_api[abutton_sel],dd_set_api_lengths[abutton_sel]);
}

int dd_detach_api_init(set_dd_button_e abutton_sel){
#ifdef GRL_DBG_LEVEL_1
    Serial.println(__FUNCTION__);
#endif
    return grl_uart_write(UART_NUM_0, dd_set_api[abutton_sel],dd_set_api_lengths[abutton_sel]);
}

int dd_port_role_set_init(set_dd_button_e abutton_sel){
#ifdef GRL_DBG_LEVEL_1
    Serial.println(__FUNCTION__);
#endif
    return grl_uart_write(UART_NUM_0, dd_set_api[abutton_sel],dd_set_api_lengths[abutton_sel]);
}


/***GET APIS FRAMING and INIT */
int dd_get_src_caps_init(get_dd_button_e abutton_sel){
#ifdef GRL_DBG_LEVEL_1
    Serial.println(__FUNCTION__);
#endif
    return grl_uart_write(UART_NUM_0, dd_get_api[abutton_sel],dd_get_api_lengths[abutton_sel]);
}

int dd_get_fw_v_init(get_dd_button_e abutton_sel){
#ifdef GRL_DBG_LEVEL_1
    Serial.println(__FUNCTION__);
#endif
    return grl_uart_write(UART_NUM_0, dd_get_api[abutton_sel],dd_get_api_lengths[abutton_sel]);
}

int dd_get_pdc_dets_init(get_dd_button_e abutton_sel){
#ifdef GRL_DBG_LEVEL_1
    Serial.println(__FUNCTION__);
#endif
    return grl_uart_write(UART_NUM_0, dd_get_api[abutton_sel],dd_get_api_lengths[abutton_sel]);
}

int dd_get_vbus_data_init(get_dd_button_e abutton_sel){
#ifdef GRL_DBG_LEVEL_1
    Serial.println(__FUNCTION__);
#endif
    return grl_uart_write(UART_NUM_0, dd_get_api[abutton_sel],dd_get_api_lengths[abutton_sel]);
}

int dd_get_vconn_data_init(get_dd_button_e abutton_sel){
#ifdef GRL_DBG_LEVEL_1
    Serial.println(__FUNCTION__);
#endif
    return grl_uart_write(UART_NUM_0, dd_get_api[abutton_sel],dd_get_api_lengths[abutton_sel]);
}

void init_set_cmd_fp()
{
    set_cmds_fp[DD_ATTACH_SET] = dd_attach_api_init;
    set_cmds_fp[DD_DETACH_SET] = dd_detach_api_init;
    set_cmds_fp[DD_PORT_ROLE_SET] = dd_port_role_set_init;
}

void init_get_cmds_fp()
{
    get_cmds_fp[DD_GET_SRC_CAPS] = dd_get_src_caps_init;
    get_cmds_fp[DD_GET_FW_VESION] = dd_get_fw_v_init;
    get_cmds_fp[DD_GET_PDC_DETAILS] = dd_get_pdc_dets_init;
    get_cmds_fp[DD_GET_VBUS_DATA] = dd_get_vbus_data_init;
    get_cmds_fp[DD_GET_VCONN_DATA] = dd_get_vconn_data_init;
}