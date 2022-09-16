/*
 * generic_messaging.c
 *
 *  Created on: Feb 9, 2022
 *      Author: aaron
 */

//#include "FreeRTOS.h"
//#include <generic_messaging.h>
//#include <ti/drivers/dpl/HwiP.h>
//
//void generic_queue_create(QueueHandle_t *queue_handle, int queue_len, void *msg_ptr)
//{
//    *queue_handle = xQueueCreate(queue_len, sizeof(msg_ptr));
//    if(queue_handle == NULL)
//    {
//        dbgEventFail(DBG_ERR_OVERFLOW);
//    }
//}
//
//BaseType_t generic_queue_send(QueueHandle_t queue_handle, void *msg_ptr)
//{
//
//     if(HwiP_inISR())
//     {
//         return xQueueSendFromISR(queue_handle, msg_ptr, 0);
//     }
//     else
//     {
//         return xQueueSend(queue_handle, msg_ptr, portMAX_DELAY);
//     }
//}
//
//BaseType_t generic_queue_receive(QueueHandle_t queue_handle, void *msg_ptr)
//{
//    if(HwiP_inISR())
//    {
//        return xQueueReceiveFromISR(queue_handle, msg_ptr, 0);
//    }
//    else
//    {
//        return xQueueReceive(queue_handle, msg_ptr, portMAX_DELAY);
//    }
//}
