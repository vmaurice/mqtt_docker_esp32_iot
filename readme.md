# MQTT INFLUXDB GRAFANA DOCKER ESP32

Project IOT, setting up of a MQTT server, an InfluxDB database, a display with GRAFANA, all starting in a docker-compose.

An ESP32 is used with two I2C sensors, one temperature device (MCP9808) and one luminosity device (TSL2561). He publishes in sensors/kerno/bedroom/temperature and sensors/kerno/bedroom/luminosity with JSON format. The data is saved in InfluxDB v1.8 local database and InfluxDB v2 cloud database. The twice databases can be seen with Grafana.

<p align="center">
    <img  src="esp32.jpg" alt="Esp32 with sensors">
</p>

## Command to run service

```
/path/docker-compose up
```

## Command to exec influxdb cli

```
docker exec -it influxdb influx
```

don't forget to allow the path in docker.
