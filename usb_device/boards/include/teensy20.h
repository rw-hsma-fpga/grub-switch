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
#define  MODE_PIN           0

#define  WRPROT_PORT        F
#define  WRPROT_PIN         0

#define  AUX0_PORT          F
#define  AUX0_PIN           1

#define  CHOICE_PORT1       B
#define  CHOICE_PIN1        1

#define  CHOICE_PORT2       B
#define  CHOICE_PIN2        2

#define  CHOICE_PORT3       B
#define  CHOICE_PIN3        3

#define  CHOICE_PORT4       B
#define  CHOICE_PIN4        7

#define  CHOICE_PORT5       D
#define  CHOICE_PIN5        0

#define  CHOICE_PORT6       D
#define  CHOICE_PIN6        1

#define  CHOICE_PORT7       D
#define  CHOICE_PIN7        2

#define  CHOICE_PORT8       D
#define  CHOICE_PIN8        3

#define  CHOICE_PORT9       C
#define  CHOICE_PIN9        6

#define  CHOICE_PORT10      C
#define  CHOICE_PIN10       7

#define  CHOICE_PORT11      D
#define  CHOICE_PIN11       5


#endif   // TARGET_BOARD==TEENSY20


#endif   // TEENSY20_H
