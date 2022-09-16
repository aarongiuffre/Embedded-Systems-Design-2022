/*
 * mqtt_stats.h
 *
 *  Created on: Mar 4, 2022
 *      Author: aaron
 */

//#ifndef MQTT_STATS_H_
//#define MQTT_STATS_H_
//#define STATS_QUEUE_LEN 2
//
//#include <stdio.h>
//#include "FreeRTOS.h"
//#include "queue.h"
//#include "generic_messaging.h"
//#include "debug.h"
//#include "globals.h"
//#include "init_main.h"
//
////static stats_message_t stats_msg;
////stats_msg.type = PUB;
////stats_queue_send(&stats_msg);
//
//typedef enum {
//    PUB,
//    SUB
//} stats_message_type_t;
//
//typedef struct stats_message_t {
//    stats_message_type_t type;
//} stats_message_t;
//
//void stats_thread_queue_create();
//
//void stats_queue_send(stats_message_t * );
//
//void stats_queue_receive(stats_message_t * );
//
//void *read_stats(void *arg0);
//
//#endif /* MQTT_STATS_H_ */
