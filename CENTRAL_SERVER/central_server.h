#ifndef CENTRAL_H_
#define CENTRAL_H_


// Lamps
#define L_KITCHEN 0
#define L_ROOM 1

#define QNTD_OUTPUTS 2
// Presence sensors
#define SP_ROOM 6
#define SP_KITCHEN 7

// Aperture sensors
#define SA_DOOR_KITCHEN 8
#define SA_WINDOW_KITCHEN 9
#define SA_DOOR_ROOM 10
#define SA_WINDOW_ROOM 11

// Temperature e humidity
#define TEMPERATURE 14
#define HUMIDITY 15
#define TEMPERATURE_CONTROL 16

struct devices
{
    int lampKitchen;
    int lampRoom;
};

struct sensors
{
    // presence sensor
    int spRoom;
    int spKitchen;

    // aperture sensor
    int saDoorKitchen;
    int saWindowKitchen;
    int saDoorRoom;
    int saWindowRoom;
};

struct climate
{
    double temperature;
    double humidity;
};

struct data
{
    struct climate *climate;
    struct sensors *sensors;
    struct devices *devices;
};

struct components
{
    char mac[50];
    char comodo[40];
    double temp;
    double hum;
    char component_in[30];
    int component_in_value;
    char component_out[30];
    int component_out_value;
};

struct list_components
{
    int total;
    int atual;
    struct components components[10];
};

#endif /* CENTRAL_H_ */
