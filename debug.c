/*
 * debug.c
 *
 *  Created on: Feb 3, 2022
 *      Author: ujjwal Jain
 */

#include "debug.h"

/* Driver Header files */
#include <FreeRTOS.h>
#include <ti/drivers/GPIO.h>

/* Driver configuration */
#include "ti_drivers_config.h"

void dbgEvent (unsigned int event)
{
    if (event <= 127)
    {
        //        8th bit high before writing
        GPIO_write(CONFIG_GPIO_7, 1);
        GPIO_write(CONFIG_GPIO_0, (event & 0x01));
        GPIO_write(CONFIG_GPIO_1, (event & 0x02));
        GPIO_write(CONFIG_GPIO_2, (event & 0x04));
        GPIO_write(CONFIG_GPIO_3, (event & 0x08));
        GPIO_write(CONFIG_GPIO_4, (event & 0x10));
        GPIO_write(CONFIG_GPIO_5, (event & 0x20));
        GPIO_write(CONFIG_GPIO_6, (event & 0x40));
        GPIO_write(CONFIG_GPIO_7, 0);
        //        8th bit low after writing
    }
    else
    {
        dbgEventFail (DBG_ERR_OVERFLOW);
    }
}

void dbgByte (unsigned int byte)
{
        GPIO_write(CONFIG_GPIO_0, (byte & 0x01));
        GPIO_write(CONFIG_GPIO_1, (byte & 0x02));
        GPIO_write(CONFIG_GPIO_2, (byte & 0x04));
        GPIO_write(CONFIG_GPIO_3, (byte & 0x08));
        GPIO_write(CONFIG_GPIO_4, (byte & 0x10));
        GPIO_write(CONFIG_GPIO_5, (byte & 0x20));
        GPIO_write(CONFIG_GPIO_6, (byte & 0x40));
        GPIO_write(CONFIG_GPIO_7, (byte & 0x80));
}

void dbgEventFail (unsigned char error_msg)
{
    dbgEvent(error_msg);

    portDISABLE_INTERRUPTS();

    /* Turn on user LED */
    GPIO_write(CONFIG_LED_0, CONFIG_GPIO_LED_ON);

    int i = 0;
    while(1)
    {
        if (i==1000000)
        {
            GPIO_toggle(CONFIG_LED_0);
            i=0;
        }
        i++;
    }
}

void dbgGPIOtest()
{
    dbgEvent(0x7F);
    dbgEvent(0x00);
    dbgEvent(0x01);
    dbgEvent(0x02);
    dbgEvent(0x04);
    dbgEvent(0x08);
    dbgEvent(0x10);
    dbgEvent(0x20);
    dbgEvent(0x40);
    dbgEvent(0x00);
    dbgEvent(0x7F);
    dbgEvent(0x00);
}
