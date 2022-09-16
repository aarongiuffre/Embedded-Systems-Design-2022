/*
 * init_main.c
 *
 *  Created on: Mar 1, 2022
 *      Author: acemc
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

/* TI-DRIVERS Header files */
#include "ti_drivers_config.h"

/* Custom-made Project Header files */
#include "init_main.h"
#include "globals.h"
#include "debug.h"
#include "uart_term.h"
#include "sensor_thread_queue.h"
#include "chain_thread_queue.h"
#include "mqtt_stats.h"

extern QueueHandle_t mqttQueueHandle;
extern MQTTClient_Handle mqttClientHandle;

//Tasks
//extern void * task_one(void *arg0);
//extern void * task_two(void *arg0);
//extern void * read_stats(void *arg0);

//Callbacks
#ifdef TASK1_VER1
extern void timer70Callback(Timer_Handle myHandle, int_fast16_t status);
extern void timer500Callback(Timer_Handle myHandle, int_fast16_t status);
#endif

//#ifdef TASK1_VER2
//extern void v2Timer_Callback(Timer_Handle myHandle, int_fast16_t status);
//extern void v2MQTT_Callback(char* topic, char* payload);
//#endif
//
//extern void Chain_Callback(char* topic, char* payload);


static bool connected = false;

//void create_threads() {
//    pthread_t           sensorThread, chainThread, statsThread;
//    pthread_attr_t      attrs;
//    struct sched_param  priParam;
//    int                 retc;
//
//    /* Initialize the attributes structure with default values */
//    pthread_attr_init(&attrs);
//
//    /* Set priority, detach state, and stack size attributes */
//    priParam.sched_priority = 1;
//    retc = pthread_attr_setschedparam(&attrs, &priParam);
//    retc |= pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);
//    retc |= pthread_attr_setstacksize(&attrs, THREADSTACKSIZE);
//    if (retc != 0) {
//        /* failed to set attributes */
//        dbgEventFail(DBG_ERR_PTHREAD_ATTR_INIT);
//    }
//
//    retc = pthread_create(&sensorThread, &attrs, task_one, NULL);
//    if(retc != 0)
//    {
//        /* pthread_create() failed */
//        dbgEventFail(DBG_ERR_PTHREAD_CREATE);
//    }
//
//    retc = pthread_create(&chainThread, &attrs, task_two, NULL);
//    if (retc != 0)
//    {
//        /* pthread_create() failed */
//        dbgEventFail(DBG_ERR_PTHREAD_CREATE);
//    }
//
//    retc = pthread_create(&statsThread, &attrs, read_stats, NULL);
//    if (retc != 0)
//    {
//        /* pthread_create() failed */
//        dbgEventFail(DBG_ERR_PTHREAD_CREATE);
//    }
//
//    LOG_TRACE("Threads created\n\r");
//}

void create_queues() {
    mqttQueueHandle = xQueueCreate(10, sizeof(msgData_t));
    //sensor_queue_handle = xQueueCreate(15, sizeof(sensor_message_t));
    //chain_queue_handle = xQueueCreate(15, sizeof(chain_message_t));
    //stats_thread_queue_create();
    LOG_TRACE("Queues created\n\r");
}

