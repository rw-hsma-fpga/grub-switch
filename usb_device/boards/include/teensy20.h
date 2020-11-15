#ifndef TEENSY20_H
#define TEENSY20_H


#include "config.h"
#ifdef TARGET_BOARD_TEENSY20

#define VALID_TARGET_BOARD_DEFINED


// External quartz frequency (kHz) -> CPU core frequency
#define FOSC 16000

#define  LED_PORT           D
#define  LED_PIN            6

#define  MODE_PORT          B
#define  MODE_PIN           6

#define  WRPROT_PORT        F
#define  WRPROT_PIN         4

#define  AUX0_PORT1         F
#define  AUX0_PIN1          1

#define  AUX0_PORT2         B
#define  AUX0_PIN2          5

#define  CHOICE_PORT1       B
#define  CHOICE_PIN1        0

#define  CHOICE_PORT2       B
#define  CHOICE_PIN2        1

#define  CHOICE_PORT3       B
#define  CHOICE_PIN3        2

#define  CHOICE_PORT4       B
#define  CHOICE_PIN4        3

#define  CHOICE_PORT5       B
#define  CHOICE_PIN5        7

#define  CHOICE_PORT6       D
#define  CHOICE_PIN6        0

#define  CHOICE_PORT7       D
#define  CHOICE_PIN7        1

#define  CHOICE_PORT8       D
#define  CHOICE_PIN8        2

#define  CHOICE_PORT9       D
#define  CHOICE_PIN9        3

#define  CHOICE_PORT10      C
#define  CHOICE_PIN10       6

#define  CHOICE_PORT11      C
#define  CHOICE_PIN11       7


#endif   // TARGET_BOARD==TEENSY20


#endif   // TEENSY20_H
