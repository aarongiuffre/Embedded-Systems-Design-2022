/*
 * init_main.h
 *
 *  Created on: Mar 1, 2022
 *      Author: acemc
 */

#ifndef INIT_MAIN_H_
#define INIT_MAIN_H_

#include <mqtt_if.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <mqueue.h>

#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/drivers/net/wifi/slnetifwifi.h>

#include <ti/drivers/SPI.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/Timer.h>

#include <ti/net/mqtt/mqttclient.h>

#include "network_if.h"
#include "uart_term.h"
#include "mqtt_if.h"
#include "debug_if.h"

#include "ti_drivers_config.h"

extern int32_t ti_net_SlNet_initConfig();

#define APPLICATION_NAME         "MQTT client"
#define APPLICATION_VERSION      "2.0.0"

#define SL_TASKSTACKSIZE            2048
#define SPAWN_TASK_PRIORITY         9

#define MQTT_MODULE_TASK_PRIORITY   2
#define MQTT_MODULE_TASK_STACK_SIZE 2048

#define MQTT_WILL_TOPIC             NULL //"jesus_cc32xx_will_topic"
#define MQTT_WILL_MSG               NULL //"will_msg_works"
#define MQTT_WILL_QOS               MQTT_QOS_0
#define MQTT_WILL_RETAIN            false

#define MQTT_CLIENT_PASSWORD        "mJ0gdBDVdBS3"
#define MQTT_CLIENT_USERNAME        "sxktyzgv"
#define MQTT_CLIENT_KEEPALIVE       0
#define MQTT_CLIENT_CLEAN_CONNECT   true
#define MQTT_CLIENT_MQTT_V3_1       true
#define MQTT_CLIENT_BLOCKING_SEND   true

#define MQTT_CONNECTION_FLAGS           MQTTCLIENT_NETCONN_URL
#define MQTT_CONNECTION_ADDRESS         "driver.cloudmqtt.com"
#define MQTT_CONNECTION_PORT_NUMBER     18645

/* Client ID                                                                 */
/* If ClientId isn't set, the MAC address of the device will be copied into  */
/* the ClientID parameter.                                                   */
static char ClientId[13] = {'\0'};

enum{
    APP_MQTT_PUBLISH,
    APP_MQTT_CON_TOGGLE,
    APP_MQTT_DEINIT,
    APP_BTN_HANDLER,
    APP_MQTT_CONNACK,
    APP_MQTT_DISCONNECT
};

struct msgQueue
{
    int   event;
    char* payload;
};

static MQTT_IF_InitParams_t mqttInitParams =
{
     MQTT_MODULE_TASK_STACK_SIZE,   // stack size for mqtt module - default is 2048
     MQTT_MODULE_TASK_PRIORITY      // thread priority for MQTT   - default is 2
};

static MQTTClient_Will mqttWillParams =
{
     MQTT_WILL_TOPIC,    // will topic
     MQTT_WILL_MSG,      // will message
     MQTT_WILL_QOS,      // will QoS
     MQTT_WILL_RETAIN    // retain flag
};

static MQTT_IF_ClientParams_t mqttClientParams =
{
     ClientId,                  // client ID
     MQTT_CLIENT_USERNAME,      // user name
     MQTT_CLIENT_PASSWORD,      // password
     MQTT_CLIENT_KEEPALIVE,     // keep-alive time
     MQTT_CLIENT_CLEAN_CONNECT, // clean connect flag
     MQTT_CLIENT_MQTT_V3_1,     // true = 3.1, false = 3.1.1
     MQTT_CLIENT_BLOCKING_SEND, // blocking send flag
     &mqttWillParams            // will parameters
};

static MQTTClient_ConnParams mqttConnParams =
{
     MQTT_CONNECTION_FLAGS,         // connection flags
     MQTT_CONNECTION_ADDRESS,       // server address
     MQTT_CONNECTION_PORT_NUMBER,   // port number of MQTT server
     0,                             // method for secure socket
     0,                             // cipher for secure socket
     0,                             // number of files for secure connection
     NULL                           // secure files
};

void *init_thread(void * args);

//void create_queues();
//int32_t SetClientIdNamefromMacAddress();
//void MQTT_EventCallback(int32_t event);
//int32_t DisplayAppBanner(char* appName, char* appVersion);
//int WifiInit();
//void mqtt_init();
//void mqtt_start();



#endif /* INIT_MAIN_H_ */