//void create_and_start_timers() {
//
//#ifdef TASK1_VER1
//    Timer_Handle timer0;
//    Timer_Params params_70;
//    Timer_Handle timer1;
//    Timer_Params params_500;
//
//    Timer_Params_init(&params_70);
//    params_70.period = 70000;
//    params_70.periodUnits = Timer_PERIOD_US;
//    params_70.timerMode = Timer_CONTINUOUS_CALLBACK;
//    params_70.timerCallback = timer70Callback;
//
//    Timer_Params_init(&params_500);
//    params_500.period = 500000;
//    params_500.periodUnits = Timer_PERIOD_US;
//    params_500.timerMode = Timer_CONTINUOUS_CALLBACK;
//    params_500.timerCallback = timer500Callback;
//
//    timer0 = Timer_open(CONFIG_TIMER_0, &params_70);
//    timer1 = Timer_open(CONFIG_TIMER_1, &params_500);
//
//    if (timer0 == NULL) {
//        dbgEventFail(DBG_ERR_TIMER_START_FAILURE);
//    }
//
//    if (timer1 == NULL) {
//        dbgEventFail(DBG_ERR_TIMER_START_FAILURE);
//    }
//
//    if (Timer_start(timer0) == Timer_STATUS_ERROR) {
//        dbgEventFail(DBG_ERR_TIMER_START_FAILURE);
//    }
//
//    if (Timer_start(timer1) == Timer_STATUS_ERROR) {
//        dbgEventFail(DBG_ERR_TIMER_START_FAILURE);
//    }
//#endif
//
//#ifdef TASK1_VER2
//    Timer_Handle timer;
//    Timer_Params params;
//
//    Timer_Params_init(&params);
//    params.period = 1000000;
//    params.periodUnits = Timer_PERIOD_US;
//    params.timerMode = Timer_CONTINUOUS_CALLBACK;
//    params.timerCallback = v2Timer_Callback;
//
//    timer = Timer_open(CONFIG_TIMER_0, &params);
//
//    if (timer == NULL) {
//        dbgEventFail(DBG_ERR_TIMER_START_FAILURE);
//    }
//
//    if (Timer_start(timer) == Timer_STATUS_ERROR) {
//        dbgEventFail(DBG_ERR_TIMER_START_FAILURE);
//    }
//#endif
//    LOG_TRACE("Timers started\n\r");
//}

int32_t SetClientIdNamefromMacAddress()
{
    int32_t ret = 0;
    uint8_t Client_Mac_Name[2];
    uint8_t Index;
    uint16_t macAddressLen = SL_MAC_ADDR_LEN;
    uint8_t macAddress[SL_MAC_ADDR_LEN];

    /*Get the device Mac address */
    ret = sl_NetCfgGet(SL_NETCFG_MAC_ADDRESS_GET, 0, &macAddressLen,
                       &macAddress[0]);

    /*When ClientID isn't set, use the mac address as ClientID               */
    if(ClientId[0] == '\0')
    {
        /*6 bytes is the length of the mac address                           */
        for(Index = 0; Index < SL_MAC_ADDR_LEN; Index++)
        {
            /*Each mac address byte contains two hexadecimal characters      */
            /*Copy the 4 MSB - the most significant character                */
            Client_Mac_Name[0] = (macAddress[Index] >> 4) & 0xf;
            /*Copy the 4 LSB - the least significant character               */
            Client_Mac_Name[1] = macAddress[Index] & 0xf;

            if(Client_Mac_Name[0] > 9)
            {
                /*Converts and copies from number that is greater than 9 in  */
                /*hexadecimal representation (a to f) into ascii character   */
                ClientId[2 * Index] = Client_Mac_Name[0] + 'a' - 10;
            }
            else
            {
                /*Converts and copies from number 0 - 9 in hexadecimal       */
                /*representation into ascii character                        */
                ClientId[2 * Index] = Client_Mac_Name[0] + '0';
            }
            if(Client_Mac_Name[1] > 9)
            {
                /*Converts and copies from number that is greater than 9 in  */
                /*hexadecimal representation (a to f) into ascii character   */
                ClientId[2 * Index + 1] = Client_Mac_Name[1] + 'a' - 10;
            }
            else
            {
                /*Converts and copies from number 0 - 9 in hexadecimal       */
                /*representation into ascii character                        */
                ClientId[2 * Index + 1] = Client_Mac_Name[1] + '0';
            }
        }
    }
    return(ret);
}

void MQTT_EventCallback(int32_t event){

    switch(event){

        case MQTT_EVENT_CONNACK:
        {
            LOG_INFO("MQTT_EVENT_CONNACK\r\n");
            connected = true;
            break;
        }

        case MQTT_EVENT_SUBACK:
        {
            LOG_INFO("MQTT_EVENT_SUBACK\r\n");
            break;
        }

        case MQTT_EVENT_PUBACK:
        {
            LOG_INFO("MQTT_EVENT_PUBACK\r\n");
            break;
        }

        case MQTT_EVENT_UNSUBACK:
        {
            LOG_INFO("MQTT_EVENT_UNSUBACK\r\n");
            break;
        }

        case MQTT_EVENT_CLIENT_DISCONNECT:
        {
            LOG_INFO("MQTT_EVENT_CLIENT_DISCONNECT\r\n");
            break;
        }

        case MQTT_EVENT_SERVER_DISCONNECT:
        {
            LOG_INFO("MQTT_EVENT_SERVER_DISCONNECT\r\n");
            break;
        }

        case MQTT_EVENT_DESTROY:
        {
            LOG_INFO("MQTT_EVENT_DESTROY\r\n");
            break;
        }
    }
}

