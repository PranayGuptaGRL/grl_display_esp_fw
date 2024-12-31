#include "main.h"
#include <lvgl.h>
#include "DisplayApp.h"
#include "grl_apis.h"

void grl_init(){

  memset(panel_obj_p, 0, sizeof(panel_obj_t));
  // Assign the addresses of the static instances to the pointers
  panel_obj_p->over_view_tab_s.sys_info_panel = &sys_info_panel_instance;
  panel_obj_p->over_view_tab_s.port_sts_panel = &port_sts_panel_instance;
  panel_obj_p->log_tab_s.log_panel = &log_panel_instance;
  // Initialize the nested structures
  memset(panel_obj_p->over_view_tab_s.sys_info_panel, 0, sizeof(sys_info_panel_t));
  memset(panel_obj_p->over_view_tab_s.port_sts_panel, 0, sizeof(port_sts_panel_t));
  memset(panel_obj_p->log_tab_s.log_panel, 0, sizeof(log_panel_t));

  init_get_cmds_fp();
  init_set_cmd_fp();
}

void grl_timer_init(){

	timer_1 = xTimerCreate("tmr_smf", pdMS_TO_TICKS(500), false, 0, Timer0_ISR);

}
void create_tasks(){
  xTaskCreate(process_task, "UI Process Task", 10000, NULL, process_task_priority, &xHandle1);
  xTaskCreate(uart_read_task, "Read Task", 10000, NULL, read_task_priority, &xHandle2);
  xTaskCreate(uart_write_task, "Write Task", 10000, NULL, write_task_priority, &xHandle3);
}

void setup()
{
  Serial.begin(115200);
  Wire.begin(19, 20);
  dht20.begin();

  //IO
  pinMode(38, OUTPUT);
  digitalWrite(38, LOW);

  grl_init();
  grl_display_init();
  grl_timer_init();
#ifdef TFT_BL
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
#endif

  // Create mutex before starting tasks that shall be used for protecting uart writes
  uart_write_mutex = xSemaphoreCreateMutex();

  /***Queues creating for handling Rx and Tx events */
  _tx_queue = xQueueCreate(TX_QUEUE_ELEMENTS_MAX, sizeof(_tx_queue_struct_t));
  if (!_tx_queue) {
    Serial.println("TX Queue Error\n");
    return;
  }
  _rx_queue = xQueueCreate(RX_QUEUE_ELEMENTS_MAX, sizeof(_rx_queue_struct_t));
  if (!_tx_queue) {
    Serial.println("RX Queue Error\n");
    return;
  } 
  create_tasks();
}

void loop()
{

}
