#ifndef PROMICRO_5V_H
#define PROMICRO_5V_H


#include "config.h"
#ifdef TARGET_BOARD_PROMICRO_5V

#define VALID_TARGET_BOARD_DEFINED


// External quartz frequency (kHz) -> CPU core frequency
#define FOSC 16000

#define  LED_PORT           D
#define  LED_PIN            5

#define  MODE_PORT          D
#define  MODE_PIN           2

#define  WRPROT_PORT        F
#define  WRPROT_PIN         4

#define  AUX0_PORT1         F
#define  AUX0_PIN1          5

#define  AUX0_PORT2         F
#define  AUX0_PIN2          5

#define  CHOICE_PORT1       D
#define  CHOICE_PIN1        1

#define  CHOICE_PORT2       D
#define  CHOICE_PIN2        0

#define  CHOICE_PORT3       D
#define  CHOICE_PIN3        4

#define  CHOICE_PORT4       C
#define  CHOICE_PIN4        6

#define  CHOICE_PORT5       D
#define  CHOICE_PIN5        7

#define  CHOICE_PORT6       E
#define  CHOICE_PIN6        6

#define  CHOICE_PORT7       B
#define  CHOICE_PIN7        4

#define  CHOICE_PORT8       B
#define  CHOICE_PIN8        5

#define  CHOICE_PORT9       B
#define  CHOICE_PIN9        6

#define  CHOICE_PORT10      F
#define  CHOICE_PIN10       7

#define  CHOICE_PORT11      F
#define  CHOICE_PIN11       6


#endif   // TARGET_BOARD==PROMICRO_5V


#endif   // PROMICRO_5V_H
