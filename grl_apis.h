#include "Includes.h"
#include"Structs.h"
/**Set API init functions declarations */
int dd_attach_api_init(set_dd_button_e abutton_sel);
int dd_detach_api_init(set_dd_button_e abutton_sel);
int dd_port_role_set_init(set_dd_button_e abutton_sel);

/**Get API init functions declarations */
int dd_get_src_caps_init(get_dd_button_e abutton_sel);
int dd_get_fw_v_init(get_dd_button_e abutton_sel);
int dd_get_pdc_dets_init(get_dd_button_e abutton_sel);
int dd_get_vbus_data_init(get_dd_button_e abutton_sel);
int dd_get_vconn_data_init(get_dd_button_e abutton_sel);
void init_get_cmds_fp();
void init_set_cmd_fp();
int polling_cmd_init(_tx_queue_struct_t tx_struct);

static const uint8_t arr_attach_api[] = {';',0x01,0x09,0x05,0x02,0x01,';'};
static const uint8_t arr_detach_api[] = {';',0x01,0x03,0x05,0x02,0x00,';'};
static const uint8_t arr_port_role_set_api[] = {';',0x01,0x03,0x05,0x01,0x01,';'};

static const uint8_t *dd_set_api[] = {arr_attach_api, 
                                        arr_detach_api, 
                                        arr_port_role_set_api};
// Array lengths for dynamic size determination
static const size_t dd_set_api_lengths[] = {
    // sizeof(arr_attach_api) / sizeof(arr_attach_api[1]),
    // sizeof(arr_detach_api) / sizeof(arr_detach_api[1]),
    // sizeof(arr_port_role_set_api) / sizeof(arr_port_role_set_api[1])
    sizeof(arr_attach_api),
    sizeof(arr_detach_api),
    sizeof(arr_port_role_set_api)
};

static const uint8_t arr_get_srccaps_api[] = {';',0x17,0x02,0x05,0x01,';'};
static const uint8_t arr_get_fwv_api[] = {';',0x17,0x02,0x05,0x02,';'};
static const uint8_t arr_getpdc_dets_api[] = {';',0x17,0x02,0x05,0x03,';'};
static const uint8_t arr_get_vbusdata_api[] = {';',0x17,0x02,0x05,0x04,';'};
static const uint8_t arr_get_vconndata_api[] = {';',0x17,0x02,0x05,0x05,';'};

static const uint8_t *dd_get_api[] = {arr_get_srccaps_api, 
                                        arr_get_fwv_api, 
                                        arr_getpdc_dets_api,
                                        arr_get_vbusdata_api,
                                        arr_get_vconndata_api };

// Array lengths for dynamic size determination
static const size_t dd_get_api_lengths[] = {
    sizeof(arr_get_srccaps_api) / sizeof(arr_get_srccaps_api[1]),
    sizeof(arr_get_fwv_api) / sizeof(arr_get_fwv_api[1]),
    sizeof(arr_getpdc_dets_api) / sizeof(arr_getpdc_dets_api[1]),
    sizeof(arr_get_vbusdata_api) / sizeof(arr_get_vbusdata_api[1]),
    sizeof(arr_get_vconndata_api) / sizeof(arr_get_vconndata_api[1])

};

extern int (*set_cmds_fp[NUM_SET_APIS_DD])(set_dd_button_e selected_id);
extern int (*get_cmds_fp[NUM_GET_APIS_DD])(get_dd_button_e selected_id);