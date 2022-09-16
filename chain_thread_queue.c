/*
 * chain_thread_queue.c
 *
 *  Created on: Mar 2, 2022
 *      Author: acemc
 */

//#include <chain_thread_queue.h>
//#include <generic_messaging.h>
//#include "debug.h"
//
//extern QueueHandle_t chain_queue_handle;
//
//void chain_queue_send(chain_message_t *chain_message_ptr)
//{
//     dbgEvent(DBG_CHAIN_TASK_QUEUE_WRITE_S);
//     if (generic_queue_send(chain_queue_handle, chain_message_ptr) != pdTRUE)
//     {
//         dbgEventFail(DBG_ERR_FATAL_ERROR);
//     }
//     dbgEvent(DBG_CHAIN_TASK_QUEUE_WRITE_E);
//}
//
//void chain_queue_receive(chain_message_t *chain_message_ptr)
//{
//    dbgEvent(DBG_CHAIN_TASK_QUEUE_READ_S);
//    if (generic_queue_receive(chain_queue_handle, chain_message_ptr) != pdTRUE)
//        dbgEventFail(DBG_ERR_FATAL_ERROR);
//    dbgEvent(DBG_CHAIN_TASK_QUEUE_READ_E);
//}
//
//void chain_thread_queue_create()
//{
//    chain_queue_handle = xQueueCreate(CHAIN_QUEUE_LEN, sizeof(chain_message_t));
//    if(chain_queue_handle == NULL)
//    {
//        dbgEventFail(DBG_ERR_FATAL_ERROR);
//    }
//}
