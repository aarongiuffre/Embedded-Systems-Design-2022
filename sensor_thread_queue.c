/*
 * sensor_threadqueue.c
 *
 *  Created on: Feb 7, 2022
 *      Author: aaron
 */

//#include <sensor_thread_queue.h>
//#include <generic_messaging.h>
//#include "debug.h"
//
//extern QueueHandle_t sensor_queue_handle;
//
//void sensor_queue_send(sensor_message_t *sensor_msg_ptr)
//{
//     dbgEvent(DBG_SENSOR_TASK_QUEUE_WRITE_S);
//     if (generic_queue_send(sensor_queue_handle, sensor_msg_ptr) != pdTRUE)
//     {
//         dbgEventFail(DBG_ERR_FATAL_ERROR);
//     }
//     dbgEvent(DBG_SENSOR_TASK_QUEUE_WRITE_E);
//}
//
//void sensor_queue_receive(sensor_message_t *sensor_msg_ptr)
//{
//    dbgEvent(DBG_SENSOR_TASK_QUEUE_READ_S);
//    if (generic_queue_receive(sensor_queue_handle, sensor_msg_ptr) != pdTRUE)
//        dbgEventFail(DBG_ERR_FATAL_ERROR);
//    dbgEvent(DBG_SENSOR_TASK_QUEUE_READ_E);
//}
//
//void sensor_thread_queue_create()
//{
//    sensor_queue_handle = xQueueCreate(SENSOR_QUEUE_LEN, sizeof(sensor_message_t));
//    if(sensor_queue_handle == NULL)
//    {
//        dbgEventFail(DBG_ERR_FATAL_ERROR);
//    }
//}