int32_t DisplayAppBanner(char* appName, char* appVersion){

    int32_t ret = 0;
    uint8_t macAddress[SL_MAC_ADDR_LEN];
    uint16_t macAddressLen = SL_MAC_ADDR_LEN;
    uint16_t ConfigSize = 0;
    uint8_t ConfigOpt = SL_DEVICE_GENERAL_VERSION;
    SlDeviceVersion_t ver = {0};

    ConfigSize = sizeof(SlDeviceVersion_t);

    // get the device version info and MAC address
    ret = sl_DeviceGet(SL_DEVICE_GENERAL, &ConfigOpt, &ConfigSize, (uint8_t*)(&ver));
    ret |= (int32_t)sl_NetCfgGet(SL_NETCFG_MAC_ADDRESS_GET, 0, &macAddressLen, &macAddress[0]);

    UART_PRINT("\n\r\t============================================\n\r");
    UART_PRINT("\t   %s Example Ver: %s",appName, appVersion);
    UART_PRINT("\n\r\t============================================\n\r\n\r");

    UART_PRINT("\t CHIP: 0x%x\n\r",ver.ChipId);
    UART_PRINT("\t MAC:  %d.%d.%d.%d\n\r",ver.FwVersion[0],ver.FwVersion[1],
               ver.FwVersion[2],
               ver.FwVersion[3]);
    UART_PRINT("\t PHY:  %d.%d.%d.%d\n\r",ver.PhyVersion[0],ver.PhyVersion[1],
               ver.PhyVersion[2],
               ver.PhyVersion[3]);
    UART_PRINT("\t NWP:  %d.%d.%d.%d\n\r",ver.NwpVersion[0],ver.NwpVersion[1],
               ver.NwpVersion[2],
               ver.NwpVersion[3]);
    UART_PRINT("\t ROM:  %d\n\r",ver.RomVersion);
    UART_PRINT("\t HOST: %s\n\r", SL_DRIVER_VERSION);
    UART_PRINT("\t MAC address: %02x:%02x:%02x:%02x:%02x:%02x\r\n", macAddress[0],
               macAddress[1], macAddress[2], macAddress[3], macAddress[4],
               macAddress[5]);
    UART_PRINT("\n\r\t============================================\n\r");

    return(ret);
}

int WifiInit(){

    int32_t ret;
    SlWlanSecParams_t security_params;
    pthread_t spawn_thread = (pthread_t) NULL;
    pthread_attr_t pattrs_spawn;
    struct sched_param pri_param;

    pthread_attr_init(&pattrs_spawn);
    pri_param.sched_priority = SPAWN_TASK_PRIORITY;
    ret = pthread_attr_setschedparam(&pattrs_spawn, &pri_param);
    ret |= pthread_attr_setstacksize(&pattrs_spawn, SL_TASKSTACKSIZE);
    ret |= pthread_attr_setdetachstate(&pattrs_spawn, PTHREAD_CREATE_DETACHED);
    ret = pthread_create(&spawn_thread, &pattrs_spawn, sl_Task, NULL);
    if(ret != 0){
        LOG_ERROR("could not create simplelink task\n\r");
        while(1);
    }


    Network_IF_ResetMCUStateMachine();

    Network_IF_DeInitDriver();

    ret = Network_IF_InitDriver(ROLE_STA);
    if(ret < 0){
        LOG_ERROR("Failed to start SimpleLink Device\n\r");
        while(1);
    }

    SetClientIdNamefromMacAddress();

    security_params.Key = (signed char*)SECURITY_KEY;
    security_params.KeyLen = strlen(SECURITY_KEY);
    security_params.Type = SECURITY_TYPE;

    ret = Network_IF_ConnectAP(SSID_NAME, security_params);
    if(ret < 0){
        LOG_ERROR("Connection to an AP failed\n\r");
    }
    else{

        SlWlanSecParams_t securityParams;

        securityParams.Type = SECURITY_TYPE;
        securityParams.Key = (signed char*)SECURITY_KEY;
        securityParams.KeyLen = strlen((const char *)securityParams.Key);

        ret = sl_WlanProfileAdd((signed char*)SSID_NAME, strlen(SSID_NAME), 0, &securityParams, NULL, 7, 0);
        if(ret < 0){
            LOG_ERROR("failed to add profile %s\r\n", SSID_NAME);
        }
        else{
            LOG_INFO("profile added %s\r\n", SSID_NAME);
        }
    }

    return ret;
}

