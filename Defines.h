#ifndef DEFINES_H
#define DEFINES_H

#define TFT_BL 2
#define VOLTAGE_PLOT
#define CURRENT_PLOT
#define STR_USE

#define TXD_PIN     GPIO_NUM_44
#define RXD_PIN     GPIO_NUM_43
#define NUM_SET_APIS_DD     12   
#define NUM_GET_APIS_DD     12 
// #define GRL_DBG_LEVEL_1         1
#define RX_QUEUE_ELEMENTS_MAX   12
#define TX_QUEUE_ELEMENTS_MAX   12

#define SET_DD_OPTIONS  "Attach\n"  \
                        "Detach\n"  \
                        "Set Active CC"

#define GET_DD_OPTIONS  "Get Src caps\n"    \
                        "Get FW version\n"  \
                        "Get PDC Details\n"   \
                        "Get Vbus Data\n"   \
                        "Get Vconn Data"    
#endif // DEFINES_H

