/*
 * mqttclient_if.c
 *
 *  Created on: Jan 22, 2020
 *      Author: a0227533
 */

#include <mqtt_if.h>

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "debug.h"
#include "debug_if.h"
#include "globals.h"
#include "jsmn.h"

/*//aaron
#define MAX_NUMBER_OF_NODES 10
#define MAX_TOPIC_STR_LEN 50
static struct Node node_array[MAX_NUMBER_OF_NODES];
static char topic_str_array[MAX_NUMBER_OF_NODES][MAX_TOPIC_STR_LEN];
int current_node_index = 0;
*/

#define MQTT_EVENT_RECV MQTT_EVENT_MAX  // event for when receiving data from subscribed topics

#define MQTTAPPTHREADSIZE   2048

// structure for linked list elements to manage the topics
struct Node
{
    MQTTClient_SubscribeParams topicParams;
    MQTT_IF_TopicCallback_f topicCB;
    struct Node* next;
};
static int nodesAllocated = 0;

struct Node* Add_Node();
int Remove_Node(char* topic);
int min(int a, int b);
int max(int a, int b);

static struct mqttContext
{
    pthread_mutex_t moduleMutex;
    MQTTClient_Handle mqttClient;
    MQTT_IF_EventCallback_f eventCB;
    struct Node* head;
    enum {
        MQTT_STATE_IDLE,
        MQTT_STATE_INITIALIZED,
        MQTT_STATE_CONNECTED
    } moduleState;
    uint8_t clientDisconnectFlag;
} mMQTTContext = {NULL, NULL, NULL, NULL, MQTT_STATE_IDLE, 0};

// Callback invoked by the internal MQTT library to notify the application of MQTT events
void MQTTClientCallback(int32_t event, void *metaData, uint32_t metaDateLen, void *data, uint32_t dataLen)
{
//    int status;
//    struct msgQueue queueElement;
    static msgData_t queueMessage;

    switch((MQTTClient_EventCB)event)
    {
        case MQTTClient_OPERATION_CB_EVENT:
        {
            switch(((MQTTClient_OperationMetaDataCB *)metaData)->messageType)
            {
                case MQTTCLIENT_OPERATION_CONNACK:
                {
                    LOG_TRACE("MQTT CLIENT CB: CONNACK\r\n");
                    queueMessage.event = MQTT_EVENT_CONNACK;
                    break;
                }

                case MQTTCLIENT_OPERATION_EVT_PUBACK:
                {
                    LOG_TRACE("MQTT CLIENT CB: PUBACK\r\n");
                    queueMessage.event = MQTT_EVENT_PUBACK;
                    break;
                }

                case MQTTCLIENT_OPERATION_SUBACK:
                {
                    LOG_TRACE("MQTT CLIENT CB: SUBACK\r\n");
                    queueMessage.event = MQTT_EVENT_SUBACK;
                    break;
                }

                case MQTTCLIENT_OPERATION_UNSUBACK:
                {
                    LOG_TRACE("MQTT CLIENT CB: UNSUBACK\r\n");
                    queueMessage.event = MQTT_EVENT_UNSUBACK;
                    break;
                }
            }
            break;
        }

        case MQTTClient_RECV_CB_EVENT:
        {
            LOG_TRACE("MQTT CLIENT CB: RECV CB\r\n");

            MQTTClient_RecvMetaDataCB *receivedMetaData;

            receivedMetaData = (MQTTClient_RecvMetaDataCB *)metaData;

            queueMessage.event = MQTT_EVENT_RECV;
            int bytes_to_write = min(MAX_TOPIC_LEN, receivedMetaData->topLen);
            strncpy(queueMessage.topic, receivedMetaData->topic, bytes_to_write);
            queueMessage.topic[bytes_to_write] = '\0';
            strncpy(queueMessage.payload, data, min(MAX_PAYLOAD_LEN, dataLen));
            queueMessage.payload[min(MAX_PAYLOAD_LEN, dataLen)] = '\0';
            break;
        }
        case MQTTClient_DISCONNECT_CB_EVENT:
        {
            LOG_TRACE("MQTT CLIENT CB: DISCONNECT\r\n");

            queueMessage.event = MQTT_EVENT_SERVER_DISCONNECT;
            break;
        }
    }

    xQueueSend(mqttQueueHandle, &queueMessage, 0);
}

