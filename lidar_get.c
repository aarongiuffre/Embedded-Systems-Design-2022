/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== pwmled2.c ========
 */
/* RTOS header files */
#include <FreeRTOS.h>
#include <stdio.h>
#include <string.h>
#include <task.h>
#include <queue.h>
#include <mqtt_if.h>

/* Driver Header files */
#include <ti/drivers/Timer.h>
#include <ti/drivers/PWM.h>
#include <ti/drivers/UART.h>

/* Driver configuration */
#include "ti_drivers_config.h"

/* For usleep() */
#include <unistd.h>
#include <stddef.h>

/* Custom-made Project Header files */
#include "debug.h"
#include "globals.h"

//extern QueueHandle_t timer_queue_handle;
extern UART_Handle uart;

//void pwm_init()
//{
//    PWM_Handle pwm1 = NULL;
//    PWM_Params pwmParams;
//    /* Period and duty in microseconds */
//    uint16_t   pwmPeriod = 3000;
//    uint16_t   duty = 1000; //0;
//
//    PWM_Params_init(&pwmParams);
//    pwmParams.dutyUnits = PWM_DUTY_US;
//    pwmParams.dutyValue = 0;
//    pwmParams.periodUnits = PWM_PERIOD_US;
//    pwmParams.periodValue = pwmPeriod;
//    pwm1 = PWM_open(CONFIG_PWM_0, &pwmParams);
//    if (pwm1 == NULL) {
//        /* CONFIG_PWM_0 did not open */
//        while (1);
//    }
//
//    PWM_start(pwm1);
//    PWM_setDuty(pwm1, duty);
//}

//void uart_init()
//{
//    extern UART_Handle uart;
//    UART_Params uartParams;
//
//    /* Create a UART with data processing off. */
//    UART_Params_init(&uartParams);
//    uartParams.writeDataMode = UART_DATA_BINARY;
//    uartParams.readDataMode = UART_DATA_BINARY;
//    uartParams.readReturnMode = UART_RETURN_FULL;
//    uartParams.baudRate = 115200;
//
//    uart = UART_open(CONFIG_UART_1, &uartParams);
//
//    if (uart == NULL) {
//        /* UART_open() failed */
//        while (1);
//    }
//}

//void timerCallback(Timer_Handle myHandle)
//{
//    char ping_msg[] = "ping";
//
//    xQueueSendFromISR(timer_queue_handle, ping_msg, 0);
//}

/*
 *  ======== mainThread ========
 *  Task sets the PWM duty for MOTOCTL.
 */
void *mainThread(void *arg0)
{
    uint8_t receivedByte = 0x00;
    const uint8_t  resetBytes[] = {0xA5, 0x40};
    const uint8_t  healthBytes[] = {0xA5, 0x52};
    const uint8_t  startBytes[] = {0xA5, 0x20};
    uint8_t receivedBytes[2] = {0x00, 0x00};
    const uint8_t  response_descriptor[] = {0xA5, 0x5A, 0x05, 0x00, 0x00, 0x40, 0x81};
    const uint8_t  first_data[] = {0x02, 0x01, 0x00, 0x00, 0x00};
    int response_descriptor_count = 0;
    int data_response_count = 0;
    int output_count = 0;
    int seqNum = 0;
    uint8_t data_byte_0;
    uint8_t data_byte_1;
    uint8_t data_byte_2;
    uint8_t data_byte_3;
    uint8_t data_byte_4;
    uint8_t quality;
    uint16_t angle_bits;
    float prev_angle_deg;
    float cur_angle_deg;
    uint16_t distance_bits;
    float prev_distance_mm;
    float cur_distance_mm;
    //#define MQTT_MSG_BUFFER_LEN 128
    //static char buff[MQTT_MSG_BUFFER_LEN];
    static char dat_buff[MQTT_MSG_BUFFER_LEN];
    static char *topic = "receivedData";
    //char ping_msg[5];

    //GPIO_init();
    //PWM_init();
    //UART_init();

    //setup PWM
    //pwm_init();
    //open UART channel
    //uart_init();
    //mqtt setup
//    mqtt_init();
//    mqtt_subscribe();
//    DisplayAppBanner(APPLICATION_NAME, APPLICATION_VERSION);
//    mqtt_start();

    //dbgGPIOtest();

    UART_write(uart, resetBytes, sizeof(resetBytes));
    //UART_read(uart, &receivedBytes, 2);
    //dbgByte(receivedBytes[0]);
    //dbgByte(receivedBytes[1]);
    usleep(800000);
    UART_write(uart, healthBytes, sizeof(healthBytes));
    usleep(800000);
    UART_write(uart, startBytes, sizeof(startBytes));
    //UART_read(uart, &receivedBytes, 2);
    //dbgByte(receivedBytes[0]);
    //dbgByte(receivedBytes[1]);
    //usleep(800000);


    while (1)
    {
        UART_read(uart, &receivedByte, 1);
        dbgByte(receivedByte);

//        if(response_descriptor_count < 5)
//        {
//            if(receivedByte == first_data[response_descriptor_count])
//            {
//                response_descriptor_count++;
//            }
//            else
//            {
//                response_descriptor_count = 0;
//            }
//        }
        if((receivedByte == 0x3E) || (receivedByte == 0x3A) || (data_response_count > 0))
        {
            switch(data_response_count)
            {
                case 0:
                    data_byte_0 = receivedByte;
                    break;
                case 1:
                    data_byte_1 = receivedByte;
                    break;
                case 2:
                    data_byte_2 = receivedByte;
                    break;
                case 3:
                    data_byte_3 = receivedByte;
                    break;
                case 4:
                    data_byte_4 = receivedByte;
                    break;
            }
            data_response_count++;

            if(data_response_count >= 5)
            {
                if(output_count >= 4)
                {
                    quality = (data_byte_0 >> 2);
                    angle_bits = ((uint16_t)data_byte_2 << 7) | ((uint16_t)data_byte_1 >> 1);
                    cur_angle_deg = angle_bits / 64.0;
                    distance_bits = ((uint16_t)data_byte_4 << 8) | (uint16_t)data_byte_3;
                    cur_distance_mm = distance_bits / 4.0;
                    size_t bytes = snprintf(dat_buff, MQTT_MSG_BUFFER_LEN, "%f %f", cur_angle_deg, cur_distance_mm);
                    if(quality >= 0x0A && cur_angle_deg >= 13.0 && cur_angle_deg <= 360.0 && !(cur_angle_deg >= 122.0 && cur_angle_deg <= 127.0) && cur_distance_mm >= 50.0 && cur_distance_mm <= 500.0 && cur_distance_mm != 128.0 && !(cur_distance_mm >= 172.0 && cur_distance_mm <= 177.0))
                    {
                        MQTT_IF_Publish(mqttClientHandle,
                                        topic,
                                        dat_buff,
                                        bytes,
                                        MQTT_QOS_0);
                    }
                    output_count = 0;
                }
                data_response_count = 0;
                output_count++;
            }
        }
    }
}

