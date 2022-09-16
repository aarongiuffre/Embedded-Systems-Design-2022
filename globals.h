/*
 * globals.h
 *
 *  Created on: Feb 19, 2022
 *      Author: acemc
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_


#include <ti/drivers/UART.h>
#include "FreeRTOS.h"
#include <queue.h>
#include <ti/net/mqtt/mqttclient.h>

/* Stack size in bytes */
#define THREADSTACKSIZE   3072

QueueHandle_t mqttQueueHandle;
//QueueHandle_t sensor_queue_handle;
//QueueHandle_t chain_queue_handle;
//QueueHandle_t stats_queue_handle;

UART_Handle uart;

MQTTClient_Handle mqttClientHandle;

/* -- ONLY UNCOMMENT ONE NAME -- */
#define AARON
//#define ANDREW
//#define HANI
//#define UJJWAL
/* ----------------------------- */





/* ---- Derived Definitions ---- */
#ifdef AARON
    #define CHAIN_1
    #define B1
#endif
#ifdef ANDREW
    #define CHAIN_2
    #define B2
#endif
#ifdef HANI
    #define CHAIN_3
    #define B3
#endif
#ifdef UJJWAL
    #define CHAIN_4
    #define B4
#endif

#if defined(AARON) || defined(HANI)
    #define TASK1_VER2
#else
    #define TASK1_VER11
#endif
/* ----------------------------- */




/* ----- Other Definitions ----- */
#define MQTT_MSG_BUFFER_LEN 128
/* ----------------------------- */
#endif /* GLOBALS_H_ */
