import paho.mqtt.client as mqtt
import paho.mqtt.subscribe as subscribe
import time
import random
import os

broker_address="server"
client = mqtt.Client("server")

devices = ["11737d43-1026-4c58-9b70-d498f11dcce8", "d3037359-76b9-4ee0-bdf6-a754fcef9417", "a33e0352-61de-4638-84b5-c80aa796c5da", "29b5b14a-99a0-4bfa-a233-cfa4f799bf54", "33d47ee9-b291-4a03-9c3b-9926836d0d67", "5b2d9634-3ef9-4a84-a4df-97a85b8e4093", "0a56eecf-2955-4756-8f5d-80adc2e55ac0", "1ce6adcf-dc59-4aee-8aef-3e1a202f2982", "411cd30a-0f87-47f7-80b3-28fec71af36a"]

while(1):
    try:
        client.tls_set(ca_certs="/certs/ca.crt", certfile="/certs/server.crt", keyfile="/certs/server.key")
        client.connect(broker_address, 1883)
        print("connected to broker / starting simulation")

        while (1):
            print("Simulation is runnning ...")
            # Simulate many device connection
            i = random.randint(0,8)
            time.sleep(random.randint(1,3))
            client.publish("logs/"+devices[i]+"/status", 'status:armed')

            i = random.randint(0,8)
            time.sleep(random.randint(1,3))
            client.publish("logs/"+devices[i]+"/info", 'connected to broker')

            i = random.randint(0,8)
            time.sleep(random.randint(1,3))
            client.publish("logs/"+devices[i]+"/battery", 'batt level '+str(random.randint(19,29)))

            i = random.randint(0,8)
            time.sleep(random.randint(1,3))
            client.publish("logs/"+devices[i]+"/disarm", 'ready for orders')

            i = random.randint(0,8)
            time.sleep(random.randint(1,3))
            client.publish("logs/"+devices[i]+"/movement", 'Movement level '+str(random.randint(12,100)))


    except Exception as e: print(e)