void mqtt_init() {
    int32_t ret;

    LOG_TRACE("MQTT initialization starting...\n\r");

    ret = ti_net_SlNet_initConfig();
    if(0 != ret)
    {
        LOG_ERROR("Failed to initialize SlNetSock\n\r");
        dbgEventFail(DBG_ERR_FATAL_ERROR);
    }

    LOG_TRACE(" - SlNet initialized...\n\r");

    ret = WifiInit();
    if(ret < 0){
        LOG_ERROR("Failed to initialize the WiFi connection\n\r");
        dbgEventFail(DBG_ERR_FATAL_ERROR);
    }

    LOG_TRACE(" - WiFi connected...\n\r");

    ret = MQTT_IF_Init(mqttInitParams);
    if(ret < 0){
        LOG_ERROR("Failed to intialize the MQTT instance\n\r");
        dbgEventFail(DBG_ERR_FATAL_ERROR);
    }

    LOG_TRACE(" - MQTT initialized\n\r");
}

//void mqtt_subscribe() {
//
//    /*
//     * In case a persistent session is being used, subscribe is called before connect so that the module
//     * is aware of the topic callbacks the user is using. This is important because if the broker is holding
//     * messages for the client, after CONNACK the client may receive the messages before the module is aware
//     * of the topic callbacks. The user may still call subscribe after connect but have to be aware of this.
//     */
//    int ret;
//
//#ifdef TASK1_VER2
//    ret |= MQTT_IF_Subscribe(mqttClientHandle, "task1/SensorData", MQTT_QOS_0, v2MQTT_Callback);
//#endif
//
//#ifdef CHAIN_1
//    ret |= MQTT_IF_Subscribe(mqttClientHandle, "task2/Chain1", MQTT_QOS_0, Chain_Callback);
//#endif
//
//#ifdef CHAIN_2
//    ret |= MQTT_IF_Subscribe(mqttClientHandle, "task2/Chain2", MQTT_QOS_0, Chain_Callback);
//#endif
//
//#ifdef CHAIN_3
//    ret |= MQTT_IF_Subscribe(mqttClientHandle, "task2/Chain3", MQTT_QOS_0, Chain_Callback);
//#endif
//
//#ifdef CHAIN_4
//    ret |= MQTT_IF_Subscribe(mqttClientHandle, "task2/Chain4", MQTT_QOS_0, Chain_Callback);
//#endif
//
//    if(ret < 0){
//        while(1);
//    }
//    else{
//        LOG_INFO("Subscribed to all topics successfully\r\n");
//    }
//}

void mqtt_start() {

    mqttClientHandle = MQTT_IF_Connect(mqttClientParams, mqttConnParams, MQTT_EventCallback);
    if(mqttClientHandle < 0){
        LOG_ERROR("Failed to start the MQTT instance\n\r");
        dbgEventFail(DBG_ERR_FATAL_ERROR);
    }

    LOG_TRACE("MQTT instance started\n\r");
    //blocking wait for MQTT connection to be confirmed. This is fine only because we are still in "startup"
    while(!connected) {}
    LOG_TRACE("MQTT instance connected\n\r");

}

void *init_thread(void * args) {

    create_queues();
    mqtt_init();
    //mqtt_subscribe();
    DisplayAppBanner(APPLICATION_NAME, APPLICATION_VERSION);
    mqtt_start();
    //create_threads();
    //create_and_start_timers();

    while (1) {} //sit in an infinite loop to suspend this task after init is finished
}



