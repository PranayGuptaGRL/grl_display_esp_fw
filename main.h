
#ifndef MAIN_H
#define MAIN_H

#include "Defines.h"
#include "Includes.h"
#include "Structs.h"
#include "grl_timers.h"

const UBaseType_t process_task_priority = 1;
const UBaseType_t read_task_priority = 2;
const UBaseType_t write_task_priority = 3;

const UBaseType_t DataStrmTaskPriority = 2;

TaskHandle_t xHandle1 = NULL;
TaskHandle_t xHandle2 = NULL;
TaskHandle_t xHandle3 = NULL;
TaskHandle_t DataStrmHandler = NULL;

//UI
int led;
DHT20 dht20;
SPIClass& spi = SPI;

#endif // MAIN_H


