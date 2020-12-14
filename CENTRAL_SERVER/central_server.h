
#define QNT_DEVICES 18
#define QNT_OUTPUTS 6
#define TOTAL_DEVICES 8

// Lamps
#define L_KITCHEN 0
#define L_ROOM 1

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
    char state[30];
};

struct list_components
{
    int total;
    int atual;
    struct components components[10];
};