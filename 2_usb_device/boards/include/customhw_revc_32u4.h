#ifndef CUSTOMHW_REVC_32U4_H
#define CUSTOMHW_REVC_32U4_H


#include "config.h"
#ifdef TARGET_BOARD_CUSTOMHW_REVC_32U4

#define VALID_TARGET_BOARD_DEFINED


// External quartz frequency (kHz) -> CPU core frequency
#define FOSC 16000

#define  LED_PORT           F
#define  LED_PIN            5

#define  MODE_PORT          C
#define  MODE_PIN           7

#define  WRPROT_PORT        F
#define  WRPROT_PIN         6

#define  AUX0_PORT1         D // unconnected pin, not actually
#define  AUX0_PIN1          1 // required on custom board

#define  AUX0_PORT2         D // unconnected pin, not actually
#define  AUX0_PIN2          1 // required on custom board

#define  CHOICE_PORT1       D
#define  CHOICE_PIN1        3

#define  CHOICE_PORT2       D
#define  CHOICE_PIN2        2

#define  CHOICE_PORT3       D
#define  CHOICE_PIN3        5

#define  CHOICE_PORT4       D
#define  CHOICE_PIN4        4

#define  CHOICE_PORT5       D
#define  CHOICE_PIN5        6

#define  CHOICE_PORT6       D
#define  CHOICE_PIN6        7

#define  CHOICE_PORT7       B
#define  CHOICE_PIN7        4

#define  CHOICE_PORT8       B
#define  CHOICE_PIN8        6

#define  CHOICE_PORT9       C
#define  CHOICE_PIN9        6

#define  CHOICE_PORT10      F
#define  CHOICE_PIN10       7

#define  CHOICE_PORT11      B
#define  CHOICE_PIN11       5


#endif   // TARGET_BOARD==CUSTOMHW_REVC_32U4


#endif   // CUSTOMHW_REVC_32U4_H