// Helper function used to compare topic strings and accounts for MQTT wild cards
int MQTTHelperTopicMatching(char *s, char *p)
{
    char *s_next = (char*)-1, *p_next;

    for(; s_next; s=s_next+1, p=p_next+1)
    {
        int len;

        if(s[0] == '#')
            return 1;

        s_next = strchr(s, '/');
        p_next = strchr(p, '/');

        len = ((s_next) ? (s_next - s) : (strlen(s))) + 1;
        if(s[0] != '+')
        {
            if(memcmp(s, p, len) != 0)
                return 0;
        }
    }
    return (p_next)?0:1;
}

// This is the application thread for the MQTT module that signals
void *MQTTAppThread(void *threadParams)
{
    int ret;
    static msgData_t queueMessage;

    while(1)
    {
        if(!xQueueReceive(mqttQueueHandle, &queueMessage, portMAX_DELAY)) {
            LOG_ERROR("Queue Receive Failed");
            while(1){}
        }

        ret = 2;

        if(queueMessage.event == MQTT_EVENT_RECV)
        {
            LOG_TRACE("MQTT APP THREAD: RECV TOPIC = %s", queueMessage.topic);

            pthread_mutex_lock(&mMQTTContext.moduleMutex);

            if(mMQTTContext.head != NULL){

                struct Node* tmp = mMQTTContext.head;

                // check what queueElement.topic to invoke the appropriate topic callback event for the user
                while(ret != 0){
                    ret = strncmp(tmp->topicParams.topic, queueMessage.topic, strlen(queueMessage.topic));
                    if(ret == 0){

                        LOG_DEBUG("TOPIC MATCH %s\r\n", queueMessage.topic);
                        tmp->topicCB(queueMessage.topic, queueMessage.payload);
                        break;
                    }
                    tmp = tmp->next;
                    if(tmp == NULL){
                        LOG_INFO("Cannot invoke CB for topic not in topic list\r\n");
                        LOG_INFO("TOPIC: %s\r\n", queueMessage.topic);
                        break;
                    }
                }
            }

            pthread_mutex_unlock(&mMQTTContext.moduleMutex);
        }
        // when MQTT_IF_Deinit is called we must close the message queue and terminate the MQTTAppThread
        else if(queueMessage.event == MQTT_EVENT_DESTROY)
        {
            LOG_TRACE("MQTT APP THREAD: DESTROY\r\n");

            mMQTTContext.eventCB(queueMessage.event);

            pthread_exit(0);
        }
        else if(queueMessage.event == MQTT_EVENT_SERVER_DISCONNECT){

            LOG_TRACE("MQTT APP THREAD: DISCONNECT\r\n");

            int tmp;    // tmp is to avoid invoking the eventCB while mutex is still locked to prevent deadlock
            pthread_mutex_lock(&mMQTTContext.moduleMutex);

            // checks if the disconnect event came because the client called disconnect or the server disconnected
            if(mMQTTContext.clientDisconnectFlag == 1){
                tmp = 1;
                mMQTTContext.clientDisconnectFlag = 0;
            }
            else{
                tmp = 0;
            }
            pthread_mutex_unlock(&mMQTTContext.moduleMutex);

            if(tmp == 1){
                mMQTTContext.eventCB(MQTT_EVENT_CLIENT_DISCONNECT);
            }
            else{
                mMQTTContext.eventCB(queueMessage.event);
            }
        }
        else if(queueMessage.event == MQTT_EVENT_CONNACK){

            LOG_TRACE("MQTT APP THREAD: CONNACK\r\n");

            pthread_mutex_lock(&mMQTTContext.moduleMutex);

            // in case the user re-connects to a server this checks if there is a list of topics to
            // automatically subscribe to the topics again
            if(mMQTTContext.head != NULL){

                struct Node* curr = mMQTTContext.head;
                struct Node* prev;

                // iterate through the linked list until the end is reached
                while(curr != NULL){

                    ret = MQTTClient_subscribe(mMQTTContext.mqttClient, &curr->topicParams, 1);
                    // if subscribing to a topic fails we must remove the node from the list since we are no longer subscribed
                    if(ret < 0){

                        LOG_ERROR("SUBSCRIBE FAILED: %s", curr->topicParams.topic);

                        // if the node to remove is the head update the head pointer
                        if(curr == mMQTTContext.head){
                            mMQTTContext.head = curr->next;
                            curr = curr->next;
                        }
                        else if(curr->next != NULL){
                            prev->next = curr->next;
                            curr = curr->next->next;
                        }
                        else{
                            prev->next = curr->next;
                            curr = curr->next;
                        }
                    }
                    else{
                        prev = curr;
                        curr = curr->next;
                    }
                }
            }
            pthread_mutex_unlock(&mMQTTContext.moduleMutex);
            // notify the user of the connection event
            mMQTTContext.eventCB(queueMessage.event);
        }
        else{

            LOG_TRACE("MQTT APP THREAD: OTHER\r\n");
            // if the module received any other event nothing else needs to be done except call it
            mMQTTContext.eventCB(queueMessage.event);
        }
    }
}

