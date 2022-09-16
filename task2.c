/*
 * task2.c
 *
 *  Created on: Mar 2, 2022
 *      Author: acemc
 */

/*

#include "FreeRTOS.h"
#include <string.h>
#define JSMN_HEADER
#include "jsmn.h"
#include "json.h"
#include "task2.h"
#include <string.h>
#include "mqtt_stats.h"
#include "debug.h"

void Chain_Callback(char* topic, char* payload) {
    static chain_message_t chain_message;

    jsmn_parser p;
    static jsmntok_t tokens[MAX_TOKENS];
    jsmn_init(&p);
    int res = jsmn_parse(&p, payload, strlen(payload), tokens, MAX_TOKENS);
    if (res < 0) {
        return;
    }

    if (res == 5) {
        chain_message.data = parse_int_token(tokens[2], payload);
        if (chain_message.data == INT_MIN) {
            return;
        }
    } else {
        return;
    }

    chain_queue_send(&chain_message);
    stats_message_t stats_msg;
    stats_msg.type = SUB;
    stats_queue_send(&stats_msg);
}

void * task_two(void *args) {

    dbgEvent(DBG_TASK_TWO_START);

    static chain_message_t chain_message;
    static char buff[MQTT_MSG_BUFFER_LEN];
    static int seq_num = 0;

#ifdef CHAIN_1
    static char *topic = "task2/Chain2";
    static const int multiplier = 2;
#endif
#ifdef CHAIN_2
    static char *topic = "task2/Chain3";
    static const int multiplier = 3;
#endif
#ifdef CHAIN_3
    static char *topic = "task2/Chain4";
    static const int multiplier = 5;
#endif
#ifdef CHAIN_4
    static char *topic = "task2/ChainOut";
    static const int multiplier = 7;
#endif

    while (1) {
        chain_queue_receive(&chain_message);

        size_t bytes = snprintf(buff, MQTT_MSG_BUFFER_LEN, "{\"Data\": %d, \"SeqNum\": %d}", chain_message.data * multiplier, seq_num);
        MQTT_IF_Publish(mqttClientHandle,
                        topic,
                        buff,
                        bytes,
                        MQTT_QOS_0);
        stats_message_t stats_msg;
        stats_msg.type = PUB;
        stats_queue_send(&stats_msg);
        seq_num ++;
    }
}

*/
