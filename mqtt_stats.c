/*
 * mqtt_stats.c
 *
 *  Created on: Mar 4, 2022
 *      Author: aaron
 */

//#include "mqtt_stats.h"
//#include "globals.h"
//
//extern QueueHandle_t stats_queue_handle;
//
//void stats_queue_send(stats_message_t *stats_msg_ptr)
//{
//     dbgEvent(DBG_STATS_TASK_QUEUE_WRITE_S);
//     if (generic_queue_send(stats_queue_handle, stats_msg_ptr) != pdTRUE)
//     {
//         dbgEventFail(DBG_ERR_FATAL_ERROR);
//     }
//     dbgEvent(DBG_STATS_TASK_QUEUE_WRITE_E);
//}
//
//void stats_queue_receive(stats_message_t *stats_msg_ptr)
//{
//    dbgEvent(DBG_STATS_TASK_QUEUE_READ_S);
//    if (generic_queue_receive(stats_queue_handle, stats_msg_ptr) != pdTRUE)
//        dbgEventFail(DBG_ERR_FATAL_ERROR);
//    dbgEvent(DBG_STATS_TASK_QUEUE_READ_E);
//}
//
//void stats_thread_queue_create()
//{
//    stats_queue_handle = xQueueCreate(STATS_QUEUE_LEN, sizeof(stats_message_t));
//    if(stats_queue_handle == NULL)
//    {
//        dbgEventFail(DBG_ERR_FATAL_ERROR);
//    }
//}
//
//void *read_stats(void *arg0)
//{
//#ifdef B1
//    static char *pub_topic = "PUB_B1";
//    static char *sub_topic = "SUB_B1";
//#endif
//#ifdef B2
//    static char *pub_topic = "PUB_B2";
//    static char *sub_topic = "SUB_B2";
//#endif
//#ifdef B3
//    static char *pub_topic = "PUB_B3";
//    static char *sub_topic = "SUB_B3";
//#endif
//#ifdef B4
//    static char *pub_topic = "PUB_B4";
//    static char *sub_topic = "SUB_B4";
//#endif
//
//    static stats_message_t stats_msg;
//    static char buff[MQTT_MSG_BUFFER_LEN];
//    static int seq_num = 0;
//
//    while (1)
//    {
//        stats_queue_receive(&stats_msg);
//        if(stats_msg.type == PUB)
//        {
//           size_t bytes = snprintf(buff, MQTT_MSG_BUFFER_LEN, "{\"Type\":%d, \"SeqNum\": %d}", stats_msg.type, seq_num);
//           MQTT_IF_Publish(mqttClientHandle,
//                                       pub_topic,
//                                       buff,
//                                       bytes,
//                                       MQTT_QOS_0);
//           seq_num ++;
//        }
//        else if(stats_msg.type == SUB)
//        {
//           size_t bytes = snprintf(buff, MQTT_MSG_BUFFER_LEN, "{\"Type\":%d, \"SeqNum\": %d}", stats_msg.type, seq_num);
//           MQTT_IF_Publish(mqttClientHandle,
//                                       sub_topic,
//                                       buff,
//                                       bytes,
//                                       MQTT_QOS_0);
//           seq_num ++;
//        }
//    }
//}