// this thread is for the internal MQTT library and is required for the library to function
void *MQTTContextThread(void *threadParams)
{
    int ret;

    LOG_TRACE("CONTEXT THREAD: RUNNING\r\n");

    MQTTClient_run((MQTTClient_Handle)threadParams);

    LOG_TRACE("CONTEXT THREAD: MQTTClient_run RETURN\r\n");

    pthread_mutex_lock(&mMQTTContext.moduleMutex);

    ret = MQTTClient_delete(mMQTTContext.mqttClient);
    if(ret < 0){
        LOG_ERROR("client delete error %d", ret);
    }

    LOG_TRACE("CONTEXT THREAD: MQTT CLIENT DELETED")

    mMQTTContext.moduleState = MQTT_STATE_INITIALIZED;

    pthread_mutex_unlock(&mMQTTContext.moduleMutex);

    LOG_TRACE("CONTEXT THREAD EXIT\r\n");

    pthread_exit(0);

    return(NULL);
}

int MQTT_IF_Init(MQTT_IF_InitParams_t initParams)
{
    int ret;
    pthread_attr_t threadAttr;
    struct sched_param schedulingparams;
    pthread_t mqttAppThread = (pthread_t) NULL;

    if(mMQTTContext.moduleState != MQTT_STATE_IDLE){
        LOG_ERROR("library only supports 1 active init call\r\n");
        return -1;
    }

    // the mutex is to protect data in the mMQTTContext structure from being manipulated or accessed at the wrong time
    pthread_mutex_t my_mutex;
    mMQTTContext.moduleMutex = my_mutex;

    ret = pthread_mutex_init(&mMQTTContext.moduleMutex, (const pthread_mutexattr_t *)NULL);
    if (ret != 0){
        LOG_ERROR("mutex init failed\r\n");
        return ret;
    }

    LOG_TRACE(" - Mutex initialized\r\n");
    pthread_mutex_lock(&mMQTTContext.moduleMutex);

    pthread_attr_init(&threadAttr);
    schedulingparams.sched_priority = initParams.threadPriority;
    ret = pthread_attr_setschedparam(&threadAttr, &schedulingparams);
    ret |= pthread_attr_setstacksize(&threadAttr, initParams.stackSize);
    ret |= pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);
    ret |= pthread_create(&mqttAppThread, &threadAttr, MQTTAppThread, NULL);
    if(ret == 0){
        mMQTTContext.moduleState = MQTT_STATE_INITIALIZED;
    }
    LOG_TRACE(" - MQTTAppThread started\r\n");
    pthread_mutex_unlock(&mMQTTContext.moduleMutex);

    return ret;
}

