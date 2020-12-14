central_server: 
	gcc central_server.c ./modules/menu/menu.c \
		./modules/mqtt/mqtt.c \
		./modules/csv/csv.c \
		./modules/alarm/alarm.c \
 	 	-I . \
 	 	-I ./modules/mqtt \
		-I ./modules/menu \
		-I ./modules/csv \
		-I ./modules/alarm \
		-lpaho-mqtt3c -lcjson -lpthread -lncurses -o central_server

clean:
	rm -v central_server

