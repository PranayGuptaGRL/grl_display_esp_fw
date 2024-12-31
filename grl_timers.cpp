#include "grl_timers.h"
#include "DisplayApp.h"
TimerHandle_t timer_1 = NULL;
uint8_t polling_buf[] = {';',0x13,0x02,0x00,0x01,';'};

// void sendBufferAsString(const uint8_t *buf, size_t len) {
//     for (size_t i = 0; i < len; ++i) {
//         Serial.print(buf[i], HEX);
//         // Serial.print(" ");
//     }
//     // Serial.println();
// }
// size_t grl_uart_write(uint8_t *atx_buf, size_t abuf_size)
// {
//     char lbuf[256]={0};
//     size_t bytes_sent = 0;
//     memcpy(lbuf,atx_buf,abuf_size);
//     bytes_sent = Serial.println(lbuf);
//     return bytes_sent;
// }


void Timer0_ISR(void *z )
{
    size_t bytes_sent = 0;

    if(g_button_pressed)
    {
        g_button_pressed = false;
        // for(int i =0; i<sizeof(polling_buf);i++){
        //     polling_buf[i] = i+1;
        // }
        // polling_buf[0] = ';';
        // polling_buf[1] = 0x13;
        // polling_buf[2] = sizeof(polling_buf)-4;
        // polling_buf[sizeof(polling_buf) - 1] = ';';

        _tx_queue_struct_t tx_struct;
        tx_struct.cmd_type = POLL_CMD;
        tx_struct.tx_buffer = polling_buf;
        tx_struct.tx_buf_size = sizeof(polling_buf);
        BaseType_t high_task_awoken = pdFALSE;
        xQueueSendFromISR(_tx_queue, &tx_struct, &high_task_awoken);
    }
    xTimerStart(timer_1, pdMS_TO_TICKS(50));//50 mS timer start
}