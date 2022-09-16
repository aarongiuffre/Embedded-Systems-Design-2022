/*
 * sensor_thread_queue.h
 *
 *  Created on: Feb 7, 2022
 *      Author: aaron
 */

//#ifndef SENSOR_THREAD_QUEUE_H_
//#define SENSOR_THREAD_QUEUE_H_
//#define SENSOR_QUEUE_LEN 30
//
//#include <stdbool.h>
//#include "FreeRTOS.h"
//#include "queue.h"
//#include <generic_messaging.h>
//#include "debug.h"
//
////QueueHandle_t sensor_queue_handle = NULL;
//
//typedef enum {
//    TIMER70_TYPE,
//    TIMER500_TYPE,
//    NONE_TYPE
//} sensor_message_type_t;
//
//typedef struct sensor_message_t {
//    sensor_message_type_t type;
//    int data; //Timer70 = mm, Timer500 = us
//} sensor_message_t;
//
//void sensor_thread_queue_create();
//
//void sensor_queue_send(sensor_message_t * );
//
//void sensor_queue_receive(sensor_message_t * );
//
//#endif /* SENSOR_THREAD_QUEUE_H_ */
