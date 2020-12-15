set(COMPONENT_SRCS "main.c ./modules/wifi/wifi.c ./modules/http/http_client.c ./modules/mqtt/mqtt.c ./modules/sensor/sensor_module.c ./modules/led/led.c ./modules/memory/config.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()

