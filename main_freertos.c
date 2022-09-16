/*
 * Copyright (c) 2016, Texas Instruments Incorporated
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
 *  ======== main_freertos.c ========
 */
#include <stdint.h>
#include <stdbool.h>

/* POSIX Header files */
#include <pthread.h>

/* RTOS header files */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* TI-RTOS Header files */
#include <ti/drivers/Board.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/ADC.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/PWM.h>
#include <ti/drivers/SPI.h>

/* TI-DRIVERS Header files */
#include "ti_drivers_config.h"

/* Custom-made Project Header files */
#include "globals.h"
#include "debug.h"
#include "init_main.h"
#include "uart_term.h"
#include "debug_if.h"

extern void * init_thread(void *arg0);

extern void BrokerCB(char* topic, char* payload);
extern void ToggleLED1CB(char* topic, char* payload);
//
static bool connected = false;

void pwm_init()
{
    PWM_Handle pwm1 = NULL;
    PWM_Params pwmParams;
    /* Period and duty in microseconds */
    uint16_t   pwmPeriod = 1000;
    uint16_t   duty = 500; //0;

    PWM_Params_init(&pwmParams);
    pwmParams.dutyUnits = PWM_DUTY_US;
    pwmParams.dutyValue = 0;
    pwmParams.periodUnits = PWM_PERIOD_US;
    pwmParams.periodValue = pwmPeriod;
    pwm1 = PWM_open(CONFIG_PWM_0, &pwmParams);
    if (pwm1 == NULL) {
        /* CONFIG_PWM_0 did not open */
        while (1);
    }

    PWM_start(pwm1);
    PWM_setDuty(pwm1, duty);
}

//void uart_init()
//{
////    =----------------------------------------------------------=
////    =- This is commented out to avoid double-opening the UART -=
////    =----------------------------------------------------------=
////    UART_Params uartParams;
////
////    UART_Params_init(&uartParams);
////    uartParams.writeDataMode = UART_DATA_BINARY;
////    uartParams.readDataMode = UART_DATA_BINARY;
////    uartParams.readReturnMode = UART_RETURN_FULL;
////    uartParams.baudRate = 115200;
////
////    uart_handle = UART_open(CONFIG_UART_0, &uartParams);
////
////    if (uart_handle == NULL)
////    {
////        dbgEventFail(DBG_ERR_UART_START_FAILURE);
////    }
//
//
//    UART_Handle uartHandle = InitTerm();
//    UART_control(uartHandle, UART_CMD_RXDISABLE, NULL);
//
//    LOG_TRACE("UART initialized\n\r");
//}

void uart_init()
{
    extern UART_Handle uart;
    UART_Params uartParams;

    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.baudRate = 115200;

    uart = UART_open(CONFIG_UART_1, &uartParams);

    if (uart == NULL) {
        /* UART_open() failed */
        while (1);
    }

    UART_Handle uartHandle = InitTerm();
    UART_control(uartHandle, UART_CMD_RXDISABLE, NULL);

    LOG_TRACE("UART initialized\n\r");
}

void create_init_thread() {
    pthread_t           thread;
    pthread_attr_t      attrs;
    struct sched_param  priParam;
    int                 retc;

    /* Initialize the attributes structure with default values */
    pthread_attr_init(&attrs);

    /* Set priority, detach state, and stack size attributes */
    priParam.sched_priority = 1;
    retc = pthread_attr_setschedparam(&attrs, &priParam);
    retc |= pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);
    retc |= pthread_attr_setstacksize(&attrs, THREADSTACKSIZE);
    if (retc != 0) {
        /* failed to set attributes */
        dbgEventFail(DBG_ERR_PTHREAD_ATTR_INIT);
    }

    retc = pthread_create(&thread, &attrs, init_thread, NULL);
    if(retc != 0)
    {
        /* pthread_create() failed */
        dbgEventFail(DBG_ERR_PTHREAD_CREATE);
    }

    LOG_TRACE("Init Thread created\n\r");
}

extern void *mainThread(void *arg0);

void create_main_thread()
{
    pthread_t           thread;
    pthread_attr_t      attrs;
    struct sched_param  priParam;
    int                 retc;

    /* Initialize the attributes structure with default values */
    pthread_attr_init(&attrs);

    /* Set priority, detach state, and stack size attributes */
    priParam.sched_priority = 1;
    retc = pthread_attr_setschedparam(&attrs, &priParam);
    retc |= pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);
    retc |= pthread_attr_setstacksize(&attrs, THREADSTACKSIZE);
    if (retc != 0) {
        /* failed to set attributes */
        while (1) {}
    }

    retc = pthread_create(&thread, &attrs, mainThread, NULL);
    if (retc != 0) {
        /* pthread_create() failed */
        while (1) {}
    }
}

/*
 *  ======== main ========
 */
int main(void)
{
    /* Call hardware init functions */
    Board_init();
    create_main_thread();
    GPIO_init();
    PWM_init();
    Timer_init();
    ADC_init();
    UART_init();
    SPI_init();
    pwm_init();
    uart_init();

    create_init_thread();

    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();

    return (0);
}

//*****************************************************************************
//
//! \brief Application defined malloc failed hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationMallocFailedHook()
{
    /* Handle Memory Allocation Errors */
    while(1)
    {
    }
}

//*****************************************************************************
//
//! \brief Application defined stack overflow hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationStackOverflowHook(TaskHandle_t pxTask,
                                   char *pcTaskName)
{
    //Handle FreeRTOS Stack Overflow
    while(1)
    {
    }
}

void vApplicationTickHook(void)
{
    /*
     * This function will be called by each tick interrupt if
     * configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
     * added here, but the tick hook is called from an interrupt context, so
     * code must not attempt to block, and only the interrupt safe FreeRTOS API
     * functions can be used (those that end in FromISR()).
     */
}

void vPreSleepProcessing(uint32_t ulExpectedIdleTime)
{
}

//*****************************************************************************
//
//! \brief Application defined idle task hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void
vApplicationIdleHook(void)
{
    /* Handle Idle Hook for Profiling, Power Management etc */
}

//*****************************************************************************
//
//! \brief  Overwrite the GCC _sbrk function which check the heap limit related
//!         to the stack pointer.
//!         In case of freertos this checking will fail.
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
#if defined (__GNUC__)
void * _sbrk(uint32_t delta)
{
    extern char _end;     /* Defined by the linker */
    extern char __HeapLimit;
    static char *heap_end;
    static char *heap_limit;
    char *prev_heap_end;

    if(heap_end == 0)
    {
        heap_end = &_end;
        heap_limit = &__HeapLimit;
    }

    prev_heap_end = heap_end;
    if(prev_heap_end + delta > heap_limit)
    {
        return((void *) -1L);
    }
    heap_end += delta;
    return((void *) prev_heap_end);
}

#endif
