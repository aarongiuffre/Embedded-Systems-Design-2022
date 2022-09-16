/*
 * debug.h
 *
 *  Created on: Feb 3, 2022
 *      Author: ujjwal Jain
 */

#ifndef DEBUG_H_
#define DEBUG_H_

//ERROR CODES
#define DBG_ERR_FATAL_ERROR             0x00
#define DBG_ERR_PTHREAD_ATTR_INIT       0x01
#define DBG_ERR_PTHREAD_CREATE          0x02
#define DBG_ERR_OVERFLOW                0x03
#define DBG_ERR_UART_START_FAILURE      0x04
#define DBG_ERR_QUEUE_HANDLER_READ_FAIL 0x05
#define DBG_ERR_TIMER_START_FAILURE     0x06
#define DBG_ERR_ADC_START_FAILURE       0x07
#define DBG_ERR_MQTT_MAX_TOPIC_NODES    0x08
#define DBG_ERR_MQTT_ORPHANED_NODE      0x09

/*
 * KEY:
 * - xxx_QUEUE_(READ/WRITE)_S = Start (before) Queue read/write
 * - xxx_QUEUE_(READ/WRITE)_E = End (after) Queue read/write
 */
//TIMER70 EVENTS
#define DBG_TIMER_70_START            0x00
#define DBG_TIMER_70_QUEUE_WRITE_S    0x01
#define DBG_TIMER_70_QUEUE_WRITE_E    0x02
#define DBG_TIMER_70_END              0x03

//TIMER500 EVENTS
#define DBG_TIMER_500_START           0x04
#define DBG_TIMER_500_QUEUE_WRITE_S   0x05
#define DBG_TIMER_500_QUEUE_WRITE_E   0x06
#define DBG_TIMER_500_END             0x07

//STATE MACHINE EVENTS
#define DBG_SENSOR_FSM_START          0x08
#define DBG_SENSOR_TASK_QUEUE_WRITE_S 0x09
#define DBG_SENSOR_TASK_QUEUE_WRITE_E 0x0A
#define DBG_SENSOR_TASK_QUEUE_READ_S  0x0B
#define DBG_SENSOR_TASK_QUEUE_READ_E  0x0C
#define DBG_SENSOR_FSM_END            0x0D

//STATS EVENTS
#define DBG_STATS_TASK_QUEUE_READ_S   0x0E
#define DBG_STATS_TASK_QUEUE_READ_E   0x0F
#define DBG_STATS_TASK_QUEUE_WRITE_S  0x10
#define DBG_STATS_TASK_QUEUE_WRITE_E  0x11

#define DBG_TASK_ONE_START            0x12
#define DBG_TASK_TWO_START            0x13

#define DBG_CHAIN_TASK_QUEUE_WRITE_S  0x14
#define DBG_CHAIN_TASK_QUEUE_WRITE_E  0x15
#define DBG_CHAIN_TASK_QUEUE_READ_S   0x16
#define DBG_CHAIN_TASK_QUEUE_READ_E   0x17

void dbgGPIOtest();
void dbgByte (unsigned int byte);
void dbgEvent (unsigned int event);
void dbgEventFail (unsigned char error_msg);

#endif /* DEBUG_H_ */
