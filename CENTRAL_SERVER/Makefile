central_server: 
	gcc central_server.c ./modules/menu/menu.c ./dependencies/bme/bme280.c  \
		./modules/mqtt/mqtt.c \
		./modules/csv/csv.c \
		./modules/alarm/alarm.c \
		./modules/i2c/temperature_module_i2c.c \
 	 	-I . \
		-I ./dependencies/bme \
 	 	-I ./modules/mqtt \
		-I ./modules/menu \
		-I ./modules/csv \
		-I ./modules/alarm \
		-I ./modules/i2c \
		-lpaho-mqtt3c -lcjson -lbcm2835 -lpthread -lncurses -lwiringPi -o central_server
			 
clean:
	rm -v central_server

