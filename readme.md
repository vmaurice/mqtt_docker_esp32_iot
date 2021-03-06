# MQTT INFLUXDB GRAFANA DOCKER ESP32

Project IOT : setting up a MQTT server, with InfluxDB database. Displayed with Grafana, everything is launched by a docker-compose.

An ESP32 is used with two I2C sensors : a temperature sensor (MCP9808) and a light sensor (TSL2561). And in more, an ultrasonic sensor (HC-SR04).

He publishes in " sensors/kerno/bedroom/temperature " and " sensors/kerno/bedroom/luminosity " in JSON format. The data is saved in InfluxDB v1.8 (local database) and InfluxDB v2 (cloud database). Both databases can be visualized in Grafana.

Run in Mac OS and Raspberry pi OS.

You can get the value of the esp32 sensors with control.py file into python folder. For example ./control.py -d : get if the door is opened or closed. More information with ./control.py -h.

<p align="center">
    <img  src="ressources/esp32-v2-2.jpg" alt="Esp32 version 2 with sensors">
</p>

<p align="center">
    <img  src="ressources/grafana.png" alt="Grafana">
</p>

## Command to run service

```
/path/docker-compose up
```

## Command to execute influxdb cli

```
docker exec -it influxdb influx
```

MAC OS :don't forget to allow the path in docker.

### Version 2 :

<p align="center">
    <img  src="ressources/esp32-v2.jpg" alt="Esp32 version 2 with sensors">
</p>

### Version 1 :

<p align="center">
    <img  src="ressources/esp32.jpg" alt="Esp32 version 1 with sensors">
</p>