int MQTT_IF_Deinit(MQTTClient_Handle mqttClientHandle)
{
    static msgData_t queueMessage;

    pthread_mutex_lock(&mMQTTContext.moduleMutex);
    if(mMQTTContext.moduleState != MQTT_STATE_INITIALIZED){
        if(mMQTTContext.moduleState == MQTT_STATE_CONNECTED){
            LOG_ERROR("call disconnect before calling deinit\r\n");
            pthread_mutex_unlock(&mMQTTContext.moduleMutex);
            return -1;
        }
        else if(mMQTTContext.moduleState == MQTT_STATE_IDLE){
            LOG_ERROR("init has not been called\r\n");
            pthread_mutex_unlock(&mMQTTContext.moduleMutex);
            return -1;
        }
    }

    queueMessage.event = MQTT_EVENT_DESTROY;

    // since user called MQTT_IF_Deinit send the signal to the app thread so it may terminate itself
    xQueueSend(mqttQueueHandle, &queueMessage, 0);

    struct Node* tmp = mMQTTContext.head;

    // in case the user did not unsubscribe to topics this loop will free any memory that was allocated
    while(tmp != NULL){
        mMQTTContext.head = tmp->next;
        tmp = mMQTTContext.head;
    }

    // setting the MQTT module state back so that user can call init if they wish to use it again
    mMQTTContext.moduleState = MQTT_STATE_IDLE;
    pthread_mutex_unlock(&mMQTTContext.moduleMutex);

    return 0;
}

