#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <MQTTAsync.h>
#include <MQTTClient.h>

#include "led.h"

#include <pthread.h>

#define ADDRESS "tcp://101.43.188.10:1883"
#define CLIENTID "ExampleClientPub"
#define TOPIC "mqtt"
#define PAYLOAD "Hello World!"
#define QOS 1
#define TIMEOUT 10000L


extern int g_LedFlag;
static MQTTClient client = NULL;
volatile MQTTClient_deliveryToken deliveredtoken;

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    int i;
    char *payloadptr;

    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: ");

    payloadptr = message->payload;
    char recvbyte[message->payloadlen + 1];
    for (i = 0; i < message->payloadlen; i++)
    {
        recvbyte[i] = *payloadptr;
        putchar(*payloadptr++);
    }
    recvbyte[message->payloadlen] = "\0";
    SetPinRevers(0);
    SetPinRevers(1);
    putchar('\n');

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);

    return 1;
}

void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int mqtt_client_publish(char *top, int qos, char *msg, int len)
{
    MQTTClient_deliveryToken token;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;

    if (client == NULL)
        return -1;

    pubmsg.payload = msg;
    pubmsg.payloadlen = len;
    pubmsg.qos = qos;
    pubmsg.retained = 0;
    deliveredtoken = 0;
    MQTTClient_publishMessage(client, top, &pubmsg, &token);
    printf("Waiting for publication of %s\n"
           "on topic %s for client\n",
           msg, top);
    while (deliveredtoken != token)
        ;
    return 0;
}

int mqtt_client_subscribe(char *top, int qos)
{
    if (client == NULL)
        return -1;

    MQTTClient_subscribe(client, top, qos);
    return 0;
}

int mqtt_client_open()
{
    int rc;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        return -1;
    }
    printf("Success to connect!\n");
    return 0;
}

void Int_High(void)
{
    printf("Sound Ctrl High\n");
    SetPinHigh(0);
    SetPinHigh(1);
    printf("High DONE\n");
}
void Int_Low(void)
{
    printf("Sound Ctrl Low\n")
    SetPinLow(0);
    SetPinLow(1);
    printf("Low DONE\n");
}

int main(int argc, char *argv[])
{
    wiringPiSetup();
    char *str = "hello world!";
    InitPin(0,OUTPUT);
    InitPin(1,OUTPUT);
    InitPin(2,INPUT);
    InitPin(5,INPUT);

    pullUpDnControl (Ctr_SoundHigh, PUD_UP);
    pullUpDnControl (Ctr_SoundLow, PUD_UP);
    wiringPiISR(Ctr_SoundHigh,INT_EDGE_FALLING,&Int_High);
    wiringPiISR(Ctr_SoundLow ,INT_EDGE_FALLING,&Int_Low);

    mqtt_client_open();
    mqtt_client_subscribe(TOPIC, QOS);
    mqtt_client_publish(TOPIC, QOS, str, strlen(str));
    sleep(1);
    while (1){sleep(10);}
        MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);

    return 0;
}

