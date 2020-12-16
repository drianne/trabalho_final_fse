#include <bcm2835.h>

#define ON 1
#define OFF 0

#define PIN_L_KITCHEN RPI_V2_GPIO_P1_11
#define PIN_L_ROOM RPI_V2_GPIO_P1_12

// Sensores de presença
#define PIN_SP_ROOM RPI_V2_GPIO_P1_22
#define PIN_SP_KITCHEN RPI_V2_GPIO_P1_37

// Sensores de abertura
#define PIN_SA_DOOR_KITCHEN RPI_V2_GPIO_P1_29
#define PIN_SA_WINDOW_KITCHEN RPI_V2_GPIO_P1_31
#define PIN_SA_DOOR_ROOM RPI_V2_GPIO_P1_32
#define PIN_SA_WINDOW_ROOM RPI_V2_GPIO_P1_36

#define QNT_SENSORS 6

void turn_on_component(int index);
void turn_off_component(int index);
void *polling(void *params);
void initGpio();