MQTTClient_Handle MQTT_IF_Connect(MQTT_IF_ClientParams_t mqttClientParams, MQTTClient_ConnParams mqttConnParams, MQTT_IF_EventCallback_f mqttCB)
{
    int ret;
    pthread_attr_t threadAttr;
    struct sched_param priParam;
    pthread_t mqttContextThread = (pthread_t) NULL;
    MQTTClient_Params clientParams;

    pthread_mutex_lock(&mMQTTContext.moduleMutex);
    // if the user has not called init this will return error since they're trying to connect without intializing the module
    if(mMQTTContext.moduleState != MQTT_STATE_INITIALIZED){

        if(mMQTTContext.moduleState == MQTT_STATE_CONNECTED){
            LOG_ERROR("mqtt library is already connected to a broker\r\n");
        }
        else{
            LOG_ERROR("must call init before calling connect\r\n");
        }
        pthread_mutex_unlock(&mMQTTContext.moduleMutex);
        return (MQTTClient_Handle)-1;
    }

    // copying the event callback the user registered for the module context
    mMQTTContext.eventCB = mqttCB;

    // preparing the data for MQTTClient_create
    clientParams.blockingSend = mqttClientParams.blockingSend;
    clientParams.clientId = mqttClientParams.clientID;
    clientParams.connParams = &mqttConnParams;
    clientParams.mqttMode31 = mqttClientParams.mqttMode31;

    mMQTTContext.mqttClient = MQTTClient_create(MQTTClientCallback, &clientParams);
    if(mMQTTContext.mqttClient == NULL){
        pthread_mutex_unlock(&mMQTTContext.moduleMutex);
        return (MQTTClient_Handle)-1;
    }

    pthread_attr_init(&threadAttr);
    priParam.sched_priority = 2;
    ret = pthread_attr_setschedparam(&threadAttr, &priParam);
    ret |= pthread_attr_setstacksize(&threadAttr, MQTTAPPTHREADSIZE);
    ret |= pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);
    ret |= pthread_create(&mqttContextThread, &threadAttr, MQTTContextThread, NULL);
    if(ret != 0){
        pthread_mutex_unlock(&mMQTTContext.moduleMutex);
        return (MQTTClient_Handle)-1;
    }

    // if the user included additional parameters for the client MQTTClient_set will be called
    if(mqttClientParams.willParams != NULL){
        MQTTClient_set(mMQTTContext.mqttClient, MQTTClient_WILL_PARAM, mqttClientParams.willParams, sizeof(MQTTClient_Will));
    }

    if(mqttClientParams.username != NULL){
        MQTTClient_set(mMQTTContext.mqttClient, MQTTClient_USER_NAME, mqttClientParams.username, strlen(mqttClientParams.username));
    }

    if(mqttClientParams.password != NULL){
        MQTTClient_set(mMQTTContext.mqttClient, MQTTClient_PASSWORD, mqttClientParams.password, strlen(mqttClientParams.password));
    }

    if(mqttClientParams.cleanConnect == false){
        MQTTClient_set(mMQTTContext.mqttClient, MQTTClient_CLEAN_CONNECT, &mqttClientParams.cleanConnect, sizeof(mqttClientParams.cleanConnect));
    }

    if(mqttClientParams.keepaliveTime > 0){
        MQTTClient_set(mMQTTContext.mqttClient, MQTTClient_KEEPALIVE_TIME, &mqttClientParams.keepaliveTime, sizeof(mqttClientParams.keepaliveTime));
    }

    ret = MQTTClient_connect(mMQTTContext.mqttClient);
    if(ret < 0){
        LOG_ERROR("connect failed: %d\r\n", ret);
    }
    else{
        mMQTTContext.moduleState = MQTT_STATE_CONNECTED;
    }
    pthread_mutex_unlock(&mMQTTContext.moduleMutex);

    if(ret < 0){
        return (MQTTClient_Handle)ret;
    }
    else{
        return mMQTTContext.mqttClient;
    }
}

int MQTT_IF_Disconnect(MQTTClient_Handle mqttClientHandle)
{
    int ret;

    pthread_mutex_lock(&mMQTTContext.moduleMutex);
    if(mMQTTContext.moduleState != MQTT_STATE_CONNECTED){
        LOG_ERROR("not connected to broker\r\n");
        pthread_mutex_unlock(&mMQTTContext.moduleMutex);
        return -1;
    }

    mMQTTContext.clientDisconnectFlag = 1;

    ret = MQTTClient_disconnect(mqttClientHandle);
    if(ret < 0){
        LOG_ERROR("disconnect error %d", ret);
    }
    else{
        mMQTTContext.moduleState = MQTT_STATE_INITIALIZED;
    }
    pthread_mutex_unlock(&mMQTTContext.moduleMutex);

    return ret;
}

int MQTT_IF_Subscribe(MQTTClient_Handle mqttClientHandle, char* topic, unsigned int qos, MQTT_IF_TopicCallback_f topicCB)
{
    int ret = 0;

    struct Node* topicEntry;

    pthread_mutex_lock(&mMQTTContext.moduleMutex);
    if(mMQTTContext.moduleState == MQTT_STATE_IDLE){
        LOG_ERROR("user must call MQTT_IF_Init before using the MQTT module\r\n");
        pthread_mutex_unlock(&mMQTTContext.moduleMutex);
        return -1;
    }

    // preparing the topic node to add it to the linked list that tracks all the subscribed topics
    topicEntry = Add_Node();
    topicEntry->topicParams.topic = topic;
    topicEntry->topicParams.qos = qos;
    topicEntry->topicCB = topicCB;

    if(mMQTTContext.moduleState == MQTT_STATE_CONNECTED){
        ret = MQTTClient_subscribe(mMQTTContext.mqttClient, &topicEntry->topicParams, 1);
        // if the client fails to subscribe to the node remove the topic from the list
        if(ret < 0){
            LOG_ERROR("subscribe failed %d. removing topic from list", ret);
        }
    }

    pthread_mutex_unlock(&mMQTTContext.moduleMutex);
    return ret;
}

