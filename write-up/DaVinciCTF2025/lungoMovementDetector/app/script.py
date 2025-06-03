import paho.mqtt.client as mqtt
import paho.mqtt.subscribe as subscribe
import time
import random
import os

password = "d3fus3m3"
broker_address="server"
client = mqtt.Client("superServer")
client2 = mqtt.Client("superServer2")

def on_message(client, userdata, message):
    print("client ", client)
    print("message received " ,str(message.payload.decode("utf-8")))
    print("message topic=",message.topic)

    payload = message.payload.decode('utf-8')
    print("Message: "+payload)
    print("Topic: "+message.topic)

    if "devices/0a56eecf-2955-4756-8f5d-80adc2e55ac0/disarm" == message.topic:
        print("Disarm procedure")
        client.publish("logs/0a56eecf-2955-4756-8f5d-80adc2e55ac0/info", "Disarm procedure starting")
        client.publish("logs/0a56eecf-2955-4756-8f5d-80adc2e55ac0/info", "Checking password")
        if payload == password:
            client.publish("logs/0a56eecf-2955-4756-8f5d-80adc2e55ac0/info", "Password ok")
            try:
                client.connect(broker_address, 1883)
                client.publish("devices/0a56eecf-2955-4756-8f5d-80adc2e55ac0/status", "disarmed")
                client.publish("logs/0a56eecf-2955-4756-8f5d-80adc2e55ac0/info", "Disarm procedure ok")
                client.publish("logs/0a56eecf-2955-4756-8f5d-80adc2e55ac0/status", "disarmed")
                client.publish("devices/0a56eecf-2955-4756-8f5d-80adc2e55ac0/flag", "DVCTF{MqTTPolici3sAreImPoRtaNt}")
            except Exception as e: print(e)
        else:
            client.publish("logs/0a56eecf-2955-4756-8f5d-80adc2e55ac0/info", "Password wrong")
    client.publish("logs/0a56eecf-2955-4756-8f5d-80adc2e55ac0/info", "Disarm procedure error")

while(1):

    try:
        client.tls_set(ca_certs="/certs/ca.crt", certfile="/certs/server.crt", keyfile="/certs/server.key")
        client.connect(broker_address, 1883)

        while (1):
            client.on_message=on_message
            client.loop_start()
            client.subscribe("devices/0a56eecf-2955-4756-8f5d-80adc2e55ac0/disarm")
            client.loop_stop()

    except Exception as e: print(e)
