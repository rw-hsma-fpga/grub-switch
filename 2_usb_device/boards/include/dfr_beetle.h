#ifndef DFR_BEETLE_H
#define DFR_BEETLE_H


#include "config.h"
#ifdef TARGET_BOARD_DFR_BEETLE

#define VALID_TARGET_BOARD_DEFINED


// External quartz frequency (kHz) -> CPU core frequency
#define FOSC 16000

#define  LED_PORT           C
#define  LED_PIN            7

#define  MODE_PORT          B
#define  MODE_PIN           6

#define  WRPROT_PORT        B
#define  WRPROT_PIN         7

#define  AUX0_PORT1         E
#define  AUX0_PIN1          6

#define  AUX0_PORT2         E
#define  AUX0_PIN2          6

#define  CHOICE_PORT1       B
#define  CHOICE_PIN1        5

#define  CHOICE_PORT2       F
#define  CHOICE_PIN2        7

#define  CHOICE_PORT3       F
#define  CHOICE_PIN3        6

#define  CHOICE_PORT4       F
#define  CHOICE_PIN4        5

// bottom pads

#define  CHOICE_PORT5       D
#define  CHOICE_PIN5        0

#define  CHOICE_PORT6       D
#define  CHOICE_PIN6        1

#define  CHOICE_PORT7       D
#define  CHOICE_PIN7        2

#define  CHOICE_PORT8       D
#define  CHOICE_PIN8        3

// not easily connectable (test pads)

#define  CHOICE_PORT9       B
#define  CHOICE_PIN9        3

#define  CHOICE_PORT10      B
#define  CHOICE_PIN10       1

#define  CHOICE_PORT11      B
#define  CHOICE_PIN11       2


#endif   // TARGET_BOARD==DFR_BEETLE


#endif   // DFR_BEETLE_H
