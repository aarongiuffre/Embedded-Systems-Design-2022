/*
 * task1.c
 *
 *  Created on: Mar 1, 2022
 *      Author: acemc
 */

//#include "FreeRTOS.h"
//#include <string.h>
//#define JSMN_HEADER
//#include "jsmn.h"
//#include "json.h"
//#include "task1.h"
//#include "sensor_thread_queue.h"
//#include "sensor_thread_state.h"
//#include "debug_if.h"
//#include "init_main.h"
//#include "globals.h"
//#include "mqtt_stats.h"

//int lerp(int inp, int low_bounds, int high_bounds, float delta)
//{
//    return delta * (inp - DIST_B) / (high_bounds - low_bounds);
//}

//void v2Timer_Callback(Timer_Handle myHandle, int_fast16_t status) {
//    static sensor_message_t sensor_msg;
//    sensor_msg.type = NONE_TYPE;
//    sensor_queue_send(&sensor_msg);
//}

//void v2MQTT_Callback(char* topic, char* payload) {
//    LOG_TRACE("RECIEVED[%s]: %s\n\r", topic, payload);
//
//    static sensor_message_t sensor_msg;
//    jsmn_parser p;
//    static jsmntok_t tokens[MAX_TOKENS];
//    jsmn_init(&p);
//    int res = jsmn_parse(&p, payload, strlen(payload), tokens, MAX_TOKENS);
//    if (res < 0) {
//        return;
//    }//else correct parse
//
//    if (res == 5) {
//        //sensor reading msg
//        int data = parse_int_token(tokens[2], payload);
//        if (data == INT_MIN) {
//            return;
//        }
//        LOG_TRACE("SENSOR READING: %d mm\n\r", data); //send a TIMER70_TYPE msg with data=sensor
//        sensor_msg.type = TIMER70_TYPE;
//        sensor_msg.data = data;
//    } else if (res == 7) {
//        //avg/time mesage
//        int avg = parse_int_token(tokens[2], payload);
//        if (avg == INT_MIN) {
//            return;
//        }
//        int time = parse_int_token(tokens[4], payload);
//        if (time == INT_MIN) {
//            return;
//        }
//        LOG_TRACE("AVERAGE: %d mm\t\tTIME: %d ms\n\r", avg, time); //send a TIMER500_TYPE msg with data=avg
//        sensor_msg.type = TIMER500_TYPE;
//        sensor_msg.data = avg;
//    } else {
//        return;
//    }
//
//    sensor_queue_send(&sensor_msg);
//    stats_message_t stats_msg;
//    stats_msg.type = SUB;
//    stats_queue_send(&stats_msg);
//}

//void timer70Callback(Timer_Handle myHandle, int_fast16_t status)
//{
//    dbgEvent(DBG_TIMER_70_START);
//    ADC_Handle adc;
//    ADC_Params params;
//    int_fast16_t res;
//    static sensor_message_t sensor_msg;
//    sensor_msg.type = TIMER70_TYPE;
//
//    ADC_Params_init(&params);
//    adc = ADC_open(CONFIG_ADC_0, &params);
//
//    if (adc == NULL)
//    {
//        dbgEventFail(DBG_ERR_ADC_START_FAILURE);
//    }
//
//    uint16_t adcValue;
//
//    res = ADC_convert(adc, &adcValue);
//    if (res == ADC_STATUS_SUCCESS)
//    {
//        if(res >= 0)
//        {
//            sensor_msg.data = lerp(adcValue, DIST_70, DIST_300, DIST_DELTA);
//        }
//        else{
//            sensor_msg.data = -1;
//        }
//    }
//    else
//    {
//        sensor_msg.data = -2;
//    }
//
//    sensor_queue_send(&sensor_msg);
//    ADC_close(adc);
//    dbgEvent(DBG_TIMER_70_END);
//}

//void timer500Callback(Timer_Handle myHandle, int_fast16_t status)
//{
//    dbgEvent(DBG_TIMER_500_START);
//    typedef uint32_t     TickType_t;
//    static sensor_message_t sensor_msg;
//
//    static TickType_t xTimeISRLastExecuted = 0;
//    TickType_t xTimeNow, xTimeBetweenInterrupts;
//    xTimeNow = xTaskGetTickCountFromISR();
//     /* Perform some operation. */
//     /* How many ticks occurred between this and the previous interrupt? */
//     xTimeBetweenInterrupts = xTimeNow - xTimeISRLastExecuted;
//     /* Remember the time at which this interrupt was entered. */
//     xTimeISRLastExecuted = xTimeNow;
//
//     sensor_msg.type = TIMER500_TYPE;
//     sensor_msg.data = xTimeBetweenInterrupts;
//
//     sensor_queue_send(&sensor_msg);
//
//}

/*
void *task_one(void *arg0)
{
    dbgEvent(DBG_TASK_ONE_START);

#ifdef TASK1_VER1
    static sensor_message_t sensor_msg;

    while (1) {
        sensor_queue_receive(&sensor_msg);
        sensor_FSM(&sensor_msg);
    }
#else
    static sensor_message_t sensor_msg;
    static int num_messages_received;
    static int num_sensor_readings;
    static int total_avg_readings;
    static int num_avg_readings;
    static char buff[MQTT_MSG_BUFFER_LEN];
    static int seq_num;

    while (1) {
        sensor_queue_receive(&sensor_msg);

        switch (sensor_msg.type) {

        case TIMER70_TYPE:
            num_messages_received ++;
            num_sensor_readings ++;
            break;

        case TIMER500_TYPE:
            num_messages_received ++;
            total_avg_readings += sensor_msg.data;
            num_avg_readings ++;
            break;

        case NONE_TYPE:
        {
            size_t bytes = snprintf(buff, MQTT_MSG_BUFFER_LEN, "{\"n_msg_rcv\": %d, \"n_sns_rdg\": %d, \"avg_sns_rdg\": %d, \"SeqNum\": %d}", num_messages_received, num_sensor_readings, total_avg_readings / num_avg_readings, seq_num);
            MQTT_IF_Publish(mqttClientHandle,
                            "task1/SensorStats",
                            buff,
                            bytes,
                            MQTT_QOS_0);
            stats_message_t stats_msg;
            stats_msg.type = PUB;
            stats_queue_send(&stats_msg);
            seq_num ++;
            num_messages_received = 0;
            num_sensor_readings = 0;
            total_avg_readings = 0;
            num_avg_readings = 0;
            break;
        }
        }
    }
#endif
}
*/
