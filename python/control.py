#!/usr/bin/python3

#
# Fichier pour communiquer avec le broker mqtt afin de recevoir des informations des capteurs sur l'ESP32
# ./control.py -d : récupère l'état de la porte (ouverte ou fermée)
# ./control.py -t : récupère la température
# ./control.py -l : récupère la luminosité
# ./control.py -h : aide
#
import time
import paho.mqtt.client as paho
import sys
import json

broker = "localhost"

received = False

#define callback
def on_message(client, userdata, message):
    global received
    
    topic = str(message.topic)
    msg = json.loads(str(message.payload.decode("utf-8")))

    received = True

    if topic == "sensors/kerno/bedroom/answer/door":
        distance = msg["value"]
        if distance > 20:
            print ("Porte fermée")
        else:
            print ("Porte ouverte")
    elif topic == "sensors/kerno/bedroom/answer/temp":
        print ("La température est de " + str(msg["value"]) + " " + msg["unit"])
    elif topic == "sensors/kerno/bedroom/answer/lum":
        print ("La luminosité est de " + str(msg["value"]) + " " + msg["unit"])
    else:
        print ("Error topic")

 
def main(list):

    global received

    if list[0] == "-d":
        txt = "sensors/kerno/bedroom/call/door"
    elif list[0] == "-t":
        txt = "sensors/kerno/bedroom/call/temp"
    elif list[0] == "-l":
        txt = "sensors/kerno/bedroom/call/lum"
    else:
        print ("./controle.py -d : récupère information porte")
        print ("./controle.py -t : récupère la température")
        print ("./controle.py -l : récupère la luminosité")
        print ("./controle.py -h : Aide")
        return

    client= paho.Client("client") 

    client.on_message=on_message

    print("connecting to broker ",broker)
    client.connect(broker)#connect

    subscribe_txt = "sensors/kerno/bedroom/answer/#"

    print("subscribing : " + subscribe_txt)
    client.subscribe(subscribe_txt)#subscribe

    client.loop_start() #start loop to process received messages

    print("publishing : " + txt)
    client.publish(txt)

    t = time.time()

    while not received:
        if (time.time() - t > 30):
            break
        pass

    client.loop_stop() #stop loop
    client.disconnect() #disconnect


if __name__ == "__main__":
    main(sys.argv[1:])