int MQTT_IF_Unsubscribe(MQTTClient_Handle mqttClientHandle, char* topic)
{
    int ret = 0;

    pthread_mutex_lock(&mMQTTContext.moduleMutex);
    if(mMQTTContext.moduleState != MQTT_STATE_CONNECTED){
        LOG_ERROR("not connected to broker\r\n");
        pthread_mutex_unlock(&mMQTTContext.moduleMutex);
        return -1;
    }

    MQTTClient_UnsubscribeParams unsubParams;
    unsubParams.topic = topic;

    ret = MQTTClient_unsubscribe(mMQTTContext.mqttClient, &unsubParams, 1);
    if(ret < 0){
        LOG_ERROR("unsub failed\r\n");
        pthread_mutex_unlock(&mMQTTContext.moduleMutex);
        return ret;
    }
    else{
        // since unsubscribe succeeded the topic linked list must be updated to remove the topic node
        ret = Remove_Node(topic);

        if (ret == 1) {
            pthread_mutex_unlock(&mMQTTContext.moduleMutex);
            return ret;
        } else {
            dbgEventFail(DBG_ERR_MQTT_ORPHANED_NODE);
        }
    }

    pthread_mutex_unlock(&mMQTTContext.moduleMutex);
    LOG_ERROR("topic does not exist\r\n");
    return -1;
}

int MQTT_IF_Publish(MQTTClient_Handle mqttClientParams, char* topic, char* payload, unsigned short payloadLen, int flags)
{
    pthread_mutex_lock(&mMQTTContext.moduleMutex);
    if(mMQTTContext.moduleState != MQTT_STATE_CONNECTED){
        LOG_ERROR("not connected to broker\r\n");
        pthread_mutex_unlock(&mMQTTContext.moduleMutex);
        return -1;
    }
    pthread_mutex_unlock(&mMQTTContext.moduleMutex);

    return MQTTClient_publish(mqttClientParams,
                              (char*)topic,
                              strlen((char*)topic),
                              (char*)payload,
                              payloadLen,
                              flags);
}

/*
 * REQUIRES MUTEX LOCKED BEFORE USING
 */
struct Node* Add_Node() {

    static struct Node nodes[MAX_NODES];

    if (nodesAllocated >= MAX_NODES) {
        pthread_mutex_unlock(&mMQTTContext.moduleMutex);
        dbgEventFail(DBG_ERR_MQTT_MAX_TOPIC_NODES);

    } else {
        nodesAllocated ++;
        nodes[nodesAllocated - 1].next = mMQTTContext.head;
        mMQTTContext.head = &nodes[nodesAllocated - 1];
        return &nodes[nodesAllocated - 1];
    }
}

/*
 * REQUIRES MUTEX LOCKED BEFORE USING
 */
int Remove_Node(char* topic) {

    if (nodesAllocated <= 0) {
        pthread_mutex_unlock(&mMQTTContext.moduleMutex);
        dbgEventFail(DBG_ERR_MQTT_ORPHANED_NODE);

    } else {

        struct Node* curr = mMQTTContext.head;
        struct Node* prev;

        while (curr != NULL) {

            if (strncmp(topic, curr->topicParams.topic, max(strlen(topic), strlen(curr->topicParams.topic)))) {
                //if curr node has same topic as node to delete, then delete it
                if (curr == mMQTTContext.head) {
                    mMQTTContext.head = curr->next;
                } else {
                    prev->next = curr->next;
                }
                nodesAllocated --;
                return 1;
            }
            prev = curr;
            curr = curr->next;
        }
    }

    return -1;
}

int min(int a, int b) {
    return (a < b) ? a : b;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}
