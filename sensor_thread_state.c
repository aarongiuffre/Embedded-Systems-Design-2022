/*
 * sensor_thread_state.c
 *
 *  Created on: Feb 7, 2022
 *      Author: aaron
 */

//#include <FreeRTOS.h>
//#include <sensor_thread_state.h>
//#include <sensor_thread_queue.h>
//#include "init_main.h"
//#include "globals.h"
//#include "mqtt_stats.h"
//
//extern MQTTClient_Handle mqttClientHandle;
//
//void sensor_FSM(sensor_message_t *sensor_msg_ptr) {
//    static sensor_state_t state = INIT_AVERAGE;
//    static uint16_t sensorTotal = 0;
//    static uint16_t sensorCount = 0;
//    static uint32_t timeElapsed = 0;
//    static char buff[MQTT_MSG_BUFFER_LEN];
//    static int seq_num = 0;
//
//    if (sensor_msg_ptr->type == TIMER500_TYPE)
//        timeElapsed += sensor_msg_ptr->data;
//
//    switch(state)
//    {
//        case INIT_AVERAGE:
//        {
//            sensorTotal = 0;
//            sensorCount = 0;
//
//            if (sensor_msg_ptr->type == TIMER500_TYPE)
//                state = UPDATE_AVERAGE;
//
//            break;
//        }
//        case UPDATE_AVERAGE:
//        {
//
//            if (sensor_msg_ptr->type == TIMER500_TYPE)
//            {
//                state = INIT_AVERAGE;
//                float average = sensorTotal / (float) sensorCount;
//                size_t bytes = snprintf(buff, MQTT_MSG_BUFFER_LEN, "{\"Avg\":%f, \"Time\":%d, \"SeqNum\": %d}", average, timeElapsed, seq_num);
//                MQTT_IF_Publish(mqttClientHandle,
//                                            "task1/SensorData",
//                                            buff,
//                                            bytes,
//                                            MQTT_QOS_0);
//                stats_message_t stats_msg;
//                stats_msg.type = PUB;
//                stats_queue_send(&stats_msg);
//                seq_num ++;
//            }
//            else
//            {
//                sensorTotal += sensor_msg_ptr->data;
//                sensorCount++;
//                size_t bytes = snprintf(buff, MQTT_MSG_BUFFER_LEN, "{\"Sensor\":%d, \"SeqNum\": %d}", sensor_msg_ptr->data, seq_num);
//                MQTT_IF_Publish(mqttClientHandle,
//                                            "task1/SensorData",
//                                            buff,
//                                            bytes,
//                                            MQTT_QOS_0);
//                stats_message_t stats_msg;
//                stats_msg.type = PUB;
//                stats_queue_send(&stats_msg);
//                seq_num ++;
//            }
//            break;
//        }
//    }
//}



